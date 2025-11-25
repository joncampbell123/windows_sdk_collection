@echo off
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by $$ROOT$$.HPJ. >hlp\$$root$$.hm
echo. >>hlp\$$root$$.hm
echo // Dialogs (IDD_*) >>hlp\$$root$$.hm
makehm IDD_,HIDD_,0x20000 resource.h >>hlp\$$root$$.hm
echo. >>hlp\$$root$$.hm

REM -- Make help for Project $$ROOT$$

call hc31 $$root$$.hpj
if exist windebug copy $$root$$.hlp windebug
if exist winrel copy $$root$$.hlp winrel
