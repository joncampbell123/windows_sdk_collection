# Nmake macros for building Windows 32-Bit apps

TARGETOS=WIN95
APPVER=4.0

!include <win32.mak>



#
# First make sure we're building on an i386 compat. platform.
#

!IF "$(CPU)" == "PPC" || "$(CPU)" == "MIPS" || "$(CPU)" == "ALPHA"
all:  WARN_MSG
!ELSE
all: icmtest.exe
!ENDIF

WARN_MSG:
   @echo Warning: ICMTest intended to execute on Windows 95 only.



# Update the resource if necessary

icmtest.res: icmtest.rc icmtest.h
    $(rc) $(rcvars) $(rcflags) icmtest.rc

# Update the object file if necessary

icmtest.obj: icmtest.c icmtest.h
    $(cc) $(cflags) $(cvars) $(cdebug) icmtest.c

print.obj: print.c icmtest.h
    $(cc) $(cflags) $(cvars) $(cdebug) print.c

dialogs.obj: dialogs.c icmtest.h
    $(cc) $(cflags) $(cvars) $(cdebug) dialogs.c

dib.obj: dib.c icmtest.h
    $(cc) $(cflags) $(cvars) $(cdebug) dib.c

# Update the executable file if necessary, and if so, add the resource back in.

icmtest.exe: icmtest.obj print.obj dib.obj dialogs.obj icmtest.res
    $(link) $(linkdebug) $(guiflags) -out:icmtest.exe icmtest.obj print.obj dib.obj dialogs.obj icmtest.res $(guilibs)
