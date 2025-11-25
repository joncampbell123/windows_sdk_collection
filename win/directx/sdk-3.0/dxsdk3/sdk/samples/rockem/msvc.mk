NAME = rockem3d
EXT = exe

GOALS = $(NAME).$(EXT)

LIBS    =kernel32.lib user32.lib advapi32.lib d3drm.lib ddraw.lib \
         dsound.lib comdlg32.lib gdi32.lib winmm.lib libc.lib

OBJS    =  control.obj directx.obj midi.obj rm.obj winmain.obj
          
!if "$(DEBUG)" == "debug"
COPT =-YX -DDEBUG -Zi -Fd$(NAME).PDB
LOPT =-debug:full -debugtype:cv -pdb:$(NAME).pdb
ROPT =-DDEBUG
!else
COPT =-YX -Ox
LOPT =-debug:none
ROPT =
!endif
RES = rockem3d.res 

CFLAGS  =$(COPT) -D_X86_ $(CDEBUG) -DUSE_FLOAT -Fo$@
LFLAGS  =$(LOPT)
RCFLAGS =$(ROPT)

NOLOGO = 1

!include ..\d3dsdk.mk

$(NAME).$(EXT): \
        $(OBJS) $(RES)
        @$(LINK) $(LFLAGS) @<<
-out:$(NAME).$(EXT)
-map:$(NAME).map
-machine:i386
-subsystem:windows,4.0
$(LIBS)
$(RES)
$(OBJS)
<<
