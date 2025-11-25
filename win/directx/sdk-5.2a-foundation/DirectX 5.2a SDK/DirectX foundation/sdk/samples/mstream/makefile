!include "..\win32.mak"

!if "$(nodebug)" == "1"
OBJ_DIR = Retail
!else
OBJ_DIR = Debug
!endif

proj=mstream

OBJ_LIST = $(OBJ_DIR)\$(proj).obj $(OBJ_DIR)\debug.obj \
           $(OBJ_DIR)\mstrconv.obj $(OBJ_DIR)\mstrhelp.obj

all: mkdir $(OBJ_DIR)\$(proj).exe

$(OBJ_DIR)\$(proj).obj:  $(proj).c resource.h
    $(cc) $(cdebug) $(cflags) -Fo$(OBJ_DIR)\$(proj).obj $(cvars) $(proj).c

$(OBJ_DIR)\debug.obj:  debug.c debug.h
    $(cc) $(cdebug) $(cflags) -Fo$(OBJ_DIR)\debug.obj $(cvars) debug.c

$(OBJ_DIR)\mstrconv.obj:  mstrconv.c 
    $(cc) $(debug) $(cflags) -Fo$(OBJ_DIR)\mstrconv.obj $(cvars) mstrconv.c

$(OBJ_DIR)\mstrhelp.obj:  mstrhelp.c 
    $(cc) $(debug) $(cflags) -Fo$(OBJ_DIR)\mstrhelp.obj $(cvars) mstrhelp.c
	
$(OBJ_DIR)\$(proj).res: $(proj).rc resource.h
   rc -r -fo $(OBJ_DIR)\$(proj).res $(proj).rc


$(OBJ_DIR)\$(proj).exe: $(OBJ_LIST) $(proj).def $(OBJ_DIR)\$(proj).res
    $(link) $(linkdebug) $(guiflags) -out:$(OBJ_DIR)\$(proj).exe \
          $(OBJ_LIST) $(OBJ_DIR)\$(proj).res \
          $(guilibs) ddraw.lib winmm.lib comctl32.lib

mkdir:
    if not exist $(OBJ_DIR)\NUL md $(OBJ_DIR)
    
# Rules for cleaning out those old files
clean:
    -echo y | del Retail
    -echo y | del Debug
    -rd Retail
    -rd Debug
