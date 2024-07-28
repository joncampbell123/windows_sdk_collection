##### Module Macro #####
NAME	= makeapp
SRCS	= app.c frame.c client.c dlg.c
OBJS	=

##### Include Macro #####
INCLS	= $(NAME).h app.h frame.h client.h dlg.h dlgdefs.h menu.h

##### C7 Macro #####
C7	= 1

##### Library Macro #####
LIBS	= libw slibcew commdlg
MOD	= -AS

##### Resource Macro #####
RCFILES = $(NAME).rc $(NAME).ico $(NAME).dlg $(NAME).ver

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
DEF     = -DSTRICT

##### Tool Macros #####
ASM	= masm -Mx $(MOPT) $(DDEF) $(DEF)
CC	= cl -nologo -c $(MOD) -G2sw -Zp -W3 $(CLOPT) $(DDEF) $(DEF)
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
goal: $(NAME).exe

##### Dependents For Goal and Command Line #####
$(NAME).exe: $(SRCS:.c=.obj) $(NAME).def $(NAME).res $(OBJS)
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
$(SRCS:.c=.obj): $(INCLS)
$(NAME).res: $(INCLS) $(RCFILES)

##### Clean Directory #####
clean:
    -del *.obj
    -del $(NAME).exe
    -del $(NAME).res
    -del $(NAME).sym
    -del $(NAME).map
