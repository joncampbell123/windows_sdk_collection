del *.log
del *.dbg

set wancard1=ndiswan3
set wancard2=ndiswan4
set wancard3=ndiswan5
set wancard4=ndiswan6

hapi -x -l0x000003ff scripts\wan\multsrv.tst
hapisum *.log
type hapi.sum

:END
