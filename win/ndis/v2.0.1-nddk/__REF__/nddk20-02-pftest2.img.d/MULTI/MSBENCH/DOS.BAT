set os=DOS
set path=%1:\multi\msbench;%path%
set local=%1:
set remote=%1:
set home=%remote%\multi
set root=%local%\multi\msbench
set lroot=c:\multi\msbench
cd %root%
%local%
test
if not %machine%== 1 goto exit1
echo Performance data collection over; Report generation begins
process %1 %2
:exit1
