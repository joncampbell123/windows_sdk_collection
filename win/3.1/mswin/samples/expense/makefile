##### APP Macros #####
APPNAME    = expense
APPSRCS    = $(APPNAME).c
APPOBJS    =

##### DLL Macros #####
DLLNAME   = custdict
DLLSRCS   = $(DLLNAME).c
DLLOBJS   = libentry.obj

##### C7 Macro #####
C7	  = 1

##### APP Library Macros #####
APPLIBS    = libw mlibcew
APPMOD	   = -AM

##### DLL Library Macros #####
DLLLIBS    = libw mdllcew penwin
DLLMOD	   = -AMw

##### APP/DLL Include Macros #####
APPINCLS   = $(DLLNAME).h expres.h
DLLINCLS   = $(DLLNAME).h

##### APP/DLL Resource Macros #####
APPPRCFILES = $(APPNAME).rc
DLLRCFILES  =

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
    $(CC) $*.c

.asm.obj:
    $(ASM) $*.asm;

.rc.res:
    $(RC) -r $*.rc

##### Main (default) Target #####
goal:  $(DLLNAME).dll $(APPNAME).exe

##### Dependents For Goal and Command Lines #####
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
$(DLLNAME).obj: $(DLLNAME).c $(DLLINCLS)
    $(CC2) $*.c

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

##### Dependents #####
$(APPSRCS:.c=.obj): $(APPINCLS)
$(APPNAME).res: $(APPINCLS) $(APPRCFILES)

$(DLLSRCS:.c=.obj): $(DLLINCLS)

##### Clean Directory #####
clean:
    -del *.obj
    -del *.res
    -del *.exe
    -del *.map
    -del *.sym
