#*************************************************************#
#**                                                         **#
#**                 Microsoft RPC Examples                  **#
#**                    yield Application                    **#
#**            Copyright(c) Microsoft Corp. 1991            **#
#**                                                         **#
#*************************************************************#

!include <ntwin32.mak>

.c.obj:
   $(cc) $(cdebug) $(cflags) $(cvars) $<

all: yields

# Make the yield server
yields : yields.exe
yields.exe : yields.obj yieldp.obj yield_s.obj yield_y.obj
    $(link) $(linkdebug) $(conflags) -out:yields.exe -map:yields.map \
      yields.obj yieldp.obj yield_s.obj yield_y.obj \
      rpcrt4.lib $(conlibs)

yields.obj : yields.c yield.h

yieldp.obj : yieldp.c yield.h

yield_s.obj : yield_s.c yield.h

yield_y.obj : yield_y.c yield.h

# Make the stubs source
yield.h yield_s.c yield_y.c : yield.idl yield.acf
    midl -cpp_cmd $(cc) -cpp_opt "-E" yield.idl

# Clean up everything
cleanall : clean
    -del *.exe

# Clean up everything but the .EXEs
clean :
    -del *.obj
    -del *.map
    -del yield_s.c
    -del yield_y.c
    -del yield.h
