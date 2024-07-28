       title   VDD - Virtual Display Device for HERC version 3.00
;******************************************************************************
;
;VDD - Virtual Display Device for HERC 
;
;   Author: MDW PAB
;
;   (C) Copyright MICROSOFT Corp. 1986 - 1989
;
;
;DESCRIPTION:
;	This driver provides virtual display hardware for all VMs except VM1.
;	Normal VMs display actions(writing to controller registers and video
;	RAM) is completely emulated.  VMDOSAPP occasionally fetches the
;	current state of the VDD and puts up the visible portion in a window
;	on the physical display. Physical VMs have control of the physical
;	display. However, the actions of the VM are monitored so that a
;	screen grab and save/restore can be performed. Since the video
;	memory can be read, only I/O port writes need be trapped. The
;	VM_VDD_Pg table contains the the page table entries for the
;	virtual VRAM or, for a physical VM, the copy of the VRAM that is
;	made when the VRAM is grabbed.	The VM_VDD_CPg table contains
;	the page table entries of memory that is used internally
;	by the VDD.  It is used as a copy of the VRAM(to keep track of changes)
;	for normal VMs and as a copy of the REAL VRAM for physical VMs(made
;	when the VM is suspended) for saving/restoring the physical VMs state.
;	Similarly, there are two copies of the video controller registers.
;
;******************************************************************************

	.386p

	INCLUDE VMM.INC
	INCLUDE VDD.INC
	INCLUDE HERC.INC
	INCLUDE DEBUG.INC

;******************************************************************************
; EXTRN and PUBLIC data
;
VxD_CODE_SEG
	EXTRN	VDD_Mem_UpdOff:NEAR
	EXTRN	VDD_Mem_ModPag:NEAR
	EXTRN	VDD_Mem_ACopy:NEAR
VxD_CODE_ENDS


VxD_DATA_SEG
	EXTRN	vgVDD:byte
	EXTRN	VDD_CB_Off:DWORD

Txt_Rect3_Off	dd	160*24
Txt_Rect2_Off	dd	160*12
Txt_Rect1_Off	dd	160*3
Txt_Rect0_Off	dd	0

RightSide	equ 720
LinesPerPage	equ 4096 / 90	;90 bytes per line
LinesPerScreen	equ 348
LinesPerFrame	equ LinesPerScreen / 4

VxD_DATA_ENDS

VxD_CODE_SEG

;******************************************************************************
;VDD_State_VMCreate
;
;DESCRIPTION:
;	Initializes current controller state for new VM.
;
;ENTRY: EBX = VM_Handle
;	EDI = VDD CB ptr
;
;EXIT:	none
;
;USES:	Flags, EAX, ECDX, EDX, ESI
;
BeginProc VDD_State_VMCreate, PUBLIC

;*******
; Initialize HERC state structure with values from System VM
;
	VMMCall Test_Sys_VM_Handle
	jz	SHORT skip_sys_copy

;	mov	esi, ebx
;	add	esi,[VDD_CB_Off]		; ESI = SYS VM VDD ptr
;	jmp	SHORT skip_sys_copy

copy_sys_state:
	push	ebx
	VMMCall Get_Sys_VM_Handle
	mov	esi,ebx
	pop	ebx
	add	esi,[VDD_CB_Off]		; ESI = SYS VM VDD ptr

	push	esi
	push	edi
	lea	edi,[edi.VDD_Stt]
.ERRNZ	HERC_State_Struc MOD 4
	mov	ecx,SIZE HERC_State_Struc/4
	push	edi
	push	ecx
	lea	esi,[esi.VDD_Stt]		; current state of system VM
        cld
	rep movsd				;   is copied to new VM state
	pop	ecx
	pop	esi
	pop	edi
	push	edi
	lea	edi,[edi.VDD_SttCopy]		;   and copy of current state
	rep movsd
	pop	edi
	pop	esi

skip_sys_copy:
	clc					; Return no error(CF=0)
	ret
EndProc VDD_State_VMCreate

;******************************************************************************
;VDD_State_GetStt      Build controller current state message
;
;DESCRIPTION:
;	Builds current controller state message at ESI
;
;ENTRY: EBX = VM_Handle
;	EDI = VDD CB ptr
;	ESI = Place to build message
;	CF = 1, use VDD_Stt; CF = 0, use VDD_SttCopy
;
;EXIT:	none
;
;USES:	Flags, EAX, ECX, EDX
;
BeginProc VDD_State_GetStt, PUBLIC

Assert_VDD_ptrs ebx,edi

	mov	ecx,esi

; Get current VM video state
	call	VDD_Get_Mode			; AL = current mode
VSGS_0:
	mov	[ecx.VDA_HERC_Mode],al		; Set current mode
%out Next two lines necessary?
;	mov	al,[esi.VPH_Rows]
;	mov	[ecx.VDA_HERC_Rows],al		; Set rows/page(text mode)

	mov	esi,ecx

; Note that grab ignores these
	call	VDD_Get_Posn			; Get position
	mov	[esi.VDA_HERC_CurX],cx		; Set cursor X position
	mov	[esi.VDA_HERC_CurY],ax		; Set cursor Y position

	movzx	eax,[edi.VDD_Stt.V_CStart]	; AX = cursor start scan line
	movzx	ecx,[edi.VDD_Stt.V_CEnd]	; AX = cursor end scan line
	test	[edi.VDD_Flags],fVDD_Hide	; Q: cursor hidden?
	jz	short VSGS_ShowCurs		;   N: return valid cursor
	mov	eax,1 			        ;   Y: return no cursor
	mov	ecx,0
VSGS_ShowCurs:
	mov	[esi.VDA_HERC_CurBeg],ax 	; Set start scan line
	mov	[esi.VDA_HERC_CurEnd],cx 	; Set end scan line
	mov	al,[edi.VDD_Stt.V_VDisp]	; Text lines displayed
	mov	[esi.VDA_HERC_Rows],al		; (normally 25)
	mov	al,[edi.VDD_Stt.V_HDisp]	; Text columns displayed
	mov	[esi.VDA_HERC_Colm],al		; (normally 80)
	mov	al,[edi.VDD_Stt.V_Cntrl]	; AL = Control port.
	mov	[esi.VDA_HERC_Cntrl],al		; Set control.
	mov	al,[edi.VDD_Stt.V_MaxScan]	; Character height (-1)
	mov	[esi.VDA_HERC_CharHgt],al	; (normally 7 or 13)
; Return the screen ON/OFF flag
	and	[esi.VDA_HERC_Flags],NOT fVDA_V_ScOff
        test    [edi.VDD_Flags],fVDD_MInit          ; Q: VMInit done yet?
        jnz     SHORT VSS_MainMemCheck              ;   Y: continue
	or	[esi.VDA_HERC_Flags],fVDA_V_ScOff    ;	N: Set screen off flag
        jmp     SHORT VSS_1
VSS_MainMemCheck:        
        test    [edi.VDD_EFlags],fVDE_NoMain        ; Q: No main mem?
        jz      SHORT VSS_1                         ;   N: continue
	or	[esi.VDA_HERC_Flags],fVDA_V_ScOff    ;	Y: Set screen off flag
VSS_1:
; Return the horizontal cursor tracking flag
	and	[esi.VDA_HERC_Flags],NOT fVDA_V_HCurTrk
	test	[edi.VDD_Flags],fVDD_HCurTrk	; Q: HCurTrk ON?
	jz	SHORT VSS_2			;   N: proceed
	or	[esi.VDA_HERC_Flags],fVDA_V_HCurTrk ;Y: Set HCurTrk flag
VSS_2:
	ret
EndProc VDD_State_GetStt

;******************************************************************************
;VDD_State_CtlrChgs	 Build controller change state by comparing prev. with cur.
;
;DESCRIPTION:
;	Compares the fields in the VM's VDD_Stt and VDDSttCopy and
;	sets bits in the message flag word pointed to by ESI. If only the cursor
;	position or size changed, just the cursor flag bit is set. If any
;	additional changes are detected, the controller change flag bit is set.
;
;ENTRY: EBX = VM_Handle
;	EDI = VDD CB ptr
;	ESI = Place to build message
;
;EXIT:  VDD_Mod_Flag bits modified
;       
;USES:	Flags, EAX
;
BeginProc VDD_State_CtlrChgs, PUBLIC

Assert_VDD_ptrs ebx,edi

	mov	al,[edi.VDD_Stt.V_Mode] 	; Current Mode
	test	al,fModVidEna			; Q: Video Enabled?
	jz	SHORT VDD_CC_Vid_Disabled	;   N: Delay reporting change
	mov	ah,[edi.VDD_SttCopy.V_Mode]	; Saved Mode
	and	ax,HERC_Mod_Mask+(HERC_Mod_Mask shl 8); Select bits
	cmp	ah,al				; Q: (Significant) Mode change?
	jne	SHORT VDD_CC_Chg_Ctl		;   Y: return controller changed

	mov	al,[edi.VDD_Stt.V_Cntrl]	; Current Control equal saved Control?
	cmp	al,[edi.VDD_SttCopy.V_Cntrl]
	jne	short VDD_CC_Chg_Ctl		;   Y: return controller changed

; Check for change in display offset
	mov	ax,word ptr [edi.VDD_Stt.V_AddrH]
	cmp	ax,word ptr [edi.VDD_SttCopy.V_AddrH] ; Q: Offset change?
	jne	SHORT VDD_CC_Chg_Ctl		;   Y: return controller changed

	mov	al,[edi.VDD_Stt.V_VDisp]	; Lines displayed
	cmp	al,[edi.VDD_SttCopy.V_VDisp]	; Q: Unchanged?
	jne	SHORT VDD_CC_Chg_Ctl		;   N: report it

	mov	al,[edi.VDD_Stt.V_HDisp]	; Columns displayed
	cmp	al,[edi.VDD_SttCopy.V_HDisp]    ; Q: Unchanged?
	jne	short VDD_CC_Chg_Ctl		;      N: report it

	cmp	[vgVDD.Vid_Type],Vid_Type_HERC112 ; Q:Check Xmode register?
	jne	short VDD_CC_NoxMd		; N: Skip this check

	mov	al,[edi.VDD_Stt.V_xMode]	; xMode register
	cmp	al,[edi.VDD_SttCopy.V_xMode]	; Q: Unchanged?
	jne	short VDD_CC_Chg_Ctl		;   N: report it

VDD_CC_NoxMd:
;	controller mode and origin not changed
	call	VDD_Chk_Cursor			;
	jne	SHORT VDD_CC_Chg_Curs		; Y: cursor changed
VDD_CC_Vid_Disabled:
	ret

; CRTC register affecting display changed
VDD_CC_Chg_Ctl:
	or	[esi.VDD_Mod_Flag],fVDD_M_Ctlr+fVDD_M_Curs ; Set ctrlr chg flag
	ret                                     ; ZF is clear
; CRTC cursor size or address changed
VDD_CC_Chg_Curs:
	or	[esi.VDD_Mod_Flag],fVDD_M_Curs	; Set cursor changed flag
	ret                                     ; ZF is clear

EndProc VDD_State_CtlrChgs

;******************************************************************************
;VDD_Chk_Cursor 	Determine if Cursor changed state
;
;DESCRIPTION:
;
;ENTRY: EBX = VM_Handle
;	EDI = VDD CB ptr
;
;EXIT:	ZF = 0 indicates change detected
;
;USES:	Flags, EAX
;
;ASSUMES:
;
BeginProc VDD_Chk_Cursor

; Check for change in cursor
	mov	ax,word ptr [edi.VDD_Stt.V_CAddrH]
	cmp	ax,word ptr [edi.VDD_SttCopy.V_CAddrH] ; Q: Curs off chg?
	jne	short VDD_CC_Change		;   Y: return cursor changed
	mov	ax,word ptr [edi.VDD_Stt.V_CStart]
	cmp	ax,word ptr [edi.VDD_SttCopy.V_CStart] ; Q: Curs scan chg?
VDD_CC_Change:
	ret
EndProc VDD_Chk_Cursor

;******************************************************************************
;
;VDD_State_MemChgs Build VRAM change state by comparing prev. with cur.
;
;DESCRIPTION:
;	Compares the data in the VM's CB_VD.VDD_Pg and CB_VD.VDD_CPg and
;	sets a flag indicating a change if any and sets the type, count
;	and creates a change list.
;	During execution of the inner loop, ECX is the chars remaining on the
;	line count and EDX is the lines remaining on the page.	These values
;	are saved in the modification list and then adjusted to an enclosing
;	rectangle on routine exit(if any changes were found).
;	e.g.: a single change at 0,0 is stored as 79,24,79,24 and
;		adjusted to 0,0,1,1
;
;ENTRY: EBX = VM_Handle
;	ESI = Place to build message
;
;EXIT:	Message modified if any changes detected
;	CX = # of bytes in change message
;
;USES:	Flags, EAX, ECX, EDX, EDI
;
;CALLS:
;
;ASSUMES:
;	Message, except for controller and cursor flag bits is zero
;	Mod_List will not be looked at unless fVDD_M_VRAM flag is set
;	any change box (scroll or modified) is specified by the row
;	and col. OUTSIDE the box. This is the way VMDOSAPP was spec'd
;	Called only for emulation apps(i.e. not physical VMs)
;
BeginProc VDD_State_MemChgs, PUBLIC

Assert_VDD_ptrs ebx,edi
	push	ebp				; EBP and EBX are scratch
	push	ebx
	call	VDD_Mem_UpdOff			; Update display offset

;*******
; Initialize Mod structure: no mem changes, length = 4 bytes
	mov	[esi.VDD_Mod_count], 0

%OUT check mem init before checking for changes?
;;	  test	  [edi.VDD_Flags],fVDD_MInit	  ; Q: memory initialized?
;;	jz	SHORT VMC_Exit			;   N: not valid just now
	call	VDD_Get_Mode			; EAX = video mode

	cmp	al,2				; Q: Herc Graphics mode?
	je	SHORT VMC_HercGraphics		;   Y: Do graphics processing
	mov	ecx,esi
	lea	esi,[edi.VDD_Pg]
	call	VDD_Mod_Text			; Build text changes, if any
VMC_Exit:
	pop	ebx
	pop	ebp
	ret

;*******
; Build Herc graphics mode change structure
VMC_HercGraphics:

	cmp	[edi.VDD_Cpg.VPH_Mode], al	; Q: Did mode change?
	je	SHORT VMC_no_mode_chg		;   N:
	xor	eax, eax			;   Y: update whole screen
	mov	edx, LinesPerScreen
	jmp	SHORT VMC_set_rect

VMC_no_mode_chg:
	call	VDD_Mem_ModPag			; Q: Pages changed?
	mov	edx, eax
	jz	SHORT VMC_Exit			;   N: No changes

	xor	eax, eax
	bsf	ecx, edx
	and	cl, 7
	shr	ecx, 1				;Q: odd page
	jnc	short VMC_not_odd		;   N:
	add	eax, LinesPerPage * 4
VMC_not_odd:
	add	eax, ecx			; eax = starting line #

	bsr	ecx, edx
	mov	edx, LinesPerPage * 4 - 3	; edx = last line in even page
	and	cl, 7
	shr	ecx, 1				;Q: odd page
	jnc	short VMC_not_odd_2		;   N:
	mov	edx, LinesPerFrame * 4 - 3	; edx = last line in frame
VMC_not_odd_2:
	add	edx, ecx			; edx = ending line #

VMC_set_rect:
	or	[esi.VDD_Mod_Flag], fVDD_M_Type_Rect+fVDD_M_VRAM
	xor	ecx, ecx
	mov	[esi.VDD_Mod_List.R_Left], cx
	mov	[esi.VDD_Mod_List.R_Top], ax
	inc	ecx
	mov	[esi.VDD_Mod_Count], cx
	mov	[esi.VDD_Mod_List.R_Right], RightSide
	mov	[esi.VDD_Mod_List.R_Botm], dx
	mov	ecx,SIZE Rect + 4		; ecx = # bytes in change msg
	jmp	VMC_Exit

VDD_State_MemChgs endp


;******************************************************************************
;VDD_State_Query    Determine if state of device is changed
;
;DESCRIPTION:
;
;ENTRY: EBX = VM Handle
;	EDI = VDD CB ptr
;
;EXIT:	CF = 1 indicates change detected
;
;USES:	Flags
;
BeginProc VDD_State_Query,PUBLIC

	push	eax
Assert_VDD_ptrs ebx,edi
	test	[edi.VDD_Flags],fVDD_Win	; Q: Windowed VM?
	jz	SHORT VSQ_Exit			;   N: report no change

;	test	[edi.VDD_Event_Flags],fVDE_NoMain ; Q: Valid state?
;	jnz	SHORT VSQ_Exit			;   N: report no change
;	test	[edi.VDD_Flags],fVDD_MEna	; Q: Valid state?
;	jz	short VSQ_Exit			;   N: report no change
	call	VDD_Mem_UpdOff			; Update display offset
	call	VDD_Mem_ModPag			; check video displayed pages
if2
%out only report changes to currently displayed pages
endif
	jnz	SHORT VSQ_Change		; ZF=1 means no change
	call	VDD_Chk_Cursor			; ZF = 0 if cursor changed
	jnz	SHORT VSQ_Change
	test	[edi.VDD_LastMode],-1		; Q: Change in display mode?
	js	SHORT VSQ_Change		;   Y: Report controller change

; Finally, check to see if screen was turned off or on
	test	[edi.VDD_Stt.V_Mode],fModVidEna ; Q: Video on?
	jnz	SHORT VSQ_1
	test	[edi.VDD_SttCopy.V_Mode],fModVidEna ; Q: Screen off change?
	jnz	SHORT VSQ_Change		;	Y: return ctlr chged
	jmp	SHORT VSQ_Exit
VSQ_1:
	test	[edi.VDD_SttCopy.V_Mode],fModVidEna ; Q: Screen off change?
	jnz	SHORT VSQ_Exit			;	    N: return no chg
VSQ_Change:
	pop	eax
	stc
	ret

VSQ_Exit:
	pop	eax
	clc
	ret
EndProc VDD_State_Query

;******************************************************************************
;VDD_State_Update
;
;DESCRIPTION:
;	Copies the fields in the VM's VDD_Stt to VDDSttCopy
;
;ENTRY: EBX = VM_Handle
;	EDI = VDD control block data ptr
;
;EXIT:
;
;USES:	Flags, ECX
;
;CALLS:
;
BeginProc VDD_State_Update, PUBLIC

Assert_VDD_ptrs ebx,edi
	pushfd

	push	esi
	push	edi
	lea	esi,[edi.VDD_Stt]		; ESI = current state ptr
	lea	edi,[edi.VDD_SttCopy]		; EDI = copy of state ptr
	mov	ecx,(SIZE HERC_State_Struc)/4	; ECX = size of state
.ERRNZ size HERC_State_Struc mod 4
	cld
	rep movsd
	pop	edi
	pop	esi
	mov	cl,[edi.VDD_Mode]
	mov	[edi.VDD_LastMode],cl
	and	[edi.VDD_Flags],NOT fVDD_HCurTrk; Cancel horiz cursor track
	popfd
	ret
EndProc VDD_State_Update


;******************************************************************************
;VDD_Get_Mode	    Determine current mode
;
;DESCRIPTION:
;	Determine current mode from controller state
;
;ENTRY: EBX = VM_Handle
;	EDI = VDD control block data ptr
;
;EXIT:	EAX (AL) = mode
;
;USES:	Flags,EAX
;
;==============================================================================

BeginProc VDD_Get_Mode

Assert_VDD_ptrs ebx,edi
	xor	eax, eax
	mov	al,[edi.VDD_Stt.V_Cntrl];Check that graphics is enabled.
	test	al,0001b		;Graphics enabled?
	jz	SHORT herc_text		;No.

	test	[edi.VDD_Stt.V_Mode],0010b ; mode in eax
	jz	SHORT herc_text		;No.

	mov	al,HERC_MODE_Grfx
	jmp	short Ret_Mode

herc_text:
	mov	al,HERC_MODE_Text
	cmp	[vgVDD.Vid_Type],Vid_Type_HERC112
	jne	short Ret_Mode		; 48K ram font mode only on 112
	mov	ah,[edi.VDD_Stt.V_xMode]
	test	ah, HERC_RamFont_Mask	;Q: ram font mode?
	jz	short Clear_AH		;   N:
	mov	al,HERC_MODE_4KText + HERC_MODE_48KText  ;   Y:
	test	ah,HERC_48KMod_Mask	;Q: 48K ram font mode?
	jz	short Clear_AH		;   N: 4K ram font mode
	mov	al,HERC_MODE_48KText	;   Y:
Clear_AH:
	xor	ah,ah
Ret_Mode:
	mov	[edi.VDD_Mode], al	; VDD_Mode can have the 4K RamFont bit set
	and	al, NOT HERC_MODE_4KText ; clear possible 4K RamFont mode bit
	ret

EndProc VDD_Get_Mode

;******************************************************************************
;VDD_Get_Posn
;
;DESCRIPTION:
;	Determine current position from controller state
;
;ENTRY: EBX = VM_Handle
;	EDI = VDD control block data ptr
;
;EXIT:	EAX = Current Line for cursor
;	ECX = Current Offset within line
;
;USES:	Flags, EAX, ECX
;
;==============================================================================
BeginProc VDD_Get_Posn

	push	edx
	movzx	ecx,[edi.VDD_Stt.V_AddrL]
	mov	ch,[edi.VDD_Stt.V_AddrH]
	movzx	eax,[edi.VDD_Stt.V_CAddrL]
	mov	ah,[edi.VDD_Stt.V_CAddrH]
	sub	eax,ecx 			; AX = byte offset into cur page
	movzx	ecx, [edi.VDD_Stt.V_HDisp]
	xor	edx, edx			; for div
	jecxz	short unknown
	div	cx				; AX = line number
	mov	ecx, edx			; CX = offset
	mov	edx, eax
unknown:
	mov	eax, edx
	pop	edx
	ret

EndProc VDD_Get_Posn


;******************************************************************************
;VDD_Mod_Text
;
;DESCRIPTION:
;	Modifies change structure to indicate rectangles of changed text.
;	Assumes the VDD_Scrn_Size has already been called for current mode,
;	setting VDD_Pg.VPH_LLen and VPH_PgSiz.
;
;ENTRY: EAX = mode
;	EDI = VDD CB ptr
;	ESI = VDD_Pg ptr
;	ECX = change structure ptr
;
;EXIT:	none
;
;USES:	Flags, EAX, ECX, EDX
;
BeginProc VDD_Mod_Text

	push	ebx
	push	ebp
	push	ecx

	push	eax
	call	VDD_Mem_ModPag			; Q: Pages changed?
	mov	ebx, eax
	pop	eax
	jz	SHORT VMT_Exit			;   N: No changes

	test	bl,1				; Q: 1st page mod'd?
	jz	SHORT VMT_Exit			;   N: No changes

	test	[edi.VDD_CPg.VPH_MState.VDA_Mem_Addr],-1; Q: Copy mem alloc'd?
	jz	SHORT VMT_AllChgd		;   N: Report all changed

	xor	ebp,ebp 			; EBP = mod'd offset from ESI

; Determine offset of first change, ESI = display start
	mov	edx,1000h
	mov	ecx,[esi.VPH_PgSz]		; ECX = displayed mem size
VMT_04:

; EBP = offset of first mod'd page from first displayed page
; ECX = bytes of displayed memory left to end from first mod'd page
	mov	edx,[esp]			; EDX = chg struc ptr
	call	VMT_Comp			; Build change structure entries
        call    VMT_Check_Scroll		   ; Check for scrolling
; Set up return
VMT_Exit:
	pop	esi
	pop	ebp
	pop	ebx
	ret

; Here for low memory situations, return entire screen changed
VMT_AllChgd:
	mov	edx,[esp]
	xor	eax,eax
	mov	[edx.VDD_Mod_List.R_Left],ax
	mov	[edx.VDD_Mod_List.R_Top],ax
	inc	eax
	mov	[edx.VDD_Mod_Count],ax
	movzx	eax,[esi.VPH_LLen]
	shr	eax,1				    ; Text is 2 bytes per column
	mov	[edx.VDD_Mod_List.R_Right],ax
	movzx	eax,[esi.VPH_Rows]
	mov	[edx.VDD_Mod_List.R_Botm],ax
	jmp	VMT_Exit

;******************************************************************************
;
; VMT_Comp
;
;DESCRIPTION:
;	Compare current and copy state for one page, generate change buffer,
;	which consists of pairs of offsets to start and end of change
;	relative to first mod'd page. Note that display offset is always
;	zero for the copy memory
;
;ENTRY: EDX = change structure pointer
;	EDI = VDD CB ptr
;	EBP = offset of first mod'd page from first displayed page
;	ECX = bytes to end of displayed video memory from first mod'd page
;
;EXIT:	EBX = last change buffer entry + 4 pointer
;
;USES:	Flags, EAX, EDX
;
VMT_Comp:
	push	edi
	push	esi
	push	ecx
	mov	ebx,edi 			; EBX = VDD CB ptr

;*******
; Search from end to first modified display memory
	movzx	esi,[ebx.VDD_Pg.VPH_MState.VDA_Mem_DispPag]
	movzx	esi,[ebx.VDD_Pg.VPH_MState.VDA_Mem_PgMap][esi]
	shl	esi,12
	add	esi,[ebx.VDD_Pg.VPH_MState.VDA_Mem_DPagOff]
	add	esi,[ebx.VDD_Pg.VPH_MState.VDA_Mem_Addr]
	add	esi,ebp
	add	esi,ecx
	sub	esi,4				; ESI = last dword disp'd mem

	mov	edi,[ebx.VDD_CPg.VPH_MState.VDA_Mem_Addr]
	add	edi,ebp
	add	edi,ecx
	sub	edi,4				; EDI = last dword disp'd mem

	shr	ecx,2
	std
	repe cmpsd				; ECX = offset of last chg
	jz	VMTC_Done 		;   Exit if no changes
	shl	ecx,2
	add	ecx,ebp

; ECX = last change offset from start of displayed pages
;
; The VMTC_CheckRect routine stores the rect in the change structure

	mov	eax,[Txt_Rect2_Off]		; EAX = Rect offset from ESI,EDI
	call	VMTC_CheckRect			; Check rectangle for changes
	jz	SHORT VMTC_Done

	mov	eax,[Txt_Rect1_Off]		; EAX = Rect offset from ESI,EDI
	call	VMTC_CheckRect			; Check rectangle for changes
	jz	SHORT VMTC_Combine

	mov	eax,[Txt_Rect0_Off]		; EAX = Rect offset from ESI,EDI
	call	VMTC_CheckRect			; Check rectangle for changes
    ;
    ; Combine rectangles if they are sufficiently close
    ; Atmost there can be three rects one each from Rect2,Rect1 and Rect0.
    ; 
    ;   |-------------|Rect0 - Lines 0-2
    ;   |             |
    ;   |-------------|
    ;   |             |Rect1 - Lines 3-11
    ;   |             |
    ;   |-------------|
    ;   |             |Rect2 - Lines 11-24
    ;   |             |
    ;   |             |
    ;   |             |
    ;   |-------------|
    ;
    ; The rects are stored such that the top rect comes last Rect2/Rect1/Rect0
    ; 
    ; if (two rects)
    ;     Try to combine the two
    ; else if (three rects)     {
    ;     if (the last two rects can be combined)
    ;           Try to combine the remaining two
    ;     else if (the last two cannot be combined)
    ;           Try to combine the first two 
    ;           If successful copy third onto the second
    ; }
    ;

VMTC_Combine:
	cmp	[edx.VDD_Mod_Count],1           
        jbe     SHORT VMTC_Done                 ; need atleast two rects

	cmp	[edx.VDD_Mod_Count],2
        je      SHORT VMTC_CombineTwo           ; only two rects to combine
    ;
    ; Three rects exist - Rect2,Rect1,Rect0
    ; Try to combine Rect1 and Rect0
    ;
        push    edx
	lea	edx,[edx.VDD_Mod_List][8]       ; EDX = 2nd rect
        call    VMTC_CombineRect                     
        pop     edx
        jz      SHORT VMTC_CombineTwo
        dec     [edx.VDD_Mod_Count]             ; dec count only if we combine
VMTC_CombineTwo:
    ;
    ; At this stage we can have 
    ; Case1: Rect2,Rect1 OR Rect1,Rect0 OR Rect2,Rect0 
    ;   Combine the two if possible.
    ; Case2: Rect2,(Rect1+Rect0) 
    ;   Combine the two if possible.
    ; Case3: Rect2,Rect1,Rect0 with Rect1 not Adj to Rect0 
    ;   Combine Rect2 and Rect1. If successful, copy Rect0 to Rect1
    ; 
        push    edx
	lea	edx,[edx.VDD_Mod_List]          ; EDX = 1st rect
        call    VMTC_CombineRect                ; try to combine top two rects
        pop     edx
        jz      SHORT VMTC_Done                 ; couldn't combine
        dec     [edx.VDD_Mod_Count]
        cmp	[edx.VDD_Mod_Count],1           
        jbe     SHORT VMTC_Done                 ; no more rects to combine
    ;
    ; Case3:
    ; Make the third rect the second rect.
    ;                
	mov	ax,[edx.VDD_Mod_List.R_Left][2*8]
	mov	[edx.VDD_Mod_List.R_Left][1*8],ax
	mov	ax,[edx.VDD_Mod_List.R_Right][2*8]
	mov	[edx.VDD_Mod_List.R_Right][1*8],ax
	mov	ax,[edx.VDD_Mod_List.R_Top][2*8]
	mov	[edx.VDD_Mod_List.R_Top][1*8],ax
	mov	ax,[edx.VDD_Mod_List.R_Botm][2*8]
	mov	[edx.VDD_Mod_List.R_Botm][1*8],ax
        
VMTC_Done:
	pop	ecx
	pop	esi
	pop	edi
	ret

;
; Called Entry point:
; EDX = Ptr to rect in the VDD_Mod_List
;
; EXIT: Combine Rect at EDX+8 into rect at EDX if possible.
; ZF set if rects could not be combined.
;
; Two rectangles are combined if there is 1 or less number of lines seperating
; them.
;
; VDD_Mod_Count not modified, caller should modify this if necessary
;
VMTC_CombineRect:
	mov	ax,[edx.R_Top]
	sub     ax,[edx.R_Botm][1*8]
        cmp     ax,2
        jg      SHORT VCR_Fail          ; WARNING! This has to be JG, not JA
                                        ; to account for AX being negative!
        mov     ax,[edx.R_Top][1*8]
        mov     [edx.R_Top],ax
        mov     [edx.R_left],0          ; Set Left=0 and Right=80 for combined rects
        mov     [edx.R_Right],80
        mov     eax,1
        or      eax,eax
        ret
VCR_Fail:
        xor     eax,eax
        ret
                
;******************************************************************************
;
; VMTC_CheckRect
;
;DESCRIPTION: Emits a rectangle if modification is found between [EAX] and
;	[EBP+ECX]
;
;ENTRY: EDX = change structure pointer
;	EBX = VDD CB ptr
;	EBP = offset from first displayed page to first mod'd page
;	ECX = offset from first mod'd page to last change
;	EAX = offset from display start to start of current rectangle
;
;EXIT:	ZF = 1 indicates no more changes
;	ECX = offset of last change before current rect
;
;ASSUMES: Text mode only
;	The rectangles are not overlapping and rectangles with
;	the larger valued offset are processed before those with a
;	smaller offset.
;	The offset from the mod'd page to the change is linear. That is,
;	all intervening pages are allocated.
;
;USES:	Flags, EAX, ECX, EDX
;
VMTC_CheckRect:
	cmp	ecx,eax 			; Q: Chg end in this rectangle?
	jb	VMTC_NoRectChg			;   N: check next
						;   Y: Save end and find start
	movzx	esi,[ebx.VDD_Pg.VPH_MState.VDA_Mem_DispPag]
	movzx	esi,[ebx.VDD_Pg.VPH_MState.VDA_Mem_PgMap][esi]
	shl	esi,12
	add	esi,[ebx.VDD_Pg.VPH_MState.VDA_Mem_DPagOff]
	add	esi,[ebx.VDD_Pg.VPH_MState.VDA_Mem_Addr]
	add	esi,eax 			; ESI = main mem start addr

	mov	edi,[ebx.VDD_CPg.VPH_MState.VDA_Mem_Addr]
	add	edi,eax 			; EDI = copy mem start addr
	push	esi
	push	edi
	push	eax				; Save rect start offset
	push	ecx				; Save offset to last change-4

; Now, while we have the registers, put the end of the rect in the structure
	mov	eax,ecx
	movzx	ecx,[edx.VDD_Mod_Count] 	; ECX = rectangle index
	push	edx
	xor	edx,edx
	div	[ebx.VDD_Pg.VPH_LLen]		; AX = line #, DX = col*2
	add	edx,4				; DWORD accuracy!
	cmp	dx,[ebx.VDD_Pg.VPH_LLen]	; Q: Start of next line?
	jc	SHORT VMTC_CR00 		;   N: Continue
	xor	edx,edx 			;   Y: adjust
	inc	eax
VMTC_CR00:
	inc	eax
	add	edx,4
	shr	edx,1				; 2 bytes per column
	xchg	ebx,[esp]
	mov	[ebx.VDD_Mod_List.R_Botm][ecx*8],ax
	mov	[ebx.VDD_Mod_List.R_Right][ecx*8],dx
	pop	edx
	xchg	ebx,edx

; Compare forward from start of rect to first change or up to last change
	mov	ecx,[esp]
	mov	eax,[esp+4]
	shr	eax,2
	shr	ecx,2
	sub	ecx,eax
	inc	ecx
	cld
	repe cmpsd				; Find first chg in Rect 3
	shl	ecx,2				; Convert to bytes
; Store start of new rectangle in VDD_Mod structure
	pop	eax
	sub	eax,ecx 			; EAX = offset of first chg
	movzx	ecx,[edx.VDD_Mod_Count] 	; ECX = rectangle index
	mov	esi,edx
	xor	edx,edx
	div	[ebx.VDD_Pg.VPH_LLen]		; AX = line #,
	shr	edx,1				; DX = col
	mov	[esi.VDD_Mod_List.R_Top][ecx*8],ax
	inc	eax
	cmp	[esi.VDD_Mod_List.R_Botm][ecx*8],ax ; Q: One line rectangle?
	jz	SHORT VMTC_OneLine		;	Y: cur vals are right
	movzx	edx,[ebx.VDD_Pg.VPH_LLen]
	shr	edx,1				;	N: right = llen/2 + 1
	mov	[esi.VDD_Mod_List.R_Right][ecx*8],dx
	xor	edx,edx 			;	    and left = 0
VMTC_OneLine:
	mov	[esi.VDD_Mod_List.R_Left][ecx*8],dx
IFDEF RECTDEBUG
	dec	eax
Trace_Out "VDD:Text changes at #AX row, #DX column" +
	inc	eax
	push	eax
	push	edx
	mov	dx,[esi.VDD_Mod_List.R_Right][ecx*8]
	mov	ax,[esi.VDD_Mod_List.R_Botm][ecx*8]
Trace_Out " to #AX row, #DX column"
	pop	edx
	pop	eax
ENDIF
	inc	ecx
	mov	[esi.VDD_Mod_Count],cx		; Update rectangle count
	mov	edx,esi
	or	[edx.VDD_Mod_Flag],fVDD_M_Type_Rect+fVDD_M_VRAM

; Find last change in next previous rectangle
	pop	ecx				; ECX = this rect offset
	shr	ecx,2				;   is len to start of mod'd pgs
	mov	eax,ecx
	pop	edi
	pop	esi
        sub     edi,4           ; last dword in the previous rect - copy mem
        sub     esi,4           ; last dword in the previous rect - main mem
	std
	repe cmpsd				; Find last chg, next rect
        pushfd                  ; safety - if ECX = 0
	shl	ecx,2				; Convert to bytes
        popfd
VMTC_NoRectChg:
	ret

;*****************************************************************************
;
; VMT_Check_Scroll
;
; Scan the VDD_Mod_List for rects which can be replaced by a Scroll rect and
; a smaller update rect.
; 
; For each rect in the VDD_Mod_List {
;    if (Rect > 1/3 of screen size) 
;       Try replacing it by a scroll rect and a smaller update rect.
; }         
;            
; ENTRY: EBX = VM CB ptr
;        EDX = Ch struct ptr
;
; USES: EAX,ECX
public VMT_Check_Scroll

VMT_Check_Scroll:
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    esi     
        push    edi

    ;
    ; Setup ESI = display start, EDI = Copy page start
    ;
	movzx	esi,[ebx.VDD_Pg.VPH_MState.VDA_Mem_DispPag]
	movzx	esi,[ebx.VDD_Pg.VPH_MState.VDA_Mem_PgMap][esi]
	shl	esi,12
	add	esi,[ebx.VDD_Pg.VPH_MState.VDA_Mem_DPagOff]
	add	esi,[ebx.VDD_Pg.VPH_MState.VDA_Mem_Addr]
	mov	edi,[ebx.VDD_CPg.VPH_MState.VDA_Mem_Addr]

        movzx   ecx,[edx.VDD_Mod_Count]
        jecxz   SHORT VCS_Exit
        mov     eax,edx                 ; EAX = Ch Struct ptr
        lea     edx,[edx.VDD_Mod_List]  ; EDX = First rect in the list
VCS_NextRect:               
    ;
    ; Rect greater than 1/3 screen size?
    ;
        push    ecx
        push    eax
        mov     ecx,25/3
        mov     ax,[edx.R_Botm]
        sub     ax,[edx.R_Top]
        cmp     ax,cx                   ; Q: Rect > 1/3 Screen Size?
        pop     eax
        pop     ecx
        jb      SHORT VCS_01            ;    N: Not worth scrolling
    ;
    ; Make ESI,EDI point to start of rect
    ;
        push    eax
        push    edx
        movzx   eax,[edx.R_Top]
        mul     [ebx.VDD_Pg.VPH_LLen]
        add     esi,eax                 
        add     edi,eax                 
        pop     edx
        pop     eax
        call    ConvertToScroll
VCS_01:
        add     edx,8                   ; point to next rect
        loopd   SHORT VCS_NextRect
VCS_Exit:
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        ret

;*****************************************************************************
;
; ConvertToScroll:
;       convert an update rect to a scroll rect + an update rect
;
; ENTRY:
;       EAX = ptr to Ch struct
;       EBX = VM CB ptr
;       EDX = ptr to a rect in the VDD_Mod_List
;       ESI = start of rect in displayed memory
;       EDI = start of rect in copy memory
;
; EXIT: If successful, converts the update rect to a scroll rect and adds
;       an update rect at the end of the VDD_Mod_List.
;
public ConvertToScroll
ConvertToScroll:

        push    eax
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        
        mov     ecx,7                   ; check upto scrolling UP by 7 lines
CompareLoop:
        push    ecx
    ;
    ; Count Dwords for comparing.
    ; count = [# of lines in updrect-(7-ECX+1)] * VPH_LLen bytes
    ;
        movzx   eax,[edx.R_Botm]
        sub     ax,[edx.R_Top]
        add     eax,ecx         
        sub     eax,8

        push    edx
        mul     [ebx.VDD_Pg.VPH_LLen]
        pop     edx                     ; EAX = # of bytes in the update rect
        shr     eax,2                   ; EAX = dword count
        mov     ecx,eax
    ;
    ; point to update rect shifted by one more line(in copy pages)
    ;
        movzx   eax,[ebx.VDD_Pg.VPH_LLen]
        add     edi,eax
        cld
    ;
    ; compare update rect with copy rect shifted by (7-ECX+1) lines
    ;
        push    esi
        push    edi
        repe cmpsd
        pop     edi
    ;
    ; compute # of lines in the scroll rect
    ;
        mov     eax,esi
        pop     esi
        push    esi
        sub     eax,esi                 ; EAX = width of scrollrect in bytes
        push    edx
	xor	edx,edx
	div	[ebx.VDD_Pg.VPH_LLen]	 
        pop     edx                     ; AX = # of lines in Scrollrect
    ;
    ; Scroll rect big enough?
    ;
        mov     cx,[edx.R_Botm]
        sub     cx,[edx.R_Top]
        inc     cx
        shr     cx,1
        cmp     ax,cx                   ; Q: Scroll rect >= 1/2 of Update Rect
        jl      CTS_NextScroll          ;   N: 
public FoundScrollRect
FoundScrollRect:
IFDEF RECTDEBUG
        push    eax
        push    ebx
        mov     ax,[edx.R_Top]
        mov     bx,[edx.R_Botm]
Trace_Out "Given UpdRect - Top=#AX,Bot=#BX"
        pop     ebx
        pop     eax
ENDIF
        pop     esi
        pop     ecx
    ;
    ; ScrCnt = 7 - ECX + 1
    ;
        sub     ecx,7
        neg     ecx
        inc     ecx
    ;
    ; Found a sufficiently big scroll rect
    ;
    ; EDX -> Scroll Rect
    ; AX = # of lines in the scrollrect
    ; CX = # of lines to be scrolled
    ; For the new smaller update rect at the end of the list,
    ;   Top = Top of old Update Rect + AX + # of lines to be scrolled
    ;   Bottom  = Bottom of old UpdateRect
    ; For the Scroll Rect
    ;   Bottom = Top of new update rect
    ;
        push    esi
        mov     esi,[esp+6*4]            ; ESI = ptr to ch Struct
    ;
    ; Add the smaller update rect at the end of the list
    ;
        push    ecx
        movzx   ecx,[esi.VDD_Mod_Count]
        inc     [esi.VDD_Mod_Count]         ; one more rect to update
        lea     esi,[esi.VDD_Mod_List][ecx*8]; ESI = end of list
        pop     ecx
    ;
    ; ESI -> New smaller update rect
    ;
        mov     [esi.R_Left],0
        mov     [esi.R_Right],80

        mov     bx,[edx.R_Botm]
        mov     [esi.R_Botm],bx          ; Set Bottom
    ;
    ; For the new UpdateRect,
    ;   Top = Top of old UpdateRect + ScrollRect lines + # of lines to be scrolled
    ;   Ensure Top < Bottom
        add     ax,[edx.R_Top]
        add     ax,cx

        cmp     ax,[esi.R_Botm]
        jb      SHORT CTS_00
        mov     ax,[esi.R_Botm]
        dec     ax
CTS_00:
        mov     [esi.R_Top],ax

IFDEF RECTDEBUG
        push    eax
        push    ebx
        mov     ax,[esi.R_Top]
        mov     bx,[esi.R_Botm]
Trace_Out "New UpdRect - Top=#AX,Bot=#BX"        
        pop     ebx
        pop     eax
ENDIF

    ;
    ; Set bottom of the Scroll Rect = Top of new update rect
    ;
        mov     bx,[esi.R_Top]
        mov     [edx.R_Botm],bx         
        pop     esi
    ;
    ; Save old Update rect values in registers
    ;
        mov     ax,[edx.R_Right]
        mov     bx,[edx.R_Botm]
        mov     si,[edx.R_Left]
        mov     di,[edx.R_Top]
IFDEF RECTDEBUG
Trace_Out "ScrollRect - Top=#DI,Bot=#BX,Count=#CX"
ENDIF
    ;
    ; Set parameters for a scroll rect.
    ;
        mov     [edx.ScrCnt],cl
        mov     [edx.ScrFlgs],Scr_M_Scroll+Scr_M_FullWid+Scr_M_Up
        mov     [edx.ScrRgt],al
        mov     [edx.ScrBot],bl
        xchg    si,ax
        xchg    di,bx
        mov     [edx.ScrLft],al
        mov     [edx.ScrTop],bl
        mov     [edx.ScrFch],' '
        mov     [edx.ScrFatt],07h

        jmp     SHORT CTS_Exit

CTS_NextScroll:
        pop     esi
        pop     ecx
        dec     ecx
        jnz     CompareLoop
                
CTS_Exit:
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        ret

EndProc VDD_Mod_Text

VxD_CODE_ENDS
	END


