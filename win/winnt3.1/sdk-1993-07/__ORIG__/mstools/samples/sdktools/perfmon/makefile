!include <ntwin32.mak>

all: perfmon.exe

perfmon.res: perfmon.rc perfmon.h
    $(rc) -r -DUNICODE=1 -DSTRICT -DSECURITY -DADVANCED_PERFMON perfmon.rc

perfmon.rbj: perfmon.res
    $(cvtres) -$(CPU) perfmon.res -o perfmon.rbj

perfmon.exe: addline.obj   \
        addlog.obj    \
        alert.obj     \
        bookmark.obj  \
        command.obj   \
        counters.obj  \
        datasrc.obj   \
        dialogs.obj   \
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
        perfmon.obj  \
        perfmops.obj  \
        playback.obj  \
        registry.obj  \
        report.obj    \
        rptoptns.obj  \
        status.obj    \
        system.obj    \
        timefrm.obj   \
        timeline.obj  \
        toolbar.obj   \
        utils.obj     \
        valuebar.obj  \
        export.obj    \
        chosecom.obj  \
        perfmon.rbj
  $(link) $(linkdebug) $(guiflags) -out:$*.exe $** $(guilibs) shell32.lib comctl32.lib advapi32.lib largeint.lib

.c.obj:
  $(cc) $(cdebug) $(cflags) $(cvars) -DUNICODE=1 -DSTRICT -DSECURITY -DADVANCED_PERFMON $*.c


