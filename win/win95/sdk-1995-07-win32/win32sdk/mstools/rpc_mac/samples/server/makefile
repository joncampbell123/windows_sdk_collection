all : hellos

# CPU defaulting to i386
# Tools Dirs
!ifndef MSVC20
MSVC20=\msvc20
!endif

!ifndef CPU
CPU=i386
!endif

##
## Server
##

!include "$(MSVC20)\include\ntwin32.mak"

cflags = $(cflags) /I.
# Make the server side application
hellos : hellos.exe
hellos.exe : hellos.obj hellop.obj hello_s.obj
    $(link) $(linkdebug) $(conflags) -out:hellos.exe \
      hellos.obj hello_s.obj hellop.obj \
      rpcrt4.lib $(conlibsmt)

# hello server main program
hellos.obj : ..\hellos.c hello.h
   $(cc) $(cdebug) $(cflags) $(cvarsmt) ..\$*.c

# remote procedures
hellop.obj : ..\hellop.c hello.h
   $(cc) $(cdebug) $(cflags) $(cvarsmt) ..\$*.c

# hellos stub file
hello_s.obj : hello_s.c hello.h
   $(cc) $(cdebug) $(cflags) $(cvarsmt) $*.c

# Stubs and header file from the IDL file
hello.h hello_c.c hello_s.c : ..\hello.idl ..\hello.acf
    midl -oldnames -cpp_cmd $(cc) -cpp_opt "-E" ..\hello.idl


