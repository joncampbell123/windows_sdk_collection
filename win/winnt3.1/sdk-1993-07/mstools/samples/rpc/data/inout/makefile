#*************************************************************#
#**                                                         **#
#**                 Microsoft RPC Examples                  **#
#**                   inout Application                     **#
#**            Copyright(c) Microsoft Corp. 1992            **#
#**                                                         **#
#*************************************************************#

!include <ntwin32.mak>

.c.obj:
   $(cc) $(cdebug) $(cflags) $(cvars) $<

all : inoutc inouts

# Make the inout client
inoutc : inoutc.exe
inoutc.exe : inoutc.obj inout_c.obj inout_x.obj
    $(link) $(linkdebug) $(conflags) -out:inoutc.exe \
      inoutc.obj inout_c.obj inout_x.obj \
      rpcrt4.lib $(conlibs)

# inout client main program
inoutc.obj : inoutc.c inout.h

# inout client stub
inout_c.obj : inout_c.c inout.h

# inout client auxiliary file
inout_x.obj : inout_x.c inout.h

# Make the inout server
inouts : inouts.exe
inouts.exe : inouts.obj inoutp.obj inout_s.obj inout_y.obj
    $(link) $(linkdebug) $(conflags) -out:inouts.exe \
      inouts.obj inout_s.obj inoutp.obj inout_y.obj \
      rpcrt4.lib $(conlibs)

# inout server main loop
inouts.obj : inouts.c inout.h

# remote procedures
inoutp.obj : inoutp.c inout.h

# inout server stub file
inout_s.obj : inout_s.c inout.h

# inout server auxiliary file
inout_y.obj : inout_y.c inout.h

# Stubs, auxiliary and header file from the IDL file
inout.h inout_c.c inout_x.c inout_s.c inout_y.c : inout.idl inout.acf
    midl -cpp_cmd $(cc) -cpp_opt "-E" inout.idl

# Clean up everything
cleanall : clean
    -del *.exe

# Clean up everything but the .EXEs
clean :
    -del *.obj
    -del *.map
    -del inout_c.c
    -del inout_x.c
    -del inout_s.c
    -del inout_y.c
    -del inout.h
