        page 60, 132

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;   PURPOSE.
;
;   Copyright (c) 1993 - 1995	Microsoft Corporation.	All Rights Reserved.
;
;   midia.asm
;
;   This module is a device driver for the Roland MPU-401 board.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        PMODE = 1

        .xlist
        include cmacros.inc                   
        include windows.inc
        include mmsystem.inc
        include mmddk.inc
        include mpu401.inc
        .list

        ?PLM=1                          ; Pascal calling convention
        ?WIN=0                          ; NO! Windows prolog/epilog code


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   extrn declarations
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 

        externNP <midSendPartBuffer>

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
;---------------------------------------------------------------------------;
;   DebugMidi
;---------------------------------------------------------------------------;

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
        D1      <--------------------------------------------------------------------->

        D1      <MIDI Output:>
        mov     ax, [_gdwDebugMODWriteErrors].lo
        mov     bx, [_gdwDebugMODWriteErrors].hi
        mov     cx, [_gdwDebugMODataWrites].lo
        mov     dx, [_gdwDebugMODataWrites].hi
        D1      <   gdwDebugMODWriteErrors:#BX#AXh     gdwDebugMODataWrites:#DX#CXh>

        mov     ax, [_gdwDebugMOShortMsgs].lo
        mov     bx, [_gdwDebugMOShortMsgs].hi
        mov     cx, [_gdwDebugMOShortMsgsRS].lo
        mov     dx, [_gdwDebugMOShortMsgsRS].hi
        D1      <      gdwDebugMOShortMsgs:#BX#AXh    gdwDebugMOShortMsgsRS:#DX#CXh>

        mov     ax, [_gdwDebugMOShortMsgsBogus].lo
        mov     bx, [_gdwDebugMOShortMsgsBogus].hi
        mov     cx, [_gdwDebugMOLongMsgs].lo
        mov     dx, [_gdwDebugMOLongMsgs].hi
        D1      < gdwDebugMOShortMsgsBogus:#BX#AXh       gdwDebugMOLongMsgs:#DX#CXh>

        D1      < >
        D1      <MIDI Input:>
        mov     ax, [_gdwDebugMIBytesRcvd].lo
        mov     bx, [_gdwDebugMIBytesRcvd].hi
        mov     cx, [_gdwDebugMIShortMsgsRcvd].lo
        mov     dx, [_gdwDebugMIShortMsgsRcvd].hi
        D1      <      gdwDebugMIBytesRcvd:#BX#AXh  gdwDebugMIShortMsgsRcvd:#DX#CXh>

        mov     ax, [_gdwDebugMILongMsgsRcvd].lo
        mov     bx, [_gdwDebugMILongMsgsRcvd].hi
        mov     cx, [_gdwDebugMidiDrvCallbacks].lo
        mov     dx, [_gdwDebugMidiDrvCallbacks].hi
        D1      <   gdwDebugMILongMsgsRcvd:#BX#AXh gdwDebugMidiDrvCallbacks:#DX#CXh>

        mov     ax, [_gwDebugMILongErrors]
        mov     bx, [_gwDebugMIShortErrors]
        D1      <      gwDebugMILongErrors:#AXh         gwDebugMIShortErrors:#BXh>

        D1      < >
        D1      <MIDI Queue Stuff:>
        mov     ax, [gwDebugQueueOF]
        mov     bx, [gwDebugMaxInQueue]
        D1      <           gwDebugQueueOF:#AXh            gwDebugMaxInQueue:#BXh>

        mov     ax, [gwDebugQueueKick]
        D1      <         gwDebugQueueKick:#AXh>

        D1      < >

        pop     ds
        assumes ds, nothing
        pop     es
        popa
        popf
        ret

DebugMidi endp
endif

;---------------------------------------------------------------------------;
; @doc INTERNAL 
;
; @api void | mpuCommandWrite | This function writes a byte to the
;       MPU-401 command port.
;
; @parm AL | The command byte to write.
;
; @rdesc The carry flag is set if the read times out.
;---------------------------------------------------------------------------;

        assumes ds, Data
        assumes es, nothing

cProc mpuCommandWrite <FAR, PUBLIC> <si>
        ParmW   phwi
        ParmB   bCommandByte
cBegin

        mov     si, phwi
        mov     al, bCommandByte
        D2 <mpuCommandWrite:#AL>

BUG <mpuCommandWrite: doesn't work if data coming in on Music Quest (no ACK)>

        mov     dx, [si.hwi_wIOAddressMPU401]
        inc     dx                      ; point to status port

        xor     cx, cx

mcw_Wait_Hardware_Ready:

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   NOTE: ALWAYS read the data port and toss it...
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

if 0
        in      al, dx                  ; read status
        test    al, DSR
        jnz     mcw_Wait_Continue
endif

        dec     dx                      ; point to data port
        in      al, dx                  ; read data and bit bucket it
        inc     dx                      ; point to status port

mcw_Wait_Continue:

        in      al, dx                  ; read status
        test    al, DRR                 ; Q: can we write command?
        jz      mcw_Hardware_Ready      ;   Y: then do it!

;;;;;;;;D3 <to1>
        loop    mcw_Wait_Hardware_Ready ; loop while busy

        D1 <MPUCMD-TO>
        ;AssertT 1

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   timed out--hardware might just be in a gummed up state, so if we are
;   trying to RESET the adapter, then go for it!
;   DX = (wPort + 1)-->the command/status port
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     al, bCommandByte
        cmp     al, MPU401_CMD_RESET    ; Q: reset command?
        jne     mcw_Hardware_Error      ;   N: then fail!

        D2 <FORCE-RESET>
        out     dx, al                  ; send reset command to command port
        jmp     short mcw_Exit_Success  ; succeed no matter what...


mcw_Hardware_Error:

        D2 <HARDWARE-ERROR>
        mov     ax, -1                  ; fail!
        jmp     short mcw_Exit


mcw_Hardware_Ready:
        mov     al, bCommandByte

        cmp     al, MPU401_CMD_RESET    ; Q: reset command (no-ack sent)?
        jne     mcw_WriteAndWaitForAck  ;   N: go write and wait for an ack
        out     dx,al                   ;   Y: just write and succeed...
        jmp     SHORT mcw_Exit_Success

mcw_WriteAndWaitForAck:
        mov     [si.hwi_fISRCanReadData], 0
        out     dx, al                  ; send byte to command port

        xor     cx, cx                  ; timeout value for ACK
mcw_Wait_ACK:

        in      al, dx                  ; get status
        test    al, DSR
        jz      mcw_Read_Data_Byte
;;;;;;;;D3 <to2>
        loop    mcw_Wait_ACK            ; wait for DSR to go low

        D2 <NO-ACK>
        mov     ax, 1                   ; fail!
        jmp     short mcw_Exit


mcw_Read_Data_Byte:

        dec     dx                      ; point to data port
        in      al, dx                  ; read data
        inc     dx                      ; back to status port
        cmp     al, 0FEh                ; Q: is this the ACK?
        jne     mcw_Wait_ACK            ;   N: must be data; bit-bucket it


mcw_Exit_Success:

        xor     ax, ax                  ; success!

mcw_Exit:
        mov     [si.hwi_fISRCanReadData], 1 ; Tell ISR it can read data.

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL 
;
; @api void | mpuFlushBuffer | Flushes any data in the on-card buffer.
;       This is done when input is (re)started.
;
; @comm If there's more than one byte in the buffer, it takes some time for
;       the DSR to go low again.  With some experimentation, 50 loops worked
;       for a 386 33 MHz.
;
; @rdesc None.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        assumes ds, Data
        assumes es, nothing

cProc mpuFlushBuffer <NEAR, PUBLIC> <si>
        ParmW   phwi
cBegin

        D2 <flush>
        mov     si, phwi
        mov     dx, [si.hwi_wIOAddressMPU401]

mfb_Next_Byte:

        inc     dx                      ; point to status port
        mov     cx, 50                  ; setup loop counter - see comment

mfb_Try_Again:

        in      al, dx                  ; read status
        test    al, DSR                 ; see if there is data (DSR is low)  
        jz      mfb_Data_Available      ; jump if there is data
        loop    mfb_Try_Again           ; try again

        jmp     short mfb_Exit          ; jump if timed out

mfb_Data_Available:

        dec     dx                      ; point to data port
        in      al, dx                  ; read data
        jmp     mfb_Next_Byte           ; see if there's more data


mfb_Exit:

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL 
;
; @api WORD | midStart | This function starts midi input.  Since we're
;       running in uart mode, that just means we enable our interrupt.
;
; @rdesc There is no return value.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        assumes ds, Data
        assumes es, nothing

cProc midStart <FAR, PUBLIC> <si>
        ParmW   pmic
cBegin

        D2 <midStart>

        mov     si, pmic
        mov     si, [si.mic_pa.pa_phwi]

        test    [si.hwi_bMidiInFlags], MIF_STARTED
        jnz     mid_Start_Exit          ; exit if already started

ifdef DEBUG
        xor     ax, ax
        mov     [gwDebugQueueOF], ax
        mov     [gwDebugMaxInQueue], ax
        mov     [gwDebugQueueKick], ax
endif

        EnterCrit                       ; !!! Trashes BX !!!

        cCall   mpuFlushBuffer, <si>    ; get rid of any crud in FIFO

        or      [si.hwi_bMidiInFlags], MIF_STARTED

        LeaveCrit                       ; !!! Trashes BX !!!

mid_Start_Exit:

cEnd


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL 
;
; @api WORD | midStop | This function stops midi input.  Since we're
;       running in uart mode, that just means we disable our interrupt.
;
; @rdesc There is no return value.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        assumes ds, Data
        assumes es, nothing

cProc midStop <FAR, PUBLIC> <si>
        ParmW   pmic
cBegin

        D2 <midStop>

        mov     si, pmic
        mov     si, [si.mic_pa.pa_phwi]

        test    [si.hwi_bMidiInFlags], MIF_STARTED
        jz      mid_Stop_Exit           ; Q: are we already stopped?

;       EnterCrit                       ; !!! Trashes BX !!!

        and     [si.hwi_bMidiInFlags], not MIF_STARTED

;       LeaveCrit                       ; !!! Trashes BX !!!

        cCall   midSendPartBuffer, <pmic>        ; send any unfinished
                                                 ;    headers back


ifdef DEBUG
        call    DebugMidi
endif

mid_Stop_Exit:

cEnd

sEnd CodeSeg

        end
