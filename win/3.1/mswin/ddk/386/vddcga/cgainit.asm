       title   VDD - Virtual Display Device for CGA version 3.00
;******************************************************************************
;
;VDD - Virtual Display Device
;
;   Author: MDW, PAB
;
;   (C) Copyright MICROSOFT Corp. 1986 - 1989
;
;
;DESCRIPTION:
;
;******************************************************************************

	.386p

	INCLUDE VMM.INC
	INCLUDE CGA.INC
	INCLUDE VDD.INC
	INCLUDE DEBUG.INC

;******************************************************************************

;******************************************************************************
; EXTRN routines
;
VxD_CODE_SEG
	EXTRN	VDD_VMSetFocus:NEAR
	EXTRN	VDD_VMCreate:NEAR
	EXTRN	@VDD_VMIdle:NEAR
	EXTRN	VDD_PFault:NEAR
	EXTRN	VDD_Int_10:NEAR
VxD_CODE_ENDS

;******************************************************************************
; EXTRN data
;
VxD_DATA_SEG
	EXTRN	VDD_I10_OldVector:DWORD
	EXTRN	vgVDD:BYTE
	EXTRN	CGANoSnow:DWORD
	EXTRN	VDD_CB_Off:DWORD
	EXTRN	VDD_PhysB8000:DWORD
	EXTRN	VDD_VMIdle_Chain:DWORD
	EXTRN	VDD_Rest_Event_Handle:DWORD
	EXTRN	VDD_Focus_VM:DWORD
	EXTRN	VDD_Win_Update_Time:DWORD
VxD_DATA_ENDS

VxD_IDATA_SEG
	EXTRN	VDD_Str_BadDevice:BYTE
	EXTRN	VDD_Str_CheckVidPgs:BYTE
	EXTRN	VDD_Time_Ini:BYTE
	EXTRN	VDD_NoSnow_Ini:BYTE

Inst_1 InstDataStruc <,,CGA_I_Addr1,CGA_I_Len1,ALWAYS_Field> ; Video stuff

VMPagesBuf	dd  9 dup (?)			       ; Buf for reserving pgs

VxD_IDATA_ENDS

VxD_ICODE_SEG
	EXTRN	VDD_IO_Init:NEAR

;******************************************************************************
;
;   NAME:
;	VDD_Sys_Critical_Init
;
;   DESCRIPTION:
;
;
;   ENTRY:
;	EBX = Sys VM handle
;
;   EXIT:
;       CF set if error
;
;   USES:
;	Flags, EAX, ECX, EDX, ESI, EDI
;
;------------------------------------------------------------------------------

BeginProc VDD_Sys_Critical_Init,PUBLIC

;*******
;Allocate part of VM control block for VDD usage
;
	VMMCall _Allocate_Device_CB_Area, <<SIZE CB_VD_Struc>, 0>
	test	eax, eax
	jnz	SHORT VDD_SI_Got_CB_Area
	Debug_Out "VDD ERROR:  Could not allocate control block area"
	jmp	VDD_SI_ErrBad
VDD_SI_Got_CB_Area:
	mov	[VDD_CB_Off], eax
	mov	edi,ebx
	add	edi,eax

;*******
; Reserve and hook pages B8-BF
;
	xor	eax,eax
	VMMCall _Get_Device_V86_Pages_Array,<eax,<OFFSET32 VMPagesBuf>,eax>
	mov	eax,[VMPagesBuf+((0A0h/32)*4)]
	test	eax,0FF000000h
	jz	SHORT VDD_SI_PagesFree
	Debug_Out "VDD ERROR:  Video pages already allocated: #EAX"
	jmp	VDD_SI_ErrPgs
VDD_SI_PagesFree:
	xor	eax,eax
	VMMCall _Assign_Device_V86_Pages,<0B8h,8,eax,eax>
	or	eax,eax
	jnz	SHORT VDD_SI_GotPages
	Debug_Out "VDD ERROR:  Could not allocate B8-BF video pages"
	jmp	SHORT VDD_SI_ErrPgs
VDD_SI_GotPages:
	mov	esi,OFFSET32 VDD_PFault
	mov	eax,0B8h
	mov	ecx,8
VDD_SI_NxtPg:
	VMMCall Hook_V86_Page
	inc	eax
	loopd	VDD_SI_NxtPg

;*******
; Set up physical addresses for the video memory
;
	VMMCall _MapPhysToLinear,<0B8000h,08000h,0>
	inc	eax
	jnz	SHORT VDD_SI_GotPhysAddr
Debug_Out "VDD: Cannot address video memory(B8000h-BFFFFh)"
	jmp	SHORT VDD_SI_ErrPgs
VDD_SI_GotPhysAddr:
	dec	eax
	mov	[VDD_PhysB8000],eax
	clc
	ret
VDD_SI_ErrPgs:
	Fatal_Error <OFFSET32 VDD_Str_CheckVidPgs>
	stc
	ret
VDD_SI_ErrBad:
	Fatal_Error <OFFSET32 VDD_Str_BadDevice>
	stc
	ret
EndProc VDD_Sys_Critical_Init


;******************************************************************************
;
;   VDD_Device_Init
;
;   DESCRIPTION:
;	Specify instance data, do any other initialization necessary
;
;   ENTRY:
;	EBX = Sys VM's handle
;
;   EXIT:
;	Carry clear if no error
;
;   USES:
;	Nothing
;
;   ASSUMES:
;	Interrupts have been (and still are) disabled.
;
;==============================================================================

BeginProc VDD_Device_Init,PUBLIC

	pushad
	mov	edi,ebx
	add	edi,[VDD_CB_Off]

;*******
;Initialize global data structures
;
	xor	eax,eax
	mov	[VDD_Rest_Event_Handle],eax

	mov	[VDD_Focus_VM],ebx	    ; System VM has focus and is
	mov	[vgVDD.Vid_VM_Handle],ebx   ;	running on hardware

	mov	[vgVDD.Vid_Flags],ax	    ; Init flags

	mov	eax,[ebx.CB_High_Linear]
	mov	eax,DWORD PTR 10h*4[eax]    ; Save INT 10 vector
	mov	[VDD_I10_OldVector],eax

;*******
;Initialize interrupt trapping
;
	mov	esi,OFFSET32 VDD_Int_10     ; ESI = address of handler
	mov	eax,10h
	VMMCall Hook_V86_Int_Chain	    ; Hook Int 10h

	mov	esi,OFFSET32 @VDD_VMIdle
	mov	eax,Release_Time_Slice
	VMMCall Hook_Device_Service
	mov	[VDD_VMIdle_Chain],esi

; Determine type of CGA
	mov	[vgVDD.Vid_Type],Vid_Type_CGA	;
	mov	dx,017C6h		; IDC Environment Register Port
	in	al,dx			; Read Register
	cmp	al,11100110b		; Q: IDC signature?
	jz	short VDD_I_IDC 	; Y: It is an IDC

; Check for existence, not wrapped, memory at BC000
	push	ebx
	push	ecx
	mov	ebx,[VDD_PhysB8000]
	mov	ecx,0BC000h-0B8000h
	add	ecx,ebx
	mov	eax,[ebx]
	push	eax
	mov	eax,[ecx]
	push	eax
	not	eax
	mov	[ecx],eax
	cmp	eax,[ecx]		; Q: Memory exist at BC000?
	jne	short VDD_I_CGAOnly	;   N: It must be normal CGA
	cmp	eax,[ebx]		; Q: Wrap to B8000?
	jne	short VDD_I_ATT 	;   N: Must be ATT card
	not	eax
	mov	[ecx],eax
	cmp	eax,[ecx]		; Q: Wrap to B8000?
	jz	short VDD_I_CGAOnly	;   Y: must be CGA card
VDD_I_ATT:				;   N: Must be ATT card
	mov	[vgVDD.Vid_Type],Vid_Type_ATT ; Must by ATT CGA

; The following test for the Olivetti Positive Video CRTC(PVC) was taken
;	from OLIBW.ASM windows 2.0 display driver
	mov	edx,03DFh	 ; begin elaborate check for PVC,
	in	al,dx		 ;   success noted ONLY if positive monitor
	IO_Delay
	in	al,dx		 ; these two 'ins' clear some bits
	IO_Delay
	cmp	al,0F0h 	 ; No positive monitor if not f0h
	jne	short VI_NotPVC
	xor	al,al		 ; Do the next check
	out	dx,al
	IO_Delay
	in	al,dx
	IO_Delay
	cmp	al,0F1h 	 ; No positive monitor if not f1h
	jne	short VI_NotPVC
	in	al,dx		 ; Last check
	IO_Delay
	cmp	al,0f0h 	 ; Positive monitor if f0h
	jne	short VI_NotPVC
	or	[vgVDD.Vid_Flags],fVid_PVC
VI_NotPVC:

VDD_I_CGAOnly:
	pop	eax
	mov	[ecx],eax
	pop	eax
	mov	[ebx],eax
	pop	ecx
	pop	ebx
	jmp	SHORT VDD_I_CGA
VDD_I_IDC:
	mov	[vgVDD.Vid_Type],Vid_Type_IDC	; Y: It is an IDC
;	For an IDC we will use some values already set by the user
;	for defaults
;	For Master Mode we determine if the Plasma or CRT is in use
;	as the Windows display
;	mov	dx,013C6h		; IDC Master Mode Register
;	in	al,dx			; read it
;	and	al,fMModDspInt		; Display select bit (1=Internal=Plasma)
;	or	CGA_Vid_Stt.I_MMode,al	; Text mode on this display
;	mov	dx,027C6h		; IDC Screen blank Register
;	in	al,dx			; read it
;	and	al,03fh 		; select time value only
;	mov	CGA_Vid_Stt.I_SBlank,al ; Set default value for each VM
;	mov	dx,023C6h		; Extended mode select register
;	in	al,dx			; read it
;	and	al,0F2h 		; Selected bits
;	or	CGA_Vid_Stt.I_EMSel,al	; Text mode
VDD_I_CGA:
	mov	[vgVDD.Vid_Mode],-1	

;*******
;Initialize I/O trapping - depends on Vid_Type
;
	call	VDD_IO_Init		    ; Initialize I/O port trapping

;*******
;Specify instanced RAM
;
	VMMCall _AddInstanceItem, <<OFFSET32 Inst_1>, 0>

;
; Get the Win.Ini entry for window update time
;
	mov	eax, VDD_Def_Update_Time    ; EAX = Default update time
	mov	ecx, 3			    ; 3 digits after decimal point
	xor	esi, esi		    ; Use [Win386] section
	mov	edi, OFFSET32 VDD_Time_Ini  ; Find this string
	VMMcall Get_Profile_Fixed_Point     ; EAX = User specified value
	test	eax, eax		    ; Q: Did bozo say "0"?
	jz	SHORT VDD_DI_Use_Default    ;	 Y: Ignore it
	cmp	eax, 10000		    ;	 N: Q: Longer than 10 sec?
	jb	SHORT VDD_DI_Set_Time	    ;	       N: Use this one
VDD_DI_Use_Default:			    ;	       Y: TOO LONG!
	mov	eax, VDD_Def_Update_Time    ; Use the default update time
VDD_DI_Set_Time:
	mov	[VDD_Win_Update_Time], eax  ; Save for time-out routine

	xor	eax,eax
	mov	edi, OFFSET32 VDD_NoSnow_Ini ; Find this string
	VMMcall Get_Profile_Boolean	    ; EAX = User specified value
	mov	[CGANoSnow],eax

;*******
;Initialize Sys VM control block
;
	call	VDD_VMCreate		    ; Init Sys VM's control block
	mov	edx, VDD_Device_ID	    ; Set focus to Sys VM
	call	VDD_VMSetFocus
	popad
	ret
EndProc VDD_Device_Init

VxD_ICODE_ENDS

	END
