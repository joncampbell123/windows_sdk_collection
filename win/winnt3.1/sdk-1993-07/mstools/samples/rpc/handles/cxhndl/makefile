#*************************************************************#
#**                                                         **#
#**                 Microsoft RPC Examples                  **#
#**                   cxhndl Application                    **#
#**            Copyright(c) Microsoft Corp. 1992            **#
#**                                                         **#
#*************************************************************#

!include <ntwin32.mak>

!if "$(CPU)" == "i386"
cflags = $(cflags:G3=Gz)
!endif

.c.obj:
   $(cc) $(cdebug) $(cflags) $(cvars) $<

all : cxhndlc cxhndls

# Make the cxhndl client
cxhndlc : cxhndlc.exe
cxhndlc.exe : cxhndlc.obj cxhndl_c.obj cxhndl_x.obj
    $(link) $(linkdebug) $(conflags) -out:cxhndlc.exe \
      cxhndlc.obj cxhndl_c.obj cxhndl_x.obj \
      rpcrt4.lib $(conlibs)

# client main program
cxhndlc.obj : cxhndlc.c cxhndl.h

# client stub
cxhndl_c.obj : cxhndl_c.c cxhndl.h

# client auxiliary file
cxhndl_x.obj : cxhndl_x.c cxhndl.h

# Make the cxhndl server
cxhndls : cxhndls.exe
cxhndls.exe : cxhndls.obj cxhndlp.obj cxhndl_s.obj cxhndl_y.obj
    $(link) $(linkdebug) $(conflags) -out:cxhndls.exe \
      cxhndls.obj cxhndlp.obj cxhndl_s.obj cxhndl_y.obj \
      rpcrt4.lib $(conlibs)

# server main loop
cxhndls.obj : cxhndls.c cxhndl.h

# remote procedures
cxhndlp.obj : cxhndlp.c cxhndl.h

# server stub file
cxhndl_s.obj : cxhndl_s.c cxhndl.h

# server auxiliary file
cxhndl_y.obj : cxhndl_y.c cxhndl.h

# Stubs, auxiliary and header file from the IDL file
cxhndl.h cxhndl_c.c cxhndl_x.c cxhndl_s.c cxhndl_y.c : cxhndl.idl cxhndl.acf
    midl -cpp_cmd $(cc) -cpp_opt "-E" cxhndl.idl

# Clean up everything
cleanall : clean
    -del *.exe

# Clean up everything but the .EXEs
clean :
    -del *.obj
    -del *.map
    -del cxhndl_c.c
    -del cxhndl_x.c
    -del cxhndl_s.c
    -del cxhndl_y.c
    -del cxhndl.h
