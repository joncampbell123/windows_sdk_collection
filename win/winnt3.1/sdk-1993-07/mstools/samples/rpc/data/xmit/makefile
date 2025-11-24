#*************************************************************#
#**                                                         **#
#**                 Microsoft RPC Examples                  **#
#**                   xmit Application                      **#
#**            Copyright(c) Microsoft Corp. 1992            **#
#**                                                         **#
#*************************************************************#

!include <ntwin32.mak>

.c.obj:
   $(cc) $(cdebug) $(cflags) $(cvars) $<

all : xmitc xmits

# Make the client
xmitc : xmitc.exe
xmitc.exe : xmitc.obj xmitu.obj xmit_c.obj xmit_x.obj
    $(link) $(linkdebug) $(conflags) -out:xmitc.exe -map:xmitc.map \
      xmitc.obj xmitu.obj xmit_c.obj xmit_x.obj \
      rpcrt4.lib $(conlibs)

# xmit client main program
xmitc.obj : xmitc.c xmit.h xmitu.h

# xmit client stub
xmit_c.obj : xmit_c.c xmit.h

# xmit client auxiliary file
xmit_x.obj : xmit_x.c xmit.h

# Make the server executable
xmits : xmits.exe
xmits.exe : xmits.obj xmitu.obj xmitp.obj xmit_s.obj xmit_y.obj
    $(link) $(linkdebug) $(conflags) -out:xmits.exe -map:xmits.map \
      xmits.obj xmitu.obj xmit_s.obj xmitp.obj xmit_y.obj \
      rpcrt4.lib $(conlibs)

# xmit server main
xmits.obj : xmits.c xmit.h xmitu.h

# remote procedures
xmitp.obj : xmitp.c xmit.h

# xmit server stub file
xmit_s.obj : xmit_s.c xmit.h

# xmit server auxiliary file
xmit_y.obj : xmit_y.c xmit.h

# Stubs, auxiliary and header file from the IDL file
xmit.h xmit_c.c xmit_x.c xmit_s.c xmit_y.c : xmit.idl xmit.acf
    midl -cpp_cmd $(cc) -cpp_opt "-E" xmit.idl

# Clean up everything
cleanall : clean
    -del *.exe

# Clean up everything but the .EXEs
clean :
    -del *.obj
    -del *.map
    -del xmit_c.c
    -del xmit_x.c
    -del xmit_s.c
    -del xmit_y.c
    -del xmit.h
