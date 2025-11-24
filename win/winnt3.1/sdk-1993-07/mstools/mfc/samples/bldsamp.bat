if "%1"=="retail" goto setret
if "%1"=="debug" goto setdbg
echo usage: bldsamp [retail^|debug]
goto done
:setret
set _EXEDIR=rexe
set _BLDLOG=bldr.log
set _BLDTYPE=RETAIL
goto start
:setdbg
set _EXEDIR=dexe
set _BLDLOG=bldd.log
set _BLDTYPE=DEBUG
goto start
:start
if not exist %_EXEDIR% mkdir %_EXEDIR%
echo START >>%_EXEDIR%\%_BLDLOG%
if "%_BLDTYPE%"=="RETAIL" goto bldret
:blddbg
rem
rem Debug builds
rem
cd about2
nmake clean DEBUG=1 
nmake DEBUG=1 >%_BLDLOG%
copy about2.exe ..\%_EXEDIR%
copy /A ..\%_EXEDIR%\%_BLDLOG%+%_BLDLOG% ..\%_EXEDIR%\%_BLDLOG%
nmake clean DEBUG=1 
cd ..
cd chart
nmake clean DEBUG=1
nmake DEBUG=1 >%_BLDLOG%  
copy chart.exe ..\%_EXEDIR%
copy /A ..\%_EXEDIR%\%_BLDLOG%+%_BLDLOG% ..\%_EXEDIR%\%_BLDLOG%
nmake clean DEBUG=1 
cd ..
cd minmdi
nmake clean DEBUG=1
nmake DEBUG=1 >%_BLDLOG%  
copy minmdi.exe ..\%_EXEDIR%
copy /A ..\%_EXEDIR%\%_BLDLOG%+%_BLDLOG% ..\%_EXEDIR%\%_BLDLOG%
nmake clean DEBUG=1 
cd ..
cd mdi
nmake clean DEBUG=1
nmake DEBUG=1 >%_BLDLOG%  
copy mdi.exe ..\%_EXEDIR%
copy /A ..\%_EXEDIR%\%_BLDLOG%+%_BLDLOG% ..\%_EXEDIR%\%_BLDLOG%
nmake clean DEBUG=1 
cd ..
cd fileview
nmake clean DEBUG=1
nmake DEBUG=1 >%_BLDLOG%
copy fileview.exe ..\%_EXEDIR%
copy /A ..\%_EXEDIR%\%_BLDLOG%+%_BLDLOG% ..\%_EXEDIR%\%_BLDLOG%
nmake clean DEBUG=1 
cd ..
cd multipad
nmake clean DEBUG=1
nmake DEBUG=1 >%_BLDLOG%  
copy multipad.exe ..\%_EXEDIR%
copy /A ..\%_EXEDIR%\%_BLDLOG%+%_BLDLOG% ..\%_EXEDIR%\%_BLDLOG%
nmake clean DEBUG=1 
cd ..
cd hello
nmake clean DEBUG=1
nmake DEBUG=1 >%_BLDLOG%  
copy hello.exe ..\%_EXEDIR%
copy /A ..\%_EXEDIR%\%_BLDLOG%+%_BLDLOG% ..\%_EXEDIR%\%_BLDLOG%
nmake clean DEBUG=1 
cd ..
cd showfont
nmake clean DEBUG=1
nmake DEBUG=1 >%_BLDLOG%  
copy showfont.exe ..\%_EXEDIR%
copy /A ..\%_EXEDIR%\%_BLDLOG%+%_BLDLOG% ..\%_EXEDIR%\%_BLDLOG%
nmake clean DEBUG=1 
cd ..
cd oclient
nmake clean DEBUG=1
nmake DEBUG=1 >%_BLDLOG%  
copy oclient.exe ..\%_EXEDIR%
copy /A ..\%_EXEDIR%\%_BLDLOG%+%_BLDLOG% ..\%_EXEDIR%\%_BLDLOG%
nmake clean DEBUG=1 
cd ..
cd oserver
nmake clean DEBUG=1
nmake DEBUG=1 >%_BLDLOG%  
copy bibref.exe ..\%_EXEDIR%
copy /A ..\%_EXEDIR%\%_BLDLOG%+%_BLDLOG% ..\%_EXEDIR%\%_BLDLOG%
nmake clean DEBUG=1 
cd ..
cd tracer
nmake clean DEBUG=1
nmake DEBUG=1 >%_BLDLOG%  
copy tracer.exe ..\%_EXEDIR%
copy /A ..\%_EXEDIR%\%_BLDLOG%+%_BLDLOG% ..\%_EXEDIR%\%_BLDLOG%
nmake clean DEBUG=1 
cd ..
cd templdef
nmake clean DEBUG=1
nmake DEBUG=1 >%_BLDLOG%  
copy templdef.exe ..\%_EXEDIR%
copy /A ..\%_EXEDIR%\%_BLDLOG%+%_BLDLOG% ..\%_EXEDIR%\%_BLDLOG%
nmake clean DEBUG=1 
cd ..
cd tutorial
nmake -f phbook clean DEBUG=1
nmake -f phbook DEBUG=1 >%_BLDLOG%
copy WIN\phbook.exe ..\%_EXEDIR%
nmake -f phbook clean DEBUG=1
copy /A ..\%_EXEDIR%\%_BLDLOG%+%_BLDLOG% ..\%_EXEDIR%\%_BLDLOG%
cd ..
cd minsvr
nmake clean DEBUG=1
nmake DEBUG=1 >%_BLDLOG%  
copy minsvr.exe ..\%_EXEDIR%
copy /A ..\%_EXEDIR%\%_BLDLOG%+%_BLDLOG% ..\%_EXEDIR%\%_BLDLOG%
nmake clean DEBUG=1
cd ..
cd minsvrmi
nmake clean DEBUG=1
nmake DEBUG=1 >%_BLDLOG%  
copy minsvrmi.exe ..\%_EXEDIR%
copy /A ..\%_EXEDIR%\%_BLDLOG%+%_BLDLOG% ..\%_EXEDIR%\%_BLDLOG%
nmake clean DEBUG=1
cd ..
cd ctrltest
nmake clean DEBUG=1
nmake DEBUG=1 >%_BLDLOG%  
copy ctrltest.exe ..\%_EXEDIR%
copy muscroll.dll ..\%_EXEDIR%
copy /A ..\%_EXEDIR%\%_BLDLOG%+%_BLDLOG% ..\%_EXEDIR%\%_BLDLOG%
nmake clean DEBUG=1
cd ..

goto cleanup
:bldret
rem
rem Retail builds
rem
cd about2
nmake clean DEBUG=0 
nmake DEBUG=0 >%_BLDLOG%
copy about2.exe ..\%_EXEDIR%
copy /A ..\%_EXEDIR%\%_BLDLOG%+%_BLDLOG% ..\%_EXEDIR%\%_BLDLOG%
nmake clean DEBUG=1 
cd ..
cd chart
nmake clean DEBUG=0
nmake DEBUG=0 >%_BLDLOG%  
copy chart.exe ..\%_EXEDIR%
copy /A ..\%_EXEDIR%\%_BLDLOG%+%_BLDLOG% ..\%_EXEDIR%\%_BLDLOG%
nmake clean DEBUG=1 
cd ..
cd minmdi
nmake clean DEBUG=0
nmake DEBUG=0 >%_BLDLOG%  
copy minmdi.exe ..\%_EXEDIR%
copy /A ..\%_EXEDIR%\%_BLDLOG%+%_BLDLOG% ..\%_EXEDIR%\%_BLDLOG%
nmake clean DEBUG=1 
cd ..
cd mdi
nmake clean DEBUG=0
nmake DEBUG=0 >%_BLDLOG%  
copy mdi.exe ..\%_EXEDIR%
copy /A ..\%_EXEDIR%\%_BLDLOG%+%_BLDLOG% ..\%_EXEDIR%\%_BLDLOG%
nmake clean DEBUG=1 
cd ..
cd fileview
nmake clean DEBUG=0
nmake DEBUG=0 >%_BLDLOG%
copy fileview.exe ..\%_EXEDIR%
copy /A ..\%_EXEDIR%\%_BLDLOG%+%_BLDLOG% ..\%_EXEDIR%\%_BLDLOG%
nmake clean DEBUG=1 
cd ..
cd multipad
nmake clean DEBUG=0
nmake DEBUG=0 >%_BLDLOG%  
copy multipad.exe ..\%_EXEDIR%
copy /A ..\%_EXEDIR%\%_BLDLOG%+%_BLDLOG% ..\%_EXEDIR%\%_BLDLOG%
nmake clean DEBUG=1 
cd ..
cd hello
nmake clean DEBUG=0
nmake DEBUG=0 >%_BLDLOG%  
copy hello.exe ..\%_EXEDIR%
copy /A ..\%_EXEDIR%\%_BLDLOG%+%_BLDLOG% ..\%_EXEDIR%\%_BLDLOG%
nmake clean DEBUG=1 
cd ..
cd helloapp
nmake clean DEBUG=0
nmake DEBUG=0 >%_BLDLOG%  
copy helloapp.exe ..\%_EXEDIR%
copy /A ..\%_EXEDIR%\%_BLDLOG%+%_BLDLOG% ..\%_EXEDIR%\%_BLDLOG%
nmake clean DEBUG=1 
cd ..
cd showfont
nmake clean DEBUG=0
nmake DEBUG=0 >%_BLDLOG%  
copy showfont.exe ..\%_EXEDIR%
copy /A ..\%_EXEDIR%\%_BLDLOG%+%_BLDLOG% ..\%_EXEDIR%\%_BLDLOG%
nmake clean DEBUG=1 
cd ..
cd oclient
nmake clean DEBUG=0
nmake DEBUG=0 >%_BLDLOG%  
copy oclient.exe ..\%_EXEDIR%
copy /A ..\%_EXEDIR%\%_BLDLOG%+%_BLDLOG% ..\%_EXEDIR%\%_BLDLOG%
nmake clean DEBUG=1 
cd ..
cd oserver
nmake clean DEBUG=0
nmake DEBUG=0 >%_BLDLOG%  
copy bibref.exe ..\%_EXEDIR%
copy /A ..\%_EXEDIR%\%_BLDLOG%+%_BLDLOG% ..\%_EXEDIR%\%_BLDLOG%
nmake clean DEBUG=1 
cd ..
cd tracer
nmake clean DEBUG=0
nmake DEBUG=0 >%_BLDLOG%  
copy tracer.exe ..\%_EXEDIR%
copy /A ..\%_EXEDIR%\%_BLDLOG%+%_BLDLOG% ..\%_EXEDIR%\%_BLDLOG%
nmake clean DEBUG=1 
cd ..
cd templdef
nmake clean DEBUG=0
nmake DEBUG=0 >%_BLDLOG%  
copy templdef.exe ..\%_EXEDIR%
copy /A ..\%_EXEDIR%\%_BLDLOG%+%_BLDLOG% ..\%_EXEDIR%\%_BLDLOG%
nmake clean DEBUG=1 
cd ..
cd tutorial
nmake -f phbook clean DEBUG=0
nmake -f phbook DEBUG=0 >%_BLDLOG%
copy WIN\phbook.exe ..\%_EXEDIR%
nmake -f phbook clean DEBUG=1
cd ..

cd minsvr
nmake clean DEBUG=0
nmake DEBUG=0 >%_BLDLOG%
copy minsvr.exe ..\%_EXEDIR%
copy /A ..\%_EXEDIR%\%_BLDLOG%+%_BLDLOG% ..\%_EXEDIR%\%_BLDLOG%
nmake clean DEBUG=0
cd ..
cd minsvrmi
nmake clean DEBUG=0
nmake DEBUG=0 >%_BLDLOG%
copy minsvrmi.exe ..\%_EXEDIR%
copy /A ..\%_EXEDIR%\%_BLDLOG%+%_BLDLOG% ..\%_EXEDIR%\%_BLDLOG%
nmake clean DEBUG=0
cd ..
cd ctrltest
nmake clean DEBUG=0
nmake DEBUG=0 >%_BLDLOG%
copy /A ..\%_EXEDIR%\%_BLDLOG%+%_BLDLOG% ..\%_EXEDIR%\%_BLDLOG%
copy ctrltest.exe ..\%_EXEDIR%
rem copy muscroll.dll ..\%_EXEDIR%
nmake clean DEBUG=0
cd ..
cd dlltrace
nmake clean DEBUG=1
nmake DEBUG=1 >%_BLDLOG%
copy tracer.dll ..\%_EXEDIR%
copy hello1.exe ..\%_EXEDIR%
copy /A ..\%_EXEDIR%\%_BLDLOG%+%_BLDLOG% ..\%_EXEDIR%\%_BLDLOG%
cd ..


goto cleanup
:cleanup
set _EXEDIR=
set _BLDLOG=
set _BLDTYPE=
:done
