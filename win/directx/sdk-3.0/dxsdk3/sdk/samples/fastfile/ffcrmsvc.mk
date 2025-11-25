NAME = ffcreate
EXT = exe

IS_32 = 1

GOALS = $(NAME).$(EXT)

LIBS    = libc.lib kernel32.lib

OBJS    = ffcreate.obj
          
!if "$(DEBUG)" == "debug"
COPT =-YX -DDEBUG -Zi -Fd$(NAME).PDB
LOPT =-debug:full -debugtype:cv -pdb:$(NAME).pdb
!else
COPT =-YX
LOPT =-debug:none
!endif
DEF = $(NAME).def

CFLAGS  =$(COPT) -Oxs -D_X86_ $(CDEBUG) -Fo$@
LFLAGS  =$(LOPT)

!include ..\..\mssdk.mk

$(NAME).$(EXT): \
        $(OBJS) ..\$(NAME).def 
        @$(LINK) $(LFLAGS) @<<
-out:$(NAME).$(EXT)
-map:$(NAME).map
-def:..\$(NAME).def
-machine:i386
-subsystem:console
$(LIBS)
$(OBJS)
<<
