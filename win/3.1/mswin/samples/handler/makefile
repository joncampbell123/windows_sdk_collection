##### Module Macro #####
APPNAME    = handtest
APPSRCS    = $(APPNAME).c
APPOBJS    =

DLLNAME   = handler
DLLSRCS   = $(DLLNAME).asm
DLLOBJS   = libentry.obj

##### C7 #####
C7	   = 1

##### Library Macro #####
APPLIBS    = libw slibcew handler
APPMOD	   = -AS

DLLLIBS    = libw mdllcew

##### Include Macro #####
APPINCLS   = $(APPNAME).h
DLLINCLS   = $(DLLNAME).inc

##### Resource Macro #####
APPRCFILES =
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
LINK	= link /NOD /NOE $(LOPT)
RC	= rc $(DDEF) $(DEF)
HC	= hc

##### Inference Rules #####
.c.obj:
    $(CC) $*.c

.asm.obj:
    $(ASM) $*.asm;

.rc.res:
    $(RC) -r $*.rc

##### Main (default) Target #####
goal:  $(DLLNAME).lib $(APPNAME).exe

##### Dependents For Goal and Command Line #####
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

$(DLLNAME).dll: $(DLLSRCS:.asm=.obj) $(DLLNAME).def
    $(LINK) @<<
    $(DLLSRCS:.asm=.obj) $(DLLOBJS),
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


##### Dependents #####
$(APPSRCS:.c=.obj): $(DLLINCLS)
$(DLLSRCS:.asm=.obj): $(DLLINCLS)
$(APPNAME).res: $(APPRCFILES) $(APPINCLS)

##### Clean Directory #####
clean:
    -del $(APPNAME).obj
    -del *.res
    -del *.exe
    -del *.dll
    -del *.map
    -del *.sym
    -del *.lib
