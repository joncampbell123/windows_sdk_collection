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

PAGE 58,132
;******************************************************************************
TITLE BIOSXlat.ASM - BIOS Interface Translation
;******************************************************************************
;
;   Title:	BIOSXlat.ASM - BIOS Interface Translation
;
;   Version:	1.00
;
;==============================================================================

	.386p

;******************************************************************************
;			      I N C L U D E S
;******************************************************************************

	INCLUDE VMM.Inc
	INCLUDE Debug.Inc
	INCLUDE V86MMGR.Inc
	INCLUDE OptTest.INC



Declare_Virtual_Device BIOSXLAT, 1, 0, BIOSXlat_Control, BIOSXlat_Device_ID, \
		       BIOSXlat_Init_Order


;******************************************************************************
;				 E Q U A T E S
;******************************************************************************



;******************************************************************************
;			     S T R U C T U R E S
;******************************************************************************

Disk_Info_Struc STRUC
DIS_Sector_Size 	dw	?
DIS_Sec_Per_Track	db	?
DIS_Max_Head		db	?
Disk_Info_Struc ENDS




;******************************************************************************
;		   I N I T I A L I Z A T I O N	 D A T A
;******************************************************************************

VxD_IDATA_SEG

VxD_IDATA_ENDS


;******************************************************************************
;			   L O C A L   D A T A
;******************************************************************************

VxD_PAGEABLE_DATA_SEG

B10_Map_Palette_API:
	Xlat_API_Fixed_Len   es, dx, 17
	Xlat_API_Exec_Int    10h

B10_Map_Color_Regs:
	Xlat_API_Calc_Len    es, dx, B10_Calc_Reg_Buff_Len
	Xlat_API_Exec_Int    10h

B10_Write_String_API:
	Xlat_API_Var_Len     es, bp, cx
	Xlat_API_Exec_Int    10h

Xlat_15h_C0:
	Xlat_API_Return_Ptr ES, BX
	Xlat_API_Exec_Int   15h

Xlat_15h_C1:
	Xlat_API_Return_Seg ES
	Xlat_API_Exec_Int   15h

Xlat_13h_No_Translation:
	Xlat_API_Exec_Int 13h

Xlat_13h_Format:
	Xlat_API_Calc_Len   es, bx, BIOSXlat_Calc_Fmt_Buff_Size
	Xlat_API_Exec_Int   13h

Xlat_13h_Return_ES_DI:
	Xlat_API_Return_Ptr es, di
	Xlat_API_Exec_Int   13h

VxD_PAGEABLE_DATA_ENDS

;******************************************************************************
; Data written only during init; read-only thereafter.	Since this data is so
; small and since it will be accessed repeatedly if used, we'll lock it.
;******************************************************************************


VxD_LOCKED_DATA_SEG


BX_I1C_Our_Hook_Seg	dw	?
BX_I1C_Our_Hook_Off	dw	?

VxD_LOCKED_DATA_ENDS

;******************************************************************************
;		   I N I T I A L I Z A T I O N	 C O D E
;******************************************************************************

VxD_ICODE_SEG

;******************************************************************************
;
;   BIOSXlat_Sys_Critical_Init
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc BIOSXlat_Sys_Critical_Init

	mov	esi, OFFSET32 BIOSXlat_Int10
	mov	edx, 10h
	VMMcall Allocate_PM_Call_Back
	jc	BXLSCI_NoPM
	xchg	edx, eax
	mov	ecx, edx
	shr	ecx, 10h
	movzx	edx, dx
	VMMcall Set_PM_Int_Vector

	mov	esi, OFFSET32 BIOSXlat_Int13
	mov	edx, 13h
	VMMcall Allocate_PM_Call_Back
	jc	BXLSCI_NoPM
	xchg	edx, eax
	mov	ecx, edx
	shr	ecx, 10h
	movzx	edx, dx
	VMMcall Set_PM_Int_Vector

	mov	esi, OFFSET32 BIOSXlat_Int15
	mov	edx, 15h
	VMMcall Allocate_PM_Call_Back
	jc	BXLSCI_NoPM
	xchg	edx, eax
	mov	ecx, edx
	shr	ecx, 10h
	movzx	edx, dx
	VMMcall Set_PM_Int_Vector

	mov	esi, OFFSET32 BIOSXlat_Int1C
	mov	edx, 1Ch
	VMMcall Allocate_PM_Call_Back
	jc	DEBFAR BXLSCI_NoPM
	xchg	edx, eax
	mov	ecx, edx
	shr	ecx, 10h
	movzx	edx, dx
	mov	[BX_I1C_Our_Hook_Seg], cx
	mov	[BX_I1C_Our_Hook_Off], dx
	VMMcall Set_PM_Int_Vector


	mov	esi, OFFSET32 BIOSXlat_V86_Int1C
	VMMcall Hook_V86_Int_Chain


;
;   Set the Int 1Eh vector to point to the same address as the real mode
;   Int 1Eh -- This is the floppy data area.
;
	Begin_Touch_1st_Meg
	mov	eax, DWORD PTR ds:[1Eh*4]
	End_Touch_1st_Meg
	movzx	ecx, ax
	shr	eax, 16
	shl	eax, 4
	add	eax, ecx

	VMMcall _BuildDescriptorDWORDs, <eax, 100h, RW_Data_Type, D_GRAN_BYTE, 0>
	VMMcall _Allocate_GDT_Selector, <edx, eax, 0>
	or	edx,eax
IFDEF DEBUG
	jnz	short BXLSCID10
	debug_out "Could not allocate GDT selector BIOSXlat_Sys_Critical_Init"
BXLSCID10:
ENDIF
	jz	short BXLSCI_Fail

	movzx	ecx, ax
	xor	edx, edx
	mov	eax, 1Eh
	VMMcall Set_PM_Int_Vector

	clc
	ret

BXLSCI_NoPM:
	debug_out "Could not allocate one of the PM call backs BIOSXlat_Sys_Critical_Init"
BXLSCI_Fail:
	VMMcall Fatal_Memory_Error

EndProc BIOSXlat_Sys_Critical_Init

VxD_ICODE_ENDS


VxD_LOCKED_CODE_SEG

;******************************************************************************
;
;   BIOSXlat_Control
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc BIOSXlat_Control

	Control_Dispatch Sys_Critical_Init, BIOSXlat_Sys_Critical_Init
	clc
	ret

EndProc BIOSXlat_Control

VxD_LOCKED_CODE_ENDS


VxD_RARE_CODE_SEG

;******************************************************************************
;
;   BIOSXlat_Reflect_Int_EDX
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;	EAX, EDX, Flags
;
;==============================================================================

BeginProc BIOSXlat_Reflect_Int_EDX, VMCREATE

	VMMcall Begin_Nest_V86_Exec
	mov	eax, edx
	mov	edx, [ebp.Client_EFlags]
	VMMcall Exec_Int
	TestReg edx, IF_Mask
	jz	short @F
	VMMCall Enable_VM_Ints
@@:
	VMMjmp	End_Nest_Exec

EndProc BIOSXlat_Reflect_Int_EDX


;******************************************************************************
;
;   BIOSXlat_Int10
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc BIOSXlat_Int10, VMCREATE

	VMMcall Simulate_Iret			; Eat the int right now

	mov	eax, [ebp.Client_EAX]		; Get entry EAX

	cmp	ah, 13h 			; Q: Write string?
	je	SHORT B10_Write_String		;    Y: Must map it.
	cmp	ah, 10h 			; Q: Get/Set palette?
	jne	SHORT BIOSXlat_Reflect_Int_EDX	;    N: Just reflect it now

;
;   Palette Register Calls
;

	cmp	al, 02h 			; Q: Set palette registers?
	je	SHORT B10_Get_Set_Palette	;    Y: Map this one
	cmp	al, 09h 			; Q: Read palette registers?
	je	SHORT B10_Get_Set_Palette	;    Y: Map this one

	cmp	al, 12h
	je	SHORT B10_Get_Set_Color_Regs
	cmp	al, 17h
	jne	SHORT BIOSXlat_Reflect_Int_EDX

B10_Get_Set_Color_Regs:
	mov	edx, OFFSET32 B10_Map_Color_Regs
	VxDjmp	V86MMGR_Xlat_API

B10_Get_Set_Palette:
	mov	edx, OFFSET32 B10_Map_Palette_API
	VxDjmp	V86MMGR_Xlat_API

B10_Write_String:
	mov	edx, OFFSET32 B10_Write_String_API
	VxDjmp	V86MMGR_Xlat_API

EndProc BIOSXlat_Int10



;******************************************************************************
;
;   B10_Calc_Reg_Buff_Len
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc B10_Calc_Reg_Buff_Len

	movzx	ecx, [ebp.Client_CX]
	lea	ecx, [ecx][ecx*2]
	ret

EndProc B10_Calc_Reg_Buff_Len


;******************************************************************************
;
;   BIOSXlat_Int13
;
;   DESCRIPTION:
;	Note that hard disk int 13h are not supported. (because get drive
;	params does not return any usefull info.!!! ie: we can't get the
;	sector size.)
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc BIOSXlat_Int13

	VMMcall Simulate_Iret
	mov	eax, [ebp.Client_EAX]
	test	[ebp.Client_DL], 80h
	jz	SHORT BX_113_To_Floppy
	Debug_Out "Int 13h to hard disk from protected mode not supported!"
	or	[ebp.Client_Flags], CF_Mask
	ret

BX_113_To_Floppy:
	cmp	ah, 2
	jb	SHORT BX_I13_No_Translation
	je	SHORT BX_I13_Read
	cmp	ah, 4
	jb	SHORT BX_I13_Write
	je	SHORT BX_I13_Verify
	cmp	ah, 5
	je	SHORT BX_I13_Format
	cmp	ah, 8
	jb	SHORT BX_I13_No_Translation
	je	SHORT BX_I13_Read_Drive_Param
	cmp	ah, 18h
	je	SHORT BX_I13_Set_Drive_Param

;
;   This command requires no translation
;
BX_I13_No_Translation:
	mov	edx, OFFSET32 Xlat_13h_No_Translation
	VxDjmp	V86MMGR_Xlat_API

;
;   Format command
;
BX_I13_Format:
	mov	edx, OFFSET32 Xlat_13h_Format
	VxDjmp	V86MMGR_Xlat_API

;
;   These commands return ES:DI -> Drive parameter table
;
BX_I13_Read_Drive_Param:
BX_I13_Set_Drive_Param:
	mov	edx, OFFSET32 Xlat_13h_Return_ES_DI
	VxDjmp	V86MMGR_Xlat_API

;
;   Very very stupid translation.  Must set ES:BX to F000:0000 so the stupid
;   BIOS won't program a silly DMA transfer!
;
BX_I13_Verify:
	VMMcall Begin_Nest_V86_Exec
	push	DWORD PTR [ebp.Client_ES]
	push	[ebp.Client_EBX]
	mov	[ebp.Client_ES], 0F000h
	mov	[ebp.Client_BX], 0
	mov	eax, 13h
	VMMcall Exec_Int
	pop	[ebp.Client_EBX]
	pop	DWORD PTR [ebp.Client_ES]
	VMMcall End_Nest_Exec
	ret


;
;   Read and Write commands
;
BX_I13_Write:
BX_I13_Read:

	sub	esp, SIZE Disk_Info_Struc
	mov	edi, esp
	Push_Client_State
	VMMcall Begin_Nest_V86_Exec
	mov	[ebp.Client_AH], 08h		; Read drive param call
	mov	eax, 13h
	VMMcall Exec_Int

	Begin_Touch_1st_Meg
	test	[ebp.Client_Flags], CF_Mask
	jnz	SHORT BX_I13_Use_Defaults
	movzx	eax, [ebp.Client_ES]
	shl	eax, 4
	movzx	ecx, [ebp.Client_DI]
	add	eax, ecx
	movzx	eax, BYTE PTR [eax+3]
	xor	ecx, ecx
	bts	ecx, eax
	shl	ecx, 7
	mov	[edi.DIS_Sector_Size], cx
	movzx	eax, [ebp.Client_CL]
	shl	eax, 2
	shr	al, 2
	mov	[edi.DIS_Sec_Per_Track], al
	mov	al, [ebp.Client_DH]
	mov	[edi.DIS_Max_Head], al
	jmp	SHORT BX_I13_Have_Drive_Info
;
;   This is an OLD floppy BIOS.  Do the best we can to figure it out.
;
BX_I13_Use_Defaults:
	Trace_Out "Get Info on floppy drive failed! -- Using parameters at int 1Eh"
	mov	ecx, DWORD PTR ds:[1Eh*4]
	movzx	eax, cx
	shr	ecx, 16
	shl	ecx, 4
	add	ecx, eax
	movzx	eax, BYTE PTR [ecx+3]		; 0=128, 1=256, etx
	xor	edx, edx
	bts	edx, eax			; Set bit
	shl	edx, 7				; * 128
	mov	[edi.DIS_Sector_Size], dx	; Save sector size
	mov	al, BYTE PTR [ecx+4]
	mov	[edi.DIS_Sec_Per_Track], al
	mov	[edi.DIS_Max_Head], 1		; ASSUME 2 heads!

BX_I13_Have_Drive_Info:
	End_Touch_1st_Meg

	VMMcall End_Nest_Exec
	Pop_Client_State

	mov	ax, [ebp.Client_ES]
	mov	edx, [ebp.Client_EBX]
	call	BIOSXlat_Get_Sel_Base
	jnc	BX_I13_RW_Must_Buffer		; Buffer not in first meg.
;
; Buffer is in first meg, so we can translate to real-mode address and
; reflect to DOS
;
; Since this will read directly into the memory (not through the buffer),
; we have to lock the pages.
;

	mov	esi, eax			; Save lin addr in ESI
	movzx	eax, [ebp.Client_AL]		; get # of sectors
	mul	WORD PTR [edi.DIS_Sector_Size]	; get transfer size in bytes
	mov	ecx, eax
	mov	eax, esi			; restore lin addr of buff.

;
; Obtain the number of pages to lock (the buffer may span a page boundary,
; so we must take both length and offset within page into account)
;
	dec	ecx
	and	eax, 0FFFh
	add	ecx, eax
	shr	ecx, 12
	inc	ecx				; ECX = # of pages to lock
	mov	eax, esi
	shr	eax, 12			; EAX = page # of first page to lock

	push	0				; Push params for unlock later
	push	ecx
	push	eax

	VMMcall _LinPageLock, <eax, ecx, 0>	; Lock the memory
	test	eax, eax			; Q: Did we lock it?
	jnz	SHORT BX_I13_Memory_Locked	;    Y: Good -- Read/write now
	add	esp, 3*4			;    N: Buffer the I/O
	jmp	SHORT BX_I13_RW_Must_Buffer

BX_I13_Memory_Locked:
	shl	esi, 12
	shr	si, 12
	push	[ebp.Client_EBX]		; save client register
	mov	[ebp.Client_BX], si		; real-mode offset of buff.
	shr	esi, 16
	xchg	[ebp.Client_Alt_ES], si		; real-mode segment of buff.
	mov	edx, 13h
	call	BIOSXlat_Reflect_Int_EDX	; Do the interrupt
	mov	[ebp.Client_Alt_ES], si		; restore registers !!!
	pop	[ebp.Client_EBX]

;
;   Parameters to _LinPageUnlock are already pushed on the stack.
;
	VMMcall _LinPageUnlock
	add	esp, 3*4

	test	[ebx.CB_VM_Status], VMStat_Use32_Mask	; Q: Is it 32-bits?
	jz	BX_I13_Common_Exit			;    N: AX returned OK
	mov	WORD PTR [ebp.Client_EAX+2], 0		;    Y: Zero high word
	jmp	BX_I13_Common_Exit


BX_I13_RW_Must_Buffer:
;
;   Now do the read or write, using a translation buffer in V86 memory
;
	push	fs
	push	DWORD PTR [ebp.Client_Alt_ES]
	push	[ebp.Client_EBX]
	push	[ebp.Client_ECX]
	push	[ebp.Client_EDX]

	mov	ax, (Client_ES * 100h)+Client_BX
	VxDcall V86MMGR_Load_Client_Ptr

	VxDcall	V86MMGR_Get_Xlat_Buff_State
	mov	eax, ecx			; EAX = # bytes available
	xor	edx, edx			; clear high dword of accum.
	movzx	ecx, [edi.DIS_Sector_Size]
	div	ecx				; # of sectors we can transfer
	push	eax				; save this on stack

	mov	eax, [ebp.Client_EAX]
	movzx	ecx, al

	jecxz	SHORT BX_I13_Done

BX_I13_RW_Loop:
	mov	edx, ecx			; Assume: can transfer all
	cmp	ecx, [esp]			; Q: is Xlat buffer too small?
	jbe	SHORT BX_I13_RW_Do_Transf	;    N: go ahead and transfer
	mov	edx, [esp]			;    Y: tranfer max sectors
BX_I13_RW_Do_Transf:
	call	BX_I13_Read_Write_Sector
	jc	SHORT BX_I13_Done
	sub	ecx, edx			; ECX = # sectors left
	jz	SHORT BX_I13_Done
;
;   Now move on to next sector
;
	add	[ebp.Client_CL], dl 		; Bump track number
BX_I13_RW_Check_Sector:
	mov	dl, [edi.DIS_Sec_Per_Track]	; DL = Maximum sector number
	cmp	[ebp.Client_CL], dl		; Q: Is this the end of track?
	jbe	BX_I13_RW_Loop			;    N: Keep reading
	sub	[ebp.Client_CL], dl		; "wrap-around" sectors

	inc	[ebp.Client_DH] 		; Go to next head
	mov	dl, [edi.DIS_Max_Head]
	cmp	[ebp.Client_DH], dl
	jbe	SHORT BX_I13_RW_Check_Sector
	mov	[ebp.Client_DH], 0

	inc	[ebp.Client_CH]
	jmp	SHORT BX_I13_RW_Check_Sector


BX_I13_Done:
	add	esp, 4				; pop max # sectors

	movzx	edx, al
	sub	edx, ecx
	mov	[ebp.Client_AL], dl

	pop	[ebp.Client_EDX]
	pop	[ebp.Client_ECX]
	pop	[ebp.Client_EBX]
	pop	DWORD PTR [ebp.Client_Alt_ES]
	pop	fs

BX_I13_Common_Exit:
	add	esp, SIZE Disk_Info_Struc

	ret

EndProc BIOSXlat_Int13


;******************************************************************************
;
;   BX_I13_Read_Write_Sector
;
;   DESCRIPTION:
;
;   ENTRY:
;	EAX = Client's original EAX
;	EDX = number of sectors to transfer (DL)
;	EBX = Current VM handle
;	EBP -> Client register structure
;	EDI -> Disk info struc
;	FS:ESI -> Buffer to read/write
;
;   EXIT:
;	If carry set then
;	    Error during read/wrie
;	else
;	    ESI points past amount read/written
;
;   USES:
;	Flags
;
;==============================================================================

BeginProc BX_I13_Read_Write_Sector

	push	ecx
	push	edi

	push	eax
	push	edx
	movzx	eax, [edi.DIS_Sector_Size]	; # bytes per sector...
	mul	edx				; * # sectors...
	mov	ecx, eax			; = # of bytes to allocate
	pop	edx
	pop	eax

	bt	eax, 8				; Carry set if WRITE
	VxDcall V86MMGR_Allocate_Buffer
	mov	[ebp.Client_BX], di
	shr	edi, 16
	mov	[ebp.Client_Alt_ES], di
	mov	[ebp.Client_AH], ah
	mov	[ebp.Client_AL], dl		; # of sectors to transfer
	VMMcall Begin_Nest_V86_Exec
	push	eax
	mov	eax, 13h
	VMMcall Exec_Int
	pop	eax
	VMMcall End_Nest_Exec
	bt	eax, 8
	cmc					; Carry set if READ
	VxDcall V86MMGR_Free_Buffer
	bt	[ebp.Client_Flags], CF_Bit
	jc	SHORT RWB_Exit
	add	esi, ecx
	clc
RWB_Exit:
	movzx	edx, [ebp.Client_AL]		; return # sectors transfered
	pop	edi
	pop	ecx
	ret

EndProc BX_I13_Read_Write_Sector


;******************************************************************************
;
;   BIOSXlat_Get_Sel_Base
;
;   DESCRIPTION:
;
;   ENTRY:
;	AX = Selector (High word ignored)
;	EBX = Current VM Handle
;	EDX = Offset (Hiword ignored if VM is 16 bit)
;
;   EXIT:
;	EAX = Selector linear address
;	If carry flag SET! then
;	    Address is in current VM's V86 memory address space (< 1Mb)
;	    Value in EAX <= 1Mb even if selector base set to high linear
;	else
;	    Address is not withing VM's V86 address space
;
;   USES:
;	EAX, Flags
;
;==============================================================================

BeginProc BIOSXlat_Get_Sel_Base

	Assert_Cur_VM_Handle ebx

	push	ecx
	push	edx
	VMMcall _SelectorMapFlat, <ebx, eax, 0>
	pop	edx
	test	[ebx.CB_VM_Status], VMSTAT_Use32_Mask
	jnz	SHORT BX_Add_Offset
	movzx	edx, dx
BX_Add_Offset:
	add	eax, edx
	cmp	eax, 100000h
	jc	SHORT BX_GSB_Exit

	sub	eax, [ebx.CB_High_Linear]
	jc	SHORT BX_GSB_Not_High
	cmp	eax, 100000h
	jc	SHORT BX_GSB_Exit
BX_GSB_Not_High:
	add	eax, [ebx.CB_High_Linear]
	clc

BX_GSB_Exit:
	pop	ecx
	ret

EndProc BIOSXlat_Get_Sel_Base






;******************************************************************************
;
;   BIOSXlat_Calc_Fmt_Buff_Size
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;	ECX = Number of bytes to copy for format buffer
;
;   USES:
;	ECX, Flags
;
;==============================================================================

BeginProc BIOSXlat_Calc_Fmt_Buff_Size

	Begin_Touch_1st_Meg
	push	eax
	mov	eax, DWORD PTR ds:[1Eh*4]
	movzx	ecx, ax
	shr	eax, 16
	shl	eax, 4
	add	ecx, eax			; ECX -> BIOS Diskette Table
	movzx	ecx, BYTE PTR [ecx+4]		; ECX = Sectors per track
	shl	ecx, 2
	pop	eax
	End_Touch_1st_Meg
	ret

EndProc BIOSXlat_Calc_Fmt_Buff_Size

;******************************************************************************
;
;   BIOSXlat_Int15
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;   NOTES:
;	C0h, C1h, and C2h are explicitly supported.  All other non-register
;	calls are NOT supported.
;
;==============================================================================

BeginProc BIOSXlat_Int15

	VMMcall Simulate_Iret			; Eat the int right now

	cmp	[ebp.Client_AH], 0C0h		; Q: Is this one we have to map?
	jb	BIOSXlat_Reflect_Int_EDX	;    N: Just reflect it now
	je	SHORT BX_I15_C0

	cmp	[ebp.Client_AH], 0C1h		; Q: Is this one we have to map?
	jne	BIOSXlat_Reflect_Int_EDX	;    N: Just reflect it now

	mov	edx, OFFSET32 Xlat_15h_C1
	VxDjmp	V86MMGR_Xlat_API

BX_I15_C0:
	mov	edx, OFFSET32 Xlat_15h_C0
	VxDjmp	V86MMGR_Xlat_API

EndProc BIOSXlat_Int15

VxD_RARE_CODE_ENDS

;
;   NOTE:  The INT 1Ch routines are locked since they will be hit all the time
;	   and because they are very small.
;

VxD_LOCKED_CODE_SEG

;******************************************************************************
;
;   BIOSXlat_Int1C
;
;   DESCRIPTION:
;	Int 1Ch is the BIOS timer call-back interrupt.	Every timer tick
;	(18.2 times per second) the BIOS does an Int 1Ch.  If
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc BIOSXlat_Int1C

	VMMjmp	Simulate_Iret

EndProc BIOSXlat_Int1C


;******************************************************************************
;
;   BIOSXlat_V86_Int1C
;
;   DESCRIPTION:
;
;   ENTRY:
;	EAX = 1Ch
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc BIOSXlat_V86_Int1C

	test	[ebx.CB_VM_Status], VMStat_PM_App
	jz	SHORT BX_V86_I1C_Exit
	VMMcall Get_PM_Int_Vector
	cmp	cx, [BX_I1C_Our_Hook_Seg]
	jne	SHORT BX_V86_I1C_Reflect_To_PM
	cmp	dx, [BX_I1C_Our_Hook_Off]
	je	SHORT BX_V86_I1C_Exit

BX_V86_I1C_Reflect_To_PM:
	VMMcall Begin_Nest_Exec
	VMMcall Exec_Int
	VMMcall End_Nest_Exec

BX_V86_I1C_Exit:
	stc
	ret

EndProc BIOSXlat_V86_Int1C


VxD_LOCKED_CODE_ENDS


	END
