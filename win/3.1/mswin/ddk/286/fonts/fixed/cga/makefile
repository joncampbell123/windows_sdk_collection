all:  cgafix.fon cgaoem.fon

### CGA System Only
cgafix.fon:  cgafix.FNT cgafix.DEF cgafix.RC cgafix.LNK makefile \
             cgafix.rc cgafix.rcv
        ..\..\link4 @cgafix.lnk
        rc -k cgafix.rc cgafix.fon
        del cgafix.res


### CGA US Oem Only
.\CGAOEM.fon:  CGAOEM.FNT CGAOEM.DEF CGAOEM.RC CGAOEM.LNK MAKEFILE \
               cgaoem.rc cgaoem.rcv
        ..\..\link4 @CGAOEM.lnk
        rc -k CGAOEM.rc cgaoem.fon
        del CGAOEM.res

