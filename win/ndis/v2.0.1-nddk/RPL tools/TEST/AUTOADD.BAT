net use r: \\%1\rpltools
r:getremtm \\%1
call setrmtod
del setrmtod.bat
echo %REMTIME% %REMDATE% - PASSED.  %2 booted DOS.>>bootrec.log
rbtdos
