        page    60, 132

;******************************************************************************
        title   VALIDATE.ASM - Detection routines
;******************************************************************************
;---------------------------------------------------------------------------;
;
;  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;  PURPOSE.
;
;  Copyright (c) 1992- 1995     Microsoft Corporation.  All Rights Reserved.
;
;---------------------------------------------------------------------------;
;
;   Title:    MSSNDSYS.386 - MICROSOFT Windows Sound System 386 Driver
;
;   Module:   VALIDATE.ASM - Hardware validation routines
;
;   Version:  1.10
;
;   Date:     April 23, 1993
;
;******************************************************************************
;
;   Functional Description:
;      DAK configuration detection routines
;
;******************************************************************************

        .386p

;==============================================================================
;                             I N C L U D E S
;==============================================================================

        .xlist
        include vmm.inc
        include debug.inc
        include mssndsys.inc
        include equates.inc
        .list

;==============================================================================
;                   N O N P A G E A B L E   D A T A
;==============================================================================

VxD_LOCKED_DATA_SEG

;------------------------------------------------------------------------------
;                 E X T E R N A L    R E F E R E N C E S
;------------------------------------------------------------------------------

EXTRN gbMode:BYTE
EXTRN gbMute:BYTE

VxD_LOCKED_DATA_ENDS

;==============================================================================
;                          P A G E A B L E   D A T A
;==============================================================================

VxD_PAGEABLE_DATA_SEG

EXTRN gabIRQValid:BYTE
EXTRN gabIRQConfigCodes:BYTE
EXTRN gabDMAValid:BYTE

;------------------------------------------------------------------------------
;          G L O B A L   I N I T   D A T A   D E C L A R A T I O N S
;------------------------------------------------------------------------------

gbDMAPageReg            db      87h, 83h, 81h, 82h, 00h, 8bh, 89h, 8ah

VxD_PAGEABLE_DATA_ENDS

;==============================================================================
;                      N O N P A G E A B L E   C O D E
;==============================================================================

VxD_LOCKED_CODE_SEG

;------------------------------------------------------------------------------
;                   E X T E R N A L    R E F E R E N C E S
;------------------------------------------------------------------------------

EXTRN CODEC_RegRead:NEAR
EXTRN CODEC_RegWrite:NEAR
EXTRN CODEC_WaitForReady:NEAR
EXTRN CODEC_ExtMute:NEAR
EXTRN OPL3_RegWrite:NEAR
EXTRN OPL3_IODelay:NEAR

VxD_LOCKED_CODE_ENDS

;==============================================================================
;                             P N P   C O D E
;==============================================================================

VxD_PNP_CODE_SEG

;---------------------------------------------------------------------------;
;
;   Is_CODEC_Valid
;
;   DESCRIPTION:
;       Verifies the IO address in EDX.
;
;       The following tests are performed:
;
;       1) Checks the CODEC_ADDRESS_REG for "busy".
;          - Should be non-busy.
;
;       2) Checks the CODEC's version.
;           - Should be one of the known versions.
;
;       3) Attempts a write to the lower nibble of
;          the CODEC's miscellaneous register.
;          - Should fail.
;
;   ENTRY:
;       EDX = IO base of CODEC to try.
;
;   EXIT:
;       if Carry clear
;           IO Address is valid
;       else Carry set
;           IO Address is not valid.
;
;   USES:
;       Flags
;
;---------------------------------------------------------------------------;

BeginProc Is_CODEC_Valid

        push    eax
        push    ecx
        push    edx

        xor     ecx, ecx

        Trace_Out "MSSNDSYS: Is_CODEC_Valid: IOAddr=#DX"

        ;
        ; Check to see if the WSS is busy - it shouldn't be
        ; so fail if so.
        ;

        in      al, dx                          
        test    al, 80h                         ; Q: Is board "busy"?
        jnz     SHORT ICV_Bad                   ;    Y: Can't be WSS.

        ;
        ; Check the CODEC version.  This is an indexed register.
        ; Select the miscellaneous register and read the version.
        ;

        mov     cl, al                          ; save off the "address"
                                                ; in case we fail.

        mov     al, CODEC_REG_MISC
        out     dx, al
        inc     edx
        .errnz  (SS_CODEC_ADDRESS + 1) - SS_CODEC_DATA
ifdef AZTECH
        xor     al, al
        out     dx, al                          ; set to mode 1
endif
        in      al, dx

        test    al, 80h
        jz      SHORT ICV_Not_CS
        and     al, NOT( CS4231_MISC_MODE2 )

ICV_Not_CS:
        cmp     al, VER_AD1848J
        je      SHORT ICV_VersionOK
        cmp     al, VER_AD1848K
        je      SHORT ICV_VersionOK
        cmp     al, VER_CSPROTO
        je      SHORT ICV_VersionOK
        cmp     al, VER_CS4248
        je      SHORT ICV_VersionOK

        ;
        ; Uh-oh!  We've either got the wrong chipset
        ; (Compaq's 1.2 CODEC) or it just isn't our CODEC.
        ;

        jmp     SHORT ICV_RestoreAddress

ICV_VersionOK:
        shl     ecx, 8                          ; next location to store
        mov     cl, al                          ; save the "version"

        ;
        ; NOTE!  Do not WRITE to the upper nibble of this
        ; register.  A very important safety tip:
        ;
        ; "You can put the chip into an undocumented test mode
        ;  and then you're history."
        ;
        ; The revision number of the MISCELLANEOUS register
        ; is not writeable, let's write a different value
        ; and read it back.
        ;

        ; This write will clear Mode 2 of the CS4231... we
        ; don't care because we'll put it in this mode later.

        xor     al, al
        out     dx, al
        in      al, dx
        cmp     al, cl
        je      SHORT ICV_OK

        ; Uh-oh!  It looks like a CODEC, it walks like a CODEC,
        ; it just doesn't talk like a CODEC.
        ;
        ; Try to undo our mess!

ICV_RestoreVersion:
        mov     al, cl                          ; restore "version"
        out     dx, al                          
        shr     ecx, 8

        ; Write back out the old data to the address
        ; register, get out and hope we didn't screw
        ; anything up!

ICV_RestoreAddress:
        dec     edx                             ; move to CODEC_ADDRESS
        .errnz  (SS_CODEC_DATA - 1) - SS_CODEC_ADDRESS
        mov     al, cl
        out     dx, al                          ; restore "address"

ICV_Bad:
        stc
        jmp     SHORT ICV_Exit

ICV_OK:
        clc
        Assumes_Fall_Through ICV_Exit

ICV_Exit:
        pop     edx
        pop     ecx
        pop     eax
        ret

EndProc Is_CODEC_Valid

;------------------------------------------------------------------------------
;
;   _Is_AutoSelect_Valid
;
;   DESCRIPTION:
;       This function determines if the hardware supports the
;       WSS 1.0 AutoSelect hardware.
;
;   ENTRY:
;       WORD wBaseAutoSel
;          IO Address to verify
;
;   EXIT:
;       if Carry clear (EAX = PAL ID)
;           IO Address is valid
;       else Carry set (EAX = FALSE)
;           IO Address is not valid
;
;   USES:
;       EAX, Flags
;
;------------------------------------------------------------------------------

BeginProc _Is_AutoSelect_Valid, PUBLIC

        wBaseAutoSel    equ     [ebp + 8]

        push    ebp
        mov     ebp, esp

        push    edx
        movzx   edx, word ptr wBaseAutoSel

        add     dx, SS_PAL_ID           ; look at PAL identification port
        in      al, dx

        Trace_Out "MSSNDSYS: Is_AutoSelect_Valid: IOAddr=#DX  ID=#AL"

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   verify that we have the correct product.revision of the card. at
;   present, the 'product' (bits 3-5) will be 0 and the latest revision
;   is 4.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        and     al, (PAL_ID_PRODUCTBITS + PAL_ID_REVBITS)
        cmp     al, SS_PAL_PRODUCTREV_RQD
        jne     SHORT DIA_Fail

        movzx   eax, al                 ; zero extend board revision

DIA_Success:
        clc
        jmp     SHORT DIA_Exit
        ret

DIA_Fail:
        xor     eax, eax
        stc

DIA_Exit:
        pop     edx
        pop     ebp

        ret

EndProc _Is_AutoSelect_Valid

;------------------------------------------------------------------------------
;
;   Is_Compaq
;
;   DESCRIPTION:
;       Determines if we are communicating with a COMPAQ machine.
;
;   ENTRY:
;       Nothing
;
;   EXIT:
;       if Carry clear
;           Compaq is detected
;       else Carry set
;           not
;
;   USES:
;       EAX, Flags
;
;------------------------------------------------------------------------------

BeginProc Is_Compaq, PUBLIC

        jmp     SHORT IC_Check_Signature

szSignature     db      'COMPAQ'
cbSigLen        equ     $-szSignature

IC_Check_Signature:

        push    ecx
        push    edi
        push    esi

        ;
        ; Read BIOS rom to determine if it is a Compaq machine.
        ; The BIOS signature will be located at physical == linear
        ; address FFFEA.  
        ;

        mov     esi, 0FFFEAh
        lea     edi, szSignature
        mov     ecx, cbSigLen
        cld
        repe    cmpsb
        jnz     SHORT IC_Failure
        clc
        jmp     SHORT IC_Exit

IC_Failure:
        stc

IC_Exit:
        pop     esi
        pop     edi
        pop     ecx
        ret

EndProc Is_Compaq

;------------------------------------------------------------------------------
;
;   _Is_AGA_Valid
;
;   DESCRIPTION:
;       Determines if the provided AGA base actually points
;       to the AGA on a Compaq Business Audio System.
;
;   ENTRY:
;       wBaseCODEC
;          IO Address of CODEC
;
;       wBaseAGA
;          IO Address of AGA
;
;   EXIT:
;       EAX is True if Compaq Business Audio detected
;       and AGA is valid otherwise False
;
;   USES:
;       EAX, Flags
;
;------------------------------------------------------------------------------

BeginProc _Is_AGA_Valid, PUBLIC

        wBaseCODEC      equ     [ebp + 8]
        wBaseAGA        equ     [ebp + 12]
        wOptions        equ     [ebp + 16]

        push    ebp
        mov     ebp, esp

        push    ebx
        push    edx

        call    Is_Compaq
        jc      SHORT IAV_Failure

        movzx   edx, word ptr wBaseAGA

        mov     ah, 10h
        cmp     word ptr wBaseCODEC, 608h
        jne     SHORT IAV_ReadIt
        or      ah, 20h

IAV_ReadIt:
        .errnz  AGA_PCR
        in      al, dx
        cmp     al, 0FFh
        je      SHORT IAV_CheckISeries

        Trace_Out "MSSNDSYS: Is_AGA_Valid: IOAddr=#DX  ID=#AL"

        and     al, 30h
        cmp     al, ah
        jne     SHORT IAV_CheckISeries
        jmp     SHORT IAV_Success

IAV_CheckISeries:

        ;
        ; fIAV_ForReal specifies that we are trying to read the
        ; config from the AGA, we only want to validate the AGA
        ; address in the I-Series configuration during the initial
        ; test of the address, everything else should fail.
        ;

        movzx   eax, word ptr wOptions
        test    eax, fIAV_ForReal
        jnz     SHORT IAV_Failure

        ;
        ; Last attempt, if Compaq-I Series, "AGA" is found
        ; at CODEC base - 4, so check for I-Series, then
        ; check AGA base.
        ;

        call    Is_Compaq_ISeries
        jc      SHORT IAV_Failure
        add     edx, 4
        cmp     dx, word ptr wBaseCODEC
        je      SHORT IAV_Success

IAV_Failure:
        mov     eax, False
        jmp     SHORT IAV_Exit

IAV_Success:
        mov     eax, NOT False

IAV_Exit:
        pop     edx
        pop     ebx
        pop     ebp
        ret

EndProc _Is_AGA_Valid

;------------------------------------------------------------------------------
;
;   _Validate_Compaq_Config
;
;   DESCRIPTION:
;       Reads the configuration from the AGA for Compaq
;       Business Audio hardware.
;
;   ENTRY:
;       wBaseAGA
;          AGA base
;
;       wIRQ
;          IRQ from ConfigMg
;
;       wPlaybackDMA
;          playback DMA from ConfigMg
;
;       wCaptureDMA
;          capture DMA from ConfigMg
;
;   EXIT:
;       EAX = TRUE if successful, FALSE otherwise
;
;   USES:
;       Flags
;
;------------------------------------------------------------------------------

BeginProc _Validate_Compaq_Config, PUBLIC

        wBaseAGA        equ     [ebp + 8]
        wIRQ            equ     [ebp + 12]
        wPlaybackDMA    equ     [ebp + 16]
        wCaptureDMA     equ     [ebp + 20]

        push    ebp
        mov     ebp, esp

        pushad

        movzx   edx, word ptr wBaseAGA

        ;
        ; Read playback configuration
        ;

        add     edx, AGA_PRMR
        in      al, dx

        Trace_Out "MSSNDSYS: Read AGA_PRMR: #DX  return=#AL"

        cmp     al, 0FFh
        je      VCC_Failure

        ;
        ; Validate IRQ setting
        ;

        mov     ah, al                                  ; save for later
        movzx   ecx, al
        shr     ecx, 4
        or      ecx, ecx
        jnz     SHORT VCC_GotIRQ
        jmp     VCC_Failure

VCC_GotIRQ:
        inc     ecx
        and     ecx, 3
        mov     al, gabIRQValid[ ecx ]

        Trace_Out "MSSNDSYS: Compaq AGA settings read IRQ:#AL"

        cmp     al, byte ptr wIRQ
        jne     SHORT VCC_Failure

        ;
        ; Validate playback DMA setting
        ; 

        movzx   ecx, ah
        shr     ecx, 6
        jecxz   SHORT VCC_Failure
        dec     ecx
        mov     al, gabDMAValid[ ecx ]

        Trace_Out "MSSNDSYS: Compaq AGA settings read playback DMA:#AL"

        cmp     al, byte ptr wPlaybackDMA

        cmp     word ptr wCaptureDMA, -1                ; Q: cap DMA spcfd?
        je      SHORT VCC_Success                       ;    N: we're done        
        cmp     al, byte ptr wCaptureDMA                ; Q: dual DMA?
        je      SHORT VCC_Success                       ;    N: we're done

        ;
        ; Read capture configuration
        ;

        sub     edx, AGA_PRMR - AGA_OSCR                ; get capture DMA
        in      al, dx

        Trace_Out "MSSNDSYS: Read AGA_OSCR: #DX  return=#AL"

        cmp     al, 0FFh
        je      DEBFAR VCC_Failure

        ;
        ; Get capture DMA setting
        ;

        movzx   ecx, al
        shr     ecx, 6

        ;
        ; We're in here because we're expecting the resource to be enabled...
        ;

        or      ecx, ecx
        jz      VCC_Failure
        jecxz   SHORT VCC_Failure
        dec     ecx
        mov     al, gabDMAValid[ ecx ]

        Trace_Out "MSSNDSYS: Compaq AGA settings read capture DMA:#AL"

        cmp     al, byte ptr wCaptureDMA
        jne     SHORT VCC_Failure

VCC_Success:
        mov     [esp.Pushad_EAX], NOT False

VCC_Exit:
        popad
        pop     ebp
        ret

VCC_Failure:
        mov     [esp.Pushad_EAX], False
        jmp     SHORT VCC_Exit

EndProc _Validate_Compaq_Config

;------------------------------------------------------------------------------
;
;   _Validate_AutoSel_IRQ
;
;   DESCRIPTION:
;       This function verifies that the IRQ specified is available
;       for use by the WSS hardware.
;
;   ENTRY:
;       wBaseAutoSel
;           valid base of AutoSelect logic
;
;       wIRQ
;           IRQ to test
;
;   EXIT:
;       EAX is True if succesful, False otherwise
;
;   USES:
;       Flags
;
;------------------------------------------------------------------------------

BeginProc _Validate_AutoSel_IRQ

        wBaseAutoSel    equ     [ebp + 8]
        wIRQ            equ     [ebp + 12]

        push    ebp
        mov     ebp, esp

        pushfd
        cli

        push    ecx
        push    edx

        movzx   edx, word ptr wBaseAutoSel
        mov     ax, word ptr wIRQ

        Trace_Out "MSSNDSYS: _Validate_AutoSelect_IRQ: AutoSel base=#DX  IRQ=#AX"

        ;
        ; Locate the IRQ in our table of valid IRQ's
        ;

        xor     ecx, ecx                                ; index = 0

VAI_Loop:
        mov     ah, gabIRQConfigCodes[ecx]
        cmp     ah, -1                                  ; Q: end of list?
        je      SHORT VAI_Exit_Failure

        cmp     gabIRQValid[ecx], al                    ; Q: is this the IRQ?
        je      SHORT VAI_Check_IRQ

        inc     ecx                                     ; next index
        jmp     SHORT VAI_Loop

        ;
        ; Found IRQ in table, now test it...
        ;
        ; AL = IRQ
        ; AH = config code
        ;

VAI_Check_IRQ:
        xchg    al, ah                                  ; AL = config value
        out     dx, al
        .errnz SS_PAL_CONFIG
        add     dx, SS_PAL_ID                           ; look at board id
        in      al, dx

        Trace_Out "MSSNDSYS: _Validate_AutoSel_IRQ: IOAddr=#DX  ID=#AL"

        test    al, PAL_ID_IRQSENSE                     ; Q: IRQ usable?
        jz      SHORT VAI_Exit_Failure

        mov     edx, 1
        mov     cl, ah
        shl     edx, cl

        test    al, PAL_ID_8BITSLOT                     ; Q: 8 bit slot?
        jz      SHORT VAI_Not_8Bit

        and     edx, 02FFh                              ; only 0-7 and 9

VAI_Not_8Bit:
        or      edx, edx                                ; Q: IRQ valid?
        jz      SHORT VAI_Exit_Failure

        mov     eax, NOT False
        jmp     SHORT VAI_Exit


VAI_Exit_Failure:
        mov     eax, False

VAI_Exit:
        pop     edx
        pop     ecx
        popfd
        pop     ebp
        ret

EndProc _Validate_AutoSel_IRQ

;------------------------------------------------------------------------------
;
;   _Validate_AutoSel_DMA
;
;   DESCRIPTION:
;       This function verifies that the DMA channel(s) specified
;       is (are) available for use by the WSS hardware.
;
;   ENTRY:
;       wBaseAutoSel
;           valid base of AutoSelect logic
;
;       wPlaybackDMA
;           selected playback DMA channel
;
;       wCaptureDMA
;           selected capture DMA channel
;
;   EXIT:
;       EAX is True if succesful, False otherwise
;
;   USES:
;       Flags
;
;------------------------------------------------------------------------------

BeginProc _Validate_AutoSel_DMA

        wBaseAutoSel    equ     [ebp + 8]
        wPlaybackDMA    equ     [ebp + 12]
        wCaptureDMA     equ     [ebp + 16]

        push    ebp
        mov     ebp, esp

        push    ecx
        push    edx

        movzx   edx, word ptr wBaseAutoSel
        mov     al, byte ptr wCaptureDMA
        mov     ah, byte ptr wPlaybackDMA

        Trace_Out "MSSNDSYS: _Validate_AutoSelect_DMA: base=#DX  DMA DAC/ADC=#AX"

        ;
        ; First, locate the playback DMA in our table of valid DMAs.
        ;

        xor     ecx, ecx                                ; index = 0

VAD_Loop:
        cmp     gabDMAValid[ ecx ], -1
        je      SHORT VAD_Exit_Failure

        cmp     gabDMAValid[ ecx ], ah                  ; Q: is this the DMA?
        je      SHORT VAD_Check_DMA

        inc     ecx                                     ; next index
        jmp     SHORT VAD_Loop

        ;
        ; Ok, found it in the table, now test it...
        ;

VAD_Check_DMA:
        push    eax                                     ; save channels

        add     dx, SS_PAL_ID                           ; look at board id
        in      al, dx

        ;
        ; If this is an 8 bit board, can't use DMA channel 0...
        ; 

        test    al, PAL_ID_8BITSLOT                     ; Q: 8-bit slot?
        pop     eax
        jz      SHORT VAD_Not_8Bit

        or      ah, ah
        jz      SHORT VAD_Exit_Failure

VAD_Not_8Bit:
        
        ;
        ; Ok, playback DMA is valid, now check capture DMA...
        ;

        cmp     al, ah
        je      SHORT VAD_Exit_Success

        ;
        ; Dual DMA, here's the combinations (anything else is invalid):
        ;
        ;       playback        capture
        ;       0               1
        ;       1               0
        ;       3               0
        ;

        or      ah, ah
        jz      SHORT VAD_Capture_On_1

        or      al, al
        jnz     SHORT VAD_Exit_Failure
        jmp     SHORT VAD_Exit_Success

VAD_Capture_On_1:
        cmp     al, 1
        jne     SHORT VAD_Exit_Failure

VAD_Exit_Success:
        mov     eax, NOT False
        jmp     SHORT VAD_Exit

VAD_Exit_Failure:
        mov     eax, False

VAD_Exit:
        pop     edx
        pop     ecx
        pop     ebp
        ret

EndProc _Validate_AutoSel_DMA

;------------------------------------------------------------------------------
;
;   Is_Compaq_ISeries
;
;   DESCRIPTION:
;       Determines if Compaq I Series (those without the AGA)
;       is present.  Must be called after first determining
;       that the AGA is NOT present.
;
;   ENTRY:
;       Nothing.
;
;   EXIT:
;       if Carry clear
;           Compaq I series with BA detected
;       else Carry set
;           not Compaq I Series
;
;   COMMENTS:
;       In the cases of non-AGA machines, the AGA base is used to
;       fiddle with the muting circuitry.  (GP bits)  See spec.
;       for details.
;
;   USES:
;       Flags
;
;------------------------------------------------------------------------------

BeginProc Is_Compaq_ISeries, PUBLIC

        pushad

        call    Is_Compaq
        jc      SHORT ICI_Failure

        ;
        ; Compaq's Business Audio
        ;
        ;    RED1 - AL = 10h, 14h, 18h, 1Ch
        ;

        push    edx
        mov     edx, 0c7ch
        in      al, dx
        pop     edx
        mov     bl, al
        mov     bh, bl
        and     bh, 0F0h
        cmp     bh, 010h
        jne     SHORT ICI_Failure
        test    bl, 03h
        jnz     SHORT ICI_Failure
        popad
        clc
        ret

ICI_Failure:
        popad
        stc
        ret

EndProc Is_Compaq_ISeries

;------------------------------------------------------------------------------
;
;   Is_OPL3_Valid
;
;   DESCRIPTION:
;       Determines if the OPL3 is present.
;
;   ENTRY:
;       EDX = suspected IO Address of OPL3
;
;   EXIT:
;       if Carry clear
;           OPL3 has been detected
;       else Carry set
;           OPL3 is not present
;
;   USES:
;       Flags
;
;------------------------------------------------------------------------------

BeginProc Is_OPL3_Valid, PUBLIC

        Trace_Out "MSSNDSYS: OPL3 validation: #EDX"

        push    eax
        push    ebx
        push    ecx

        pushfd

        cli                                     ; NO INTERRUPTS HERE!!!

        mov     ebx, AD_MASK                    ; mask both timers
        mov     al, 60h
        call    OPL3_RegWrite
        mov     ebx, AD_MASK                    ; reset timers
        mov     al, 80h
        call    OPL3_RegWrite
        mov     ebx, AD_TIMER1                  ; set timer value
        mov     al, 80h
        call    OPL3_RegWrite
        mov     ebx, AD_MASK                    ; start timer
        mov     al, 01h
        call    OPL3_RegWrite

        mov     cx, 4000h

IOV_TimerLoop:
        in      al, dx
        test    al, 40h
        jnz     SHORT IOV_TimerFired
        loop    IOV_TimerLoop

IOV_TimerFired:
        push    eax
        mov     ebx, AD_MASK                    ; mask both timers
        mov     al, 60h
        call    OPL3_RegWrite
        mov     ebx, AD_MASK                    ; reset timers
        mov     al, 80h
        call    OPL3_RegWrite
        pop     eax

        popfd

        test    al, 40h                         ; Q: Did timer 1 fire?
        jz      SHORT IOV_Failure               ;    N: Not found

        ;
        ; NOTE:  This information comes from Yamaha.  On the OPL3,
        ;        data bits 1 and 2 on the status register should be
        ;        low... if not it's probably an OPL2.
        ;

        test    al, 06h
        jnz     SHORT IOV_Failure
        clc
        jmp     SHORT IOV_Exit

IOV_Failure:
        stc
        
IOV_Exit:
        pop     ecx
        pop     ebx
        pop     eax
        ret

EndProc Is_OPL3_Valid

;---------------------------------------------------------------------------;
;
;   Get_CODEC_Class
;
;   DESCRIPTION:
;       Determines CODEC classification based on CODEC revision.
;
;   ENTRY:
;       EDX = _valid_ IO Address of SndSys
;
;   EXIT:
;       AL contains actual version, AH contains classification
;
;   USES:
;       EAX
;
;---------------------------------------------------------------------------;

BeginProc Get_CODEC_Class, PUBLIC

        push    ebx

        ;
        ; default for sanity
        ;

        mov     bh, VER_AD1848J
        mov     ah, CODEC_REG_MISC
        call    CODEC_RegRead
        jc      SHORT GCC_CODEC_J_Class         ; Failure???

        mov     bh, al                          ; BH contains chip ver.

        ;
        ; Determine the CODEC "class" - see if we have to mute!
        ;

        cmp     al, VER_AD1848K
        je      SHORT GCC_CODEC_K_Class

        cmp     al, VER_CS4248
        jne     SHORT GCC_CODEC_J_Class

        mov     al, CS4231_MISC_MODE2
        mov     ah, CODEC_REG_MISC
        call    CODEC_RegWrite
        IO_Delay
        call    CODEC_RegRead
        test    al, CS4231_MISC_MODE2
        jnz     SHORT GCC_CODEC_KPlus_Class
        jmp     SHORT GCC_CODEC_K_Class

        ; Everything else defaults to CODEC_J_CLASS

GCC_CODEC_J_Class:
        mov     bl, CODEC_J_CLASS
        jmp     SHORT DGCC_Exit

GCC_CODEC_K_Class:
        mov     bl, CODEC_K_CLASS
        jmp     SHORT DGCC_Exit

GCC_CODEC_KPlus_Class:
        mov     bl, CODEC_KPLUS_CLASS

DGCC_Exit:
        movzx   eax, bx
        pop     ebx
        ret

EndProc Get_CODEC_Class

;---------------------------------------------------------------------------;
;
;   CODEC_Generate_IRQ
;
;   DESCRIPTION:
;       Use PPIO mode to generate the IRQ.
;
;   ENTRY:
;       EDX = _valid_ IO Address of SndSys
;
;   EXIT:
;       Interrupt has been generated by the CODEC and CODEC is in
;       interrupt mode.  Need to ACK and restore the state using
;       CODEC_ACK_And_Disable_Interrupt.
;
;   USES:
;       Flags
;
;---------------------------------------------------------------------------;

BeginProc CODEC_Generate_IRQ

        push    eax
        push    ecx
        push    edx

        ;
        ; Mute the CODEC.
        ;

        mov     eax, 1
        call    CODEC_ExtMute

        ;
        ; Mute the DAC.
        ;

        mov     ah, CODEC_REG_LEFTOUTPUT
        cCall   CODEC_RegRead
        shl     eax, 8
        push    eax
        mov     ah, CODEC_REG_RIGHTOUTPUT
        cCall   CODEC_RegRead
        movzx   eax, al
        or      [esp], eax
        mov     eax, 80h
        mov     ah, CODEC_REG_LEFTOUTPUT
        cCall   CODEC_RegWrite
        inc     ah
        cCall   CODEC_RegWrite

        mov     gbMode, CODEC_MODE_MCE
        mov     eax, (CODEC_REG_TEST or CODEC_MODE_MCE)
        add     edx, SS_CODEC_ADDRESS
        out     dx, al
        sub     edx, SS_CODEC_ADDRESS

        ;
        ; Configure for 11kHz, 8-bit, mono...
        ;

        mov     ah, CODEC_REG_DATAFORMAT
        cCall   CODEC_RegRead
        push    eax                             ; save previous format
        mov     al, 03h
        cCall   CODEC_RegWrite

        ;
        ; Write out PIO counter
        ;

        xor     eax, eax
        mov     ah, CODEC_REG_LOWERBASE
        cCall   CODEC_RegWrite
        dec     ah
        cCall   CODEC_RegWrite
        .errnz  (CODEC_REG_LOWERBASE - 1) - CODEC_REG_UPPERBASE

        ;
        ; Configure for interrupt generation
        ;

        mov     ah, CODEC_REG_DSP
        mov     al, gbMute
        or      al, 2
        cCall   CODEC_RegWrite

        ;
        ; Enable PPIO and PEN
        ;

        mov     ah, CODEC_REG_INTERFACE
        mov     al, (AD1848_CONFIG_PPIO or AD1848_CONFIG_PEN)
        cCall   CODEC_RegWrite

        ;
        ; Reset MCE but don't un-mute
        ;

        mov     eax, CODEC_REG_TEST
        add     edx, SS_CODEC_ADDRESS
        out     dx, al
        mov     gbMode, 0

        call    CODEC_WaitForReady

        ;
        ; Write the data
        ;

        add     edx, 3                          ; move to SS_CODEC_DIRECT
        xor     eax, eax
        out     dx, al
        .errnz  (SS_CODEC_ADDRESS + 3) - SS_CODEC_DIRECT
        sub     edx, SS_CODEC_DIRECT            ; back to base

        ;
        ; Delay long enough for CODEC to generate interrupt.
        ;

        add     edx, SS_CODEC_STATUS
        mov     ecx, 1000h

CGI_WaitIRQ:
        in      al, dx                          ; check IRQ status
        test    al, 1                           ; Q: IRQ generated?
        jnz     SHORT CGI_GotIRQ                ;    Y: OK.
        loop    CGI_WaitIRQ                     ;    N: Loop

        mov     ecx, 500                        ; Give PIC some time
        loop    $

CGI_GotIRQ:
        sub     edx, SS_CODEC_STATUS

        ;
        ; Enter MCE
        ;

        or      gbMode, CODEC_MODE_MCE
        mov     eax, CODEC_REG_TEST
        or      al, gbMode
        add     edx, SS_CODEC_ADDRESS
        out     dx, al
        sub     edx, SS_CODEC_ADDRESS

        ;
        ; Turn of PEN and PPIO
        ;

        mov     ah, CODEC_REG_INTERFACE
        xor     al, al
        cCall   CODEC_RegWrite

        ;
        ; Restore the format...
        ;

        pop     eax
        call    CODEC_RegWrite

        ;
        ; Leave MCE
        ;

        and     gbMode, NOT( CODEC_MODE_MCE )
        mov     eax, CODEC_REG_TEST
        or      al, gbMode
        add     edx, SS_CODEC_ADDRESS
        out     dx, al
        sub     edx, SS_CODEC_ADDRESS

        ;
        ; Un-mute the DAC.
        ;

        mov     eax, [esp]
        mov     ah, CODEC_REG_RIGHTOUTPUT
        cCall   CODEC_RegWrite
        pop     eax
        shr     eax, 8
        mov     ah, CODEC_REG_LEFTOUTPUT
        cCall   CODEC_RegWrite

        pop     edx
        pop     ecx
        pop     eax

        ret

EndProc CODEC_Generate_IRQ

;---------------------------------------------------------------------------;
;
;   CODEC_ACK_And_Disable_Interrupt
;
;   DESCRIPTION:
;       Acknowledges interrupt and disables interrupts from CODEC.
;
;   ENTRY:
;       EDX = _valid_ IO Address of SndSys
;
;   EXIT:
;
;   USES:
;       Flags
;
;---------------------------------------------------------------------------;

BeginProc CODEC_ACK_And_Disable_Interrupt

        push    eax                             ; save regs.

        ;
        ; Check status of CODEC and ACK IRQ only when necessary
        ;

        add     edx, SS_CODEC_STATUS
        in      al, dx
        sub     edx, SS_CODEC_STATUS
        test    al, 1
        jz      SHORT CAADI_Disable_Int_And_Playback

        ;
        ; Acknowledge interrupt from CODEC
        ;

        xor     eax, eax
        add     edx, SS_CODEC_STATUS            ; outp( CODEC_STATUS, 0 )
        out     dx, al
        sub     edx, SS_CODEC_STATUS            ; back to base

CAADI_Disable_Int_And_Playback:

        ;
        ; Disable interrupts from CODEC
        ;

        mov     ah, CODEC_REG_DSP
        mov     al, gbMute
        call    CODEC_RegWrite

        pop     eax

        ret

EndProc CODEC_ACK_And_Disable_Interrupt

;---------------------------------------------------------------------------;
;
;   _Validate_IRQ
;
;   DESCRIPTION:
;       Validates the IRQ setting for the DAK compliant
;       Windows Sound System board when auto-select is not available.
;
;   ENTRY:
;       
;
;   EXIT:
;       EAX is TRUE if IRQ is as specified or FALSE otherwise.
;
;   USES:
;       EAX, Flags
;
;---------------------------------------------------------------------------;

BeginProc _Validate_IRQ, PUBLIC

        wBaseCODEC      equ     [ebp + 8]
        wIRQ            equ     [ebp + 12]

        push    ebp
        mov     ebp, esp

        push    ebx                             ; save some regs.
        push    ecx
        push    edx

        movzx   edx, word ptr wBaseCODEC

        ;-------------------------------------------------------
        ; Tell hardware to generate an IRQ and watch the
        ; IRR to see if it fired.
        ;-------------------------------------------------------

        movzx   eax, word ptr wIRQ
        xor     ecx, ecx
        bts     ecx, eax

        pushfd                                  ; save IF
        cli                                     ; Disable interrupts!

        in      al, PIC_SLAVE_MASK_REG          ; Get slave PIC mask
        mov     ah, al                          ; Shift it over
        in      al, PIC_MASTER_MASK_REG         ; Get master PIC mask
        push    eax                             ; Save mask for later

        or      eax, ecx                        ; Mask possible IRQs

        ;
        ; Write new mask to PIC.
        ;

        xchg    al, ah
        out     PIC_SLAVE_MASK_REG, al                  
        xchg    al, ah
        out     PIC_MASTER_MASK_REG, al

        ;
        ; Set IRQ to low... should be reflected in IRR.
        ;

        mov     al, PIC_IRR_NEXT
        out     PIC_SLAVE_OCW_REG, al
        IO_Delay
        out     PIC_MASTER_OCW_REG, al
        IO_Delay

        ;
        ; Clear IRQ on hardware
        ;

        call    CODEC_ACK_And_Disable_Interrupt

        in      al, PIC_SLAVE_OCW_REG
        mov     ah, al
        in      al, PIC_MASTER_OCW_REG

        Trace_Out "MSSNDSYS: _Validate_IRQ (low): PIC IRR: #AX, looking for: #CX"

        test    eax, ecx
        jnz     SHORT VI_IRQ_Failure

        ;
        ; Tell the CODEC to generate an IRQ.
        ;

        call    CODEC_Generate_IRQ

        push    ecx
        mov     ecx, 500

VI_SillyDelay:
        in      al, dx
        loop    VI_SillyDelay
        pop     ecx

        ;
        ; Check the IRR registers to see if an interrupt fired.
        ;

        in      al, PIC_SLAVE_OCW_REG
        mov     ah, al
        in      al, PIC_MASTER_OCW_REG

        Trace_Out "MSSBLST: _Validate_IRQ (high): PIC IRR: #AX, looking for: #CX"

        test    eax, ecx
        jnz     SHORT VI_CleanUp

VI_IRQ_Failure:
        cCall   _MSSNDSYS_Log_IRQ_Error, <eax, ecx>
        xor     eax, eax

VI_CleanUp:
        ;-------------------------------------------------------
        ; Clear the IRQ...
        ;-------------------------------------------------------

        call    CODEC_ACK_And_Disable_Interrupt

        mov     edx, eax                        ; save return value

        ;
        ; Restore PIC to previous state
        ;

        pop     eax                             ; Get saved PIC mask
        xchg    al, ah                          
        out     PIC_SLAVE_MASK_REG, al          ; Write mask to PIC
        xchg    al, ah
        out     PIC_MASTER_MASK_REG, al

        mov     eax, edx                        ; get return value

        popfd                                   ; restore IF

        pop     edx
        pop     ecx                             ; Restore saved regs.
        pop     ebx

        pop     ebp

        ret

EndProc _Validate_IRQ

;---------------------------------------------------------------------------;
;
;   _Validate_Channel
;
;   DESCRIPTION:
;       Validates the setting for specified DMA channel.
;
;   ENTRY:
;       
;
;   EXIT:
;       EAX is TRUE if DMA appears to be functioning or FALSE otherwise.
;
;   USES:
;       EAX, Flags
;
;---------------------------------------------------------------------------;

VC_OptionsF_Write       equ     0000h
VC_OptionsF_Read        equ     0001h
VC_OptionsF_SingleDMA   equ     0002h

BeginProc _Validate_Channel, PUBLIC

        wBaseCODEC      equ     [ebp + 8]
        wChannel        equ     [ebp + 12]
        dwPhysAddr      equ     [ebp + 16]
        fdwOptions      equ     [ebp + 20]

        push    ebp
        mov     ebp, esp

        movzx   edx, word ptr wBaseCODEC

        ;
        ; set format
        ;

;        mov     eax, 22050
;        call    CODEC_SetFormat

        pushfd                                          ; save IF
        mov     eax, NOT False                          ; assume success
        pushad
        cli                                             ; Disable interrupts!

        ;
        ; program DMA controller for transfer
        ;

        movzx   ebx, word ptr wChannel                  ; get DMA channel
        and     ebx, 03h

        mov     eax, ebx

        or      al, 4                                   ; mask the channel
        out     DMA8SMR, al
        xor     al, al
        out     DMA8CLR, al

        mov     eax, ebx

        test    dword ptr fdwOptions, VC_OptionsF_Read
        jnz     SHORT VC_Read_DMA

        add     al, 48h                                 ; DMA write
        jmp     SHORT VC_Mode_Set

VC_Read_DMA:
        add     al, 44h

VC_Mode_Set:
        out     DMA8MOD, al

        mov     edx, ebx                                ; setup DMA address
        shl     edx, 1
        add     edx, DMA8ADR
        mov     eax, dwPhysAddr
        out     dx, al
        shr     eax, 8
        out     dx, al

        movzx   ebx, word ptr wChannel
        movzx   edx, gbDMAPageReg[ ebx ]
        shr     eax, 8
        out     dx, al

        and     ebx, 03h
        mov     edx, ebx
        shl     edx, 1
        add     edx, DMA8CNT

        mov     al, 0fh                                 ; set count
        out     dx, al
        xor     al, al
        out     dx, al

        mov     eax, ebx                                ; unmask the channel
        out     DMA8SMR, al

        ;
        ; set count on CODEC
        ;

        movzx   edx, word ptr wBaseCODEC

        mov     ah, CODEC_REG_LOWERBASE
        mov     al, 0fh
        call    CODEC_RegWrite

        mov     ah, CODEC_REG_UPPERBASE
        xor     al, al
        call    CODEC_RegWrite

        ;
        ; fully attenuate DAC
        ;

        mov     ah, CODEC_REG_LEFTOUTPUT
        mov     al, 03fh
        call    CODEC_RegWrite

        mov     ah, CODEC_REG_RIGHTOUTPUT
        call    CODEC_RegWrite

        ;
        ; no interrupt generation
        ;

        mov     ah, CODEC_REG_DSP
        mov     al, gbMute
        call    CODEC_RegWrite

        ;
        ; Enter MCE
        ;

        mov     gbMode, CODEC_MODE_MCE
        mov     eax, (CODEC_REG_TEST or CODEC_MODE_MCE)
        add     edx, SS_CODEC_ADDRESS
        out     dx, al
        sub     edx, SS_CODEC_ADDRESS

        ;
        ; start DAC or ADC
        ;

        mov     al, AD1848_CONFIG_PEN
        test    dword ptr fdwOptions, VC_OptionsF_Read
        jz      SHORT VC_CODEC_Check_SDC
        mov     al, AD1848_CONFIG_CEN

VC_CODEC_Check_SDC:
        test    dword ptr fdwOptions, VC_OptionsF_SingleDMA
        jz      SHORT VC_CODEC_Mode_Set
        or      al, AD1848_CONFIG_SDC

        ;
        ; Hack for AD1848J compatibility in SDC mode
        ;

        test    dword ptr fdwOptions, VC_OptionsF_Read
        jz      SHORT VC_CODEC_Mode_Set
        or      al, AD1848_CONFIG_PPIO


VC_CODEC_Mode_Set:

        mov     ah, CODEC_REG_INTERFACE
        call    CODEC_RegWrite

        ;
        ; Leave MCE
        ;

        mov     eax, CODEC_REG_TEST
        add     edx, SS_CODEC_ADDRESS
        out     dx, al
        mov     gbMode, 0

        call    CODEC_WaitForReady

        mov     edx, ebx                                ; setup cnt
        shl     edx, 1                                  ;    reg => EDX
        add     edx, DMA8CNT

        ;
        ; wait for transfer to complete or timeout
        ;

        mov     ecx, 0ffffh

DVDS_WaitXfer:

        xor     al, al
        out     DMA8CLR, al
        IO_Delay
        in      al, dx
        mov     ah, al
        IO_Delay
        in      al, dx

        cmp     ax, -1
        je      SHORT DVDS_Exit                         

        loop    DVDS_WaitXfer

        cCall   _MSSNDSYS_Log_DMA_Error, <ebx, eax>

        Debug_Out "MSSNDSYS: _Validate_Channel: failed wait."

        mov     [esp.Pushad_EAX], False

DVDS_Exit:

        movzx   edx, word ptr wBaseCODEC                ; kill DMA
        mov     ah, CODEC_REG_INTERFACE
        xor     al, al
        call    CODEC_RegWrite

        mov     eax, ebx                                ; mask DMA channel
        and     eax, 03h
        or      eax, 04h
        out     DMA8SMR, al

        add     edx, SS_CODEC_STATUS                    ; ack any pending
        xor     al, al
        out     dx, al

        popad
        popfd                                           ; restore IF

        pop     ebp
        ret

EndProc _Validate_Channel

;---------------------------------------------------------------------------;
;
;   _Validate_DMA
;
;   DESCRIPTION:
;       Validates the DMA channel settings by trying a DMA transfer.
;       available DMA settings.
;
;   ENTRY:
;       wBaseCODEC      
;          valid base of AD1848 CODEC
;
;       wPlaybackDMA    
;          proposed playback DMA channel
;
;       wCaptureDMA     
;          proposed capture DMA channel
;
;   EXIT:
;       EAX is True if successful, False otherwise
;
;   USES:
;       Flags
;
;---------------------------------------------------------------------------;

BeginProc _Validate_DMA

        wBaseCODEC      equ     [ebp + 8]
        wPlaybackDMA    equ     [ebp + 12]
        wCaptureDMA     equ     [ebp + 16]

        dwPhysAddr      equ     [ebp - 4]
        hMem            equ     [ebp - 8]

        push    ebp
        mov     ebp, esp
        sub     esp, 8                                  ; reserve space

        pushad

        movzx   edx, word ptr wBaseCODEC

        ;
        ; Verify that transfers succeed in both directions...
        ;

        ; Allocate memory block for transfer

        lea     ebx, dwPhysAddr
        VMMcall _PageAllocate <1, PG_SYS, 0, 0, 0, 0FFFh, ebx,\
                               PageUseAlign+PageContig+PageFixed>

        mov     hMem, eax
        or      eax, eax
        jz      ValDMA_Exit

        ;
        ; fill with silence
        ;

        mov     edi, edx
        mov     ecx, 400h                               ; 1 page / 4 bytes
        mov     eax, 80808080h                          ; 8-bit unsigned
        cld
        rep     stosd

        cCall   _Validate_Channel, <wBaseCODEC, wPlaybackDMA,\
                                    dwPhysAddr, VC_OptionsF_Write>
        or      eax, eax
        jz      SHORT ValDMA_Exit

        mov     ecx, VC_OptionsF_Read
        mov     ax, word ptr wPlaybackDMA
        cmp     ax, word ptr wCaptureDMA
        jnz     SHORT ValDMA_NotSingle
        or      ecx, VC_OptionsF_SingleDMA

ValDMA_NotSingle:
        cCall   _Validate_Channel, <wBaseCODEC, wCaptureDMA,\
                                    dwPhysAddr, ecx>

ValDMA_Exit:

        ;
        ; store return value
        ;

        mov     [esp.Pushad_EAX], eax

        ;
        ; free the memory block
        ;

        mov     eax, hMem
        or      eax, eax
        jz      SHORT ValDMA_NoMem

        VMMcall _PageFree <eax, 0>

ValDMA_NoMem:

        popad
        add     esp, 8                                  ; clean up stack
        pop     ebp                                     ; restore ebp
        ret

EndProc _Validate_DMA

VxD_PNP_CODE_ENDS

end
