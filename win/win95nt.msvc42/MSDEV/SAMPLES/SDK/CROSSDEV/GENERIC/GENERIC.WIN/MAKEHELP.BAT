
echo Building Win32 Help files
call hcrtf -x generic.hpj
if exist debug copy generic.hlp debug
if exist release copy generic.hlp release
