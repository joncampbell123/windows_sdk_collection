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

#-----------------------------------------------------------------------
# Microsoft Windows 4.0 DIB Engine based frame buffer Resource Makefile
#-----------------------------------------------------------------------
SRCDIR= .
TARGETS = display.res
LINK = link /NOE/NOD/LI/MAP/AL:16
ASM = ml
A6FLAGS = /c /Zm /nologo /Cp

#-----------------------------------------------------------------------
# Object files
#-----------------------------------------------------------------------
OBJ1  =	config.bin 	colortab.bin 	fonts.bin 	display.rcv  
OBJ2  = fonts120.bin
OBJ6  =	hand.ico    	ques.ico	bang.ico  	note.ico sample.ico

OBJS =	$(OBJ1) $(OBJ2) $(OBJ3) $(OBJ4) $(OBJ5) $(OBJ6) 

default: display.res

depend:
	@echo "dependancies must be modified manually"

display.res: $(OBJS) 
    rc -r -v display.rc

#-----------------------------------------------------------------------
# Dependancies
#-----------------------------------------------------------------------
config.obj:	config.asm
	$(ASM) $(A6FLAGS) config.asm

fonts.obj:	fonts.asm
	$(ASM) $(A6FLAGS) fonts.asm

fonts120.obj:	fonts120.asm
	$(ASM) $(A6FLAGS) fonts120.asm

colortab.obj:	colortab.asm
	$(ASM) $(A6FLAGS) colortab.asm

config.bin:    config.obj
    link config ,config;
    exe2bin config
    del config.exe

fonts.bin:	fonts.obj
    link fonts;
    exe2bin fonts
    del fonts.exe

fonts120.bin:	fonts120.obj
    link fonts120;
    exe2bin fonts120
    del fonts120.exe

colortab.bin:  colortab.obj
    link colortab ,colortab;
    exe2bin colortab
    del colortab.exe

# END Dependencies 



