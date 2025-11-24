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
