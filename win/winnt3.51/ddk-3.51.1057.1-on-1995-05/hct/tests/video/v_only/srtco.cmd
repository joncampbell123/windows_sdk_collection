mtrun rt /c "/CU: "
del rtco.log
ren rt.log rtco.log
copy rtco.log %HCTDIR%\logs
