        page 60, 132

;---------------------------------------------------------------------------;
;
;   COMMONA.ASM
;
;   DESCRIPTION:
;       Contains wave and midi support routines that don't need to be fixed.
;
;---------------------------------------------------------------------------;
;---------------------------------------------------------------------------;
;
;  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;  PURPOSE.
;
;  Copyright (c) 1991 - 1995	Microsoft Corporation.	All Rights Reserved.
;
;---------------------------------------------------------------------------;

ifdef MASM6
	option oldmacros
        option oldstructs
endif

        .286

        .xlist
        include cmacros.inc                   
        include windows.inc
        include mmsystem.inc
        include driver.inc
        include mssndsys.inc
        .list

        ?PLM=1                          ; Pascal calling convention
        ?WIN=0                          ; NO! Windows prolog/epilog code

;===========================================================================;
;   segmentation
;===========================================================================;
     
IFNDEF SEGNAME
        SEGNAME equ <_TEXT>
ENDIF

createSeg %SEGNAME, CodeSeg, word, public, CODE

	.386p

;===========================================================================;
;   data segment
;===========================================================================;

sBegin Data

        globalD _glpVSNDSYSEntry, 0     ; VSNDSYS API entry point

sEnd Data


;===========================================================================;
;   code segment
;===========================================================================;

sBegin CodeSeg

        assumes cs, CodeSeg
        assumes ds, Data

;---------------------------------------------------------------------------;
;
;   WORD FAR PASCAL HardwareAcquire( WORD phwi, WORD wPortBase, WORD wFlags )
;
;   DESCRIPTION:
;       This function is used to 'acquire' access to either the CODEC
;       or OPL3 on the MS WSS Audio card. You must call this function
;       _before_ touching the hardware. If this function succeeds, you
;       are free to party all over the hardware you requested. If this
;       function fails, you will NOT be allowed to access the hardware.
;
;       NOTE: you currently cannot request access to the CODEC and OPL3
;       with one call to this function by OR'ing the acquire bits together.
;       Instead, just call this function twice: once for the CODEC and
;       once for the OPL3.
;
;   ENTRY:
;       ParmW phwi      :   pointer to hardware instance 
;
;       ParmW wPortBase :   Port base of CODEC or OPL3 to acquire. This
;                           argument must be the same as the base port
;                           that the VxD is virtualizing.
;
;       ParmW wFlags    :   fSS_ASS_Acquire_CODEC, fSS_ASS_Acquire_OPL3
;
;   EXIT:
;       IF success
;           AX = 0000h or 8000h
;           if (AX & MSS_API_ASS_Err_State_Unknown)
;               hardware needs reset
;           go ahead and open
;       ELSE
;           AX = non-zero error code:
;               MSS_API_ASS_Err_Bad_Base_Port    equ 01h
;                   The base port specified is not being virtualized
;
;               MSS_API_ASS_Err_Already_Owned    equ 02h
;                   The CODEC/OPL3 is currently owned by another VM.
;
;   USES:
;       Flags, AX, BX, DX
;
;   NOTES:
;       The VxD does NOT keep a 'lock count' for a VM because it does
;       not make sense to do so in most cases. Therefore, this function
;       must maintain a lock count itself. This is required if, for
;       example, the CODEC was to be 'opened' for wave input and output
;       simultaneously (which is possible).
;
;   HISTORY:
;       5/14/92     cjp     wrote it
;
;---------------------------------------------------------------------------;

        assumes ds, Data
        assumes es, nothing

cProc HardwareAcquire <FAR, PASCAL, PUBLIC> <si>
        ParmW   phwi
        ParmW   wPortBase
        ParmW   wFlags
cBegin

        mov     si, phwi
        mov     bx, wFlags

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   call the VxD requesting access to the hardware--note that calling
;   acquire while the hardware is owned by us will always succeed.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     ax, wPortBase               ; base port to acquire
        mov     dx, MSS_API_Acquire
        call    [_glpVSNDSYSEntry]
        jc      vss_Acquire_Exit            ; if can't acquire, AX = error


;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   increment the acquire counts for the CODEC and OPL3 if they were 
;   acquired.
;
;       AX = 0000h or 8000h
;       BX = Flags: fSS_ASS_Acquire_CODEC, fSS_ASS_Acquire_OPL3
;       DX = MSS_API_Acquire (1)
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

vss_Acquire_Success:

        shr     bx, 1
        adc     [si.hwi_bAcquireCountCODEC], al
        .errnz fSS_ASS_Acquire_CODEC - 0001h
        shr     bx, 1
        adc     [si.hwi_bAcquireCountOPL3], al
        .errnz fSS_ASS_Acquire_OPL3 - 0002h
        clc

vss_Acquire_Exit:

cEnd


;---------------------------------------------------------------------------;
;
;   WORD FAR PASCAL HardwareRelease( WORD phwi, WORD wPortBase, WORD wFlags )
;
;   DESCRIPTION:
;       This function is used to 'release' either the CODEC or OPL3 on
;       the MS WSS Audio card. You must call this function when you are
;       no longer using the hardware so other VM's can gain access to
;       the hardware. No other VM will be able to access the hardware
;       until you release it (if you own it).
;
;       NOTE: you currently cannot release the CODEC and OPL3 with one
;       call to this function by OR'ing the release bits together.
;       Instead, just call this function twice: once for the CODEC and
;       once for the OPL3.
;
;   ENTRY:
;       ParmW phwi      :   pointer to hardware instance 
;
;       ParmW wPortBase :   Port base of CODEC or OPL3 to release. This
;                           argument must be the same as the base port
;                           that the VxD is virtualizing.
;
;       ParmW wFlags    :   fSS_ASS_Acquire_CODEC, fSS_ASS_Acquire_OPL3
;
;   EXIT:
;       IF success
;           AX = 0, you no longer 'own' the hardware
;       ELSE
;           AX = non-zero error code:
;               MSS_API_RSS_Err_Bad_Base_Port    equ 01h
;                   The base port specified is not being virtualized
;
;               MSS_API_RSS_Err_Not_Yours        equ 02h
;                   The CODEC/OPL3 is NOT owned by caller's VM.
;
;   USES:
;       Flags, AX, BX, DX
;
;   NOTES:
;       The VxD does NOT keep a 'lock count' for a VM because it does
;       not make sense to do so in most cases. Therefore, this function
;       must maintain a lock count itself. This is required if, for
;       example, the CODEC was to be 'opened' for wave input and output
;       simultaneously (which is possible).
;
;   HISTORY:
;       5/14/92     cjp     wrote it
;
;---------------------------------------------------------------------------;

        assumes ds, Data
        assumes es, nothing

cProc HardwareRelease <FAR, PASCAL, PUBLIC> <si>
        ParmW   phwi
        ParmW   wPortBase
        ParmW   wFlags
cBegin

        mov     si, phwi

        xor     ax, ax                      ; assume success

        mov     bx, wFlags
        test    bx, fSS_ASS_Acquire_CODEC
        jz      vss_Release_Not_CODEC

        AssertF [si.hwi_bAcquireCountCODEC]
        cmp     [si.hwi_bAcquireCountCODEC], 1  ; Q: final release?
        je      vss_Release_Hardware            ;   Y: then call VxD
        jne     vss_Release_Success             ;   N: succeed...

vss_Release_Not_CODEC:

        test    bx, fSS_ASS_Acquire_OPL3
        jz      vss_Release_Success             ; hmm.... 

        AssertF [si.hwi_bAcquireCountOPL3]
        cmp     [si.hwi_bAcquireCountOPL3], 1   ; Q: final release?
        jne     vss_Release_Success             ;   N: succeed...
        ; fall through to vss_Release_Hardware              ;   Y: then call VxD


vss_Release_Hardware:

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   BX = Flags: fSS_ASS_Acquire_CODEC, fSS_ASS_Acquire_OPL3
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     ax, wPortBase               ; base port to release
        mov     dx, MSS_API_Release
        call    [_glpVSNDSYSEntry]
        jc      vss_Release_Exit            ; if fail (AX = error code)

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   decrement the acquire counts for the CODEC and OPL3 if they were 
;   released.
;
;       AX = 0000h
;       BX = Flags: fSS_ASS_Acquire_CODEC, fSS_ASS_Acquire_OPL3
;       DX = MSS_API_Release (2)
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

vss_Release_Success:

        shr     bx, 1
        sbb     [si.hwi_bAcquireCountCODEC], al
        .errnz fSS_ASS_Acquire_CODEC - 0001h
        shr     bx, 1
        sbb     [si.hwi_bAcquireCountOPL3], al
        .errnz fSS_ASS_Acquire_OPL3 - 0002h
        clc                                 ; flag success

vss_Release_Exit:

cEnd


;---------------------------------------------------------------------------;
;
;   WORD FAR PASCAL wodHardwareAcquire( PHARDWAREINSTANCE phwi )
;
;   DESCRIPTION:
;
;   ENTRY:
;       Nothing.
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
;---------------------------------------------------------------------------;

        assumes ds, Data
        assumes es, nothing

cProc wodHardwareAcquire <FAR, PASCAL, PUBLIC> <si>
        ParmW   phwi

cBegin

        D2 <wodHardwareAcquire>

        mov     si, phwi

        mov     ax, -1                      ; assume bad news
        test    [si.hwi_bIntUsed], al       ; Q: is the hardware available?
        jnz     wod_AH_Exit

        test    [si.hwi_bwodFlags], WOF_ALLOCATED
        jnz     wod_AH_Exit                 ; Q: is the hardware available?

        D2 <acquire>

        cCall   HardwareAcquire, <si, [si.hwi_wIOAddressCODEC],\
                                  fSS_ASS_Acquire_CODEC>
        or      ax, ax
        jz      wod_AH_Success
        jns     wod_AH_Exit                 ; if high bit not set, error
        .errnz MSS_API_ASS_Err_State_Unknown - 8000h

;BUG <wodHardwareAcquire: --must reset hardware if owned by other VM-->

        ; fall through to wod_AH_Success

wod_AH_Success:
        or      [si.hwi_bwodFlags], WOF_ALLOCATED
        mov     [si.hwi_bIntUsed], INT_WAVEOUT      ; allocate it...
        xor     ax, ax                          ; success

wod_AH_Exit:

cEnd


;---------------------------------------------------------------------------;
;
;   WORD FAR PASCAL wodHardwareRelease( PHARDWAREINSTANCE phwi )
;
;   DESCRIPTION:
;
;   ENTRY:
;       Nothing.
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
;---------------------------------------------------------------------------;

        assumes ds, Data
        assumes es, nothing

cProc wodHardwareRelease <FAR, PASCAL, PUBLIC> <si>
        ParmW   phwi

cBegin

        D2 <wodHardwareRelease>

        mov     si, phwi

        mov     ax, -1                          ; assume bad news
        test    [si.hwi_bwodFlags], WOF_ALLOCATED
        jz      wod_RH_Exit

        cmp     [si.hwi_bIntUsed], INT_WAVEOUT  ; Q: wave output owned?
        jne     wod_RH_Exit

        D2 <release>

        mov     [si.hwi_bIntUsed], INT_FREE     ; release it...
        cCall   HardwareRelease, <si, [si.hwi_wIOAddressCODEC],\
                                  fSS_ASS_Acquire_CODEC>

        and     [si.hwi_bwodFlags], not WOF_ALLOCATED
        xor     ax, ax                          ; success

wod_RH_Exit:

cEnd


;---------------------------------------------------------------------------;
;
;   WORD FAR PASCAL widHardwareAcquire( PHARDWAREINSTANCE phwi )
;
;   DESCRIPTION:
;
;   ENTRY:
;       Nothing.
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
;---------------------------------------------------------------------------;

        assumes ds, Data
        assumes es, nothing

cProc widHardwareAcquire <FAR, PASCAL, PUBLIC> <si>
        ParmW   phwi

cBegin

        D2 <widHardwareAcquire>

        mov     si, phwi

        mov     ax, -1                      ; assume bad news
        test    [si.hwi_bIntUsed], al       ; Q: is the hardware available?
        jnz     wid_AH_Exit

        test    [si.hwi_bwidFlags], WIF_ALLOCATED
        jnz     wid_AH_Exit                 ; Q: is the hardware available?

        D2 <acquire>

        cCall   HardwareAcquire, <si, [si.hwi_wIOAddressCODEC],\
                                  fSS_ASS_Acquire_CODEC>
        or      ax, ax
        jz      wid_AH_Success
        jns     wid_AH_Exit                     ; if high bit not set, error
        .errnz MSS_API_ASS_Err_State_Unknown - 8000h

        ; fall through to wid_AH_Success

wid_AH_Success:
        or      [si.hwi_bwidFlags], WIF_ALLOCATED
        mov     [si.hwi_bIntUsed], INT_WAVEIN   ; allocate it...
        xor     ax, ax                          ; success

wid_AH_Exit:

cEnd


;---------------------------------------------------------------------------;
;
;   WORD FAR PASCAL widHardwareRelease( PHARDWAREINSTANCE phwi )
;
;   DESCRIPTION:
;
;   ENTRY:
;       Nothing.
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
;---------------------------------------------------------------------------;

        assumes ds, Data
        assumes es, nothing

cProc widHardwareRelease <FAR, PASCAL, PUBLIC> <si>
        ParmW  phwi

cBegin

        D2 <widHardwareRelease>

        mov     si, phwi

        mov     ax, -1                      ; assume bad news
        test    [si.hwi_bwidFlags], WIF_ALLOCATED
        jz      wid_RH_Exit

        cmp     [si.hwi_bIntUsed], INT_WAVEIN     ; Q: wave input owned?
        jne     wid_RH_Exit

        D2 <release>

        mov     [si.hwi_bIntUsed], INT_FREE       ; release it...
        cCall   HardwareRelease, <si, [si.hwi_wIOAddressCODEC],\
                                  fSS_ASS_Acquire_CODEC>

        and     [si.hwi_bwidFlags], not WIF_ALLOCATED
        xor     ax, ax                      ; success

wid_RH_Exit:

cEnd

sEnd CodeSeg

        end
