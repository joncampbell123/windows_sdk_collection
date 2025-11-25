!include <ntwin32.mak>

all: mscuistf.dll

# NOTE: In order to build MSCUISTF.DLL, you must ensure that your 
# LIB environment variable points to the libraries provided with 
# the Win32 SDK (C:\MSTOOLS\LIB) first.

!if "$(DEBUG)" == "yes"
cflags = $(cflags) $(cdebug) $(cvarsdll) -nologo -DDEBUG
lflags = $(ldebug)
!else
cflags = $(cflags) $(cvarsdll) -nologo
lflags =
!endif


dialogs.rbj: dialogs.rc dialogs.dlg bldver.h
	$(rc) $(rcvars) -r -fo dialogs.res dialogs.rc
	cvtres -$(CPU) dialogs.res -o dialogs.rbj

dlgprocs.obj: cui.h dlgprocs.c
	$(cc) $(cflags) dlgprocs.c


mscuistf.exp: mscuistf.def
	$(implib) -machine:$(CPU)	\
	-def:mscuistf.def		\
	-out:mscuistf.lib		\
	dlgprocs.obj

mscuistf.dll: ..\lib\mscomstf.lib ..\lib\msuilstf.lib ..\lib\msshlstf.lib \
		dlgprocs.obj dialogs.rbj mscuistf.exp
	$(link) $(lflags)		\
	-dll				\
	-entry:_CRT_INIT$(DLLENTRY)	\
	-out:mscuistf.dll		\
	dlgprocs.obj dialogs.rbj mscuistf.exp mscomstf.lib \
	msuilstf.lib msshlstf.lib $(crtdll) $(guilibsdll)


clean:
	del   mscuistf.dll
	del   dlgprocs.obj
	del   dialogs.res
