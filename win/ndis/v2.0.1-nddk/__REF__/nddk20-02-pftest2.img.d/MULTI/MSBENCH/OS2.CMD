set os=OS2
set local=%1:
set remote=%1:
set home=%remote%\multi
set root=%local%\multi\msbench
cd %root%
%local%
if not %machine%== 1 goto doit1

copy *.bat *.cmd

:doit1
test
if not %machine%== 1 goto exit1
echo Performance data collection complete; report generation begins
process %1 %2
:exit1
