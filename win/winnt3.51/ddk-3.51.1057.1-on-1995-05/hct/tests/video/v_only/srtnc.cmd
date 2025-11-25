mtrun rt /c "/I: /B: "
del rtnc.log
ren rt.log rtnc.log
copy rtnc.log %HCTDIR%\logs
