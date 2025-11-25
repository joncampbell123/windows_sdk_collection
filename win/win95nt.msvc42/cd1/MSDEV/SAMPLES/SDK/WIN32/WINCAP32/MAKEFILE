!include <ntwin32.mak>

all: dibapi32.dll wincap32.exe

# Update DIBAPI32.DLL
# Update the resources for DIBAPI32.DLL if necessary

OBJS = copy.obj dibutil.obj dllinit.obj errors.obj file.obj hook.obj print.obj

copy.obj: copy.c
  $(cc) $(cdebug) $(cflags) $(cvarsdll) $(scall) $*.c

dibuitl.obj: dibuitl.c
  $(cc) $(cdebug) $(cflags) $(cvarsdll) $(scall) $*.c

dllinit.obj: dllinit.c
  $(cc) $(cdebug) $(cflags) $(cvarsdll) $(scall) $*.c

errors.obj: errors.c
  $(cc) $(cdebug) $(cflags) $(cvarsdll) $(scall) $*.c

file.obj: file.c
  $(cc) $(cdebug) $(cflags) $(cvarsdll) $(scall) $*.c

hook.obj: hook.c
  $(cc) $(cdebug) $(cflags) $(cvarsdll) $(scall) $*.c

print.obj: print.c
  $(cc) $(cdebug) $(cflags) $(cvarsdll) $(scall) $*.c


dialogs.obj: dialogs.c
  $(cc) $(cdebug) $(cflags) $(cvarsdll) $(scall) $*.c

dlgopen.obj: dlgopen.c
  $(cc) $(cdebug) $(cflags) $(cvarsdll) $(scall) $*.c

wincap.obj: wincap.c
  $(cc) $(cdebug) $(cflags) $(cvarsdll) $(scall) $*.c

 
dibapi.res: dibapi.rc
    rc -r dibapi.rc

dibapi.rbj: dibapi.res
    cvtres -$(CPU) dibapi.res -o dibapi.rbj

dibapi32.lib: $(OBJS) dibapi.def
    $(implib) -machine:$(CPU) \
        -def:dibapi.def \
        $(OBJS) \
        -out:dibapi32.lib

dibapi32.dll: $(OBJS) dibapi.def dibapi.rbj dibapi32.lib
    $(link) $(linkdebug) $(dlllflags) \
    -base:0x1c000000 \
    -out:dibapi32.dll \
    dibapi.rbj dibapi32.exp $(OBJS) $(guilibsdll)

# Update WINCAP32.EXE
# Update the resources for WINCAP32.EXE if necessary

WINOBJS = dialogs.obj dlgopen.obj wincap.obj

wincap.res: wincap.rc wincap.h wincap.ico
    rc -r wincap.rc

wincap.rbj: wincap.res
    cvtres -$(CPU) wincap.res -o wincap.rbj

wincap32.exe: $(WINOBJS)  \
        wincap.rbj \
        dibapi32.lib
  $(link) $(linkdebug) $(guiflags) -out:$*.exe $(WINOBJS) dibapi32.lib wincap.rbj $(guilibsdll)

.c.obj:
  $(cc) $(cdebug) $(cflags) $(cvars) $(scall) $*.c
