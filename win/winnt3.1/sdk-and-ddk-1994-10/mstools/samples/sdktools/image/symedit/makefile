!include <ntwin32.mak>

all: ..\bin\symedit.exe

.c.obj:
  $(cc) $(cdebug) $(cflags) $(cvars) -I..\include -I. $*.c

..\bin\symedit.exe:  cv.obj  \
         cvcommon.obj   \
         file.obj      \
         symedit.obj   \
         symcvt.obj   \
         symedit.rbj   \
	 symtocv.obj
  $(link) $(linkdebug) $(conflags) -out:$@ $** $(conlibs) ..\lib\imagehlp.lib user32.lib

symedit.res: symedit.rc strings.i
	$(rc) $(rcvars) -r -fo symedit.res $(cvars) symedit.rc 

symedit.rbj: symedit.res
	cvtres -$(CPU) symedit.res -o symedit.rbj

strings.i:
        $(cc) -E -DRESOURCES strings.c | findstr -v /C:"#" > strings.i
