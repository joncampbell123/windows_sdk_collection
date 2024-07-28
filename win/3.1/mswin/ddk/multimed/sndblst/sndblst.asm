        page 60, 132
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   sndblst.asm
;
;   Copyright (c) 1991-1992 Microsoft Corporation.  All rights reserved.
;
;   General Description:
;      This module is a device driver for the Sound Blaster board.  It
;      provides drivers for wave and midi input and output.
;
;   Restrictions:
;      The card uses DMA channel 1.
;      Any of the prt options are valid.
;      Any of the interrupt options are valid although 3 is best
;      with Windows (secondary COM port).
;
;      WARNING: calls to dspWrite must not be allowed to be divided
;      by an end of DMA interrupt so turn ints off to make the
;      calls and then back on again after.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        .286

        .xlist
        include cmacros.inc                   
        include windows.inc
        include mmsystem.inc
        include mmddk.inc
        include sndblst.inc
        .list

        ?PLM=1                          ; Pascal call convention
        ?WIN=0                          ; NO! Windows prolog/epilog code

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   extrn declarations
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 

        externA <__AHINCR>                  ; kernel

        externFP <wodLoadDMABuffer>         ; wavefix.c
        externFP <wodPostAllHeaders>        ; wavefix.c
        externNP <widFillBuffer>            ; wavefix.c
        externNP <midByteRec>               ; midifix.c

        externD <gdwOldISR>                 ; inita.asm
        externW <gwEOICommands>             ; inita.asm
        externW <gwPort>                    ; inita.asm
        externW <gwMidiInPersistence>       ; inita.asm
        externB <gbDMAChannel>              ; inita.asm
        externB <gbMidiInFlags>             ; midia.asm

        externFP <StackEnter>               ; from MMSYSTEM.DLL
        externFP <StackLeave>

        externD <_lpSilenceStart>           ; wavefix.c

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

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   DMA buffer data. Note that the DMA buffer is split in two
;   so we can ping-pong between two actual buffers
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        globalW gwDMASelector,      0  ; DMA buffer selector
        globalW gwDMAOffset,        0  ; offset from selector to start of buffer
        globalW gwDMAPhysAddr,      0  ; 16 bit address for DMAC

        globalW gwCurSampleRate,    0  ; 

        globalB gbDMAPhysPage,      0  ; DMA physical page
        globalB gbDMABuffer,        0  ; 0 = none, 1 = ping, 2 = pong
        globalB _gfDMABusy,         0  ; 1 if busy, 0 if idle
        globalB gbIntUsed,          0  ; who's using the interrupt

        globalW gwMidiOutDelay,     0

sEnd DATA

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   code segment
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

sBegin CodeSeg

        assumes cs, CodeSeg
        assumes ds, DATA

ifdef DEBUG

cProc AssertBreak <FAR, PUBLIC> <>
cBegin
        int     3
cEnd

endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL
;
; @asm dspRead | Read a byte from the DSP.
;
; @rdesc If time out occurs before the DSP is ready then the carry flag
;     is set and no value is returned; otherwise, the carry flag is cleared
;     and the data value returned in AL.
;
; @comm The timeout value is very machine dependent.
;
; @comm USES: FLAGS, AL--all other registers are preserved.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

        public dspRead
dspRead proc far

        AssertF byte ptr [gwAcquireCount]

        push    cx
        push    dx

        mov     dx, [gwPort]
        add     dx, DSP_PORT_DATAAVAIL  ; point to data available status port
        mov     cx, 1000                ; set time out value
read1:
        in      al, dx                  ; get status
        or      al, al                  ; Q: MSB set?
        js      read2                   ;   Y: jump if data ready

        loop    read1

        ; timed out
        stc 
        jmp     read3

read2:
        sub     dx, (DSP_PORT_DATAAVAIL - DSP_PORT_RDDATA)  ; base + A
        in      al, dx                  ; get data byte
        clc

read3:
        pop     dx
        pop     cx

        ret

dspRead endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL
;
; @asm dspWrite | Write a command or data byte to the DSP.
;
; @reg AL | The byte to be written.
;
; @comm Carry is set on exit if it times out. The reason the timeout loop
;    is so big is that the SpeakerOff command can take up to 220 msec
;    to execute - if you time out before that the next few DSP commands
;    you write won't work.
;
; @comm Uses only FLAGS--all other registers are preserved.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

        public dspWrite
dspWrite proc far

        AssertF byte ptr [gwAcquireCount] ; we better have acquired it already!

        push    cx
        push    dx

        push    ax                      ; save data byte to write...

        mov     dx, [gwPort]
        add     dx, DSP_PORT_WRBUSY     ; point to data status port

        xor     cx, cx                  ; timeout loop counters
        mov     ah, 10
dspwr1:
        in      al, dx
        or      al, al                  ; test busy bit
        jns     dspwr2                  ; exit if not busy

        loop    dspwr1                  ; loop if not timed out
        dec     ah
        jnz     dspwr1

        stc                             ; set carry to show time out
        pop     ax                      ; dump the data
        jmp     dspwr3

dspwr2:
        pop     ax                      ; get the data byte
        out     dx, al                  ; write it (same port)
        .errnz (DSP_PORT_WRBUSY - DSP_PORT_WRDATA)
        clc                             ; no error

dspwr3:
        pop     dx
        pop     cx
        ret

dspWrite endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
;
;   dmaMaskChannel
;
;   DESCRIPTION:
;       Masks the DMA channel.
;
;   ENTRY:
;       Nothing.
;
;   EXIT:
;       Carry cleared.
;
;   USES:
;       FLAGS
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
        assumes ds, Data
        assumes es, nothing

        public dmaMaskChannel
dmaMaskChannel proc far

        D3 <dmaMaskChannel>

        push    ax

        mov     al, [gbDMAChannel]      ; get the channel number
        or      al, 00000100b           ; set mask bit
        out     DMASMR, al              ; enable DMA channel (reset mask)

        pop     ax
        clc
        ret

dmaMaskChannel endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL 
;
; @asm WORD | wodHaltDMA | This function halts DMA.
;
; @rdesc There is no return value.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        assumes ds, Data
        assumes es, nothing

        public wodHaltDMA
wodHaltDMA proc far

        D2 <wodHaltDMA>

        push    ax
        mov     [_gfDMABusy], 0         ; _must_ set to zero FIRST

        mov     al, HALTDMA             ; halt DMA
        call    dspWrite

        call    dmaMaskChannel          ; no more!
        pop     ax
        ret

wodHaltDMA endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL
;
; @asm dspISR | This function services interrupts from the DSP.
;
; @comm Special version for DSP ver 2.0 software
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

isr_Srv_Table label word
            .errnz INT_FREE
        dw  offset isr_Int_Unexpected   ; INT_FREE      equ 0
            .errnz INT_WAVEOUT - 1
        dw  offset isr_Int_Wave_Output  ; INT_WAVEOUT   equ 1
            .errnz INT_WAVEIN - 2
        dw  offset isr_Int_Wave_Input   ; INT_WAVEIN    equ 2
            .errnz INT_MIDIIN - 3
        dw  offset isr_Int_MIDI_Input   ; INT_MIDIIN    equ 3

        assumes ds, nothing
        assumes es, nothing

        public dspISR
dspISR proc far

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   For the 2.0 DSP it is not critical to keep interrupts off because of
;   auto-init DMA.   We will not get another interrupt until we un-mask
;   the PIC
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        push    ds
        push    ax
        mov     ax, DGROUP              ; set up local DS
        mov     ds, ax
        assumes ds, Data

        mov     al, [gbIntUsed]
        or      al, al                  ; Q: expecting an interrupt?
        jz      isr_Int_Unexpected      ;   N: CHAIN!

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   dispatch the interrupt to the correct handler...
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

isr_Handle_Interrupt:

        cCall   StackEnter              ; switch to another stack
        sti                             ; enable interrupts on new stack

        push    es
        pusha

        D3 <(v2>

        mov     di, ax                  ; convert gbIntUsed to table index
        and     di, 0003h               ; clear high byte...
        add     di, di
        call    isr_Srv_Table[di]       ; service the interrupt

        D3 <)>

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   NOTE: We switch back to our original stack BEFORE EOI'ing.  This way
;   we don't use too many stacks; if interrupts are pending that will fire
;   just after the EOI.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

isr_Exit:

        popa
        pop     es

        cli                             ; turn off ints for EOI
        cCall   StackLeave              ; get back to original stack

        mov     ax, [gwEOICommands]     ; get EOI commands for slave & master
        or      al, al                  ; Q: need to EOI slave? (IRQ's 8-15)
        jz      isr_EOI_Master          ;   N: just do master (IRQ's 0-7)

        out     PIC_EOI_SLAVE, al       ; EOI the slave first--then master

isr_EOI_Master:

        mov     al, ah                  ; move master EOI command to AL
        out     PIC_EOI_MASTER, al

        pop     ax
        pop     ds
        assumes ds, nothing

        iret

dspISR endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
;
;   isr_Int_Unexpected
;
;   We got an interrupt when we did not expect it!!!  In an attempt to
;   recover from this situation, we will CHAIN the interrupt and hope for
;   the best...
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
        assumes ds, Data
        assumes es, nothing

        public isr_Int_Unexpected
isr_Int_Unexpected proc near

        D1 <UNEXPECTED-INTERRUPT>

        pop     ax
        push    [gdwOldISR].sel
        push    [gdwOldISR].off

        push    bp                      ; restore DS from stack
        mov     bp, sp
        mov     ds, [bp + 6]            ; stack: [ds] [gdwOldISR].sel [gdwOldISR].off [bp]
        assumes ds, nothing
        pop     bp

        retf    2                       ; chain!

isr_Int_Unexpected endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
;
;   isr_Int_MIDI_Input
;
;   This routine is called to service a MIDI input interrupt.  The hardware
;   has not been touched since the interrupt was generated, so the status
;   register must be read, etc.
;
;   STATE:
;       DS is set to DGROUP.
;       All non-extended registers are saved.
;       Interrupts are ENABLED.
;       We are on a safe stack for callbacks.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
        assumes ds, Data
        assumes es, nothing

        public isr_Int_MIDI_Input
isr_Int_MIDI_Input proc near

        mov     si, [gwMidiInPersistence]
        mov     dx, [gwPort]
        add     dx, DSP_PORT_DATAAVAIL  ; point to data available status port
        mov     di, dx                  ; save status port in DI for later
    
isr_Int_MIDI_Input_Loop:

        in      al, dx                  ; get status (and clear int request)
        or      al, al                  ; Q: MSB set (ie data ready)?
        jns     isr_Int_MIDI_Input_Exit ;   N: no more data ready...

        sub     dx, (DSP_PORT_DATAAVAIL - DSP_PORT_RDDATA)  ; DX = data port
        in      al, dx                  ; get data byte

        cCall   midByteRec, <ax>        ; !!! trashes AX, BX, CX, DX, and ES
        mov     dx, di                  ; restore status port 
        dec     si
        jnz     isr_Int_MIDI_Input_Loop

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   We are bailing out because we have been in here for a long time; we 
;   will be back after allowing interrupts to kick on other devices.  There
;   is a good chance of losing data if we do this too frequently--this is
;   why there is a 'MIDIInPersistence=xxxx' key.  However, DEF_PERSISTENCE
;   should be sufficient in most circumstances (currently defined as 50).
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        D1 <TKO>                        ; Technical Knock Out!

isr_Int_MIDI_Input_Exit:

        ret

isr_Int_MIDI_Input endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
;
;   isr_Int_Wave_Input
;
;   This routine is called to service a Wave input interrupt.  The hardware
;   has not been touched since the interrupt was generated, so the status
;   register must be read, etc.
;
;   STATE:
;       DS is set to DGROUP.
;       All non-extended registers are saved.
;       Interrupts are ENABLED.
;       We are on a safe stack for callbacks.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
        assumes ds, Data
        assumes es, nothing

        public isr_Int_Wave_Input
isr_Int_Wave_Input proc near

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   acknowledge the interrupt from the DSP
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     dx, [gwPort]
        add     dx, DSP_PORT_DATAAVAIL  ; point to data avail port
        in      al, dx                  ; read status (reset interrupt)

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   copy the current data and toggle ping/pong... note that auto-DMA is
;   already at work with the opposite buffer.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        AssertF [gbDMABuffer]           ; always assumed 1 or 2 for input
        xor     [gbDMABuffer], (DMA_BUFFER_PING xor DMA_BUFFER_PONG)
        mov     ax, [gwDMAOffset]

        cmp     [gbDMABuffer], DMA_BUFFER_PONG  ; Q: pong buffer?
        jne     isr_Int_Wave_Input_Ping         ;   N: 

        add     ax, DMAHALFBUFSIZE              ; offset to pong buffer

isr_Int_Wave_Input_Ping:

        cCall   widFillBuffer, <gwDMASelector, ax, DMAHALFBUFSIZE>

        ret

isr_Int_Wave_Input endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
;
;   isr_Int_Wave_Output
;
;   This routine is called to service a Wave output interrupt.  The hardware
;   has not been touched since the interrupt was generated, so the status
;   register must be read, etc.
;
;   STATE:
;       DS is set to DGROUP.
;       All non-extended registers are saved.
;       Interrupts are ENABLED.
;       We are on a safe stack for callbacks.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
        assumes ds, Data
        assumes es, nothing

        public isr_Int_Wave_Output
isr_Int_Wave_Output proc near

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   acknowledge the interrupt from the DSP
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     dx, [gwPort]
        add     dx, DSP_PORT_DATAAVAIL  ; point to data avail port
        in      al, dx                  ; read status (reset interrupt)

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   if DMA not busy, then this is an interrupt pending just before halting
;   DMA (wodHaltDMA).
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        xor     ax, ax
        cmp     [_gfDMABusy], al
        je      isr_Int_Wave_Out_Exit

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   if we set lpSilenceStart last time, that means we're about to start
;   DMA'ing an incompletely filled buffer.  Reset lpSilenceStart to 0 so
;   that new data arriving now has to wait until the next time around.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     [_lpSilenceStart].sel, ax
        cmp     [gbDMABuffer], al       ; Q: another DMA buffer ready?
        jne     isr_Int_Wave_Out_Go     ;   Y: jump if more to do

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   DMA all finished so tidy up; free up all dead headers (callback to
;   owner) and mask the DMA channel--we're done!
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

isr_Int_Wave_Out_Done:

        D3      <shutdown>
        call    wodHaltDMA        ; we are done - stop NOW (gfDMABusy is zero'd)
        call    wodPostAllHeaders
        jmp     short isr_Int_Wave_Out_Exit

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   Read the next data block and set up for next interrupt.  gbDMABuffer
;   is reset here to reflect either a new buffer or no more to do.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

isr_Int_Wave_Out_Go:

        AssertF [gbDMABuffer]                   ; always assumed 1 or 2 here

        xor     [gbDMABuffer], (DMA_BUFFER_PING xor DMA_BUFFER_PONG)

        mov     ax, [gwDMAOffset]

        cmp     [gbDMABuffer], DMA_BUFFER_PONG  ; Q: pong?
        jne     isr_Int_Wave_Out_Ping           ;   N:

        add     ax, DMAHALFBUFSIZE              ; advance for pong

isr_Int_Wave_Out_Ping:

        cCall   wodLoadDMABuffer, <gwDMASelector, ax, DMAHALFBUFSIZE>

        or      ax, ax                          ; Q: any data copied?
        jnz     isr_Int_Wave_Out_Exit           ;   Y: then continue...

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   No data was copied, so shut down DMA at end of next half buffer
;   (silence...)
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     [gbDMABuffer], ah               ; set next buffer to 'none'
        errnz   DMA_BUFFER_DONE-0

isr_Int_Wave_Out_Exit:

        ret

isr_Int_Wave_Output endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL 
;
; @asm void | modDataWrite | This function writes a byte to the midi port.
;
; @parm BYTE | bDataByte | The byte to be written.
;
; @rdesc BOOL | 
;
; @comm This function polls the hardware (no choice with this card) so
;     it may take a while.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

cProc modDataWrite <NEAR, PASCAL, PUBLIC> <>
        ParmB   bDataByte
cBegin

        D3 <{dw>

        test    [gbMidiInFlags], MIF_STARTED
        jnz     mdw_MIDI_In_Started     ; if MIDI input active don't send MIDIWR

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   MIDI input is not started, so we need to send the MIDIWR command
;   before writing the data.  Note that we must enter a critical section
;   because an interrupt between the MIDIWR command and writing the
;   data byte will break (from wave input/output during MIDI output).
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        test    [gbIntUsed], 0FFh       ; Q: is there an ISR active?
        jz      mdw_No_Crit_Section_In  ;   N: then don't use crit section!

        EnterCrit                       ; !!! Trashes BX !!!

mdw_No_Crit_Section_In:

        mov     al, MIDIWR              ; send midi write command
        call    dspWrite
        mov     al, bDataByte           ; send midi data
        D4      <#AL>
        call    dspWrite

        test    [gbIntUsed], 0FFh       ; Q: is there an ISR active?
        jz      mdw_Exit                ;   N: then don't use crit section!

        LeaveCrit                       ; !!! Trashes BX !!!

        jmp     short mdw_Exit          ; done, get out!

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   MIDI input is currently active so there is no need to send the MIDIWR
;   command before writing a MIDI data byte.
;
;   NOTE: the gwMidiOutDelay must pause >= 400 us between data bytes so
;   the Sound Blaster's DSP can finish its internal bookkeeping, etc. This
;   is a 'bug' in the DSP's priorities of MIDI data, etc.  It would be nice
;   if we could at least READ data to keep the internal FIFO from overflowing,
;   but we cannot.  The DSP is busy and will not have data available for
;   us... bummer.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

mdw_MIDI_In_Started:

        mov     cx, [gwMidiOutDelay]
        jcxz    mdw_MIDI_In_No_Delay    ; Q: jmp if no delay required
        loop    $

mdw_MIDI_In_No_Delay:

        mov     al, bDataByte           ; send midi data
        D4      <#AL>
        call    dspWrite

mdw_Exit:

        D3 <}>

        xor     ax, ax                  ; always succeed for now

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL 
;
; @asm LPVOID | MemCopySrc | Block memory copy.
;
; @parm LPVOID | lpDst | Destination.
;
; @parm LPVOID | lpSrc | Source.
;
; @parm WORD | cnt | Number of bytes to copy.
;
; @rdesc Returns the source pointer advanced by <p cnt> bytes.
;
; @comm This function handles segment crossings in the source *only*.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, nothing
        assumes es, nothing

cProc MemCopySrc <NEAR, PASCAL, PUBLIC> <ds, si, di>
        ParmD   lpDst
        ParmD   lpSrc
        ParmW   wCount
cBegin

        lds     si, lpSrc             ; get source pointer
        mov     cx, wCount            ; cx is count of bytes
        jcxz    mcs_Exit              ; copy no bytes--return current pointer...

        cld                           ; let's not assume this
        les     di, lpDst             ; get dest pointer

        mov     ax, si                ; check for a segment cross in the source
        add     ax, cx
        sbb     bx, bx                ; if C BX=FFFF, NC BX=0000
        and     ax, bx                ;
        sub     cx, ax                ; CX contains amount to copy now, AX has
                                      ; ...amount to copy later

mcs_Copy_It:

        jcxz    mcs_Exit
        shr     cx, 1                 ; copy the memory
        rep     movsw
        adc     cl, cl
        rep     movsb

        or      si, si                ; check for a segment wrap
        jnz     mcs_Exit

        mov     cx, ax
        mov     ax, ds
        add     ax, __AHINCR
        mov     ds, ax
        jmp     short mcs_Copy_It

mcs_Exit:

        mov     dx, ds                ; DX:AX = advanced source pointer
        mov     ax, si

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL 
;
; @asm LPVOID | MemCopyDest | Block memory copy.
;
; @parm LPVOID | lpDst | Destination.
;
; @parm LPVOID | lpSrc | Source.
;
; @parm WORD | wCount | Number of bytes to copy.
;
; @rdesc Returns the destination pointer advanced by <p wCount> bytes.
;
; @comm This function handles segment crossings in the destination *only*.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, nothing
        assumes es, nothing

cProc MemCopyDst <NEAR, PASCAL, PUBLIC> <ds, si, di>
        ParmD   lpDst
        ParmD   lpSrc
        ParmW   wCount
cBegin

        cld                             ; let's not assume this

        lds     si, lpSrc               ; get source pointer
        les     di, lpDst               ; get dest pointer
        mov     cx, wCount              ; cx is count of bytes

        mov     ax, di                  ; check for a segment cross in the dest
        add     ax, cx
        sbb     bx, bx                  ; if C BX=FFFF, NC BX=0000
        and     ax, bx                  ;
        sub     cx, ax                  ; CX contains amount to copy now, AX has
                                        ; ...amount to copy later
mcd_Copy_It:

        jcxz    mcd_Exit
        shr     cx, 1                   ; copy the memory
        rep     movsw
        adc     cl, cl
        rep     movsb

        or      di, di                  ; check for a segment wrap
        jnz     mcd_Exit

        mov     cx, ax                  ; cross huge boundary
        mov     ax, es
        add     ax, __AHINCR
        mov     es, ax
        jmp     short mcd_Copy_It

mcd_Exit:

        mov     dx, es                  ; DX:AX = advanced dest pointer
        mov     ax, di

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL 
;
; @asm void | MemFillSilent | Fill memory with silence.
;
; @parm LPVOID | lpDst | Destination.
;
; @parm WORD | wCount | Number of bytes to copy.
;
; @comm Doesn't check for segment crossing since we're only padding DMA buffer.
;     This is specific to 8-bit data since it fills 0x80 for silence.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, nothing
        assumes es, nothing

cProc MemFillSilent <NEAR, PASCAL, PUBLIC> <di>
        ParmD   lpDst
        ParmW   wCount
cBegin

        cld                             ; let's not assume this

        les     di, lpDst               ; get dest pointer
        mov     cx, wCount              ; cx is count of bytes
        jcxz    mfs_Exit                ; any bytes at all?

        shr     cx, 1                   ; copy the memory
        mov     ax, 8080h               ; silence for 8-bit data
        rep     stosw
        adc     cl, cl                  ; is there a byte left over?
        rep     stosb

mfs_Exit:

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL
;
; @asm WEP | This function is called when the DLL is unloaded.
;
; @parm WORD | wUselessParm | This parameter has no meaning.
;
; @comm WARNING: This function is basically useless since you can't call any
;     kernel function that may cause the LoadModule() code to be reentered.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        assumes ds, nothing
        assumes es, nothing

cProc WEP <FAR, PUBLIC, NODATA>, <>
;       ParmW   wUselessParm
cBegin nogen

        mov     ax, 1
        retf    2

cEnd nogen


sEnd CodeSeg

        end
