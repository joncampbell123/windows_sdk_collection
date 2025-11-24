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

INCLUDES=-I..\..\..\include

!ifndef SUBDIR
SUBDIR = MSSNDSYS
!MESSAGE SUBDIR undefined, defaulting to build in SUBDIR=$(SUBDIR)
!else
!MESSAGE building in SUBDIR=$(SUBDIR)
!endif

!if "$(SUBDIR)" == "MSSNDSYS"
SUBDIR  =MSSNDSYS  # ensure upper case for ifdefs in the code
NAME    =mssndsys
SYMNAME =mssvxd
!elseif "$(SUBDIR)" == "AZTECH"
SUBDIR  =AZTECH    # ensure upper case for ifdefs in the code
NAME    =azt16
SYMNAME =aztvxd
!else
!message usage: nmake SUBDIR=AZTECH | MSSNDSYS
!error SUBDIR must be defined to one of the above
!endif

DEF     =$(DEF) -D$(SUBDIR)

EXT	=vxd
GOALS	=$(NAME).$(EXT) $(SYMNAME).sym
OBJS    =cnfgmgr.obj mssndsys.obj\
         api.obj hardware.obj pipe.obj\
         messages.obj startup.obj\
         validate.obj wraps.obj

!if "$(SUBDIR)" == "MSSNDSYS"
OBJS    = $(OBJS) sbvirt.obj
!endif

!if "$(SUBDIR)" == "AZTECH"
OBJS    = $(OBJS) wss_sbp.obj
!endif         

AINCS = ..\mssndsys.inc ..\equates.inc ..\sbvirt.inc
CINCS = ..\cnfgmgr.h

!if "$(DEBUG)" == "NO" 
COPT	=
AOPT	=
LOPT	= 
!else
COPT	=-Zi -DDEBUG
AOPT	=-Zi -DDEBUG
LOPT	=-debug:full
!endif

CC	=cl -c -nologo -W3 -Zp -Gs -Ze -Ox -DIS_32 $(DEF) $(COPT) $(INCLUDES)
ASM	=ml -coff -DMASM6 -DBLD_COFF -I. /c -Cx -DIS_32 $(DEF) $(AOPT) $(INCLUDES)
LINK	=link
LFLAGS	=$(LOPT)
LIBS	=vxdwraps.clb
RC	=rc -DIS_32 $(DEF) $(ROPT) $(INCLUDES)

goal: makedir $(GOALS)

makedir:
	@md $(SUBDIR) >nul
	@cd $(SUBDIR)
        

cnfgmgr.obj:  ..\$*.c $(CINCS)

mssndsys.obj: ..\$*.asm $(AINCS)

api.obj:      ..\$*.asm $(AINCS)

hardware.obj: ..\$*.asm $(AINCS)

messages.obj: ..\$*.asm $(AINCS)

pipe.obj:     ..\$*.c $(CINCS)

sbvirt.obj:   ..\$*.asm $(AINCS)

startup.obj:  ..\$*.asm $(AINCS)

validate.obj: ..\$*.asm $(AINCS)

wraps.obj:    ..\$*.asm $(AINCS)

wss_sbp.obj:    ..\$*.asm $(AINCS)

{..}.asm{}.obj:
    $(ASM) ..\$*.asm

{..}.c{}.obj:
    $(CC) ..\$*.c

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
#define VERSIONSUBTYPE            UNDEFINED_DEVICE_ID

#include "verinfo.ver"

<<

clean:
	-@cd $(SUBDIR)
        -@del *.pdb
        -@del *.obj
        -@del *.map
        -@del *.sym
        -@del $(NAME).$(EXT)
        -@del *.res
	-@cd ..
	-@rd $(SUBDIR)
