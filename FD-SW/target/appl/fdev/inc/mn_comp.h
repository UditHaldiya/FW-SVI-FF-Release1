#ifndef _COMP_H_
#define _COMP_H_

#define PERIODIC_RESET  0
#if PERIODIC_RESET
 #define BREAKPOINT	(1u << 31) //16384
#endif

#define DL_ADJ			1				// byte len in output is reduced by this value
#define SL_ADJ			2				// string len in output is reduced by this value
#define OFS_ADJ			1				// string offset in output is reduced by this value

#define MAX_DATA_SIZE	0x10			// maximum byte length
#define DICT_SZ			4096
#define DICT_MSK		(DICT_SZ - 1)

#define MAX_FILE_SIZE	0x80000			// 512 Kb
#define MAX_WINDOW		(4095 + OFS_ADJ)

#define MIN_STRING		3				// minimum string length
#define MAX_STRING		(15 + SL_ADJ)	// maximum string length

typedef struct
{
   u32  Found;                           // Flag found
   u32  Index;                           // Index in the buffer (sequence)
   u32  DicInd;                          // Index to the dictionary
   u32  Size;                            // Size of the detected sequence
} tagScanStruc;


extern void DecompressBlock(const u8 *b, u32 l, u8 (*rls)(u8 kh));
extern u8   DecompressChar(u8 kh, u8 (*rls)(u8 kh));
extern u32	Compress      (u8 *inbuf, u8 *outbuf, u32 insize, u32 flag);


#endif

// end of source
