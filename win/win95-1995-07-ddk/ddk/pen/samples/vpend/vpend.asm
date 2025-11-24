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
TITLE VpenD - Virtual Pen Driver's basic shell file.  Contains entry points
;******************************************************************************
;
;	Title:	VpenD.ASM
;
;	Version:	4.00
;
;==============================================================================

	.386p

;******************************************************************************
;			      I N C L U D E S
;******************************************************************************

	.XLIST
	INCLUDE VMM.Inc
	INCLUDE VPICD.Inc
;	INCLUDE basedef.inc  ; added this
;	INCLUDE vpend.inc
	INCLUDE pendrv.inc
	INCLUDE ins8250.inc
	INCLUDE Debug.inc
	.LIST

	Create_VpenD_Service_Table EQU True

VpenD	equ	1

VxD_LOCKED_DATA_SEG

IFDEF VpenD

VpenD_Device_ID     equ     00025h  ; this value can also be found in vmm.inc

Begin_Service_Table VpenD
VpenD_Service	VpenD_Get_Version, LOCAL
VpenD_Service	VpenD_Get_Pen_Owner, LOCAL
End_Service_Table VpenD

ENDIF


;-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
;
; EXTRN Section

	EXTRN _pVMHwInfo:DWORD
;	EXTRN _ddPM_API_ebp:DWORD



	EXTRN _ActiveDevice:DWORD
	;EXTRN _ActiveDevice:P_VpenD_Register

cVpenD_API_Proc PROTO NEAR C one:DWORD, two:DWORD, three:DWORD

	EXTRN _cVpenD_Device_Load:NEAR
	EXTRN _cVpenD_Device_Exit:NEAR

PUBLIC _VpenD_Hw_Int
PUBLIC _VpenD_EOI

cVpenD_Hw_Int PROTO NEAR C
;	EXTRN _cVpenD_Hw_Int:NEAR
	EXTRN VpenD_Register:NEAR

cCall_Installable_Driver PROTO NEAR C one:DWORD, two:DWORD, three:DWORD
PUBLIC _VpenD_Call_Installable_Driver
	;EXTRN cCall_Installable_Driver:NEAR

;-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

;******************************************************************************
;		 V I R T U A L	 D E V I C E   D E C L A R A T I O N
;******************************************************************************

Declare_Virtual_Device VPEND, 0, 4, VpenD_Control, VpenD_Device_ID, VCD_Init_Order+1, VpenD_API_Proc, VpenD_API_Proc

;******************************************************************************
;			  L O C A L   D A T A
;******************************************************************************

VxD_LOCKED_DATA_ENDS

;******************************************************************************

VxD_LOCKED_CODE_SEG


;******************************************************************************
;
;   VpenD_Control
;
;   DESCRIPTION:  Every virtual driver needs a Control procedure.  See the
;   Virtual device programing books for more information.
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc VpenD_Control
;
; Dispatch routine, shouldn't change
;
	Control_Dispatch Device_Init, _cVpenD_Device_Load	; VpenD_Device_Init
;	Control_Dispatch Set_Device_Focus, <DEBFAR VpenD_Set_Focus>
;	Control_Dispatch Begin_Message_Mode, VpenD_Begin_Message_Mode
;	Control_Dispatch End_Message_Mode, VpenD_End_Message_Mode
	Control_Dispatch Sys_Critical_Exit, _cVpenD_Device_Exit	; VpenD_Exit
	clc
	ret
EndProc VpenD_Control


VxD_LOCKED_CODE_ENDS


VxD_CODE_SEG
;******************************************************************************
;
;   VpenD_Set_Focus
;
;   DESCRIPTION:  This routine needs to be modified so that it will update
;   the "hot" pointers so pen information will be passed to the right VM.
;
;   The code for this routine might look something like:
;
;   back up current pointers into DeviceList
;   get the VM Handle that will be active
;   walk DeviceList looking to see if a device in that VM registered itself
;   if found, update ActivePointers structure with the registered device
;   if not found, update the ActivePointers structure with the NullDevice
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;	EAX, EBX, ECX, Flags
;
;==============================================================================

BeginProc VpenD_Set_Focus
	int 3
	test	edx, edx			; Q: Critial set-focus?
	jz	SHORT VPenD_SF_Focus_Change	;    Y: Change pen too!
;	cmp	edx, VPenD_Device_ID		;    N: Q: Pen?
	cmp	edx, VMD_Device_ID		;    N: Q: Pen?
	jne	SHORT VPenD_SF_Exit		;	   N: Done!

VPenD_SF_Focus_Change:
	int 3
	mov _ActiveDevice.Device_VM_Handle,ebx	; Get old/set new owner
;				mov	[VPenD_Owner], ebx		; Get old/set new owner
;				mov	[Event_Handle],0		; needed for crtl-alt-del screen

	mov	eax, Set_Device_Focus
	mov	edx, VCD_Device_ID
;;;	mov	esi, _HwInfo.ddCom_Port
;				mov	esi, [COM_Number]
	VMMcall	System_Control

VPenD_SF_Exit:
	clc
	ret
EndProc VpenD_Set_Focus


;******************************************************************************
;			       S E R V I C E S
;******************************************************************************


;******************************************************************************
;
; Protect mode API entry point.
;
;******************************************************************************
BeginProc VpenD_API_Proc
;
; Any time the pen driver needs information from the virtual pen driver,
; it calls this routine.  The ax:dx registers that were saved on the stack
; contain the VpenD_ value of the desired service. This routine just
; calls the requested services.
;
; Need to save a pointer to the base pointer in order to convert
; client registers from other routines like cVpenD_Register().

	pushfd
	pushad
;	mov 	_ddPM_API_ebp,ebp		; save base pointer

	mov	ax,[ebp.Client_DX]		; dx hi half of message number
	shl 	eax,16
	mov	ax,[ebp.Client_AX]		; eax contains message number

API_Param1:

	; cx:si will contain any data/pointers from ring 3
	mov cx,[ebp.Client_CX]
	mov si,[ebp.Client_SI]

	test eax,VpenD_lParam1_ptr
	jz @f
	Client_Ptr_Flat edx, cx, si
	jmp API_Param2
@@: mov dx,cx
	shl edx,16
	mov dx,si				; edx now contains the data or pointer

API_Param2:
	; bx:di will contain any data/pointers from ring 3
	mov cx,[ebp.Client_BX]
	mov di,[ebp.Client_DI]

	test eax,VpenD_lParam2_ptr
	jz @f
	Client_Ptr_Flat ebx, cx, di
	jmp API_Continue
@@: mov bx,cx
	shl ebx,16
	mov bx,di
API_Continue:					  ; ebx contains data or pointer

;
; Check to see if someone is calling to register.  If so, need to
; convert a number of pointers before continuing.
;
	cmp eax,VpenD_Load+VpenD_lParam1_ptr
	jne  API_Standard
;
; Yes, want esi to contain a pointer to the register structure.
;
	mov esi,edx
	call VpenD_Register
	xor edx,edx
;
; All the pointers are stored, so continue to process message.
;
API_Standard:
	; Call the C routine and get the message processed.
	invoke	cVpenD_API_Proc, eax,edx,ebx

;	cmp ebp,_ddPM_API_ebp
;	je @f
;	int 3
;@@:
	mov [ebp.Client_AX],ax			; place return value in dx:ax
	shr eax,16
	mov [ebp.Client_DX],ax
	popad
	popfd

	or [ebp.Client_Flags],CF_Mask	; things are OK.
	ret
EndProc VpenD_API_Proc



BeginDoc
;******************************************************************************
;
;   VpenD_Get_Version
;
;   DESCRIPTION:
;
;   ENTRY:
;	None
;
;   EXIT:
;	AH = Major version number
;	AL = Minor version number
;	Carry flag clear
;
;   USES:
;	EAX, Flags
;
;==============================================================================
EndDoc

BeginProc VpenD_Get_Version, Service
	mov	eax, 400h
	clc
	ret
EndProc VpenD_Get_Version

BeginDoc
;******************************************************************************
;
;   VpenD_Get_Pen_Owner
;
;   DESCRIPTION:
;
;   ENTRY:
;	None
;
;   EXIT:
;	EBX = Handle of VM with Pen focus
;
;   USES:
;	EBX, Flags
;
;==============================================================================
EndDoc

BeginProc VpenD_Get_Pen_Owner, Service
	int 3
	mov ebx,_ActiveDevice
	;mov ebx,OFFSET32 _ActiveDevice
	mov ebx,[ebx+Device_VM_Handle]
	ret
EndProc VpenD_Get_Pen_Owner

VxD_CODE_ENDS


VxD_LOCKED_CODE_SEG


;******************************************************************************
;
;	_VpenD_Hw_Int
;
;   DESCRIPTION:
; This routine will be called every time a hardware interrupt occurs.
;
; Psuedo code might look something like:
;
;   check to see if interrupt is for this routine
;   if so,
;	check to see if currently in message mode
;	    handle it, end
;	check to see if Windows owns pen
;	    if not, eat interrupt, end
;
;   read interrupt information
;   if not complete, end
;   else,
;	convert (x,y) and other information
;	place in buffer
;	end physical interrupt
;   schedule callback when interrupts are enabled in system VM
;
; This routine should be modified so that the OEM_Hw_Int routine handles
; gathering the interrupt time information.
;
;   ENTRY:
;
;   eax contains the IRQ handle
;   ebx contains the current VM handle
;
;   EXIT:
;
;   USES:
;
;==============================================================================
BeginProc _VpenD_Hw_Int, High_Freq

	push	eax		; pass on interrupt if it isn't for us
				;;;push	edx
				;;;push	esi
;
; The first thing to do when servicing an interrupt is to determine
; if the virtual pen driver is interacting with a serial device or a BIOS device.
;
	mov 	esi,_pVMHwInfo
	mov 	edx,[esi.dwHwFlags]
	test	edx,HW_BIOS
				;;;pop	esi
				;;;pop	edx
	pop 	eax
	jnz 	SHORT @f  ; jump if bios device
;
; Else interacting with a serial device.  Check to see if there is an
; interrupt pending.
;
;
;;; Preserve the IRQ handle! VPICD_Phys_EOI needs it.
	push	eax
	mov	esi,_pVMHwInfo
	mov	edx,[esi.com_base]
	add	dx,ACE_IIDR
	in	al,dx
	test	al,1
	pop	eax
				;;;pop	esi
				;;;pop	edx
				;;;pop	eax
	jz	SHORT @f
; Should not say this interrupt is serviced!
	VxDCall	VPICD_Phys_EOI	; no interrupt pending
	clc
	stc
	ret
@@:
;
; Service interrupt.
;
       ;mov	ebx, _ActiveDevice		; Does PenWin own the Pen?

	push eax
	lea ebx, _ActiveDevice	;.pDataType
	mov eax,[ebx]
	mov ebx,[eax.Device_VM_Handle]
	pop eax

       ;mov	ebx, OFFSET32 _ActiveDevice	; Does PenWin own the Pen?
       ;mov ebx,[ebx.Device_VM_Handle]
	cmp ebx,0
	je	short bye			; No one is set up to handle the event!

	mov esi,_pVMHwInfo
	cmp	ebx, [esi.SystemVMHandle]
	je	SHORT VHI_WindowsOwnsPen

	cmp	[esi.com_base],0		; Ignore DOS VM?
	je	short bye

				;;;push	eax			; handle message mode behavior
				;;;push	edx
				;;;push	ebx
			;;;;	call	CheckForPenPressed
				;;;pop	ebx
				;;;pop	edx
				;;;pop	eax
	VxDCall	VPICD_Phys_EOI
	clc
	ret

bye:
; Ideally, should reflect the current interrupt with the instruction
; 
;	VxDjmp	VPICD_Set_Int_Request
;
; However, there is currently no DOS-mode driver, so this interrrupt
; will be ignored. Then the com port locks. So for now, the
; current interrupt is not reflected.

	push	eax
				;;;push	edx
				;;;push	esi

	mov 	esi,_pVMHwInfo		; get address to hardware info structure
	mov 	edx,[esi.dwHwFlags]  	; get flags
	test	edx,HW_BIOS
	jnz 	SHORT VHI_around_in  	; don't read the serial port

	mov	edx,[esi.com_base]	; it's a serial device
	add	dx,ACE_LSR
	in	al,dx
	test	al,ACE_DR
	jz	SHORT VHI_around_in
	mov	edx,[esi.com_base]
	in	al,dx
VHI_around_in:
				;;;pop	esi
				;;;pop		edx
	pop	eax
	VxDcall	VPICD_Phys_EOI
	clc
	ret

VHI_WindowsOwnsPen:

	; Invoke the interrupt servicing code.
	invoke cVpenD_Hw_Int

	cmp eax,0
	jne VHI_Ok
	stc
	jmp VHI_Leave
VHI_Ok:
	clc
VHI_Leave:
	ret
EndProc _VpenD_Hw_Int



;******************************************************************************
;
;	_VpenD_EOI
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

BeginProc _VpenD_EOI, High_Freq

	int 3
	VxDCall VPICD_Phys_EOI
	VxDjmp	VPICD_Clear_Int_Request

EndProc _VpenD_EOI



;******************************************************************************
;
;	_VpenD_Call_Installable_Driver
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

BeginProc _VpenD_Call_Installable_Driver

	; call the C routine that will handle this message
	;
	; DWORD dwVMHandle == ebx
	; DWORD dwClientRegStruct == ebp
	; DWORD dwInfo == edx
	;
	invoke cCall_Installable_Driver, ebx,ebp,edx
	ret
EndProc _VpenD_Call_Installable_Driver


VxD_LOCKED_CODE_ENDS

END

;VxD_REAL_INIT_SEG
;
;******************************************************************************
;
;   Bogus_Real_Init
;
;==============================================================================
;BeginProc Bogus_Real_Init
;
;	xor	bx,bx
;	xor	si,si
;	xor	edx,edx
;	mov	ax,Device_Load_Ok
;	ret
;
;EndProc Bogus_Real_Init
;
;VxD_REAL_INIT_ENDS
;
;	END	Bogus_Real_Init
