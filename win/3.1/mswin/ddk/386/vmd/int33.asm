PAGE 58,132
;******************************************************************************
TITLE Int33.Asm - Mouse software interface
;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1989
;
;   Title:	INT33.ASM - Mouse software interface
;
;   Version:	1.00
;
;   Date:	17-Mar-1989
;
;   Author:	RAL
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;   17-Mar-1989 RAL Original
;   22-Mar-1989 RAL Only update cursor if it has been shown at least once
;   08-Jan-1990 RAL Fix for suspending a VM while in middle of cursor update
;   05-Oct-1991 Sandeep Support for mouse in windowed dos apps/updated docs
;
;==============================================================================

	.386p


;******************************************************************************
;			      I N C L U D E S
;******************************************************************************

	.XLIST
	INCLUDE VMM.Inc
	INCLUDE Debug.Inc
	INCLUDE VDD.Inc
	INCLUDE VMD.Inc
	INCLUDE DOSMGR.Inc
	INCLUDE Int2FAPI.Inc
	INCLUDE V86MMGR.Inc
	INCLUDE VDD.Inc
	INCLUDE OptTest.Inc

	include arena.asm

	.LIST


;******************************************************************************
;				E Q U A T E S
;******************************************************************************


I33F_Hidden		EQU	000000001b
I33F_Hidden_Bit 	EQU	0
I33F_Update_Sched	EQU	000000010b
I33F_Update_Sched_Bit	EQU	1
I33F_Update		EQU	000000100b
I33F_Update_Bit 	EQU	2
I33F_Update_Delayed	EQU	000001000b
I33F_Update_Delayed_Bit EQU	3
I33F_Ever_Shown 	EQU	000010000b
I33F_Ever_Shown_Bit	EQU	4
I33F_In_Msg_Mode	EQU	000100000b
I33F_In_Msg_Mode_Bit	EQU	5
I33F_I33_Installed	EQU	001000000b
I33F_I33_Installed_Bit	EQU	6
I33F_Change_State	EQU	010000000b
I33F_Change_State_Bit	EQU	7
I33F_Ever_Called	EQU	100000000b	; foxpro uses pushf, far call
I33F_Ever_Called_Bit	EQU	8
I33F_Set_Usr_Handler	EQU	1000000000b
I33F_Set_Usr_Handler_bit EQU	9
I33F_Set_Ptr		EQU	10000000000b
I33F_Set_Ptr_Bit	EQU	10
I33F_Focus_Changed	EQU	100000000000b
I33F_Focus_Changed_Bit	EQU	11
I33F_GetMousePos	EQU	1000000000000b
I33F_GetMousePos_Bit	EQU	12

VxD_DATA_SEG

	EXTRN	VMD_Owner:DWORD

Int33_Standard_Cursor LABEL DWORD


	dw	0011111111111111b
	dw	0001111111111111b
	dw	0000111111111111b
	dw	0000011111111111b
	dw	0000001111111111b
	dw	0000000111111111b
	dw	0000000011111111b
	dw	0000000001111111b
	dw	0000000000111111b
	dw	0000000000011111b
	dw	0000000111111111b
	dw	0001000011111111b
	dw	0011000011111111b
	dw	1111100001111111b
	dw	1111100001111111b
	dw	1111110001111111b

	dw	0000000000000000b
	dw	0100000000000000b
	dw	0110000000000000b
	dw	0111000000000000b
	dw	0111100000000000b
	dw	0111110000000000b
	dw	0111111000000000b
	dw	0111111100000000b
	dw	0111111110000000b
	dw	0111110000000000b
	dw	0110110000000000b
	dw	0100011000000000b
	dw	0000011000000000b
	dw	0000001100000000b
	dw	0000001100000000b
	dw	0000000000000000b
Int33_Std_Cur_Size EQU $-Int33_Standard_Cursor

I33_Win_API_Addr	dd	0	    ; CS:IP of call-back function

Int33_CB_Offset 	dd	?

I33_Installed		db	False
I33_Notify_VDD		db	False


I33_V86_BP_Seg		dw	?
I33_V86_BP_Offset	dw	?

I33_init_func		db	0
I33_hard_init		db	0
I33_Skip_Update_Cntr	db	0

VxD_DATA_ENDS

Int33_CB_Struc STRUC
I33_CB_Flags		dw	?
I33_CB_Horiz_Hot_Spot	dw	?
I33_CB_Vert_Hot_Spot	dw	?
I33_CB_Text_Cur_Select	dw	?
I33_CB_Text_Screen_Mask dw	?
I33_CB_Text_Cur_Mask	dw	?
I33_CB_Event_Handle	dd	0
I33_CB_Button_State	dw	?
I33_CB_Int_Cond		dw	?
I33_CB_XY_Pos		dd	?
I33_CB_Old_XY_Pos	dd	?
I33_CB_Gr_Cursor	db	Int33_Std_Cur_Size dup (?)
I33_CB_Call_Back_Seg	dw	0
I33_CB_Call_Back_Offset	dd	0
I33_CB_Save_Buff_Size	dw	?
Int33_CB_Struc ENDS

XY_Pos_Struc STRUC
Y_Pos			dw	?
X_Pos			dw	?
XY_Pos_Struc ENDS

VxD_IDATA_SEG

ALIGN 4

EXTRN I33_Soft_Init_INI:BYTE
EXTRN I33_Hard_Init_INI:BYTE

TSR_Driver_Inst InstDataStruc <>

MS_Driver_Name	db	"MS$MOUSE"

VxD_IDATA_ENDS


VxD_ICODE_SEG

;******************************************************************************
;
;   Int33_Critical_Init
;
;   DESCRIPTION:
;	Set up the int33h mapper (for protected mode ints)
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc Int33_Critical_Init, PUBLIC

	mov	esi, OFFSET32 Int33_Pmode_Mapper
	xor	edx, edx			; no reference data

	VMMcall Allocate_PM_Call_Back
	jnc	SHORT I33IOk
	Debug_Out "VMD INT33 ERROR:  Could not alloc PM call back"
	VMMcall Fatal_Memory_Error

I33IOk:
	mov	ecx, eax
	shr	ecx, 10h			; CX = CS to set into vector

	mov	edx, eax
	movzx	edx, dx				; EDX=EIP to set into vector

	mov	eax, 33h			; interrupt number

	VMMjmp	Set_PM_Int_Vector

EndProc Int33_Critical_Init


;******************************************************************************
;
;   Int33_Init
;
;   DESCRIPTION:
;
;   ENTRY:
;	EDX = Reference data from real mode portion
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc Int33_Init, PUBLIC

	test	edx, edx  			; Q: Any int 33 at all?
	jz	SHORT I33_I_Alloc_Data_Area	;    N: skip driver init.

I33_Exists:
	mov	[I33_Installed], True		; Remember we're here

	cmp	dh, VMD_Type_Serial		; Q: Is the mouse type serial?
	jne	SHORT I33_No_Fake_Init		;    N: Don't do this hack
						;    Y: Only allow soft inits

	xor	eax, eax			; default = FALSE
	mov	edi, OFFSET32 I33_Soft_Init_INI
	xor	esi, esi			; [386enh] section
	VMMCall Get_Profile_Boolean
	or	al, al				; Q: user want conversion?
	jz	short dont_convert_inits	;   N:
	mov	al, 33				;   Y: convert call 0's to 33's
dont_convert_inits:
	mov	[I33_init_func], al


I33_No_Fake_Init:

	cmp	dh, VMD_Type_PS2
	jne	SHORT dont_sim_hint

	mov	eax,1				; default true
	mov	edi, OFFSET32 I33_Hard_Init_INI
	xor	esi,esi
	VMMCall	Get_Profile_Boolean
	or	al,al
	jz	short dont_sim_hint
	inc	[I33_hard_init]			; do hardware init

dont_sim_hint:

	cmp	dh, VMD_Type_Undefined		; Q: Did we get mouse type?
	je	SHORT I33_I_Alloc_Data_Area	;    N: Don't bother

	test	dl, dl				; Q: Is it on a valid IRQ
	jz	SHORT I33_I_Alloc_Data_Area	;    N: Zero is silly! (HP)

	movzx	eax, dl 			; EAX = IRQ number
	movzx	ecx, dh 			; ECX = Mouse type
	VxDcall VMD_Set_Mouse_Type		; Tell the mouse driver

I33_I_Alloc_Data_Area:

	VMMcall _Allocate_Device_CB_Area, <<SIZE Int33_CB_Struc>, 0>
	test	eax, eax
	jz	I33_Init_Fatal_Error
	mov	[Int33_CB_Offset], eax

	VxDcall VDD_Get_Version
	cmp	eax, 30Ah			; Q: Is display a 3.10 driver?
	jb	SHORT Int33_IC_Success		;    N: Can't call it
	mov	[I33_Notify_VDD], True
Int33_IC_Success:

;
;   Find out the entry point for DOS app support in a window (if there is one)
;
	Push_Client_State
	VMMcall Begin_Nest_Exec
	mov	[ebp.Client_AX], (W386_Int_Multiplex SHL 8) + W386_Device_Broadcast
	mov	[ebp.Client_BX], VMD_Device_ID
	mov	[ebp.Client_CX], VMD_CO_API_Get_Call_Back   ; Our API multiplex
	mov	eax, W386_API_Int		; Int 2Fh
	VMMcall Exec_Int			; Ask Mr. VM for info
	movzx	eax, [ebp.Client_AX]		; Zero if responded
	mov	cx, [ebp.Client_DS]
	shl	ecx, 16
	mov	cx, [ebp.Client_SI]		; ECX = Address to call
	VMMcall End_Nest_Exec
	Pop_Client_State

	test	eax, eax			; no reply
	jnz	SHORT Int33_I_Success

	mov	[I33_Win_API_Addr], ecx
	mov	esi,OFFSET32 Int33_Smart_Int	; int33 hook for updates
	mov	eax,33h
	VMMCall	Hook_V86_Int_Chain	    	; hook int 16
	jmp	SHORT SkipInt33_Hook		; int33 left unhooked....
Int33_I_Success:
	mov	eax, 33h
	mov	esi, OFFSET32 Int33_Soft_Int
	VMMcall Hook_V86_Int_Chain
SkipInt33_Hook:
	mov	esi, OFFSET32 I33_V86_Call_Back
	xor	edx, edx
	VMMcall	Allocate_V86_Call_Back
	mov	[I33_V86_BP_Offset], ax		; save offset and...
	shr	eax, 16
	mov	[I33_V86_BP_Seg], ax		; ...real-mode segment

	CallRet Int33_Create_VM

I33_Init_Fatal_Error:
	Debug_Out "VMD INT33 ERROR:  Unable to allocate CB area"
	VMMcall Fatal_Memory_Error

EndProc Int33_Init

;******************************************************************************
;   Get_Mouse_Instance
;	grovel around in the dos arena header and int vector to
;	determine if TSR mouse driver is hung off of int 33h. If
;	so, add it to our instance list
;
;	If no TSR mouse driver, instance MS$MOUSE.
;
;	A DOS mouse device driver can instance it's own data and turn off
;	our default handling by hooking Int 2F and watching for:
;	    AX = 1607h	(Device call-out API)
;	    BX = 000Ch	(VMD_Device_ID)
;	    CX = 0	(VMD Instance Check API -- Equate VMD_CO_API_Test_Inst)
;	It must return with CX <> 0 to turn off the automatic VMD instance.
;
;   ASSUMES:
;	- If a mouse driver loaded, it uses INT 33h
;	- driver is a .com (cs=ds)
;	- driver is a TSR program
;	- entry point segment is driver code segment
;
;   ENTRY:
;
;   EXIT:
;	Carry set if error
;	    EAX is error message PTR
;
;   USED:   All but EBP
;
;------------------------------------------------------------------------------
BeginProc Get_Mouse_Instance, PUBLIC

	cmp	[I33_Installed], True
	jne	GMI_Done

	;
	; See if this driver has already instanced it's data
	;
	Push_Client_State
	VMMcall Begin_Nest_Exec
	mov	[ebp.Client_AX], (W386_Int_Multiplex SHL 8) + W386_Device_Broadcast
	mov	[ebp.Client_BX], VMD_Device_ID
	mov	[ebp.Client_CX], VMD_CO_API_Test_Inst	; Our API multiplex
	mov	eax, W386_API_Int		; Int 2Fh
	VMMcall Exec_Int			; Ask Mr. VM for info
	movzx	eax, [ebp.Client_CX]		; Non-zero means instanced
	VMMcall End_Nest_Exec
	Pop_Client_State
	test	eax, eax
	jnz	SHORT GMI_Done

	;
	; We should instance this DOS device
	;
	movzx	edi, word ptr ds:[33h*4+2]	; segment of `mouse driver'
	mov	eax,edi 			; Save in eax
	dec	edi				; arena seg
	shl	edi,4				; esi points to `driver'

	;
	; does this point to a TSR `driver'?
	; check program arena
	;
	cmp	[edi].arena_signature, 4dh	; Q: signature?
	jne	short GMI_no_TSR_mouse		;   N: not TSR mouse
	cmp	[edi].arena_owner, ax		; Q: correct owner?
	jne	short GMI_no_TSR_mouse		;   N: not TSR mouse
	;
	; we have validated the envt arena and the code arena,
	; it must be a real TSR loaded and hooked onto int 33h
	; we unilaterally declare this to be a mouse driver
	; we also instance the envt (is this necessary?)
	;
	movzx	ecx, [edi].arena_size		; get program size
	shl	ecx, 4				; size in bytes
	add	edi,10h 			; Point back to driver

	mov	esi,offset32 TSR_Driver_Inst

	mov	[esi.InstLinAddr],edi		; Address of item
	mov	[esi.InstSize],ecx		; instance whole program
	mov	[esi.InstType],ALWAYS_Field

	VMMCALL _AddInstanceItem,<esi,0>
	or	eax,eax
	jz	short GMI_MemErr
GMI_Done:
	clc
	ret

GMI_no_TSR_mouse:
	mov	esi,offset32 MS_Driver_Name
	VxdCall DOSMGR_Instance_Device
IFDEF DEBUG
	jnc	short GMI10
	debug_out "Didn't find mouse device??? Get_Mouse_Instance"
GMI10:
ENDIF
	jmp	short GMI_Done


GMI_MemErr:
	debug_out "Get_Mouse_Inst Error insuf mem"
	VMMCall Fatal_Memory_Error

EndProc Get_Mouse_Instance


VxD_ICODE_ENDS

;******************************************************************************

VxD_CODE_SEG

;******************************************************************************
;
;   Int33_Create_VM
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

BeginProc Int33_Create_VM, PUBLIC

	cmp	[I33_Installed], True
	jne	SHORT I33_CV_Exit

;
; Force the int33h driver to reset
;
	mov	edx, [Int33_CB_Offset]
	add	edx, ebx
	SetFlag	[edx.I33_CB_Flags], I33F_I33_Installed
	ClrFlag	[edx.I33_CB_Flags], (I33F_Hidden+I33F_Ever_Shown)
	mov	[edx.I33_CB_Horiz_Hot_Spot], -1
	mov	[edx.I33_CB_Vert_Hot_Spot], -1
	mov	[edx.I33_CB_Text_Cur_Select], 0
	mov	[edx.I33_CB_Text_Screen_Mask], 77FFh
	mov	[edx.I33_CB_Text_Cur_Mask], 7700h
	lea	edi, [edx.I33_CB_Gr_Cursor]
	mov	esi, OFFSET32 Int33_Standard_Cursor
	mov	ecx, Int33_Std_Cur_Size / 4
	cld
	rep movsd
	xor	eax, eax
	VxDcall VDD_Hide_Cursor

I33_CV_Exit:
	clc
	ret

EndProc Int33_Create_VM


BeginDoc
;*****************************************************************************
;
;   Int33_API
;
;   DESCRIPTION:
;	This call is made by VMDOSAPP to inform the special mouse driver the 
;	new state of mouse buttons/ mouse position for the app being shown
;	in a window. If the special mouse driver is not installed, then this
;	call returns with carry flag set. If the api is not provided, then 
;	also it returns with carry flag set.
;
;   ENTRY:
;	EAX = Client_AX (high word always ZERO!)
;	EBX = Handle of VM making call
;	EAX = 1 MOUSE API
;	      Client_EBX = Handle of VM on whose behalf the call is made
;	      Client_EDI = (X,Y) position of mouse in pixels
;	      Client_ECX = (event flags,state) where state is the button state
;			   of the mouse and event flags convey the change in
;			   button state from previous call. Refere to int 33h
;			   0Ch for exact values.
;	EAX = 2	special mouse API presence test.
;
;   EXIT: CF Set = Unsupported API/ No special mouse driver.
;	  CF Clear = supported API
;
;   USES: Flags
;
;=============================================================================
EndDoc

BeginProc Int33_API

	cmp	[I33_Win_API_Addr], 0
	je	SHORT I33_API_Error

	VMMcall Test_Sys_VM_Handle
	jne	SHORT I33_API_Error

	cmp	eax, 2				; Presence detection API?
	ja	SHORT I33_API_Error		;   > 2 is invalid
	je	SHORT I33_API_Success		;   = 2 is presence detection

;
;   AX = 1 -- Mouse event call procedure.
;
	mov	ebx, [ebp.Client_EBX]		; EBX = VM handle
	Assert_VM_Handle ebx

	mov	edx, [Int33_CB_Offset]
	add	edx, ebx


	mov	ecx, [ebp.Client_ECX]		; CX = Button state
	mov	edi, [ebp.Client_EDI]		; EDI = X:Y position
 	cmp	[edx.I33_CB_Button_State], cx
 	je	SHORT DontAdjustSkipCntr
EXPER_SKIP_CNTR	EQU	3
 	mov	[I33_Skip_Update_Cntr],EXPER_SKIP_CNTR
DontAdjustSkipCntr:
	mov	[edx.I33_CB_Button_State], cx
	rol	ecx,16
	mov	[edx.I33_CB_Int_Cond], cx
	mov	ecx,[edx.I33_CB_XY_Pos]
	mov	[edx.I33_CB_Old_XY_Pos],ecx	; save
	mov	[edx.I33_CB_XY_Pos], edi

	cmp	[edx.I33_CB_Event_Handle], 0
	jne	SHORT I33_API_Success

	mov	esi, OFFSET32 Int33_Event_Call_Back
	mov	eax,TIME_CRITICAL_BOOST		; switch quickly?
	mov	ecx, PEF_Wait_For_STI OR PEF_Always_Sched
	VMMcall Call_Priority_VM_Event
	mov	[edx.I33_CB_Event_Handle], esi

I33_API_Success:
	ClrFlag [ebp.Client_EFlags], CF_Mask
	ret

I33_API_Error:
	SetFlag [ebp.Client_EFlags], CF_Mask
	ret

EndProc Int33_API


;******************************************************************************
;
;   Int33_Event_Call_Back
;
;   DESCRIPTION:
;
;   ENTRY:
;	EDX -> Int 33 VM private data area
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc Int33_Event_Call_Back

	mov	[edx.I33_CB_Event_Handle], 0

	Push_Client_State
	VMMcall Begin_Nest_V86_Exec
 	mov	edi,[edx.I33_CB_XY_Pos]
	xor	eax,eax
	btr	[edx.I33_CB_Flags],I33F_GetMousePos_Bit	; mouse pos?
	jc	SHORT gotMousePos
	btr	[edx.I33_CB_Flags],I33F_Focus_Changed_Bit
	jnc	SHORT NoFocusChanged
getMousePos:
	mov	[ebp.Client_EAX],3			; parameters...
	mov	eax,33h
	VMMCall	Exec_Int
	mov	ax,[ebp.Client_CX]
	shl	eax,16
	mov	ax,[ebp.Client_DX]
gotmousepos:
	mov	[edx.I33_CB_Old_XY_Pos],eax		; save
NoFocusChanged:
	testMem	[edx.I33_CB_Flags],I33F_Set_Usr_Handler	; is handler set ?
	jnz	SHORT HandlerSet			; yes
NoSpecialAccl:
	mov	[edx.I33_CB_Old_XY_Pos],edi		;No acceleration(hope)
	jmp	DEBFAR Next_X_Pos_Found
HandlerSet:
	TestMem	[edx.I33_CB_Flags],I33F_Set_Ptr		; cursor set ?
	jnz	SHORT NoSpecialAccl			; acceleration!
	TestMem	[edx.I33_CB_Flags],I33F_Ever_Called
	jz	SHORT NoSpecialAccl
	mov	ecx,[edx.I33_CB_Old_XY_Pos]
 ;
 ; Now we compare Old XY position and new XY position.
 ; We keep on adding/subtracting 8 from X position
 ; and 16 from Y position till we get the new position
 ; within 8/16 of X/Y and then make the direct call
 ;
StartCallLoop:

	mov	ax,cx
	add	cx,8
	sub	ax,8
	jnc	LimitY
	xor	ax,ax
LimitY:
	cmp	cx,di				 ; old Y + 8
	ja	SHORT TryLowerBoundY		 ; is > New Y
	mov	[edx.I33_CB_Old_XY_Pos.Y_Pos],cx ; is <= New Y
	jmp	SHORT Next_Y_Pos_Found		 ; current call = oldY+16
TryLowerBoundY:
	cmp	ax,di				 ; old Y - 8
	jbe	SHORT Last_Y_Pos_Found		 ; is <= New Y
	mov	[edx.I33_CB_Old_XY_Pos.Y_Pos],ax ; is greater than New Y
	jmp	SHORT Next_Y_Pos_Found
Last_Y_Pos_Found:
	mov	[edx.I33_CB_Old_XY_Pos.Y_Pos],di
Next_Y_Pos_Found:
	shr	edi,16
	shr	ecx,16
	mov	ax,cx
	add	cx,8
	sub	ax,8
	jnc	LimitX
	xor	ax,ax
LimitX:
	cmp	cx,di				 ; old X + 8
	ja	SHORT TryLowerBoundX		 ; is > New X
	mov	[edx.I33_CB_Old_XY_Pos.X_Pos],cx ; is <= New X
	jmp	SHORT Next_X_Pos_Found		 ; current call = oldX+8
TryLowerBoundX:
	cmp	ax,di				 ; old X - 8
	jbe	SHORT Last_X_Pos_Found		 ; is <= New X
	mov	[edx.I33_CB_Old_XY_Pos.X_Pos],ax ; id greater than New X
	jmp	SHORT Next_X_Pos_Found
Last_X_Pos_Found:
	mov	[edx.I33_CB_Old_XY_Pos.X_Pos],di
Next_X_Pos_Found:

	mov	edi,[edx.I33_CB_Old_XY_Pos]	; next call
	mov	cx,  [edx.I33_CB_Button_State]	; button state same for al
	xor	esi,esi
	cmp	edi,[edx.I33_CB_XY_Pos]		; is this the last call?
	jnz	SHORT NotLastCall		; NO
	xchg	si, [edx.I33_CB_Int_Cond]	; Int cond set for last call
NotLastCall:
	mov	[ebp.Client_DX],cx		; DX = Button state
	mov	[ebp.Client_SI],si		; int condition...
	mov	[ebp.Client_AX],1
	mov	[ebp.Client_CX],di		; EDX = X:Y mickey position
	rol	edi,16
	mov	[ebp.Client_BX],di

	push	edx
	mov	ecx, [I33_Win_API_Addr]
	movzx	edx, cx
	shr	ecx, 16

	VMMcall Simulate_Far_Call
	VMMcall Resume_Exec

	pop	edx
	mov	edi,[edx.I33_CB_XY_Pos]
	mov	ecx,[edx.I33_CB_Old_XY_Pos]
	cmp	ecx,edi
	jne	StartCallLoop

	VMMcall End_Nest_Exec
	Pop_Client_State

	ret

EndProc Int33_Event_Call_Back

;******************************************************************************
;
; Int33_Smart_Int
;
;DESCRIPTION: Call VDD for update soon message
;	      Also sets an ever called flag for switching to dos
;	      VM purpose.
;ENTRY: EBX = current VM handle
;	EBP = client register pointer
;
;EXIT:	CF = 1
;
;USES:	Flags
;
;==============================================================================
BeginProc Int33_Smart_Int, High_Freq

	mov	edx, [Int33_CB_Offset]
	add	edx, ebx			; edx -> VM CB

	mov	eax,[ebp.Client_EAX]

	cmp	ax,0Bh				; get mickey stuff?
	je	SHORT support_GetPos0B
	cmp	ax,3
	je	SHORT support_GetPos

	or	ax,ax				; disabling mouse?
	jnz	SHORT NotHardReset
	ClrFlag	[edx.I33_CB_Flags],<I33F_Set_Usr_Handler+I33F_Set_Ptr+I33F_Ever_Called>
	SetFlag	[edx.I33_CB_Flags],I33F_GetMousePos
NotHardReset:
	cmp	ax,21h				; soft reset
	jnz	SHORT NotSoftReset
	ClrFlag	[edx.I33_CB_Flags],<I33F_Set_Usr_Handler+I33F_Set_Ptr+I33F_Ever_Called>
	SetFlag	[edx.I33_CB_Flags],I33F_GetMousePos
NotSoftReset:
	cmp	ax,0Ch				; set user handler call?
	jne	SHORT NotSetUserHandler
	SetFlag	[edx.I33_CB_Flags],I33F_Set_Usr_Handler
NotSetUserHandler:
	cmp	ax,0Ah				; Set Text pointer ?
	jne	SHORT NotSetTextPointer
	SetFlag	[edx.I33_CB_Flags],I33F_Set_Ptr
NotSetTextPointer:
	cmp	ax,09h				; Set graphics pointer ?
	jne	SHORT SkipI33_SI
	SetFlag	[edx.I33_CB_Flags],I33F_Set_Ptr
	jmp	SHORT SkipI33_SI

Support_GetPos0B:
	SetFlag	[edx.I33_CB_Flags],I33F_Ever_Called

Support_GetPos:
	cmp	ebx,[VMD_Owner]
	je	SHORT skipI33_SI

	cmp	[I33_Notify_VDD],TRUE		; inform vdd ?
	jne	SHORT skipI33_SI		; skip!
	Assert_Cur_VM_Handle ebx

	inc	[I33_Skip_Update_Cntr]
	cmp	[I33_Skip_Update_Cntr],EXPER_SKIP_CNTR
	jbe	SHORT skipI33_SI		; skip update
	mov	[I33_Skip_Update_Cntr],ah	; zero
	VxdCall	VDD_Check_Update_Soon		; check update
skipI33_SI:
	stc					; always reflect
	ret
EndProc Int33_Smart_Int


;*************************************************************************
;
; Int33_VM_Init:
;
; DESCRIPTION:
;	Simulates a hardware reset call for the mouse if I33_Hard_Init
;	is not zero. I33_Hard_Init is nonzero iff PS-2 style mouse is
;	being used and MouseHardResetAtInit is NOT set to 0.
; ENTRY:
;	EBX = VM handle
; EXIT:
;
; USES:
;************************************************************************ 
PUBLIC	Int33_VM_Init
BeginProc Int33_VM_init

	cmp	[I33_Win_API_Addr],0		; special mouse?
	je	SHORT OldMouse
	mov	edx,[Int33_CB_Offset]
	add	edx,ebx
	SetFlag	[edx.I33_CB_Flags],I33F_GetMousePos
	cmp	ebx,[VMD_Owner]			; full screen VM ?
	je	SHORT OldMouse			; yes
	Push_Client_State
	VMMCall	Begin_Nest_V86_Exec

	mov	[ebp.Client_AX],2			; parameters...
	mov	ecx, [I33_Win_API_Addr]
	movzx	edx, cx
	shr	ecx, 16

	VMMcall Build_Int_Stack_Frame
	VMMcall Resume_Exec
	VMMCall	End_Nest_Exec
	Pop_Client_State

OldMouse:
	cmp	[I33_Hard_Init],0
	je	SHORT VM_Hard_Reset_Done	; only if PS2 style mouse

	Push_Client_State
	VMMcall Begin_Nest_V86_Exec
	mov	[ebp.Client_EAX],0
	mov	eax,33h				; simulate hard reset
	VMMCall	Exec_Int
	VMMCall	End_Nest_Exec
	Pop_Client_State

VM_Hard_Reset_Done:

	ret

EndProc	Int33_VM_Init

;******************************************************************************
;
;   Int33_Soft_Int
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

BeginProc Int33_Soft_Int, High_Freq

	movzx	eax, [ebp.Client_AX]
	cmp	eax, 1
	jb	SHORT I33_Reset
	je	SHORT I33_Show_Cursor
	cmp	eax, 9
	je	SHORT I33_Set_Graphics_Cursor
	cmp	eax, 0Ah
	je	SHORT I33_Set_Text_Cursor
I33_Reflect_Int:
	stc
	ret


I33_Reset:
	call	Int33_Create_VM

	movzx	eax, [I33_init_func]
	mov	[ebp.Client_AX], ax
	mov	esi, OFFSET32 Int33_Update_Event
	xor	eax, eax
	VMMcall Call_When_VM_Returns
	jmp	I33_Reflect_Int
;
;
;
I33_Show_Cursor:
	mov	edx, [Int33_CB_Offset]
	SetFlag	[ebx+edx.I33_CB_Flags], I33F_Ever_Shown
	jmp	I33_Reflect_Int


;
;   Set Graphics Cursor
;
I33_Set_Graphics_Cursor:
	mov	edx, [Int33_CB_Offset]
	add	edx, ebx
	test	[edx.I33_CB_Flags], I33F_Update
	jnz	I33_Reflect_Int

	mov	ax, [ebp.Client_BX]
	mov	[edx.I33_CB_Horiz_Hot_Spot], ax
	mov	ax, [ebp.Client_CX]
	mov	[edx.I33_CB_Horiz_Hot_Spot], ax

	xor	eax, eax
	VxDcall VDD_Hide_Cursor
	Client_Ptr_Flat esi, ES, DX
	lea	edi, [edx.I33_CB_Gr_Cursor]
	mov	ecx, Int33_Std_Cur_Size / 4
	cld
	rep movsd
	cmp	[VMD_Owner], ebx
	je	I33_Reflect_Int
	clc
	ret

;
;   Set Text Cursor
;
I33_Set_Text_Cursor:
	mov	edx, [Int33_CB_Offset]
	add	edx, ebx
	test	[edx.I33_CB_Flags], I33F_Update
	jnz	I33_Reflect_Int

	mov	ax, [ebp.Client_BX]
	mov	[edx.I33_CB_Text_Cur_Select], ax
	mov	ax, [ebp.Client_CX]
	mov	[edx.I33_CB_Text_Screen_Mask], ax
	mov	ax, [ebp.Client_DX]
	mov	[edx.I33_CB_Text_Cur_Mask], ax

	cmp	[VMD_Owner], ebx
	je	I33_Reflect_Int
	cmp	[edx.I33_CB_Text_Cur_Select], 0
	je	SHORT I33_Eat_Int
	or	eax, -1
	VxDcall VDD_Hide_Cursor
I33_Eat_Int:
	clc
	ret

EndProc Int33_Soft_Int

;******************************************************************************
;
;   Int33_Update_State
;
;   DESCRIPTION:
;
;   ENTRY:
;	EBX = Handle of VM to update or 0 for no update
;
;   EXIT:
;	None
;
;   USES:
;
;==============================================================================

BeginProc Int33_Update_State, Public

	cmp	[I33_Installed], True
	jne	SHORT I33_US_Exit

	test	ebx, ebx
	jz	SHORT I33_US_Exit

	VMMcall Test_Sys_VM_Handle		; sysVM,I33F_Ever_Shown = False
	je	SHORT I33_US_Exit

	Assert_VM_Handle ebx

	mov	eax, [Int33_CB_Offset]
	mov	ecx, [I33_Win_API_Addr]
	or	ecx,ecx
	jnz	SHORT SkipShowTest		; wonderful mouse driver...
	test	[ebx+eax.I33_CB_Flags], I33F_Ever_Shown
	jz	SHORT I33_US_Exit
SkipShowTest:
	SetFlag	[ebx+eax.I33_CB_Flags], I33F_Change_State
	bts	[ebx+eax.I33_CB_Flags], I33F_Update_Sched_Bit
	jc	SHORT I33_US_Exit
	mov	eax, Low_Pri_Device_Boost
	mov	ecx, 11b
	mov	esi, OFFSET32 Int33_Update_Event
	VMMjmp	Call_Priority_VM_Event

I33_US_Exit:
	ret

EndProc Int33_Update_State


;******************************************************************************
;
;   Int33_Begin_Message_Mode
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

BeginProc Int33_Begin_Message_Mode

	cmp	[I33_Installed], True
	jne	SHORT I33_BMM_Exit

	add	ebx, [Int33_CB_Offset]
	SetFlag	[ebx.I33_CB_Flags],  I33F_In_Msg_Mode

I33_BMM_Exit:
	clc
	ret

EndProc Int33_Begin_Message_Mode


;******************************************************************************
;
;   Int33_End_Message_Mode
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

BeginProc Int33_End_Message_Mode

	cmp	[I33_Installed], True
	jne	SHORT I33_EMM_Exit

	mov	eax, [Int33_CB_Offset]

	ClrFlag	[ebx+eax.I33_CB_Flags], I33F_In_Msg_Mode
	btr	[ebx+eax.I33_CB_Flags], I33F_Update_Delayed_Bit
	jnc	SHORT I33_EMM_Exit
	ClrFlag	[ebx+eax.I33_CB_Flags], I33F_Update_Sched

	call	Int33_Update_State

I33_EMM_Exit:
	clc
	ret

EndProc Int33_End_Message_Mode


;******************************************************************************
;
;   Int33_Update_Event
;
;   DESCRIPTION:
;
;   NOTE:
;	Since the VDD may suspend the VM while this device is updating the
;	state of the mouse cursor (it can't handle the display mode in a
;	window, for example), this code looks for the "I33F_Change_State"
;	flag to be set at the end of the procedure.  It resets the flag
;	at the start, and if it is set when the procedure terminates, it
;	will restore the state of the mouse cursor AGAIN.  This is to prevent
;	leaving the VM in a random state since the mouse focus will be changed
;	while we are in the middle of updating the state.
;
;   ENTRY:
;	EBX = Current VM handle
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc Int33_Update_Event

	mov	edx, [Int33_CB_Offset]
	add	edx, ebx

I33_UE_Restart:
	testMem	[edx.I33_CB_Flags], I33F_In_Msg_Mode
	jnz	I33_UE_Delay

	SetFlag	[edx.I33_CB_Flags], I33F_Update
	ClrFlag	[edx.I33_CB_Flags], I33F_Change_State

	Push_Client_State
	VMMcall Begin_Nest_V86_Exec

	cmp	[I33_Win_API_Addr],0
	jz	SHORT NoSpecialMouse
	push	edx
	mov	ax,2					; disable code
	SetFlag	[edx.I33_CB_Flags], I33F_Focus_Changed
	cmp	ebx,[VMD_Owner]
	jne	SHORT DisableDrawCursor
	ClrFlag	[edx.I33_CB_Flags], I33F_Focus_Changed
	inc	ax					; ax = 3
DisableDrawCursor:
	mov	[ebp.Client_AX],ax			; parameters...
	mov	ecx, [I33_Win_API_Addr]
	movzx	edx, cx
	shr	ecx, 16

	VMMcall Build_Int_Stack_Frame
	VMMcall Resume_Exec
	pop	edx					; get back edx
	ClrFlag	[edx.I33_CB_Flags], (I33F_Update_Sched+I33F_Update)
	jmp	Int33_UE_Cant_Do_It

NoSpecialMouse:

	mov	[ebp.Client_AX], 9
	mov	ax, [edx.I33_CB_Horiz_Hot_Spot]
	mov	[ebp.Client_BX], ax
	mov	ax, [edx.I33_CB_Vert_Hot_Spot]
	mov	[ebp.Client_CX], ax

;
; Allocate a temporary V86 buffer for the cursor
;
	push	fs			; Save this
	mov	ax, ds
	mov	fs, ax			; current data segment
	lea	esi, [edx.I33_CB_Gr_Cursor]
	mov	ecx, Int33_Std_Cur_Size	; number of bytes to copy to buff.
	stc				; copy data
	VxdCall V86MMGR_Allocate_Buffer ; returns: edi = offset of buff.
					;	   ecx = # bytes copied
IFDEF DEBUG
	jnc	SHORT I33_Got_That_There_Buffer
	Debug_Out "VMD WARNING:  Unable to update cursor state for VM #EBX -- Can not allocate buffer"
I33_Got_That_There_Buffer:
ENDIF
	pop	fs
	jc	Int33_UE_Cant_Do_It

	mov	eax, edi
	mov	[ebp.Client_DX], ax
	shr	eax, 16
	mov	[ebp.Client_ES], ax

	cmp	ebx, [VMD_Owner]
	je	SHORT I33_UE_Show_Cursor
	SetFlag	[edx.I33_CB_Flags], I33F_Hidden


	mov	ecx, Int33_Std_Cur_Size / 8
	cld

	movzx	eax, di
	shr	edi, 12
	and	edi, NOT 1111b
	add	edi, eax

	push	ecx
	or	eax, -1
	rep stosd
	pop	ecx
	xor	eax, eax
	rep stosd

	mov	eax, 33h
	VMMcall Exec_int

	cmp	[edx.I33_CB_Text_Cur_Select], 0
	je	SHORT I33_UE_Hide_Text_Cur
	or	eax, -1
	VxDcall VDD_Hide_Cursor
	jmp	SHORT I33_UE_Exit

I33_UE_Hide_Text_Cur:
	mov	[ebp.Client_AX], 0Ah
	mov	[ebp.Client_CX], 0FFFFh
	xor	eax, eax
	mov	[ebp.Client_BX], ax
	mov	[ebp.Client_DX], ax

	mov	eax, 33h
	VMMcall Exec_Int
	jmp	SHORT I33_UE_Exit

;
;   Cursor should be displayed
;
I33_UE_Show_Cursor:
	ClrFlag	[edx.I33_CB_Flags], I33F_Hidden
	mov	eax, 33h
	VMMcall Exec_Int

	mov	[ebp.Client_AX], 0Ah
	mov	ax, [edx.I33_CB_Text_Cur_Select]
	mov	[ebp.Client_BX], ax
	mov	ax, [edx.I33_CB_Text_Screen_Mask]
	mov	[ebp.Client_CX], ax
	mov	ax, [edx.I33_CB_Text_Cur_Mask]
	mov	[ebp.Client_DX], ax

	mov	eax, 33h
	VMMcall Exec_int

	xor	eax, eax
	VxDcall VDD_Hide_Cursor

I33_UE_Exit:
	ClrFlag	[edx.I33_CB_Flags], (I33F_Update_Sched+I33F_Update)

;
; Free temporary V86 buffer
;
	clc				; do not copy back!!!
	mov	ecx, Int33_Std_Cur_Size	; number of bytes in buff.
	VxDCall V86MMGR_Free_Buffer

Int33_UE_Cant_Do_It:
	VMMcall End_Nest_Exec
	Pop_Client_State

	btr	[edx.I33_CB_Flags], I33F_Change_State_Bit
	jc	I33_UE_Restart

	ret


I33_UE_Delay:
	SetFlag	[edx.I33_CB_Flags], I33F_Update_Delayed
	ret

EndProc Int33_Update_Event


;******************************************************************************
;
;   Int33_Pmode_Mapper
;
;   DESCRIPTION:
;	Map interrupts from Pmode to the Real Mode int 33h driver
;
;   ENTRY:
;	EBX = current VM handle
;	EBP -> Current VM's client register structure
;
;   EXIT:
;
;   USES:
;
;==============================================================================


BeginProc Int33_Pmode_Mapper, PUBLIC

	VMMcall Simulate_IRET
;
; Make sure there is a real-mode int33h driver before attempting to reflect
;
	mov	esi, ebx
	add	esi, [Int33_CB_Offset]
	test	[esi.I33_CB_Flags], I33F_I33_Installed	; Q: Flag set ?
	jnz	SHORT I33_Map_Can_Reflect		;    Y: Good, go on!
;
; Flag is not set, but int33 driver may have been installed since last
; check. Get real-mode int. vector: if it points to 0 then we can not
; reflect, else we can.
;
	mov	eax, 33h
	VMMcall Get_V86_Int_Vector		; Q: vector points to 0:0 ?
	jz	SHORT I33_Map_Exit		;    Y: can NOT reflect !!!
						;    N: flag as installed
	SetFlag	[esi.I33_CB_Flags], I33F_I33_Installed

I33_Map_Can_Reflect:
	movzx	eax, [ebp.Client_AX]		; get function #

;
; Check for unsupported calls
;
	cmp	eax, 14h
	je	SHORT I33_Map_Not_Supported
	cmp	eax, 18h
	je	SHORT I33_Map_Not_Supported
	cmp	eax, 19h
	je	SHORT I33_Map_Not_Supported
	cmp	eax, 1Fh
	je	SHORT I33_Map_Not_Supported
;
; Check for calls requiring special action
;
	cmp	eax, 09h
	je	SHORT I33_Map_Ptr_Shape
	cmp	eax, 0Ch
	je	I33_Map_Set_Handler
	cmp	eax, 15h
	je	I33_Map_Get_Buff_Size
	cmp	eax, 16h
	je	I33_Map_Save_State
	cmp	eax, 17h
	je	I33_Map_Restore_State
;
; Reflect all other calls to real mode interrupt handler
;
	callret SHORT I33_Reflect_to_V86

I33_Map_Not_Supported:
IFDEF DEBUG
	Trace_Out "Int33h Mapper : Warning, unsupported int33h call #eax"
ENDIF
	SetFlag	[ebp.Client_FLAGS], CF_Mask	; flag the error

I33_Map_Exit:
	ret

EndProc Int33_Pmode_Mapper

BeginProc I33_Reflect_to_V86

	push	eax
	VMMcall Begin_Nest_V86_Exec
	mov	eax, 33h
	VMMcall Exec_Int
	VMMcall End_Nest_Exec
	pop	eax
	ret

EndProc I33_Reflect_to_V86




;******************************************************************************
;
;   I33_Map_Ptr_Shape
;
;   DESCRIPTION:
;	Set cursore shape.
;	Must copy the pmode buffer into a V86 buffer
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc I33_Map_Ptr_Shape

	push	fs

	mov	ax, (Client_ES * 100h) + Client_DX
	VxDcall V86MMGR_Load_Client_Ptr

	mov	ecx, Int33_Std_Cur_Size
	stc				; copy the cursor into V86 memory
	VxDcall	V86MMGR_Allocate_Buffer
	jc	SHORT I33_Map_Ptr_Shape_Exit
	push	[ebp.Client_Alt_ES]
	push	[ebp.Client_EDX]
	mov	[ebp.Client_DX], di    	; offset of V86 buffer
	shr	edi, 16		       
	mov	[ebp.Client_Alt_ES], di	; real-mode segment of V86 buffer
	call	I33_Reflect_to_V86

	pop	[ebp.Client_DX]
	pop	[ebp.Client_Alt_ES]

	clc				; not necessary to copy back
	VxDcall V86MMGR_Free_Buffer

I33_Map_Ptr_Shape_Exit:
	pop	fs
	ret

EndProc I33_Map_Ptr_Shape



;******************************************************************************
;
;   I33_Map_Set_Handler
;   I33_V86_Call_Back
;
;   DESCRIPTION:
;	Set user-defined mouse event handler (function 0Ch)
;	Client ES:(E)DX points to the handler.
;	We must store this in the CB area, and pass a pointer to a V86
;	breakpoint (call-back) instead.
;	The code at the breakpoint will look up the handler address in the
;	CB area and call it.
;	If no handler has yet been defined (segment is 0), do nothing
;	(but print a nasty message in debug)
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc I33_Map_Set_Handler

	push	fs
	mov	ax, (Client_ES * 100h) + Client_DX
	VxDcall V86MMGR_Load_Client_Ptr

	add	ebx, [Int33_CB_Offset]
	mov	[ebx.I33_CB_Call_Back_Seg], fs
	mov	[ebx.I33_CB_Call_Back_Offset], esi

	mov	ax, [I33_V86_BP_Seg]
	mov	cx, [I33_V86_BP_Offset]

	xchg	ax, [ebp.Client_Alt_ES]
	xchg	cx, [ebp.Client_DX]

	call	I33_Reflect_to_V86

	mov	[ebp.Client_Alt_ES], ax
	mov	[ebp.Client_DX], cx

	pop	fs
	ret

EndProc I33_Map_Set_Handler

BeginProc I33_V86_Call_Back

	VMMcall Simulate_Far_Ret
	add	ebx, [Int33_CB_Offset]
	mov	cx, [ebx.I33_CB_Call_Back_Seg]	; get seg...
	test	cx, cx				; Q: Is there a handler ?
	jz	SHORT I33_V86_Call_Back_Error	;    N: Can't call it then!

	mov	edx, [ebx.I33_CB_Call_Back_Offset] ; ...offset of handler

	Push_Client_State
	VMMcall Begin_Nest_Exec
	VMMcall Simulate_Far_Call		; call handler
	VMMcall Resume_Exec
	VMMcall End_Nest_Exec
	Pop_Client_State
	ret

I33_V86_Call_Back_Error:
IFDEF DEBUG
	Debug_Out "ERROR - mouse handler called, but none defined!!!!"
ENDIF
	ret

EndProc I33_V86_Call_Back



;******************************************************************************
;
;   I33_Map_Get_Buff_Size
;
;   DESCRIPTION:
;	Gets the mouse save state buffer size (function 15h)
;	The buffer size is obtained by reflecting the interrupt to the
;	real-mode int33 handler, and then adding 6 bytes. This extra space
;	is used to store the protect-mode event handler pointer (the real-
;	mode buffer will contain the address of the V86 breakpoint, but
;	not the address of the protect-mode handler).
;	This is actually done only once - the result is then stored in the
;	VM's CB area.
;	Note that "save mouse driver state" and "restore mouse driver state"
;	use this info, so they may call this routine if the info is not yet
;	in the CB area.
;
;   ENTRY:
;	Client_AX = 015h
;
;   EXIT:
;	Client_BX = buffer size
;
;   USES:
;
;==============================================================================

I33_Save_State_Extra STRUC
	I33_SSB_Pmode_Seg	dw	?
	I33_SSB_Pmode_Offset	dd	?
I33_Save_State_Extra ENDS

BeginProc I33_Map_Get_Buff_Size
	push	eax
	push	ebx

	add	ebx, [Int33_CB_Offset]
	mov	ax, [ebx.I33_CB_Save_Buff_Size]	
	test	ax, ax				 ; Q: Size already known?
	jnz	SHORT I33_Map_Get_Buff_Size_Exit ;    Y: Don't reflect int
						 ;    N: Get size from real-
	call	I33_reflect_to_V86		 ;       mode int33 handler
	mov	ax, [ebp.Client_BX]
	mov	[ebx.I33_CB_Save_Buff_Size], ax  ; Store size in CB area

I33_Map_Get_Buff_Size_Exit:
	add	ax, SIZE I33_Save_State_Extra
	mov	[ebp.Client_BX], ax		 ; Return size to client
						 ; Room for pmode ptr
	pop	ebx
	pop	eax
	ret

EndProc I33_Map_Get_Buff_Size


;******************************************************************************
;
;   I33_Map_Save_State
;
;   DESCRIPTION:
;	Save Mouse driver state (function 16h)
;	Reflect the interrupt to real-mode, using a V86-addressable buffer.
;	(Get the size of the buffer from the CB area; if it is not yet
;	defined, call I33_Map_Get_Buff_Size).
;	Then, append the Pmode handler pointer to the buffer.
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc I33_Map_Save_State
	push	fs
	push	[ebp.Client_EDX]
	push	[ebp.Client_Alt_ES]

	mov	ax, (Client_ES * 100h) + Client_DX
	VxDcall	V86MMGR_Load_Client_Ptr

	mov	edi, ebx
	add	edi, [Int33_CB_Offset]

	movzx	ecx, [edi.I33_CB_Save_Buff_Size]
	test	ecx, ecx
	jnz	SHORT I33_Map_SS_Allo_Buff

	mov	ecx, [ebp.Client_EBX]			; save Client_EBX
	mov	eax, [ebp.Client_EAX]			; save Client_EAX
	mov	[ebp.Client_AX], 015h			; set up int33 call
	call	I33_Map_Get_Buff_Size
	mov	[ebp.Client_EAX], eax			; restore Client_EAX
	xchg	ecx, [ebp.Client_EBX]			; get buff size and
							; restore Client_EBX
	movzx	ecx, cx

I33_Map_SS_Allo_Buff:
	push	edi
	clc
	VxDcall V86MMGR_Allocate_Buffer
	jc	SHORT I33_Map_SS_Error

	mov	[ebp.Client_EDX], edi
	shr	edi, 16
	mov	[ebp.Client_Alt_ES], di
	pop	edi
	call	I33_Reflect_to_V86

	stc
	VxDcall V86MMGR_Free_Buffer

	add	esi, ecx
	mov	ax, [edi.I33_CB_Call_Back_Seg]
	mov	[esi.I33_SSB_Pmode_Seg], ax
	mov	eax, [edi.I33_CB_Call_Back_Offset]
	mov	[esi.I33_SSB_Pmode_Offset], eax

I33_Map_SS_Exit:
	pop	[ebp.Client_Alt_ES]
	pop	[ebp.Client_EDX]
	pop	fs
	ret

I33_Map_SS_Error:
	pop	edi
IFDEF DEBUG
	Debug_Out "I33 Mapper, Save State: could not allocate V86 buffer"
ENDIF
	jmp	SHORT I33_Map_SS_Exit

EndProc I33_Map_Save_State


;******************************************************************************
;
;   I33_Map_Restore_State
;
;   DESCRIPTION:
;	The Pmode handler address is taken from the end of the buffer and
;	written to the VM's CB area (for use by the V86 breakpoint routine).
;	The "regular" buffer is then copied into a V86 memory block and
;	the interrupt is reflected to the real-mode int33 handler.
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc I33_Map_Restore_State
	push	fs
	push	[ebp.Client_Alt_ES]
	push	[ebp.Client_EDX]

	mov	ax, (Client_ES * 100h) + Client_DX
	VxDcall	V86MMGR_Load_Client_Ptr

	mov	edi, ebx
	add	edi, [Int33_CB_Offset]

	movzx	ecx, [edi.I33_CB_Save_Buff_Size]
	test	ecx, ecx
	jnz	SHORT I33_Map_RS_Pmode_Ptr

	mov	ecx, [ebp.Client_EBX]			; save Client_EBX
	mov	eax, [ebp.Client_EAX]			; save Client_EAX
	mov	[ebp.Client_AX], 015h			; set up int33 call
	call	I33_Map_Get_Buff_Size
	mov	[ebp.Client_EAX], eax			; restore Client_EAX
	xchg	ecx, [ebp.Client_EBX]			; get buff size and
							; restore Client_EBX
	movzx	ecx, cx

I33_Map_RS_Pmode_Ptr:
	add	esi, ecx
	mov	ax, [esi.I33_SSB_Pmode_Seg]
	mov	[edi.I33_CB_Call_Back_Seg], ax
	mov	eax, [esi.I33_SSB_Pmode_Offset]
	mov	[edi.I33_CB_Call_Back_Offset], eax

	sub	esi, ecx
	stc
	VxDcall V86MMGR_Allocate_Buffer
	jc	SHORT I33_Map_RS_Error

	mov	[ebp.Client_EDX], edi
	shr	edi, 16
	mov	[ebp.Client_Alt_ES], di
	pop	edi
	call	I33_Reflect_to_V86

	clc
	VxDcall V86MMGR_Free_Buffer

IFNDEF DEBUG
I33_Map_RS_Error:
ENDIF
I33_Map_RS_Exit:
	pop	[ebp.Client_EDX]
	pop	[ebp.Client_ES]
	pop	fs
	ret

IFDEF DEBUG
I33_Map_RS_Error:
	Debug_Out "I33 Mapper, Restore State: could not allocate V86 buffer"
	jmp	SHORT I33_Map_RS_Exit
ENDIF
	
EndProc I33_Map_Restore_State


VxD_CODE_ENDS


;******************************************************************************
;	  R E A L   M O D E   I N I T I A L I Z A T I O N   C O D E
;******************************************************************************

VxD_REAL_INIT_SEG

;******************************************************************************
;
;   Int33_Real_Init
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

BeginProc Int33_Real_Init
;
;   If another mouse driver is loaded then don't load -- Just abort our load
;
	test	bx, Duplicate_From_INT2F OR Duplicate_Device_ID
	jnz	SHORT Int33_RI_Abort_Load

	push	es

	mov	ax, 3533h			; Get interrupt vector for
	int	21h				; int 33h through DOS

	xor	edx, edx

	mov	dx, es
	or	dx, bx				; Q: Point to 0:0?
	jz	SHORT Int33_RI_Done		;    Y: No Int 33h driver

	cmp	BYTE PTR es:[bx], 0CFh		; Q: Point to an IRET?
	jne	SHORT Int33_RI_Get_Type 	;    Y: No Int 33h driver!
	xor	edx, edx
	jmp	SHORT Int33_RI_Done

;
;   Now get the mouse type
;
Int33_RI_Get_Type:
	mov	edx, -1				; edx never 0 if mouse present
	mov	ax, 24h 			; Mouse get type call
	xor	cx, cx				; Zero in case call fails
	int	33h				; Do it
	mov	dx, cx				; Pass this info to prot mode

Int33_RI_Done:
	xor	bx, bx
	xor	si, si
	mov	ax, Device_Load_Ok
	pop	es
	ret


;
;   Another mouse driver exists.  Don't load
;
Int33_RI_Abort_Load:
	xor	bx, bx
	xor	si, si
	mov	ax, Abort_Device_Load + No_Fail_Message
	pop	es
	ret

EndProc Int33_Real_Init


VxD_REAL_INIT_ENDS

	END	Int33_Real_Init
