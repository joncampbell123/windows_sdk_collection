##### Module Macros #####
##### APP 1 #####
NAME	= client
SRCS	= $(NAME).c clidata.c clidde.c
OBJS	=

##### APP 2 #####
NAME2	= server
SRCS2	= $(NAME2).c servdata.c servdde.c
OBJS2	=

##### C7 #####
C7   =	 1

##### Library Macros #####
!if $(C7)
LIBS	= libw mlibcew oldnames
!else
LIBS	= libw mlibcew
!endif
MOD	= -AM

##### Include Macros #####
INCLS	= $(NAME).h
INCLS2	= $(NAME2).h

##### Resource Macros #####
RCFILES = $(NAME).dlg
RCFILES2=

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
    $(CC) -NT $* $*.c

.asm.obj:
    $(ASM) $*.asm;

.rc.res:
    $(RC) -r $*.rc

##### Main (default) Target #####
goal: $(NAME).exe $(NAME2).exe

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

$(NAME2).exe: $(SRCS2:.c=.obj) $(NAME2).def $(NAME2).res
    $(LINK) @<<
    $(SRCS2:.c=.obj) $(OBJS2),
    $(NAME2).exe,
    $(NAME2).map,
    $(LIBS),
    $(NAME2).def
<<
    $(RC) -T $(NAME2).res
!if $(DEBUG)
!if !$(C7)
    cvpack -p $(NAME2).exe
!endif
    mapsym $(NAME2).map
!endif

##### Dependents For Inference Rules #####
$(SRCS:.c=.obj): $(INCLS)
$(NAME).res: $(RCFILES)

$(SRCS2:.c=.obj): $(INCLS2)
$(NAME2).res: $(RCFILES2)

##### Clean Directory #####
clean:
    -del *.obj
    -del *.res
    -del *.exe
    -del *.map
    -del *.sym
