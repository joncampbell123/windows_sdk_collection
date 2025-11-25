NAME = mstream
EXT = exe
GLOBAL_RECOMPILE = $(MANROOT)\recompdd.log

IS_32 = 1

GOALS = $(PBIN)\$(NAME).$(EXT)

LIBS    =kernel32.lib user32.lib crtdll.lib comctl32.lib comdlg32.lib \
         gdi32.lib winmm.lib

OBJS  =  mstream.obj debug.obj mstrconv.obj mstrhelp.obj
      
!if "$(DEBUG)" == "debug"
COPT =-YX -DDEBUG -Zi -Fd$(NAME).PDB
AOPT =-DDEBUG
LOPT =-debug:full -debugtype:cv -pdb:$(NAME).pdb
ROPT =-DDEBUG
!else
COPT =-YX
AOPT =
LOPT =-debug:none
ROPT =
!endif
DEF = $(NAME).def
RES = $(NAME).res 

CFLAGS  =$(COPT) -Oxa -D_X86_ $(CDEBUG) -Fo$@
LFLAGS  =$(LOPT)
RCFLAGS =$(ROPT)

NOLOGO = 1

!include ..\..\..\proj.mk

$(NAME).$(EXT): \
    $(OBJS) ..\$(NAME).def $(RES)
    @$(LINK) $(LFLAGS) @<<
-out:$(NAME).$(EXT)
-map:$(NAME).map
-machine:i386
-subsystem:windows,4.0
-def:..\$(NAME).def
$(LIBS)
$(RES)
$(OBJS)
<<
