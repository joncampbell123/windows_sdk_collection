:loop
if not %machine%==1 %root%\pf /n:%3 /%1 /l:128 /m:1 /s:%2 /u /k >nul
if not %machine%==1 if not exist %home%\sync\ready.1 goto loop
if %machine%==1 %root%\sleep 3
if %machine%==1 %root%\pf /n:%4 /%1 /l:128 /m:1 /s:%2 /u /k  >>%output%\test04b.out
call sync

