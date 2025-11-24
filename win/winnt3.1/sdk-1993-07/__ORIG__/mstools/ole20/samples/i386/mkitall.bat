REM -----------------------------------
REM Build bttncur demo
REM
cd bttncur
nmake
copy bttncur.lib ..\lib
copy bttncur.dll ..\bin
cd ..

REM -----------------------------------
REM Build gizmobar demo
REM
cd gizmobar
nmake
copy gizmobar.lib ..\lib
copy gizmobar.dll ..\bin
cd ..

REM -----------------------------------
REM Build ole2ui demo
REM
cd ole2ui
echo # > uimake.ini
nmake
copy outlui.dll ..\bin
copy outlui.lib ..\lib
copy uiclass.h ..\h
cd ..

REM -----------------------------------
REM Build outline demo
REM
cd outline
nmake APP=cntroutl
nmake APP=svroutl
nmake APP=icntrotl
nmake APP=isvrotl
nmake APP=outline
copy cntroutl\cntroutl.exe ..\bin
copy svroutl\svroutl.exe   ..\bin
copy icntrotl\icntrotl.exe ..\bin
copy isvrotl\isvrotl.exe   ..\bin
copy outline\outline.exe   ..\bin
copy outline.reg           ..\bin
cd ..

REM -----------------------------------
REM Build dispcalc demo
REM
cd dispcalc
nmake clean
nmake dev=win32
copy *.exe ..\bin
copy *.reg ..\bin
cd ..

REM -----------------------------------
REM Build dspcalc2 demo
REM
cd dspcalc2
nmake clean
nmake dev=win32
copy *.exe ..\bin
copy *.reg ..\bin
copy *.tlb ..\bin
cd ..

REM -----------------------------------
REM Build tibrowse demo
REM
cd tibrowse
nmake clean
nmake dev=win32
copy *.exe ..\bin
cd ..

REM -----------------------------------
REM Build dispdemo/spoly/spoly2 demo
REM
cd dispdemo
nmake clean
nmake dev=win32
copy *.exe ..\bin
cd ..\spoly
nmake clean
nmake dev=win32
copy *.exe ..\bin
copy *.reg ..\bin
cd ..\spoly2
nmake clean
nmake dev=win32
copy *.exe ..\bin
copy *.reg ..\bin
cd ..
