echo off
rem create the following scratch file to eliminate file not found errors
rem from fgrep
echo xxx > foo.log
if exist pass.lst copy pass.lst passlst.old
if exist fail.lst copy fail.lst faillst.old
walk /d /p . fgrep -y SQLKEY %%s\*.log | fgrep -y PASS > pass.lst
walk /d /p . fgrep -y SQLKEY %%s\*.log | fgrep -y FAIL > fail.lst
echo Number of PASS's^: > pass.cnt
fgrep -cy PASS pass.lst >> pass.cnt
echo Number of FAIL's^: > fail.cnt
fgrep -cy FAIL fail.lst >> fail.cnt
del foo.log
echo **********************************************
type pass.cnt
type fail.cnt
echo.
echo See pass.lst and fail.lst for specific listing
echo **********************************************
echo on

