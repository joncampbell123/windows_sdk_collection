echo off
if exist addmcadr.log copy *.log *.old
if exist addmcadr.log del addmcadr.log
if exist close.log copy *.log *.old
if exist close.log del close.log
if exist delmcadr.log copy *.log *.old
if exist delmcadr.log del delmcadr.log
if exist handle0.log copy *.log *.old
if exist handle0.log del handle0.log
if exist interupt.log copy *.log *.old
if exist interupt.log del interupt.log
if exist mactable.log copy *.log *.old
if exist mactable.log del mactable.log
if exist open.log copy *.log *.old
if exist open.log del open.log
if exist setpkfil.log copy *.log *.old
if exist setpkfil.log del setpkfil.log
if exist stnadr.log copy *.log *.old
if exist stnadr.log del stnadr.log
if exist initdiag.log copy *.log *.old
if exist initdiag.log del initdiag.log
if exist updstats.log copy *.log *.old
if exist updstats.log del updstats.log
if exist clrstats.log copy *.log *.old
if exist clrstats.log del clrstats.log
if exist reset.log copy *.log *.old
if exist reset.log del reset.log
if exist setlook.log copy *.log *.old
if exist setlook.log del setlook.log
if exist xmitchn.log copy *.log *.old
if exist xmitchn.log del xmitchn.log
if exist setfaddr.log copy *.log *.old
if exist setfaddr.log del setfaddr.log
if exist rderrlog.log copy *.log *.old
if exist rderrlog.log del rderrlog.log
if exist indicate.log copy *.log *.old
if exist indicate.log del indicate.log
del INITDIAG\*.log
del CLOSE\*.log
del MACTABLE\*.log
del OPEN\*.log
del SETPKFIL\*.log
del UPDSTATS\*.log
del CLRSTATS\*.log
del SETLOOK\*.log
del XMITCHN\*.log
del RESET\*.log
del SETFADDR\*.log
del ADDMCADR\*.log
del DELMCADR\*.log
del HANDLE0\*.log
del INTERUPT\*.log
del RDERRLOG\*.log
del INDICATE\*.log
del STNADR\*.log
echo on
