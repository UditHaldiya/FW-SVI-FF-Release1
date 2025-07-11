#ifndef MN_FLASH_H_
#define MN_FLASH_H_

#define NO_ERASE    1u      // don't erase flash if page change
#define UPD_CRC     2u      // flash driver should compute and return CRC

// offsets in IPC messages (hart command 245 payload)
#define CMD_OFFSET  0
#define FLAG_OFFSET 1
#define ADDR_OFFSET 2
#define DATA_OFFSET 6

#define MAX_PROG_DATA       52      // in bytes - must be muliple of 4 should be
                                    //  at least 52

#define SECT_SIZE			(1u << 11)          // 2 Kb
#define BANK_SIZE			(1u << 19)          // 512 Kb
#define BANK_MASK			(~(BANK_SIZE - 1u)) //
//#define BANK1_LOC           0x8080000u          // maybe should get this from flash driver
//#define BANK_SIZE   (0x80000u)                    // 512 kb
//#define BANK_MASK   (~(BANK_SIZE-1u))
#define BANK1_BASE  (0x8000000u)                  // base of flash bank 1
#define BANK2_BASE  (BANK1_BASE + BANK_SIZE)    // base of flash bank 2

extern s8_least GetFwInfo(void *buf);

typedef struct FFInfo_t
{
    u8 bank;
    u8 sw_revision;
    u8 trans_spec_revision;
    u8 hw_revision;
    u8 fw_type;
    u8 ManufacturerDeviceCode;
    u8 date_str[4];
} FFInfo_t;

extern u32 flash_SetPartition(u32 BootFromBank2);
extern u32 flash_OpenPartition(u32 sCrc);
extern u32 flash_ClosePartition(u32 dummy);
extern u32 flash_ProgramBlock(const u16 *data, u16 *adr, u32 len, u8_least flags);
extern u32 flash_GetPartitionBaseAddress(u32 dummy);

MN_DECLARE_API_FUNC(fwdk_WriteAppCPU, fwdk_WriteAppCPU32)   // these are only used in FF CPU
extern u32 fwdk_WriteAppCPU  (const void *data, u32 addr, u8_least len, u8_least flags);
extern u32 fwdk_WriteAppCPU32(u8_least cmdtype, u32 value);
MN_DECLARE_API_FUNC(fwdk_GetVerInfo)    // only used in FF CPU
extern void *fwdk_GetVerInfo(void);
MN_DECLARE_API_FUNC(fwdl_GetVerInfo,fwdl_CreateVVerInfo)    // only used in FF CPU
extern FFInfo_t *fwdl_GetVerInfo(void);
extern void fwdl_CreateVVerInfo(void);

// list of (sub) commands for HC 245
enum {SET_PARTN, OPEN_PARTN, CLOSE_PARTN, PROG_BLOCK, PARTN_BASE, VER_INFO, FMAX_OP};

//-------------------------------------------------------

extern u8   flash_GetRunningPartitionID(void);
MN_DECLARE_API_FUNC(flash_GetRunningPartitionID)

#endif // MN_FLASH_H_

// end of source
