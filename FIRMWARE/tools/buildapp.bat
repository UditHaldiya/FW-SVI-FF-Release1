 @echo off
 if exist cmdbuild\Dbg\project_FFAP_0\. rd cmdbuild\Dbg\project_FFAP_0 /s /q
 gnumake proj=FFAP BUILD
 cp -r  cmdbuild\Dbg\project_FFAP cmdbuild\Dbg\project_FFAP_0
 gnumake proj=FFAP MEMMAP MEMMAP_ROMSTART=08080000 MEMMAP_ROMEND=080F7777 -B
 gnumake proj=FFAP BUILD NOGEN=1 NOLINKREFRESH=1
 gnumake proj=FFAP MEMMAP -B
