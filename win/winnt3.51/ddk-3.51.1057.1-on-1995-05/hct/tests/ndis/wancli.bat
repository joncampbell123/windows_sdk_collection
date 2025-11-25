del *.log
del *.dbg

set wancard1=ndiswan3
set wancard2=ndiswan4

hapi -x -l0x000003ff scripts\wan\ndiswan
hapisum *.log
type hapi.sum

:END
