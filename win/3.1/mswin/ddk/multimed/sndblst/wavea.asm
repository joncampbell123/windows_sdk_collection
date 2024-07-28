        page 60, 132
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   wavea.asm
;
;   Copyright (c) 1991-1992 Microsoft Corporation.  All rights reserved.
;
;   General Description:
;      Contains wave support routines that don't need to be fixed.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        .286

        .xlist
        include cmacros.inc                   
        include windows.inc
        include mmsystem.inc
        include mmddk.inc
        include sndblst.inc
        include vsbd.inc
        .list

        ?PLM=1                              ; Pascal calling convention
        ?WIN=0                              ; NO! Windows prolog/epilog code

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   extrn declarations
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 

        externFP <timeGetTime>              ; MMSystem

        externFP <vsbdAcquireSoundBlaster>  ; commona.asm
        externFP <vsbdReleaseSoundBlaster>  ; commona.asm
        externFP <dspReset>                 ; commona.asm
        externFP <dspSpeakerOn>             ; commona.asm
        externFP <dspSpeakerOff>            ; commona.asm

        externFP <dspWrite>                 ; sndblst.asm
        externFP <dspRead>                  ; sndblst.asm
        externFP <dmaMaskChannel>           ; sndblst.asm

        externNP <widSendPartBuffer>        ; wavein.c

        externFP <wodLoadDMABuffer>         ; wavefix.c

        externD <_glpWOQueue>               ; sndblst.asm
        externD <_glpWIQueue>               ; sndblst.asm
        externB <gbDMABuffer>               ; sndblst.asm
        externW <gwCurSampleRate>           ; sndblst.asm
        externB <gbIntUsed>                 ; sndblst.asm
        externB <_gfDMABusy>                ; sndblst.asm
        externB <_gfEnabled>                ; sndblst.asm
        externB <gbDMAPhysPage>             ; sndblst.asm
        externW <gwDMAOffset>               ; sndblst.asm
        externW <gwDMAPhysAddr>             ; sndblst.asm
        externW <gwDMASelector>             ; sndblst.asm

        externD <_lpSilenceStart>           ; wavefix.c
        externW <_wSilenceSize>             ; wavefix.c
        externB <_gfWaveOutPaused>          ; wavefix.c

        externB <gbDMAChannel>              ; inita.asm
        externB <gbDMAPageReg>              ; inita.asm

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

        globalB gbWaveInFlags,      0
        globalB gbWaveOutFlags,     0

sEnd Data

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   code segment
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

sBegin CodeSeg

        assumes cs, CodeSeg
        assumes ds, Data

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL
;
; @asm dspSetSampleRate | Set the DSP sample rate.
;
; @parm WORD | wSampleRate | The sample rate in Hz.
;
; @rdesc The carry flag is cleared if the operation was successful.
;   This function will currently always succeed.  The sample rate is
;   assumed to be valid.  This is because the range of frequencies is
;   different for input vs output and this function does not differentiate
;   between the two modes.
;
; @comm The sample rate must be in the range 4000 to 23000 for output;
;       4000 to 12000 for input.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

cProc dspSetSampleRate <FAR, PASCAL, PUBLIC> <>
        ParmW   wSampleRate
cBegin

        AssertT [_gfDMABusy]            ; DMA better NOT be going!

        mov     ax, wSampleRate
        mov     [gwCurSampleRate], ax

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   compute the timing value = 256 - 1000000 / samples
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     bx, ax
        mov     dx, 000Fh
        mov     ax, 4240h               ; DX:AX = 1,000,000
        div     bx                      ; AL has result (43 - 250)
        neg     al                      ; AL = 256 - AL

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   program the DSP
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        push    ax                      ; save AL
        mov     al, SETSAMPRATE
        call    dspWrite                ; set sample rate command
        pop     ax
        call    dspWrite                ; set the value
        clc                             ; no error, always succeed

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   dmaStartDMA
;
;   DESCRIPTION:
;       Starts DMA for both wave output and input.
;
;   ENTRY:
;       AX = 0 for DMA read (wave output)
;            non-zero for DMA write (wave input),
;
;   EXIT:
;       Carry cleared.  All other registers are preserved.
;
;   USES:
;       FLAGS
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

        public dmaStartDMA
dmaStartDMA proc near

        push    ax
        push    bx
        push    cx
        push    dx

        D2 <startdma>

        xor     cl, cl                  ; cl = (al ? FF : 00)
        cmp     cl, al                  ; set carry if al != 0
        sbb     cl, cl                  ; 0 = wave out, FF = wave in

        xor     bh, bh                  ; clear this
        mov     bl, [gbDMAChannel]      ; get the channel number

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   set up DMAC for next chunk
;
;   AX = nothing
;   BX = DMA channel number
;   CX = 0 for wave out, 1 for wave in
;   DX = nothing
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        out     DMACLR, al              ; clear the byte pointer latch

        mov     al, DMARDA xor DMAWRA   ; al = (cl == FF) ? DMARDA : DMAWRA
        and     al, cl
        xor     al, DMAWRA

        or      al, bl                  ; create cmd in AL
        out     DMAMOD, al              ; set up the DMA mode

        mov     dx, bx                  ; get channel number
        shl     dl, 1                   ; channel -> base/cur addr register
        mov     ax, [gwDMAPhysAddr]
        out     dx, al                  ; set lo addr
        mov     al, ah
        pause
        out     dx, al                  ; set hi addr

        inc     dx                      ; go to the count register

        mov     ax, (DMAPHYSBUFSIZE - 1); program size - 1
        out     dx, al                  ; lo count
        mov     al, ah
        pause
        out     dx, al                  ; hi count

        mov     al, [gbDMAPhysPage]     ; get DMA page
        mov     dl, [gbDMAPageReg]      ; get page register for DMA channel
        out     dx, al                  ; set DMA page register

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   enable the DMA channel
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     al, bl                  ; get channel
        out     DMASMR, al              ; enable DMA channel (reset mask)

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   program the DSP for the next chunk set up for the DMA mode we want    
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        EnterCrit                       ; !!! trashes BX !!!

        mov     al, SETBLCKSIZE
        call    dspWrite                ; set block size

        mov     ax, (DMAHALFBUFSIZE - 1)
        call    dspWrite                ; send AL (LSB)
        mov     al, ah
        call    dspWrite                ; send AH (MSB)

        mov     al, WAVERDA xor WAVEWRA ; al = (cl == FF) ? WAVERDA : WAVEWRA
        and     al, cl
        xor     al, WAVEWRA
        call    dspWrite                ; auto mode

        LeaveCrit                       ; !!! trashes BX !!!

        pop     dx
        pop     cx
        pop     bx
        pop     ax
        clc
        ret

dmaStartDMA endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL
;
; @asm wodKickstartWaveOutput | This function starts wave output DMA with the
;       block of data at the head of the queue.
;
; @rdesc There is no return value.
;
; @comm General process: 1) load 1st DMA buffer. 2) start DMA from
;     first buffer. 3) load 2nd buffer. 4) set up params for
;     fast change to 2nd buffer at interrupt time. 5) start interrupts.
;     Note that interrupts are off for the duration of all of this,
;     including during a possible callback to the invoking application.
;
; @comm USES: Flags (carry set if no data/DMA not started; else carry clr)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

cProc wodKickstartWaveOutput <NEAR, PASCAL, PUBLIC> <>
cBegin

        D3 <wodKickstartWaveOutput>

        AssertT [_gfDMABusy]            ; DMA better *not* be on

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   set up ping pong system and load the first buffer we load the lower
;   half of the phys buffer first
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     [gbDMABuffer], DMA_BUFFER_DONE
        cCall   wodLoadDMABuffer, <gwDMASelector, gwDMAOffset, %(DMAHALFBUFSIZE*2)>
        D2      <dmaload:#AX>
        cmp     ax, DMAHALFBUFSIZE
        jbe     dma_KWO_Only_Ping

        D2      <2bufs>
        mov     [gbDMABuffer], DMA_BUFFER_PONG

dma_KWO_Only_Ping:

        or      ax, ax                  ; Q: any data copied? (clear carry)
        jz      dma_KWO_Exit            ;   N: then exit with carry set

        mov     [_gfDMABusy], TRUE      ; say we are running now
        xor     ax, ax                  ; start 'wave out' DMA
        call    dmaStartDMA             ; kick-start; destroys only flags
        mov     ax, 1

dma_KWO_Exit:

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   WORD FAR PASCAL wodAcquireHardware( void )
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

cProc wodAcquireHardware <FAR, PASCAL, PUBLIC> <>
cBegin

        D2 <wodAcquireHardware>

        mov     ax, -1                      ; assume bad news
        test    [gbIntUsed], al             ; Q: is the hardware available?
        jnz     wod_AH_Exit

        test    [gbWaveOutFlags], WOF_ALLOCATED
        jnz     wod_AH_Exit                 ; Q: is the hardware available?

        D2 <acquire>

        cCall   vsbdAcquireSoundBlaster
        or      ax, ax
        jnz     wod_AH_Exit
        
        or      [gbWaveOutFlags], WOF_ALLOCATED
        mov     [gbIntUsed], INT_WAVEOUT    ; allocate it...
        xor     ax, ax                      ; success

wod_AH_Exit:

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   WORD FAR PASCAL wodReleaseHardware( void )
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

cProc wodReleaseHardware <FAR, PASCAL, PUBLIC> <>
cBegin

        D2 <wodReleaseHardware>

        mov     ax, -1                      ; assume bad news
        test    [gbWaveOutFlags], WOF_ALLOCATED
        jz      wod_RH_Exit

        cmp     [gbIntUsed], INT_WAVEOUT    ; Q: wave output owned?
        jne     wod_RH_Exit

        D2 <release>

        mov     [gbIntUsed], INT_FREE       ; release it...
        cCall   vsbdReleaseSoundBlaster

        and     [gbWaveOutFlags], not WOF_ALLOCATED
        xor     ax, ax                      ; success

wod_RH_Exit:

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   WORD FAR PASCAL widAcquireHardware( void )
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

cProc widAcquireHardware <FAR, PASCAL, PUBLIC> <>
cBegin

        D2 <widAcquireHardware>

        mov     ax, -1                      ; assume bad news
        test    [gbIntUsed], al             ; Q: is the hardware available?
        jnz     wid_AH_Exit

        test    [gbWaveInFlags], WIF_ALLOCATED
        jnz     wid_AH_Exit                 ; Q: is the hardware available?

        D2 <acquire>

        cCall   vsbdAcquireSoundBlaster
        or      ax, ax
        jnz     wid_AH_Exit
        
        or      [gbWaveInFlags], WIF_ALLOCATED
        mov     [gbIntUsed], INT_WAVEIN     ; allocate it...
        xor     ax, ax                      ; success

wid_AH_Exit:

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   WORD FAR PASCAL widReleaseHardware( void )
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

cProc widReleaseHardware <FAR, PASCAL, PUBLIC> <>
cBegin

        D2 <widReleaseHardware>

        mov     ax, -1                      ; assume bad news
        test    [gbWaveInFlags], WIF_ALLOCATED
        jz      wid_RH_Exit

        cmp     [gbIntUsed], INT_WAVEIN     ; Q: wave input owned?
        jne     wid_RH_Exit

        D2 <release>

        mov     [gbIntUsed], INT_FREE       ; release it...
        cCall   vsbdReleaseSoundBlaster

        and     [gbWaveInFlags], not WIF_ALLOCATED
        xor     ax, ax                      ; success

wid_RH_Exit:

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL 
;
; @asm WORD | wodWrite | This function sends a wave data block to the driver.
;
; @parm LPWAVEHDR | lpHdr | Far pointer to a wave data block header.
;
; @rdesc There is no return value.
;
; @comm When a block is written it is added to the queue for the physical
;     device (only one in this case).  If the DSP is currently sending data
;     the routine returns.  If the DSP is idle then the DMA process is
;     started and a request for a new block is made immediately.  As each
;     block is finished its callback function is invoked (if not NULL).  If
;     the app doesn't supply a callback then it's responsible for keeping
;     the queue full and monitoring all blocks for 'done bit' set when the
;     DSP is done with them.
;
;     We assume that the header and block are both page locked when they
;     get here.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

cProc wodWrite <NEAR, PASCAL, PUBLIC> <si, di>
        ParmD   lpHdr                   ; pointer to header block
cBegin

        D2 <wodWrite>

;BUG <wodWrite: this should NOT be in assembly>

        ; make sure that the 'next' pointer is zeroed

        les     bx, lpHdr               ; get the header pointer
        xor     ax, ax
        mov     es:[bx].lpWaveNext.off, ax
        mov     es:[bx].lpWaveNext.sel, ax

        ; walk down the current list to the last block

        EnterCrit                       ; don't want the ISR to mod the list
                                        ; while we walk it

        les     bx, _glpWOQueue
        mov     ax, es
        or      ax, bx
        jnz     write1                  ; jump if there is a queue

        ; no queue, so point to current block

        les     bx, lpHdr
        mov     _glpWOQueue.sel, es
        mov     _glpWOQueue.off, bx        ; point to current block
        jmp     write3                  ; go see if we need to start DSP

write1:
        ; see if this is the last block

        mov     ax, es:[bx].lpWaveNext.off
        or      ax, es:[bx].lpWaveNext.sel
        jz      write2                  ; jump if last block

        ; move on down the list

        les     bx, es:[bx].lpWaveNext
        jmp     write1

write2:
        ; ES:BX points to last block

        mov     ax, lpHdr.off
        mov     es:[bx].lpWaveNext.off, ax
        mov     ax, lpHdr.sel
        mov     es:[bx].lpWaveNext.sel, ax

write3:
        ; see if DSP DMA needs restarting
        ; ints still off here

        test    [_gfDMABusy], 0FFh
        jnz     write4                  ; jump if already running

        call    wodKickstartWaveOutput
        jmp     write5

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   if we had a padded buffer, fill it out
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

write4:

        mov     ax, [_lpSilenceStart].sel
        mov     bx, [_lpSilenceStart].off
        or      ax, ax
        jz      write5                  ; jump if no partially-filled buffer

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   GET DATA IN NOW!
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        push    bx

        xor     cx, cx
        mov     [_lpSilenceStart].sel, cx
        xchg    cx, [_wSilenceSize]
        D2      <fillpad:#CX>
        cCall   wodLoadDMABuffer, <ax, bx, cx>

        pop     bx

        D2 <partial>

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        cmp     [gbDMABuffer], DMA_BUFFER_DONE
        jne     write5

        D2 <silencekick>

        mov     [gbDMABuffer], DMA_BUFFER_PING
        sub     bx, [gwDMAOffset]
        cmp     bx, DMAHALFBUFSIZE
        jb      write5

        inc     [gbDMABuffer]
        errnz   DMA_BUFFER_PONG-DMA_BUFFER_PING-1

write5:
        LeaveCrit

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL 
;
; @asm WORD | wodPause | This function signals the DSP to halt
;      DMA at the end of the next 2K buffer.
;
; @rdesc There is no return value.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

cProc wodWaitForDMA <FAR, PASCAL, PUBLIC> <si, di>
cBegin
         
        D1 <wodWaitForDMA>

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   Since ALL KINDS of problems can result if we do not receive an interrupt
;   when we are expecting one, we need to 'time-out' in waiting for DMA
;   to complete.  If we DO time-out, then we Disable the driver as much
;   as possible (making it look like the driver isn't working--which it
;   isn't).  This can happen if the wrong IRQ is selected or some other
;   naughty driver hooks our IRQ away from us, etc, etc...
;
;   NOTE: the LONGEST a DMA block should EVER take would be for a 4 kHz
;   8 bit mono sample running for 4 kbytes (a little over 1 second). So
;   we will be _very conservative_ and wait 2 seconds.  Since this can
;   ONLY happen in bad conditions--and will ONLY happen once, this shouldn't
;   be too bad.  Interrupts are enabled.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        cCall   timeGetTime            ; get initial time stamp in DX:AX
        mov     si, ax                 ; then put in DI:SI
        mov     di, dx

wod_Wait_DMA_Loop:

        xor     ax, ax                 ; always assume the best (waste time too)
        test    [_gfDMABusy], 0FFh     ; Q: DMA done?
        jz      wod_Wait_DMA_Exit      ;   Y: quick exit!

        cCall   timeGetTime            ; get new time stamp in DX:AX
        sub     ax, si                 ; compute difference
        sbb     dx, di

        or      dx, dx
        jnz     wod_Wait_DMA_Timeout
        cmp     ax, WOD_DMA_TIMEOUT    ; Q: has it been 2 seconds?
        jb      wod_Wait_DMA_Loop

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   This is really really bad news!  We don't seem to be getting our ints,
;   so we will 'pseudo-disable' ourselves to keep from hanging the machine.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

wod_Wait_DMA_Timeout:

        D1 <DMA-BUSY TIMEOUT>
        AssertT 1

        call    dspReset                ; reset the DSP (hack hack!!)
        xor     ax, ax
        mov     [_gfDMABusy], al        ; reset this...
        mov     [_gfEnabled], al        ; reset this...

;BUG <WAITDMA: best way to DISABLE ourselves???>

        dec     ax                      ; seriously BAD NEWS! (-1)

wod_Wait_DMA_Exit:

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL 
;
; @asm WORD | wodPause | This function signifies the DSP to halt
;      DMA at the end of the next 2K buffer.
;
; @rdesc There is no return value.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

cProc wodPause <FAR, PASCAL, PUBLIC> <>
cBegin
         
        D2 <wodPause>

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   To pause, we only need to set the 'pause' flag and wait for the ISR
;   to finish up (by waiting for the last DMA to complete).
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     [_gfWaveOutPaused], TRUE    ; pause...
        cCall   wodWaitForDMA

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL 
;
; @asm WORD | wodResume | This function resumes paused wave output.
;
; @rdesc There is no return value.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

cProc wodResume <FAR, PASCAL, PUBLIC> <>
cBegin
         
        D2 <wodResume>

        test    [_gfWaveOutPaused], 0FFh    ; Q: are we paused?
        jz      wod_Resume_Exit             ; exit if we aren't

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   set sample rate: 1. because suspend/reactivate chain.  2. because we
;   try very hard to recover from bad apps that talk directly to hardware!
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        cCall   dspSetSampleRate, <gwCurSampleRate>

        mov     [_gfWaveOutPaused], 0
        call    wodKickstartWaveOutput

wod_Resume_Exit:

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL
;
; @asm WORD | widStart | This function starts wave input if it isn't started.
;
; @rdesc There is no error return.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

cProc widStart <FAR, PASCAL, PUBLIC> <>
cBegin

        D2 <widStart>

        test    [gbWaveInFlags], WIF_STARTED
        jnz     wid_Start_Exit          ; exit if already started

        or      [gbWaveInFlags], WIF_STARTED

        cCall   dspSpeakerOff           ; turn speaker off while recording
        cCall   dspSetSampleRate, <gwCurSampleRate>

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   set DMA mode - gbDMABuffer must be set to 'pong' here...
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     [gbDMABuffer], DMA_BUFFER_PONG
        mov     ax, 1                   ; start 'wave in' DMA
        call    dmaStartDMA

wid_Start_Exit:
 
cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL 
;
; @asm WORD | widStop | This function stops waveform input if it is started.
;
; @rdesc There is no error return.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

cProc widStop <FAR, PASCAL, PUBLIC> <>
cBegin

        D2 <widStop>

        test    [gbWaveInFlags], WIF_STARTED
        jz      wid_Stop_Exit           ; Q: are we already stopped?

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   stop any DMA the DSP is doing
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
 
        mov     al, HALTDMA
        call    dspWrite
        call    dmaMaskChannel

        cCall   dspSpeakerOn            ; turn speaker back ON (default)

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   we're just trashing the last block, so we need to set the done bit of
;   the header.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        cCall   widSendPartBuffer
        and     [gbWaveInFlags], not WIF_STARTED

wid_Stop_Exit:

cEnd


sEnd CodeSeg

        end
