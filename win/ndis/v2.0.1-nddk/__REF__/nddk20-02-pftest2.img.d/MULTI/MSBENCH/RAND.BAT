:loop1
if not exist start.up goto loop1
pf /w  /l:128  /s:32k  /m:1 /n:1 /o:test.%machine% > nul
if  not %machine% == 1 pf /3 /s:32 /l:128  /m:3  /n:2 /o:test.%machine% > nul
if  %machine% == 1 pf /3 /s:32 /l:128  /m:2  /n:1 /o:test.%machine% 
pause
if  not %machine% == 1 pf /2 /s:32 /l:128  /m:6  /n:2 /o:test.%machine% > nul
if  %machine% == 1 pf /2 /s:32 /l:128  /m:2  /n:1 /o:test.%machine% 
