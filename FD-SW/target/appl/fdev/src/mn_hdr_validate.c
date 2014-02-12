#ifndef __ICCARM__
 #include <windows.h>
 #include <stdio.h>
#endif
#include <string.h>
#include <softing_base.h>
#include "swdl_api.h"
#include "crc.h"
#include "mn_comp.h"
#include "mnhart2ff.h"
#include "mn_fwdl.h"

#ifndef SW_DOWNLOAD
static T_DOMAIN_HDR_API dH =
{
    1,							// Header_Version_Number;
	sizeof(T_DOMAIN_HDR_API),	// Header_Size;
	"GE Ene",					// Manufacturer_Id[6];
	"SVIP",						// Device_Family[4];
	"0002",						// Device_Type[4];
	1,							// Device_Revision;
	1,							// DD_Revision;
	"Rev 1.02",					// Software_Revision[8];
	"APPLICAT",					// Software_Name[8];
	"FF_BOARD"					// Domain_Name[8];
};
#else
 extern T_DOMAIN_HDR_API       appl_fd_dom_hdr;
#endif

T_DOMAIN_HDR_API *vh_GetDomainHeader(void)
{
#ifdef SW_DOWNLOAD
    return &appl_fd_dom_hdr;
#else
    return &dH;
#endif
}

bool_t vh_CheckDomainHeader(T_DOMAIN_HDR_API vH)
{
    (void)vH;
    return true;
}

// end of source
