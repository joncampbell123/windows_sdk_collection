NAME = dptest
EXT = exe

GOALS = $(NAME).$(EXT)

LIBS    =kernel32.lib user32.lib advapi32.lib \
     comdlg32.lib gdi32.lib winmm.lib msvcrt.lib dplay.lib

OBJS    =  cdlggame.obj cdlgsp.obj dptesdlg.obj dptest.obj cplist.obj
      
!if "$(DEBUG)" == "debug"
COPT =-YX -MD -D_WINDOWS -D_MBCS -DWIN32 -D_AFXDLL -DDEBUG -Zi -Fd$(NAME).PDB
LOPT =-debug:full -debugtype:cv -pdb:$(NAME).pdb
LIBS = $(LIBS) mfc30d.lib
ROPT =-DDEBUG
!else
COPT =-YX -MD -D_WINDOWS -D_MBCS -DWIN32 -D_AFXDLL 
LOPT =-debug:none 
LIBS = $(LIBS) mfc30.lib
ROPT =
!endif
DEF = $(NAME).def
RES = $(NAME).res

CFLAGS  =$(COPT) -Oxa -D_X86_ $(CDEBUG) -Fo$@
LFLAGS  =$(LOPT)
RCFLAGS =$(ROPT)

NOLOGO = 1

!include ..\..\mssdk.mk

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
