!include <ntwin32.mak>

all: maze.exe

# Update the resources if necessary

maze.res: maze.rc
    rc -DWIN32 -r maze.rc

maze.rbj: maze.res
    cvtres -$(CPU) maze.res -o maze.rbj

maze.exe: bitmap.obj draw.obj drones.obj initmaze.obj network.obj readsgrd.obj \
	mazewnd.obj mazedlg.obj scorewnd.obj textwnd.obj topwnd.obj maze.rbj \
        maze.obj
  $(link) $(linkdebug) $(guiflags) -out:$*.exe $** $(guilibs) winmm.lib advapi32.lib

.c.obj:
  $(cc) $(cdebug) $(cflags) $(cvars) $*.c
