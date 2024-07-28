       title   VDD - Virtual Display Device for HERC version 3.00
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
	INCLUDE HERC.INC
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
	EXTRN	VDD_CB_Off:DWORD
	EXTRN	VDD_PhysB0000:DWORD
	EXTRN	VDD_VMIdle_Chain:DWORD
	EXTRN	VDD_Rest_Event_Handle:DWORD
	EXTRN	VDD_Focus_VM:DWORD
	EXTRN	VDD_Win_Update_Time:DWORD
VxD_DATA_ENDS

VxD_IDATA_SEG
	EXTRN	VDD_Str_BadDevice:BYTE
	EXTRN	VDD_Str_CheckVidPgs:BYTE
	EXTRN	VDD_Time_Ini:BYTE

Inst_1 InstDataStruc <,,HERC_I_Addr1,HERC_I_Len1,ALWAYS_Field> ; Video stuff

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
; Reserve and hook pages B0-BF
;
	xor	eax,eax
	VMMCall _Get_Device_V86_Pages_Array,<eax,<OFFSET32 VMPagesBuf>,eax>
	mov	eax,[VMPagesBuf+((0A0h/32)*4)]
	test	eax,0FFFF0000h
	jz	SHORT VDD_SI_PagesFree
	Debug_Out "VDD ERROR:  Video pages already allocated: #EAX"
	jmp	VDD_SI_ErrPgs
VDD_SI_PagesFree:
	xor	eax,eax
	VMMCall _Assign_Device_V86_Pages,<0B0h,16,eax,eax>
	or	eax,eax
	jnz	SHORT VDD_SI_GotPages
	Debug_Out "VDD ERROR:  Could not allocate B0-BF video pages"
	jmp	SHORT VDD_SI_ErrPgs
VDD_SI_GotPages:
	mov	esi,OFFSET32 VDD_PFault
	mov	eax,0B0h
	mov	ecx,16
VDD_SI_NxtPg:
	VMMCall Hook_V86_Page
	inc	eax
	loopd	VDD_SI_NxtPg

;*******
; Set up physical addresses for the video memory
;
	VMMCall _MapPhysToLinear,<0B0000h,010000h,0>
	inc	eax
	jnz	SHORT VDD_SI_GotPhysAddr
Debug_Out "VDD: Cannot address video memory(B0000h-BFFFFh)"
	jmp	SHORT VDD_SI_ErrPgs
VDD_SI_GotPhysAddr:
	dec	eax
	mov	[VDD_PhysB0000],eax
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
;
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
;
;Initialize interrupt trapping
;
	mov	esi,OFFSET32 VDD_Int_10     ; ESI = address of handler
	mov	eax,10h
	VMMCall Hook_V86_Int_Chain	    ; Hook Int 10h

	mov	esi,OFFSET32 @VDD_VMIdle
	mov	eax,Release_Time_Slice
	VMMCall Hook_Device_Service
	mov	[VDD_VMIdle_Chain],esi
;
;Determine type of Herc adapter
;
	mov	[vgVDD.Vid_Type],Vid_Type_HERC102
	mov	[vgVDD.Vid_IndxMax],17

	mov	dx,pStatMono		; Status port
	in	al,dx			; test ID bits
	and	al,00110000B		; clear all but bits 4 and 5
	cmp	al,00010000B		; test for GB112
	jne	short VDD_NotHerc112	; Not 112, must be 102.

	mov	[vgVDD.Vid_Type],Vid_Type_HERC112
	mov	[vgVDD.Vid_IndxMax],22

VDD_NotHerc112:
	mov	[vgVDD.Vid_Mode],1	; -1=Unset, 0=640x200, 1=640x400.
;
;Initialize I/O trapping - depends on Vid_Type
;
	call	VDD_IO_Init		    ; Initialize I/O port trapping
;
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
;
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
