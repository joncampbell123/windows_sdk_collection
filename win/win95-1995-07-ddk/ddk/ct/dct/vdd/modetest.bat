@echo off

rem *************************************************************************
rem  save old dircmd environment variable, set no dircmd
rem *************************************************************************
set oldcmd=%dircmd%
if not "%oldcmd%"=="%dircmd%" goto err_lowenv
set dircmd=


rem *************************************************************************
rem  Start of Test
rem *************************************************************************

rem *** 25 lines ***
echo Changing Mode to "CON:LINES=25" >> %1
mode con:lines=25
delayf 5 "noSound" "noNum" "[Preparing 25 line mode.  Please wait...]"
echo Scrolling Text >> %1
for %%i in (a b c d e) do call mt2.bat 25_Lines

rem *** 40 columns ***
echo Changing Mode to "CON:COLS=40" >> %1
mode con:cols=40
delayf 5 "noSound" "noNum" "[Preparing 40 column mode.  Please wait...]"
echo Scrolling Text >> %1
for %%i in (a b c d e) do call mt2.bat 40_Columns

rem *** 80 columns ***
echo Changing Mode to "CON:COLS=80" >> %1
mode con:cols=80
delayf 5 "noSound" "noNum" "[Preparing 80 column mode.  Please wait...]"
echo Scrolling Text >> %1
for %%i in (a b c d e) do call mt2.bat 80_Columns

rem *** 43 lines ***
echo Changing Mode to "CON:LINES=43" >> %1
mode con:lines=43
delayf 5 "noSound" "noNum" "[Preparing 43 line mode.  Please wait...]"
echo Scrolling Text >> %1
for %%i in (a b c d e) do call mt2.bat 43_Lines

rem *** 50 lines ***
echo Changing Mode to "CON:LINES=50" >> %1
mode con:lines=50
delayf 5 "noSound" "noNum" "[Preparing 50 line mode.  Please wait...]"
echo Scrolling Text >> %1
for %%i in (a b c d e) do call mt2.bat 50_Lines

rem *** 25 lines ***
echo Changing Mode to "CON:LINES=25" >> %1
mode con:lines=25
delayf 5 "noSound" "noNum" "[Preparing 25 line mode.  Please wait...]"
echo Scrolling Text >> %1
for %%i in (a b c d e) do call mt2.bat 25_Lines

rem *** mono ***
echo Changing Mode to "MONO" >> %1
mode mono
delayf 5 "noSound" "noNum" "[Preparing mono mode.  Please wait...]"
echo Scrolling Text >> %1
for %%i in (a b c d e) do call mt2.bat MONO

rem *** co40 ***
echo Changing Mode to "CO40" >> %1
mode co40
delayf 5 "noSound" "noNum" "[Preparing 40 column (co40) mode.  Please wait...]"
echo Scrolling Text >> %1
for %%i in (a b c d e) do call mt2.bat 40_Columns_Color

rem *** bw80 ***
echo Changing Mode to "BW80" >> %1
mode bw80
delayf 5 "noSound" "noNum" "[Preparing 80 column (bw80) mode.  Please wait...]"
echo Scrolling Text >> %1
for %%i in (a b c d e) do call mt2.bat 80_Columns_Black_and_White

rem *** bw40 ***
echo Changing Mode to "BW40" >> %1
mode bw40
delayf 5 "noSound" "noNum" "[Preparing 40 column (bw40) mode.  Please wait...]"
echo Scrolling Text >> %1
for %%i in (a b c d e) do call mt2.bat 40_Columns_Black_and_White

rem *** co80 ***
echo Changing Mode to "CO80" >> %1
mode co80                  
delayf 5 "noSound" "noNum" "[Preparing 80 column (co80) mode.  Please wait...]"
echo Scrolling Text >> %1
for %%i in (a b c d e) do call mt2.bat 80_Columns_Color

rem *** all done ***
goto finish

:finish
rem *************************************************************************
rem  restore dircmd environment variable, delete oldcmd
rem *************************************************************************
set dircmd=%oldcmd%
set oldcmd=
goto end

:err_lowenv
rem *************************************************************************
rem  Insufficent Envronment Space
rem *************************************************************************
set dircmd=%oldcmd%
set oldcmd=
echo.
echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
echo ! ERROR : Not Enough Environment Space !
echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
echo.
pause
goto end

:end
