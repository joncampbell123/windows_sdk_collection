NAME = fastfile
EXT = lib

IS_32 = 1

GOALS = $(NAME).$(EXT) 
        
OBJS    = fastfile.obj
          
!if ("$(DEBUG)" == "debug") 
COPT =-YX -DDEBUG -Zi -Fd$(NAME).PDB
!else
COPT =-YX
!endif
DEF = $(NAME).def

CFLAGS  =$(COPT) -Oxa -D_X86_ $(CDEBUG) -Fo$@
LFLAGS  =$(LOPT)

!include ..\..\mssdk.mk

$(NAME).$(EXT): $(OBJS) 
        @$(LIBEXE) @<<
-name:$(NAME).$(EXT)
$(OBJS)
<<
