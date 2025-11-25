##### Module Macro #####
NAME  = an
SRCS  = $(NAME).c
OBJS  =

##### Library Macro #####
LIBS  = user32.lib gdi32.lib kernel32.lib libc.lib pkpd32.lib penwin32.lib  comdlg32.lib

##### Include Macro #####
INCLS = anres.h

##### Resource Macro #####
RCFILES  = $(NAME).rc

##### DEBUG Defined #####
DEBUG = 1

##### Build Option Macros #####
!if $(DEBUG)
DDEF  = -DDEBUG -DWIN32 -DJAPAN 
CLOPT = -Zi -Od
MOPT  = -Zi
LOPT  = /MAP /SUBSYSTEM:windows,4.0
!else
DDEF  = -DWIN32 -DJAPAN 
CLOPT = -Os
LOPT  = /SUBSYSTEM:windows,4.0
!endif

##### General Macros #####
KEEPFLAG = KEEP
DEF   =

##### Tool Macros #####
# (note: -C 932 is Japanese codepage)

ASM   = masm -Mx $(MOPT) $(DDEF) $(DEF)
CC    = cl -nologo -c $(MOD) -Zp -W3 $(CLOPT) $(DDEF) $(DEF)
LINK  = link -link /NOD $(LOPT)
RC    = rc $(DDEF) $(DEF) -C 932 
HC    = hc

##### Inference Rules #####
.c.obj:
   $(CC) $<

.asm.obj:
   $(ASM) $<;

##### Main (default) Target #####
goal : $(NAME)32.exe

##### Dependents For Goal and Command Line #####
$(NAME)32.exe : $(NAME)32.res $(NAME)32.def $(SRCS:.c=.obj)
   $(LINK) @<<$(NAME)32.lnk
   -out:$(NAME)32.exe
   -map:$(NAME)32.map
   -base:0x400000
   $(SRCS:.c=.obj) $(OBJS)
   $(LIBS)
   $(NAME)32.res
<<$(KEEPFLAG)
!if $(DEBUG)
   mapsym $(NAME)32.map
!endif

##### Dependents #####
$(SRCS:.c=.obj) : $(INCLS)

$(NAME)32.res : $(RCFILES) $(INCLS)
   $(RC) -r -fo$*.res $(NAME).rc

$(NAME)32.def : $(NAME).def
   $(CC) /EP /Tc$(NAME).def > $(NAME)32.def

##### Clean Directory #####
clean:
   -del *.obj
   -del *.res
   -del *.exe
   -del *.map
   -del *.sym
   -del *.lnk
   -del *.pdb
   -del $(NAME)32.def
