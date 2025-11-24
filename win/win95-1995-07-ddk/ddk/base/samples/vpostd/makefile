#****************************************************************************
#                                                                           *
# THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
# KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
# IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
# PURPOSE.                                                                  *
#                                                                           *
# Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
#                                                                           *
#****************************************************************************

#
#   Makefile for VPOSTD.VXD, POSTHOST.DLL, POSTAPP.EXE, and POSTDOS.EXE
#
#   This is a sample demonstrating how to PostMessage from a VxD.
#

NAME = VPOSTD

# supply the tools location information below

LINK16	= link
CL16	= cl
RC16	= rc

#
# for POSTDOS.EXE build
#

!IFDEF DEBUG_POSTDOS

DAS =   ml -c -Zm -Zi
DCC =   $(CL16) -c -Zi
DLN =   $(LINK16) /CO
DZZ =   POSTDOS: ***** DEBUG ***** DEBUG ***** DEBUG ***** DEBUG *****

!ELSE

DAS =   ml -c -Zm 
DCC =   $(CL16) -c
DLN =   $(LINK16)
DZZ =   POSTDOS: **** RELEASE *** RELEASE *** RELEASE *** RELEASE ****

!ENDIF

DSRCS   =   postdos.c
DOBJS   =   postdos.obj

#
# for POSTAPP.EXE build
#

!IFDEF DEBUG_POSTAPP

AAS =   ml -DIS_32 -W2 -c -Cx -Zm -Zi -DMASM6 -DDEBLEVEL=1 -DDEBUG
ACC =   $(CL16) -c -DDEBUG -AM -G2sw -Owd -W3 -Ziped
ALN =   $(LINK16) /NOD/NOE/A:16/CO
ARC =   $(RC16) -v -DDEBUG
AZZ =   POSTAPP: ***** DEBUG ***** DEBUG ***** DEBUG ***** DEBUG *****

!ELSE

AAS =   ml -DIS_32 -W2 -c -Cx -Zm -DMASM6 -DDEBLEVEL=0 
ACC =   $(CL16) -c -DRELEASE -AM -G2sw -Ows -W3 -Zpe
ALN =   $(LINK16) /NOD/NOE/A:16
ARC =   $(RC16) -v -DRELEASE
AZZ =   POSTAPP: **** RELEASE *** RELEASE *** RELEASE *** RELEASE ****

!ENDIF

AMISC   =   postapp.def     postapp.lnk
AINCS   =   postapp.h
ASRCS   =   postapp.c
AOBJS   =   postapp.obj


#
# for POSTHOST.DLL build
#

!IFDEF DEBUG_POSTHOST

LAS =   ml -c -Zm -Zi
LCC =   $(CL16) -c -DDEBUG -Alnw -G2sw -Owd -W3 -Ziped
LLN =   $(LINK16) /NOD/NOE/A:16/CO
LRC =   $(RC16) -v -DDEBUG
LZZ =   POSTHOST: ***** DEBUG ***** DEBUG ***** DEBUG ***** DEBUG *****

!ELSE

LAS =   ml -c -Zm 
LCC =   $(CL16) -c -DRELEASE -Alnw -G2sw -Ows -W3 -Zpe
LLN =   $(LINK16) /NOD/NOE/A:16
LRC =   $(RC16) -v -DRELEASE
LZZ =   POSTHOST: **** RELEASE *** RELEASE *** RELEASE *** RELEASE ****

!ENDIF

LMISC   =   posthost.def    posthost.lnk

LINCS   =   posthost.h
LSRCS   =   posthost.c      libentry.asm
LOBJS   =   posthost.obj    libentry.obj

#
# for VPOSTD.VXD build
#

!IFDEF DEBUG_VPOSTD

ASM    = ml
AFLAGS = -coff -DBLD_COFF -DIS_32 -W2 -c -Cx -Zm -DMASM6 -DDEBUG
ASMENV = ML
LFLAGS = /VXD /NOD
VRC =   rc -v -DDEBUG
VZZ =   VPOSTD: ***** DEBUG ***** DEBUG ***** DEBUG ***** DEBUG *****

!ELSE

ASM    = ml
AFLAGS = -coff -DBLD_COFF -DIS_32 -W2 -c -Cx -Zm -DMASM6 -DRELEASE
ASMENV = ML
LFLAGS = /VXD /NOD
VRC =   rc -v -DRELEASE
VZZ =   VPOSTD: **** RELEASE *** RELEASE *** RELEASE *** RELEASE ****

!ENDIF

VMISC   =   vpostd.def  vpostd.lnk
VINCS   =   vpostd.inc
VSRCS   =   vpostd.asm
VOBJS   =   vpostd.obj

#
# now the rules
#

all: $(NAME).vxd 

# VPOSTD.vxd

$(NAME).vxd: $(NAME).def $(VOBJS)
        link @<<$(NAME).lnk
$(LFLAGS) 
/OUT:$(NAME).vxd
/MAP:$(NAME).map
/DEF:$(NAME).def
$(VOBJS)
<<
        mapsym -s -o $(NAME).sym $(NAME).map

vpostd.obj: vpostd.asm  vpostd.inc
		set $(ASMENV)=$(AFLAGS)
		$(ASM) -Fo$*.obj $*.asm 

vpostd.lst: vpostd.asm  vpostd.inc
                $(VAS) $*;

#
# to build posthost.dll run ddkenv 16 and run nmake posthost.dll
#
# POSTHOST.DLL

posthost.dll:   $(LOBJS) posthost.def
                $(LLN) @posthost.lnk
                implib posthost.lib posthost.def
                $(LRC) posthost.dll
                @echo $(LZZ)
                @echo $(LZZ)
                @echo $(LZZ)

posthost.obj:   posthost.c posthost.h
                $(LCC) $*.c

libentry.obj:   libentry.asm
                $(LAS) -Fo$*.obj $*.asm

#
# to build postapp.exe run ddkenv 16 and run nmake postapp.exe
#
# POSTAPP.EXE

postapp.exe ::   $(AOBJS) postapp.def postapp.res posthost.dll
                $(ALN) @postapp.lnk
                $(ARC) postapp.res
                @echo $(AZZ)
                @echo $(AZZ)
                @echo $(AZZ)

postapp.exe ::   postapp.res
                $(ARC) postapp.res
                @echo $(AZZ)
                @echo $(AZZ)
                @echo $(AZZ)

postapp.obj :   postapp.c postapp.h posthost.h
                $(ACC) $*.c

postapp.res :   postapp.rc postapp.h postapp.ico
                $(ARC) -r postapp.rc


#
# to build postdos.exe run ddkenv 16 and run nmake postdos.exe
#
# POSTDOS.EXE

postdos.exe :   $(DOBJS)
                $(DLN) $*;
                @echo $(DZZ)
                @echo $(DZZ)
                @echo $(DZZ)

postdos.obj :   postdos.c
                $(DCC) $*.c


