##### APP Macros #####
APPNAME    = instver
APPSRCS    = $(APPNAME).c
APPOBJS    = instexe.obj dos.obj

##### DLL Macros #####
DLLNAME   = instver
DLLSRCS   = $(DLLNAME).c
DLLOBJS   = instdll.obj dllentry.obj dos.obj

##### C7 #####
C7	  = 1

##### APP Library Macros #####
!if $(C7)
APPLIBS    =  slibce vers lzexps oldnames
!else
APPLIBS    =  slibce vers lzexps
!endif
APPMOD	   = -Foinstexe.obj -AS

##### DLL Library Macros #####
DLLLIBS    = libw sdllcew ver
DLLMOD	   = -Foinstdll.obj -ASw -DDLL

##### APP/DLL Include Macros #####
APPINCLS   =
DLLINCLS  =

##### APP/DLL Resource Macros #####
APPRCFILES =
DLLRCFILES =

##### DEBUG Version Built #####
DEBUG	= 1

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
DEF	=

##### Tool Macros #####
ASM	= masm -DmemS=1  $(MOPT) $(DDEF) $(DEF)
CC	= cl -nologo -c $(APPMOD)  -G2sw -Zp -W3 $(CLOPT) $(DDEF) $(DEF)
CC2	= cl -nologo -c $(DLLMOD) -G2sw -Zp -W3 $(CLOPT) $(DDEF) $(DEF)
LINK	= link /NOD /NOE $(LOPT)
RC	= rc $(DDEF) $(DEF)
HC	= hc

##### Inference Rules #####
.asm.obj:
    $(ASM) $*.asm;

.rc.res:
    $(RC) -r $*.rc

##### Main (default) Target #####
goal:  $(DLLNAME).dll $(APPNAME).exe

##### Dependents For Goal and Command Lines #####
##### APP Build Separately #####
instexe.obj: instver.c
    $(CC) $(APPSRCS)

$(APPNAME).exe: $(APPOBJS) $(APPNAME).def $(APPNAME).res
    $(LINK) @<<
    $(APPOBJS)/ST:9226 ,
    $(APPNAME).exe,
    $(APPNAME).map,
    $(APPLIBS);
<<
!if $(DEBUG)
!if !$(C7)
    cvpack -p $(APPNAME).exe
!endif
    mapsym $(APPNAME).map
!endif

##### DLL Built Separately #####
instdll.obj: instver.c
    $(CC2) $(DLLSRCS)

$(DLLNAME).dll: $(DLLOBJS) $(DLLNAME).def
    $(LINK) @<<
    $(DLLOBJS),
    $(DLLNAME).dll,
    $(DLLNAME).map,
    $(DLLLIBS),
    $(DLLNAME).def
<<
    implib $(DLLNAME).lib $(DLLNAME).def
    $(RC) -T $(DLLNAME).dll
!if $(DEBUG)
!if !$(C7)
    cvpack -p $(DLLNAME).dll
!endif
    mapsym $(DLLNAME).map
!endif

##### Dependents #####
$(APPOBJS): $(APPINCLS)
$(APPNAME).res: $(APPINCLS) $(APPRCFILES)

$(DLLOBJS): $(DLLINCLS)

##### Clean Directory #####
clean:
    -del instdll.obj
    -del instexe.obj
    -del *.exe
    -del *.map
    -del *.sym
