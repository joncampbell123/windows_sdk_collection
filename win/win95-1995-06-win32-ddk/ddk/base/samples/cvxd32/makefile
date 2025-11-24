#****************************************************************************
#                                                                           *
# THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
# KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
# IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
# PURPOSE.                                                                  *
#                                                                           *
# Copyright 1993-95  Microsoft Corporation.  All Rights Reserved.           *
#                                                                           *
#****************************************************************************

CFLAGS     = -DWIN32 -DCON -Di386 -D_X86_ -D_NTWIN -W3 -Gs -D_DEBUG -Zi
CVXDFLAGS  = -Zdp -Gs -c -DIS_32 -Zl -DDEBLEVEL=1 -DDEBUG
LFLAGS     = -machine:i386 -debug:notmapped,full -debugtype:cv -subsystem:console kernel32.lib 
AFLAGS     = -coff -DBLD_COFF -DIS_32 -W2 -Zd -c -Cx -DMASM6 -DDEBLEVEL=1 -DDEBUG

all: direxist bin\con_samp.exe bin\cvxdsamp.vxd

direxist:
        if not exist bin\nul    md bin

bin\cvxdsamp.obj: cvxdsamp.c
        cl $(CVXDFLAGS) -Fo$@ %s

bin\cvxdctrl.obj: cvxdctrl.asm
        ml $(AFLAGS) -Fo$@ %s

bin\vkdgko.obj: vkdgko.asm
        ml -coff -DBLD_COFF -W2 -c -Cx -DMASM6 -DSEGNUM=1 -Fo$@ %s

bin\cvxdsamp.vxd: bin\cvxdsamp.obj bin\cvxdctrl.obj bin\vkdgko.obj \
                  cvxdsamp.def
        echo >NUL @<<cvxdsamp.crf
-MACHINE:i386 -DEBUG:NONE -PDB:NONE 
-DEF:cvxdsamp.def -OUT:bin\cvxdsamp.vxd -MAP:bin\cvxdsamp.map
-VXD vxdwraps.clb bin\cvxdsamp.obj bin\cvxdctrl.obj bin\vkdgko.obj
<<
        link @cvxdsamp.crf
        cd bin
        mapsym cvxdsamp
        cd ..

bin\con_samp.obj: con_samp.c 
        cl $(CFLAGS) -c -Fo$@ %s

bin\con_samp.exe: bin\con_samp.obj
        echo >NUL @<<con_samp.crf
$(LFLAGS)
-PDB:NONE -OUT:bin\con_samp.exe -MAP:bin\con_samp.map bin\con_samp.obj 
<<
        link @con_samp.crf

clean:
        del bin\*.obj
        del bin\*.vxd
        del bin\*.exe
        del bin\*.exp
        del bin\*.map
        del bin\*.sym
        del bin\*.lib
        rd  bin
        del *.pdb
        
