#*************************************************************#
#**                                                         **#
#**                 Microsoft RPC Examples                  **#
#**                   dunion Application                    **#
#**            Copyright(c) Microsoft Corp. 1992            **#
#**                                                         **#
#*************************************************************#

!include <ntwin32.mak>

.c.obj:
   $(cc) $(cdebug) $(cflags) $(cvars) $<

all : dunionc dunions

# Make the dunionc
dunionc : dunionc.exe
dunionc.exe : dunionc.obj dunion_c.obj dunion_x.obj
    $(link) $(linkdebug) $(conflags) -out:dunionc.exe \
      dunionc.obj dunion_c.obj dunion_x.obj \
      rpcrt4.lib $(conlibs)

# dunionc main program
dunionc.obj : dunionc.c dunion.h

# dunionc stub
dunion_c.obj : dunion_c.c dunion.h

# dunionc auxiliary file
dunion_x.obj : dunion_x.c dunion.h

# Make the dunions
dunions : dunions.exe
dunions.exe : dunions.obj dunionp.obj dunion_s.obj dunion_y.obj
    $(link) $(linkdebug) $(conflags) -out:dunions.exe \
      dunions.obj dunion_s.obj dunionp.obj dunion_y.obj \
      rpcrt4.lib $(conlibs)

# dunions main loop
dunions.obj : dunions.c dunion.h

# remote procedures
dunionp.obj  : dunionp.c dunion.h

# dunions stub file
dunion_s.obj : dunion_s.c dunion.h

# dunions auxiliary file
dunion_y.obj : dunion_y.c dunion.h

# Stubs, auxiliary and header file from the IDL file
dunion.h dunion_c.c dunion_x.c dunion_s.c dunion_y.c : dunion.idl dunion.acf
    midl -cpp_cmd $(cc) -cpp_opt "-E" dunion.idl

# Clean up everything
cleanall : clean
    -del *.exe

# Clean up everything but the .EXEs
clean :
    -del *.obj
    -del *.map
    -del dunion_c.c
    -del dunion_x.c
    -del dunion_s.c
    -del dunion_y.c
    -del dunion.h
