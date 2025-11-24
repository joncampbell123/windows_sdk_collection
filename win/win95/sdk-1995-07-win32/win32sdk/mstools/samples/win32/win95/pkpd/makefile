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
all: an_pkpd.exe
!ENDIF

WARN_MSG:
   @echo Warning: PKPD intended to execute on Windows 95 only.



# Update the resource if necessary

an_pkpd.res: an_pkpd.rc anres.h
    $(rc) $(rcflags) $(rcvars) an_pkpd.rc 

# Update the object file if necessary

an_pkpd.obj: an_pkpd.c 
    $(cc) $(cflags) $(cvars) $(cdebug) an_pkpd.c

# Update the executable file if necessary, and if so, add the resource back in.

an_pkpd.exe: an_pkpd.obj an_pkpd.res
    $(link) $(linkdebug) $(guilflags) -out:an_pkpd.exe an_pkpd.obj an_pkpd.res $(guilibs) pkpd32.lib
