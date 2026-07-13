call sync
:loop
if not %machine%==1 %root%\pf /n:%2 /w /l:128 /m:1 /s:%1 /o:test.%machine%  >nul
if not %machine%==1 if not exist %home%\sync\ready.1 goto loop
if %machine%==1 %root%\sleep 3
if %machine%==1 %root%\pf /n:%3 /w /l:128 /m:1 /s:%1 /o:test.%machine%  >>%output%\test01a.out
del test.%machine%

