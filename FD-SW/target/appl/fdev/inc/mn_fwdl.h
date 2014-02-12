#ifndef DOWNLOAD_H_
#define DOWNLOAD_H_

#define PC !defined(__ICCARM__)				// otherwise the actual STMicro target
#ifdef _lint
 #define SW_DOWNLOAD
#endif

#ifndef SW_DOWNLOAD
 typedef unsigned char  USIGN8;
 typedef unsigned long  USIGN32;
 typedef unsigned short USIGN16;
 typedef char		   CHAR;

 typedef enum {FD_SW_DOM_ID, AP_SW_DOM_ID } GOM_ID;
#endif

#ifndef SW_DOWNLOAD
 enum
 {
	E_OK,
	E_SWDL_CHECKSUM_FAIL,
	E_SWDL_OTHER,
	E_SWDL_TEMP_DENIED,
	E_SWDL_NOT_POSSIBLE
 } ;
 enum {SWDL_LOAD_FIRST_SEGM, SWDL_LOAD_SEGM, SWDL_LOAD_LAST_SEGM};
#endif

#define SET_BITD(a,n)  ( a[(n) / 8] |= (0x80 >> ((n) & 7)))
#define TST_BITD(a,n)  ((a[(n) / 8]  & (0x80 >> ((n) & 7))) != 0)

/*                       min   typ  max
16-bit programming time       52.5   70 us
page (2 KB) erase time    20         40 ms
*/

#ifndef SW_DOWNLOAD
typedef struct T_DOMAIN_HDR_API
{
     USIGN16 Header_Version_Number;
     USIGN16 Header_Size;
     CHAR    Manufacturer_Id[6];
     CHAR    Device_Family[4];
     CHAR    Device_Type[4];
     USIGN8  Device_Revision;
     USIGN8  DD_Revision;
     CHAR    Software_Revision[8];
     CHAR    Software_Name[8];
     CHAR    Domain_Name[8];
} T_DOMAIN_HDR_API;
#endif

typedef struct private_header_t
{
    USIGN16 header_version;		// private header format: initial value 1
    USIGN16 header_length;		// private header length: initial value 36
    USIGN8  FF_fw_version[4];   //
    USIGN8  APP_fw_version[4];	//
    USIGN32 spare[5];			// room to grow
} private_header_t;

typedef struct image_header_t
{
    USIGN16 header_version;		// image header format: initial value 1
    USIGN16 header_length;		// image header length: initial value 24
    USIGN32 offset_to_next_ih;	// relative offset to next image in .ffd …
					            //    file relative to end of image header
    USIGN32 image_address;	 	// used if relocation_size == 0
    USIGN32 image_size;
    USIGN16 image_CRC;               	//
    USIGN16 relocation_size;        	// 0 == no relocation
    USIGN16 relocation_CRC;
    USIGN8  destination_cpu_flag;   	// 0 == FF,   1 == APP
    USIGN8  compression_method;     	// 0 == NONE; 1 = modifed LZ77, ...
} image_header_t;

/*
Notes:
	1. there iis one domain header and 1 private header per file.
    2. the image header immediately follows the private header
    3. the relocation table, if any, immediately follows the image header
	4. the firmware image immediately follows the relocation table.
	5. the image address is the load (vector) address if there is no relocation
       table; otherwise ignored
	6. in the relocation case, the load address comes from the partition manager.
	7  items 2 throgh 6 may occur for the other CPU
*/

#ifndef SW_DOWNLOAD
 #define FUNCTION
 #define PUBLIC
 #define GLOBAL
 #define IN
 #define VOID void
#endif
#if (!defined  SW_DOWNLOAD) || (defined _lint)
 FUNCTION GLOBAL T_DOMAIN_HDR_API * start_prepare_swdl  ( VOID  );
 FUNCTION GLOBAL USIGN8 appl_check_fd_dom_hdr  ( IN const T_DOMAIN_HDR_API * p_dwnld_dom_hdr );
 FUNCTION GLOBAL USIGN8 dwnld_handle_data ( IN USIGN8 segment_id, IN USIGN8 segment_len, IN USIGN8 *p_segment);
 FUNCTION GLOBAL BOOL activate_fd_sw  ( VOID );
#endif

extern bool_t vh_CheckDomainHeader(T_DOMAIN_HDR_API vH);
extern bool_t vh_CheckPrivateHeader(private_header_t pH);
extern T_DOMAIN_HDR_API *vh_GetDomainHeader(void);

#if PC
void DeCheck2(u8 *dat, u32 sz);
#endif

#endif // DOWNLOAD_H_

// end of source
