# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: pwalk.exe

# object files for pwalk.exe
pstat.obj : pstat.c pwalk.h
    $(cc) $(cflags) $(cvars) $(cdebug) pstat.c

pdebug.obj : pdebug.c pwalk.h
    $(cc) $(cflags) $(cvars) $(cdebug) pdebug.c

pwalkio.obj : pwalkio.c pwalk.h
    $(cc) $(cflags) $(cvars) $(cdebug) pwalkio.c

pwalk.obj : pwalk.c pwalk.h
    $(cc) $(cflags) $(cvars) $(cdebug) pwalk.c

pview.obj : pview.c pwalk.h
    $(cc) $(cflags) $(cvars) $(cdebug) pview.c


# object files for pefile.dll
pefile.obj : pefile.c pefile.h
    $(cc) $(cflags) $(cvars) $(cdebug) pefile.c


# object files for probe.dll
probe.obj: probe.c pwalk.h
    $(cc) $(cflags) $(cvarsdll) $(cdebug) probe.c


# resrouces for pwalk.exe
pwalk.res: pwalk.rc pwalk.h pwalk.ico
    rc -r pwalk.rc

pwalk.rbj: pwalk.res
    cvtres -$(CPU) pwalk.res -o pwalk.rbj


# resrouces for probe.dll
probe.res: probe.rc pwalk.h
    rc -r probe.rc

probe.rbj: probe.res
    cvtres -$(CPU) probe.res -o probe.rbj


# resrouces for pefile.dll
pefile.res: pefile.rc pefile.h
    rc -r pefile.rc

pefile.rbj: pefile.res
    cvtres -$(CPU) pefile.res -o pefile.rbj


# export library for probe.dll
probe.lib: probe.obj probe.def probe.rbj
    lib32 -machine:$(CPU)	 \
    -def:probe.def		 \
    probe.obj			 \
    -out:probe.lib


# export library for pefile.dll
pefile.lib: pefile.obj pefile.def pefile.rbj
    lib32 -machine:$(CPU)	 \
    -def:pefile.def		 \
    pefile.obj			 \
    -out:pefile.lib


# link probe.dll
probe.dll: probe.obj probe.def probe.lib pefile.lib pefile.dll
    $(link) $(linkdebug)	 \
    -base:0x1C000000		 \
    -dll			 \
    -entry:_DllMainCRTStartup$(DLLENTRY)	 \
    -out:probe.dll		 \
    probe.exp pefile.lib probe.obj probe.rbj $(guilibsdll)


# link pefile.dll
pefile.dll: pefile.obj pefile.def pefile.lib
    $(link) $(linkdebug)	 \
    -base:0x1D000000		 \
    -dll			 \
    -entry:_DllMainCRTStartup$(DLLENTRY)	 \
    -out:pefile.dll		 \
    pefile.exp pefile.obj pefile.rbj $(guilibsdll)


# link pwalk.exe
pwalk.exe: pwalk.rbj pwalk.obj probe.lib probe.dll pefile.lib pefile.dll \
	   pwalkio.obj pdebug.obj pstat.obj pview.obj
    $(link) $(linkdebug) $(lflags)	 \
    $(guiflags) 		 \
    -out:pwalk.exe		 \
    pwalk.obj pdebug.obj probe.lib pefile.lib pstat.obj pwalkio.obj pview.obj pwalk.rbj $(guilibs)
