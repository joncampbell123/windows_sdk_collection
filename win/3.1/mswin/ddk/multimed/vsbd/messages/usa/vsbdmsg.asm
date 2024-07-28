    page    , 132
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   vsbdmsg.asm
;
;   Copyright (c) 1991-1992 Microsoft Corporation.  All Rights Reserved.
;
;   Description:
;       This file contains the localizable messages for VSBD.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        .386

        .xlist
        include vmm.inc
        .list

VxD_DATA_SEG
BeginMsg

;
; This message is displayed when contention is noticed in a VM.
;
        public gszNoAccessMessage
gszNoAccessMessage  label   byte
        db  "Unable to play sound - the Sound Blaster is in use by another application.", 0

EndMsg
VxD_DATA_ENDS

END
