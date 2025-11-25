!include <ntwin32.mak>

OBJS = setup.obj setupapi.obj msregdb.obj msdetect.obj msshared.obj

all: sample.exe

!if "$(DEBUG)" == "yes"
cflags = $(cflags) $(cdebug) $(cvarsdll) -nologo -DDEBUG
lflags = $(ldebug)
!else
cflags = $(cflags) $(cvarsdll) -nologo
!endif


setup.rbj setup.res: setup.rc
	rc -r -fo setup.res setup.rc
	cvtres -$(CPU) setup.res -o setup.rbj

setup.obj: sample.h ..\include\setupapi.h setup.c
	$(cc) $(cflags) setup.c


setupapi.obj: ..\include\setupapi.h setupapi.c
	$(cc) $(cflags) setupapi.c

msregdb.obj: ..\include\msregdb.h ..\include\setupapi.h msregdb.c
	$(cc) $(cflags) msregdb.c

msdetect.obj: ..\include\msdetect.h ..\include\setupapi.h msdetect.c
	$(cc) $(cflags) msdetect.c

msshared.obj: ..\include\msshared.h ..\include\setupapi.h msshared.c
	$(cc) $(cflags) msshared.c


sampled.exe: setup.def setup.rbj $(OBJS)
	$(link)	$(ldebug) $(guilflags) -out:setupd.exe $(OBJS) \
	msuilstf.lib msdetstf.lib msinsstf.lib msshlstf.lib mscomstf.lib $(guilibsdll)

sample.exe: setup.def setup.rbj $(OBJS)
	$(link)	$(guilflags) -out:setup.exe $(OBJS) \
	msuilstf.lib msdetstf.lib msinsstf.lib msshlstf.lib mscomstf.lib $(guilibsdll)

clean:
	rm   	sample.exe
	rm   	sampled.exe
	rm   	sample.res
	rm   	$(OBJS)
