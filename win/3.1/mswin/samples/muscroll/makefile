##### APP Macros #####
APPNAME    = mustest
APPSRCS    = $(APPNAME).c
APPOBJS    =

##### DLL Macros #####
DLLNAME   = muscroll
DLLSRCS   = $(DLLNAME).c wep.c init.c ctldlg.c paint.c msapi.c
DLLOBJS   = libentry.obj

##### C7 Macro #####
C7	   = 1

##### APP Library Macros #####
APPLIBS    = libw slibcew muscroll
APPMOD	   = -AS

##### DLL Library Macros #####
DLLLIBS    = libw sdllcew
DLLMOD	   = -ASw

##### APP/DLL Include Macros #####
APPINCLS   = $(APPNAME).h
DLLINCLS  = $(DLLNAME).h muscrdll.h

##### APP/DLL Resource Macros #####
APPRCFILES = $(APPNAME).ico $(APPNAME).rc
DLLRCFILES= ctlstyle.dlg  $(DLLNAME).rc

##### DEBUG Version Built #####
DEBUG	= 1

##### Build Option Macros #####
!if $(DEBUG)
DDEF	= -DDEBUG
CLOPT	= -Zid -Od
MOPT	= -Zi
LOPT	= /CO /LI /MAP
!else
DDEF	=
CLOPT	= -Os
LOPT	=
!endif

##### General Macros #####
DEF	=

##### Tool Macros #####
ASM	= masm -Mx $(MOPT) $(DDEF) $(DEF)
CC	= cl -nologo -c $(APPMOD) -G2sw -Zp -W3 $(CLOPT) $(DDEF) $(DEF)
CC2	= cl -nologo -c $(DLLMOD) -G2sw -Zp -W3 $(CLOPT) $(DDEF) $(DEF)
LINK	= link /NOD /NOE $(LOPT)
RC	= rc $(DDEF) $(DEF)
HC	= hc

##### Inference Rules #####
.c.obj:
    $(CC2) $*.c

.asm.obj:
    $(ASM) $*.asm;

.rc.res:
    $(RC) -r $*.rc

##### Main (default) Target #####
goal:  $(DLLNAME).lib $(APPNAME).exe

##### Dependents For Goal and Command Lines #####
##### APP Built Separately #####
mustest.obj: mustest.c
    $(CC) $*.c

$(APPNAME).exe: $(APPSRCS:.c=.obj) $(APPNAME).def $(APPNAME).res
    $(LINK) @<<
    $(APPSRCS:.c=.obj) $(APPOBJS),
    $(APPNAME).exe,
    $(APPNAME).map,
    $(APPLIBS),
    $(APPNAME).def
<<
    $(RC) -T $(APPNAME).res
!if $(DEBUG)
!if !$(C7)
    cvpack -p $(APPNAME).exe
!endif
    mapsym $(APPNAME).map
!endif

##### DLL Built Separately #####
$(DLLNAME).dll: $(DLLSRCS:.c=.obj) $(DLLNAME).def $(DLLNAME).res
    $(LINK) @<<
    $(DLLSRCS:.c=.obj) $(DLLOBJS),
    $(DLLNAME).dll,
    $(DLLNAME).map,
    $(DLLLIBS),
    $(DLLNAME).def
<<
    $(RC) -T $(DLLNAME).res $(DLLNAME).dll
!if $(DEBUG)
!if !$(C7)
    cvpack -p $(DLLNAME).dll
!endif
    mapsym $(DLLNAME).map
!endif

$(DLLNAME).lib: $(DLLNAME).dll
    implib $(DLLNAME).lib $(DLLNAME).def

##### Dependents #####
$(APPSRCS:.c=.obj): $(APPINCLS)
$(APPNAME).res: $(APPINCLS) $(APPRCFILES)

$(DLLSRCS:.c=.obj): $(DLLINCLS)
$(DLLNAME).res:  $(DLLINCLS) $(DLLRCFILES)

##### Clean Directory #####
clean:
    -del *.obj
    -del *.res
    -del *.exe
    -del *.map
    -del *.sym
    -del *.lib
