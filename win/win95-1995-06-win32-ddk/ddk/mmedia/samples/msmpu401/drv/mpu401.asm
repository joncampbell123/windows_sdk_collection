        page 60, 132

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;   PURPOSE.
;
;   Copyright (c) 1993 - 1995  Microsoft Corporation.  All Rights Reserved.
;
;   mpu401.asm
;
;   This module is a device driver for the Roland MPU-401 board.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        .286

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

        externNP <midByteRec>           ; in midifix.c

ifdef DEBUG
        externW <gwDebugQueueOF>
        externW <gwDebugInQueue>
        externW <gwDebugMaxInQueue>
        externW <gwDebugQueueKick>
endif

JUMP macro inst, dest
ifdef DEBUG
	inst	dest
else
	inst	short dest
endif
endm

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
;   code segment
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

sBegin CodeSeg

        assumes cs, CodeSeg
        assumes ds, Data

ifdef DEBUG

cProc AssertBreak <FAR, PUBLIC> <>
cBegin
        int     3
cEnd

endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL 
;
; @api BOOL | modDataWrite | This function writes a byte to the
;       MPU-401 data port
;
; @parm AL | data byte to write
;
; @rdesc The carry flag is set if the read times out.
;
;---------------------------------------------------------------------------;
;   To write a byte to the data port, we need to wait for the status
;   register to give us Data Receive Ready (DRR*, active low).  However,
;   on real MPU-401 cards and many clones you cannot write data if the
;   MPU-401 has data ready for the HOST.  So we need to buffer these bytes
;   so we don't lose them during full-duplex comms.  NOTE: some clone
;   cards (Music Quest for example) do not require that you clear the
;   data that is ready before writing--so always test for DRR* first!
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        assumes ds, Data
        assumes es, nothing

cProc modDataWrite <NEAR, PUBLIC> <si, di>
        ParmW   phwi
        ParmB   bDataByte
cBegin

        D3 <(dw>

        mov     si, phwi
        mov     dx, [si.hwi_wIOAddressMPU401]   ; get the base port address
        inc     dx                              ; point to status port
        xor     cx, cx                          ; timeout value

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;
;   !!! READ OR DIE !!!
;
;   There are three different states that we could be in while trying to
;   output a data byte:
;
;       1.  only output is open
;       2.  input and output are open; output is performed at NON-interrupt
;           time.
;       3.  input and output are open; output is performed inside mpuISR.
;
;   (1) is easy; just output the data throwing away any data read from 
;       the FIFO. mdw_NIT_Wait_Dont_Crit takes care of this case
;   (2) requires that we enter a critical section so we can read the 
;       status register and data without being interrupted in between.
;       mdw_NIT_DRR_Wait_Loop takes care of this case.
;   (3) we cannot be interrupted because the MPU-401 has not been EOI'd
;       yet--therefore, we do NOT have to enter a critical section.
;       mdw_IT_DRR_Wait_Loop takes care of this case.
;
;   The biggest performance gain is for (3) in Enhanced Mode.  As soon
;   as mpuISR is entered, we enabled interrupts: this is good. But if
;   we call EnterCrit, it will issue a CLI at this time.  In Enhanced 
;   Mode, this CLI instruction takes several HUNDRED clocks!  We cannot
;   afford this overhead in the ISR!!!
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;


        test    [si.hwi_bMidiInFlags], MIF_INISR ; Q: are we 'in' the ISR?
        jz      mdw_Non_Interrupt_Time           ;   Y: then don't cli/sti!!!

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;
;   mdw_IT_DRR_Wait_Loop
;
;   MIDI input and output are active; the output is occurring at interrupt
;   time.  We need to Queue up any data we read for the ISR to process
;   when we return for outputing this MIDI data...
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     si, [si.hwi_pmic]
        lea     di, [si.mic_abDataQueue]        ; set up Q base pointer

mdw_IT_DRR_Wait_Loop:

        in      al, dx                  ; read status
        test    al, DRR                 ; Q: bit 6 set (DRR*)?
        jz      mdw_Write_Data_zYz      ;   N: cool! write the data

        test    al, DSR                 ; Q: bit 7 set (DSR*)?
        jnz     mdw_IT_DRR_Time_Out     ;   Y: Hmm... continue time-out

        dec     dx                      ; point to data port
        in      al, dx                  ; read data
;;;;;;;;D3 <drI:#AL>
        inc     dx                      ; back to status port

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   store data in a queue to be read by the ISR...
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     bx, [si.mic_wDataQHead] ; get current queue pointer
        mov     [di + bx], al           ; store the byte in the queue

        inc     bx                      ; advance head pointer
        and     bx, (MIDIIN_QUEUE_SIZE - 1)

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   if the Queue overflows, we don't have any choice but to drop data.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

ifdef DEBUG
        inc     [gwDebugInQueue]
endif
        cmp     [si.mic_wDataQTail], bx ; Q: did we catch our tail?
        je      mdw_IT_Queue_Overflow   ;   Y: then deal with it...

        mov     [si.mic_wDataQHead], bx ;   N: cool! save advanced pointer

ifndef DEBUG
mdw_IT_Queue_Overflow:
endif

mdw_IT_DRR_Time_Out:

;;;;;;;;D3 <toI>
        loop    mdw_IT_DRR_Wait_Loop    ; keep waiting for DRR to go low
        jmp     short mdw_Timeout_Error

ifdef DEBUG
mdw_IT_Queue_Overflow:

        D1 <Q-OVERFLOW>
        inc     [gwDebugQueueOF]
        int     3
        jmp     short mdw_IT_DRR_Time_Out     
endif

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;
;   MIDI input is active, but output is is occurring at non-interrupt time.
;   For example, some app is recording data while another is outputing
;   data... perfectly legal.  We MUST enter a critical section so the ISR
;   does not read the data between our reading of the status port and
;   finally reading the data.  Also, we must 'record' the bytes we read!!
;   
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

mdw_Non_Interrupt_Time:

        test    [si.hwi_bMidiInFlags], MIF_STARTED
        jz      mdw_NIT_Wait_Dont_Crit  ; MIDI input not started...

        mov     si, [si.hwi_pmic]       ; get pointer to midi-in client

mdw_NIT_DRR_Wait_Loop:

        EnterCrit                       ; !!! Trashes BX !!!

        in      al, dx                  ; read status
        test    al, DRR                 ; Q: bit 6 set (DRR*)?
        jz      mdw_NIT_Leave_Crit_Write_Data

        test    al, DSR                 ; Q: bit 7 set (DSR*)?
        jnz     mdw_NIT_DRR_Time_Out    ;   Y: Hmm... continue time-out

        dec     dx                      ; point to data port
        in      al, dx                  ; read data
;;;;;;;;D3 <drC:#AL>
        inc     dx                      ; back to status port

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   MIDI input is started, record data we just read!
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        push    cx                      ; save these...
        push    dx
        cCall   midByteRec, <si, ax>    ; !!! trashes AX, BX, CX, DX, and ES
        pop     dx
        pop     cx

mdw_NIT_DRR_Time_Out:

;;;;;;;;D3 <toC>

        LeaveCrit                       ; !!! Trashes BX !!!
        loop    mdw_NIT_DRR_Wait_Loop   ; keep waiting for DRR to go low
        jmp     short mdw_Timeout_Error


;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;
;   mdw_NIT_Wait_Dont_Crit
;
;   Only MIDI output is active.  Simply throw away any data read and do
;   NOT enter a critical section (we won't be interrupted by anything that
;   will _read_ data...)
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

mdw_NIT_Wait_Dont_Crit:

        in      al, dx                  ; read status
        test    al, DRR                 ; Q: bit 6 set (DRR*)?

mdw_Write_Data_zYz:

        jz      mdw_Write_Data

        test    al, DSR                 ; Q: bit 7 set (DSR*)?
        jnz     mdw_NIT_Dont_Crit_DRR_Time_Out

        dec     dx                      ; point to data port
        in      al, dx                  ; read data
;;;;;;;;D3 <drN:#AL>
        inc     dx                      ; back to status port

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   MIDI input NOT started, bit-bucket data we read...
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

mdw_NIT_Dont_Crit_DRR_Time_Out:

;;;;;;;;D3 <toN>
        loop    mdw_NIT_Wait_Dont_Crit  ; keep waiting for DRR to go low
        ; fall through to mdw_Timeout_Error

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   This is very bad; the hardware is not letting us send the data byte
;   quickly enough--so signal an error and get out!
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

mdw_Timeout_Error:

ifdef DEBUG
        D1 <DW-TIMEOUT>
        int     3
endif
        mov     ax, -1                  ; return error
        jmp     short mdw_Exit


mdw_NIT_Leave_Crit_Write_Data:

        LeaveCrit                       ; !!! Trashes BX !!!
        ; fall through to mdw_Write_Data


;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   Data Receive Ready (DRR, bit 6 of status register) is low; this means
;   we can write the data...
;
;       DX = status port (gwPort + 1)
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

mdw_Write_Data:

        dec     dx                      ; point to data port
        mov     al, bDataByte
;;;;;;;;D3 <wd:#AL>
        out     dx, al                  ; send it

        xor     ax, ax                  ; success!
        ; fall through to mdw_Exit

mdw_Exit:

        D3 <)>

cEnd

;============================================================================

cProc MPU401InterruptHandler, <FAR, PASCAL, PUBLIC>, <si>

        ParmW   phwi

cBegin
        push    ds
        push    ax
        mov     ax, DGROUP                      ; set up local DS
        mov     ds, ax
        assumes ds, Data

        D3 <[>

        mov     si, phwi

        ;
        ; If MIDI input isn't started, ignore the interrupt
        ;

        test    [si.hwi_bMidiInFlags], MIF_STARTED
        jnz     isr_Handle_Interrupt

        ;
        ; We're not expecting interrupts, but we got one from our device.
        ; We'll reset the int (DSR) by reading the data.  UNLESS the int
        ; was caused by mpuCommandWrite.  Then we won't read the data cuz
        ; that would make DSR false and mpuCommandWrite might never
        ; see it.
        ;

        cmp     [si.hwi_fISRCanReadData], 0     ; Q: is mpuCommandWrite
                                                ;       waiting for DSR?
        jnz     isr_BitBucket                   ;    N: read data port
        JUMP    jmp, isr_Just_Ret               ;    Y: don't read data port

isr_BitBucket:
        push    ax
        push    dx

        mov     dx, [si.hwi_wIOAddressMPU401]
        in      al,dx

        D1      <UNEXP DAT>

        pop     dx
        pop     ax
        JUMP    jmp, isr_Just_Ret

        ;
        ; MIDI input
        ;
        ; we have recieved a midi interrupt, empty upto
        ; [si.wMidiInPersistence] midi bytes from the FIFO,
        ; and get out.  By emptying multiple bytes in a single ISR,
        ; we cut down the interrupt overhead (ie less interrupts
        ; win386 must deal with)
        ;


isr_Handle_Interrupt:

        ;
        ; IMPORTANT: we MUST set the MIF_INISR flag and read the current
        ; tail pointer (to compare it with the head) BEFORE we enable
        ; interrupts!!!
        ;
        ; See NASTY note at end of this ISR for more info...
        ;

        or      [si.hwi_bMidiInFlags], MIF_INISR
        mov     si, [si.hwi_pmic]
        mov     di, [si.mic_wDataQTail]

ifdef DEBUG
        cmp     [si.mic_wDataQHead], di
        je      @F
        D1 <MAJOR QUEUE PROBLEM>
        int     3                       ; break to debugger...
        xor     di, di
        mov     [si.mic_wDataQTail], di ; recover for debugging purposes ONLY
        mov     [si.mic_wDataQHead], di
@@:
endif

        sti                                     ; turn interrupts ON

        push    si
        mov     si, phwi
        mov     cx, [si.hwi_wMidiInPersistence]
        test    [si.hwi_bMidiInFlags], MOF_ALLOCATED
        jz      SHORT @F
        shr     cx, 1
@@:
        mov     dx, [si.hwi_wIOAddressMPU401]   ; get the base port address
        pop     si

isr_Next_Byte:
        inc     dx                              ; point to status port
        in      al, dx                          ; read status
        test    al, DSR
        jnz     isr_No_More_Data

        dec     dx                              ; get the base port address
        in      al, dx                          ; read data

        D3 <P>

isr_Record_Byte:

        D3 <#AL>

ifdef DEBUG
        mov     [gwDebugInQueue], 0
endif

        push    cx
        push    dx
        cCall   midByteRec, <si, ax>    ; !!! trashes AX, BX, CX, DX, and ES
        pop     dx
        pop     cx

isr_Record_Queue_Data:

ifdef DEBUG
        mov     bx, [gwDebugInQueue]
        cmp     [gwDebugMaxInQueue], bx
        ja      @F
        mov     [gwDebugMaxInQueue], bx
@@:
endif
        cmp     [si.mic_wDataQHead], di         ; Q: any data in the queue?
        je      isr_No_Queue_Data               ;   N: then loop...

        lea     bx, [si.mic_abDataQueue]        ; grab the next byte
                                                ;    from the Q
        mov     al, [di + bx]

        inc     di
        and     di, (MIDIIN_QUEUE_SIZE - 1)    
        mov     [si.mic_wDataQTail], di         ; update tail pointer..

        D3 <Q>

        jmp     short isr_Record_Byte


isr_No_Queue_Data:
        loop    isr_Next_Byte                   ; loop until midi persistence
                                                ;    is completed...

        D1 <TKO>                                ; TKO - data but no time
        ; fall through to isr_No_More_Data

isr_No_More_Data:


        D3 <]>
        ; fall through to isr_Exit

;----------------------------------------------------------------------------
;   NASTY HORRIBLE STUFF: it is entirely possible for MIDI output to have
;   occured between the last time we checked the Queue (isr_Record_Queue_Data)
;   and this spot.  This is caused by MIDI output being done based on a
;   higher priority interrupt than what the MPU-401 is on. Remember that we
;   ENABLE interrupts while we are recording data. The best example of this:
;
;       1.  App A has the _input_ port open and is actively recording data.
;       2.  App B has the _output_ port open and is streaming data based
;           on timer interrupts (MCISEQ does this).
;
;   If we are in between isr_Record_Queue_Data and isr_Exit when a timer
;   interrupt occurs causing App B to output data, then the queue has a
;   very good chance of having data in it.  We MUST read this data before
;   leaving the ISR!
;----------------------------------------------------------------------------

isr_Exit:

        cli                                     ; go critical

        cmp     [si.mic_wDataQHead], di         ; Q: any data in the queue?
        je      isr_Queue_Empty                 ;   N: good!

ifdef DEBUG
        inc     [gwDebugQueueKick]
endif
        D2 <KICK>

        sti                                     ; re-enable interrupts!
        inc     cx                              ; in case loop got TKO'd...
        jmp     short isr_Record_Queue_Data


isr_Queue_Empty:
        mov     si, phwi
        and     [si.hwi_bMidiInFlags], not MIF_INISR

isr_Just_Ret:
        pop     ax
        pop     ds
        assumes ds, nothing
cEnd

sEnd CodeSeg

        end
