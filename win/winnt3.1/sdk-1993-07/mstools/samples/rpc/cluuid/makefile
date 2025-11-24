#*************************************************************#
#**                                                         **#
#**                 Microsoft RPC Examples                  **#
#**                   cluuid Application                            **#
#**            Copyright(c) Microsoft Corp. 1992            **#
#**                                                         **#
#*************************************************************#

!include <ntwin32.mak>

.c.obj:
   $(cc) $(cdebug) $(cflags) $(cvars) $<

all : cluuidc cluuids

# Make the client side application cluuidc
cluuidc : cluuidc.exe
cluuidc.exe : cluuidc.obj cluuid_c.obj cluuid_x.obj
    $(link) $(linkdebug) $(conflags) -out:cluuidc.exe \
      cluuidc.obj cluuid_c.obj cluuid_x.obj \
      rpcrt4.lib $(conlibs)

# cluuidc main program
cluuidc.obj : cluuidc.c cluuid.h

# cluuidc stub
cluuid_c.obj : cluuid_c.c cluuid.h

# cluuidc auxiliary file
cluuid_x.obj : cluuid_x.c cluuid.h

# Make the server side application
cluuids : cluuids.exe
cluuids.exe : cluuids.obj cluuidp.obj cluuid_s.obj cluuid_y.obj
    $(link) $(linkdebug) $(conflags) -out:cluuids.exe \
      cluuids.obj cluuid_s.obj cluuidp.obj cluuid_y.obj \
      rpcrt4.lib $(conlibs)

# cluuid server main program
cluuids.obj : cluuids.c cluuid.h

# remote procedures
cluuidp.obj : cluuidp.c cluuid.h

# cluuids stub file
cluuid_s.obj : cluuid_s.c cluuid.h

# cluuids auxiliary file
cluuid_y.obj : cluuid_y.c cluuid.h

# Stubs, auxiliary and header file from the IDL file
cluuid.h cluuid_c.c cluuid_x.c cluuid_s.c cluuid_y.c : cluuid.idl cluuid.acf
    midl -cpp_cmd $(cc) -cpp_opt "-E" cluuid.idl

# Clean up everything
cleanall : clean
    -del *.exe

# Clean up everything but the .EXEs
clean :
    -del *.obj
    -del *.map
    -del cluuid_c.c
    -del cluuid_x.c
    -del cluuid_s.c
    -del cluuid_y.c
    -del cluuid.h
