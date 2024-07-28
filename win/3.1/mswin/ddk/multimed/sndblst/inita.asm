        page 60, 132
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   inita.asm
;
;   Copyright (c) 1991-1992 Microsoft Corporation.  All rights reserved.
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

VDS_VERSION_REQD    equ 0200h           ; V2.00 of VDS for auto-DMA support

        ?PLM=1                          ; Pascal calling convention
        ?WIN=0                          ; NO! Windows prolog/epilog code

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   extrn declarations
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 

        externA <__WinFlags>                ; kernel

        externD <glpVSBDEntry>              ; commona.asm
        externFP <vsbdAcquireSoundBlaster>  ; commona.asm
        externFP <vsbdReleaseSoundBlaster>  ; commona.asm

        externFP <timeGetTime>              ; MMSystem

        externFP <GlobalAlloc>              ; Windows
        externFP <GlobalFree>               ; Windows
        externFP <GetSelectorBase>          ; Windows
        externFP <GlobalPageLock>           ; Windows
        externFP <GlobalPageUnlock>         ; Windows

        externFP <dspRead>                  ; sndblst.asm
        externFP <dspWrite>                 ; sndblst.asm
        externFP <dspISR>                   ; sndblst.asm

        externFP <dspReset>                 ; commona.asm
        externFP <dspSpeakerOn>             ; commona.asm

        externB <gbDMAPhysPage>             ; sndblst.asm
        externW <gwDMAOffset>               ; sndblst.asm
        externW <gwDMAPhysAddr>             ; sndblst.asm
        externW <gwDMASelector>             ; sndblst.asm
        externW <gwMidiOutDelay>            ; sndblst.asm
        externB <gbIntUsed>                 ; sndblst.asm

        externW <gbMidiOutFlags>            ; midia.asm
        externW <gbMidiInFlags>             ; midia.asm

        externW <gbWaveOutFlags>            ; wavea.asm
        externW <gbWaveInFlags>             ; wavea.asm

        externB <_gfWaveOutPaused>          ; wavefix.c

        externB <_gfVerifyInt>              ; initc.c

        externFP <widStop>                  ; wavea.asm
        externFP <widStart>                 ; wavea.asm
        externFP <wodPause>                 ; wavea.asm
        externFP <wodResume>                ; wavea.asm
        externFP <midStop>                  ; midia.asm
        externFP <midStart>                 ; midia.asm

        externFP <widAcquireHardware>       ; wavea.asm
        externFP <widReleaseHardware>       ; wavea.asm
        externFP <wodAcquireHardware>       ; wavea.asm
        externFP <wodReleaseHardware>       ; wavea.asm

        externFP <midAcquireHardware>       ; midia.asm
        externFP <midReleaseHardware>       ; midia.asm
        externFP <modAcquireHardware>       ; midia.asm
        externFP <modReleaseHardware>       ; midia.asm

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
        globalD gdwOldISR,           0
        globalW gwPort,             -1  ; gwPort gbInt and gbDMAChannel *MUST*
        globalB gbInt,              -1  ; ...be initialized to -1, -1, and 1!!!
        globalB gbIntMask,           0  ; saved interrupt mask
        globalB gbDMAChannel,        1
        globalB gbDMAPageReg,        0
        globalW gwEOICommands,       0
        globalW gwDSPVersion,        0  ; DSP version of card we are ENABLED for
        globalW gwMidiInPersistence, 0
        globalB _gfEnabled,          0

        globalB gbIntCount,          0
        globalD isrTestNext,         0
sEnd Data

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   code segment
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

sBegin CodeSeg
        assumes cs, CodeSeg
        assumes ds, Data
        assumes es, nothing

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL
;
; @asm WORD | CheckForMCA | This function returns non-zero if the 
;     machine is an MCA machine.
;
; @rdesc If the machine architecture is Micro-Channel, then non-zero is
;   returned.  Otherwise, zero is returned.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

GETSYSTEMCONFIG     equ 0C0h            ; System config BIOS function

SysDescStruc STRUC
SD_len      dw  ?
SD_model    db  ?
SD_submodel db  ?
SD_ROM_rev  db  ?
SD_feature1 db  ?
SD_feature2 db  ?
SD_feature3 db  ?
SD_feature4 db  ?
SD_feature5 db  ?
SysDescStruc ENDS

; Feature byte 1 bits assignments:

SF1_FD_uses_DMA3        = 10000000b
SF1_PIC_2_present       = 01000000b
SF1_RealTimeClock       = 00100000b
SF1_INT15s_called       = 00010000b
SF1_ExtEventWait        = 00001000b
SF1_EBIOS_allocated     = 00000100b
SF1_MicroChnPresent     = 00000010b

        assumes ds, nothing
        assumes es, nothing

cProc CheckForMCA <NEAR, PASCAL, PUBLIC> <>
cBegin

        xor     cx, cx
        stc                           ; set this in case BIOS doesn't
        mov     ah, GETSYSTEMCONFIG
        int     15h
        jc      CFM_NoMicroChannel    ; successful call?
        or      ah, ah                ; AH = 0 on all modern PC's if call worked
        jnz     CFM_NoMicroChannel
        test    es:[bx.SD_feature1], SF1_MicroChnPresent
        jz      CFM_NoMicroChannel
        inc     cx                    ; return true

CFM_NoMicroChannel:

        mov     ax, cx        

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   WORD NEAR PASCAL vdmadGetVersion(void)
;
;   DESCRIPTION:
;       This function gets the version number of the Virtual DMA Services
;       provided by VDMAD.  This is used to determine if it is safe to do
;       the auto-init DMA.  Under pre 3.1.54 builds of enhanced mode
;       Windows, drivers that programmed auto-init DMA transfers needed to
;       use VADMAD.386.
;
;       We return the 'implementation version number' which should be
;       >= 0103h if VDMAD supports auto-init DMA.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

cProc vdmadGetVersion <NEAR, PASCAL, PUBLIC> <>
        LocalW  wVersion
cBegin

        pusha                       ; int 4Bh trashes a bunch of registers

        mov     ax, 8102h           ; VDS Get Version
        xor     dx, dx              ; flags should be zero
        int     4Bh                 ; fire it off
        jnc     gvExit              ; carry clear if no error

        xor     cx, cx              ; show that there was an error

gvExit:
        mov     wVersion, cx        ; implementation version (needs to be 0200h)
        popa

        mov     ax, wVersion        ; return version number

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL
;
; @asm dspAbsRead | Read a byte from the DSP at port base given.
;
; @rdesc If time out occurs before the DSP is ready then the carry flag
;     is set and no value is returned otherwise the carry flag is cleared
;     and the data value returned in AL.
;
; @parm WORD | wPort | port base to read from.
;
; @comm The timeout value is very machine dependent.
;
; @comm USES: FLAGS, AL--all other registers are preserved.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, nothing
        assumes es, nothing

cProc dspAbsRead <NEAR, PASCAL, PUBLIC> <>
        ParmW   wPort
cBegin
        push    cx
        push    dx

        mov     dx, wPort
        add     dx, DSP_PORT_DATAAVAIL  ; point to data available status port
        mov     cx, 1000                ; set time out value

abs_Read_Wait_Data:
        in      al, dx                  ; get status
        test    al, 80h                 ; Q: MSB set?
        jnz     abs_Read_Data_Avail     ;   Y: jump if data ready
        loop    abs_Read_Wait_Data

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   timed out
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        stc 
        jmp     abs_Read_Exit

abs_Read_Data_Avail:
        sub     dx, (DSP_PORT_DATAAVAIL - DSP_PORT_RDDATA)  ; base + A
        in      al, dx                                      ; get data byte
        clc

abs_Read_Exit:
        pop     dx
        pop     cx

cEnd dspAbsRead

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL
;
; @asm dspAbsWrite | Write a command or data byte to the DSP at the port
;       base given.
;
; @parm WORD | wPort | Port base to write byte to.
;
; @parm BYTE | bByte | The byte to be written.
;
; @comm Carry is set on exit if it times out. The reason the timeout loop
;    is so big is that the SpeakerOff command can take up to 220 msec
;    to execute - if you time out before that the next few DSP commands
;    you write won't work.
;
; @comm Uses only FLAGS--all other registers are preserved.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, nothing
        assumes es, nothing

cProc dspAbsWrite <NEAR, PASCAL, PUBLIC> <ax, cx, dx>
        ParmW   wPort
        ParmB   bByte
cBegin
        mov     dx, wPort
        add     dx, DSP_PORT_WRBUSY     ; point to data status port

        xor     cx, cx                  ; timeout loop counters
        mov     ah, 10

abs_Write_Wait:

        in      al, dx
        test    al, 80h                 ; test busy bit
        jz      abs_Write_Ready         ; exit if not busy

        loop    abs_Write_Wait          ; loop if not timed out
        dec     ah
        jnz     abs_Write_Wait

        stc                             ; set carry to show time out
        jmp     abs_Write_Exit

abs_Write_Ready:

        mov     al, bByte
        out     dx, al                  ; write it (same port)
        .errnz (DSP_PORT_WRBUSY - DSP_PORT_WRDATA)
        clc                             ; no error

abs_Write_Exit:

cEnd dspAbsWrite

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   BOOL NEAR PASCAL vsbdGetEntryPoint(void)
;
;   DESCRIPTION:
;       This function is responsible for setting up the entry point to 
;       the pmode API of VSBD.386 _if_ it is installed.  If VSBD is not
;       installed, zero is returned; a non-zero return means that VSBD
;       is around and responding.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

cProc vsbdGetEntryPoint <NEAR, PASCAL, PUBLIC> <si, di>
cBegin

        xor     di, di                  ; zero ES:DI before call
        mov     es, di
        mov     ax, 1684h               ; get device API entry point
        mov     bx, VSBD_Device_ID      ; virtual device ID
        int     2Fh                     ; call WIN/386 INT 2F API
        mov     ax, es                  ; return farptr
        or      ax, di
        jz      gvsbd_exit
        mov     [glpVSBDEntry].off, di
        mov     [glpVSBDEntry].sel, es

gvsbd_exit:
cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   WORD NEAR PASCAL vsbdGetSoundBlasterInfo(void)
;
;   DESCRIPTION:
;       This function calls VSBD to get the configuration of the SB
;       card that it is virtualizing.  If VSBD is not installed, the carry
;       is set and AX is zero.
;
;   ENTRY:
;       Nothing.
;
;   EXIT:
;       IF Carry Clear
;           AX = DSP Version, major = high byte, minor = low byte
;           BX = Flags
;           CH = IRQ
;           CL = DMA Channel
;           DX = Port base being virtualized
;       ELSE
;           carry set, AX = 0 (VSBD not around)
;
;   USES:
;       Flags, AX, BX, CX, DX
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

cProc vsbdGetSoundBlasterInfo <NEAR, PASCAL, PUBLIC> <>
cBegin
        mov     ax, [glpVSBDEntry].off      ; Q: is VSBD installed?
        or      ax, [glpVSBDEntry].sel
        stc                                 ;   (assume failure)
        jz      vsbd_Get_Info_Done          ;   N: then leave (return 0)

        mov     dx, SB_API_Get_Sound_Blaster_Info
        call    [glpVSBDEntry]

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   Return (carry clear, success):
;       AX = DSP version
;       BX = Flags: fSB_ASB_Acquire_DSP, fSB_ASB_Acquire_AdLib_Synth
;       CH = IRQ
;       CL = DMA channel
;       DX = Base port of SB
;   Return (carry set, fail):
;       AX = 0
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        D1 <VSBD_Info: DSP #AX  Flags #BX  IRQ/DMA #CX  Port #DX>
        clc

vsbd_Get_Info_Done:

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   DWORD FAR PASCAL InitGetConfiguration(void)
;
;   DESCRIPTION:
;       This function simply gets the current configuration _after_
;       the driver has been Enabled.  This is used by the configuration
;       dialog (which is the only C code that requires access to this
;       hardware dependent info).
;
;   ENTRY:
;       Nothing.
;
;   EXIT:
;       AX = port base
;       DL = int
;       DH = DMA Channel
;
;       NOTE: if the driver is not Enabled, AX will be -1, DL will be -1,
;       and DH will be the default DMA channel (1).
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

cProc InitGetConfiguration <FAR, PUBLIC> <>
cBegin

        mov     ax, [gwPort]
        mov     dl, [gbInt]
        mov     dh, [gbDMAChannel]

        D1 <InitGetConfiguration: wPort:#AX  bInt:#DL  bDMAChannel:#DH>

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   WORD NEAR PASCAL InitPreliminary(void)
;
;   DESCRIPTION:
;       This function is called before allowing the driver to install.  It
;       is responsible for checking for critical errors that keep this 
;       driver from working.  For example, this Sound Blaster driver will
;       not work correctly on an MCA machine--so we do NOT allow it to
;       load on an MCA.
;
;       The following checks are made:
;           o check for MCA
;           o check for auto-init support in VDMAD.386 (>= V2.00 of VDS)
;
;       In addition, communication with VSBD is setup and the CPU is
;       profiled for software timing loops.
;
;   ENTRY:
;       Nothing.
;
;   EXIT:
;       If success,
;           zero is returned.
;       else failure,
;           the non-zero IDS_xxx value is returned. If this function
;           fails, it is assumed to be *critical* error and the driver
;           should NOT load.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

cProc InitPreliminary <NEAR, PUBLIC> <si, di>
cBegin

        D1 <InitPreliminary>

        cCall   CheckForMCA
        or      ax, ax                      ; Q: are we running on MCA?
        jz      ipre_Not_MCA                ;   N: good!

        mov     ax, IDS_ERRMCANOTSUPPORTED
        jmp     ipre_Exit

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   If we are running in Enhanced mode, we need to get the entry point to
;   VSBD (if it is installed) and verify that VDMAD is new enough to support
;   auto-init DMA.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

ipre_Not_MCA:

        mov     cx, [__WinFlags]
        test    cx, WF_ENHANCED             ; Q: are we running in enh mode?
        jz      ipre_Profile_CPU            ;   N: then profile CPU for MIDI
        
        cCall   vsbdGetEntryPoint           ; get entry if installed

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   Starting with Win 3.1.55, auto-init DMA is implemented correctly.
;   The implementation version number of VDS is 2.00 for this new
;   VDMAD.  This driver will not work on an old version of VDMAD!
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        cCall   vdmadGetVersion
        cmp     ax, VDS_VERSION_REQD        ; Q: VDMAD support auto-init?
        jae     ipre_Profile_CPU

        mov     ax, IDS_ERROLDVDMAD
        jmp     ipre_Exit                   ; return error

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   Due to a timing bug in the Sound Blaster 2.x and 3.x DSP's, we need to
;   be able to delay >= 400us between MIDI data bytes if MIDI input and 
;   output are going at the same time.  This gives rough estimate of 500us.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

ipre_Profile_CPU:

;BUG <ipre_Profile_CPU: should only do this for MIDI...>

        cCall   timeGetTime
        mov     di, dx
        mov     si, ax

        xor     cx, cx
        loop    $

        cCall   timeGetTime
        sub     ax, si
        sbb     dx, di                     ; DX:AX == num milliseconds
        mov     bx, ax
        shl     bx, 1                      ; assume not > ~32 seconds
        jz      ipre_Profile_CPU_Uh_Oh     ; if 0 then timeGetTime not accurate!
        mov     ax, -1
        div     bx

ipre_Profile_CPU_Uh_Oh:

        mov     [gwMidiOutDelay], ax

        D1 <MODELAY: #AX>

        xor     ax, ax                      ; succeed!
        errn$ ipre_Exit

ipre_Exit:

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   DWORD FAR PASCAL InitVerifyConfiguration(wPort, bInt, bDMAChannel)
;
;   DESCRIPTION:
;       This function is responsible for verifying that the configuration
;       information is correct.  We attempt to detect the hardware and
;       gaurantee that the configuration information is valid.  This function
;       should be written in such a way that it can be called _at any time_
;       (enabled or disabled) to verify settings.  Believe it or not, it is
;       ok (but NOT desirable) to crash here... if we DO crash, the .INI
;       settings will NOT be written, so the user can reboot their machine.
;
;       DO EVERYTHING POSSIBLE TO MAKE THIS FUNCTION AS SAFE AND CRASH
;       PROOF AS POSSIBLE!  DO NOT DO STUPID THINGS!  THIS IS *VERIFYING*
;       SETTINGS-->NOT AUTO-DETECTING THE HARDWARE.
;
;   ENTRY:
;       ParmW   wPort       :   Port base to verify
;       ParmB   bInt        :   IRQ to verify
;       ParmB   bDMAChannel :   DMA channel to verify
;
;   EXIT:
;       If success,
;           AX = 0
;           DX = DSP Version
;       else failure,
;           AX = non-zero IDS_xxx error
;           DX = DSP Version (invalid for bad port)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

cProc InitVerifyConfiguration <FAR, PUBLIC> <>
        ParmW   wPort
        ParmB   bInt
        ParmB   bDMAChannel

        LocalW  wDSPVersion
cBegin
        xor     ax, ax
        mov     wDSPVersion, ax

        mov     dx, wPort
        mov     al, bInt
        mov     ah, bDMAChannel
        D1      <VerifyConfig: wPort:#DX  bInt:#AL  bDMAChannel:#AH>

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   if the driver is enabled then verify by checking globals
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
        test    [_gfEnabled], 0FFh
        jz      VerifyConfig_Not_Enabled

VerifyConfig_Enabled:
        mov     ax, IDS_ERRBADPORT      ; return improper port!
        mov     dx, wPort
        cmp     dx, [gwPort]            ; the same as enabled value?
        jne     VerifyConfig_Exit       ; no.. fail with error
        mov     dx, [gwDSPVersion]
        mov     wDSPVersion, dx 

        mov     ax, IDS_ERRBADINT       ; return improper int!
        mov     dl, bInt
        cmp     dl, [gbInt]             ; is the int the same?
        jne     VerifyConfig_Exit       ; not the same, it is a error

        xor     ax, ax
        jmp     short VerifyConfig_Exit

VerifyConfig_Not_Enabled:

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   if we cannot acquire it, we won't be hurting anything--we either own
;   it which we deal with or another VM owns it and we sorta deal with it.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
        mov     ax, [glpVSBDEntry].off      ; Q: is VSBD installed?
        or      ax, [glpVSBDEntry].sel
        jz      VerifyConfig_Acquire_Continue

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   AX = Base of SB to acquire (for example, 0240h)
;   BX = Flags: fSB_ASB_Acquire_DSP, fSB_ASB_Acquire_AdLib_Synth
;   DX = SB_API_Acquire_Sound_Blaster (1)
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
        mov     ax, wPort                   ; base port to acquire
        mov     bx, fSB_ASB_Acquire_DSP+fSB_ASB_Auto_Reset_DSP
        mov     dx, SB_API_Acquire_Sound_Blaster
        call    [glpVSBDEntry]

VerifyConfig_Acquire_Continue:
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
        cCall   InitVerifyPort, <wPort>
        or      ax, ax
        jnz     VerifyConfig_Exit_Release
        mov     wDSPVersion, dx         ; save this

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
        cCall   InitVerifyInt, <wPort, bInt>
        or      ax, ax
        jnz     VerifyConfig_Exit_Release

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
        cCall   InitVerifyDMA, <wPort, bDMAChannel>
        or      ax, ax
        jnz     VerifyConfig_Exit_Release

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
VerifyConfig_Valid:
        errn$   VerifyConfig_Exit_Release

VerifyConfig_Exit_Release:
        mov     bx, [glpVSBDEntry].off      ; Q: is VSBD installed?
        or      bx, [glpVSBDEntry].sel
        jz      VerifyConfig_Exit

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   AX = Base of SB to release (for example, 0240h)
;   BX = Flags: fSB_ASB_Acquire_DSP, fSB_ASB_Acquire_AdLib_Synth
;   DX = SB_API_Release_Sound_Blaster (2)
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
        push    ax
        mov     ax, wPort                   ; base port to release
        mov     bx, fSB_ASB_Acquire_DSP     ; just the DSP
        mov     dx, SB_API_Release_Sound_Blaster
        call    [glpVSBDEntry]
        pop     ax

VerifyConfig_Exit:
        mov     dx, wDSPVersion
        D1      <VerifyConfig: Error=#AX  DSP Version=#DX>
cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   WORD NEAR PASCAL InitVerifyPort(wPort)
;
;   DESCRIPTION:
;
;       Verify that the hardware is found at the given port.
;
;   ENTRY:
;       ParmW   wPort       :   Port base to verify
;
;   EXIT:
;       If success,
;           AX = 0
;           DX = DSP Version
;       else failure,
;           AX = non-zero IDS_xxx error
;           DX = DSP Version (zero if no card found)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

cProc InitVerifyPort <NEAR, PUBLIC> <>
        ParmW   wPort
cBegin
        cmp     wPort, -1
        je      VerifyPort_Invalid

        call    vsbdGetSoundBlasterInfo
        jc      VerifyPort_VSBD_Not_Installed

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   VSBD.386 is installed, use the info it has
;
;   If carry clear, VSBD is installed and:
;       AX = DSP version
;       BX = Flags: fSB_ASB_Acquire_DSP, fSB_ASB_Acquire_AdLib_Synth
;       CH = IRQ
;       CL = DMA channel
;       DX = Base port of SB
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        xchg    ax, dx
        cmp     wPort, ax               ; Q: same port?
        je      VerifyPort_Valid        ;   Y: success

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   VSBD.386 is either *not* installed, or is virtualizeing another port.
;   so try to verify the port without its help.
;
;   NOTE: if VSBD.386 is installed and is working with same card that we
;   are trying to verify, we won't get here!
;
;   We are not enabled or the port is not the same as the current one, so
;   anything is game!  We don't need to worry about a VxD cuz that is taken
;   care of above...
;
;   At this point, the port could be valid, so check the hardware
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
VerifyPort_VSBD_Not_Installed:
        mov     dx, wPort
        add     dx, DSP_PORT_RESET      ; point to reset port
        mov     al, 1
        out     dx, al                  ; reset active

        mov     cx, 100                 ; wait 3 microseconds for DSP reset
        loop    $                       ; ...to take a good hold

        xor     al, al
        out     dx, al                  ; clear the reset
        sub     dx, DSP_PORT_RESET      ; point back to base

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   read the data port to confirm 0AAH is there
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
        mov     cx, 25                  ; lots of tries

VerifyPort_Reset_Wait:
        cCall   dspAbsRead, <dx>
        jc      VerifyPort_Reset_Not_Done ; jump if it timed out

        cmp     al, 0AAh                ; correct return value ?
        je      VerifyPort_Get_Version  ; jump if it is

VerifyPort_Reset_Not_Done:
        loop    VerifyPort_Reset_Wait

VerifyPort_Invalid:
        mov     ax, IDS_ERRBADPORT
        xor     dx, dx
        jmp     short VerifyPort_Exit

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   at this point, there *should* be a valid card located at wPort... now
;   grab the DSP version
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

VerifyPort_Get_Version:
        cCall   dspAbsWrite, <dx, GETDSPVER>
        jc      VerifyPort_Invalid      ; jump if timed out (shouldn't happen here)

        cCall   dspAbsRead, <dx>
        jc      VerifyPort_Invalid

        mov     ah, al                  ; save major version
        cCall   dspAbsRead, <dx>
        jc      VerifyPort_Invalid

        mov     bx, ax                  ; we have the DSP version

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   Check for a Media Vision Thunder Board.  The DSP version will be 
;   different the second time we query.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        cCall   dspAbsWrite, <dx, GETDSPVER>
        jc      VerifyPort_Invalid      ; jump if timed out (shouldn't happen here)

        cCall   dspAbsRead, <dx>
        jc      VerifyPort_Invalid

        mov     ah, al                  ; save major version
        cCall   dspAbsRead, <dx>
        jc      VerifyPort_Invalid

        mov     dx, bx
        cmp     dx, ax                  ; Q: Thunder Board?
        je      VerifyPort_Valid        ;   N: then continue normally...

        D1      <THUNDER BOARD DETECTED>
        or      dh, 80h                 ; flag we have a Thunder Board
        errn$   VerifyPort_Valid

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   we have found the hardware at the specified port
;
;       DX = DSP version (high bit set in DH if Thunder Board...)
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
VerifyPort_Valid:
        xor     ax, ax                  ; show no error
        mov     [gwDSPVersion], dx      ; save this.

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
VerifyPort_Exit:

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   WORD NEAR PASCAL InitVerifyInt(wPort, bInt)
;
;   DESCRIPTION:
;
;       Verify that the hardware is using the specifed interrupt.
;
;   ENTRY:
;       ParmW   wPort       :   port (assumed correct)
;       ParmW   bInt        :   interrupt to verify
;
;   EXIT:
;       If success,
;           AX = 0
;       else failure,
;           AX = non-zero IDS_xxx error code
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

NUM_INTS_TO_VERIFY = 10

cProc InitVerifyInt <NEAR, PUBLIC> <si, di>
        ParmW   wPort
        ParmB   bInt
cBegin

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   int must be from 1-15, or we know it is bad.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
        mov     al, bInt
        or      al, al
        jz      VerifyInt_Invalid
        cmp     al, 0Fh
        jbe     VerifyInt_Verify

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
VerifyInt_Invalid:
        mov     ax, IDS_ERRBADINT
        jmp     VerifyInt_Exit

VerifyInt_Valid:
        xor     ax, ax
        jmp     VerifyInt_Exit

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   Hook the interrupt vector and tell the DSP to fire an interrupt.
;   See if it happens, and do it a few times just to be sure.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
VerifyInt_Verify:

        test    [_gfVerifyInt], 0FFh        ; over-ride just in case
        jz      VerifyInt_Valid

        cCall   GlobalPageLock, <cs>        ; just for fun!

        mov     dx, cs                      ; !!! seg isrTest
        mov     ax, offset isrTest
        mov     cl, bInt

        EnterCrit

        cCall   InitSetInterruptVector, <cx, dx, ax>

        mov     [isrTestNext].off, ax
        mov     [isrTestNext].sel, dx       ; set oldISR, save

        LeaveCrit

        cCall   InitSetIntMask, <bInt, 0>   ; unmask the IRQ
        push    ax                          ; save old state

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
        mov     cx, NUM_INTS_TO_VERIFY
        mov     dx, wPort                   ; get port
        mov     si, IDS_ERRBADINT           ; assume failure
        mov     bl, [gbIntCount]            ; get current int count
        add     bl, cl                      ; this is what we expect
    
VerifyInt_Loop:
        pusha       
        cCall   InitSetIntMask, <bInt, 0>   ; unmask the IRQ!
        popa

        mov     al, [gbIntCount]

        cCall   dspAbsWrite, <dx, GENERATEINT>
        jc      VerifyInt_BadInt_dspWrite

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   wait for the interrupt to fire; time out after 64k tries
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
        push    cx
        xor     cx, cx
@@:     cmp     [gbIntCount], al
        loope   @b
        pop     cx
        je      VerifyInt_BadInt_TimeOut    ; we never got an interrupt

        inc     al
        cmp     [gbIntCount], al
        jne     VerifyInt_BadInt            ; we got too many

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;
;   HACK HACK HACK!
;
;   On some machines the default handler for IRQ7 does not do an EOI
;   so we will check for this and do the EOI for it.
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
        push    bx
        EnterCrit
        mov     al, 0Bh                     ; read the ISR
        out     20h, al
        pause
        in      al, 20h

        or      al, al                      ; are any IRQ left un-EOIed?
        jz      @F

        D1      <ISR = #al, doing non-specific EOI>

        mov     al, 20h                     ; send non-specific EOI
        out     20h, al
@@:
        LeaveCrit
        pop     bx

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   tell the DSP we did the interrupt.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
        add     dx, DSP_PORT_DATAAVAIL      ; point to data avail port
        in      al, dx                      ; read status (reset interrupt)
        sub     dx, DSP_PORT_DATAAVAIL      ; point to data avail port

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
        loop    VerifyInt_Loop              ; try some more, just to be sure.

        cmp     bl, [gbIntCount]            ; did we get exactly NUM_INTS???
        jne     VerifyInt_BadInt            ; ...we got more/less card not found

VerifyInt_GoodInt:
        D1      <VerifyInt: IRQ appears to be correct>
        xor     si, si                      ; show success
        jmp     short VerifyInt_Done

VerifyInt_BadInt_TimeOut:
        D1      <VerifyInt: interrupt TIME OUT!>
        jmp     short VerifyInt_Done

VerifyInt_BadInt_dspWrite:
        D1      <VerifyInt: dspWrite TIME OUT!>
        jmp     short VerifyInt_Done

VerifyInt_BadInt:
        D1      <VerifyInt: IRQ not valid>
        errn$   VerifyInt_Done

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
VerifyInt_Done:
        pop     ax                          ; restore PIC
        cCall   InitSetIntMask, <bInt, ax>

        mov     cl, bInt
        cCall   InitSetInterruptVector, <cx, isrTestNext>

        cCall   GlobalPageUnlock, <cs>      ; more fun!

        mov     ax, si                      ; get error code
        errn$   VerifyInt_Exit              ; and exit

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

VerifyInt_Exit:

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   isrTest
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

public isrTest
isrTest proc far

        push    ds

        push    DGROUP                  ; set up local DS
        pop     ds
        assumes ds, Data

        inc     [gbIntCount]            ; we got an interrupt

        D1      <TEST-INTERRUPT>

        push    [isrTestNext].sel
        push    [isrTestNext].off

        push    bp                      ; restore DS from stack
        mov     bp, sp
        mov     ds, [bp + 6]            ; stack: [ds] [gdwOldISR].sel [gdwOldISR].off [bp]
        assumes ds, nothing
        pop     bp
        retf    2                       ; chain!

isrTest endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   WORD NEAR PASCAL InitVerifyDMA(wPort, bDMA)
;
;   DESCRIPTION:
;
;       Verify that the hardware is using the specifed DMA channel.
;
;   ENTRY:
;       ParmW   wPort       :   Port (assumed correct)
;       ParmW   bDMA        :   DMA channel to verify
;
;   EXIT:
;       If success,
;           AX = 0
;       else failure,
;           AX = non-zero IDS_xxx error code
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

cProc InitVerifyDMA <NEAR, PUBLIC> <>
        ParmW   wPort
        ParmB   bDMA
cBegin
        xor     ax, ax                  ; always valid
cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   BOOL NEAR PASCAL InitSetConfiguration(wPort, bInt, bDMAChannel, wMidiInPersistence)
;
;   DESCRIPTION:
;       This routine is used to initialize all variables needed to handle
;       the hardware based on INI settings.  It is _assumed_ that the IRQ
;       and Port Base are valid!
;
;   ENTRY:
;       ParmW wPort             :   The port base to be 'set.'
;       ParmB bInt              :   The IRQ to be 'set.'
;       ParmB bDMAChannel       :   The DMA channel to be 'set.'
;       ParmW wMidiInPersistence:   MIDI input persistence
;
;   EXIT:
;       FALSE if failed; non-zero if success
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

cProc InitSetConfiguration <NEAR, PUBLIC> <>
        ParmW   wPort
        ParmB   bInt
        ParmB   bDMAChannel
        ParmW   wMidiInPersistence
cBegin
        AssertT [_gfEnabled]             ; !!! this would be very bad

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   set the port base and MIDI input persistence
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     ax, wPort
        mov     [gwPort], ax
        mov     ax, wMidiInPersistence
        mov     [gwMidiInPersistence], ax
        mov     al, bInt
        mov     [gbInt], al             ; set 'gbInt' to return value

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;  set the IRQ specific variables appropriately...
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        cmp     al, 8
        jae     isc_Slave_IRQ

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   IRQ is for 'master' PIC--set global variables for IRQ appropriately
;   AL = bInt
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     ah, al
        or      ah, 60h                 ; specific EOI command for master
        xor     al, al                  ; no slave command
        jmp     short isc_Save_IRQ_Info

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   IRQ is for 'slave' PIC--set global variables for IRQ appropriately
;   AL = bInt
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

isc_Slave_IRQ:
        and     al, 07h                 ; map to slave IRQ
        or      al, 60h                 ; specific EOI for slave
        mov     ah, 62h                 ; specific EOI (#2) command for master

isc_Save_IRQ_Info:

        mov     [gwEOICommands], ax     ; save EOI commands for ISR
        errn$ isc_Set_DMA_Channel

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   Now compute the DMA channel stuff
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

isc_Set_DMA_Channel:

        mov     al, bDMAChannel
        or      al, al
        jne     @F
        mov     ah, 87h                 ; DMA channel 0 page register
        jmp     isc_Save_DMA_Info

@@:     cmp     al, 2
        jne     @F
        mov     ah, 81h                 ; DMA channel 2 page register
        jmp     isc_Save_DMA_Info

@@:     cmp     al, 3
        jne     @F
        mov     ah, 82h                 ; DMA channel 3 page register
        jmp     isc_Save_DMA_Info

@@:     mov     ax, 8301h               ; default to DMA channel 1

isc_Save_DMA_Info:

        mov     [gbDMAChannel], al      ; save info...
        mov     [gbDMAPageReg], ah

        errn$ isc_Exit

isc_Exit:

ifdef DEBUG
        mov     ax, [gwPort]
        D1      <gwPort:#AX>
        mov     al, [gbInt]
        D2      <gbInt:#AL>
        mov     al, [gbDMAChannel]
        D1      <gbDMAChannel:#AL>
        mov     al, [gbDMAPageReg]
        D2      <gbDMAPageReg:#AL>
endif
        mov     ax, 1                   ; return success

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL
;
; @asm dmaAllocateBuffer | This function allocates a page locked DMA buffer
;     which is guaranteed not to contain a physical page boundary.
;
; @parm wBufferSize | The size of the buffer in bytes.  It must be <= 4k!!!
;
; @rdesc The return value is non-zero if successful, and zero on failure.
;
;     The buffer selector and offset are stored in global variables:
;
;       gwDMASelector   : pmode selector of DMA buffer
;       gwDMAOffset     : offset to even page boundary
;       gwDMAPhysAddr   : for DMA address register
;       gbDMAPhysPage   : for DMA page register
;
; @comm
;     The algorithm used here is very simple.  It first attempts
;     to allocate the desired buffer size.  If this fails,
;     it returns with an out of memory error.  If the buffer
;     is allocated, it checks to see if it contains a physical
;     page boundary.  If not, it saves the selector and
;     offset of the buffer.  If the block contains a physical
;     page boundary then it frees the block and attempts to
;     allocate a block of twice the size.  If the allocation
;     fails it returns an out of memory error.  If successful, it
;     tests the first half of the block for a physical page 
;     boundary.  If none is found, it sets the selector and
;     offset.  If it contains a boundary, it sets the
;     same selector but the offset is set to the upper half
;     of the block which is assumed to be boundary free.
;
;     This is obviously a bit simple, but there is currently no
;     way to move a block to a better place.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

cProc dmaAllocateBuffer <NEAR, PASCAL, PUBLIC> <>
        ParmW   wBufferSize
cBegin

        D2 <dmaAllocateBuffer>

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   This code assumes the caller wants <= 4kb, so fail if this is not the
;   case.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     ax, wBufferSize
        cmp     ax, P_SIZE              ; Q: size <= 4kb?
        jbe     alloc_Continue          ;   Y: then attempt allocation

        xor     ax, ax                  ;   N: fail!
        jmp     alloc_Exit

alloc_Continue:

        xor     dx, dx
        mov     [gwDMAOffset], dx       ; assume offset is zero for now
        cCall   GlobalAlloc, <GMEM_FIXED+GMEM_SHARE, dx, ax>

        mov     [gwDMASelector], ax
        or      ax, ax                  ; Q: failed?
        jz      alloc_Exit              ;   Y: hmm... bad news!

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   check if it contains a page boundary (it almost ALWAYS contains a page
;   boundary...)
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        cCall   GetSelectorBase, <ax>   ; returns DX:AX = linear base
        or      dh, dh                  ; Q: is the base > 16Mb?
        jnz     alloc_Bad_Mem           ;   Y: yes it is not usable!

        test    ax, (P_SIZE - 1)        ; Q: even page boundary?
        jz      alloc_Success           ;   Y: DX:AX == phys addr...

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   this block is no good so free it and try a bigger chunk...
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        cCall   dmaFreeBuffer           ; free previous block
                
        mov     ax, (P_SIZE * 2)        ; try 2 pages...
        xor     dx, dx
        cCall   GlobalAlloc, <GMEM_FIXED+GMEM_SHARE, dx, ax>

        mov     [gwDMASelector], ax
        or      ax, ax                  ; Q: failed?
        jz      alloc_Exit              ;   Y: hmm... bad news!

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   calculate offset into double size block
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        cCall   GetSelectorBase, <ax>
        or      dh, dh                  ; Q: is the base > 16Mb?
        jnz     alloc_Bad_Mem           ;   Y: yes it is not usable!

        mov     bx, ax
        neg     bx                  
        and     bx, (P_SIZE - 1)        ; BX contains offset into linear pg.

        mov     [gwDMAoffset], bx
        add     ax, bx
        adc     dl, dh
        jmp     alloc_Success           ; DX:AX == phys addr...

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
; couldn't allocate a physically contiguous block of memory... fail!
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

alloc_Bad_Mem:

        cCall   dmaFreeBuffer           ; free bogus memory...
        xor     ax, ax                  ; fail!
        jmp     alloc_Exit

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   DX:AX contains linear base of DMA buffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

alloc_Success:

        mov     [gbDMAPhysPage], dl     ; save it
        mov     [gwDMAPhysAddr], ax

ifdef DEBUG
        mov     cx, [gwDMASelector]
        mov     bx, [gwDMAOffset]
        D1      <DMA BUFFER: DMA=#cx:#bx, addr=#dl#ax>
endif
        mov     ax, 1

alloc_Exit:

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL
;
; @api BOOL | dmaFreeBuffer | Frees a DMA buffer allocated with previous call
;       to dmaAllocateBuffer.
;
; @rdesc The return value is non-zero if successful, and zero on failure.
;
; @xref dmaAllocateBuffer
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

cProc dmaFreeBuffer <NEAR, PASCAL, PUBLIC> <>
cBegin

        D2 <dmaFreeBuffer>

        mov     ax, [gwDMASelector]
        or      ax, ax
        jz      free_Exit

        cCall   GlobalFree, <ax>
        xor     ax, ax
        mov     [gwDMASelector], ax
        mov     [gwDMAOffset], ax
        mov     [gwDMAPhysAddr], ax
        mov     [gbDMAPhysPage], al

free_Exit:

        inc     ax                      ; succeed!

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   BOOL NEAR PASCAL InitResetAdapter(bMode)
;
;   DESCRIPTION:
;       This function is responsible for putting the adapter into a known
;       state from a random state.  It is should ONLY be called during 
;       Enable and Disable.
;
;   ENTRY:
;       ParmB bMode:  IRA_MODE_POWERON - back to power on state (Disable)
;                     IRA_MODE_DEFAULT - to normal 'sndblst.drv' state (Enable)
;
;   EXIT:
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

cProc InitResetAdapter <NEAR, PUBLIC> <>
        ParmB   bMode
cBegin

        D2 <InitResetAdapter>

        cCall   vsbdAcquireSoundBlaster
        or      ax, ax
        jnz     ira_Exit

        call    dspReset
        or      ax, ax
        jnz     ira_Release_Hardware

        cmp     bMode, IRA_MODE_DEFAULT
        jne     ira_Release_Hardware

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   If VSBD is installed, use the DSP version that it already knows.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        call    vsbdGetSoundBlasterInfo
        jnc     ira_Save_DSP_Version

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   Get the DSP version.  We just did a fresh reset, so this is perfectly
;   valid.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     al, GETDSPVER
        call    dspWrite
        jc      ira_Fail_And_Release    ; jump if timed out (shouldn't happen here)

        call    dspRead
        jc      ira_Fail_And_Release

        mov     dh, al                  ; save major version
        call    dspRead
        jc      ira_Fail_And_Release

        mov     ah, dh                  ; AL has minor, AH has major

ira_Save_DSP_Version:

        mov     [gwDSPVersion], ax
        D1 <DSP VERSION: #AX>

        cCall   dspSpeakerOn
        xor     ax, ax
        jz      ira_Release_Hardware

ira_Fail_And_Release:

        mov     ax, -1                  ; fail! (but release hardware)

ira_Release_Hardware:

        cCall   vsbdReleaseSoundBlaster

ira_Exit:

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   BOOL NEAR PASCAL InitSetIntMask(bIRQ, bMask)
;
;   DESCRIPTION:
;       This function sets or unsets interrupt vector mask.
;
;   ENTRY:
;       ParmB   bIRQ        :   The IRQ (0 - 15) to mask/unmask
;       ParmB   bMask       :   The mask
;
;   EXIT:
;       AX    :   The return value is the previous interrupt mask.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

cProc InitSetIntMask <NEAR, PUBLIC> <>
        ParmB   bIRQ
        ParmB   bMask
cBegin
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   see if we need to talk to the slave or master PIC
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
        mov     cl, bIRQ
        mov     dx, PIC_IMR_MASTER
        cmp     cl, 8
        jb      SetIntMask_Master
        and     cl, 07h
        mov     dx, PIC_IMR_SLAVE
SetIntMask_Master:

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   compute the interrupt mask.
;       DX = slave or master mask register
;       CL = 0-7 bit to set/clear
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
        mov     ch, 1                   ; CH = 1
        shl     ch, cl                  ; CH = int mask

        mov     cl, bMask               ; get mask
        or      cl, cl
        jz      SetIntMask_UnMask
        mov     cl, ch
SetIntMask_UnMask:

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   CH  = PIC mask         (1 << (bInt&7))
;   CL  = wanted mask      bMask ? ch : 0
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
        not     ch                      ; we need inverse of mask

        EnterCrit                       ; !!! Trashes BX !!!
        in      al, dx                  ; grab current mask
        mov     ah, al                  ; save it
        and     al, ch                  ; clear bit
        or      al, cl                  ; clear or set based on bMask
        cmp     al, ah                  ; don't set the same state again!
        je      SetIntMask_Same
        out     dx, al                  ; enable/disable ints...
SetIntMask_Same:
        LeaveCrit                       ; !!! Trashes BX !!!

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   we have set/cleared the PIC, now return the old state.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
        not     ch                      ; return previous mask state
        mov     al, ah
        and     al, ch
        xor     ah, ah
cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   LPFUNC NEAR PASCAL InitSetInterruptVector(bIRQ, lpNewISR)
;
;   DESCRIPTION:
;       This function takes the IRQ and sets the appropriate interrupt
;       vector; and returns the pointer to the previous handler of the IRQ.
;
;   ENTRY:
;       ParmB   bIRQ        :   The IRQ (0 - 15) to install handler for.
;       ParmD   lpNewISR    :   The handler
;
;   EXIT:
;       DX:AX   :   The return value is the previous interrupt handler.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

cProc InitSetInterruptVector <NEAR, PUBLIC> <>
        ParmB   bIRQ
        ParmD   lpNewISR
cBegin

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   convert IRQ to interrupt vector.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     al, bIRQ
        mov     ah, 08h
        cmp     al, ah                  ; Q: slave or master IRQ?
        jl      isv_Continue

        mov     ah, (70h - 08h)         ;   slave

isv_Continue:

        add     al, ah                  ; AL = interrupt vector

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   get old interrupt vector (AL == interrupt vector)
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     ah, 35h
        int     21h                     ; get the old vector in es:bx

        push    es                      ; save for a bit
        push    bx

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   set new interrupt vector (AL == interrupt vector)
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     ah, 25h
        push    ds
        lds     dx, lpNewISR
        assumes ds, nothing
        int     21h                     ; set the new vector
        pop     ds

        pop     ax                      ; restore old ISR for return value
        pop     dx                      ; ... DX:AX is old handler

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL 
;
; @api BOOL | widSuspend | This function suspends wave input.
;
; @rdesc The return is zero if successful, and non-zero on failure.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

cProc widSuspend <NEAR, PUBLIC> <>
cBegin

        D2 <widSuspend>

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   Release the hardware if we own it.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        xor     ax, ax                         ; assume success

        test    [gbWaveInFlags], WIF_ALLOCATED
        jz      wid_Suspend_Exit               ; exit if hardware not allocated

        test    [gbWaveInFlags], WIF_STARTED   ; Q: currently active?
        jz      wid_Suspend_Release            ;   N: then just release hardware

        call    widStop
        or      [gbWaveInFlags], WIF_RESTART   ; flag we need to restart...

wid_Suspend_Release:

        cCall   widReleaseHardware             ; AX == 0 if success...
        or      [gbWaveInFlags], WIF_SUSPENDED

wid_Suspend_Exit:

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL 
;
; @api BOOL | widReactivate | This function resumes paused wave input.
;
; @rdesc The return is zero if successful, and non-zero on failure.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

cProc widReactivate <NEAR, PUBLIC> <>
cBegin

        D2 <widReactivate>

        test    [gbWaveInFlags], WIF_SUSPENDED
        jz      wid_Reactivate_Success  ; succeed if not suspended...

        D2 <reactivating>

        cCall   widAcquireHardware
        or      ax, ax                  ; Q: we get it?
        jnz     wid_Reactivate_Exit     ;   N: fail!

        test    [gbWaveInFlags], WIF_RESTART
        jz      wid_Reactivate_Success  ; succeed if wasn't started...

        call    widStart                ; start it back up

wid_Reactivate_Success:

        and     [gbWaveInFlags], not (WIF_RESTART or WIF_SUSPENDED)
        xor     ax, ax                  ; success

wid_Reactivate_Exit:

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL 
;
; @api BOOL | wodSuspend | This function suspends wave output.
;
; @rdesc The return is zero if successful, and non-zero on failure.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        assumes ds, Data
        assumes es, nothing

cProc wodSuspend <NEAR, PUBLIC> <>
cBegin

        D2 <wodSuspend>

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   Release the hardware if we own it.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        xor     ax, ax                          ; assume success

        test    [gbWaveOutFlags], WOF_ALLOCATED
        jz      wod_Suspend_Exit                ; exit if hardware not allocated

        test    [_gfWaveOutPaused], 0FFh        ; Q: currently paused?
        jnz     wod_Suspend_Release             ;   Y: then DON'T flag restart!

        call    wodPause
        or      [gbWaveOutFlags], WOF_RESTART   ; flag we need to restart...

wod_Suspend_Release:

        cCall   wodReleaseHardware              ; AX == 0 if success...
        or      [gbWaveOutFlags], WOF_SUSPENDED

wod_Suspend_Exit:

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL 
;
; @api BOOL | wodReactivate | This function resumes suspended wave output.
;
; @rdesc The return is zero if successful, and non-zero on failure.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

cProc wodReactivate <NEAR, PUBLIC> <>
cBegin

        D2 <wodReactivate>

        test    [gbWaveOutFlags], WOF_SUSPENDED
        jz      wod_Reactivate_Success  ; succeed if not suspended...

        D2 <reactivating>

        cCall   wodAcquireHardware
        or      ax, ax                  ; Q: we get it?
        jnz     wod_Reactivate_Exit     ;   N: fail!

        test    [gbWaveOutFlags], WOF_RESTART
        jz      wod_Reactivate_Success  ; succeed if wasn't started...

        call    wodResume               ; start it back up

wod_Reactivate_Success:

        and     [gbWaveOutFlags], not (WOF_RESTART or WOF_SUSPENDED)
        xor     ax, ax                  ; success

wod_Reactivate_Exit:

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL 
;
; @api BOOL | midSuspend | This function suspends midi input.
;
; @rdesc The return is zero if successful, and non-zero on failure.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

cProc midSuspend <NEAR, PUBLIC> <>
cBegin

        D2 <midSuspend>

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   Release the hardware if we own it.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        xor     ax, ax                         ; assume success

        test    [gbMidiInFlags], MIF_ALLOCATED
        jz      mid_Suspend_Exit               ; exit if hardware not allocated

        test    [gbMidiInFlags], MIF_STARTED   ; Q: currently active?
        jz      mid_Suspend_Release            ;   N: then just release hardware

        call    midStop
        or      [gbMidiInFlags], MIF_RESTART   ; flag we need to restart...

mid_Suspend_Release:

        cCall   midReleaseHardware             ; AX == 0 if success...
        or      [gbMidiInFlags], MIF_SUSPENDED

mid_Suspend_Exit:

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL 
;
; @api BOOL | midReactivate | This function resumes suspended midi input.
;
; @rdesc The return is zero if successful, and non-zero on failure.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

cProc midReactivate <NEAR, PUBLIC> <>
cBegin

        D2 <midReactivate>

        test    [gbMidiInFlags], MIF_SUSPENDED
        jz      mid_Reactivate_Success  ; succeed if not suspended...

        D2 <reactivating>

        cCall   midAcquireHardware
        or      ax, ax                  ; Q: we get it?
        jnz     mid_Reactivate_Exit     ;   N: fail!

        test    [gbMidiInFlags], MIF_RESTART
        jz      mid_Reactivate_Success  ; succeed if wasn't started...
        call    midStart                ; start it back up

mid_Reactivate_Success:

        and     [gbMidiInFlags], not (MIF_RESTART or MIF_SUSPENDED)
        xor     ax, ax                  ; success

mid_Reactivate_Exit:

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL 
;
; @api BOOL | modSuspend | This function suspends midi output.
;
; @rdesc The return is zero if successful, and non-zero on failure.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

cProc modSuspend <NEAR, PUBLIC> <>
cBegin

        D2 <modSuspend>

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   Release the hardware if we own it.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        xor     ax, ax
        test    [gbMidiOutFlags], MOF_ALLOCATED
        jz      mod_Suspend_Exit

        cCall   modReleaseHardware
        or      [gbMidiOutFlags], MOF_SUSPENDED

mod_Suspend_Exit:

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL 
;
; @api BOOL | modReactivate | This function resumes suspended midi output.
;
; @rdesc The return is zero if successful, and non-zero on failure.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

cProc modReactivate <NEAR, PUBLIC> <>
cBegin

        D2 <modReactivate>

        xor     ax, ax                  ; assume success
        test    [gbMidiOutFlags], MOF_SUSPENDED
        jz      mod_Reactivate_Exit

        D2 <reactivating>

        cCall   modAcquireHardware
        or      ax, ax                  ; Q: can we get hardware?
        jnz     mod_Reactivate_Exit     ;   N: bummer (stay suspended!)

        and     [gbMidiOutFlags], not MOF_SUSPENDED
        xor     ax, ax                  ; success

mod_Reactivate_Exit:

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL 
;
; @asm WORD | Enable | This function enables the driver.  It will allocate
;     DMA buffers, hook interrupts and validate the hardware.
;
; @rdesc The return value is zero if the call is successful; otherwise
;     an error code is returned.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

cProc Enable <FAR, PASCAL, PUBLIC> <>
cBegin
         
        D1 <Enable>

        mov     al, [gbInt]             ; if not configured, DON'T enable!
        cbw     
        or      ax, [gwPort]
        cmp     ax, -1
        jne     enable_We_Are_Configured


enable_Fail:

        mov     ax, -1                  ; return error!
        jmp     enable_Exit 

        
enable_We_Are_Configured:

        test    [_gfEnabled], 0FFh      ; Q: are we already enabled?
        jnz     enable_Success          ;   Y: then succeed

        cCall   dmaAllocateBuffer, <DMAPHYSBUFSIZE>
        or      ax, ax
        jz      enable_Fail             ; ...fail if we can't alloc DMA buffer!


;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   reset the adapter because we don't know what state it is in
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        cCall   InitResetAdapter, <IRA_MODE_DEFAULT>
        or      ax, ax
        jnz     enable_Fail

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   set our ISR--save old interrupt vector
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     dx, seg dspISR
        mov     ax, offset dspISR
        mov     bl, [gbInt]
        cCall   InitSetInterruptVector, <bx, dx, ax>

        mov     [gdwOldISR].sel, dx
        mov     [gdwOldISR].off, ax

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   enable interrupts for the Sound Blaster
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        cCall   InitSetIntMask, <gbInt, 0>
        mov     [gbIntMask], al   ; save old state

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   Reactivate MIDI and/or Wave if was going during Disable
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        cCall   widReactivate
        cCall   wodReactivate
        cCall   midReactivate
        cCall   modReactivate
        errn$ enable_Success

enable_Success:

        D1      <Enable ok>
        inc     [_gfEnabled]            ; mark as being enabled
        xor     ax, ax                  ; no error

enable_Exit:

cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL 
;
; @asm WORD | Disable | This function disables the driver.  It disables the
;     the hardware, unhooks interrupts and frees any memory allocated.
;
; @rdesc The return value is zero if the call is successful; otherwise
;     an error code is returned.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assumes ds, Data
        assumes es, nothing

cProc Disable <FAR, PASCAL, PUBLIC> <>
cBegin

        D1 <Disable>

        test    [_gfEnabled], 0FFh      ; Q: are we currently enabled?
        jz      disable_Exit            ;   N: then exit (success)


;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   Suspend MIDI and/or Wave if it is going
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        cCall   widSuspend
        cCall   wodSuspend
        cCall   midSuspend
        cCall   modSuspend

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   restore PIC to original state
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        cCall   InitSetIntMask, <gbInt, gbIntMask>

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   restore hardware and the old interrupt vector
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

disable_Success:

        cCall   InitResetAdapter, <IRA_MODE_POWERON>

        mov     bl, [gbInt]
        mov     ax, [gdwOldISR].off
        mov     dx, [gdwOldISR].sel

        cCall   InitSetInterruptVector, <bx, dx, ax>
        cCall   dmaFreeBuffer           ; free the DMA buffer

disable_Exit:

        xor     ax, ax                  ; no error
        mov     [_gfEnabled], al        ; flag we are disabled now

cEnd

sEnd CodeSeg

        end
