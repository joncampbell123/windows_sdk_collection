#*************************************************************#
#**                                                         **#
#**                 Microsoft RPC Examples                  **#
#**                   bndbuf Application                    **#
#**            Copyright(c) Microsoft Corp. 1992, 1993      **#
#**                                                         **#
#*************************************************************#

!include <ntwin32.mak>

.c.obj:
   $(cc) $(cdebug) $(cflags) $(cvarsmt) $<

all : bndbufc bndbufs

# Make the bndbufc
bndbufc : bndbufc.exe
bndbufc.exe : bndbufc.obj bndbuf_c.obj bndbuf_x.obj
    $(link) $(linkdebug) $(conflags) -out:bndbufc.exe \
      bndbufc.obj bndbuf_c.obj bndbuf_x.obj \
      rpcrt4.lib rpcns4.lib $(conlibsmt)

# bndbufc main program
bndbufc.obj : bndbufc.c bndbuf.h

# bndbufc stub
bndbuf_c.obj : bndbuf_c.c bndbuf.h

# bndbufc auxiliary file
bndbuf_x.obj : bndbuf_x.c bndbuf.h

# Make the bndbufs
bndbufs : bndbufs.exe
bndbufs.exe : bndbufs.obj bndbufp.obj bndbuf_s.obj bndbuf_y.obj
    $(link) $(linkdebug) $(conflags) -out:bndbufs.exe \
      bndbufs.obj bndbuf_s.obj bndbufp.obj bndbuf_y.obj \
      rpcrt4.lib rpcns4.lib $(conlibsmt)

# bndbufs main loop
bndbufs.obj : bndbufs.c bndbuf.h

# remote procedures
bndbufp.obj  : bndbufp.c bndbuf.h

# bndbufs stub file
bndbuf_s.obj : bndbuf_s.c bndbuf.h

# bndbufs auxiliary file
bndbuf_y.obj : bndbuf_y.c bndbuf.h

# Stubs, auxiliary and header file from the IDL file
bndbuf.h bndbuf_c.c bndbuf_x.c bndbuf_s.c bndbuf_y.c : bndbuf.idl bndbuf.acf
    midl -cpp_cmd $(cc) -cpp_opt "-E" bndbuf.idl

# Clean up everything
cleanall : clean
    -del *.exe

# Clean up everything but the .EXEs
clean :
    -del *.obj
    -del *.map
    -del bndbuf_c.c
    -del bndbuf_x.c
    -del bndbuf_s.c
    -del bndbuf_y.c
    -del bndbuf.h
