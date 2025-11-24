!include <ntwin32.mak>

all: main.exe the_dll.dll

main.obj: main.c
    $(cc) $(cdebug) $(cflags) $(cvars) main.c

the_dll.rbj: the_dll.rc the_dll.bmp the_dll.cur the_dll.ico
    rc -r -fo the_dll.res the_dll.rc
    cvtres -$(CPU) the_dll.res -o the_dll.rbj

the_dll.dll: the_dll.def the_dll.rbj
    $(link)              \
    -dll                 \
    -out:the_dll.dll     \
    -subsystem:windows   \
    the_dll.rbj

main.exe: main.obj main.def
    $(link) $(linkdebug) $(guiflags) -out:main.exe main.obj $(guilibsdll)
