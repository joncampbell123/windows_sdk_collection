##### Module Macros ####
NAME	= clidemo
SRCS	= $(NAME).c dialog.c utility.c object.c register.c stream.c
OBJS	=

##### C7 Macro #####
C7	= 1

##### Library Macros #####
LIBS	= libw slibcew commdlg olecli shell
MOD	= -AS

##### Include Macro #####
INCLS	= $(NAME).h dialog.h utility.h object.h register.h global.h demorc.h

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
    $(CC) -NT _$* $*.c

.asm.obj:
    $(ASM) $*.asm;

.rc.res:
    $(RC) -r $*

##### Main (default) Target #####
goal: $(NAME).exe

##### Dependents For Goal and Command Line #####
$(NAME).obj: $(NAME).c
    $(CC) $*.c

$(NAME).exe: $(SRCS:.c=.obj) $(NAME).def $(NAME).res
    $(LINK) @<<
    $(SRCS:.c=.obj) $(OBJS),
    $(NAME).exe,
    $(NAME).map,
    $(LIBS),
    $(NAME).def
<<
    $(RC) -T $(NAME).res
!if $(DEBUG)
!if !$(C7)
    cvpack -p $(NAME).exe
!endif
    mapsym $(NAME).map
!endif

##### Dependents #####
$(SRCS:.c=.obj):  $(INCLS)
$(NAME).res: $(RCFILES)

##### Clean Directory #####
clean:
    -del *.obj
    -del *.res
    -del *.exe
    -del *.map
    -del *.sym
