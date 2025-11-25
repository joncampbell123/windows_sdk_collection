#*************************************************************#
#**                                                         **#
#**                 Microsoft RPC Examples                  **#
#**                   hello Application                     **#
#**            Copyright(c) Microsoft Corp. 1992            **#
#**                                                         **#
#*************************************************************#

!include <ntwin32.mak>

.c.obj:
   $(cc) $(cdebug) $(cflags) $(cvars) $<

all : helloc hellos

# Make the client side application helloc
helloc : helloc.exe
helloc.exe : helloc.obj hello_c.obj hello_x.obj
    $(link) $(linkdebug) $(conflags) -out:helloc.exe \
      helloc.obj hello_c.obj hello_x.obj \
      rpcrt4.lib $(conlibs)

# helloc main program
helloc.obj : helloc.c hello.h

# helloc stub
hello_c.obj : hello_c.c hello.h

# helloc auxiliary file
hello_x.obj : hello_x.c hello.h

# Make the server side application
hellos : hellos.exe
hellos.exe : hellos.obj hellop.obj hello_s.obj hello_y.obj
    $(link) $(linkdebug) $(conflags) -out:hellos.exe \
      hellos.obj hello_s.obj hellop.obj hello_y.obj \
      rpcrt4.lib $(conlibs)

# hello server main program
hellos.obj : hellos.c hello.h

# remote procedures
hellop.obj : hellop.c hello.h

# hellos stub file
hello_s.obj : hello_s.c hello.h

# hellos auxiliary file
hello_y.obj : hello_y.c hello.h

# Stubs, auxiliary and header file from the IDL file
hello.h hello_c.c hello_x.c hello_s.c hello_y.c : hello.idl hello.acf
    midl -cpp_cmd $(cc) -cpp_opt "-E"  hello.idl

# Clean up everything
cleanall : clean
    -del *.exe

# Clean up everything but the .EXEs
clean :
    -del *.obj
    -del *.map
    -del hello_c.c
    -del hello_x.c
    -del hello_s.c
    -del hello_y.c
    -del hello.h
