TARGETOS = WINNT

!include <win32.mak>

all: dlgedit.exe

# Update the resources if necessary

dlgedit.res: dlgedit.rc dlgedit.h
    $(rc) $(rcflags) $(rcvars) dlgedit.rc

dlgedit.exe: addctrl.obj  \
        ctrlproc.obj \
        custcntl.obj \
        dialogs.obj  \
        dlgedit.obj  \
        drag.obj     \
        file.obj     \
        globals.obj  \
        groupdlg.obj \
        include.obj  \
        menu.obj     \
        reslist.obj  \
        restodlg.obj \
        resutil.obj  \
        rwdlg.obj    \
        rwinc.obj    \
        rwres.obj    \
        select.obj   \
        status.obj   \
        styles.obj   \
        toolbox.obj  \
        util.obj     \
        viewinc.obj  \
        dlgedit.res
  $(link) $(linkdebug) $(guiflags) -out:$*.exe $** $(guilibs)

.c.obj:
  $(cc) $(cdebug) $(cflags) $(cvars) -DUNICODE $*.c


