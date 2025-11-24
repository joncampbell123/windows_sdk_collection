        page 60, 132

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
;
;   ASSEMU.ASM
;
;   General Description:
;      Contains non-fixed assembly routines.
;
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

VDS_VERSION_REQD    equ 0200h           ; V2.00 of VDS for auto-DMA support


        ?PLM=1                          ; Pascal calling convention
        ?WIN=0                          ; NO! Windows prolog/epilog code


;===========================================================================;
;   extrn declarations
;===========================================================================;

        externD _glpVSNDSYSEntry        ; commona.asm

        externA __WinFlags              ; KERNEL

        externFP GlobalAlloc            ; KERNEL
        externFP GlobalLock             ; KERNEL
        externFP GlobalUnlock           ; KERNEL
        externFP GlobalFree             ; KERNEL
        externFP AllocSelector          ; KERNEL
        externFP FreeSelector           ; KERNEL
        externFP PrestoChangoSelector   ; KERNEL

        externFP StackEnter             ; from MMSYSTEM.DLL
        externFP StackLeave

        externFP widHardwareAcquire     ; commona.asm
        externFP widHardwareRelease
        externFP wodHardwareAcquire
        externFP wodHardwareRelease

        externFP HwStopDACDMA
        externFP HwBeginDACDMA
        externFP HwBeginADCDMA
        externFP HwEndADCDMA
        externFP HwSetFormat

        externD  isr_Srv_Table          ; assemf.asm

;===========================================================================;
;   segmentation
;===========================================================================;

IFNDEF SEGNAME
        SEGNAME equ <_TEXT>
ENDIF

createSeg %SEGNAME, CodeSeg, word, public, CODE

        .386p

;===========================================================================;
;   code segment
;===========================================================================;

sBegin CodeSeg

        assumes cs, CodeSeg
        assumes ds, Data
        assumes es, nothing

;---------------------------------------------------------------------------;
;
;   WORD NEAR PASCAL GetVDSVersion( void )
;
;   DESCRIPTION:
;       This function gets the version number of the Virtual DMA Services
;       provided by VDMAD.  This is used to determine if it is safe to do
;       the auto-init DMA.
;
;       We return the 'implementation version number' which should be
;       >= 0103h if VDMAD supports auto-init DMA.
;
;   ENTRY:
;       Nothing.
;
;   EXIT:
;       AX = Virtual DMA Services' version number (AH=major, AL=minor)
;
;   USES:
;       Flags, AX
;
;   NOTE!
;       It is absolutely necessary to check this even in the final release
;       of Windows 3.1 because some third party ISV's
;       _replace_ VDMAD.386 with an old version that does not support auto-
;       init DMA.  If we were to attempt to do auto-init DMA on this old
;       version of VDMAD, our VM would be nuked... which is bad.
;
;---------------------------------------------------------------------------;

        assumes ds, Data
        assumes es, nothing

cProc GetVDSVersion <NEAR, PASCAL, PUBLIC> <>
        LocalW  wVersion
cBegin

        pusha                           ; int 4Bh trashes a bunch of registers

        mov     ax, 8102h               ; VDS Get Version
        xor     dx, dx                  ; flags should be zero
        int     4Bh                     ; fire it off
        jnc     vds_Get_Version_Exit    ; carry clear if no error

        xor     cx, cx                  ; show that there was an error

vds_Get_Version_Exit:

        mov     wVersion, cx            ; implementation version (needs to be 0200h)
        popa

        mov     ax, wVersion            ; return version number

cEnd


;---------------------------------------------------------------------------;
;
;   BOOL SetInterruptMask( bIRQ, fMask )
;
;   DESCRIPTION:
;       This function sets or unsets interrupt vector mask.
;
;   ENTRY:
;       ParmB   bIRQ        :   The IRQ (0 - 15) to mask/unmask
;       ParmB   fMask       :   TRUE if should be masked--FALSE if unmask
;
;   EXIT:
;       AX    :   The return value is the previous interrupt mask.
;
;   USES:
;       Flags, AX, CX, DX
;
;
;---------------------------------------------------------------------------;

        assumes ds, Data
        assumes es, nothing

cProc SetInterruptMask <NEAR, PUBLIC> <>
        ParmB   bIRQ
        ParmB   fMask
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

        mov     cl, fMask               ; get mask
        or      cl, cl
        jz      SetIntMask_UnMask
        mov     cl, ch

SetIntMask_UnMask:

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   CH  = PIC mask         (1 << (bInt&7))
;   CL  = wanted mask      fMask ? ch : 0
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        not     ch                      ; we need inverse of mask

        EnterCrit                       ; !!! Trashes BX !!!
        in      al, dx                  ; grab current mask
        mov     ah, al                  ; save it
        and     al, ch                  ; clear bit
        or      al, cl                  ; clear or set based on fMask
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


;---------------------------------------------------------------------------;
;
;   LPFUNC SetInterruptVector( bIRQ, lpNewISR )
;
;   DESCRIPTION:
;       This function takes the IRQ and sets the appropriate interrupt
;       vector; and returns the pointer to the previous handler of the
;       IRQ.
;
;   ENTRY:
;       ParmB   bIRQ        :   The IRQ (0 - 15) to install handler for.
;       ParmD   lpNewISR    :   The handler
;
;   EXIT:
;       DX:AX   :   The return value is the previous interrupt handler.
;
;   USES:
;       Flags, AX, BX, DX, ES
;
;
;---------------------------------------------------------------------------;

        assumes ds, Data
        assumes es, nothing

cProc SetInterruptVector <NEAR, PUBLIC> <>
        ParmB   bIRQ
        ParmD   lpNewISR
cBegin

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   convert IRQ to interrupt vector...
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

;---------------------------------------------------------------------------;
;
;   GetVxDEntry
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;       Flags, AX, BX, CX, DX, ES
;
;
;---------------------------------------------------------------------------;

        assumes ds, Data
        assumes es, nothing

cProc GetVxDEntry <NEAR, PASCAL, PUBLIC> <es, di>
cBegin

        ;
        ; See if we've already got it...
        ;

        mov     ax, word ptr [_glpVSNDSYSEntry]
        or      ax, word ptr [_glpVSNDSYSEntry + 2]
        jnz     SHORT gve_Exit_Success

        ;
        ; Get the callgate for the API entry of the VxD
        ;

        xor     ax, ax                      ; assume the worst
        xor     di, di                      ; zero ES:DI before call
        mov     es, di
        mov     ax, 1684h                   ; get device API entry point
        mov     bx, MSSNDSYS_Device_ID      ; virtual device ID
        int     2Fh                         ; call WIN/386 INT 2F API
        mov     ax, es                      ; return farptr
        or      ax, di
        jz      gve_Exit_Failure

        mov     word ptr [_glpVSNDSYSEntry], di
        mov     word ptr [_glpVSNDSYSEntry + 2], es

gve_Exit_Success:
        mov     ax, 1
        clc
        jmp     SHORT gve_Exit

gve_Exit_Failure:
        xor     ax, ax
        stc

gve_Exit:

cEnd

;---------------------------------------------------------------------------;
;
;   WORD GetVxDVersion( VOID )
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;       Flags, AX, BX, CX, DX, ES
;
;
;---------------------------------------------------------------------------;

        assumes ds, Data
        assumes es, nothing

cProc GetVxDVersion <NEAR, PASCAL, PUBLIC> <>
cBegin

        D1 <GetVxDVersion>

        cCall   GetVxDEntry
        jc      SHORT gvv_Exit

        mov     dx, MSS_API_Get_Version
        call    [_glpVSNDSYSEntry]

gvv_Exit:

cEnd

;---------------------------------------------------------------------------;
;
;   BOOL NEAR PASCAL GetVxDInfo( PHARDWAREINSTANCE phwi, DWORD dn )
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;       Flags, AX, BX, CX, DX, ES
;
;
;---------------------------------------------------------------------------;

        assumes ds, Data
        assumes es, nothing

cProc GetVxDInfo <NEAR, PASCAL, PUBLIC> <es, ds, si, di>
        ParmW   phwi
        ParmD   dn
        LocalV  ssi, <size SNDSYSINFO>
cBegin

        D1 <GetVxDInfo>

        cCall   GetVxDEntry
        jnc     gss_Ok
        jmp     gssi_Exit

gss_Ok:
        push    ss                          ; ES:BX -> ssi structure
        pop     es
        assumes es, nothing
        lea     bx, ssi

        ;
        ;    Set size of the structure before calling the VxD
        ;    to get the info.
        ;

        mov     word ptr es:[bx].ssi_dwSize, size SNDSYSINFO
        mov     word ptr es:[bx].ssi_dwSize + 2, 0


        mov     ax, MSS_API_GetInfoF_DevNode

        push    ecx
        mov     ecx, dn 
        mov     dx, MSS_API_Get_Info
        call    [_glpVSNDSYSEntry]
        pop     ecx

        jnc     SHORT gss_copy
        jmp     SHORT gssi_Exit             ; fail (AX = 0)

gss_copy:
        mov     si, phwi
        assumes ds, Data

        mov     ax, es:[bx].ssi_wOEM_ID
        mov     ds:[si].hwi_wOEM_ID, ax

        mov     ax, es:[bx].ssi_wFlags
        mov     ds:[si].hwi_wFlags, ax

        mov     ax, es:[bx].ssi_wCODECBase
        mov     ds:[si].hwi_wIOAddressCODEC, ax

        mov     ax, es:[bx].ssi_wAGABase
        mov     ds:[si].hwi_wIOAddressAGA, ax

        mov     ax, es:[bx].ssi_wIOAddressOPL3
        mov     ds:[si].hwi_wIOAddressOPL3, ax

        mov     ax, es:[bx].ssi_wPCMCIA_SRAMBase
        mov     ds:[si].hwi_wSRAMBase, ax

        mov     al, es:[bx].ssi_bIRQ
        mov     ds:[si].hwi_bIRQ, al

        mov     al, es:[bx].ssi_bDMADAC
        mov     ds:[si].hwi_bPlaybackDMA, al

        mov     al, es:[bx].ssi_bDMAADC
        mov     ds:[si].hwi_bCaptureDMA, al

        mov     ax, es:[bx].ssi_wCODECClass
        mov     ds:[si].hwi_wCODECClass, ax

        mov     ax, es:[bx].ssi_wDMABufferSelector
        mov     ds:[si].hwi_wDMABufferSelector, ax

        mov     ax, word ptr es:[bx].ssi_lpDMABufferPhys
        mov     word ptr ds:[si].hwi_lpDMABufferPhys, ax
        mov     ax, word ptr es:[bx].ssi_lpDMABufferPhys + 2
        mov     word ptr ds:[si].hwi_lpDMABufferPhys + 2, ax

        mov     ax, word ptr es:[bx].ssi_dwDMABufferLen
        mov     word ptr ds:[si].hwi_dwDMABufferLen, ax
        mov     ax, word ptr es:[bx].ssi_dwDMABufferLen + 2
        mov     word ptr ds:[si].hwi_dwDMABufferLen + 2, ax

        mov     ax, es:[bx].ssi_wHardwareOptions
        mov     ds:[si].hwi_wHardwareOptions, ax

        mov     ax, 1

gssi_Exit:

cEnd

;---------------------------------------------------------------------------;
;
;   ISR_Stub
;
;   DESCRIPTION:
;
;   This function is duplicated for the actual interrupt service
;   routine for a given IRQ.  The code is modified to load SI
;   with the appropriate offset of the HARDWAREINSTANCE associated
;   with the IRQ.
; 
;
;---------------------------------------------------------------------------;

	assumes ds, nothing
	assumes es, nothing

cProc ISR_Stub <FAR, PASCAL, PUBLIC> <>
cBegin nogen

	push    ds
	push    ax
	mov     ax, DGROUP              ; set up local DS
	mov     ds, ax
	assumes ds, Data
	push    es
                                        ; pushad
        db      66h
	pusha

        mov     si, 1234h

ref_phwi        equ     ($-ISR_Stub) - 2

	mov     al, [si.hwi_bIntUsed]
	or      al, al                  ; Q: expecting an interrupt?
	jz      SHORT isr_Exit          ;   N: out of here!

isr_Handle_Interrupt:

        ;
        ; Get jump location from table
        ;

        dec     ax
	mov     di, ax                  ; convert gbIntUsed to table index
	and     di, 0003h               ; clear high byte...
        shl     di, 2                   ; dword indirect

        sti
	cCall   isr_Srv_Table[di], <si> ; service the interrupt
        cli

        ;
        ; ACK the interrupt to the CODEC.
        ;


isr_Exit:
        mov     dx, [si.hwi_wIOAddressCODEC]
        add     dx, CODEC_STATUS
        out     dx, al                  ; acknowledge the interrupt

isr_EOI_Clear:
	mov     ax, [si.hwi_wEOICommands] ; get EOI commands for slave & master
	or      al, al                    ; Q: need to EOI slave? (IRQ's 8-15)
	jz      isr_EOI_Master          ;   N: just do master (IRQ's 0-7)

	out     PIC_EOI_SLAVE, al       ; EOI the slave first--then master

isr_EOI_Master:
	mov     al, ah                  ; move master EOI command to AL
	out     PIC_EOI_MASTER, al


        db      66h                     ; popad
	popa

        pop     es
	pop     ax
	pop     ds
	assumes ds, nothing

	iret

ISR_Stub_Len    equ     $-ISR_Stub

cEnd nogen

;---------------------------------------------------------------------------;
;
;   Create_ISR
;
;   DESCRIPTION:
;
; 
;
;---------------------------------------------------------------------------;

        assumes ds, Data
        assumes es, nothing

cProc Create_ISR <NEAR, PASCAL, PUBLIC> <es, si, di>
        ParmW   phwi
cBegin
        mov     si, phwi
        mov     ax, [si.hwi_uISRCodeSel]
        or      ax, ax
        jnz     SHORT ci_Exit_Failure

        cCall   GlobalAlloc, <GMEM_FIXED+GMEM_ZEROINIT+GMEM_SHARE,\
                              0, ISR_Stub_Len>
        or      ax, ax
        jz      SHORT ci_Exit_Failure
IF 0
        cCall   GlobalLock, <ax>
ELSE
	mov	dx, ax
	xor	ax, ax
ENDIF
        mov     [si.hwi_uISRDataSel], dx
        xor     ax, ax
        cCall   AllocSelector, <ax>
        cCall   PrestoChangoSelector, <dx, ax>
        mov     [si.hwi_uISRCodeSel], ax

        push    ds
        push    si
        push    cs
        pop     ds

        Assumes ds, Code

        mov     si, OFFSET ISR_Stub 
        mov     es, dx
        xor     di, di
        mov     cx, ISR_Stub_Len
        rep     movsb
        pop     si

        pop     ds
        Assumes ds, Data

        mov     di, ref_phwi
        mov     word ptr es:[di], si

        mov     ax, 1
        jmp     SHORT ci_Exit

ci_Exit_Failure:
        xor     ax, ax

ci_Exit:
        
cEnd

sEnd CodeSeg

        end
