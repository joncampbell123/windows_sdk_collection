##### Module Macro #####
APPNAME    = listhorz
APPSRCS    = $(APPNAME).c
APPOBJS    =

DLLNAME   = listhscr
DLLSRCS   = $(DLLNAME).c wep.c
DLLOBJS   = libentry.obj

##### C7 Macro #####
C7	   = 1

##### Library Macro #####
APPLIBS    = libw slibcew listhscr
APPMOD	   = -AS

DLLLIBS    = libw sdllcew
DLLMOD	   = -ASw

##### Include Macro #####
APPINCLS   = $(APPNAME).h listhapi.h
DLLINCLS   = $(DLLNAME).h

##### Resource Macro #####
APPRCFILES = $(APPNAME).ico $(APPNAME).rc
DLLRCFILES =

##### DEBUG Defined #####
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

##### Dependents For Goal and Command Line #####
$(APPNAME).obj: $(APPNAME).c $(APPINCLS)
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


$(DLLNAME).dll: $(DLLSRCS:.c=.obj) $(DLLNAME).def
    $(LINK) @<<
    $(DLLSRCS:.c=.obj) $(DLLOBJS),
    $(DLLNAME).dll,
    $(DLLNAME).map,
    $(DLLLIBS),
    $(DLLNAME).def
<<
    $(RC) -T $(DLLNAME).dll
!if $(DEBUG)
!if !$(C7)
    cvpack -p $(DLLNAME).dll
!endif
    mapsym $(DLLNAME).map
!endif

$(DLLNAME).lib: $(DLLNAME).dll
    implib $(DLLNAME).lib $(DLLNAME).def

##### Dependents For Inference Rules #####
$(APPSRCS:.c=.obj): $(APPINCLS)
$(APPNAME).res: $(APPRCFILES) $(APPINCLS)

$(DLLSRCS:.c=.obj): $(DLLINCLS)
$(DLLNAME).res: $(DLLINCLS) $(DLLRCFILES)

##### Clean Directory #####
clean:
    -del *.obj
    -del *.res
    -del *.exe
    -del *.map
    -del *.sym
    -del *.dll
    -del *.lib
