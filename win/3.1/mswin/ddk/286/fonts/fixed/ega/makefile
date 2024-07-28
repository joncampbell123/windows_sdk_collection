all:  egafix.fon egaoem.fon

### EGA System Only
.\egafix.fon:  egafix.FNT egafix.DEF egafix.RC egafix.LNK makefile \
               egafix.rc egafix.rcv
        ..\..\link4 @egafix.lnk
        rc -k egafix.rc egafix.fon
        del egafix.res

### EGA US Oem Only
.\EGAOEM.fon:  EGAOEM.FNT EGAOEM.DEF EGAOEM.LNK EGAOEM.RC MAKEFILE \
               egaoem.rc egaoem.rcv
        ..\..\link4 @EGAOEM.lnk
        rc -k EGAOEM.rc EGAOEM.fon
        del EGAOEM.res

