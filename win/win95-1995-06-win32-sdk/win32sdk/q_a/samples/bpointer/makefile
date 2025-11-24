# Nmake macros for building Win32 applications
!Include <NTWin32.Mak>

# Inference Rules
.C.Obj:
    $(cc) $(cdebug) $(cflags) $(cvarsmt) $*.C
.Obj.Exe:
    $(link) $(ldebug) $(conlflags) -out:$*.Exe $** $(conlibsmt) User32.Lib

# Macro Defines
PROJ=ReadData
PROJ2=ChgData

# Target List
All: $(PROJ).Exe $(PROJ2).Exe

# Source Objects
$(PROJ).obj: $(PROJ).c shrstuff.h
$(PROJ2).obj: $(PROJ2).c shrstuff.h

# Executables
$(PROJ).exe: $(PROJ).obj
$(PROJ2).exe: $(PROJ2).obj

# Clean Up Everything
CleanAll: clean
    del *.Exe

# Clean Up Everything Except .Exe Files
Clean:
    del *.Obj
