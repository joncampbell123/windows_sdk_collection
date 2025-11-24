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

################################
#####                      #####  
##### THIS IS A 16-BIT DLL #####
#####                      #####  
################################

NAME   = serialui
OBJS   = libinit.obj pch.obj init.obj serialui.obj err.obj comm.obj 
OBJS   = $(OBJS) cstrings.obj port.obj
LFLAGS = /ALIGN:16 /MAP /NOE /NOD
LIBS   = LIBW SHELL MNOCRTDW MLIBCEW COMMCTRL
CFLAGS = -c -AMw -G3Dcs -W3 -Oxs -Zldp -DDEBLEVEL=0
INCL   = suiprv.h mcx16.h cstrings.h comm.h err.h    

default: serialui.dll

pch.c:
        echo #include "suiprv.h" > pch.c

pch.obj: pch.c $(INCL)
        cl $(CFLAGS) -Ycsuiprv.h -NT _TEXT $**

libinit.obj: libinit.asm
        ml -Zm -Zd -c -W2 -DDEBLEVEL=0 $**
        
init.obj: init.c suiprv.pch $(INCL)
	cl $(CFLAGS) -Yusuiprv.h -NT INIT_TEXT $*.c

serialui.obj: serialui.c suiprv.pch $(INCL)
        cl $(CFLAGS) -Yusuiprv.h -NT COMMON_TEXT $*.c

err.obj: err.c suiprv.pch $(INCL)
        cl $(CFLAGS) -Yusuiprv.h -NT COMMON_TEXT $*.c

comm.obj: comm.c suiprv.pch $(INCL)
        cl $(CFLAGS) -Yusuiprv.h -NT COMMON_TEXT $*.c

cstrings.obj: cstrings.c suiprv.pch $(INCL)
        cl $(CFLAGS) -Yusuiprv.h -NT COMMON_TEXT $*.c

port.obj: port.c suiprv.pch $(INCL)
        cl $(CFLAGS) -Yusuiprv.h -NT COMMON_TEXT $*.c

$(NAME).res: $(NAME).rc
        rc -r $(NAME).rc

serialui.dll:  $(OBJS) $(NAME).res $(NAME).def 
        link @<<$(NAME).lnk
$(LFLAGS) +
$(OBJS)
$(NAME).dll
$(NAME).map
$(LIBS)
$(NAME).def
<<
        mapsym $(NAME).map
        rc -fe $(NAME).dll $(NAME).res

clean:
        del *.obj
        del *.map
        del *.sym
        del *.res
        del *.dll
        del *.pch
        del pch.c
