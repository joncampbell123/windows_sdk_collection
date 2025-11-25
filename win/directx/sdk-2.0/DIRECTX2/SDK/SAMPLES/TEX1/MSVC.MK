NAME = tex1
EXT = exe

GOALS = $(NAME).$(EXT)

LIBS    =kernel32.lib user32.lib advapi32.lib d3drm.lib ddraw.lib \
         comdlg32.lib gdi32.lib winmm.lib libc.lib

OBJS    =  tex1.obj rmmain.obj rmerror.obj
          
!if "$(DEBUG)" == "debug"
COPT =-YX -DDEBUG -DD3DRMDEMO -Zi -Fd$(NAME).PDB
LOPT =-debug:full -debugtype:cv -pdb:$(NAME).pdb
ROPT =-DDEBUG -DD3DRMDEMO
!else
COPT =-YX -Otyb1 -DD3DRMDEMO
LOPT =-debug:none
ROPT =-DD3DRMDEMO
!endif
DEF = $(NAME).def
RES = rmmain.res 

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
