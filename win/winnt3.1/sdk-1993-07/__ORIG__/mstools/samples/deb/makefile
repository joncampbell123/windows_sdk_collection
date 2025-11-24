# -------------------------------------------------------------------------
#
#                       Microsoft Developer Support
#              Copyright (c) 1992, 1993 Microsoft Corporation
#
# -------------------------------------------------------------------------
# MODULE  : MakeFile
# PURPOSE : Nmake macros for building the Windows 32-Bit Debug Event
#           Browser Application
# -------------------------------------------------------------------------

# Nmake macros for building Windows 32-Bit apps
!include <NtWin32.Mak>

# macro defines
OBJS=DEBMain.Obj DEBDebug.Obj DEBMisc.Obj LinkList.Obj

# target list
all: DEB.Exe ToolBar.Dll DEB.Hlp

# Update the onlink help file if necessary.
DEB.Hlp: DEB.Hpj DEB.Rtf
    @if exist DEB.Ph del DEB.Ph
    $(hc) -n DEB.Hpj

# Update the object files if necessary
DEBMain.Obj: DEBMain.C DEBMain.H DEBDebug.H DEBMisc.H LinkList.H DEB.H
    $(cc) $(cdebug) $(cflags) $(cvarsdll) DEBMain.C

DEBDebug.Obj: DEBDebug.C DEBDebug.H DEBMisc.H LinkList.H DEB.H
    $(cc) $(cdebug) $(cflags) $(cvarsdll) DEBDebug.C

DEBMisc.Obj: DEBMisc.C DEBMisc.H LinkList.H ToolBar.H DEB.H
    $(cc) $(cdebug) $(cflags) $(cvarsdll) DEBMisc.C

LinkList.Obj: LinkList.C LinkList.H
    $(cc) $(cdebug) $(cflags) $(cvarsdll) LinkList.C

ToolBar.Obj: ToolBar.C ToolBar.H
    $(cc) $(cdebug) $(cflags) $(cvarsdll) ToolBar.C

# Update the resource if necessary
DEB.Res: DEB.Rc DEB.Dlg DEB.H
    $(rc) $(rcvars) -r -fo DEB.Res DEB.Rc
    $(cvtres) -$(CPU) DEB.Res -o DEB.Rbj

# Update the import library
ToolBar.Lib: ToolBar.Obj ToolBar.Def
    $(implib)\
    -machine:$(CPU)\
    -def:ToolBar.Def\
    -out:ToolBar.Lib

# Update the dynamic link library
ToolBar.Dll: ToolBar.Obj ToolBar.Lib
    $(link) $(ldebug) $(guilflags)\
    -base:0x1C000000\
    -dll\
    -entry:_DllMainCRTStartup$(DLLENTRY)\
    -out:ToolBar.Dll\
    ToolBar.Obj ToolBar.Exp\
    $(guilibsdll)

# Update the Executable file if necessary.
DEB.Exe: $(OBJS) DEB.Res DEB.Def ToolBar.Lib
    $(link) $(ldebug) $(guilflags)\
    -out:DEB.Exe\
    $(OBJS) ToolBar.Lib DEB.Rbj\
    $(guilibsdll)
