# Master make file.  Creates all necessary sub-components
# of the Immortal Klowns project, while allowing those components
# to be compiled w/VC

!IF ("$(CFG)" == "") || ("$(CFG)" == "Win32 Debug")
CFG=Win32 Debug
TARGDIR=debug
TARGET=debug
!ELSE
CFG=Win32 Release
TARGDIR=retail
TARGET=retail
!ENDIF

default:
    nmake $(MISC) /nologo /f linklist.mak  CFG="$(CFG)"
    nmake $(MISC) /nologo /f cgutil.mak  CFG="$(CFG)"

    nmake $(MISC) /nologo /f splash.mak CFG="$(CFG)"

    nmake $(MISC) /nologo /f iklowns.mak CFG="$(CFG)"

    nmake $(MISC) /nologo /f cgkrusty.mak CFG="$(CFG)"

    nmake $(MISC) /nologo /f misc.mak CFG="$(CFG)"

all: debug retail

retail:
    nmake CFG="Win32 Release"

debug:
    nmake CFG="Win32 Debug"

clean:
    @deltree /y retail
    @deltree /y debug
