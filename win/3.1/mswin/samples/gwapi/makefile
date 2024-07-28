##### Module Macros #####
NAME	= gwapi
SRCS	= $(NAME).c
OBJS	= libentry.obj

##### C7 Macro #####
C7	= 1

##### Library Macros #####
LIBS	= libw mdllcew penwin
MOD	= -AMw

##### Include Macro #####
INCLS	=

##### Resource Macro #####
RCFILES =

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

##### General Macro #####
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

##### Dependents For Goal and Command Lines #####
$(NAME).dll: $(SRCS:.c=.obj) $(NAME).def
    $(LINK) @<<
    $(SRCS:.c=.obj) $(OBJS),
    $(NAME).dll,
    $(NAME).map,
    $(LIBS),
    $(NAME).def
<<
    $(RC) -T $(NAME).dll
!if $(DEBUG)
!if !$(C7)
    cvpack -p $(NAME).dll
!endif
    mapsym $(NAME).map
!endif

##### Dependents #####
$(SRCS:.c=.obj): $(INCLS)

##### Clean Directory #####
clean:
    -del *.obj
    -del *.dll
    -del *.map
    -del *.sym
