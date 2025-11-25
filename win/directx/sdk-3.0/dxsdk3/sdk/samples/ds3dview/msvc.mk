NAME = ds3dview
EXT = exe

GOALS = $(NAME).$(EXT)

LIBS    =kernel32.lib user32.lib advapi32.lib d3drm.lib ddraw.lib\
         comdlg32.lib gdi32.lib winmm.lib libc.lib dsound.lib

OBJS  = viewer.obj file.obj rodcone.obj rlds3d.obj dsutil3d.obj
          
!if "$(DEBUG)" == "debug"
COPT =-DDEBUG -Zi -Fd$(NAME).PDB
LOPT =-debug:full -debugtype:cv -pdb:$(NAME).pdb
ROPT =-DDEBUG
!else
COPT =-Otyb1
LOPT =-debug:none
ROPT =
!endif
DEF = $(NAME).def
RES = viewer.res 

CFLAGS  =$(COPT) -D_X86_ $(CDEBUG) -DUSE_FLOAT -Fo$@
LFLAGS  =$(LOPT)
RCFLAGS =$(ROPT)

NOLOGO = 1

!include ..\..\d3dsdk.mk

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
