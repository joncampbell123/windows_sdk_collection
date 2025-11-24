!include <ntwin32.mak>


all: readdata.exe chgdata.exe


readdata.obj: readdata.c shrstuff.h
    $(cc) $(cdebug) $(cflags) $(cvarsmt) -Zp1 readdata.c

chgdata.obj: chgdata.c shrstuff.h
    $(cc) $(cdebug) $(cflags) $(cvarsmt) -Zp1 chgdata.c

readdata.exe: readdata.obj
    $(link) $(linkdebug) $(conflags) -out:readdata.exe readdata.obj $(conlibsmt)

chgdata.exe: chgdata.obj
    $(link) $(linkdebug) $(conflags) -out:chgdata.exe chgdata.obj $(conlibsmt)
