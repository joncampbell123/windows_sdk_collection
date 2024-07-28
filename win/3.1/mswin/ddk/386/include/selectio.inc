;    (C) Copyright MICROSOFT Corp. 1986-1990
;
;    ************* MICROSOFT CONFIDENTIAL ******************
;
;
; Selection structure for selecting regions of old app Windows screens
;
TTYSelect	struc
    SelctSRect	DB	(size RECT) dup(?)
    GrabArea	DW	16 dup (0)
TTYSelect	ends
