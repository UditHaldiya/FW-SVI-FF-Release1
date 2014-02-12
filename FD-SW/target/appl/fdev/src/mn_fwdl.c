#ifndef __ICCARM__
 #include <windows.h>
 #include <stdio.h>
#endif
#include <string.h>
#if (defined SW_DOWNLOAD) || (defined _lint)
 #include "base.h"
 #include "base_cfg.h"
 #include "keywords.h"
 #include "swdl_api.h"
 #include "rtos.h"
#endif
//#include <softing_base.h>
#include "crc.h"
#include "mn_comp.h"
#include "mnhart2ff.h"
#include "mn_fwdl.h"
#include "mn_flash.h"
#include <eep_if.h>
#include <hw_if.h>

#if !PC
 #define CRC_Compute  Crc16
#endif

static FFInfo_t allvers[4];
//extern u8  smdls_dwnld_hdr[];           // kludge

// flags passed to WriteToFlash()
#define NE  4u          // do not erase (driver knows it is blank)
#define ER  0u          // erase when crossing page boundary
#define NU	0u			// flash write no CRC or  address update
#define UP	2u			// flash write do CRC and address update
#define LCL 0u			// flash write to local  (FF)  flash even if updating APP
#define REM 1u			// flash write to remote (APP) flash when    updating APP

#define WD_SZ	sizeof(u32)
#define HW_SZ	sizeof(u16)

#define IMG_BUF_SZ		MAX_PROG_DATA   	// size of an intermediate RAM buffer for firmware image
											// relocation is applied in this buffer
CONST_ASSERT((IMG_BUF_SZ % 4) == 0);		// must be a multiple of 4
CONST_ASSERT((IMG_BUF_SZ + DATA_OFFSET) <= 100); // MAX_HART_TELEGRAMM_LEN can't include "hm_cfg.h"

typedef enum {IDLE, LOAD_IH,   LOAD_PH, LOAD_RELOC, LOAD_FW} DL_STATE;
typedef enum {NOTACTIVE, ACTIVATE0, ACTIVATE1} PARTN_STATE;

#define NO_BYTES_TO_HOLD	8				// number of vector bytes to defer writing
                                            // 2 wds - stack pointer and reset vect address

CONST_ASSERT((NO_BYTES_TO_HOLD % 4) == 0);

#define ALIGN(x, y)			(((x) + ((y) - 1)) & ~((y) - 1))


static	u32					byteCount,          // relative to start of private header
							bufIx;              // indiex into flash hold buffer
static	DL_STATE			dlState = IDLE;

// pH and iH are arrays to make Lint happy
static  private_header_t    pH[1];
static	image_header_t		iH[1];              // the current image header
static	u32					relocationAmount;   // amount to add when fixing up the image
static	u8					*fwCurrentAddress,  // absolute address at which to write next flash image byte(s)
							*fwBaseAddress;     // absolute address of image base (vector table)

// variables pertaining to current image
static u8		            cpuDest,            // 0 == FF, 1 == APP
                            *relocTable;        // an address near the top of the inactive FF bank
static bool_t	            compressed,         // true if relocation table and image are compressed
				            reloc;              // true if image needs to be relocated

// structure for Download Recovery sync
typedef struct sync_t
{
	u32 inbytes;			// offset in bytes into the DL file
	u32 outbytes;			// offset in bytes into the FW image
	u16 fwCRC,				// CRC of relocated firmware image  planted at end
                            //  of fw image in target flash
		dlCRC;				// CRC of downloaded firmware image (must match
                            //    iH.image_CRC)
} sync_t;

static sync_t ce;           // RAM copy (for DL Recovery this will be updated to
                            // flash ~ every 16 Kb

typedef struct pstate_t
{
    PARTN_STATE  st[2];
} pstate_t;

// number of elements of sync_t array that will fit in a flash sector
//#define ENTRIES ((SECT_SIZE - NO_BYTES_TO_HOLD - (8 * sizeof(u16))) / sizeof(sync_t))

// download state info kept in last sector of inactive FF CPU bank
typedef struct keepme_t
{
	pstate_t	state[8];                       // state of each partition
	sync_t	    entry[32];                      // not yet maintained - for DL Recovery
	u8		    firstBytes[NO_BYTES_TO_HOLD];   // first vector bytes for the firmware ...
                                                //  held until the partition CRC is ...
                                                //  validated
} keepme_t;
static keepme_t *phInfo;

CONST_ASSERT(sizeof(keepme_t) <= SECT_SIZE);

static void DownloadPrepare(DL_STATE state)
{
	byteCount       = 0;
	bufIx           = 0;
	reloc           = false;
	compressed		= false;
	dlState         = state;
	ce.outbytes     = 0;
}

static void CloseFlashPartition(u8_least flags);

FUNCTION GLOBAL T_DOMAIN_HDR_API * start_prepare_swdl  ( VOID  )
{
    CloseFlashPartition(LCL);
    CloseFlashPartition(REM);

	return  vh_GetDomainHeader();
}

FUNCTION GLOBAL USIGN8 appl_check_fd_dom_hdr  ( IN const T_DOMAIN_HDR_API * p_dwnld_dom_hdr )
{
	UNUSED_OK(p_dwnld_dom_hdr);
    DownloadPrepare(LOAD_PH);
	ce.inbytes      = 0;
#if PC
	//printf("Size is %d\n", sizeof(keepme_t));
#endif
	return E_OK;
}

/** \brief
*/
static void SetActive(u8_least otherCPU, PARTN_STATE st)
{
#if PC
#else
    if ((st == ACTIVATE0) || (st == ACTIVATE1))
    {
        u32 partn = (st == ACTIVATE1) ? 1 : 0;

    	if ((otherCPU & REM) == REM)
	    {
            (void)fwdk_WriteAppCPU32(SET_PARTN, partn);
	    }
    	else
	    {
		    (void)flash_SetPartition(partn);
    	}
    }
    // delay to allow update of NVRAM before reset
    //OS_Delay(200);

#endif
}

/** \brief Get the base (vector table) address of the partition.  The address
    is returned by a call to the flash driver.

    \param otherCPU - 0 if FF CPU, 1 if APP CPU
*/
static u32 GetPartitonBaseAddress(bool_t otherCPU)
{
#if PC
	return (u32)0x8080000;
#else
	if (otherCPU)
	{
        return fwdk_WriteAppCPU32(PARTN_BASE, 0);   // get partiton addr from app CPU
	}
	return flash_GetPartitionBaseAddress(0u);
#endif
}

/** \brief
*/
static void OpenFlashPartition(u8_least flags)
{
#if PC
#else
	if ((flags & REM) == REM)
	{
        // deferred for (;;) {}
	}
    (void)flash_OpenPartition(CRC_SEED);      // need to open local regardless
#endif
}

/** \brief
*/
static void CloseFlashPartition(u8_least flags)
{
#if PC
#else
    if ((flags & REM) == REM)
	{
        (void)fwdk_WriteAppCPU32(CLOSE_PARTN, 0u);
	}
    (void)flash_ClosePartition(0u);     // need to close local regardless
#endif
}

#define STATS 0
#if STATS
 extern u32 tickz;
 u32 tyme;
#endif

/** \brief
*/
static u32 WriteToFlash(const void *dat, void *adr, u32 noWds, u8_least flags)
{
	if ((flags & UP) == UP)		// want to compute the CRC and update the flash address?
	{
		fwCurrentAddress += (noWds * HW_SZ);
		ce.fwCRC = CRC_Compute((const u8*)dat, noWds * HW_SZ, ce.fwCRC);
	}
#if !PC
    u8_least flgs = (flags & NE) != 0 ? NO_ERASE : 0u;

    if ((flags & UP) != 0u)
    {
        flgs |= UPD_CRC;
    }
	if ((flags & REM) == REM)
	{
        return fwdk_WriteAppCPU(dat, HARDWARE(u32, adr), noWds * HW_SZ, flgs);
	}
	else
	{
        return flash_ProgramBlock(dat, adr,  noWds * HW_SZ, flgs);
	}
#else
	while (noWds--)
	{
		*adr++ = *dat++;
	}
    return ce.fwCRC;
#endif
}

/** \brief
*/
static u8 LoadPrivateHeader(u8 kh)
{
	u8 *ph = HARDWARE(u8*, &pH[0]);

	ph[byteCount++] = kh;
	if (byteCount == sizeof(pH[0]))
	{
		DownloadPrepare(LOAD_IH);
	}
	return E_OK;
}

/** \brief Record the state of the selected partition.  This maintains a state
    for each CPU in the flash at the top of the FF CPU.  Noite that opdating the
    staew erases the top segment of flash in the FF CPU.  This is OK because we
    do not change it during a download.

    \param state - one of the state enumerated in PARTN_STATE.
*/
static void SetPartitionState(PARTN_STATE state)
{
    pstate_t lstate = phInfo->state[0];

    UNUSED_OK(phInfo->entry[0]);       // future use
    lstate.st[cpuDest] = state;
    void *statewr = &lstate, *gstate = &phInfo->state[0];
	(void)WriteToFlash(statewr, gstate, sizeof(pstate_t) / HW_SZ, (NU | LCL | ER));
}

/** \brief Accumulate the image header one byte at a time.

    \param kh - a character to store in the header structure.
*/
static u8 LoadImageHeader(u8 kh)
{
	u8 *ih = HARDWARE(u8*, &iH[0]);
	u32 temp, relocTblsz;

	ih[byteCount++] = kh;
	if (byteCount == sizeof(iH[0]))
	{
		ce.dlCRC   = CRC_SEED;
		compressed =  iH[0].compression_method   != 0;
//        cpuDest    = REM;
		cpuDest    = (iH[0].destination_cpu_flag != 0) ? REM : LCL;
		byteCount  = 0;
		if (iH[0].relocation_size != 0)
		{
			reloc   = true;
			dlState = LOAD_RELOC;
		}
		else
		{
			dlState = LOAD_FW;
		}
		relocTblsz       = ALIGN(iH[0].relocation_size, SECT_SIZE); // sector bytes for reloc table
		relocationAmount = GetPartitonBaseAddress((cpuDest & REM) == REM);
#if PC
		fwBaseAddress    = (u32)malloc(iH[0].image_size);
		temp             = (u32)malloc(relocTblsz + sizeof(keepme_t));
#else
		fwBaseAddress    = HARDWARE(u8*, relocationAmount);
		temp             = GetPartitonBaseAddress(false);
		temp             = (temp & BANK_MASK) + (BANK_SIZE - SECT_SIZE) - relocTblsz;
#endif
		fwCurrentAddress = fwBaseAddress;
		relocTable       = HARDWARE(u8*, temp);
		phInfo           = HARDWARE(keepme_t*, (temp + relocTblsz));
        OpenFlashPartition(cpuDest);
        SetPartitionState(NOTACTIVE);
	}
	return E_OK;
}

/** \brief Load the relocation table into local flash memory - one byte at a time. Once
	we have received iH.relocation_size bytes, the CRC is checked.

	\return E_OK if everything is good; other wise E_SWDL_CHECKSUM_FAIL
*/
static u8 LoadRelocationTable(u8 kh)
{
	static	u16 word = 0;
			u8	rslt = E_OK;

	if ((byteCount & 1) != 0)
	{
		word |= (kh << 8);
        void *relok = relocTable + byteCount - 1;
		(void)WriteToFlash(&word, relok, 1, (NU | LCL | ER));
	}
	else
	{
		word = kh;
	}
	byteCount++;
	if (byteCount >= iH[0].relocation_size)
	{
		if (ce.dlCRC == iH[0].relocation_CRC)
		{
			ce.dlCRC  = CRC_SEED;		// prepare for firmware block
			ce.fwCRC  = CRC_SEED;		// prepare for firmware block
			dlState   = LOAD_FW;
			byteCount = 0;
		}
		else
		{
			rslt = E_SWDL_CHECKSUM_FAIL;
		}
	}
	return rslt;
}

/** \brief Process a byte of downloaded firmware image. Writes to flash are
    performed at IMG_BUF_SZ size intervals and at other, smaller intervals:
        1. First 8 bytes copied to reserved memory in FF CPU flash
        2. Blocks of IMG_BUF_SZ
        3. Whatever is accumulated when we reach 2 bytes (CRC size) from the end
        4. The last two bytes (CRC) after validation
        5, The first 8 byte previously saved

    \param kh - on byte of firmwware image data
	\return E_OK if everything is good; otherwise E_SWDL_CHECKSUM_FAIL
*/
static u8 FirmwareLoad(u8 kh)
{
	static	u32 fwWds[IMG_BUF_SZ / sizeof(u32)];    // IMG_BUF_SZ is multiple of
    static  u32 fwCRC = (u16)0;

            u8  *fwBuffer = (u8*)(void*)fwWds;
            u16 *fwBf16   = (u16*)(void*)fwWds;
			u8 rslt = E_OK;

	fwBuffer[bufIx++] = kh;
	byteCount++;

	if (reloc && ((bufIx & 3) == 0))		    // time to check for relocation fixup?
	{
		u32 wofs = byteCount / WD_SZ;			// word offset in fw image
		if (TST_BITD(relocTable, wofs - 1))
		{
            fwWds[(bufIx / WD_SZ) - 1]  += relocationAmount;
		}
	}
	if (byteCount == NO_BYTES_TO_HOLD)			// got first 8 bytes?
	{
		// put in a temporary place in the FF flash
		fwCRC = WriteToFlash(fwBuffer, phInfo->firstBytes, bufIx / HW_SZ, (UP | LCL | NE));
        if (cpuDest == REM)
        {
		    // open APP CPU Partition here. Download Recovery will need different handling
            fwCRC = fwdk_WriteAppCPU32(OPEN_PARTN, ce.fwCRC);   // open APP partiton
        }
		bufIx    = 0;
	}
	if (byteCount >= iH[0].image_size)         // done?
	{
		if ((bufIx >= 2) && (ce.dlCRC == iH[0].image_CRC) && (fwCRC == ce.fwCRC))
		{
            fwBf16[(bufIx / HW_SZ) - 1] = ce.fwCRC;     // correct dlCRC

			fwCRC = WriteToFlash(fwBuffer,         fwCurrentAddress, bufIx            / HW_SZ, (NU | cpuDest | ER));
			(void)WriteToFlash(phInfo->firstBytes, fwBaseAddress,    NO_BYTES_TO_HOLD / HW_SZ, (NU | cpuDest | NE));
            SetPartitionState((HARDWARE(u32, fwCurrentAddress) >= BANK2_BASE) ? ACTIVATE1 : ACTIVATE0);
            CloseFlashPartition(cpuDest);
#if PC
		    DeCheck2((u8*)fwBaseAddress, iH[0].image_size);
#endif
        }
		else
		{
			rslt = E_SWDL_CHECKSUM_FAIL;
		}
		DownloadPrepare(LOAD_IH);         // get ready for another image header, etc
#if PC
		free((void*)relocTable);
		free((void*)fwBaseAddress);
#endif
	}
	if ((bufIx >= IMG_BUF_SZ) || (byteCount == (iH[0].image_size - 2)))
	{
		// buffer full or only two bytes (the CRC) left
        fwCRC = WriteToFlash(fwBuffer, fwCurrentAddress, bufIx / HW_SZ, (UP | cpuDest | ER));
		bufIx    = 0;
	}
	return rslt;
}

/** \brief the download state machine for the binary section of the file

	\param kh - a plaintext character that is part of a private header,
				image header, relocation table or the firmmware image.
*/
static u8 ProcessDownloadByte(u8 kh)
{
	u8	rslt;

	ce.dlCRC = CRC_Compute(&kh, 1, ce.dlCRC);
	switch (dlState)
	{
		case LOAD_PH:
			rslt = LoadPrivateHeader(kh);
			break;

		case LOAD_IH:
			rslt = LoadImageHeader(kh);
			break;

		case LOAD_RELOC:
			rslt = LoadRelocationTable(kh);
			break;

		case LOAD_FW:
			ce.outbytes++;          // for possible download recovery
			rslt = FirmwareLoad(kh);
			break;

		default:
			rslt = E_SWDL_OTHER;

	}
	return rslt;
}

/** \brief Process a block of firmware download data received by the FF stack
    This function receives data for the Private Header, Image Header, Relocation
    Table or the Firmware image.

    \param segment_id - one of: SWDL_LOAD_FIRST_SEGM, SWDL_LOAD_SEGM , SWDL_LOAD_LAST_SEGM
    \param segment lenghth - up to 100 bytes of data
    \param p_segment - pointer to the downalod data
    \return - E_OK if everything is good; otherwise one of the E_SWDL_... values
*/
//lint -e{818} p_segment could be const * - don't mess with Softing header files
FUNCTION GLOBAL USIGN8 dwnld_handle_data (IN USIGN8 segment_id, IN USIGN8 segment_len, IN USIGN8 *p_segment)
{
	u8 rslt = E_OK;

	while ((segment_len-- != 0u) && (rslt == E_OK))
	{
		ce.inbytes++;
		if (compressed)
		{
		    //rslt = DecompressChar(*p_segment++, ProcessDownloadByte);
		}
		else
		{
			rslt = ProcessDownloadByte(*p_segment++);
		}
	}
	if (segment_id == SWDL_LOAD_LAST_SEGM)
	{

		if ((dlState != LOAD_IH) && (dlState != IDLE))
		{
			rslt = E_SWDL_OTHER;
		}
		dlState = IDLE;
	}
	return rslt;
}

/** \brief Activate the downloaded firmwware.  If either, or both, CPUs are
    updated, the preferred boot bank bit in each is toggled.  On return
    the stack will cause a reset.
*/
FUNCTION GLOBAL BOOL activate_fd_sw  ( VOID )
{
    pstate_t cur = phInfo->state[0], news = {{NOTACTIVE, NOTACTIVE}};

    u32 stamp1 = eep_get_stamp(); //we need to wait until deferred write completes

    OpenFlashPartition(LCL);
	(void)WriteToFlash(&news, &phInfo->state[0], sizeof(pstate_t) / HW_SZ, (NU | LCL | ER));
    CloseFlashPartition(LCL);

    SetActive(REM, cur.st[REM]);
    SetActive(LCL, cur.st[LCL]);

    //First wait for eep cycle to complete
    u32 stamp2;
    do
    {
        Reset_Watchdog();
        OS_Delay(200);
        stamp2 = eep_get_stamp(); //watching for stamp1 change
    } while(stamp2 == stamp1);
    //Second wait for eep cycle to complete
    do
    {
        Reset_Watchdog();
        OS_Delay(200);
        stamp1 = eep_get_stamp(); //now watching for stamp2 change
    } while(stamp2 == stamp1);

    return TRUE;
}

/** \brief Get a  pointer to the version info created below.

    \return - pointer to the FFInfo_t array[4]
*/
FFInfo_t *fwdl_GetVerInfo(void)
{
    return allvers;
}

/** \brief get the assembel the firmware version ofo for each of the four banks of flash
    Two in the FF CPU and two in the APP CPU.
*/
void fwdl_CreateVVerInfo(void)
{
#if 0
    GetFwInfo(smdls_dwnld_hdr + 4);
    memcpy(smdls_dwnld_hdr + 24, fwdk_GetVerInfo(), 20);
#else
    (void)GetFwInfo(allvers);
    memcpy(allvers + 2, fwdk_GetVerInfo(), sizeof(FFInfo_t) * 2);
#endif
}


// end of source
