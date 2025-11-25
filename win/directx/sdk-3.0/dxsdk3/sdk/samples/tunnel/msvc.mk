NAME = tunnel
EXT = exe

GOALS = $(NAME).$(EXT)

LIBS    =kernel32.lib user32.lib advapi32.lib ddraw.lib \
         comdlg32.lib gdi32.lib winmm.lib libc.lib

OBJS    =  d3dapp.obj ddcalls.obj d3dcalls.obj texture.obj misc.obj lclib.obj d3dmain.obj stats.obj tunnel.obj d3dmath.obj
          
!if "$(DEBUG)" == "debug"
COPT =-DDEBUG -DD3DDEMO -Zi -Fd$(NAME).PDB
LOPT =-debug:full -debugtype:cv -pdb:$(NAME).pdb
ROPT =-DDEBUG -DD3DDEMO
!else
COPT =-Otyb1 -DD3DDEMO
LOPT =-debug:none
ROPT =-DD3DDEMO
!endif
DEF = $(NAME).def
RES = d3dmain.res

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
