;****************************************************************************
;                                                                           *
; THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
; KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
; IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
; PURPOSE.                                                                  *
;                                                                           *
; Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
;                                                                           *
;****************************************************************************

;----------------------------------------------------------------------------
; VGA.ASM
;----------------------------------------------------------------------------
        .xlist
        include cmacros.inc
        incDevice = 1                   ;Include control for gdidefs.inc
        incDrawMode = 1                 ;Include DRAWMODE structure
        include gdidefs.inc
        include macros.inc
        include dibeng.inc
	include minivdd.inc
	include valmode.inc
        .list
;----------------------------------------------------------------------------
; S T R U C T U R E S
;----------------------------------------------------------------------------
DataBlockStruc struc
	bMode	 	db	?	;mode number
	bMemory		db	?	;Amount of memory for this mode
	wPitch		dw	?	;Mode pitch (width bytes)
	wModeFlags	dw	?	;Mode flags (currently unused)
	wBitBltDevProc	dw	?	;Hardware bitblt routine
DataBlockStruc ends

SCREENRGN	struc
	sr_rect		db size(RECT) dup (?)
	sr_width	dw		   ?
	sr_height	dw		   ?
SCREENRGN ends

AdapterEntryStruc struc
        bResolution     db      ?
        bBpp            db      ?
        pDataBlock      dw      ?
AdapterEntryStruc ends
;----------------------------------------------------------------------------
; E Q U A T E S
;----------------------------------------------------------------------------
GET386API       equ     1684h           ;Get API entry point from VxD.
VFLATD_VXD      equ     11Fh            ;id of VFLATD VxD
GET_VIDEO_SEL   equ     1               ;function # to call to get selector
VDD             equ     000Ah           ;id of Virtual Display Driver.
STOP_IO_TRAP    equ     4000h           ; stop io trapping
START_IO_TRAP	equ	4007h		; re-start io trapping

;----------------------------------------------------------------------------
; E X T E R N S  and  P U B L I C S
;----------------------------------------------------------------------------
	externNP XYtoRes		   ;in INIT.ASM 
	externFP SetRAMDAC_far		   ;in PALETTE.ASM

;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin  Data
externB bReEnable                       ;in INIT.ASM
externW wResolution                     ;in INIT.ASM
externW wDpi                            ;in INIT.ASM
externW wBpp                            ;in INIT.ASM
externD lpColorTable
externD lpDriverPDevice

public Rgn1
public Rgn2
Rgn1 SCREENRGN <>			;Off-screen regions.
Rgn2 SCREENRGN <>			;Off-screen regions.

globalW	BitBltDevProc,0	
globalW pAdapterEntry,0
globalW nNumEntries,0
externD VDDEntryPoint

globalW wCurrentMode,0
globalW wPDeviceFlags,0
globalD dwVideoMemorySize,0
globalW wScreenWidth,0
globalW wScreenHeight,0
globalW wMaxWidth,0
globalW wMaxHeight,0
globalW wScreenWidthBytes,0
externW OurVMHandle
globalW ScreenSelector,0                ;selector for the screen
sEnd    Data

;----------------------------------------------------------------------------
; S T R U C T U R E S
;----------------------------------------------------------------------------
FOURTHMEG = 0
HALFMEG	  = 1
ONEMEG	  = 2*HALFMEG
TWOMEG	  = 2*ONEMEG
THREEMEG  = 3*ONEMEG
FOURMEG   = 4*ONEMEG
FIVEMEG   = 5*ONEMEG
SIXMEG    = 6*ONEMEG
SEVENMEG  = 7*ONEMEG
EIGHTMEG  = 8*ONEMEG

;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
createSeg _INIT,InitSeg,word,public,CODE
sBegin  InitSeg
assumes cs,InitSeg
        .386

;----------------------------------------------------------------------------
; Adapter Table
;----------------------------------------------------------------------------
AdapterEntry macro      r,b,db
        NextEntry = NextEntry + 1
        AdapterEntryStruc <r,b,db>
        endm

NextEntry = 0
AdapterTable    label word
	AdapterEntry 1, 8,DB_1_8
	AdapterEntry 2, 8,DB_2_8
	AdapterEntry 3, 8,DB_3_8
nEntries = NextEntry

;----------------------------------------------------------------------------
; A D A P T E R   D A T A   B L O C K S
;----------------------------------------------------------------------------
; NOTE: these mode numbers do not work on ATI WONDER/Mach32 hardware.
; You must substitute with mode numbers for your particular hardware.
;
; The BANKEDSCAN bit is set if the scan width is such that scanlines can cross
; a bank boundary. Please clear this bit if you program your hardware so that
; scanlines will never cross a bank boundary. Of course you don't need this bit
; if your hardware operates in linear mode.

DB_1_8:
	DataBlockStruc <62h,ONEMEG,640,MINIDRIVER+VRAM+BANKEDVRAM+BANKEDSCAN,0>
					;640x480   8  bpp mode
DB_2_8:
	DataBlockStruc <63h,ONEMEG,800,MINIDRIVER+VRAM+BANKEDVRAM+BANKEDSCAN,0>
					;640x480   8  bpp mode
DB_3_8:
	DataBlockStruc <64h,ONEMEG,1024,MINIDRIVER+VRAM+BANKEDVRAM,0>
					;1024x768  8  bpp mode

;----------------------------------------------------------------------------
; PhysicalEnable
;----------------------------------------------------------------------------
PPROC	PhysicalEnable	near
        assumes ds,Data
        assumes es,nothing
        assumes gs,nothing
        assumes fs,nothing
	xor	ax,ax
	mov	pAdapterEntry,ax	;Force FindMode to search from top.
;----------------------------------------------------------------------------
; IF FirstTime THEN
;   IF not correct adapter then FAIL
;   GET Adapter memory size
; ENDIF
;----------------------------------------------------------------------------
	cmp	ScreenSelector,0
	jnz	PE_FindMode
	call	Is_MyAdapter
	or	ax,ax
	jz	PE_Fail

PLABEL PE_GetMemSize
	Call	GetMemSize
        mov     word ptr [dwVideoMemorySize],ax
        mov     word ptr [dwVideoMemorySize+2],dx

;----------------------------------------------------------------------------
; WHILE (ModeNum = FindMode(bpp,res,memsize)) 
;   IF SetMode(ModeNum,bpp,res,memsize) THEN
;     ScreenSelector = GetLinearSelector();
;     HOOK up with the VDD.
;     CLEAR the screen.
;   RETURN( SUCCESS )
; ENDWHILE
; RETURN( FAIL )
;----------------------------------------------------------------------------
PLABEL PE_FindMode
	call	FindMode
	or	bx,bx
	jz	PE_Fail
	mov	wCurrentMode,bx
	mov	ax,1
	call	SetMode
	jc	PE_FindMode

;----------------------------------------------------------------------------
; Get screen selector.
;----------------------------------------------------------------------------
PLABEL PE_GetVFlatDSelector
        push    si                      ;
        push    di                      ;
        call    GetScreenSelector       ;Get the screen selector from VFlatD.
        pop     di                      ;
        pop     si                      ;
        or      ax,ax                   ;Is screen selector zero?
        jz      PE_Fail                 ;yes. Error exit.
	mov	ScreenSelector,ax	;no. Store it.

;----------------------------------------------------------------------------
; Hook up with the vdd.
;
; The MiniVDD requires us to register ourselves with it in order to activate
; its ability to virtualize graphics mode DOS apps in a window or in the
; background.  The VDD will also allocate some memory so that it can perform
; this virtualization correctly.  Let's setup this call and pass down the
; proper information to the MiniVDD.
;----------------------------------------------------------------------------
PLABEL PE_HookUpWithVDD
	movzx	eax,wScreenWidthBytes	;get screen pitch
	movzx	edx,wScreenHeight	;get height of visible screen
	mul	edx			;EAX has total bytes of visible screen
	mov	ecx,eax 		;pass visible memory size in ECX
	mov	eax,VDD_DRIVER_REGISTER ;this is function code for VDD PM API
	movzx	ebx,OurVMHandle 	;VDD PM API needs this
	mov	di,_TEXT		;send ES:DI --> routine that VDD calls
	mov	es,di			;to set us back into Windows mode
	mov	di,CodeOFFSET ResetHiResMode
	xor	edx,edx 		;tell VDD to attempt to virtualize
	call	dword ptr VDDEntryPoint

;----------------------------------------------------------------------------
;The MiniVDD will return the total memory used by both the visible screen
;and the VDD itself.  We can therefore calculate the location of our off-screen
;cache areas and where the cursor is supposed to be cached etc.
;
;At this point:
; EAX contains VDD_DRIVER_REGISTER if the call failed.
; EAX contains -1 if we're running in a memory-shy state
;     (that is, if the visible screen extends into the last
;      physical 64K bank on card -- ie: 800x600x8 on a 512K card).
; EAX contains the size in bytes of the visible screen plus the
;     memory allocated by the VDD.
;
;----------------------------------------------------------------------------
	cmp	eax,VDD_DRIVER_REGISTER ;did the call fail?
	je	short PE_SaveState	;yes, just use values that we've got.

;----------------------------------------------------------------------------
;The VDD returned normally.  EAX contains the total amount of video memory
;used by both the visible screen and the VDD.  The VDD is using the memory
;directly below the visible screen.  That means that our stuff will have to
;be located below the VDD's reserved memory.
;----------------------------------------------------------------------------
PLABEL	PE_VDDReturnedNormal
	push	eax			;save memory used for now
	mov	edx,dwVideoMemorySize	;
	sub	edx,eax 		;EDX == bytes of scratch memory left
	mov	eax,edx 		;(get this into EAX for divide)
	xor	edx,edx 		;zero EDX for dword divide
	movzx	ebx,wScreenWidthBytes	;
	div	ebx			;AX == # of lines in scratch memory
	mov	Rgn2.sr_height,ax
	pop	eax			;get back memory used returned by VDD
	xor	edx,edx 		;zero EDX for dword lengthed divide
	div	ebx			;AX == # of lines used by screen & VDD
	or	dx,dx			;any remainder?
	jz	short @f		;nope, we're OK
	inc	ax			;yes! bump up another line
@@:	mov	Rgn2.top,ax		;save adjusted value

PLABEL PE_SaveState
;
;Call the VDD to order it to save the current register state.  This will
;hopefully assure that the Windows VM has the running state saved:
;
	mov	eax,VDD_POST_MODE_CHANGE;function code goes in EAX
	movzx	ebx,OurVMHandle 	;this is needed for the call
	call	dword ptr VDDEntryPoint ;call the VDD to do post setup
	mov	eax,VDD_SAVE_DRIVER_STATE
	movzx	ebx,OurVMHandle 	;
	call	dword ptr VDDEntryPoint ;

PLABEL PE_ClearScreen
        mov     si,wBpp
        call    ClearScreen
	clc				;Indicate success
	ret

PLABEL PE_Fail
	stc				;indicate failure
	ret
PhysicalEnable	endp

;----------------------------------------------------------------------------
; FindMode
; Exit:
;  bx = next mode, or 0 if no more matching modes.
; Sets:
;   nNumEntries, pAdapterEntry
;----------------------------------------------------------------------------
PPROC	FindMode	near
	mov	cx,nNumEntries
	cmp	pAdapterEntry,0
	jne	FM_NextMode
        mov     cx,nEntries
        mov     di,CodeOFFSET AdapterTable

PLABEL FM_CheckMode
	mov	ax,wResolution
	mov	dx,wBpp

PLABEL FM_CheckModeLoop
	cmp	al,cs:[di].bResolution
        jne     short FM_NextMode       ;no. skip this table entry.
	cmp	dl,cs:[di].bBpp
        jne     short FM_NextMode       ;no. skip this table entry.
	mov	eax,dwVideoMemorySize
	shr	eax,19			;eax = mem size in 512k units.
	mov	si,cs:[di].pDataBlock
	cmp	al,cs:[si].bMemory
	jl	short FM_NextMode
@@:	movzx	bx,cs:[si].bMode
	mov	pAdapterEntry,di
	mov	nNumEntries,cx
	ret
PLABEL FM_NextMode
	add	di,SIZE AdapterEntryStruc
        dec     cx                      ;Have we gone through the entire table?
        jnz     FM_CheckModeLoop        ;nope. Try the next entry.
	xor	bx,bx
	ret
FindMode	endp

;----------------------------------------------------------------------------
; GetMemSize
; Exit:
;   ax:dx == memory size of adapter
;----------------------------------------------------------------------------
PPROC	GetMemSize	near
	xor	ax,ax
	mov	dx,20h		;just report 2M for now
	ret
GetMemSize	endp

;----------------------------------------------------------------------------
; ClearScreen
; Entry: si = wBpp
;----------------------------------------------------------------------------
PPROC	ClearScreen_Far far
	call	ClearScreen
	retf
ClearScreen_Far 	endp

PPROC	ClearScreen	near
        assumes ds,Data
        assumes es,nothing
        assumes gs,nothing
        assumes fs,nothing
        mov     dx,ScreenSelector
        mov     es,dx
        shr     si,1                    ;si = 0,2,4,8,12,16
        movzx   ecx,PixelToByteTable[si]
        mov     dx,wScreenWidth
        mov     bx,dx                   ;bx = Screen Width in pels.
        cmp     si,12                   ;24 bpp?
        je      short @f                ;if no,
        xor     bx,bx                   ; then clear bx.
@@:     rol     dx,cl                   ;dx = screen width (adjusted for bpp).
        add     dx,bx                   ;Add in extra for 24 bpp (if needed).
        movzx   ebx,wScreenWidthBytes
        sub     bx,dx
        shr     dx,2
        xor     edi,edi
        mov     eax,edi
        mov     si,wScreenHeight
@@:     mov     cx,dx
        rep     stos dword ptr es:[edi]
        add     edi,ebx
        dec     si
        jnz     @b
        ret
ClearScreen     endp

;----------------------------------------------------------------------------
; GetScreenSelector
; Retrieves a screen selector from VFlatD.  This need only
; be done when a bank switched video adapter is installed.
; Entry:
;   cs:si-->Data Block
; Exit:
;   ax = Screen selector, 0 if error.
;----------------------------------------------------------------------------
PPROC GetScreenSelector near
        assumes ds,Data
        assumes es,nothing
        assumes gs,nothing
        assumes fs,nothing
        xor     di,di
        mov     es,di
        mov     ax,GET386API
        mov     bx,VFLATD_VXD
        int     2fh                     ;returns with es:di-->VFlatD Entry point
        xor     ax,ax                   ;assume error.
        mov     bx,es                   ;Is VFlatD entry point zero?
        or      bx,di                   ;
        jz      short GSS_None          ;Yes. Return a zero screen selector.
        pop     ax                      ;no. ax = our near return address
        push    cs                      ;push a far return address for the VDD
        push    ax                      ;to return to.
        push    es                      ;push the VDD entry point to call
        push    di
        mov     dx, GET_VIDEO_SEL       ;the argument to the VxD function
        mov     ax,2048                 ;size of frame buffer in pages
        mov     cx,cs                   ;es:di --> bank switch template
        mov     es,cx

	mov	di,CodeOFFSET BankSwitchTemplate
	mov	cx,BankSwitchTemplateSize

PLABEL GSS_VDDRet
        retf                            ;far ret to VxD entry pt.
PLABEL GSS_None
        ret                             ;error return point.
GetScreenSelector endp

;----------------------------------------------------------------------------
; BANK SWITCH TEMPLATES
;  This template is given to vflatd which copies it inline in to the
;  page fault handling code.
; NOTE: This code runs at ring 0 in a USE32 code segment, so be carefull!!!
; ALL REGISTERS MUST BE PRESERVED (except for ax,dx)
;
; You need to replace the template with adapter specific bank switch code.
; The following template works for ATI Wonder/Mach32 cards
;----------------------------------------------------------------------------
;----------------------------------------------------------------------------
; BankSwitchTemplate
; Entry:
;   ax = bank number (0-15)
; Exit:
; ALL REGISTERS MUST BE PRESERVED (except for ax,dx)
;----------------------------------------------------------------------------
BankSwitchTemplate label byte
	mov	ah,al
	shl	ah,1
	mov	al,0B2h
        db      66h,0bah,0ceh,01h       ;mov dx, 1CEh
	out	dx,eax

BankSwitchTemplateSize = $ - BankSwitchTemplate

;----------------------------------------------------------------------------
; Is_MyAdapter
; Returns:
;  ax:dx == 0 if not my adapter
;  ax:dx == 1 if is my adapter
;----------------------------------------------------------------------------
PPROC	Is_MyAdapter	near
        assumes ds,Data
        assumes es,nothing
        assumes gs,nothing
        assumes fs,nothing
;
; Adapter detection goes here
;
	xor	dx,dx
	mov	ax,1
	ret
Is_MyAdapter endp

;----------------------------------------------------------------------------
; SetMode
; Entry:
;  ax = 1: Full Init (reset globals)
;  ax = 0: Basic init (h/w only -- assumes Full Init has 
;                   happened before).
;  bx = mode number
; Exit:
;  CARRY if error
; Sets:
;   wScreenWidth, wScreenHeight, wScreenWidthBytes, wMaxWidth, 
;   wMaxHeight, wPDeviceFlags, Rgn1, Rgn2
;----------------------------------------------------------------------------
PPROC	SetMode	far

; call the VDD before setting the mode so that it can do some pre-mode change
; initializations.

	push	eax			;save these over call
	push	ebx			;
	mov	eax,VDD_PRE_MODE_CHANGE ;function code goes in EAX
	movzx	ebx,OurVMHandle 	;this is needed for the call
	call	dword ptr VDDEntryPoint ;call the VDD to set refresh rate
	pop	ebx			;restore saved registers
	pop	eax			;

; You need to replace the following mode set code with adapter specific mode
; set code. The following code works for ATI Wonder/Mach32 cards

        push    ax
	mov	dx,1ceh
	mov	al,0b6h
	out	dx,al
	inc	dx
	in	al,dx
	and	al,not 5
	mov	ah,al
	mov	al,0b6h
	dec	dx
	out	dx,ax
	mov	ax,bx
	push	ax
	or	al,80h			;don't erase screen at this time
	int	10h
	mov	ax,0F00h
	int	10h
	and	al,NOT 80h		;get rid of "don't erase screen" bit
	pop	bx
	cmp	al,7bh
	jne	short @f
	mov	al,62h
@@:	cmp	al,bl			;do modes match?
        pop     ax
	jne	SM_Error
	or	ax,ax
	jz	SM_Exit

;
; Get screen resolution
;
	mov	di,wResolution
        shl     di,2                    ;Resolution table has dword entries
        mov     cx,cs:ResolutionTable[di];get width.
        mov     dx,cs:ResolutionTable[di+2];get height
        mov     wScreenWidth,cx         ;store it.
	mov	wScreenHeight,dx	;store it.
;
; Get bytes/scan, pixels/scan
;
	mov	di,pAdapterEntry
	mov	si,cs:[di].pDataBlock
	movzx	eax,cs:[si].wPitch	;Width bytes.
	mov	wScreenWidthBytes,ax	;store width bytes.
	mov	dx,cs:[si].wBitBltDevProc; Get bitblt dev proc address
	mov	BitBltDevProc,dx	;   and save
	mov	dx,cs:[si].wModeFlags	;Get mode flags
	mov	wPDeviceFlags,dx	;   and save
	shl	eax,3			;eax = # of bits/scan
	mov	cx,wBpp
	xor	dx,dx
	div	cx			;ax = pels/scan
	mov	wMaxWidth,ax
;
; Compute max screen height
;
@@:     mov     ax,word ptr dwVideoMemorySize
        mov     dx,word ptr dwVideoMemorySize+2
        div     wScreenWidthBytes       ;AX contains # of scan lines
        mov     wMaxHeight,ax		;If max height < screen height then
					;AX = max screen height
        mov     di,wMaxWidth            ;DI = max screen width
        mov     dx,wScreenHeight        ;DX = screen height
        mov     bx,wScreenWidth         ;BX = screen width
        sub     di,bx                   ;DI = avail screen width of rgn1.
        js      SM_Error                ;Error if negative
        sub     ax,dx                   ;AX = height of rgn2.
        js      SM_Error                ;Error if negative

;----------------------------------------------------------------------------
; Compute offscreen memory layout.
;
; We have defined 2 regions:
;
; +-------------------------+       * RGN1 is non-existent in some modes.
; |                  |      |       
; |                  |      |
; |                  |      |
; |     VISIBLE      | RGN1 |
; |                  |      |
; |                  |      |
; |                  |      |
; |                  |      |
; |------------------+------|
; |                         |
; |                         |
; |          RGN2           |
; |                         |
; |                         |
; +-------------------------+
;
;----------------------------------------------------------------------------
	mov	Rgn1.left,bx
	mov	Rgn1.top,0
	mov	si,wMaxWidth
	mov	Rgn1.right,si
	mov	Rgn1.bottom,dx
	sub	si,bx
	mov	Rgn1.sr_width,si
	mov	Rgn1.sr_height,dx

	mov	Rgn2.left,0
	mov	Rgn2.top,dx
	mov	si,wMaxWidth
	mov	Rgn2.right,si
	mov	Rgn2.sr_width,si
	mov	si,wMaxHeight
	mov	Rgn2.bottom,si
	sub	si,dx
	mov	Rgn2.sr_height,si

PLABEL SM_Exit
	clc
	ret

PLABEL SM_Error
	stc
	ret

SetMode	endp

ResolutionTable label word
        dw      320,200
        dw      640,480
        dw      800,600
        dw      1024,768
        dw      1280,1024
        dw      1152,864
        dw      1600,1200

PixelToByteTable        label   word
        dw      -3              ;1 bpp
        dw      -1              ;4 bpp
        dw      0               ;8 bpp
        dw      0               ;unused
        dw      1               ;16 bpp
        dw      0               ;unused
        dw      1               ;24 bpp, additional adj required.
        dw      0               ;unused
        dw      2               ;32 bpp.

;----------------------------------------------------------------------------
; ValidateMode
; Attempt to set the mode and verify that it works.  Then set back to
; the original mode and return yes, maybe or mo.
; Entry:
;   lpValMode = DISPVALMODE structure
; Exit:
;   ax = VALMODE_YES (0), VALMODE_MAYBE (1), VALMODE_NO (2)
;----------------------------------------------------------------------------
cProc   ValidateMode,<FAR,PUBLIC,PASCAL,NODATA>,<esi,edi>
	parmD	lpValMode
cBegin
	mov	ax,DGROUP
	mov	ds,ax
        assumes ds,Data
        assumes es,nothing
        assumes gs,nothing
        assumes fs,nothing

	push	wResolution		;save current resolution 
	push	wBpp			; and bpp.
	push	pAdapterEntry

	cmp	ScreenSelector,0	;Is the driver currently running?
	jnz	VM_GetModeInfo		;yes.
	call	Is_MyAdapter		;no. Make sure we're on our adapter
	or	ax,ax			;If not, then fail the mode
	jz	VM_FailWrongDrv		; in question.
	Call	GetMemSize
        mov     word ptr [dwVideoMemorySize],ax
        mov     word ptr [dwVideoMemorySize+2],dx

PLABEL VM_GetModeInfo
	les	di,lpValMode
	mov	si,es:[di].dvmXRes
	shl	esi,16
	mov	si,es:[di].dvmYRes
	call	XYtoRes			;returns with ax = wResolution value
	mov	wResolution,ax
	mov	ax,es:[di].dvmBpp
	mov	wBpp,ax

	xor	ax,ax
	mov	pAdapterEntry,ax	;Force FindMode to search from top.
	call	FindMode
	or	bx,bx
	jz	short VM_FailNoMem
	mov	ax,VALMODE_YES
PLABEL VM_Done
	pop	pAdapterEntry
	pop	wBpp
	pop	wResolution
cEnd

PLABEL VM_FailNoMem
	mov	ax,VALMODE_NO_NOMEM
	jmp	VM_Done

PLABEL VM_FailWrongDrv
	mov	ax,VALMODE_NO_WRONGDRV
	jmp	VM_Done

sEnd	InitSeg

sBegin	Code
assumes	cs,Code
;----------------------------------------------------------------------------
; ResetHiResMode
;   This function is called by the VDD to restore the graphics adapter 
;   into graphics mode when switching to the system vm.  Note that this
;   function is in a page-locked segment.
;   
;   Called when reentering the windows vm.
;----------------------------------------------------------------------------
PPROC   ResetHiResMode far
	mov	ax,DGROUP
	mov	ds,ax
        assumes ds,Data
        assumes es,nothing
        assumes gs,nothing
	assumes fs,nothing
	push	esi
	push	edi
	mov	bx,wCurrentMode
	xor	ax,ax			;basic init.
	call	SetMode

	cmp	wBpp,8
	ja	short RHR_CallVDD
	mov	cx,256
	je	short @f
	mov	cx,16
	cmp	wBpp,4
	je	short @f
	mov	cx,2
@@:	xor	ax,ax
	push	ds
	lds	si,lpColorTable		;ds:[si]-->color table
	call	SetRAMDAC_far		;set up initial palette
	pop	ds

	les	si,lpDriverPDevice
lock	and	es:[si].deFlags,not BUSY

PLABEL RHR_CallVDD
;
;Call the VDD to order it to save the current register state.  This will
;hopefully assure that the Windows VM has the running state saved:
;
	mov	eax,VDD_SAVE_DRIVER_STATE
	movzx	ebx,OurVMHandle 	;
	call	dword ptr VDDEntryPoint ;
;
;Now clear the screen so the user sees black until UserRepaint happens:
;
	mov	si,wBpp 		;needed for call
	call	ClearScreen_Far 	;

	pop	edi
	pop	esi
	retf

ResetHiResMode endp



sEnd	Code

end
