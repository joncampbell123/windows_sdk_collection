!include "..\win32.mak"

!if "$(nodebug)" == "1"
OBJ_DIR = Retail
!else
OBJ_DIR = Debug
!endif

proj=mid2strm

all: mkdir $(OBJ_DIR)\$(proj).exe

$(OBJ_DIR)\$(proj).obj:  $(proj).c
    $(cc) $(cdebug) $(cflags) -Fo$(OBJ_DIR)\$(proj).obj $(cvars) $(proj).c
	

$(OBJ_DIR)\$(proj).exe: $(OBJ_DIR)\$(proj).obj  $(proj).def
    $(link) $(linkdebug) $(conflags) -out:$(OBJ_DIR)\$(proj).exe \
           $(OBJ_DIR)\$(proj).obj \
           $(conlibs) ddraw.lib winmm.lib

mkdir:
    if not exist $(OBJ_DIR)\NUL md $(OBJ_DIR)
    
# Rules for cleaning out those old files
clean:
    -echo y | del Retail
    -echo y | del Debug
    -rd Retail
    -rd Debug
