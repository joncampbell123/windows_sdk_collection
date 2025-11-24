Rem Make the RPC runtime only setup disks for Windows.

set RPCSRC=a:
set RPCDST=a:

if not `%1 == ` set RPCSRC=%1
if not `%2 == ` set RPCDST=%2

pause Put distribution disk 1 into drive A: and then hit enter.

md rpcset.t
cd rpcset.t

copy %RPCSRC%setuprun.lst setup.lst
copy %RPCSRC%setup.exe
copy %RPCSRC%RPCSDK.mst
copy %RPCSRC%RPCRUN.mst
copy %RPCSRC%RPCRUN.in_ rpcsdk.in_
copy %RPCSRC%setupapi.in_
copy %RPCSRC%msdetect.in_
copy %RPCSRC%mscomstf.dl_
copy %RPCSRC%msinsstf.dl_
copy %RPCSRC%msuilstf.dl_
copy %RPCSRC%msshlstf.dl_
copy %RPCSRC%mscuistf.dl_
copy %RPCSRC%msdetstf.dl_
copy %RPCSRC%_mstest.ex_
copy %RPCSRC%aver.dl_
copy %RPCSRC%dnetapi.dl_
copy %RPCSRC%rpc.hl_
copy %RPCSRC%netapi.dl_
copy %RPCSRC%readme.tx_
copy %RPCSRC%toolhelp.dl_

copy %RPCSRC%rpc*.dl*
copy %RPCSRC%rpc*.rp*
copy %RPCSRC%secur*.*

ver >midl.ex_

pause Insert a formatted floppy disk into %RPCDST% and then hit enter

cd ..
copy rpcset.t  %RPCDST%
echo y | del rpcset.t
rd rpcset.t

set RPCSRC=
set RPCDST=
