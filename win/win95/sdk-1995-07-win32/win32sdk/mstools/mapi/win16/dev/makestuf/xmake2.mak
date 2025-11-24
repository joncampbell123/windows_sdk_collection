# -----------------------------------------------------------------------------
# $(MAPITEST)\samples\makestuf\xmake2.mak
#
# Copyright (C) 1995 Microsoft Corporation
# -----------------------------------------------------------------------------

# -----------------------------------------------------------------------------
#
#   Part 4: build executable targets
#

.SUFFIXES: .asm .bas .c .cpp .cxx .des .rc .pch

# TARGET ----------------------------------------------------------------------

TARGET      = $(BASENAME)

# PCHDIR

!IF "$(PCHDIR)" == ""
PCHDIR      = $H
!ENDIF

# Resources -------------------------------------------------------------------

!IF "$(RCSOURCE)" != ""
!IF $(WIN32) && "$(MSVCNT)" == "0"
RCTARGET    = rc.rbj
!ELSE
RCTARGET    = rc.res
!ENDIF
!ENDIF

# Other Includes and Libraries ------------------------------------------------

LIBS        = $(OTHERLIBS) $(LIBS)
INCDIRS     = $(OTHERINCS) -I$H -I$(MAPIH) -I$(MAPIINC) -I$(MAPIEXTINC) $(INCDIRS)

# Entry point -----------------------------------------------------------------

!IF $(WIN32) && "$(ENTRYPOINT)" != ""
ENTRYPOINT  = -entry:$(ENTRYPOINT)
!ELSE
!IF $(WIN32) && $(MSVCNT)
!ENDIF
!ENDIF

# C Precompiled Header --------------------------------------------------------

!IF "$(PCH)" != ""
OBJS        = $(PCH).obj $(OBJS)
$(PCH).obj: $(PCH).pch
!IF $(DOECHO)
    echo ignoring precompiled header "C" file $(PCH).c
!ELSE
    rem
!ENDIF

$(PCH).pch: $(PCHDIR)\$(PCH).c $(PCHDEPS)
!IF $(DOECHO)
    echo ++++++++++++++++++++++++++++++++++++++++
    echo BUILDING Pre-compiled header $(PCH).pch
!ENDIF
    SET CL=$(CFLAGS) $(DEFS) $(INCDIRS)
    $(CC) -I$(PCHDIR) -Yc -Fp$(PCH).pch -Fo$(PCH).obj $(PCHDIR)\$(PCH).c

USEPCH      = -Yu$(PCH).h
!ENDIF

# .C to .OBJ ------------------------------------------------------------------

{$H}.c.obj:
!IF $(DOECHO)
    echo +++++++++
    echo COMPILING $*.c
!ENDIF
!IF $(DOECHO) && "$(USEPCH)" != ""
    echo USING Precompiled Header $(PCH).h
!ENDIF
    SET CL=$(CFLAGS) $(DEFS) $(INCDIRS)
    $(CC) $(USEPCH) $H\$*.c

# C++ Precompiled Header ------------------------------------------------------

!IF "$(CPPPCH)" != ""
OBJS    = $(CPPPCH).obj $(OBJS)
$(CPPPCH).OBJ: $(CPPPCH).pch
!IF $(DOECHO)
    echo ignoring precompiled header "C++" file $(CPPPCH).cpp
!ELSE
    rem
!ENDIF

$(CPPPCH).pch: $(PCHDIR)\$(CPPPCH).cpp $(CPPPCHDEPS)
!IF $(DOECHO)
    echo ++++++++++++++++++++++++++++++++++++++++
    echo BUILDING Pre-compiled header $(CPPPCH).pch
!ENDIF
    SET CL=$(CFLAGS) $(DEFS) $(INCDIRS)
    $(CC) -I$(PCHDIR) -Yc -Fp$(CPPPCH).pch -Fo$(CPPPCH).obj $(PCHDIR)\$(CPPPCH).cpp

USECPPPCH	= -Yu$(CPPPCH).h
!ENDIF

# .CPP to .OBJ ----------------------------------------------------------------

{$H}.cpp.obj:
!IF $(DOECHO)
    echo +++++++++
    echo COMPILING $*.cpp
!ENDIF
!IF $(DOECHO) && "$(USECPPPCH)" != ""
    echo USING Precompiled Header $(CPPPCH).h
!ENDIF
    SET CL=$(CFLAGS) $(DEFS) $(INCDIRS)
    $(CC) $(USECPPPCH) $H\$*.cpp

# .ASM to .OBJ ----------------------------------------------------------------

{$H}.asm.obj:
!IF $(DOECHO)
   echo +++++++++
   echo ASSEMBLING $*.asm
!ENDIF
    $(AS) $(AFLAGS) $(DEFS) $H\$*.asm;

# .RC to .RES/.RBJ ------------------------------------------------------------

!IF "$(RCTARGET)" != ""
$(RCTARGET): $(RCSOURCE) $(RCDEPS)
!IF $(DOECHO)
    echo +++++++++
    echo BUILDING $(VERSION) $@.res
!ENDIF
    @SET INCLUDE=$H;$H\..;$(MAPIINC);$(MAPIRESRC);$(INCLUDE)
!IF $(WIN16)
    $(RC) -r $(DEFS) $(OTHERINCS) -forc.res $(RCSOURCE)
!ELSE
    $(RC) -r $(DEFS) $(OTHERINCS) -forc.res $(RCSOURCE)
!ENDIF
!IF $(WIN32) && $(MSVCNT) == 0
    cvtres -$(CPU) rc.res -o $(RCTARGET)
!ENDIF
!ENDIF


# Build a subsystem target ----------------------------------------------------

!IF $(WIN16)
subsys: $(OBJS)
!IF "$(OBJS)" != ""
    type >>allobjs.txt <<
$(OBJS) +
<<
!ENDIF
!ENDIF

!IF $(WIN32)
subsys: $(OBJS)
!IF "$(OBJS)" != ""
    type >>allobjs.txt <<
$(OBJS)
<<
!ENDIF
!ENDIF


# Build other subsystems target -----------------------------------------------

allsubs: $(OTHERSUBS)
!IF $(DOECHO)
    echo ++++++++++++++++++++++++++++++++++++++++
    echo BUILDING INCLUDED SUBSYSTEMS
!ENDIF
    type nul > allobjs.txt
    type nul > link.txt
    !$(MAKE) -nologo $(MAKEDEFS) -f $** subsys

# Building Executable ---------------------------------------------------------

!IF $(PRODEXE)

goal: $(TARGET).exe

$(TARGET).exe: allsubs $(RCTARGET)
!IF $(WIN16)
    type allobjs.txt > link.txt
    type >> link.txt <<

$(TARGET).exe
$(TARGET).map
$(LIBS: = +^
)
$H\$(TARGET)$(WINTAG).def
<<
    $(LINK) $(LFLAGS) @link.txt
!IF "$(RCTARGET)" != ""
    $(RC) -nologo -31 -K $(RCTARGET) $@
!ENDIF
!ENDIF
!IF $(WIN32)
    type >> link.txt <<
$(LFLAGS) 
$(ENTRYPOINT) 
-map:$(TARGET).map 
-out:$@
$(RCTARGET) 
$(LIBS)
<<
    $(LINK) @link.txt @allobjs.txt 
!ENDIF
!IF "$(DEVTREE)" != "0"
    -copy $(TARGET).exe $(MAPI)\lib\$(WINTAG)\$(VERTAG)
!ENDIF

!ENDIF

# Building DLL ----------------------------------------------------------------

!IF $(PRODDLL)

goal: $(TARGET).dll

lib: $(TARGET).lib

!IF $(WIN16)
$(TARGET).lib: $H\dll$(WINTAG).def
!ELSE
$(TARGET).lib: $H\dll.def
!ENDIF
!IF $(WIN16)
    $(IMPLIB) -nologo $(TARGET).lib $H\dll$(WINTAG).def
!ELSE
    $(LIBR) -machine:$(CPU) -def:$H\dll.def -out:$@
!ENDIF
!IF "$(DEVTREE)" != "0"
    -copy $(TARGET).lib $(MAPI)\lib\$(WINTAG)
!ENDIF

$(TARGET).dll: allsubs $(TARGET).lib $(RCTARGET)
!IF $(WIN16)
    type allobjs.txt > link.txt
    type >> link.txt <<

$(TARGET).dll
$(TARGET).map
$(LIBS: = +^
)
$H\dll$(WINTAG).def
<<
    $(LINK) $(LFLAGS) @link.txt
!IF "$(RCTARGET)" != ""
    $(RC) -nologo -31 -K $(RCTARGET) $(@F)
!ENDIF
!ENDIF
!IF $(WIN32)
    type >> link.txt <<
$(LFLAGS)
$(ENTRYPOINT)
-dll
-out:$@
-map:$(TARGET).map
$(RCTARGET)
$(TARGET).exp
$(LIBS)
<<
    $(LINK) @link.txt @allobjs.txt
!ENDIF
!IF "$(DEVTREE)" != "0"
    -copy $(TARGET).lib $(MAPI)\lib\$(WINTAG)
    -copy $(TARGET).dll $(MAPIDLL)
    
!ENDIF

!ENDIF

# Building Library ------------------------------------------------------------

!IF $(PRODLIB)

goal: $(TARGET).lib

lib: $(TARGET).lib

$(TARGET).lib: allsubs
!IF $(WIN16)
    type > lib.txt <<
$(OBJS: = &^
)
<<
    @if exist $(TARGET).lib del $(TARGET).lib
    $(LIBR) $(TARGET).lib @lib.txt
!ENDIF
!IF $(WIN32)
    $(LIBR) -out:$(TARGET).lib @allobjs.txt
!ENDIF
!IF "$(DEVTREE)" != "0"
    -copy $(TARGET).lib $(MAPIDLL)
!ENDIF

!ENDIF

# Building Clean --------------------------------------------------------------

clean:
!IF $(DOECHO)
    echo ++++++++++++++++++++++++++++++++++++++++
    echo MAKING CLEAN
!ENDIF
!IF "$(OS)" == "Windows_NT" || "$(FASTCLEAN)" != ""
    -del *.obj *.rbj *.exe *.dll *.exp *.lib *.pch *.pdb *.map *.sym *.txt *.res *.out *.vcw *.wsp $(OTHERCLEAN)
!ELSE
    -del *.obj 
    -del *.rbj 
    -del *.exe 
    -del *.dll 
    -del *.exp 
    -del *.lib 
    -del *.pch 
    -del *.pdb 
    -del *.map 
    -del *.sym 
    -del *.txt 
    -del *.res 
    -del *.out 
    -del *.vcw 
    -del *.wsp
!IF "$(OTHERCLEAN)" != ""
    -del $(OTHERCLEAN)
!ENDIF
!ENDIF
