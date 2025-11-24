##************************************************************************
##**
##**  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
##**  ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
##**  TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR
##**  A PARTICULAR PURPOSE.
##**
##**  Copyright (C) 1993 - 1995 Microsoft Corporation. All Rights Reserved.
##**
##**  makefile
##**
##**  to build a debug version:
##**       NMAKE
##**  to build a non debug version:
##**       NMAKE DEBUG=NO
##**
##**  HISTORY:
##**     04/12/94       created.
##**
##************************************************************************

INCLUDES=-I..\..\include

NAME	=msopl
SYMNAME	=msoplvxd
EXT	=vxd
GOALS	=$(NAME).$(EXT) $(SYMNAME).sym
OBJS	=cnfgmgr.obj msopl.obj api.obj messages.obj startup.obj validate.obj

AINCS = equates.inc
CINCS = cnfgmgr.h

!if "$(DEBUG)" == "NO" 
COPT	=
AOPT	=
LOPT	= 
!else
COPT	=-Zi -DDEBUG
AOPT	=-Zi -DDEBUG
LOPT	=-debug:full
!endif

CC	=cl -c -nologo -W3 -Zp -Gs -Ze -Ox -DIS_32 $(COPT) $(INCLUDES)
ASM	=ml -coff -DMASM6 -DBLD_COFF -I. /c -Cx -DIS_32 $(AOPT) $(INCLUDES)
LINK	=link
LFLAGS	=$(LOPT)
LIBS	=vxdwraps.clb
RC	=rc -DIS_32 $(ROPT) $(INCLUDES)

goal: $(GOALS)

msopl.obj:    msopl.asm $(AINCS)

cnfgmgr.obj:  cnfgmgr.c $(CINCS)

api.obj:      api.asm $(AINCS)

messages.obj: messages.asm $(AINCS)

startup.obj:  startup.asm $(AINCS)

validate.obj: validate.asm $(AINCS)

.asm.obj:
    $(ASM) $*.asm

.c.obj:
    $(CC) $*.c

DESCRIPTION = $(NAME) Virtual Device	(Version 4.0)

!ifndef DEVICETYPE
DEVICETYPE = DYNAMIC
!endif

!ifndef DDB_NAME
DDB_NAME = $(NAME)_DDB
!endif

$(NAME).$(EXT) $(NAME).map: $(OBJS) $(NAME).res
	@$(LINK) @<<$(NAME).lnk /def:<<$(NAME).def
$(LFLAGS)
-out:$(@B).$(EXT)
-map:$(@B).map
-vxd
-version:4.0
-nodefaultlib
$(OBJS)
$(LIBS)
<<
VXD $(SYMNAME) $(DEVICETYPE)
DESCRIPTION '$(DESCRIPTION)'
EXETYPE		DEV386
PROTMODE

SEGMENTS
	_LPTEXT		CLASS 'LCODE'	PRELOAD NONDISCARDABLE
	_LTEXT		CLASS 'LCODE'	PRELOAD NONDISCARDABLE
	_LDATA		CLASS 'LCODE'	PRELOAD NONDISCARDABLE
	_TEXT		CLASS 'LCODE'	PRELOAD NONDISCARDABLE
	_DATA		CLASS 'LCODE'	PRELOAD NONDISCARDABLE
	CONST		CLASS 'LCODE'	PRELOAD NONDISCARDABLE
	_TLS		CLASS 'LCODE'	PRELOAD NONDISCARDABLE
	_BSS		CLASS 'LCODE'	PRELOAD NONDISCARDABLE
	_LMSG		CLASS 'MCODE'	PRELOAD NONDISCARDABLE IOPL
	_IMSG		CLASS 'MCODE'	PRELOAD DISCARDABLE IOPL
	_ITEXT		CLASS 'ICODE'	DISCARDABLE
	_IDATA		CLASS 'ICODE'	DISCARDABLE
	_PTEXT		CLASS 'PCODE'	NONDISCARDABLE
	_PMSG		CLASS 'MCODE'	NONDISCARDABLE IOPL
	_PDATA		CLASS 'PDATA'	NONDISCARDABLE SHARED
	_STEXT		CLASS 'SCODE'	RESIDENT
	_SDATA		CLASS 'SCODE'	RESIDENT
	_DBOSTART	CLASS 'DBOCODE'	PRELOAD NONDISCARDABLE CONFORMING
	_DBOCODE	CLASS 'DBOCODE'	PRELOAD NONDISCARDABLE CONFORMING
	_DBODATA	CLASS 'DBOCODE'	PRELOAD NONDISCARDABLE CONFORMING
	_16ICODE	CLASS '16ICODE'	PRELOAD DISCARDABLE
	_RCODE		CLASS 'RCODE'
EXPORTS
	$(DDB_NAME) @1
<<
!if exist( $(DDKROOT)\bin\adrc2vxd.exe )
	@adrc2vxd $*.$(EXT) $(NAME).res
!endif

$(SYMNAME).sym:  $(NAME).map
        mapsym -mo $(SYMNAME).sym $(NAME).map

$(NAME).res:
	$(SDKROOT)\binw16\$(RC) -fo$*.res -r -i $(SDKROOT)\inc16 <<$(NAME).rc
#include <ver.h>
#include "verinfo.h"
#define Not_VxD
#include <vmm.h>                  // for UNDEFINED_DEVICE_ID

#define VERSIONNAME               "$(NAME).$(EXT)\0"
#define VERSIONDESCRIPTION        "$(NAME) Sample Virtual Device\0"
#define VERSIONTYPE               VFT_VXD
#define VERSIONSUBTYPE              UNDEFINED_DEVICE_ID

#include "verinfo.ver"

<<

clean:
        -@del *.obj
        -@del *.map
        -@del *.sym
        -@del $(NAME).$(EXT)
        -@del *.res
