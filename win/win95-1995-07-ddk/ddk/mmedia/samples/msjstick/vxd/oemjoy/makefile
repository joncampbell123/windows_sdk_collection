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

!if "$(DDKROOT)" == ""
!error Environment variable DDKROOT must be set
!else
INCLUDES=-I.. -I$(DDKROOT)\mmedia\samples\include
!endif

NAME    =oemjoy
SYMNAME =oemjoy
EXT     =vxd
GOALS   =$(NAME).$(EXT) $(SYMNAME).sym
OBJS    =oemjoy.obj poll.obj callback.obj

AINCS = oemjoy.inc
CINCS = 

!if "$(DEBUG)" == "NO" 
COPT    =
AOPT    =
LOPT    = 
!else
COPT    =-Zi -DDEBUG
AOPT    =-Zi -DDEBUG
LOPT    =-debug:full
!endif

CC      =cl -DIS_32 -c -nologo -W3 -Zp -Gs -Ze -Ox $(COPT) $(INCLUDES)
ASM	=ml -coff -DMASM6 -DBLD_COFF -I.  /c -Cx $(AOPT) $(INCLUDES)
LINK    =link
LFLAGS  =$(LOPT) -nodefaultlib
LIBS    =vxdwraps.clb
RC      =rc $(ROPT) $(INCLUDES)

goal: $(GOALS)

callback.obj: $*.c $(CINCS)

oemjoy.obj:   $*.asm $(AINCS)

poll.obj:     $*.asm $(AINCS)

.asm.obj:
    $(ASM) $*.asm

.c.obj:
    $(CC) $*.c

$(NAME).$(EXT): $(OBJS) $(NAME).def $(NAME).res
        $(LINK) $(LFLAGS) @<<
$(OBJS)
-noentry
-vxd
-nodefaultlib
-out:$(NAME).$(EXT)
-map:$(NAME).map
-def:$(NAME).def
-noentry
-subsystem:windows,4.0
$(LIBS)
<<
!if exist( $(DDKROOT)\bin\adrc2vxd.exe )
	adrc2vxd $@ $(NAME).res
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
