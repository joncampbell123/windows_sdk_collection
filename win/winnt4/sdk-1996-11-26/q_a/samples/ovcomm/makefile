# Nmake macros for building Win32 applications
!Include <NTWin32.Mak>

# Inference Rules
.C.Obj:
    $(cc) $(cdebug) $(cflags) $(cvars) $*.C
.Obj.Exe:
    $(link) $(ldebug) $(conlflags) -out:$*.Exe $** $(conlibs) User32.lib

# Macro Defines
PROJ=OvComm

# Target List
All: $(PROJ).Exe

# Source Objects
$(PROJ).obj: $(PROJ).c $(PROJ).H

# Executables
$(PROJ).exe: $(PROJ).obj

# Clean Up Everything
CleanAll: clean
    del *.Exe

# Clean Up Everything Except .Exe Files
Clean:
    del *.Obj
