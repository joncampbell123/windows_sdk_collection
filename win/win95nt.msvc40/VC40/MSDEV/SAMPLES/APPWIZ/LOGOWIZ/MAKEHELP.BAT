@echo off
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by LOGOWIZ.HPJ. >hlp\logowiz.hm
echo. >>hlp\logowiz.hm
echo // Dialogs (IDD_*) >>hlp\logowiz.hm
makehm IDD_,HIDD_,0x20000 resource.h >>hlp\logowiz.hm
echo. >>hlp\logowiz.hm

REM -- Make help for Project LOGOWIZ

call hc31 logowiz.hpj
if exist windebug copy logowiz.hlp windebug
if exist winrel copy logowiz.hlp winrel
