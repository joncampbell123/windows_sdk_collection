NAME = Flipcube
TARGET = Flip2D
EXT = exe

GOALS = $(TARGET).$(EXT)

LIBS    =kernel32.lib user32.lib advapi32.lib ddraw.lib \
         comdlg32.lib gdi32.lib winmm.lib libc.lib dinput.lib

OBJS    =  flipcube.obj dumb3d.obj tri.obj
          
!if "$(DEBUG)" == "debug"
COPT =-YX -DDEBUG -Zi -Fd$(NAME).PDB
LOPT =-debug:full -debugtype:cv -pdb:$(NAME).pdb
ROPT =-DDEBUG
!else
COPT =-YX
LOPT =-debug:none
ROPT =
!endif
RES = $(NAME).res 

CFLAGS  =$(COPT) -Oxa -D_X86_ $(CDEBUG) -Fo$@
LFLAGS  =$(LOPT)
RCFLAGS =$(ROPT)

NOLOGO = 1

!include ..\..\mssdk.mk

$(TARGET).$(EXT): \
        $(OBJS) $(RES)
        @$(LINK) $(LFLAGS) @<<
-out:$(TARGET).$(EXT)
-map:$(TARGET).map
-machine:i386
-subsystem:windows,4.0
$(LIBS)
$(RES)
$(OBJS)
<<
