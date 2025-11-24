all : helloc

# Tools Dirs
!ifndef MSVC20
MSVC20=\msvc20
!endif

#
# Mac is built with VC++ for the Macintosh
#
MAC_BIN=$(MSVC20)\bin
MAC_BIN_68K=$(MSVC20)\m68k\bin
MAC_LIB=$(MSVC20)\m68k\lib
MAC_INC=$(MSVC20)\m68k\include
MACSDK_LIB=$(MACSDK)\lib
MACSDK_INC=$(MACSDK)\include

#
# Common Tools
#
MAKE=nmake -nologo 
MIDL=midl

CC           =$(MAC_BIN_68K)\cl
LIBRARIAN    =$(MAC_BIN)\lib
LINK         =$(MAC_BIN)\link
RC           =$(MAC_BIN_68K)\rc
MRC          =$(MAC_BIN_68K)\mrc

LIBFLAGS      =$(LIBFLAGS) -nologo
LINKFLAGS     =$(LINKFLAGS) -nologo -nod
MIDLFLAGS     =$(MIDLFLAGS) -env mac -DMAC -I.. -cpp_cmd $(CC)
MRCFLAGS      =$(MRCFLAGS) /I$(MAC_INC)\mrc /I$(MAC_INC) /D_MAC /DMAC
RCFLAGS       =$(RCFLAGS) /r /D_MAC /D_68K_ /I$(MAC_INC) /I$(MSVC20)\include

#INCLUDESFLAGS =$(INCLUDESFLAGS) -e -S -L $(CINC) \
#               -P$$(MAC_INC)=$(MAC_INC) -P$$(MAC_BIN)=$(MAC_BIN) -P$$(MSVC20)=$(MSVC20)\
#               -nwindows.h -nnt.h -nntrtl.h -nnturtl.h\
#               -nbse.h -nos2def.h -nwchar.h

!ifdef RELEASE

CFLAGSBASE   =$(CFLAGSBASE) -Ogisy -Ob1 -Gs

!else # ! RELEASE

CFLAGSBASE   =$(CFLAGSBASE) -Q68m -Zi -Od
CDEF         =$(CDEF) -DDEBUGRPC
LINKFLAGS    =$(LINKFLAGS) -debug:full -debugtype:both
LIBFLAGS     =$(LIBFLAGS) -debugtype:both
MRCFLAGS     =$(MRCFLAGS) /D_DEBUG

!endif 

CFLAGSBASE   =$(CFLAGSBASE) -nologo -W2 -AL /Q68020 /Q68s
CINC         =$(CINC) -I. -I $(MACSDK_INC) -I$(MSVC20)\m68k\include -I$(INCLUDE) 
CDEF         =$(CDEF) -DMAC -Dcdecl=

CXXFLAGS     =$(CXXFLAGS) $(CFLAGSBASE) $(CINC) $(CDEF)
CFLAGS       =$(CFLAGS)   $(CFLAGSBASE) $(CINC) $(CDEF)

# Make the client side application helloc
helloc : helloc.exe
helloc.exe : helloc.obj hello_c.obj hello.rsc wgen.rsc $(MACSDK_LIB)\rpc.lib
   $(LINK) $(LINKFLAGS) /MAC:bundle /MAC:type="APPL" /MAC:creator="????"\
	-out:$@  $**     \
	$(MAC_LIB)\libcs.lib $(MAC_LIB)\interfac.lib $(MAC_LIB)\swapd.lib $(MAC_LIB)\lsanes.lib  \
	$(MAC_LIB)\wlm.lib $(MAC_LIB)\aslm.lib $(MAC_LIB)\wlm.rsc $(MAC_LIB)\commdlg.rsc 

# helloc main program
helloc.obj : ..\helloc.c hello.h
   $(CC)  $(CFLAGS) ..\$*.c -c

# helloc stub
hello_c.obj : hello_c.c hello.h
   $(CC)  $(CFLAGS) $*.c -c

wgen.rsc: ..\wgen.r
   $(MRC) $(MRCFLAGS) -o .\$*.rsc ..\$*.r

hello.rsc: ..\hello.rc
   $(RC) $(RCFLAGS) -fo .\$*.rsc ..\$*.rc

# Stubs and header file from the IDL file
hello.h hello_c.c: ..\hello.idl ..\hello.acf
    midl $(MIDLFLAGS) ..\hello.idl


