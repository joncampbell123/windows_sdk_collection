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
##**  Please note that if the mmsystem.inc which comes with this DDK is not
##**  on the INCLUDE path before the SDK mmsystem.inc, the new structures
##**  will not be defined.
##**
##************************************************************************

!if "$(DDKROOT)" == ""
!error Environment variable DDKROOT must be set
!else
INCLUDES=-I$(DDKROOT)\mmedia\samples\include
!endif

#make mmsystem.inc's location last in include path, which is  after -Ipaths
INCLUDE=$(INCLUDE);$(SDKROOT)\INC16   #append to inherited environment variable

NAME	=vjoyd
SYMNAME	=vjoydvxd
EXT	=vxd
GOALS	=$(NAME).$(EXT) $(SYMNAME).sym
OBJS	=vjoyd.obj configh.obj getpos.obj fixdata.obj getbtns.obj poll1.obj poll2.obj poll3.obj poll4.obj othervxd.obj

AINCS =
CINCS =

!if "$(DEBUG)" == "NO" 
COPT	=
AOPT	=
LOPT	= 
!else
COPT	=-Zi -DDEBUG
AOPT	=-Zi -DDEBUG
LOPT	=-debug:full
!endif

CC	=cl -c -nologo -W3 -Zp -Gs -Ze -Ox -DIS_32 -DWINVER=0x0400 $(COPT) $(INCLUDES)
ASM	=ml -coff -DMASM6 -DBLD_COFF -I. /c -Cx -DIS_32 $(AOPT) $(INCLUDES)
LINK	=link
LFLAGS	=$(LOPT)
LIBS	=vxdwraps.clb
RC	=rc -DIS_32 $(ROPT) $(INCLUDES)

goal: $(GOALS)

fixdata.obj:  $*.c $(CINCS)

getpos.obj:   $*.c $(CINCS)

othervxd.obj: $*.c $(CINCS)

configh.obj:  $*.c $(CINCS)

getbtns.obj:  $*.asm $(AINCS)

poll1.obj:    $*.asm $(AINCS)

poll2.obj:    $*.asm $(AINCS)

poll3.obj:    $*.asm $(AINCS)

poll4.obj:    $*.asm $(AINCS)

vjoyd.obj:    $*.asm $(AINCS)


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
-machine:i386
-subsystem:windows,4.0
-noentry
-nodefaultlib
$(OBJS)
$(LIBS)
<<
VXD $(NAME) $(DEVICETYPE)
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
	_LMSGTABLE	CLASS 'MCODE'	PRELOAD NONDISCARDABLE IOPL
	_LMSGDATA	CLASS 'MCODE'	PRELOAD NONDISCARDABLE IOPL
	_IMSGTABLE	CLASS 'MCODE'	PRELOAD DISCARDABLE IOPL
	_IMSGDATA	CLASS 'MCODE'	PRELOAD DISCARDABLE IOPL
	_ITEXT		CLASS 'ICODE'	DISCARDABLE
	_IDATA		CLASS 'ICODE'	DISCARDABLE
	_PTEXT		CLASS 'PCODE'	NONDISCARDABLE
	_PMSGTABLE	CLASS 'MCODE'	NONDISCARDABLE IOPL
	_PMSGDATA	CLASS 'MCODE'	NONDISCARDABLE IOPL
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
