proj=DSShow

!if "$(nodebug)" == "1"
OBJ_DIR = Retail
!else
OBJ_DIR = Debug
!endif

!include "$(DXSDKROOT)\include\dxsdk.inc"

mycflags=

OBJ_LIST = $(OBJ_DIR)\debug.obj $(OBJ_DIR)\dsenum.obj $(OBJ_DIR)\shell.obj $(OBJ_DIR)\wassert.obj $(OBJ_DIR)\wave.obj

all: mkdir $(OBJ_DIR)\$(proj).exe

$(OBJ_DIR)\debug.obj:  debug.c
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\debug.obj debug.c

$(OBJ_DIR)\dsenum.obj:  dsenum.c
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\dsenum.obj dsenum.c

$(OBJ_DIR)\shell.obj:  shell.c
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\shell.obj shell.c

$(OBJ_DIR)\wassert.obj:  wassert.c
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\wassert.obj wassert.c

$(OBJ_DIR)\wave.obj:  wave.c
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\wave.obj wave.c

$(OBJ_DIR)\$(proj).res: $(proj).rc resource.h
   $(rc) $(rcflags) $(OBJ_DIR)\$(proj).res $(proj).rc

$(OBJ_DIR)\$(proj).exe: $(OBJ_LIST) $(OBJ_DIR)\$(proj).res
    $(link) $(linkdebug) $(linkflags) $(OBJ_LIST) $(borobjs) $(commas) $(outexe)$(OBJ_DIR)\$(proj).exe $(commas) $(commas) $(deflibs) ddraw.lib dsound.lib msacm32.lib $(commas) $(commas) $(OBJ_DIR)\$(proj).res

mkdir:
    @if not exist $(OBJ_DIR)\NUL md $(OBJ_DIR)
    
# Rules for cleaning out those old files
clean:
    -@echo y | del Retail
    -@echo y | del Debug
    -@rd Retail
    -@rd Debug
