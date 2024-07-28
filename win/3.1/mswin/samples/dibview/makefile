##### Module Macro #####
NAME	= dibview
SRCS	= $(NAME).c dib.c errors.c file.c paint.c init.c capture.c frame.c child.c palette.c print.c options.c clipbrd.c about.c
OBJS	=

##### C7/Display Compatibility Test #####
C7	=  1
DCT	=  0

##### Library Macro #####
!if $(C7)
LIBS	= libw mlibcew commdlg oldnames
!else
LIBS	= libw mlibcew commdlg
!endif
MOD	= -AM

##### Include Macro #####
INCLS	= $(NAME).h dibview.h dib.h errors.h file.h paint.h init.h capture.h frame.h child.h palette.h print.h options.h about.h

##### Resource Macro #####
RCFILES = options.dlg  select.cur

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

##### DCT #####
!if $(DCT)
CC	= $(CC) -DDCT
LIBS	= $(LIBS) hctlib
!endif

##### Inference Rules #####
.c.obj:
    $(CC) -NT _$* $*.c

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

##### Dependents #####
$(SRCS:.c=.obj): $(INCLS)
$(NAME).res: $(RCFILES) $(INCLS)

##### Clean Directory #####
clean:
    -del *.obj
    -del *.res
    -del *.exe
    -del *.map
    -del *.sym
