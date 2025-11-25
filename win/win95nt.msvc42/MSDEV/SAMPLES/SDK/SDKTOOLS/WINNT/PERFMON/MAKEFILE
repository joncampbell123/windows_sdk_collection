TARGETOS=WINNT
APPVER=3.51

!include <win32.mak>

all: perfmon.exe

perfmon.res: perfmon.rc perfmon.h
    $(rc) -r -DUNICODE=1 -DSTRICT -DSECURITY -DADVANCED_PERFMON perfmon.rc

perfmon.exe: addline.obj   \
        addlog.obj    \
        alert.obj     \
        bookmark.obj  \
        chosecom.obj  \
        command.obj   \
        counters.obj  \
        datasrc.obj   \
        dialogs.obj   \
        export.obj    \
        fileopen.obj  \
        filesave.obj  \
        fileutil.obj  \
        grafdata.obj  \
        grafdisp.obj  \
        graph.obj     \
        init.obj      \
        intrline.obj  \
        legend.obj    \
        line.obj      \
        log.obj       \
        logoptns.obj  \
        memory.obj    \
        perfdata.obj  \
        perfmon.obj   \
        perfmops.obj  \
        playback.obj  \
        registry.obj  \
        report.obj    \
        rptfct.obj    \
        rptoptns.obj  \
        status.obj    \
        system.obj    \
        timefrm.obj   \
        timeline.obj  \
        toolbar.obj   \
        utils.obj     \
        valuebar.obj  \
        perfmon.res
  $(link) $(linkdebug) $(guilflags) -out:$*.exe $** \
    $(guilibs) shell32.lib comctl32.lib advapi32.lib \
    $(int64lib)

.c.obj:
  $(cc) $(cdebug) $(cflags) $(cvars) \
    -DUNICODE=1 -D_UNICODE=1 -DSTRICT -DSECURITY \
    -DADVANCED_PERFMON -DWINVER=0x0400 $*.c


clean:
  del *.obj *.exe *.res
