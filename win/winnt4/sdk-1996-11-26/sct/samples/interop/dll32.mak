#
# dll32.mak
#


# Nmake macros for building Windows 32-Bit applications

!include <ntwin32.mak>

proj1 = dll32
proj2 = utdll32
proj3 = stub32
proj4 = disp32

all: $(proj1).dll $(proj2).dll $(proj3).exe $(proj4).exe

# Update the object files if necessary

$(proj1).obj: $(proj1).c
    $(cc) $(cdebug) $(cflags) $(cvars) $(proj1).c

$(proj2).obj: $(proj2).c
    $(cc) $(cdebug) $(cflags) $(cvars) $(proj2).c

$(proj3).obj: $(proj3).c
    $(cc) $(cdebug) $(cflags) $(cvars) $(proj3).c

$(proj4).obj: $(proj4).c
    $(cc) $(cdebug) $(cflags) $(cvars) $(proj4).c

# Update the import library

$(proj1).lib $(proj1).exp : $(proj1).def
   $(implib) -machine:$(CPU)     \
             -def:$(proj1).def        \
             -out:$(proj1).lib \
             $(proj1).obj

$(proj2).lib $(proj2).exp : $(proj2).def
   $(implib) -machine:$(CPU)     \
             -def:$(proj2).def        \
             -out:$(proj2).lib \
             $(proj2).obj

# Update the dynamic link library

$(proj1).dll: $(proj1).obj $(proj1).exp
    $(link) $(linkdebug)     \
            -base:0x1C000000  \
            -dll              \
            -entry:_DllMainCRTStartup$(DLLENTRY)    \
            -out:$(proj1).dll   \
            $(proj1).exp $(proj1).obj $(guilibs)

$(proj2).dll: $(proj2).obj $(proj2).exp $(proj1).lib
    $(link) $(linkdebug)     \
            -base:0x1C000000  \
            -dll              \
            -entry:_DllMainCRTStartup$(DLLENTRY)    \
            -out:$(proj2).dll   \
            $(proj2).exp $(proj2).obj $(guilibs) $(proj1).lib w32sut32.lib

$(proj3).exe: $(proj3).obj $(proj2).lib
    $(link) $(ldebug) $(guilflags) \
    $(proj3).obj $(guilibs) $(proj2).lib -out:$(proj3).exe

$(proj4).exe: $(proj4).obj $(proj1).lib
    $(link) $(ldebug) $(guilflags) \
    $(proj4).obj $(guilibs) $(proj1).lib -out:$(proj4).exe
