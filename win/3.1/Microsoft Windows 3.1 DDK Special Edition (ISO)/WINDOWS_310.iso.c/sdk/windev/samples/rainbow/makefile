##### Module Macro #####
NAME	= rainbow
SRCS	= $(NAME).c
OBJS	= libentry.obj

##### C7 Macro #####
C7	= 1

##### Library Macro #####
LIBS	= libw sdllcew
MOD	= -ASw

##### Include Macro #####
INCLS	= $(NAME).h

##### Resource Macro #####
RCFILES = $(NAME).dlg $(NAME).rc

##### DEBUG Defined #####
DEBUG	= 1

##### Build Option Macros #####
!if $(DEBUG)
DDEF	= -DDEBUG
CLOPT	= -Zid -Od
MOPT	= -Zi
LOPT	= /CO /LI /MAP
!else
DDEF	=
CLOPT	= -Os
LOPT	=
!endif

##### General Macros #####
DEF	=

##### Tool Macros #####
ASM	= masm -Mx $(MOPT) $(DDEF) $(DEF)
CC	= cl -nologo -c $(MOD) -G2sw -Zp -W3 $(CLOPT) $(DDEF) $(DEF)
LINK	= link /NOD /NOE $(LOPT)
RC	= rc $(DDEF) $(DEF)
HC	= hc

##### Inference Rules #####
.c.obj:
    $(CC) $*.c

.asm.obj:
    $(ASM) $*.asm;

.rc.res:
    $(RC) -r $*.rc

##### Main (default) Target #####
goal: $(NAME).dll

##### Dependents For Goal and Command Line #####
$(NAME).dll: $(SRCS:.c=.obj) $(NAME).def $(NAME).res
    $(LINK) @<<
    $(SRCS:.c=.obj) $(OBJS),
    $(NAME).dll,
    $(NAME).map,
    $(LIBS),
    $(NAME).def
<<
    $(RC) -T $(NAME).res $(NAME).dll
!if $(DEBUG)
!if !$(C7)
    cvpack -p $(NAME).dll
!endif
    mapsym $(NAME).map
!endif

##### Dependents #####
$(SRCS:.c=.obj): $(INCLS)
$(NAME).res: $(INCLS) $(RCFILES)

##### Clean Directory #####
clean:
    -del *.obj
    -del *.res
    -del *.dll
    -del *.map
    -del *.sym
