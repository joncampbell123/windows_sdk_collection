all: mscuistf.dll


!if "$(DEBUG)" == "yes"
CC = cl -c -nologo -AMw -Gw  -Zpei -W3 -Od -DDEBUG
LNK = cuistfd.lnk
!else
CC = cl -c -nologo -AMw -Gsw2 -Ocegiltw -Zpe -W3
LNK = cuistf.lnk
!endif


dialogs.res: dialogs.rc dialogs.dlg bldver.h dialogs.h
	rc -r dialogs.rc

dlgprocs.obj: cui.h dlgprocs.c dialogs.h
	$(CC) dlgprocs.c


mscuistf.dll: mscuistf.def mscomstf.lib msuilstf.lib msshlstf.lib \
		dlgprocs.obj ..\iniupd\w32sgra.obj dialogs.res $(LNK)
	link @$(LNK)
	copy   mscuistf.dll mscuistf.nrc
	rc -30 dialogs.res  mscuistf.dll


clean:
	-del   mscuistf.dll
	-del   mscuistf.nrc
	-del   dlgprocs.obj
	-del   dialogs.res
