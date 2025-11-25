!include <ntwin32.mak>

all: remote.exe

# Update the resources if necessary

remote.exe: client.obj  \
        server.obj  \
        remote.obj
  $(link) $(linkdebug) $(conflags) -out:$*.exe $** $(conlibs) advapi32.lib user32.lib

.c.obj:
  $(cc) $(cdebug) $(cflags) $(cvars) $*.c


