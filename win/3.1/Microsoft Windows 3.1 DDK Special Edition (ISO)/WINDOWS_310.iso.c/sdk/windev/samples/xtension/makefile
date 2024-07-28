##### Module Macros ####
NAME	= xtension
SRCS	= $(NAME).c
OBJS	= libentry.obj

##### C7 Macro #####
C7	= 1

##### Library Macros #####
LIBS	= libw sdllcew
MOD	= -ASw

##### Include Macro #####
INCLS	= $(NAME).h

##### Resource Macro #####
RCFILES = $(NAME).rc $(NAME).ico

##### DEBUG Macro Defined #####
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
CC	= cl -nologo -c -G2sw -Zp -W3 $(MOD) $(CLOPT) $(DDEF) $(DEF)
LINK	= link /NOD /NOE $(LOPT)
RC	= rc $(DDEF) $(DEF)
HC	= hc

##### Inference Rules #####
.c.obj:
    $(CC) $*.c

.rc.res:
    $(RC) -r $*.rc

.asm.obj:
    $(ASM) $*.asm;

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
    $(RC) -p -T $(NAME).res $(NAME).dll
!if $(DEBUG)
!if !$(C7)
    cvpack -p $(NAME).dll
!endif
    mapsym $(NAME).map
!endif

##### Dependents #####
$(SRCS:.c=.obj):  $(INCLS)
$(NAME).res: $(RCFILES) $(INCLS)

##### Clean Directory #####
clean:
    -del *.obj
    -del *.res
    -del *.dll
    -del *.map
    -del *.sym
