@echo off
REM -- First make map file from App Studio generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by HIERSVR.HPJ. >hlp\hiersvr.hm
echo. >>hlp\hiersvr.hm
echo // Commands (ID_* and IDM_*) >>hlp\hiersvr.hm
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>hlp\hiersvr.hm
echo. >>hlp\hiersvr.hm
echo // Prompts (IDP_*) >>hlp\hiersvr.hm
makehm IDP_,HIDP_,0x30000 resource.h >>hlp\hiersvr.hm
echo. >>hlp\hiersvr.hm
echo // Resources (IDR_*) >>hlp\hiersvr.hm
makehm IDR_,HIDR_,0x20000 resource.h >>hlp\hiersvr.hm
echo. >>hlp\hiersvr.hm
echo // Dialogs (IDD_*) >>hlp\hiersvr.hm
makehm IDD_,HIDD_,0x20000 resource.h >>hlp\hiersvr.hm
echo. >>hlp\hiersvr.hm
echo // Frame Controls (IDW_*) >>hlp\hiersvr.hm
makehm IDW_,HIDW_,0x50000 resource.h >>hlp\hiersvr.hm
REM -- Make help for Project HIERSVR
call hc31 hiersvr.hpj
echo.
