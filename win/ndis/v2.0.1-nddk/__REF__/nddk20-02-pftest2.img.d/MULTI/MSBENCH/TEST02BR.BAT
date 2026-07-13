call sync
:loop
if not %machine%==1 %root%\pf /n:%2 /r /l:1024 /m:1 /s:%1 /u >nul
if not %machine%==1 if not exist %home%\sync\ready.1 goto loop
if %machine%==1 %root%\sleep 3
if %machine%==1 %root%\pf /n:%3 /r /l:1024 /m:1 /s:%1 /u >>%output%\test02b.out

