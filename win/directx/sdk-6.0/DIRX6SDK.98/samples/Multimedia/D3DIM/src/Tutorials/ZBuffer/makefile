proj=zbuffer

!if "$(nodebug)" == "1"
OBJ_DIR = Retail
!else
OBJ_DIR = Debug
!endif


!IFDEF BORLAND
cc = bcc32
rc = brc32
link = ilink32
bldlib = tlib
cflags = -c -3 -j1 -X -Vd -I$(INCLUDE)
rcflags = -r -fo
linkflags = -n -V4.0 -aa 
linkflags = $(linkflags) -Tpe
borobjs=c0w32.obj
linkflags = $(linkflags) -j$(LIB) -L$(LIB)
libflags=$(OBJ_DIR)\$(proj).lib
outobj = -o
commas =,
outexe=
borlib=+
deflibs = import32.lib cw32mti.lib ole2w32.lib
!IFDEF nodebug
cdebug = -Od
linkdebug = /x
cflags = $(cflags) /DNDEBUG;
!ELSE
cdebug = -Od -v -y
linkdebug = /v
cflags = $(cflags) /DDEBUG;
!ENDIF

!ELSE
cc = cl
rc = rc
link = link
bldlib = lib
deflibs = kernel32.lib gdi32.lib user32.lib advapi32.lib gdi32.lib comdlg32.lib winspool.lib shell32.lib winmm.lib comctl32.lib ole32.lib oleaut32.lib
cflags = -c /D "WIN32" /D "_WINDOWS"

!IFDEF NODEBUG
cflags =$(cflags) /D "NDEBUG"
cdebug = -Ox
cflags =$(cflags) /ML
!ELSE
cflags =$(cflags) /D "DEBUG"
cdebug = -Z7 -Ox
cflags =$(cflags) /MLd
!ENDIF
rcflags = -r -fo
outobj = -Fo
commas=
outexe=-out:
borobjs=
borlib=

!IFDEF WATCOM

!IFDEF NODEBUG
linkdebug =
!ELSE
linkdebug = -map -debug:full
!ENDIF
linkflags = /INCREMENTAL:NO /NOLOGO
linkflags = $(linkflags) /SUBSYSTEM:WINDOWS
libflags=/OUT:$(OBJ_DIR)\$(proj).lib

!ELSE
!IFDEF NODEBUG
linkdebug = /RELEASE
linkflags=
!ELSE
linkdebug = -map -debug:full -debugtype:cv
linkflags = /NODEFAULTLIB:LIBC
!ENDIF
cflags =$(cflags) 
linkflags = $(linkflags) /INCREMENTAL:NO /PDB:NONE /NOLOGO
linkflags = $(linkflags) /SUBSYSTEM:WINDOWS
libflags=/OUT:$(OBJ_DIR)\$(proj).lib /SUBSYSTEM:WINDOWS
!ENDIF
!ENDIF

OBJ_LIST = $(OBJ_DIR)\$(proj).obj $(OBJ_DIR)\winmain.obj

all: mkdir $(OBJ_DIR)\$(proj).exe

$(OBJ_DIR)\$(proj).obj:  $(proj).cpp
    $(cc) $(cdebug) $(cflags) $(outobj)$(OBJ_DIR)\$(proj).obj $(proj).cpp

$(OBJ_DIR)\winmain.obj:  winmain.cpp
    $(cc) $(cdebug) $(cflags) $(outobj)$(OBJ_DIR)\winmain.obj winmain.cpp

$(OBJ_DIR)\winmain.res: winmain.rc resource.h
   $(rc) $(rcflags) $(OBJ_DIR)\winmain.res winmain.rc

$(OBJ_DIR)\$(proj).exe: $(OBJ_LIST) $(OBJ_DIR)\winmain.res
    $(link) $(linkdebug) $(linkflags) $(OBJ_LIST) $(borobjs) $(commas) $(outexe)$(OBJ_DIR)\$(proj).exe $(commas) $(commas) $(deflibs) ddraw.lib dxguid.lib $(commas) $(commas) $(OBJ_DIR)\winmain.res

mkdir:
    @if not exist $(OBJ_DIR)\NUL md $(OBJ_DIR)
    
# Rules for cleaning out those old files
clean:
    -@echo y | del Retail
    -@echo y | del Debug
    -@rd Retail
    -@rd Debug
