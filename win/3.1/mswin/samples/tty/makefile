##### Module Macro #####
NAME	= tty
SRCS	= $(NAME).c
OBJS	=

##### C7 Macro #####
C7	= 1

##### Library Macro #####
LIBS	= libw mlibcew commdlg
MOD	= -AM

##### Include Macro #####
INCLS	= $(NAME).h resource.h version.h

##### Resource Macro #####
RCFILES = $(NAME).ico $(NAME).rc $(NAME).bmp

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
goal: $(NAME).exe

##### Dependents For Goal and Command Line #####
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

##### Dependents For Inference Rules #####
$(SRCS:.c=.obj): $(INCLS)
$(NAME).res: $(INCLS) $(RCFILES)

##### Clean Directory #####
clean:
    -del *.obj
    -del *.res
    -del *.exe
    -del *.map
    -del *.sym
