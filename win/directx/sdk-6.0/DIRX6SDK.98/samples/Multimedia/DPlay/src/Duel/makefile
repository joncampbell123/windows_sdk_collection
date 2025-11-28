proj=Duel

!if "$(nodebug)" == "1"
OBJ_DIR = Retail
!else
OBJ_DIR = Debug
!endif

!include "$(DXSDKROOT)\include\dxsdk.inc"

mycflags=

OBJ_LIST = $(OBJ_DIR)\duel.obj $(OBJ_DIR)\comm.obj $(OBJ_DIR)\ddutil.obj $(OBJ_DIR)\ds3dutil.obj \
	$(OBJ_DIR)\gameproc.obj $(OBJ_DIR)\gfx.obj $(OBJ_DIR)\input.obj $(OBJ_DIR)\lobby.obj \
	$(OBJ_DIR)\sfx.obj $(OBJ_DIR)\util.obj $(OBJ_DIR)\wizard.obj

all: mkdir $(OBJ_DIR)\$(proj).exe

$(OBJ_DIR)\duel.obj:  duel.c
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\duel.obj duel.c

$(OBJ_DIR)\comm.obj:  comm.c
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\comm.obj comm.c

$(OBJ_DIR)\ddutil.obj:  ddutil.cpp
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\ddutil.obj ddutil.cpp

$(OBJ_DIR)\ds3dutil.obj:  ds3dutil.c
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\ds3dutil.obj ds3dutil.c

$(OBJ_DIR)\gameproc.obj:  gameproc.c
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\gameproc.obj gameproc.c

$(OBJ_DIR)\gfx.obj:  gfx.c
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\gfx.obj gfx.c

$(OBJ_DIR)\input.obj:  input.c
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\input.obj input.c

$(OBJ_DIR)\lobby.obj:  lobby.c
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\lobby.obj lobby.c

$(OBJ_DIR)\sfx.obj:  sfx.c
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\sfx.obj sfx.c

$(OBJ_DIR)\util.obj:  util.c
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\util.obj util.c

$(OBJ_DIR)\wizard.obj:  wizard.c
    $(cc) $(cdebug) $(cflags) $(mycflags) $(outobj)$(OBJ_DIR)\wizard.obj wizard.c

$(OBJ_DIR)\$(proj).res: duel.rc resource.h
   $(rc) $(rcflags) $(OBJ_DIR)\$(proj).res duel.rc

$(OBJ_DIR)\$(proj).exe: $(OBJ_LIST) $(OBJ_DIR)\$(proj).res
    $(link) $(linkdebug) $(linkflags) $(OBJ_LIST) $(borobjs) $(commas) $(outexe)$(OBJ_DIR)\$(proj).exe $(commas) $(commas) $(deflibs) comctl32.lib ddraw.lib dinput.lib dplayx.lib dsound.lib $(commas) $(commas) $(OBJ_DIR)\$(proj).res

mkdir:
    @if not exist $(OBJ_DIR)\NUL md $(OBJ_DIR)
    
# Rules for cleaning out those old files
clean:
    -@echo y | del Retail
    -@echo y | del Debug
    -@rd Retail
    -@rd Debug
