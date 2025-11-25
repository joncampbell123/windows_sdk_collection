# Nmake macros for building Windows 32-Bit apps

TARGETOS=WINNT
APPVER=4.0

all:secclnt.exe secsvr.exe psmyprop.dll

!include <win32.mak>

cflags=$(cflags) -Gz -DREGISTER_PROXY_DLL

# --------------------------------------
# Security Sample Client:

secclnt.cpp: myprop.h

secclnt.obj: secclnt.cpp
    $(cc) $(cflags) $(cvarsmt) $(cdebug) -Zi -Fdsecclnt.pdb secclnt.cpp

secclnt.rc: secclnt.h

secclnt.res: secclnt.rc
    $(rc) $(rcvars) $(rflags) secclnt.rc

secclnt.exe: secclnt.obj secclnt.res
    $(link) $(lflags) -debug -pdb:secclnt.pdb -out:secclnt.exe secclnt.obj secclnt.res $(olelibs)


# --------------------------------------
# Security Sample Server (LocalServer, LocalService, and UI):

secsvr.cpp: myprop.h

secsvr.obj: secsvr.cpp
    $(cc) $(cflags) $(cvarsmt) $(cdebug) -Zi -Fdsecsvr.pdb secsvr.cpp

secsvr.rc: secsvr.h

secsvr.res: secsvr.rc
    $(rc) $(rcvars) $(rflags) secsvr.rc

secsvr.exe: secsvr.obj secsvr.res
    $(link) $(lflags) -debug -pdb:secsvr.pdb -out:secsvr.exe secsvr.obj secsvr.res $(olelibs)


# --------------------------------------
# Security Sample IMyProperties Proxy-Stub DLL:

myprop_p.c myprop_i.c dlldata.c myprop.h : myprop.idl
    midl /ms_ext /c_ext /Oi2 myprop.idl

myprop_p.obj: myprop_p.c
    $(cc) $(cflags) $(cvarsmt) $(cdebug) -Zi -Fdpsmyprop.pdb myprop_p.c

myprop_i.obj: myprop_i.c
    $(cc) $(cflags) $(cvarsmt) $(cdebug) -Zi -Fdpsmyprop.pdb myprop_i.c

dlldata.obj: dlldata.c
    $(cc) $(cflags) $(cvarsmt) $(cdebug) -Zi -Fdpsmyprop.pdb dlldata.c

psmyprop.dll: myprop_p.obj myprop_i.obj dlldata.obj psmyprop.def
    $(link) $(lflags) -dll -debug -pdb:psmyprop.pdb -def:psmyprop.def -out:psmyprop.dll myprop_p.obj myprop_i.obj dlldata.obj $(olelibs) rpcrt4.lib

clean:
    del *.exe
    del *.dll
    del *.obj
    del *.pdb
    del *.ilk
    del *.res
    del *.lib
    del *.exp
    del myprop.h
    del myprop_p.c
    del myprop_i.c
    del dlldata.c

