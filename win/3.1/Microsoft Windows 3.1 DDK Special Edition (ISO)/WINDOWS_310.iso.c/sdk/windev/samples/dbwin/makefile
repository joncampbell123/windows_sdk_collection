##### APP Macros #####
APPNAME    = dbwin
APPSRCS    = $(APPNAME).c setdb.c allocbrk.c
APPOBJS    =

##### DLL Macros #####
DLLNAME   = dbwindll
DLLSRCS   = $(DLLNAME).c monoout.c comout.obj nfyfmt.obj
DLLOBJS   = libentry.obj

##### C7 Macro #####
C7	  =   1

##### APP Library Macros #####
APPLIBS    =  libw slibcew commdlg toolhelp $(APPNAME)
APPMOD	   =  -AS

##### DLL Library Macros #####
DLLLIBS    = libw sdllcew commdlg toolhelp
DLLMOD	   =  -ASw

##### APP/DLL Include Macros #####
APPINCLS   = $(APPNAME).h dlgdefs.h
DLLINCLS   =

##### APP/DLL Resource Macros #####
APPRCFILES = $(APPNAME).rc $(APPNAME).ver $(APPNAME).ico
DLLRCFILES = $(DLLNAME).rc $(DLLNAME).ver

##### DEBUG Version Built #####
DEBUG	=  1

##### Build Option Macros #####
!if $(DEBUG)
DDEF	= -DDEBUG
CLOPT	= -Zid -Od
MOPT	= -Zi
LOPT	=  /CO /LI /MAP
!else
DDEF	=
CLOPT	= -Os
LOPT	=
!endif

##### General Macros #####
DEF	= -DSTRICT

##### Tool Macros #####
ASM	= masm -p $(MOPT) $(DDEF) $(DEF)
CC	= cl -nologo -c $(APPMOD) -Gc -G2sw -Zp -W3 $(CLOPT) $(DDEF) $(DEF)
CC2	= cl -nologo -c $(DLLMOD) -Gc -G2sw -Zp -W3 $(CLOPT) $(DDEF) $(DEF)
LINK	= link /NOD /NOE $(LOPT)
RC	= rc
HC	= hc

##### Inference Rules #####
.c.obj:
    $(CC2) $*.c

.asm.obj:
    $(ASM) $*.asm;

.rc.res:
    $(RC) -r $*.rc

##### Main (default) Target #####
goal:  $(APPNAME).dll $(APPNAME).exe

##### Dependents For Goal and Command Lines #####
##### APP Build Separately #####
$(APPNAME).obj:$(APPNAME).c
    $(CC) $*.c

setdb.obj:setdb.c
    $(CC) $*.c

allocbrk.obj:allocbrk.c
    $(CC) $*.c

$(APPNAME).exe: $(APPNAME).def $(APPNAME).res $(APPSRCS:.c=.obj) $(APPOBJS) $(APPNAME).dll
    $(LINK) @<<
    $(APPSRCS:.c=.obj) $(APPOBJS)
    $(APPNAME).exe,
    $(APPNAME).map,
    $(APPLIBS)
    $(APPNAME).def;
<<
    $(RC) -T $(APPNAME).rc
!if $(DEBUG)
!if !$(C7)
    cvpack -p $(APPNAME).exe
!endif
    mapsym $(APPNAME).map
!endif

##### DLL Built Separately #####
$(APPNAME).dll: $(DLLSRCS:.c=.obj) $(DLLNAME).res $(DLLNAME).def $(DLLOBJS)
    $(LINK) @<<
    $(DLLOBJS) $(DLLSRCS:.c=.obj),
    $(APPNAME).dll,
    $(DLLNAME).map,
    $(DLLLIBS),
    $(DLLNAME).def
<<
    $(RC) -T $(DLLNAME).res $(APPNAME).dll
    implib $(APPNAME).lib $(APPNAME).dll
!if $(DEBUG)
!if !$(C7)
   cvpack -p $(APPNAME).dll
!endif
   mapsym $(DLLNAME).map
!endif

##### Dependents #####
$(APPSRCS:.c=.obj): $(APPINCLS)
$(APPNAME).res: $(APPINCLS) $(APPRCFILES)

$(DLLOBJS): $(DLLINCLS)
$(DLLNAME).res: $(DLLINCLS) $(DLLRCFILES)

##### Clean Directory #####
clean:
    -del *.obj
    -del *.res
    -del *.map
    -del *.sym
    -del $(APPNAME).exe
    -del $(APPNAME).dll
    -del $(APPNAME).lib
