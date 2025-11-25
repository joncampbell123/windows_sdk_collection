!include <win32.mak>

proj=gridmap

all: $(proj).exe

$(proj).exe: $(proj).obj $(proj).res
  $(link) $(ldebug) $(conflags) -out:$*.exe $**\
    user32.lib imm32.lib $(conlibs)

.c.obj:
  $(cc) $(cdebug) $(cflags) $(cvars) $*.c

.rc.res:
  $(rc) $(rcvars) -r $<
