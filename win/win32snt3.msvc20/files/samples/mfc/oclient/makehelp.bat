@echo off
REM -- First make map file from App Studio generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by OCLIENT.HPJ. >hlp\oclient.hm
echo. >>hlp\oclient.hm
echo // Commands (ID_* and IDM_*) >>hlp\oclient.hm
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>hlp\oclient.hm
echo. >>hlp\oclient.hm
echo // Prompts (IDP_*) >>hlp\oclient.hm
makehm IDP_,HIDP_,0x30000 resource.h >>hlp\oclient.hm
echo. >>hlp\oclient.hm
echo // Resources (IDR_*) >>hlp\oclient.hm
makehm IDR_,HIDR_,0x20000 resource.h >>hlp\oclient.hm
echo. >>hlp\oclient.hm
echo // Dialogs (IDD_*) >>hlp\oclient.hm
makehm IDD_,HIDD_,0x20000 resource.h >>hlp\oclient.hm
echo. >>hlp\oclient.hm
echo // Frame Controls (IDW_*) >>hlp\oclient.hm
makehm IDW_,HIDW_,0x50000 resource.h >>hlp\oclient.hm
REM -- Make help for Project OCLIENT
call hc31 oclient.hpj
echo.
