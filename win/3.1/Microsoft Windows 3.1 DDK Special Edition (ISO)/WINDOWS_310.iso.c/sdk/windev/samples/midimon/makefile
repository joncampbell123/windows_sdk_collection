##### Module Macro #####
APPNAME    = midimon
APPSRCS    = $(APPNAME).c about.c circbuf.c display.c filter.c instdata.c prefer.c
APPOBJS    =

DLLNAME   = callback
DLLSRCS   = $(DLLNAME).c
DLLOBJS   = libentry.obj

##### C7 Macro #####
C7	   = 1

##### Library Macro #####
APPLIBS    = libw slibcew callback mmsystem
APPMOD	   = -AS

DLLLIBS    = libw sdllcew mmsystem
DLLMOD	   = -ASw

##### Include Macro #####
APPINCLS   = $(APPNAME).h about.h circbuf.h display.h filter.h instdata.h prefer.h
DLLINCLS2  = $(APPNAME).h $(DLLNAME).h instdata.h circbuf.h

##### Resource Macro #####
APPRCFILES = $(APPNAME).rc $(APPNAME).ico
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
DEF	= -DSTRICT

##### Tool Macros #####
ASM	= masm -Mx $(MOPT) $(DDEF) $(DEF)
CC	= cl -nologo -c $(APPMOD) -G2sw -Zp -W3 $(CLOPT) $(DDEF) $(DEF)
CC2	= cl -nologo -c $(DLLMOD) -G2sw -Zp -W3 $(CLOPT) $(DDEF) $(DEF)
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
$(APPNAME).exe: $(APPSRCS:.c=.obj) $(APPNAME).def $(APPNAME).res $(DLLNAME).lib
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

$(DLLNAME).obj: $(DLLSRCS) $(DLLINCLS)
    $(CC2) $*.c

$(DLLNAME).dll: $(DLLSRCS:.c=.obj) $(DLLNAME).def
    $(LINK) @<<
    $(DLLSRCS:.c=.obj) $(DLLOBJS2),
    $(DLLNAME).dll,
    $(DLLNAME).map,
    $(DLLLIBS),
    $(DLLNAME).def
<<
    $(RC) -T $(DLLNAME).dll $(DLLNAME).res
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
