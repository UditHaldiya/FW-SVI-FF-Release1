#if 0

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
#endif
#include <softing_base.h>
#include "crc.h"
#include "mn_comp.h"

#else
#ifndef __ICCARM__
 #include <stdio.h>
 #include <stdarg.h>
 #include <stdlib.h>
#endif
#include <softing_base.h>
#include "mn_comp.h"
#endif

// decompression states
typedef enum		{CMND, DATA, STRING} DE_STATE;

static u8		dictionary[DICT_SZ];
static u32		outCount = 0;
static u32		deBytesIn = 0, deBytesOut = 0;

/** \brief copy decompressed char to output and to the dictionary

	\param kh - a decompressed character
*/
static u8 StoreChar(u8 kr, u8 (*rls)(u8 kh))
{
	deBytesOut++;
    UNUSED_OK(deBytesOut);

	dictionary[outCount++ & DICT_MSK] = kr;		// enter char in dictionary
	return rls(kr);								// send char to output stream
}
#if PERIODIC_RESET
 static u32 /*breakpoint,*/ index = 0;
#endif

u8 DecompressChar(u8 kh, u8 (*rls)(u8 kh))
{
	static  DE_STATE	deState = CMND;
	static	u32			size;			// size of the data chunk or string
			u32			offset;			// offset of the string in the dictionary
			u8			rslt = 0;       //E_OK;

	deBytesIn++;
    UNUSED_OK(deBytesIn);
	switch (deState)
	{
		case CMND:							// Data Prefix
#if PERIODIC_RESET
			if (index >= (breakpoint + BREAKPOINT))
			{
				printf("Dec break %d\n", index);
				breakpoint= index;
			}
#endif
			size = kh;
			if (size < MAX_DATA_SIZE)
			{
				deState = DATA;
				size   += DL_ADJ;
			}
			else
			{
				deState = STRING;
			}
			break;

		case DATA:							// Data block
			rslt = StoreChar(kh, rls);
			size--;
			if (size == 0)
			{
				deState = CMND;
			}
			break;

		case STRING:						// string from dictionary
			offset = ((size & 0xf) << 8) | kh;
			offset = outCount - (offset + OFS_ADJ);
			size   = (size >> 4) + SL_ADJ;
			while (size-- != 0)
			{
				rslt = StoreChar(dictionary[offset & DICT_MSK], rls);
				if (rslt != 0)
				{
					break;
				}
				offset++;
			}
			deState = CMND;
			break;
	}
	return rslt;
}

/** \brief decompression state machine

	\param b - pointer to a compressed data block
	\param l - length of the compressed block
*/
void DecompressBlock(const u8 *b, u32 l, u8 (*rls)(u8 kh))
{
	while (l-- != 0)
	{
		(void)DecompressChar(*b++, rls);
#if PERIODIC_RESET
		index++;
#endif
	}
}
