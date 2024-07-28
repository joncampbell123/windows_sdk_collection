all:  8514fix.fon 8514oem.fon

### 8514 System Only
8514fix.fon:  8514fix.FNT 8514fix.DEF 8514fix.RC 8514fix.LNK makefile \
              8514fix.rc 8514fix.rcv
        ..\..\link4 @8514fix.lnk
        rc -k 8514fix.rc 8514fix.fon
        del 8514fix.res

### 8514 US Oem Only
8514OEM.fon:  8514OEM.FNT 8514OEM.DEF 8514OEM.RC 8514OEM.LNK MAKEFILE \
              8514oem.rc 8514oem.rcv
        ..\..\link4 @8514OEM.lnk
        rc -k 8514OEM.rc 8514OEM.fon
        del 8514OEM.res

