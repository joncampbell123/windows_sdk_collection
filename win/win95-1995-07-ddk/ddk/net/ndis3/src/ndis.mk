##################################################################
#
#       Microsoft Confidential
#       Copyright (C) Microsoft Corporation 1993-95
#       All Rights Reserved.
#
#       Master Makefile for NDIS 3.1 drivers
#
#       INPUT:
#               BIN: Where to put the stuff
#               DEB: Flags to control debug level
#
##################################################################

NDIS_STDCALL=1

# Set this to 1 if you use an older compiler and link386.exe (OMB format)
# dont set this if you use MSVC20 compiler and link.exe (COFF format, default)

#OMB=1

!IFNDEF DEBLEVEL
DEBLEVEL=1
!ENDIF

DDEB            =       -DDEBUG -DDBG=1 -DDEBLEVEL=$(DEBLEVEL) -DNDIS2=1 -DCHICAGO
RDEB            =       -DDEBLEVEL=0 -DNDIS2=1 -DCHICAGO

!IFNDEF BIN
BIN             =       retail
DEB             =       $(RDEB)
!ENDIF


WIN32           =       $(DDKROOT)
NETROOT         =       $(DDKROOT)\net
NDISROOT        =       $(NETROOT)\ndis3
NDISSRC         =       $(NDISROOT)\src
LIBDIR          =       $(NDISROOT)\lib
INCLUDE         =       $(INCLUDE);.

DDKTOOLS        =       $(WIN32)\bin

ASM             =       ml.exe
CL              =       cl.exe -bzalign
CHGNAM          =       chgnam.exe
CHGNAMSRC       =       $(DDKTOOLS)\chgnam.vxd
INCLUDES        =       $(NETROOT)\bin\includes.exe
MAPSYM          =       mapsym

!ifdef OMB
LIBNDIS         =       $(LIBDIR)\$(BIN)\libndis.lib
LINK            =       link386.exe
!else
LIBNDIS         =       $(LIBDIR)\$(BIN)\libndis.clb
LINK            =       link.exe
!endif


LFLAGS  =   /m /NOD /MA /LI /NOLOGO /NOI 

CFLAGS  = -nologo -W2 -Zdp -Gs -DIS_32 -Zl -Oy- -Ox -c
AFLAGS  = -DIS_32 -nologo -W2 -Zd -Cx -DMASM6 -DVMMSYS -Zm -DWIN31COMPAT

!ifdef OMB
AFLAGS  = $(AFLAGS) -DNDIS_WIN -c 
!else
AFLAGS  = $(AFLAGS) -DNDIS_WIN -c -coff -DBLD_COFF
!endif

!ifdef NDIS_MINI_DRIVER

NDIS_STDCALL=1

CFLAGS = $(CFLAGS) -DNDIS_MINI_DRIVER
AFLAGS = $(AFLAGS) -DNDIS_MINI_DRIVER

!endif

!ifdef NDIS_STDCALL
CFLAGS = $(CFLAGS) -Gz -DNDIS_STDCALL
AFLAGS = $(AFLAGS) -DNDIS_STDCALL
!endif

.asm{$(BIN)}.obj:
		set INCLUDE=$(INCLUDE)
		set ML= $(AFLAGS) $(DEB)
		$(ASM) -Fo$*.obj $<

.asm{$(BIN)}.lst:
		set INCLUDE=$(INCLUDE)
		set ML= $(AFLAGS) $(DEB)
		$(ASM) -Fl$*.obj $<

.c{$(BIN)}.obj:
		set INCLUDE=$(INCLUDE)
		set CL= $(CFLAGS) $(DEB)
		$(CL) -Fo$*.obj $<
#                $(CHGNAM) $(CHGNAMSRC) $*.obj

{$(NDISSRC)}.asm{$(BIN)}.obj:
		set INCLUDE=$(INCLUDE)
		set ML= $(AFLAGS) $(DEB) -DMAC=$(DEVICE)
		$(ASM) -Fo$*.obj $<

{$(NDISSRC)}.asm{$(BIN)}.lst:
		set INCLUDE=$(INCLUDE)
		set ML= $(AFLAGS) $(DEB) -DMAC=$(DEVICE)
		$(ASM) -Fl$*.obj $<

{$(NDISSRC)}.c{$(BIN)}.obj:
		set INCLUDE=$(INCLUDE)
		set CL= $(CFLAGS) $(DEB)
		$(CL) -Fo$*.obj $<
#                $(CHGNAM) $(CHGNAMSRC) $*.obj
		

target: $(BIN) $(BIN)\$(DEVICE).VXD

$(BIN):
        if not exist $(BIN)\nul md $(BIN)

dbg:    depend
		$(MAKE) BIN=debug DEB="$(DDEB)"

rtl:    depend
		$(MAKE) BIN=retail DEB="$(RDEB)"


all: rtl dbg

!if EXIST (depend.mk)
!include depend.mk
!endif

VERSION =   4.0

!ifdef OMB

$(BIN)\$(DEVICE).VXD: $(OBJS) $(DEVICE).def $(LIBNDIS)
				$(LINK) @<<
$(OBJS: =+^
)
$(BIN)\$(DEVICE).VXD $(LFLAGS)
$(BIN)\$(DEVICE).map
$(LIBNDIS)
$(DEVICE).def
<<

!else

$(BIN)\$(DEVICE).VXD: $(OBJS) $(DEVICE).def $(LIBNDIS)
		$(LINK) @<<
-MACHINE:i386
-DEBUG:NONE
-PDB:NONE
-DEF:$(DEVICE).def
-OUT:$(BIN)\$(DEVICE).VXD
-MAP:$(BIN)\$(DEVICE).map
-VXD
$(LIBNDIS)
$(OBJS: =^
)


<<
!endif
		cd      $(BIN)
		$(MAPSYM) $(DEVICE)
		cd      ..
		


depend:
#        -mkdir debug
#        -mkdir retail
		set INCLUDE=$(INCLUDE)
		$(INCLUDES) -i -L$$(BIN) -S$$(BIN) *.asm *.c > depend.mk
		$(INCLUDES) -i -L$$(BIN) -S$$(BIN) $(NDISSRC)\ndisdev.asm >> depend.mk


clean :
		- del debug\*.obj
		- del debug\*.sym
                - del debug\*.VXD
		- del debug\*.map
		- del debug\*.lst
		- del retail\*.obj
		- del retail\*.sym
                - del retail\*.VXD
		- del retail\*.map
		- del retail\*.lst
		- del depend.mk


