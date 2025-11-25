#*************************************************************#
#**                                                         **#
#**                 Microsoft RPC Examples                  **#
#**                   bndbuf Application                    **#
#**            Copyright(c) Microsoft Corp. 1992-1996       **#
#**                                                         **#
#*************************************************************#

# Nmake macros for building Win32 applications
!Include <NTWin32.Mak>

# Inference Rules
.C.Obj:
    $(cc) $(cdebug) $(cflags) $(cvarsmt) $*.C
.Obj.Exe:
    $(link) $(ldebug) $(conlflags) -out:$*.Exe $**\
      rpcrt4.lib rpcns4.lib $(conlibsmt) User32.Lib

# Macro Defines
PROJ=BndBufC
PROJ2=BndBufS

# Target List
All: $(PROJ).Exe $(PROJ2).Exe

# Make bndbufc
bndbufc : $(PROJ).exe
$(PROJ).exe : $(PROJ).obj bndbuf_c.obj

# bndbufc main program
$(PROJ).obj: $(PROJ).c bndbuf.h

# bndbufc stub
bndbuf_c.obj: bndbuf_c.c bndbuf.h


# Make the bndbufs
bndbufs: $(PROJ2).exe
$(PROJ2).exe : $(PROJ2).obj bndbufp.obj bndbuf_s.obj

# bndbufs main loop
$(PROJ2).obj: $(PROJ2).c bndbuf.h

# remote procedures
bndbufp.obj: bndbufp.c bndbuf.h

# bndbufs stub file
bndbuf_s.obj: bndbuf_s.c bndbuf.h


# Stubs, auxiliary and header file from the IDL file
bndbuf.h bndbuf_c.c bndbuf_s.c : bndbuf.idl bndbuf.acf
    midl -oldnames -cpp_cmd $(cc) -cpp_opt "-E" bndbuf.idl

# Clean Up Everything
CleanAll: clean
    @del *.exe 2> nul

# Clean Up Everything Except .Exe And .Dll Files
Clean:
    @del *.obj      2> nul
    @del *.map      2> nul
    @del bndbuf_c.c 2> nul
    @del bndbuf_s.c 2> nul
    @del bndbuf.h   2> nul
