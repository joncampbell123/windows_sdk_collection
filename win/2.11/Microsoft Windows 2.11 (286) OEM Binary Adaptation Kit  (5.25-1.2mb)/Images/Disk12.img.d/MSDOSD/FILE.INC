FINDBUF     STRUC
            DB      21 DUP (?)
FB_attr     DB      ?
FB_time     DW      ?
FB_date     DW      ?
FB_size     DD      ?
FB_name     DB      13 DUP (?)
            DB      ?
FINDBUF     ENDS

FCB         STRUC
FCB_drive   DB      ?
FCB_name    DB      8 DUP (?)
FCB_ext     DB      3 DUP (?)
FCB_curblk  DW      ?
FCB_recsize DW      ?
FCB_fsize   DD      ?
FCB_date    DW      ?
FCB_time    DW      ?
            DB      8 DUP (?)
FCB_currec  DB      ?
FCB_relrec  DW      ?
FCB         ENDS

XFCB        STRUC
XFCB_flag   DB      ?
            DB      5 DUP (?)
XFCB_attr   DB      ?
XFCB        ENDS
