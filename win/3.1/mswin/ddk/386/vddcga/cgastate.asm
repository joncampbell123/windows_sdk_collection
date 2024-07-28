       title   VDD - Virtual Display Device for CGA version 3.00
;******************************************************************************
;
;VDD - Virtual Display Device for CGA and Compaq Plasma Display
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
;	The 640 x 400 resolution of the Compaq Plasma Display is tracked
;	in the VDD and the Grabber as mode 7.  It is important to note that
;	this is NOT Monochrome mode 7.	The number 7 was picked to minimize
;	the lengths of the tables found at the beginning of this module.
;
;******************************************************************************

	.386p

	INCLUDE VMM.INC
	INCLUDE VDD.INC
	INCLUDE CGA.INC
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
;
;Only three rectangles required for CGA text modes
;
Txt_Rect2_Off	dd	160*12
Txt_Rect1_Off	dd	160*3
Txt_Rect0_Off	dd	0

; Table of page masks for pages that VMDOSAPP wants to know about
PUBLIC	VDD_Mod_MskTab
VDD_Mod_MskTab	LABEL DWORD
	DD	000000001h		; mode 0, 40x25 alpha, 2 bytes/char
	DD	000000001h		; mode 1, 40x25 alpha, 2 bytes/char
	DD	000000001h		; mode 2, 80x25 alpha, 2 bytes/char
	DD	000000001h		; mode 3, 80x25 alpha, 2 bytes/char
	DD	00000000Fh		; mode 4, 320x200x2 graphics
	DD	00000000Fh		; mode 5, 320x200x2 graphics
	DD	00000000Fh		; mode 6, 640x200x1 graphics
	DD	0000000FFh		; mode 7, 640x400x1 graphics (ATT&IDC)

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
;USES:	Flags, EAX, ECX, EDX, ESI
;
BeginProc VDD_State_VMCreate, PUBLIC

;*******
; Initialize CGA state structure with values from System VM
;
	VMMCall Test_Sys_VM_Handle
	jnz	SHORT copy_sys_state            ; Not Sys VM

        mov     dx,13C6h
        in      al,dx
        mov     [edi.VDD_Stt.I_MMode],al
                
        mov     dx,23C6h
        in      al,dx
        mov     [edi.VDD_Stt.I_EMSel],al

        mov     dx,27C6h
        in      al,dx
        mov     [edi.VDD_Stt.I_SBlank],al

        mov     dx,2bC6h
        in      al,dx
        mov     [edi.VDD_Stt.I_UCtrl],al

        jmp     SHORT skip_sys_copy

copy_sys_state:
	push	ebx
	VMMCall Get_Sys_VM_Handle
	mov	esi,ebx
	pop	ebx
	add	esi,[VDD_CB_Off]		; ESI = SYS VM VDD ptr

	push	esi
	push	edi
	lea	edi,[edi.VDD_Stt]
.ERRNZ	CGA_State_Struc MOD 4
	mov	ecx,SIZE CGA_State_Struc/4
	push	edi
	push	ecx
	lea	esi,[esi.VDD_Stt]		; current state of system VM
        cld
	rep movsd				;   is copied to new VM state
	pop	ecx
	pop	esi
	pop	edi
	push	edi

; For a PVC adapter, we need to clear the PVC mode bit so that the new
;   VM starts up in PVC mode, even if Windows is not in PVC mode
	test	[vgVDD.Vid_Flags],fVid_PVC
	jz	SHORT VSVMC_NotPVC
	and	[esi.V_Mode3],0FEh
VSVMC_NotPVC:

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

; Assume geting grabbed video state(CF = 0)
	lea	edx,[edi.VDD_SttCopy]		    ; EDX = grabbed ctlr state
	mov	ecx,esi
	lea	esi,[edi.VDD_CPg]
	mov	al,[esi.VPH_Mode]
	jnc	SHORT VSGS_0

; Get current VM video state
	call	VDD_Get_Mode			; AL = current mode
	lea	esi,[edi.VDD_Pg]		; ESI = VPH struc ptr
	lea	edx,[edi.VDD_Stt]		; EDX = ctlr state
	call	VDD_Scrn_Size			; Set Rows, Cols, PgSz
VSGS_0:
	mov	[ecx.VDA_CGA_Mode],al		; Set current mode
	mov	al,[esi.VPH_Rows]
	mov	[ecx.VDA_CGA_Rows],al		; Set rows/page(text mode)
	mov	al,[edx.V_Colr] 		; AL = color select
	and	al,7Fh
	test	[vgVDD.Vid_Flags],fVid_PVC
	jz	short VSS_0
	mov	ah,[edx.V_Mode3]		; bit 0 = not inv. vid flag
	sal	al,1				; shift not LSB ah into al
	ror	ah,1
	cmc
	rcr	al,1
VSS_0:
	mov	[ecx.VDA_CGA_Colr],al		; Set current color state
	mov	esi,ecx

; Note that grab ignores these
	call	VDD_Get_Posn			; Get position
	mov	[esi.VDA_CGA_CurX],cx		; Set cursor X position
	mov	[esi.VDA_CGA_CurY],ax		; Set cursor Y position

	movzx	eax,[edx.V_CStart]		; AX = cursor start scan line
	movzx	ecx,[edx.V_CEnd]		; AX = cursor end scan line
	test	[edi.VDD_Flags],fVDD_Hide	; Q: cursor hidden?
	jz	short VSGS_ShowCurs		;   N: return valid cursor
	mov	eax,1    			;   Y: return no cursor
	mov	ecx,0
VSGS_ShowCurs:
	mov	[esi.VDA_CGA_CurBeg],ax 	; Set start scan line
	mov	[esi.VDA_CGA_CurEnd],cx 	; Set end scan line
	mov	al,[edx.V_VDisp]		; Text lines displayed
	mov	[esi.VDA_CGA_Rows],al		; (normally 25)
	mov	al,[edx.V_MaxScan]		; Character height (-1)
	mov	[esi.VDA_CGA_CharHgt],al	; (normally 7 or 13)
; Return the screen ON/OFF flag
	and	[esi.VDA_CGA_Flags],NOT fVDA_V_ScOff
        test    [edi.VDD_Flags],fVDD_MInit          ; Q: VMInit done yet?
        jnz     SHORT VSS_MainMemCheck              ;   Y: continue
	or	[esi.VDA_CGA_Flags],fVDA_V_ScOff    ;	N: Set screen off flag
        jmp     SHORT VSS_1
VSS_MainMemCheck:        
        test    [edi.VDD_EFlags],fVDE_NoMain        ; Q: No main mem?
        jz      SHORT VSS_1                         ;   N: continue
	or	[esi.VDA_CGA_Flags],fVDA_V_ScOff    ;	Y: Set screen off flag
VSS_1:
; Return the horizontal cursor tracking flag
	and	[esi.VDA_CGA_Flags],NOT fVDA_V_HCurTrk
	test	[edi.VDD_Flags],fVDD_HCurTrk	    ; Q: HCurTrk ON?
	jz	SHORT VSS_2			    ;	N: proceed
	or	[esi.VDA_CGA_Flags],fVDA_V_HCurTrk  ;	Y: Set HCurTrk flag
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
;       Zero flag set if some ctrl change 
;       Zero flag reset if no ctrl change or Video is disabled
;USES:	Flags, EAX
;
BeginProc VDD_State_CtlrChgs, PUBLIC

Assert_VDD_ptrs ebx,edi
	cmp	[vgVDD.Vid_Type],VID_Type_IDC	; Q: Is it IDC
	jne	SHORT VDD_CC_CGA		;   N: Just CGA
	mov	al,[edi.VDD_Stt.I_MMode]	; Get master mode
	and	al,fMModExtBl+fMModIntBl	; keep blanking bits
	cmp	al,fMModExtBl+fMModIntBl	; Q: All screens blanked?
	je	SHORT VDD_CC_No_Chg		;   Y: Delay reporting change
	test	[edi.VDD_Stt.I_EMSel],fEMSelFont ; Q: font ram selected?
	jnz	SHORT VDD_CC_No_Chg		;   Y: Delay reporting change
VDD_CC_CGA:
	mov	al,[edi.VDD_Stt.V_Mode] 	; Current Mode
	test	al,fModVidEna			; Q: Video Enabled?
	jz	SHORT VDD_CC_No_Chg		;   N: Delay reporting change
	mov	ah,[edi.VDD_SttCopy.V_Mode]	; Saved Mode
	and	ax,CGA_Mod_Mask+(CGA_Mod_Mask shl 8); Select bits
	cmp	ah,al				; Q: (Significant) Mode change?
	jne	SHORT VDD_CC_Chg_Ctl		;   Y: return controller changed

	mov	al,[edi.VDD_Stt.V_Mode2]	; Q: Current Mode = saved mode?
	cmp	al,[edi.VDD_SttCopy.V_Mode2]
	jne	SHORT VDD_CC_Chg_Ctl		;   Y: return controller changed

; Check for change in display offset
	mov	ax,word ptr [edi.VDD_Stt.V_AddrH]
	cmp	ax,word ptr [edi.VDD_SttCopy.V_AddrH] ; Q: Offset change?
	jne	SHORT VDD_CC_Chg_Ctl		;   Y: return controller changed
; *** IDC ***
	mov	al,[edi.VDD_Stt.V_VDisp]	; Lines displayed
	cmp	al,[edi.VDD_SttCopy.V_VDisp]	; Q: Unchanged?
	jne	SHORT VDD_CC_Chg_Ctl		;   N: report it
; *** END ***
	;	controller mode and origin not changed
	call	VDD_Chk_Cursor			;
	jne	SHORT VDD_CC_Chg_Curs		; Y: cursor changed

        mov     eax,1
        or      eax,eax
        ret
        
VDD_CC_No_Chg:                                  
	xor	eax,eax 			; set ZF
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
; VDD_State_MemChgs
;
;DESCRIPTION:
;	This routine builds a state change structure at ESI. 
;	(A single change at 0,0 is stored as 0,0,1,1)
;
;ENTRY: EBX = VM_Handle
;	EDI = VDD CB ptr
;	ESI = Place to build change structure
;
;EXIT:	Memory change structure possibly modified to reflect changes
;
;USES:	Flags, EAX, ECX, EDX
;
;ASSUMES:
;	Message, except for controller and cursor flag bits is zero
;	Mod_List will not be looked at unless fVDD_M_VRAM flag is set
;	any change box (scroll or modified) is specified by the row
;	and col. OUTSIDE the box. This is the way VMDOSAPP was spec'd
;
BeginProc VDD_State_MemChgs, PUBLIC

Assert_VDD_ptrs ebx,edi
	push	ebx
	call	VDD_Mem_UpdOff			; Update display offset

;*******
; Initialize Mod structure: no mem changes, length = 4 bytes
	mov	[esi.VDD_Mod_count], 0
	mov	ecx,4

	call	VDD_Get_Mode			; EAX = video mode
        push    esi
	lea	esi,[edi.VDD_Pg]		; ESI = VPH struc ptr
	call	VDD_Scrn_Size			; Get screen size parameters
        pop     esi
	cmp	al,3				; Q: Text mode?
	ja	SHORT VMC_CGAGraphics		;   N: Do CGA graphics
	mov	ecx,esi
	lea	esi,[edi.VDD_Pg]
	call	VDD_Mod_Text			; Build text changes, if any
VMC_Exit:
	pop	ebx
	ret
VMC_NoCGAChg:
	pop	ecx
	jmp	VMC_Exit

;*******
; Build CGA graphics mode change structure
VMC_CGAGraphics:
	push	ecx                     ; save size of modf structure
        mov     edx,eax                 ; EDX = Mode
    ;
    ; Set CL = memory mask, 16K for mode 4 and 6, 32K for mode 7
    ;
	mov	ecx,0Fh                 ; Assume Mode 4/6
	cmp	dl,7                    ; Q: Assumption correct?
	jne	SHORT VMCGA_0           ;   Y: 
	mov	cl,0FFh                 ;   N: 32K for Mode 7

VMCGA_0:
	cmp	[edi.VDD_LastMode],dl	; Q: Did mode change?
	jne	SHORT VMC_GotModfPages	;   Y: Return all pages modified
    ;
    ; Get dirty pages
    ;
	call	VDD_Mem_ModPag
	and	eax,ecx
	jz	SHORT VMC_NoCGAChg
	or	[edi.VDD_CPg.VPH_PgAccMsk],eax
	push	eax                     ; Modf Page Mask
	mov	al,dl
	call	VDD_Mem_ACopy
	pop	ecx                     ; ECX = Modf Page Mask
VMC_GotModfPages:
    ;
    ; Mode 4,6 is 2 way interleaved.
    ; Modes 4, 5, 6: The even scans run from B8000 -> BA000 which is two pages
    ;	             The odd scans run from BA000 -> BC000 which is two pages
    ; Mode 7 is 4 way interleaved,
    ;   B8000 - Lines 0, 4, 8, ...
    ;   BA000 - Lines 1, 5, 9, ...
    ;   BC000 - Lines 2, 6, 10, ...
    ;   BE000 - Lines 3, 7, 11, ...
    ;
    ;
        cmp     dl,7                    ; Q: 4 way Interleave?
        je      SHORT VMC_4WayInterleave;   Y:
VMC_2WayInterleave:                     ;   N: 
	mov	al,cl
	mov	ah,cl
	and	al,03h	                ; AH = change mask for Pages 0,1
	shr	ah,2
	or	al,ah                   ; Combine odd/even line masks
        jmp     SHORT VMC_GotCombinedPgMask        
VMC_4WayInterleave:
        mov     ah,cl
        mov     al,cl                   
	and	al,03h	                ; AH = change mask for Pages 0,1
        shr     ah,2
        and     ah,03h
        or      al,ah                   ; OR changes to Pages 2,3

        mov     ah,cl
        shr     ah,4
        and     ah,03h  
        or      al,ah                   ; OR changes to Pages 4,5

        mov     ah,cl
        shr     ah,6
        or      al,ah                   ; OR changes to Pages 6,7
        
VMC_GotCombinedPgMask:
    ;
    ; AL = combined page mask for changed pages.
    ;
	or	[esi.VDD_Mod_Flag],fVDD_M_Type_Rect+fVDD_M_VRAM
	mov	[esi.VDD_Mod_Count],1
        
        mov     ebx,OFFSET32 VMC_Table_Rect
        cmp     dl,4
        je      SHORT VMC_SetRects
        add     ebx,SIZE Rect * 3       ; skip 3 rects for mode 6 
        cmp     dl,6
        je      SHORT VMC_SetRects
        add     ebx,SIZE Rect * 3       ; skip 3 more rects for mode 7 
VMC_SetRects:
        and     eax,03h                 ; atmost 2 pages
        dec     eax                     ; convert to 0 based index 
        shl     eax,3                   ; 8 bytes per RECT
        add     ebx,eax        
        mov     ax,[ebx.R_Left]
	mov	[esi.VDD_Mod_List.R_Left],ax
        mov     ax,[ebx.R_Top]
	mov	[esi.VDD_Mod_List.R_Top],ax
        mov     ax,[ebx.R_Right]
	mov	[esi.VDD_Mod_List.R_Right],ax
        mov     ax,[ebx.R_Botm]
	mov	[esi.VDD_Mod_List.R_Botm],ax
                        
        pop     ecx
        add     ecx,SIZE Rect
	jmp	VMC_Exit

    ;
    ; Each entry is a rect = <L,T,R,B>
    ; The first three entries are used for Mode 4, next three for Mode 6
    ; and the last three for Mode 7.
    ; Once the dirty page mask is combined after accounting for interleaving,
    ; there can be only 2 pages in all the different modes 4,6 and 7.
    ; Use 1st Rect for PgMask 001B - first  half of screen
    ; Use 2nd Rect for PgMask 010B - second half of screen
    ; Use 3rd Rect for PgMask 011B - full screen - first and second rects combined
    ;

VMC_Table_Rect  Rect    <0,0,  320,104> ; Mode 4 - 1st Rect
                Rect    <0,104,320,200> ; Mode 4 - 2nd Rect
                Rect    <0,0,  320,200> ; Mode 4 - 3rd Rect-whole screen

                Rect    <0,0,  640,104> ; Mode 6 - 1st Rect
                Rect    <0,104,640,200> ; Mode 6 - 2nd Rect
                Rect    <0,0,  640,200> ; Mode 6 - 3rd Rect-whole screen

                Rect    <0,0,  640,206> ; Mode 7 - 1st Rect
                Rect    <0,206,640,400> ; Mode 7 - 2nd Rect
                Rect    <0,0,  640,400> ; Mode 7 - 3rd Rect-whole screen


EndProc VDD_State_MemChgs

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
	jnz	SHORT VSQ_Change		; CF=0 means no change
	call	VDD_Chk_Cursor			; ZF = 0 if cursor changed
	jnz	SHORT VSQ_Change
	test	[edi.VDD_LastMode],-1		; Q: Change in display mode?
	js	SHORT VSQ_Change		;   Y: Report controller change

; Finally, check to see if screen was turned off or on
	test	[ebx.VDD_Stt.V_Mode],fModVidEna ; Q: Video on?
	jnz	SHORT VSQ_1
	test	[ebx.VDD_SttCopy.V_Mode],fModVidEna ; Q: Screen off change?
	jnz	SHORT VSQ_Change		;	Y: return ctlr chged
	jmp	SHORT VSQ_Exit
VSQ_1:
	test	[ebx.VDD_SttCopy.V_Mode],fModVidEna ; Q: Screen off change?
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
;VDD_Mod_Text
;
;DESCRIPTION:
;	Modifies change structure to indicate rectangles of changed text.
;	Assumes the VDD_Scrn_Size has already been called for current mode,
;	setting VDD_Pg.VPH_LLen and VPH_PgSiz.
;
;Pseudo Code:
;
; Get Modf pages - VDD_Mem_ModPag
; If (No Modf pages || modf page not displayed)
;    return;
; else if (no copy mem alloc'ed)
;    return entire screen modf.
;
; ECX=PgSz
; EDX = Ch Struc ptr
; Call VMT_Comp
; return;
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

	call	VDD_Mem_ModPag		 
	and	eax,0Fh 		 ; Q: Displayed pages dirty?
	jz	SHORT VMT_Exit		 ;   N: No changes
	call	Setup_Start_Count
	jz	SHORT VMT_Exit		 ;   N: No changes
; EBP = offset of first mod'd page from first displayed page
; ECX = bytes of displayed memory left to end from first mod'd page
;
	test	[edi.VDD_CPg.VPH_MState.VDA_Mem_Addr],-1; Q: Copy mem alloc'd?
	jz	SHORT VMT_AllChgd		;   N: Report all changed
	mov	edx,[esp]		 ; EDX = chg struc ptr
	call	VMT_Comp		 ; Build change structure entries
        call    VMT_Check_Scroll	 ; Check for scrolling

VMT_Exit:
	pop	ecx
	pop	ebp
	pop	ebx
	test	[ecx.VDD_Mod_Count],-1
	jz	SHORT VMT_NoChange
	ret

VMT_NoChange:
	xor	eax,eax
	mov	[edi.VDD_AccPages],eax
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
	shr	eax,1			 ; Text is 2 bytes per column
	mov	[edx.VDD_Mod_List.R_Right],ax
	movzx	eax,[esi.VPH_Rows]
	mov	[edx.VDD_Mod_List.R_Botm],ax
	jmp	VMT_Exit


;******************************************************************************
;
; Setup_Start_Count - Depending on the dirty pages mask in EAX
;			 setup EBP = Start of search offset, ECX = search len
;
; For 25 line mode:
;   Offset = Offset within the displayed page and PgSz = VPH_PgSz. Both are
;	calculated by VDD_Scrn_Size.
;   Note that discontiguous changes are assumed contiguous (i.e. the
;	in between page is included in the compare).
;
;    Mask | EBP,start |  ECX,count  (if negative, no changes)
;   ------|-----------|-------
;   0001B | Offset    | MIN(PgSz,(4k-Offset))
;   0010B | 4K	      | MIN((PgSz+Offset-4K),(8k-4k))
;   0011B | Offset    | MIN(PgSz,(8K-Offset))
;   0100B | 8K	      | MIN(PgSz+Offset-8K,(12k-8k))
;   0101B | Offset    | MIN(PgSz,(12k-Offset)
;   0110B | 4K	      | MIN(PgSz+Offset-4K,(12k-4k))
;   0111B | Offset    | MIN(PgSz,(12k-Offset)
;   1000B | 12k       | MIN(PgSz+Offset-12K,(16k-12k))
;   1001B | Offset    | MIN(PgSz,(16k-Offset))
;   1010B | 4K	      | MIN((PgSz+Offset-4K),(16k-4k))
;   1011B | Offset    | MIN(PgSz,(16K-Offset))
;   1100B | 8K	      | MIN((PgSz+Offset-8K),(16k-8k))
;   1101B | Offset    | MIN(PgSz,(16k-Offset)
;   1110B | 4K	      | MIN((PgSz+Offset-4K),(16k-4k))
;   1111B | Offset    | MIN(PgSz,(16k-Offset)
;
; ENTRY: EAX = mask for dirty pages
;
; EXIT: Set ZF if displayed pages not mod'd
;	EBP = offset in the copy mem of the first possible mod'd byte
;	    =	offset of first mod'd char from first displayed page
;	ECX = count to end of mod'd displayed memory from first mod'd byte
;
IFDEF	DEBUG
PUBLIC Setup_Start_Count
ENDIF
Setup_Start_Count:
	and	eax,0Fh
	jz	SHORT SSC_Exit
	mov	ebp,[esi.VPH_MState.VDA_Mem_DPagOff]
	mov	ecx,[esi.VPH_PgSz]
	bsr	edx,eax
	inc	edx			    ; EDX = most sig bit number + 1
	shl	edx,12			    ; EDX = 4k * (most sig bit + 1)
	bsf	eax,eax 		    ; EAX = least sig bit number
	or	eax,eax 		    ; Q: Bit 0 set?
	jnz	SHORT SSC_NotFirst	    ;	N: 1st displayed page not mod'd

; Modification is contiguous from first displayed page
	sub	edx,ebp 		    ; EDX = last mod'd page - offset
	cmp	ecx,edx 		    ; ECX = MIN(EDX,ECX)
	jb	SHORT SSC_Exit
	mov	ecx,edx
SSC_Exit:
	ret

; First displayed page not modified,
SSC_NotFirst:
	shl	eax,12
	xchg	ebp,eax 		    ; EBP = 4k * (least sig bit - 1)
	add	ecx,eax 		    ; ECX = PgSz+Offset
	sub	ecx,ebp 		    ; ECX = PgSz+Offset-start
	jb	SHORT SSC_NoChanges	    ; Changes not displayed
	sub	edx,ebp 		    ; EDX = MSB*4k-(LSB-1)*4k
	cmp	ecx,edx 		    ; ECX = MIN(EDX,ECX)
	jb	SHORT SSC_Exit
	mov	ecx,edx
	ret
SSC_NoChanges:
	xor	eax,eax
	ret

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
;	ECX = byte count in displayed page
;
;EXIT:	EBX = last change buffer entry + 4 pointer
;
;USES:	Flags, EAX, EDX
;
VMT_Comp:
	push	edi
	push	esi
	push	ecx
	mov	ebx,edi 		; EBX = VDD CB ptr

;*******
; Search from end to first modified display memory
	movzx	esi,[ebx.VDD_Pg.VPH_MState.VDA_Mem_DispPag]
	movzx	esi,[ebx.VDD_Pg.VPH_MState.VDA_Mem_PgMap][esi]
	shl	esi,12
;	add	esi,[ebx.VDD_Pg.VPH_MState.VDA_Mem_DPagOff]
	add	esi,[ebx.VDD_Pg.VPH_MState.VDA_Mem_Addr]
	add	esi,ebp 		; ESI = address of first possible diff
	add	esi,ecx
	sub	esi,4			; ESI = last dword disp'd mem

	mov	edi,[ebx.VDD_CPg.VPH_MState.VDA_Mem_Addr]
	add	edi,ebp 		; ESI = address of first possible diff
	add	edi,ecx
	sub	edi,4			; EDI = last dword disp'd mem

	shr	ecx,2
	std
	repe cmpsd			; ECX = offset of last chg
	jz	SHORT VMTC_Done 		;   Exit if no changes
	shl	ecx,2
	add	ecx,ebp
	sub	ecx,[ebx.VDD_Pg.VPH_MState.VDA_Mem_DPagOff]
;
; ECX = last change offset from start of displayed pages
;
; The VMTC_CheckRect routine stores the rect in the change structure
;
	mov	eax,[Txt_Rect2_Off]		; EAX = Rect offset from ESI,EDI
	call	VMTC_CheckRect			; Check rectangle for changes
	jz	SHORT VMTC_Done                       ; atmost one rect-can't combine

	mov	eax,[Txt_Rect1_Off]		; EAX = Rect offset from ESI,EDI
	call	VMTC_CheckRect			; Check rectangle for changes
	jz	SHORT VMTC_Combine              ; see if rects can be combined

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
;	[ECX]
;
;ENTRY: EDX = change structure pointer
;	EBX = VDD CB ptr
;	ECX = offset from display start to last change
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
	add	edi,[ebx.VDD_CPg.VPH_MState.VDA_Mem_DPagOff]
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
	add	edi,[ebx.VDD_CPg.VPH_MState.VDA_Mem_DPagOff]

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
        cmp     [ebx.VDD_Pg.VPH_Rows],25
        jbe     SHORT VCS_00
        mov     ecx,43/3                ; use 43/3 for 52 line mode also
VCS_00:
        mov     ax,[edx.R_Botm]
        sub     ax,[edx.R_Top]
;        inc     ax
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
	mov	ecx,(SIZE CGA_State_Struc)/4	; ECX = size of state
.ERRNZ size CGA_State_Struc mod 4
	cld
	rep movsd
	pop	edi
	pop	esi
        push    eax
        call    VDD_Get_Mode                        ; EAX = Mode
        pop     eax
	mov	cl,[edi.VDD_Mode]
	mov	[edi.VDD_LastMode],cl
	and	[edi.VDD_Flags],NOT fVDD_HCurTrk; Cancel horiz cursor track
	popfd
	ret
EndProc VDD_State_Update


;******************************************************************************
;VDD_Scrn_Size	    Text size calculations
;
;DESCRIPTION:
;	Set value for VPH_PgSz, VPH_LLen and VPH_Rows.	Although VPH_LLen
;	is a word value, the below code makes use of the fact that it
;	never greater than 255.
;
;ENTRY: EAX = mode(text)
;	ESI = VDD CB VPH structure ptr (either main mem or copy mem)
;
;EXIT:	none
;
;USES:	Flags
;
BeginProc VDD_Scrn_Size, PUBLIC

	mov	[esi.VPH_LLen],40*2		    ; 80 byte lines
	cmp	al,3                                ; Q: Graphics mode?
	ja	SHORT VSS_Grfx                      ;   Y:

	mov	[esi.VPH_Rows],25		    
	mov	[esi.VPH_PgSz],25*(40*2)            ; Assume 25 rows X 40 cols
	cmp	al,1                                ; Q: Assumption correct?
	jbe	SHORT VSS_Ex                        ;   Y: 
; mode 2 or 3
	mov	[esi.VPH_LLen],80*2		    ; 160 byte lines
	mov	[esi.VPH_PgSz],25*(80*2)            ; 25 rows X 80 cols
VSS_Ex:
	ret

; mode 4, 5 or 6
VSS_Grfx:
	mov	[esi.VPH_Rows],0		    ; no text rows
	mov	[esi.VPH_PgSz],8192+(80*100)	    ; odd scans offset by 8192
	cmp	al,6
	jbe	SHORT VSS_Ex
; mode "7", 400 line mode. Still 80 byte lines. rest changes
VSS_400:
	mov	[esi.VPH_PgSz],400*80		    ; 400 lines * 80 bytes/line
        ret

EndProc VDD_Scrn_Size


;******************************************************************************
;VDD_Get_Mode	    Determine current mode
;
;DESCRIPTION:
;	Determine current mode from controller state
;
;ENTRY: EDI = CB ptr
;
;EXIT:	EAX (AL) = mode
;
;USES:	EDX, Flags
;
;CALLS: none
;
;ASSUMES:
;	160 x 100 mode is not programmed by VM
;
;==============================================================================
VDD_Mode_Table:
	db	CGA_Mod_HiBW AND CGA_Mod_Mask	 ; 6 640x200 B & W Graphics
	db	CGA_Mod_LoBW AND CGA_Mod_Mask	 ; 5 320x200 B & W Graphics
	db	CGA_Mod_LoClr AND CGA_Mod_Mask	 ; 4 320x200 Color Graphics
	db	CGA_Mod_80Clr AND CGA_Mod_Mask	 ; 3 80x25 alpha color
	db	CGA_Mod_80BW AND CGA_Mod_Mask	 ; 2 80x25 alpha B & W
	db	CGA_Mod_40Clr AND CGA_Mod_Mask	 ; 1 40x25 alpha color
	db	CGA_Mod_40BW AND CGA_Mod_Mask	 ; 0 40x25 alpha B & W
VDD_Mode_Max	equ $-VDD_Mode_Table


BeginProc VDD_Get_Mode

	push	ecx
	push	edi
	lea	edx,[edi.VDD_Stt]		; EDX = ctlr state
	movzx	eax,[edx.V_Mode]		; mode in eax
	and	al,CGA_Mod_Mask 		; turn off don't care bits
	mov	edi,OFFSET32 VDD_Mode_Table	; ESI = table of valid modes
	mov	ecx,VDD_Mode_Max		; ECX = count of valid modes
	cld
	repne scasb
	jne	short VDD_G_Not
	cmp	cl,6				; Q: Mode 6 or "Mode 7 (640x400)"
	jne	short VDD_G_Good		;   N: Ok
	cmp	[vgVDD.Vid_Type],VID_Type_IDC	; Q: Is it IDC?
	jz	short VDD_G_IDC
	jb	short VDD_G_Good
	test	[edx.V_Mode2],1
	jz	SHORT VDD_G_Good
	jmp	SHORT VDD_G_Hires
VDD_G_IDC:
	test	[edx.I_EMsel],fEMSelEMod	; Y: Which One? Q: Ext Mode?
	jz	short VDD_G_Good		;   N: Mode 6
VDD_G_Hires:
	mov	cl,7				;   Y: Mode "7"
VDD_G_Good:
	pop	edi
	mov	eax,ecx 			; EAX = mode
        mov     [edi.VDD_Mode],al
	pop	ecx
	ret
VDD_G_Not:
	mov	al,3				; If invalid mode, return 3
	pop	edi
	pop	ecx
	ret
EndProc VDD_Get_Mode

;******************************************************************************
;VDD_Get_Posn
;
;DESCRIPTION:
;	Determine current position from controller state
;
;ENTRY: EDX = controller state
;
;EXIT:	EAX = Current Line for cursor
;	ECX = Current Offset within line
;
;USES:	Flags, EAX, ECX
;
;==============================================================================
BeginProc VDD_Get_Posn

	push	edx
	movzx	ecx,[edx].V_AddrL
	mov	ch,[edx].V_AddrH
	movzx	eax,[edx].V_CAddrL
	mov	ah,[edx].V_CAddrH
	sub	eax,ecx 			; AX = byte offset into cur page
	movzx	ecx, [edx].V_HDisp
	xor	edx, edx			; for div
	div	cx				; AX = line number
	mov	ecx, edx			; CX = offset
	pop	edx
	ret
EndProc VDD_Get_Posn

VxD_CODE_ENDS
	END


