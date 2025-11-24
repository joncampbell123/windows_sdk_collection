#*************************************************************#
#**                                                         **#
#**                 Microsoft RPC Examples                  **#
#**                   auto Application                      **#
#**            Copyright(c) Microsoft Corp. 1992            **#
#**                                                         **#
#*************************************************************#

!include <ntwin32.mak>

.c.obj:
   $(cc) $(cdebug) $(cflags) $(cvars) $<

all : autoc autos

# Make the autoc
autoc : autoc.exe
autoc.exe : autoc.obj auto_c.obj auto_x.obj
    $(link) $(linkdebug) $(conflags) -out:autoc.exe \
      autoc.obj auto_c.obj auto_x.obj \
      rpcrt4.lib rpcns4.lib $(conlibs)

# autoc main program
autoc.obj : autoc.c auto.h

# autoc stub
auto_c.obj : auto_c.c auto.h

# autoc auxiliary file
auto_x.obj : auto_x.c auto.h

# Make the autos
autos : autos.exe
autos.exe : autos.obj autop.obj auto_s.obj auto_y.obj
    $(link) $(linkdebug) $(conflags) -out:autos.exe \
      autos.obj auto_s.obj autop.obj auto_y.obj \
      rpcrt4.lib rpcns4.lib $(conlibs)

# autos main loop
autos.obj : autos.c auto.h

# remote procedures
autop.obj  : autop.c auto.h

# autos stub file
auto_s.obj : auto_s.c auto.h

# autos auxiliary file
auto_y.obj : auto_y.c auto.h

# Stubs, auxiliary and header file from the IDL file
auto.h auto_c.c auto_x.c auto_s.c auto_y.c : auto.idl auto.acf
    midl -cpp_cmd $(cc) -cpp_opt "-E" auto.idl

# Clean up everything
cleanall : clean
    -del *.exe

# Clean up everything but the .EXEs
clean :
    -del *.obj
    -del *.map
    -del auto_c.c
    -del auto_x.c
    -del auto_s.c
    -del auto_y.c
    -del auto.h
