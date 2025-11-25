!include "..\win32.mak"

!if "$(nodebug)" == "1"
OBJ_DIR = Retail
!else
OBJ_DIR = Debug
!endif

proj=dsshow3d

OBJ_LIST = $(OBJ_DIR)\$(proj).obj $(OBJ_DIR)\debug.obj \
           $(OBJ_DIR)\lsnrinfo.obj $(OBJ_DIR)\wave.obj \
           $(OBJ_DIR)\mainwnd.obj $(OBJ_DIR)\fileinfo.obj \
           $(OBJ_DIR)\finfo3d.obj

all: mkdir $(OBJ_DIR)\$(proj).exe

$(OBJ_DIR)\$(proj).obj:  $(proj).cpp $(proj).h resource.h
    $(cc) $(cdebug) $(cflags) -Fo$(OBJ_DIR)\$(proj).obj $(cvars) $(proj).cpp
	
$(OBJ_DIR)\debug.obj:  debug.c debug.h
    $(cc) $(cdebug) $(cflags) -Fo$(OBJ_DIR)\debug.obj $(cvars) debug.c

$(OBJ_DIR)\lsnrinfo.obj:  lsnrinfo.cpp lsnrinfo.h
    $(cc) $(cdebug) $(cflags) -Fo$(OBJ_DIR)\lsnrinfo.obj $(cvars) lsnrinfo.cpp

$(OBJ_DIR)\wave.obj:  wave.c wave.h
    $(cc) $(cdebug) $(cflags) -Fo$(OBJ_DIR)\wave.obj $(cvars) wave.c

$(OBJ_DIR)\mainwnd.obj:  mainwnd.cpp mainwnd.h
    $(cc) $(cdebug) $(cflags) -Fo$(OBJ_DIR)\mainwnd.obj $(cvars) mainwnd.cpp

$(OBJ_DIR)\fileinfo.obj:  fileinfo.cpp fileinfo.h
    $(cc) $(cdebug) $(cflags) -Fo$(OBJ_DIR)\fileinfo.obj $(cvars) fileinfo.cpp

$(OBJ_DIR)\finfo3d.obj:  finfo3d.cpp finfo3d.h
    $(cc) $(cdebug) $(cflags) -Fo$(OBJ_DIR)\finfo3d.obj $(cvars) finfo3d.cpp

$(OBJ_DIR)\$(proj).res: $(proj).rc resource.h
   rc -r -fo $(OBJ_DIR)\$(proj).res $(proj).rc

$(OBJ_DIR)\$(proj).exe: $(OBJ_LIST) $(proj).def $(OBJ_DIR)\$(proj).res
    $(link) $(linkdebug) $(guiflags) -out:$(OBJ_DIR)\$(proj).exe \
           $(OBJ_LIST) $(OBJ_DIR)\$(proj).res \
           $(guilibs) comctl32.lib winmm.lib uuid.lib msacm32.lib ole32.lib \
           shell32.lib ddraw.lib dsound.lib

mkdir:
    if not exist $(OBJ_DIR)\NUL md $(OBJ_DIR)
    
# Rules for cleaning out those old files
clean:
    -echo y | del Retail
    -echo y | del Debug
    -rd Retail
    -rd Debug
