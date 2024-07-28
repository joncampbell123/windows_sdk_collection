        page    60, 132
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   midia.asm
;
;   Copyright (c) 1991-1992 Microsoft Corporation.  All rights reserved.
;
;   General Description:
;      Contains midi support routines that don't need to be fixed.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        .286

        .xlist
        include cmacros.inc                   
        include windows.inc
        include mmsystem.inc
        include mmddk.inc
        include sndblst.inc
        .list

        ?PLM=1                          ; Pascal calling convention
        ?WIN=0                          ; NO! Windows prolog/epilog code

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   extrn declarations
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 

        externNP <midSendPartBuffer>    ; midiin.c
        externFP <dspWrite>             ; sndblst.asm
        externFP <dspReset>             ; sndblst.asm
        externFP <dspSpeakerOn>         ; wavea.asm
       
        externFP <vsbdAcquireSoundBlaster>
        externFP <vsbdReleaseSoundBlaster>

        externB <gbIntUsed>             ; sndblst.asm

ifdef DEBUG
        externD <_gdwDebugMODWriteErrors>
        externD <_gdwDebugMODataWrites>
        externD <_gdwDebugMOShortMsgs>
        externD <_gdwDebugMOShortMsgsRS>
        externD <_gdwDebugMOShortMsgsBogus>
        externD <_gdwDebugMOLongMsgs>

        externD <_gdwDebugMIBytesRcvd>
        externD <_gdwDebugMIShortMsgsRcvd>
        externD <_gdwDebugMILongMsgsRcvd>
        externW <_gwDebugMILongErrors>
        externW <_gwDebugMIShortErrors>

        externD <_gdwDebugMidiDrvCallbacks>
endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   segmentation
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

IFNDEF SEGNAME
        SEGNAME equ <_TEXT>
ENDIF

createSeg %SEGNAME, CodeSeg, word, public, CODE

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   data segment
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

sBegin Data

        globalB gbMidiInFlags,      0
        globalB gbMidiOutFlags,     0

ifdef DEBUG
        globalW gwDebugQueueOF,     0
        globalW gwDebugInQueue,     0
        globalW gwDebugMaxInQueue,  0
        globalW gwDebugQueueKick,   0
endif

sEnd Data

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   code segment
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

sBegin CodeSeg

        assumes cs, CodeSeg
        assumes ds, Data

ifdef DEBUG
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   DebugMidi
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, nothing
        assumes es, nothing

        public DebugMidi
DebugMidi proc far

        pushf
        pusha
        push    es
        push    ds

        mov     ax, DGROUP              ; set up local DS
        mov     ds, ax
        assumes ds, Data

        D1      < >
        D1      <MIDI--Debug Dump-O-Matic>
        D1      <-------------------------------------------------------------------->

        D1      <MIDI Output:>
        mov     ax, [_gdwDebugMODWriteErrors].lo
        mov     bx, [_gdwDebugMODWriteErrors].hi
        mov     cx, [_gdwDebugMODataWrites].lo
        mov     dx, [_gdwDebugMODataWrites].hi
        D1      <  gdwDebugMODWriteErrors:#BX#AXh     gdwDebugMODataWrites:#DX#CXh>

        mov     ax, [_gdwDebugMOShortMsgs].lo
        mov     bx, [_gdwDebugMOShortMsgs].hi
        mov     cx, [_gdwDebugMOShortMsgsRS].lo
        mov     dx, [_gdwDebugMOShortMsgsRS].hi
        D1      <     gdwDebugMOShortMsgs:#BX#AXh    gdwDebugMOShortMsgsRS:#DX#CXh>

        mov     ax, [_gdwDebugMOShortMsgsBogus].lo
        mov     bx, [_gdwDebugMOShortMsgsBogus].hi
        mov     cx, [_gdwDebugMOLongMsgs].lo
        mov     dx, [_gdwDebugMOLongMsgs].hi
        D1      <gdwDebugMOShortMsgsBogus:#BX#AXh       gdwDebugMOLongMsgs:#DX#CXh>

        D1      < >
        D1      <MIDI Input:>
        mov     ax, [_gdwDebugMIBytesRcvd].lo
        mov     bx, [_gdwDebugMIBytesRcvd].hi
        mov     cx, [_gdwDebugMIShortMsgsRcvd].lo
        mov     dx, [_gdwDebugMIShortMsgsRcvd].hi
        D1      <     gdwDebugMIBytesRcvd:#BX#AXh  gdwDebugMIShortMsgsRcvd:#DX#CXh>

        mov     ax, [_gdwDebugMILongMsgsRcvd].lo
        mov     bx, [_gdwDebugMILongMsgsRcvd].hi
        mov     cx, [_gdwDebugMidiDrvCallbacks].lo
        mov     dx, [_gdwDebugMidiDrvCallbacks].hi
        D1      <  gdwDebugMILongMsgsRcvd:#BX#AXh gdwDebugMidiDrvCallbacks:#DX#CXh>

        mov     ax, [_gwDebugMILongErrors]
        mov     bx, [_gwDebugMIShortErrors]
        D1      <     gwDebugMILongErrors:#AXh         gwDebugMIShortErrors:#BXh>

        D1      < >
        D1      <MIDI Queue Stuff:>
        mov     ax, [gwDebugQueueOF]
        mov     bx, [gwDebugMaxInQueue]
        D1      <          gwDebugQueueOF:#AXh            gwDebugMaxInQueue:#BXh>

        mov     ax, [gwDebugQueueKick]
        D1      <        gwDebugQueueKick:#AXh>

        D1      < >

        pop     ds
        assumes ds, nothing
        pop     es
        popa
        popf
        ret

DebugMidi endp
endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   WORD FAR PASCAL modAcquireHardware(void)
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;       IF success
;           AX = 0, go ahead and open
;       ELSE
;           AX = non-zero error code:
;
;   USES:
;       Flags, AX, BX, DX
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

cProc modAcquireHardware <FAR, PASCAL, PUBLIC> <>
cBegin

        D2 <modAcquireHardware>

        mov     ax, -1                      ; assume bad news
        test    [gbMidiOutFlags], MOF_ALLOCATED
        jnz     mod_AH_Exit                 ; Q: is the hardware available?

        D2 <acquire>

        cCall   vsbdAcquireSoundBlaster
        or      ax, ax
        jnz     mod_AH_Exit

        or      [gbMidiOutFlags], MOF_ALLOCATED
        xor     ax, ax                      ; success

mod_AH_Exit:

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   WORD FAR PASCAL modReleaseHardware(void)
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;       IF success
;           AX = 0, go ahead and close
;       ELSE
;           AX = non-zero error code:
;
;   USES:
;       Flags, AX, BX, DX
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

cProc modReleaseHardware <FAR, PASCAL, PUBLIC> <>
cBegin

        D2 <modReleaseHardware>

        mov     ax, -1                      ; assume bad news
        test    [gbMidiOutFlags], MOF_ALLOCATED
        jz      mod_RH_Exit                 ; exit if not allocated


        D2 <release>

        cCall   vsbdReleaseSoundBlaster

        and     [gbMidiOutFlags], not MOF_ALLOCATED
        xor     ax, ax                      ; success

mod_RH_Exit:

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   WORD FAR PASCAL midAcquireHardware(void)
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;       IF success
;           AX = 0, go ahead and open
;       ELSE
;           AX = non-zero error code:
;
;   USES:
;       Flags, AX, BX, DX
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

cProc midAcquireHardware <FAR, PASCAL, PUBLIC> <>
cBegin

        D2 <midAcquireHardware>

        mov     ax, -1                      ; assume bad news
        test    [gbIntUsed], al             ; Q: is the hardware available?
        jnz     mid_AH_Exit

        test    [gbMidiInFlags], MIF_ALLOCATED
        jnz     mid_AH_Exit                 ; Q: is the hardware available?

        D2 <acquire>

        cCall   vsbdAcquireSoundBlaster
        or      ax, ax
        jnz     mid_AH_Exit
        
        or      [gbMidiInFlags], MIF_ALLOCATED
        mov     [gbIntUsed], INT_MIDIIN     ; allocate it...
        xor     ax, ax                      ; success

mid_AH_Exit:

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   WORD FAR PASCAL midReleaseHardware(void)
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;       IF success
;           AX = 0, go ahead and close
;       ELSE
;           AX = non-zero error code:
;
;   USES:
;       Flags, AX, BX, DX
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

cProc midReleaseHardware <FAR, PASCAL, PUBLIC> <>
cBegin

        D2 <midReleaseHardware>

        mov     ax, -1                      ; assume bad news
        test    [gbMidiInFlags], MIF_ALLOCATED
        jz      mid_RH_Exit

        cmp     [gbIntUsed], INT_MIDIIN     ; Q: midi input owned?
        jne     mid_RH_Exit

        D2 <release>

        mov     [gbIntUsed], INT_FREE       ; release it...
        cCall   vsbdReleaseSoundBlaster

        and     [gbMidiInFlags], not MIF_ALLOCATED
        xor     ax, ax                      ; success

mid_RH_Exit:

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL 
;
; @asm WORD | midStart | This function starts midi input if it isn't started.
;
; @rdesc There is no return value.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

cProc midStart <FAR, PASCAL, PUBLIC> <>
cBegin

        D2 <midStart>

        test    [gbMidiInFlags], MIF_STARTED
        jnz     mid_Start_Exit          ; exit if already started

ifdef DEBUG
        xor     ax, ax
        mov     [gwDebugQueueOF], ax
        mov     [gwDebugMaxInQueue], ax
        mov     [gwDebugQueueKick], ax
endif

        or      [gbMidiInFlags], MIF_STARTED
        mov     al, MIDIRD2             ; turns on interrupt-driven midi-input
        call    dspWrite

mid_Start_Exit:

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL 
;
; @asm WORD | midStop | This function stops midi input if it is started.
;
; @rdesc There is no return value.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

cProc midStop <FAR, PASCAL, PUBLIC> <>
cBegin

        D2 <midStop>

        test    [gbMidiInFlags], MIF_STARTED
        jz      mid_Stop_Exit           ; Q: are we already stopped?

        call    dspReset
        call    dspSpeakerOn

        cCall   midSendPartBuffer       ; send any unfinished headers back

ifdef DEBUG
        call    DebugMidi
endif

        and     [gbMidiInFlags], not MIF_STARTED


mid_Stop_Exit:

cEnd


sEnd CodeSeg

        end
