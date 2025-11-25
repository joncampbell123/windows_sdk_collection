NAME = mid2strm
EXT = exe

GOALS = $(NAME).$(EXT)

LIBS    =kernel32.lib user32.lib advapi32.lib \
         comdlg32.lib gdi32.lib winmm.lib libc.lib

OBJS    =  mid2strm.obj
          
!if "$(DEBUG)" == "debug"
COPT =-YX -DDEBUG -Zi -Fd$(NAME).PDB
LOPT =-debug:full -debugtype:cv -pdb:$(NAME).pdb
ROPT =-DDEBUG
!else
COPT =-YX
LOPT =-debug:none
ROPT =
!endif
DEF = $(NAME).def

CFLAGS  =$(COPT) -Oxa -D_X86_ $(CDEBUG) -Fo$@
LFLAGS  =$(LOPT)
RCFLAGS =$(ROPT)

NOLOGO = 1

!include ..\..\mssdk.mk

$(NAME).$(EXT): \
        $(OBJS) ..\$(NAME).def
        @$(LINK) $(LFLAGS) @<<
-out:$(NAME).$(EXT)
-map:$(NAME).map
-machine:i386
-subsystem:console
-def:..\$(NAME).def
$(LIBS)
$(OBJS)
<<
