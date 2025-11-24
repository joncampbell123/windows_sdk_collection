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
TITLE VCD.ASM - Virtual COM Device
;******************************************************************************
;
;   Title:	VCD.ASM - Virtual COM Device
;
;==============================================================================
;
;   Functional Description:
;
;	This module virtualizes the COM devices.  It performs
;	many functions to virtualize the physical devices.  These functions
;	are described in the sequence they will be used below:
;
;	    When a VM initializes a COM device the state of the device will
;	    be held in the VMs Control Block until the VM attempts to do
;	    something with the device (such as transmit data) that requires
;	    the physical device to be assigned.
;
;	    When a physical COM port is assigned to a VM, the physical device
;	    is initialized to the VMs virtual state.  Then VCD turns off port
;	    trapping, so that the owner VM can access them without any overhead.
;
;	    If a second VM attempts to use (other than simple initialization)
;	    a COM device that is already assigned to a VM then VCD will resolve
;	    the contention.
;
;	    VMs that have contended for and lost a COM device will NOT contend
;	    for it again.  They will be allowed to own the device if the
;	    original owner is destroyed.  The I/O for these VMs will be
;	    virtualized by the virtual initialization code whenever possible.
;
;******************************************************************************

	.386p


;******************************************************************************
;			L O C A L   C O N S T A N T S
;******************************************************************************

.XLIST
	INCLUDE VMM.Inc
	INCLUDE VPICD.INC
	INCLUDE Debug.Inc
	INCLUDE DOSMGR.INC
	INCLUDE SHELL.INC
	INCLUDE VMPOLL.INC
	INCLUDE	OPTTEST.INC

	Create_VCD_Service_Table EQU 1	    ; VCD service table created
	INCLUDE VCD.INC
	INCLUDE VCDSYS.INC
        INCLUDE MSGMACRO.INC
        INCLUDE VCDMSG.INC
	INCLUDE	VCOMM.INC
	INCLUDE	CONFIGMG.INC
	INCLUDE	regstr.inc

.LIST


;******************************************************************************
;		 V I R T U A L	 D E V I C E   D E C L A R A T I O N
;******************************************************************************

Declare_Virtual_Device VCD, 3, 0Ah, VCD_Control, VCD_Device_ID, VCD_Init_Order \
		       , , VCD_PM_Svc_Call



;******************************************************************************
;		      S E G M E N T   D E F I N I T I O N S
;******************************************************************************


VxD_IDATA_SEG

EXTRN Com_Ports_Ini:BYTE
EXTRN Com_Boost_Time:BYTE
EXTRN verify_base_ini:BYTE
EXTRN VCD_INI_Buf:BYTE
EXTRN VCD_INI_Num:BYTE
EXTRN VCD_Base_Ini:BYTE
EXTRN VCD_IRQ_Ini:BYTE
EXTRN VCD_assign_Ini:BYTE
EXTRN COM_Device_Name_Base:BYTE
EXTRN COM_Device_Name_Num_Off:BYTE

EXTRN VCD_COMM_Int_Addr:DWORD

IFDEF SHARE_IRQ
EXTRN COM_IRQs_Sharable:BYTE
EXTRN VCD_Sharable_COMM_Int_Addr:DWORD
ENDIF

MCA_COM3_Base	equ 3220h
MCA_COM3_IRQ	equ 3

VCD_Init_Data_Struc STRUC
VCD_id_base	dw  ?
VCD_id_irq	db  ?
VCD_id_virt	db  0		; non-zero, if virtualized
VCD_id_extra	dd  ?
VCD_id_CBextra	dd  ?
VCD_id_coms	dd  0
VCD_id_num	dd  ?		; COM port number
VCD_Init_Data_Struc ENDS

VCD_Inst_Base_Addr InstDataStruc <,,400h, 8, ALWAYS_Field>
VCD_Inst_Timeout   InstDataStruc <,,47Ch, 4, ALWAYS_Field>

AUX_Dev_Name	db  'AUX     '
COM_Dev_Name	db  'COM0    '

COM1_extras	dd  0	    ; extra COM struc bytes
		dd  0	    ; extra CB struc bytes
COM2_extras	dd  0	    ; extra COM struc bytes
		dd  0	    ; extra CB struc bytes
COM3_extras	dd  0	    ; extra COM struc bytes
		dd  0	    ; extra CB struc bytes
COM4_extras	dd  0	    ; extra COM struc bytes
		dd  0	    ; extra CB struc bytes

VxD_IDATA_ENDS


;==============================================================================
VxD_DATA_SEG

EXTRN VCD_Setup_Ptrs_Addr:DWORD
EXTRN VCD_Xmit_CallBack_Addr:DWORD

VCD_not_idle_proc_addr	dd  OFFSET32 VCD_Bogus_Idle

VCD_Xmit_CallBack_CSIP	dd  0

VCD_COMList	dd	0

VCDTable	dd	0		; Pointer to array of pointers to 
					; VCD_COM_Struc

;
; WARNING: THESE FOUR VARIABLES ARE TREATED AS A 4 ELEMENT DWORD ARRAY.
;	Do not shuffle them around.
;
VCD_COM1	dd	0		; Null pointer if port dosen't exist
VCD_COM2	dd	0		; Null pointer if port dosen't exist
VCD_COM3	dd	0		; Null pointer if port dosen't exist
VCD_COM4	dd	0		; Null pointer if port dosen't exist


Default_Assign = -2		    ; Fail access silently

Int_Boost_Amount = 2		    ; boost VM's execution time by 2 msec after
				    ; each hardware interrupt (Default)
VCD_Int_Boost_Amount	dd  ?	    ; amount of execution time boost after ints


VCD_PM_Call_Table LABEL DWORD
		  .errnz VCD_PM_API_Get_Version - ($-VCD_PM_Call_Table)/4
	dd	OFFSET32 VCD_PM_Get_Version
		  .errnz VCD_PM_API_Get_Port_Array - ($-VCD_PM_Call_Table)/4
	dd	OFFSET32 VCD_PM_Get_Port_Array
		  .errnz VCD_PM_API_Get_Port_Behavior - ($-VCD_PM_Call_Table)/4
	dd	OFFSET32 VCD_PM_Get_Port_Behavior
		  .errnz VCD_PM_API_Set_Port_Behavior - ($-VCD_PM_Call_Table)/4
	dd	OFFSET32 VCD_PM_Set_Port_Behavior
		  .errnz VCD_PM_API_Acquire_Port - ($-VCD_PM_Call_Table)/4
	dd	OFFSET32 VCD_PM_Acquire_Port
		  .errnz VCD_PM_API_Free_Port - ($-VCD_PM_Call_Table)/4
	dd	OFFSET32 VCD_PM_Free_Port
		  .errnz VCD_PM_API_Steal_Port - ($-VCD_PM_Call_Table)/4
	dd	OFFSET32 VCD_PM_Steal_Port

Max_VCD_PM_Service	  EQU ($-VCD_PM_Call_Table)/4


Mask_For_Sharing    equ 1111011b	; always have to virtualize IIR, because
.errnz	UART_IIR - 2			; we need to read it in Ring 0, which
					; clears the physical port!!!!!!!!!!!

Normal_Mask	    equ 1111111b


VCD_Disable_Mask    dd	Normal_Mask

VCD_Def_Int_Proc    dd	OFFSET32 VCD_Int
VCD_Def_EOI_Proc    dd	OFFSET32 VCD_EOI
VCD_Windows_Int     dd	?

VCD_Global_Flags    dd	0

fVCD_Init_Complete  equ 00000001b
bVCD_Init_Complete  equ        0

VCD_IRQs_virtualized	dd  0

VxD_DATA_ENDS


VxD_Pageable_Data_Seg

KnownBases	dw	3F8h, 2F8h, 3E8h, 2E8h

device_init_flag    db	0
fDevice_Init_Done   equ 00000001b
fMCA_machine	    equ 00000010b

machine_info	dd	?

prd_Temp	dd	0

RegBuf	db	9	DUP (0)

PortName	db	'PortName',0

Overrides	dd	0

Name_Buf            db 9 dup (0)

NumPorts	dd	0
MaxPort		dd	0

Contention_Handler	dd	OFFSET32 _VCD_Map_Name_To_Resource
			dd	OFFSET32 _VCD_Acquire_Resource
			dd	OFFSET32 _VCD_Steal_Resource
			dd	OFFSET32 _VCD_Release_Resource
			dd	OFFSET32 _VCD_Port_Arrival
			dd	OFFSET32 _VCD_Port_Departure

VxD_Pageable_Data_Ends

;******************************************************************************

VxD_ICODE_SEG

EXTRN VCD_Relocate_COMM_Int:NEAR

;******************************************************************************
;
;   VCD_add_portnum
;
;   DESCRIPTION:    Add 1 or 2 ASCII digits of port # into a string at EDI
;
;   ENTRY:	    EBX = port # - 1  (0 = COM1, 1 = COM2, ...)
;		    EDI = position in buf to start digit(s)
;
;   EXIT:	    EDI = next available position in buf
;
;   USES:	    EDI, Flags
;
;==============================================================================
BeginProc VCD_add_portnum

	push	eax
	push	edx
	cld
	inc	ebx
	mov	eax, ebx
	cmp	eax, 9		;Q: 2 digits?
	jbe	short @F	;   N:
	mov	dl, 10		;   Y: do tens digit
	div	dl
	add	al, '0'
	stosb
	mov	al, ah
@@:
	add	al, '0' 	; do ones digit
	stosb
	dec	ebx
	pop	edx
	pop	eax
	ret

EndProc VCD_add_portnum


;******************************************************************************
;
;   VCD_copy_ini_string
;
;   DESCRIPTION:    Append an INI suffix (BASE, IRQ, AUTOASSIGN) to VCD_INI_Buf
;		    starting at EDI.
;
;   ENTRY:	    EDI = destination in VCD_INI_Buf
;		    EDX = string suffix to append to VCD_INI_Buf
;
;   EXIT:	    none
;
;   USES:	    Flags
;
;==============================================================================
BeginProc VCD_copy_ini_string

	pushad
	mov	esi, edx
	cld
@@:
	lodsb
	stosb
	or	al, al
	jnz	@B
	popad
	ret

EndProc VCD_copy_ini_string

VxD_IDATA_Seg

VCDName	db	'VCD',0
hKey	dd	0
sharable	db	0
temp	dd	0

VxD_IDATA_Ends

;******************************************************************************
;
;   VCD_Initialize
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;		CLC => OK, else failed mem alloc
;   USES:
;		ALL
;
;==============================================================================
BeginProc VCD_Initialize

	cmp	[VCD_COMList], 0		; Q: Alloc'ed list yet ?
	jnz	VCD_Initialize_Done		;    Y:

	mov	ecx, SIZE VCD_Init_Data_Struc	; size
	xor	eax, eax			; flags
	VMMCall	List_Create
	jc	sci_failed_alloc

	mov	[VCD_COMList], esi		; save list handle

	call	VCD_Relocate_COMM_Int
	mov	eax, [VCD_COMM_Int_Addr]
	mov	[VCD_Windows_Int], eax

	VMMCall Get_Machine_Info
	mov	[machine_info], ebx
	TestReg	ebx, GMIF_MCA		    ; Q: Micro channel?
	jz	short @F		    ;	N:
	or	[device_init_flag], fMCA_machine
@@:
;
; Check the registry for IRQ sharability
;
	xor	edi, edi		    ; default = FALSE
	TestReg	ebx, <GMIF_MCA OR GMIF_EISA>  ; Q: Micro channel or EISA?
	jz	SHORT not_MCA_EISA	    ;	N:
	or	edi, -1 		    ;	Y: default = TRUE

not_MCA_EISA:
	mov	esi, OFFSET32 hKey
	VMMCall	_GetRegistryKey,<REGTYPE_VXD, \
		OFFSET32 VCDName, 0, esi>
	test	eax, eax		; Q: Does such a key exist ?
	jnz	VI_FoundDefault		;    N: use default

	mov	prd_Temp, 1
	VMMCall	_RegQueryValueEx,<[esi], OFFSET32 COM_IRQs_Sharable, \
		0, OFFSET32 RegType, OFFSET32 sharable, OFFSET32 prd_Temp>
	test	eax, eax
	jnz	VI_FoundDefault
	movzx	edi, sharable

VI_FoundDefault:
	or	edi, edi		; Q: sharable ?
	jz	not_sharable		;    N:
	mov	[VCD_Disable_Mask], Mask_For_Sharing
	mov	[VCD_Def_Int_Proc], OFFSET32 VCD_Sharable_Int
	mov	[VCD_Def_EOI_Proc], OFFSET32 VCD_Sharable_EOI
	mov	eax, [VCD_Sharable_COMM_Int_Addr]
	mov	[VCD_Windows_Int], eax
not_sharable:
;
; Check the registry for interrupt boost time specifications
;
	mov	eax, Int_Boost_Amount
	cmp	DWORD PTR [esi], 0
	jz	VI_Boost_Found

	mov	prd_Temp, 4
	VMMCall	_RegQueryValueEx,<[esi], OFFSET32 Com_Boost_Time, \
		0, OFFSET32 RegType, OFFSET32 temp, OFFSET32 prd_Temp>
	test	eax, eax
	mov	eax, Int_Boost_Amount
	jnz	VI_Boost_Found
	mov	eax, [temp]

VI_Boost_Found:
	mov	[VCD_Int_Boost_Amount], eax

VCD_Initialize_Done:

	clc
	ret

sci_failed_alloc:
	Debug_Out 'VCD: failed to get memory for initialization'
%OUT fatal mem error?
	stc
	ret

EndProc VCD_Initialize


;******************************************************************************
;
;   NAME:
;	VCD_Device_Init
;
;   DESCRIPTION:
;	This procedure will initialize all COM ports.  It converts the list
;	pointed to by [VCD_COMList] from a list of VCD_Init_Data_Strucs to
;	a table of dword pointers to VCD_COM_Struc's.
;
;   ENTRY:
;	EBX = SYS VM handle
;
;   EXIT:
;	Carry flag set if error (device will not be installed)
;
;   USES:
;	Every register & flags
;
;------------------------------------------------------------------------------

BeginProc VCD_Device_Init

	or	[device_init_flag], fDevice_Init_Done
	mov	esi, [VCD_COMList]
	test	esi, esi
	jz	VCD_End_Init
	VMMCall	List_Get_First

init_ports:
	jz	vdi_ip_done
	inc	[numPorts]
	mov	edi, eax
	cmp	[edi.VCD_id_base], 0		;Q: COM exists?
	je	short skip_port 		;   N:
	cmp	[edi.VCD_id_coms], 0		;Q: COM exists & not alloc'ed?
	jne	short skip_port 		;   N:
	pushad
	movzx	eax, [edi.VCD_id_base]		; eax = base port
	movzx	ebx, [edi.VCD_id_irq]		; ebx = irq #
	mov	ecx, [edi.VCD_id_num]		; COM port num
	call	VCD_Init_Port
	jnc	short @F
	dec	[numPorts]
	mov	[edi.VCD_id_base], 0
@@:
	popad

skip_port:
	mov	eax, edi
	mov	ecx, [edi.VCD_id_Num]
	cmp	ecx, [MaxPort]
	jb	@F
	mov	[MaxPort], ecx
@@:
	mov	esi, [VCD_COMList]
	VMMCall	List_Get_Next
	jnz	init_ports

vdi_ip_done:
;
; Now we just make a list of all the VCD_id_coms and release
; all elements of VCD_COMList.
;
	mov	eax, [MaxPort]
	cmp	eax, [numPorts]
	jae	@F
	mov	eax, [numPorts]
	mov	[MaxPort], eax
@@:
	shl	eax, 2
	VMMCall	_HeapAllocate,<eax,HEAPZEROINIT>
	test	eax, eax
	jnz	@F
	VMMJmp	Fatal_Memory_Error
@@:
	mov	[VCDTable], eax
	mov	edi, eax			; save ptr
	mov	esi, [VCD_COMList]

vdi_convert_loop:
	VMMCall	List_Get_First
	jz	vdi_conversion_done
	VMMCall	List_Remove
	cmp	[eax.VCD_id_base], 0
	je	vdi_dealloc_continue
	mov	ecx, [eax.VCD_id_num]
	mov	ebx, [eax.VCD_id_coms]
	mov	[edi+ecx*4-4], ebx

vdi_dealloc_continue:
	VMMCall	List_Deallocate
	jmp	vdi_convert_loop

vdi_conversion_done:
	VMMCall	List_Destroy			; destroy the list.
	xor	eax, eax
	mov	[VCD_COMList], eax

;
;   Hook BIOS interrupt 14h
;
	mov	al, 14h
	mov	esi, OFFSET32 VCD_Soft_Int_14h
	VMMcall Hook_V86_Int_Chain

;
;   Instance the COM BIOS areas
;
	VMMCall _AddInstanceItem, <<OFFSET32 VCD_Inst_Base_Addr>, 0>
IFDEF DEBUG
	or	eax, eax
	jnz	short @F
	Debug_Out 'AddInstanceItem of com base port address area failed'
@@:
ENDIF
	VMMCall _AddInstanceItem, <<OFFSET32 VCD_Inst_Timeout>, 0>
IFDEF DEBUG
	or	eax, eax
	jnz	short @F
	Debug_Out 'AddInstanceItem of com timeout value failed'
@@:
ENDIF
;
;   Check to make sure VMPOLL is around
;
	VxDcall VMPoll_Get_Version
	or	eax, eax
	jz	SHORT VCD_DI_No_VMPoll
	mov	[VCD_not_idle_proc_addr], OFFSET32 VCD_not_idle
VCD_DI_No_VMPoll:


;
;   Initialize SYS VM's control block
;
	VMMcall Get_Sys_VM_Handle		; EBX = System VM's handle
	call	VCD_Create_VM			; Initialize VM1's control blk

	clc					; No error
VCD_End_Init:
	ret

EndProc VCD_Device_Init


;******************************************************************************
;
;   VCD_Init_Complete
;
;   DESCRIPTION:    This procedure calls DOSMGR to instance the DOS AUX and
;		    COM devices
;
;   ENTRY:	    EBX = SYS VM handle
;
;   EXIT:	    Carry clear
;
;   USES:	    EAX, ESI, Flags
;
;==============================================================================
BeginProc VCD_Init_Complete

	or	[VCD_Global_Flags], fVCD_Init_Complete
	mov	esi, OFFSET32 AUX_Dev_Name
	VxDCall DOSMGR_Instance_Device

	mov	ecx, [MaxPort]
	jecxz	vic_done

	mov	edi, [VCDtable]
instance_port_device:
	mov	esi, [edi]
	or	esi, esi
	jz	short @F
	lea	esi, [esi.VCD_Name]
	VxDCall DOSMGR_Instance_Device
@@:
	add	edi, 4
	loop	instance_port_device

vic_done:
	clc
	ret

EndProc VCD_Init_Complete


;******************************************************************************
;
;   NAME:
;	VCD_Init_Port
;
;   DESCRIPTION:
;	This procedure will initialize a single COM port.  It first tests
;	for the existance of the port, then reads the current state of the
;	device registers into the default values (VCD_Def_xxx).
;
;   ENTRY:
;	EAX = Base port for this COM
;	EBX = IRQ #
;	ECX = COM number (1, 2, 3, or 4)
;	EDI -> VCD_Init_Data_Struc
;
;   EXIT:
;	If Carry then
;	    ERROR
;
;   USES:
;	EAX, EBX, EDX, ESI, Flags
;
;------------------------------------------------------------------------------
BeginProc VCD_Init_Port

	pushad

	mov	eax, SIZE VCD_Com_Struc + Thunk_Area
	add	eax, [edi.VCD_id_extra]
	VMMCall _HeapAllocate, <eax, HeapZeroInit>
	test	eax, eax			; Q: Did we get the memory?
	jnz	SHORT VCD_Alloc_Worked		;    Y: GOOD!
	Debug_Out "VCD ERROR:  Could not allocate memory for COM structure"
	popad
	jmp	VCD_Dont_Virt_Port

VCD_Alloc_Worked:
	mov	esi, eax
	add	esi, Thunk_Area 		; esi -> VCD_COM_Struc
	mov	[edi.VCD_id_coms], esi

;
; copy template
;
	push	edi
	push	esi
	mov	edi, eax
	mov	esi, OFFSET32 Thunk_Template
	mov	ecx, Thunk_Area / 4
	cld
	rep	movsd
	pop	esi
;
; fixup call to VCD_Handler
;
	mov	edi, OFFSET32 VCD_Handler
	sub	edi, esi
	mov	dword ptr [esi-4], edi
;
; fixup call to VCD_Dispatch_IO
;
	mov	edi, OFFSET32 VCD_Dispatch_IO - Call_Ins_Size
	sub	edi, eax
	mov	[esi.VCD_IO_Thunk+1], edi
	pop	edi

;
;   Initialize data structure
;
	mov	eax, SIZE VCD_CB_Struc
	add	eax, [edi.VCD_id_CBextra]
	VMMCall _Allocate_Device_CB_Area, <eax, 0>
	test	eax, eax
	jnz	SHORT VCD_Got_CB_Area
	Debug_Out "VCD ERROR:  Could not allocate control block area"
	popad
	jmp	VCD_Dont_Virt_Port

VCD_Got_CB_Area:
	mov	[esi.VCD_CB_Offset], eax
	mov	[esp.Pushad_ESI], esi
	popad

	mov	[esi.VCD_Number], cl	    ; (1 based)
	mov	[esi.VCD_IObase], eax
	mov	[esi.VCD_IRQN], bl

	push	eax
	call	ReadDefaultState		; read hw state into registers.
	pop	eax

;
;   Enable trapping of COM ports
;
	push	esi
	lea	esi, [esi.VCD_IO_Thunk] 	; trap handler in header of
	push	ecx				;   VCD_COM_Struc
	mov	ecx, UART_PORTS
	mov	edx, eax			; edx = port #
VCDI_Trap_Loop:
	VMMcall Install_IO_Handler		; Install the port trap routine
IFDEF DEBUG
	jnc	short @F
	mov	cl, [esi.VCD_Number]
	Trace_Out 'VCD failed to install I/O handlers for COM#cl'
@@:
ENDIF
	jc	short VCDI_io_failed
	inc	edx				; EDX = Next port
	loop	VCDI_Trap_Loop			; Do it for every port
VCDI_io_failed:
	pop	ecx
	pop	esi
	jc	VCD_End_Init_Port

	cmp	bl, -1				;Q: irq disabled?
	jne	short irq_not_disabled		;   N:
	SetFlag	[esi.VCD_Flags], VCD_IRQ_Init	;   Y: flag IRQ as initialized
						;      so we won't attempt to
						;      virtualize
irq_not_disabled:
	push	edi
	mov	eax, dword ptr [COM_Device_Name_Base]
	mov	dword ptr [esi.VCD_Name], eax
	mov	eax, dword ptr [COM_Device_Name_Base+4]
	mov	dword ptr [esi.VCD_Name+4], eax

	movzx	edi, [COM_Device_Name_Num_Off]
	lea	edi, [edi][esi.VCD_Name]
	dec	ecx
	mov	ebx, ecx
	call	VCD_add_portnum

; get auto-assign specification from INI
;
	push	ecx
	push	esi
	mov	edi, OFFSET32 VCD_INI_Num
	call	VCD_add_portnum
	mov	edx, OFFSET32 VCD_assign_Ini
	call	VCD_copy_ini_string
	mov	edi, OFFSET32 VCD_INI_Buf
	mov	eax, Default_Assign
	mov	ecx, 3
	xor	esi, esi
	VMMCall Get_Profile_Fixed_Point
	pop	esi
	pop	ecx
	cmp	eax, -1000			; Q: Is entry -1.000?
	jne	SHORT VCD_Valid_Timeout 	;    N: Use value in EAX
	or	eax, -1 			;    Y: Set to -1
VCD_Valid_Timeout:
	mov	[esi.VCD_Auto_Assign], eax

	xor	eax, eax
	mov	[esi.VCD_IRQ_Desc], eax
	pop	edi

	call	VCD_Set_IRQ_Procs

	clc					; No error
VCD_End_Init_Port:
        ret                                     ; Return

VCD_Dont_Virt_Port:
	stc
	ret

EndProc VCD_Init_Port

BeginDoc
;******************************************************************************
;
;   VCD_Virtualize_Port
;
;   DESCRIPTION:
;	Virtualize a COM port.	This service allows additional VxD's to
;	provide enhanced capabilities to COM port virtualization.  An example
;	VxD is one which buffers high speed input data, and then simulates
;	interrupts into the owner VM, at a slower rate, so that the VM can
;	keep up with the data without losing input.
;	CAN ONLY BE CALLED DURING Sys_Critical_Init.
;
;
;   ENTRY:
;	EAX = Port # (1, 2, 3, 4)
;	EBX = Flags
;		= 00000001h = IRQ is sharable
;		  all other bits reserved
;	ECX = # of extra bytes needed in VCD_COM_Struc
;	EDX = # of extra bytes needed in VCD_CB_Struc
;	ESI -> VCD_ProcList_Struc - a structure which contains a list of
;	       call back procedures (NOTE:  VCD saves the pointer to this
;	       structure, so it must be placed in VxD_DATA_SEG)
;
;   EXIT:
;	If Carry Clear then
;	    port can be virtualized,
;	    EAX = COM Handle -> COM Struc (extra data allocated at end)
;	Else
;	    port not available
;	    EAX = 0, port doesn't exist
;	    EAX = 1, port already virtualized
;
;   USES:
;
;   CALL BACKS:
;	Call back procedures are provided in the list pointed to by ESI.
;
;	The VPS_Control_Proc call back gets called when the virtualization
;	state changes for a COM port.  Currently the only call is for ownership
;	changes.
;	    EAX = VCD_Control_Set_Owner
;	    EBX = VM Handle of new owner, or 0, if virtualization handled by
;		  a different device
;	    EDX = VM Handle of previous owner, or 0
;	    ESI -> VCD_COM_Struc
;	    On entry, when EBX is new owner, port trapping will be enabled for
;	    all I/O ports of the COM adapter.  The control proc can disable
;	    any I/O trapping that it desires.
;
;	The IRQ virtualization procs are the same as if the VxD virtualized the
;	IRQ directly thru VPICD, except that ESI points to the VCD_COM_Struc
;	on entry, so see VPICD documentation for actual entry parameters and
;	return values.
;	    EAX = IRQ handle
;	    EBX = VM handle
;	    ESI -> VCD_COM_Struc
;	These call backs have default actions, if the call back offset is set
;	to 0:
;	    VPS_Hw_Int_Proc	 - assign owner to current VM, if not owned,
;				   and request int in owner VM
;	    VPS_Virt_Int_Proc	 - not virtualized
;	    VPS_EOI_Proc	 - physically EOI & clear int request
;	    VPS_Mask_Change_Proc - assign owner to current VM, if not owned
;	    VPS_IRET_Proc	 - not virtualized
;
;	VPS_In_RxTxB, VPS_Out_RxTxB, VPS_In_IER, VPS_Out_IER, VPS_In_IIR,
;	VPS_Out_IIR, VPS_In_LCR, VPS_Out_LCR, VPS_In_MCR, VPS_Out_MCR,
;	VPS_In_LSR, VPS_Out_LSR, VPS_In_MSR, and VPS_Out_MSR are call backs
;	for dealing with I/O for ports that have trapping enabled while the
;	virtualizing VxD owns a COM port.
;	    EBX = VM handle
;	    ESI -> VCD_COM_Struc
;	    EDX = port #
;	    ECX = Byte_Input or Byte_Output
;	    AL = data, if ECX = Byte_Output
;	    Procs should return AL = data, if ECX = Byte_Input
;
;==============================================================================
EndDoc
BeginProc VCD_Virtualize_Port, SERVICE

	pushad
IFDEF DEBUG
	test	ebx, 0FFFEh
	jz	short VVP_D00
	Debug_Out 'Non-zero reserved flag bits in #EBX'
VVP_D00:
ENDIF
	TestMem	[VCD_Global_Flags], fVCD_Init_Complete
	jnz	no_com_port

	mov	esi, [VCD_COMList]
	test	esi, esi
	jz	no_com_port
	mov	ecx, eax			; save # id
	VMMCall	List_Get_First

vvp_get_Init_struc:
	jz	no_com_port
	cmp	[eax.VCD_id_num], ecx
	je	vvp_found_port
	VMMCall	List_Get_Next
	jmp	vvp_get_Init_Struc

vvp_found_port:
	mov	esi, [esp.Pushad_ESI]
	mov	ecx, [esp.Pushad_ECX]

	cmp	[esi.VPS_Hw_Int_Proc], 0
	je	short no_com_port

	cmp	[eax.VCD_id_virt], 0	    ;Q: COM port virtualized?
	jne	short com_virtualized	    ;	Y:

	mov	[eax.VCD_id_extra], ecx
	mov	[eax.VCD_id_CBextra], edx

	mov	edi, eax
	mov	ecx, [esp.Pushad_EAX]	    ; ecx = hard com #
	movzx	ebx, [edi.VCD_id_irq]	    ; ebx = irq #
	movzx	eax, [edi.VCD_id_base]	    ; eax = base port
	call	VCD_Init_Port
	jnc	@F
	mov	[edi.VCD_id_base], 0	    ; set base port to 0.
	jc	short no_com_port
@@:
	inc	[edi.VCD_id_virt]
	mov	edi, [edi.VCD_id_coms]
	TestMem	[esp.Pushad_EBX], 1
	jnz	short IRQ_sharable
	SetFlag	[edi.VCD_Flags], VCD_not_sharable_IRQ
IRQ_sharable:

	mov	esi, [esp.Pushad_ESI]
	mov	[edi.VCD_virt_procs], esi
	SetFlag	[edi.VCD_Flags], VCD_Virtualized    ; flag as virtualized
	mov	[esp.Pushad_EAX], edi
	popad
	clc
	ret

com_virtualized:
	popad
	mov	eax, 1
	jmp	short virt_failed

no_com_port:
	popad
	xor	eax, eax
virt_failed:
	stc
	ret

EndProc VCD_Virtualize_Port


VxD_ICODE_ENDS



VxD_Locked_CODE_SEG


EXTRN VCD_Eat_Interrupt:NEAR


;******************************************************************************
;
;   VCD_not_idle
;
;   DESCRIPTION:    Call VMPOLL if necessary and possible to tell it that
;		    this VM is definitely not idle.
;
;   NOTE:	    DONT CALL THIS PROCEDURE DIRECTELY!  Call indirect through
;		    [VCD_not_idle_proc_addr].  It will be set to VCD_Bogus_Idle
;		    if the VMPoll device is not installed.
;
;   ENTRY:	    EBX = VM Handle or 0
;
;   EXIT:	    none
;
;   USES:	    Flags
;
;==============================================================================
BeginProc VCD_not_idle

	or	ebx, ebx
	jz	short @F
	Assert_VM_Handle ebx
	TestMem	[VCD_Global_Flags], fVCD_Init_Complete
	jz	short @F
	VxDCall VMPoll_Reset_Detection
@@:
VCD_Bogus_Idle LABEL NEAR
	ret

EndProc VCD_not_idle

VxD_Locked_Code_Ends

VxD_Pageable_Code_Seg

;
; Template code copied to header of each COM data area allocated in VCD_Init_Port
;
Virt_PIC_procs	= 5	; 5 procs for IRQ virtualization thru VPICD

	Dword_Align
Thunk_Template:
VCD_IO_Thunk = $
	call	VCD_Dispatch_IO
Call_Ins_Size = $ - Thunk_Template
VCD_IRET_Thunk = $
	mov	dl, VCD_IRET_Proc-VCD_Hw_Int_Proc	; offset to VCD_IRET_Proc
	jmp	short @F
VCD_Mask_Thunk = $
	mov	dl, VCD_Mask_Change_Proc-VCD_Hw_Int_Proc; offset to VCD_Mask_Change_Proc
	jmp	short @F
VCD_EOI_Thunk = $
	mov	dl, VCD_EOI_Proc-VCD_Hw_Int_Proc	; offset to VCD_EOI_Proc
	jmp	short @F
VCD_VirtInt_Thunk = $
	mov	dl, VCD_Virt_Int_Proc-VCD_Hw_Int_Proc	; offset to VCD_Virt_Int_Proc
	jmp	short @F
VCD_HwInt_Thunk = $
	mov	dl, VCD_Hw_Int_Proc-VCD_Hw_Int_Proc	; offset to VCD_Hw_Int_Proc
@@:
	call	VCD_Handler

Thunk_Area	= ($ - Thunk_Template)
.errnz	Thunk_Area AND 3

VCD_IO_Thunk	  = VCD_IO_Thunk - $
VCD_IRET_Thunk	  = VCD_IRET_Thunk - $
VCD_Mask_Thunk	  = VCD_Mask_Thunk - $
VCD_EOI_Thunk	  = VCD_EOI_Thunk - $
VCD_VirtInt_Thunk = VCD_VirtInt_Thunk - $
VCD_HwInt_Thunk   = VCD_HwInt_Thunk - $

VCD_IO_COMS_bias  = -VCD_IRET_Thunk ; offset from ret of VCD_IO_Thunk to
				    ; start of VCD_COM_Struc

;***
;
; ReadDefaultState
;
; Entry:
;	ESI -> VCD_COM_Struc
; Exit:
;	None
; Uses:
;	EDX, EAX
;
BeginProc ReadDefaultState

;
;   Read current state of registers into default variables
;
	mov	edx, [esi.VCD_IObase]
	add	edx, UART_LCR			; DX = Line Control Register
	in	al, dx				; AL = LCR value
        mov     [esi.VCD_Def_LCR],al            ; Save default line control
	or	al,LCR_DLAB			; Set Divisor Latch Access Bit
	IO_Delay
        out     dx,al                           ; Set access to divisor latch
	add	edx, (UART_BAUD_LSB - UART_LCR)
	IO_Delay
        in      al,dx                           ; AL = LSB of default BAUD
        mov     [esi.VCD_Def_BAUD_LSB],al       ; Save it
	inc	edx
	IO_Delay
        in      al,dx                           ; AL = MSB of default BAUD
        mov     [esi.VCD_Def_BAUD_MSB],al       ; Save it
	add	edx, (UART_LCR - UART_BAUD_MSB) ; DX = Line Control Register
        mov     al,[esi.VCD_Def_LCR]            ; AL = Original LCR value
	IO_Delay
        out     dx,al                           ; Restore original LCR
	add	edx, (UART_IER - UART_LCR)	; DX = Interrupt Enable Reg
	IO_Delay
        in      al,dx                           ; AL = IER value
        mov     [esi.VCD_Def_IER],al            ; Save it
	add	edx, (UART_MCR - UART_IER)	; DX = MCR port
	IO_Delay
	in	al,dx				; AL = MCR value

	mov	[esi.VCD_Def_MCR],al		; Save it
	inc	edx				; DX = Line Status Port
	.errnz	UART_LSR - UART_MCR - 1
	IO_Delay
        in      al,dx                           ; AL = LSR value
        or      al,LSR_TXBITS                   ; Set THRE and TEMT
	mov	[esi.VCD_Def_LSR],al		; Save it
	inc	edx
	.errnz	UART_MSR - UART_LSR - 1 	; DX = Modem Status Port
	IO_Delay
        in      al,dx                           ; AL = MSR value
        mov     [esi.VCD_Def_MSR],al            ; Save it
	mov	[esi.VCD_Virt_IIR], 1		; init to no ints identified

	ret

EndProc ReadDefaultState

BeginDoc
;******************************************************************************
;
; VCD_Virtualize_Port_Dynamic
;
; Entry:
;
;	EAX = size of extra are needed in VCD_COM_Struc
;	ECX = size of extra area needed per VM
;	EDX = Port number.
;	EBX = Flags
;		bit 0 = 1 => IRQs are sharable
;	EDI -> Virt Procs
;	ESI = (BASE, IRQ)
;
; Exit:
;	CLC = OK, else not OK
;	ESI -> VCD_Com_Struc.
; Uses:
;	ESI.
;==============================================================================
EndDoc
BeginProc VCD_Virtualize_Port_Dynamic, Service

	pushad

	add	eax, SIZE VCD_Com_Struc + Thunk_Area
	VMMCall	_HeapAllocate, <eax, HeapZeroInit>
	test	eax, eax			; Q: Did we get enough mem ?
	Debug_OutZ "VCD ERROR: Could not alloc mem for COM struct"
	jz	VIPMI_Dont_Virt_Port

	mov	esi, eax
	add	esi, Thunk_Area			; ESI -> VCD_Com_Struc

	mov	ecx, [VCDTable]
	mov	edx, [esp.Pushad_EDX]		; get port number.
	mov	DWORD PTR [ecx+edx*4-4], esi	; save VCD_COM_Struc
	mov	[esi.VCD_Number], dl
	mov	edx, [esp.Pushad_ESI]
	mov	[esi.VCD_IRQN], dl
	shr	edx, 16
	mov	[esi.VCD_IOBase], edx
 ;
 ; copy template
 ;
	push	esi
	mov	edi, eax			; EDI -> Thunk area
	mov	esi, OFFSET32 Thunk_Template
	mov	ecx, Thunk_Area /4
	cld
	rep	movsd
	pop	esi

 ;
 ; Fixup call to VCD_Handler
 ;
	mov	edi, OFFSET32 VCD_Handler
	sub	edi, esi
	mov	DWORD PTR [esi-4], edi
 ;
 ; fixup call to VCD_Dispatch_IO
 ;
	mov	edi, OFFSET32 VCD_Dispatch_IO - Call_Ins_Size
	sub	edi, eax
	mov	[esi.VCD_IO_Thunk+1], edi
 ;
 ; Initialize data structure
 ;
	mov	eax, SIZE VCD_CB_Struc
	add	eax, [esp.Pushad_ECX]
	VMMCall	_Allocate_Device_CB_Area, <eax, 0>
	test	eax, eax
	Debug_OutZ "VCD Error: Could not allocate control block area"
	jz	VIPMI_Dont_Virt_Port

	mov	[esi.VCD_CB_Offset], eax	; save the CB offset

	call	ReadDefaultState

 ;
 ; Enable trapping of COM ports.
 ;
	mov	edx, [esp.Pushad_ESI]		; EDX = port #
	shr	edx, 16
	push	esi
	lea	esi, [esi.VCD_IO_Thunk]		; trap handler in header of
	push	ecx				; VCD_COM_Struc
	mov	ecx, UART_PORTS

VIPMI_Trap_Loop:
	VMMCall	Install_IO_Handler		; Install the port trap routine
	Trace_OutC "VCD Failed to install IO handler"
	jc	VIPMI_io_Failed
	inc	edx				; next port
	loop	VIPMI_Trap_Loop			; do it for all

VIPMI_io_Failed:
	pop	ecx
	pop	esi
	jc	VIPMI_End_Virt_Port

	TestMem	[esp.Pushad_EBX], 1		; Q: irq sharable
	jnz	VIPMI_irq_sharable		;    Y:
	SetFlag	[esi.VCD_Flags], VCD_Not_Sharable_Irq	;    N: flag it so

VIPMI_irq_sharable:
	mov	[esi.VCD_Auto_Assign], Default_Assign
	xor	eax, eax
	mov	[esi.VCD_IRQ_Desc], eax

	call	VCD_Set_IRQ_Procs

	mov	[esp.Pushad_ESI], esi
	popad
	mov	[esi.VCD_Virt_procs], edi	; save virt procs
	test	edi, edi
	jz	@F
	SetFlag	[esi.VCD_Flags], VCD_Virtualized
@@:
	clc
	ret

VIPMI_End_Virt_Port:
%OUT remove IO handlers for these ports.

VIPMI_Dont_Virt_Port:
	popad
	stc
	ret

EndProc VCD_Virtualize_Port_Dynamic

;******************************************************************************
;
; VCD_UnVirtualize_Port_Dynamic
;
; Description:
;		Unvirtualizes a given port
; Entry:
;		ESI -> VCD_COM_Struc
; Exit:
;		None
; Uses:
;		Flags.
;
BeginProc VCD_UnVirtualize_Port_Dynamic, Service

	pushad

	testMem	[esi.VCD_Flags], VCD_Owns_IRQ
	jz	vupd_skip_unvirt_irq
	mov	eax, [esi.VCD_IRQ_Handle]
	VxdCall	VPICD_Force_Default_Behavior

vupd_skip_unvirt_irq:
	mov	edx, [esi.VCD_IOBase]
	mov	ecx, UART_PORTS

vupd_remove_handler_loop:
	VMMCall	Remove_IO_Handler
	Trace_OutC "VCD Failed to remove IO handler"
	inc	edx
	loop	vupd_remove_handler_loop

	VMMCall	_Deallocate_Device_CB_Area,<[esi.VCD_CB_Offset], 0>

	sub	esi, Thunk_Area
	VMMCall	_HeapFree, <esi, 0>

	popad
	ret

EndProc VCD_UnVirtualize_Port_Dynamic

BeginDoc
;******************************************************************************
;
;   VCD_Get_Version
;
;   DESCRIPTION:    Get VCD device version
;
;   ENTRY:
;
;   EXIT:	    IF Carry clear
;			EAX is version
;		    ELSE VCD device not installed, and VCD pages are
;			not allocated
;
;   USES:
;
;==============================================================================
EndDoc
BeginProc VCD_Get_Version, SERVICE

	mov	eax, 400h
	clc
	ret

EndProc VCD_Get_Version


BeginDoc
;******************************************************************************
;
;   VCD_Set_Port_Global
;
;   DESCRIPTION:    Enable/Disable Global handling of a COM port between
;		    multiple VM's.  When a port is declared to be a global
;		    port, then no contention detection is performed, and no
;		    I/O ports are trapped.  This service is provided mainly
;		    for a COM port mouse, or other serial device, where a
;		    separate virtual device handles the arbitration of the
;		    port and its interrupts.  A Set_Device_Focus System_Control
;		    call can be made to specify which VM should receive
;		    interrupt requests and VCD_Get_Focus returns the current
;		    owner of a port.
;
;   ENTRY:	    EAX = Hardware COM port #	(1-4)
;		    EDX = 0, declare port to be a global port
;		    EDX # 0, declare port to be a local port
;
;   EXIT:	    Carry clear if okay
;		    Carry set - port not available
;
;   USES:	    flags
;
;==============================================================================
EndDoc
BeginProc VCD_Set_Port_Global, SERVICE

	push	eax

	test	[device_init_flag], fDevice_Init_Done ; must call after devinit
	jz	VSPG_no_port		; device_init not done yet. 
	cmp	eax, [MaxPort]
	ja	short VSPG_no_port	; return Carry set, if port # > max
	sub	eax, 1
	jb	short VSPG_invalid_port ; return Carry set, if port # < 1
	shl	eax, 2
	add	eax, [VCDtable]
	mov	eax, [eax]
	or	eax, eax
	jz	short VSPG_no_port
	or	edx, edx		; Q: global port?
	jnz	short VSPG_not_global	;   N:
	SetFlag	[eax.VCD_Flags], VCD_global_port
	push	ebx
	VMMCall Get_Cur_VM_Handle	; set owner to cur VM
	jmp	short VSPG_exit

VSPG_not_global:
	ClrFlag	[eax.VCD_Flags], VCD_global_port
	push	ebx
	mov	ebx, [eax.VCD_Owner]	; set owner to current owner
	or	ebx, ebx
	jz	short VSPG_no_owner
	inc	ebx			;Q: port owned by a VxD?
	jz	short VSPG_no_owner	;   Y: so no VM owner
	dec	ebx			;   N: restore VM handle

VSPG_exit:
	push	esi
	mov	esi, eax
	push	ebx
	xor	ebx, ebx
	call	VCD_Set_Owner		; zero current owner
	pop	ebx
	call	VCD_Set_Owner		; reset to cur VM, or current owner
	pop	esi
VSPG_no_owner:
	pop	ebx
	stc
VSPG_no_port:
	cmc
VSPG_invalid_port:
	pop	eax
	ret

EndProc VCD_Set_Port_Global

BeginDoc
;******************************************************************************
;
; VCD_Set_Port_Global_Special
;
; Description:
;		Service very similar to VCD_Set_Port_Global, except that
;	no virtualization of IRQ is done.
; Entry:
;	Same as VCD_Set_Port_Global
; Exit:
;	Same as VCD_Set_Port_Global
; Uses:
;	Same as VCD_Set_Port_Global
;==============================================================================
EndDoc
BeginProc VCD_Set_Port_Global_Special, Service

	SetFlag	[Overrides],1
	VxDJmp	VCD_Set_Port_Global
	
EndProc VCD_Set_Port_Global_Special

BeginDoc
;******************************************************************************
;
;   VCD_Get_Focus
;
;   DESCRIPTION:    Return the VM handle of the current owner of a COM port
;		    (Focus can be set with a Set_Device_Focus System_Control
;		    call specifying the VCD device)
;
;   ENTRY:	    EAX = Hardware COM port #	(1-4)
;
;   EXIT:	    Carry clear - EBX = VM Handle, 0 if no owner
;		    Carry set - port not available
;
;   USES:	    EBX, flags
;
;==============================================================================
EndDoc
BeginProc VCD_Get_Focus, SERVICE

	push	eax
	cmp	eax, [MaxPort]
	ja	short gf_no_port	; return Carry set, if port # > max
	sub	eax, 1
	jb	short gf_invalid_port	; return Carry set, if port # < 1
	mov	ebx, [VCDtable]
	mov	ebx, [eax*4][ebx]
	or	ebx, ebx
	jz	short gf_no_port
	mov	ebx, [ebx.VCD_Owner]
	inc	ebx			;Q: port owned by a VxD?
	jz	short @F		;   Y: return 0 for owner VM handle
	dec	ebx			;   N: restore ebx
@@:
	stc
gf_no_port:
	cmc
gf_invalid_port:
	pop	eax
	ret

EndProc VCD_Get_Focus


BeginDoc
;******************************************************************************
;
;   VCD_Acquire_Port
;
;   DESCRIPTION:    Allow another VxD to claim ownership of a COM port.  This
;		    service will check to see if the COM port is currently
;		    owned, if it is, then the normal contention rules apply.
;
;		    The VxD claiming ownership can disable trapping of the
;		    COM port's I/O ports by calling Disable_Global_Trapping
;		    for all 8 I/O ports, but then VCD will not be able to
;		    provide contention detection.  Disabling trapping needs
;		    to be done, if the VxD does not handle all access to the
;		    port hardware; it is preferable for the VxD to handle
;		    the hardware and provide the API's to read and write data.
;
;   ENTRY:	    EAX = Hardware COM port #	(1-4) (or base port if ECX&2)
;		    EDI -> nul terminated VxD name
;		    ECX = flags
;			Bit 1 = 1 => EAX actually contains base port.
;
;   EXIT:	    Carry set, if the port could not be acquired, or does not
;		    exist.
;		     If Carry is set, then Z flag is set if owned by somebody
;		     else, else unknown port.
;		    If Carry is clear, then EAX is the COM port handle which
;		    is needed to call VCD_Free_Port.
;
;   USES:	    EAX, Flags
;
;==============================================================================
EndDoc
BeginProc VCD_Acquire_Port, SERVICE

	pushad

IFDEF DEBUG
	test	ecx,NOT 2
	jz	@F
	Debug_Out 'Incorrect flags (#ecx) sent to VCD_Acquire_Port service'
@@:
ENDIF
	test	cl,2
	jz	VAP_Index_Known
	call	Find_Index			; 0 if unknown, else index.

VAP_Index_Known:
	sub	eax, 1
	jb	VAP_failed
	cmp	eax, [MaxPort]			;Q: Port index in range?
        jae     VAP_failed_Unknown              ;   N:
	shl	eax, 2
	add	eax, [VCDtable]
	mov	esi, [eax]
	or	esi, esi
        jz      VAP_failed_Unknown              ; Port is not valid

	cmp	[esi.VCD_IObase], 0		;Q: non-zero base?
	je	short VAP_failed_Unknown	;   N: can't assign port

;
; This has to be done in this order!  We first check for no current owner,
; if true, then go ahead and assign.  Then check for "forced assignment", if
; true, then jump down to see if owner is different.  If we check for "forced
; assignment" first, then we do the wrong thing on the "is owner different"
; check when the port doesn't have an owner (0 is different than cur VM handle)
;
	mov	ecx, [esi.VCD_Owner]
	jecxz	SHORT VAP_OK_To_Assign		; jump if no current owner
	inc	ecx				;Q: already owned by a VxD?
%OUT may want a message when the port is already owned
	jz	short VAP_failed		;   Y: can't steal

	cmp	[esi.VCD_Auto_Assign], 0	;Q: INI specified auto assign?
	je	SHORT VAP_OK_To_Assign		;   Y: steal the port
	jl	short VAP_port_busy		;   N: always contend
						;   N: INI specified timeout

	VMMCall Get_System_Time
	sub	eax, [esi.VCD_Last_Use]
	cmp	eax, [esi.VCD_Auto_Assign]	;      Q: Can we steal port?
	ja	short VAP_OK_To_Assign		;	    Y:

VAP_port_busy:
;
; message box for contention, copy COM port name into message text first
;
        mov     ebx, OFFSET32 Name_Buf
	mov	eax, dword ptr [esi.VCD_Name]
        mov     [ebx], eax
	mov	eax, dword ptr [esi.VCD_Name+4]
        mov     [ebx+4], eax

	mov	ebx, ecx			; use VM handle for current owner
        PUSH_SPRINTF <MSG_VCD_VxD_Contention>, <OFFSET32 Name_Buf>
        xchg    eax, ecx                        ; ECX = message on stack
        mov     eax, MB_ASAP + MB_ICONHAND + MB_NOWINDOW + MB_YESNO + MB_SYSTEMMODAL

	VxDCall SHELL_SysModal_Message		; send message to Windows
        POP_SPRINTF
	cmp	al, IDYES
	jne	short VAP_failed

VAP_OK_To_Assign:

IFDEF DEBUG  ;_verbose
	mov	al, [esi.VCD_Number]
	Trace_Out 'VxD acquiring COM#al'
ENDIF
	push	ebx
	xor	ebx, ebx			;   Y: clear owner
	call	VCD_Set_Owner
	dec	ebx
	mov	[esi.VCD_Owner], ebx
	pop	ebx
	mov	[esp.Pushad_EAX], esi
	popad
	clc
	ret

VAP_failed:
	xor	eax,eax				; Set Z flag
	popad
	stc
	ret

VAP_Failed_Unknown:
	test	esp,esp				; reset z flag
	popad
	stc
	ret

EndProc VCD_Acquire_Port


BeginDoc
;******************************************************************************
;
;   VCD_Free_Port
;
;   DESCRIPTION:    This service is used to free a COM port from VxD ownership
;		    so it is possible for VM applications to use the port
;		    again.
;
;   ENTRY:	    EAX = COM port handle returned from VCD_Acquire_Port
;
;   EXIT:	    none
;
;   USES:	    Flags
;
;==============================================================================
EndDoc
BeginProc VCD_Free_Port, SERVICE

IFDEF DEBUG
	cmp	word ptr [eax.VCD_Name], 'OC'
	je	short @F
	Debug_Out 'VCD_Free_Port called with a possibly invalid COM port handle (#eax)'
	jmp	short vfp_exit
@@:
ENDIF
	cmp	[eax.VCD_Owner], -1
IFDEF DEBUG
	je	short @F
	Debug_Out 'ignoring VCD_Free_Port called on a port not owned by a VxD'
	jmp	short vfp_exit
@@:
ELSE
	jne	short vfp_enable_trapping
ENDIF
	mov	[eax.VCD_Owner], 0

vfp_enable_trapping:
	pushad
	mov	edx, [eax.VCD_IObase]
	mov	ecx, UART_PORTS
@@:
	VMMcall Enable_Global_Trapping
	inc	edx
	loopd	@B
	popad
vfp_exit:
	ret

EndProc VCD_Free_Port

;******************************************************************************
;
; Find_Index
;
; Description:
;		Given a base port, finds the 1 based index of the port.
; Entry:
;	EAX = Port base
; Exit:
;	Port Index (starts at 1) if known port, else MaxPort+1
; Uses:
;	EAX, EDX, Flags
;==============================================================================
BeginProc Find_Index

	push	ebx
	push	ecx

	mov	edx,[VCDTable]			; get ptrs to VCD_COM_Struc
	xor	ecx,ecx

FI_lp:
	cmp	ecx,[MaxPort]			; Q: port index in range ?
	jae	FI_Done				;    N:
	mov	ebx,[edx]			;    get the VCD_COM_Struc
	or	ebx,ebx				; Q: valid port ?
	jz	FI_NextPort			;    N: look for next
	cmp	[ebx.VCD_IObase],eax		; Q: Does base match ?
	jz	FI_Done				;    Y: found mouse

FI_NextPort:
	inc	ecx				; next VCD_COM_Struc
	add	edx,4
	jmp	FI_lp

FI_Done:
	mov	eax,ecx				; port index 0 based
	inc	eax				; port index 1 based
						; eax=MaxPort+1 =>invalid port

	pop	ecx
	pop	ebx
	ret

EndProc Find_Index

BeginDoc
;******************************************************************************
;
; VCD_Find_COM_Index
;
; Description:
;		Given a base address, returns the hardware port number (as
;	far as the VCD is concerned). (COM1 = 1, COM2 = 2 etc.).
; Entry:
;	EAX = Base port
; Exit:
;	NC if OK, EAX = port number
;	CY if unknown port, EAX is undefined.
; Uses:
;	EAX, flags.
;==============================================================================
EndDoc
BeginProc VCD_Find_COM_Index,Service

	push	edx
	call	Find_Index
	pop	edx
	cmp	[MaxPort],eax		; NC if OK, else CY (MaxPort is OK).
	ret

EndProc VCD_Find_COM_Index

;******************************************************************************
;
; VCD_Acquire_Port_Windows_Style
;
;   DESCRIPTION:
;	DDI for port VxDs to grab a port.
;
;   ENTRY:
;	EBX = VM handle	for which to acquire the port
;	EAX = Port index
;		0 = COM1
;		1 = COM2
;		etc.
;	      OR....
;		if (ECX & 2), EAX = port Base.
;
;	ECX = Flags
;	    Bit 0 = 1 if port should be assigned regardless of current
;			    ownership
;	    Bit 1 = if EAX actually stands for Port Base.
;	    Bit 2 = mouse port.
;	    Bit 3 = if ESI = Notify proc for loss of port.
;		       EDI = refdata
;	    All other flags must be zero
;	ESI ->  if Bit 3 is not set = _PortData structure for the port
;		else = Notify proc
;	EDI =	if Bit 3 is not set = offset from start of _PortData to 
;				      flag byte which VCD can use
;				      to flag loss of port to another VM.
;		else = reference data for notify proc
;   EXIT:
;	If Carry clear port is now owned by port driver
;	 EAX = handle to acquired port
;
;       Else
;	 if Z-flag set , if port owned by a DOS VM
;	 else port doesn't exist
;
; Uses:
;
;==============================================================================
BeginProc VCD_Acquire_Port_Windows_Style,Service

	pushad

	btr	[Overrides], 1
	jc	@F
	SetFlag	[Overrides], 1
@@:

IFDEF	DEBUG
	test	ecx,NOT 0Fh
	Debug_OutNZ 'VCD_Acquire_Port_Windows_Style: incorrect flags (#ecx)'
ENDIF
	test	cl,2				; Q: EAX contains base port ?
	jz	VAPWS_index			;    N:
	call	Find_Index

VAPWS_index:
	sub	eax,1
	jb	VWS_NoPort
	cmp	eax,[MaxPort]			; Q: Port index in range ?
	jae	VWS_NoPort			;    N:
	mov	esi,[VCDTable]
	mov	esi,[esi+eax*4]
	or	esi,esi
	jz	VWS_NoPort			; Port is not valid

	cmp	[esi.VCD_IObase],0		; Q: non-zero base ?
	je	VWS_NoPort			;    N: can't assign port
	cmp	[esi.VCD_IRQN], -1		;Q: irq disabled?
	je	VWS_NoPort			;   Y: can't assign port

;
; If COM1 or COM2 and current BIOS specified base is 0 and it has been
; declared as a global port, then don't allow it to be acquired.
;
	cmp	al, 2
	jae	short @F
	TestMem	[esi.VCD_Flags], VCD_global_port ;Q: global port?
	jz	short @F			;   N:
	BEGIN_Touch_1st_Meg
	cmp	word ptr [eax*2+VCD_Comm_0_Base_Addr], 0 ; Q: COM1 or COM2 base zeroed?
	END_Touch_1st_Meg
	je	VWS_NoPort
@@:

;
; This has to be done in this order!  We first check for no current owner,
; if true, then go ahead and assign.  Then check for "forced assignment", if
; true, then jump down to see if owner is different.  If we check for "forced
; assignment" first, then we do the wrong thing on the "is owner different"
; check when the port doesn't have an owner (0 is different than cur VM handle)
;
	mov	ecx, [esi.VCD_Owner]
	jecxz	VWS_OK_To_Assign		; jump if no current owner
	inc	ecx				;Q: port owned by a VxD?
	jz	VWS_Failed			;   Y: can't steal
	dec	ecx				;   N: restore VM handle

	TestMem	[esp.Pushad_ECX], 1		;Q: forced assignment?
	jz	VWS_Not_Forced			;   N:

	add	ecx, [esi.VCD_CB_Offset]
	SetFlag	[ecx.VCD_CB_Flags], VCD_Contended ; flag failed contention
	jmp	VWS_OK_To_Assign

VWS_Not_Forced:
	cmp	[esi.VCD_Owner],ebx		;Q: attach to current owner?
	je	VWS_OK_To_Assign		;   Y: Done
	TestMem	[esi.VCD_Flags],VCD_global_port	;Q: normal contention handling?
	jnz	VWS_OK_To_Assign		;   N: steal the port
	cmp	[esi.VCD_Auto_Assign],0		;Q: INI specified auto assign?
	je	VWS_OK_To_Assign		;   Y: steal the port
	jl	VWS_PortOwned			;   N:
						;   N: INI specified timeout
	VMMCall	Get_System_Time
	sub	eax, [esi.VCD_Last_Use]
	cmp	eax, [esi.VCD_Auto_Assign]	;      Q: can we steal port?
	jb	VWS_PortOwned			;	  N: put contention dlg

.errnz	$-VWS_OK_To_Assign

VWS_OK_To_Assign:
	cmp	ebx,[esi.VCD_Owner]		; Q: VM already owns port ?
	jne	VWS_New_Owner			;    N:
	push	ebx
	xor	ebx,ebx				;    Y: clear owner
	call	VCD_Set_Owner
	pop	ebx

VWS_New_Owner:
	mov	edi,ebx
	add	edi,[esi.VCD_CB_Offset]

	mov	[edi.VCD_IER],0		; zero IER
	SetFlag	[edi.VCD_CB_Flags],VCD_CB_Windows_Port
	TestMem	[esp.Pushad_ECX],4		; Q: Mouse port ?
	jz	@F
	xor	[edi.VCD_CB_Flags],VCD_CB_Windows_Port OR VCD_CB_Mouse_Port
@@:
	mov	eax,[esp.Pushad_ESI]	; get _PortData struct/notifyproc
	mov	ecx,[esp.Pushad_EDI]	; offset of byte to update/refdata

	TestMem	[esp.Pushad_ECX],8	; Q: special bit set ?
	jnz	VWS_Special
	cmp	[esi.VCD_Notify], 0	; Q: Does anyone own this one ?
	je	@F
	pushad
	cCall	[esi.VCD_Notify],<[esi.VCD_RefData],0>
	popad
@@:
	mov	[esi.VCD_COMDEB],eax
	add	eax,ecx
	mov	[esi.VCD_COMDEB_Flag],eax
	jmp	VWS_Notification_Handled

VWS_Special:
	mov	[esi.VCD_Notify],eax
	mov	[esi.VCD_RefData],ecx

VWS_Notification_Handled:
	mov	ebx,[esp.Pushad_EBX]
	call	VCD_Set_Owner
	mov	[esp.Pushad_EAX],esi

	popad
	ClrFlag	[overrides],1
	clc
	ret

VWS_PortOwned:
	xor	eax,eax				; set Z flag
	jmp	VWS_Failed

VWS_NoPort:
	or	ebx,ebx				; clear Z flag (EBX=VM handle)

VWS_Failed:
	popad
	pushfd
	ClrFlag	[overrides], 1
	popfd
	stc					; failed to open
	ret

EndProc VCD_Acquire_Port_Windows_Style

;******************************************************************************
;
; Validate_VCD_Handle
;
; Description:
;		Given a handle, validates it
;
; Entry:
;		EAX = handle
; Exit:
;		Z if valid, else NZ
; Uses:
;		Flags.
;==============================================================================
BeginProc VCD_Validate_Handle, PUBLIC

	push	edi
	push	ecx

	mov	edi, [VCDTable]
	mov	ecx, [MaxPort]
	test	esp, esp			; set NZ
	jecxz	vvh_done
	cld
	repne	scasd

vvh_done:
	pop	ecx
	pop	edi
	ret

EndProc VCD_Validate_Handle

;******************************************************************************
;
; VCD_Free_Port_Windows_Style
;
; DESCRIPTION:
;		We clear the Windows owner bit, so that we start reflecting
;		interrupts into the VM.
;
; ENTRY:
;	EAX = COM port handle returned from VCD_Acquire_Port_Windows_Style
;	EBX = VM handle
;
; EXIT:
;
; USES:
;==============================================================================
BeginProc VCD_Free_Port_Windows_Style,Service

	pushad

	call	VCD_Validate_Handle
	Debug_Outnz 'VCD_Free_Port_Windows_Style called with a possibly invalid COM port handle (#eax)'
	jnz	VRWS_Exit

	mov	esi,eax
	mov	edi,ebx
	add	edi,[esi.VCD_CB_Offset]
	TestMem	[edi.VCD_CB_Flags],<VCD_CB_Windows_Port OR VCD_CB_Mouse_Port>
	jz	VRWS_Exit
	ClrFlag	[edi.VCD_CB_Flags], VCD_CB_Windows_Bits

	cmp	[esi.VCD_Owner], ebx		; Q: Attached to current owner?
	jne	VRWS_exit			;   N:

	xor	ebx,ebx				; Owner = nobody
	call	VCD_Set_Owner			; Set to no owner

VRWS_Exit:
	popad
	ret

EndProc VCD_Free_Port_Windows_Style

;******************************************************************************
;
; VCD_Steal_Port_Windows_Style
;
; Description:
;		Allow a VxD to steal a port from some other owner.
;
; Entry:
;		ESI -> COM port handle
;		EBX = VM handle
; Exit:
;		EAX = 0  => failure
;		      else Success
; Uses:
;		C style
;==============================================================================
BeginProc VCD_Steal_Port_Windows_Style,Service

	Assert_VM_Handle ebx

	mov	ecx, esp			; assume failure
	mov	eax, esi
	call	VCD_Validate_Handle
	Debug_OutNZ 'VCD_Steal_Port_Windows_Style called with invalid COM port handle (#esi)'
	jnz	VSWS_Check_Result

	call	VCD_Assign

VSWS_Check_Result:
	xor	eax,eax
	or	ecx,ecx
	jnz	VSWS_Done
	inc	eax
VSWS_Done:
	ret

EndProc VCD_Steal_Port_Windows_Style

VxD_Pageable_Code_Ends

VxD_Locked_Code_Seg

;******************************************************************************
;
;   VCD_Control
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

BeginProc VCD_Control

	Control_Dispatch Sys_Critical_Init,	VCD_Initialize
	Control_Dispatch Device_Init,		VCD_Device_Init
	Control_Dispatch Init_Complete, 	VCD_Init_Complete
	Control_Dispatch Create_VM,		VCD_Create_VM
	Control_Dispatch Destroy_VM,		VCD_Destroy_VM
	Control_Dispatch Set_Device_Focus,	VCD_Set_Focus
	Control_Dispatch VM_Suspend,		VCD_Suspend_VM
	Control_Dispatch GET_CONTENTION_HANDLER, VCD_Get_Contention_Handler
IFDEF DEBUG
	Control_Dispatch Debug_Query,		VCD_Debug_Dump
ENDIF
	clc
	ret

EndProc VCD_Control

VxD_Locked_Code_Ends

VxD_Pageable_Code_Seg

;******************************************************************************
;
;   VCD_PM_Svc_Call - Calls from Windows Control Panel
;
;  ENTRY:
;	EBP -> client frame
;	EBX = VM handle that called
;	Client_DX = function #
;
;  EXIT:
;	Dispatches to correct call
;
;******************************************************************************

BeginProc VCD_PM_Svc_Call, RARE

	VMMcall Test_Sys_VM_Handle
IFDEF DEBUG
	jz	short VMCVMDC10
	debug_out "VCD_PM_Svc_Call not from SYS VM"
VMCVMDC10:
ENDIF
	jnz	short VCDCall_Bad2

	movzx	eax, [ebp.Client_DX]		; Get function number

	cmp	eax, Max_VCD_PM_Service
	jae	SHORT VCDCall_Bad
VMDACall_OK:
	ClrFlag	[ebp.Client_EFLAGS], CF_Mask	; Clear carry
	Call	VCD_PM_Call_Table[eax*4]	; Call appropriate code
	ret

VCDCall_Bad:
IFDEF DEBUG
	Debug_Out "VCD ERROR: Invalid function #EAX on VCD_PM_Svc_Call"
ENDIF
VCDCall_Bad2:
	mov	[ebp.Client_EAX], 0FFFFFFFFh

VCD_PM_API_Failed:
	SetFlag	[ebp.Client_EFLAGS], CF_Mask	; Set carry
	ret

EndProc VCD_PM_Svc_Call

;******************************************************************************
;
;   VCD_PM_Get_Version
;
;   DESCRIPTION:    Version call for PM API entry point
;
;   ENTRY:	    Client_DX = 0
;
;   EXIT:	    Client_AX = Version
;
;   USES:	    FLAGS,EAX
;
;==============================================================================
BeginProc VCD_PM_Get_Version

	VxDCall VCD_Get_Version
	mov	[ebp.Client_AX],ax
	ret

EndProc VCD_PM_Get_Version

;******************************************************************************
;
;   VCD_PM_Get_Port_Array
;
;   DESCRIPTION:    Get bit array of valid ports for PM API entry point
;
;   ENTRY:	Client_DX = 1
;
;   EXIT:	Client_AX = Bit array of valid ports
;		    Bit Set   -> Port valid
;		    Bit clear -> Port invalid
;
;		    Bit 0 -> COM1
;		    Bit 1 -> COM2
;		    Bit 2 -> COM3
;		    ...
;
;   USES:	FLAGS,EAX
;
;==============================================================================
BeginProc VCD_PM_Get_Port_Array, RARE

	xor	eax,eax
	mov	esi, [VCDtable]
	mov	ecx, [MaxPort]
	jecxz	gpa_scan_done
%OUT max of 16 ports supported in VCD_PM_Get_Port_Array
	cmp	ecx, 16
	jbe	short @F
	mov	ecx, 16
@@:
	mov	edx, 1
gpa_scan_lp:
	cmp	dword ptr [esi], 0
	je	short gpa_no_port
	or	eax, edx
gpa_no_port:
	shl	edx, 1
	add	esi, 4
	loop	gpa_scan_lp
gpa_scan_done:
	mov	[ebp.Client_AX],ax
	ret

EndProc VCD_PM_Get_Port_Array

;******************************************************************************
;
;   VCD_PM_Get_Data_Ptr
;
;   DESCRIPTION:
;
;   ENTRY:	    Client_CX = Port Index
;			0 = COM1
;			1 = COM2
;			2 = COM3
;			...
;
;   EXIT:	    If Carry clear
;			ECX -> VCD_COM_Struc
;		    Else Carry set, invalid port
;
;   USES:
;
;==============================================================================
BeginProc VCD_PM_Get_Data_Ptr, RARE

	movzx	ecx,[ebp.Client_CX]
	cmp	ecx, [MaxPort]			; Port index in range?
	jae	short VCD_gdp_bad		; No
	shl	ecx, 2
	add	ecx, [VCDtable]
	mov	ecx, [ecx]
	jecxz	short VCD_gdp_bad		; Port is not valid
	clc
	ret

VCD_gdp_bad:
	stc
	ret

EndProc VCD_PM_Get_Data_Ptr


;******************************************************************************
;
;   VCD_PM_Get_Port_Behavior
;
;   DESCRIPTION:    Get the behavior parameter for a COM port
;
;   ENTRY:	Client_DX = 2
;		Client_CX = Port Index
;		    0 = COM1
;		    1 = COM2
;		    2 = COM3
;		    ...
;
;   EXIT:	Client Carry Set
;		    ERROR
;		       port value is invalid
;		Client Carry Clear
;		     Client_BX:Client_AX = Port behavior value
;			   == -1 Always Warn on contention (no auto assign)
;			   ==  0 Never Warn on contention (auto assign)
;			   != 0 or -1, value is auto assign Idle Miliseconds
;				       auto assign after AX msec of no use
;
;   USES:	FLAGS,EAX
;
;==============================================================================
BeginProc VCD_PM_Get_Port_Behavior, RARE

	call	VCD_PM_Get_Data_Ptr
	jc	VCD_PM_API_Failed		; Port is not valid
	mov	eax,[ecx.VCD_Auto_Assign]
	mov	[ebp.Client_AX],ax
	shr	eax,16
	mov	[ebp.Client_BX],ax
VCDGPDone:
	ret

EndProc VCD_PM_Get_Port_Behavior

;******************************************************************************
;
;   VCD_PM_Set_Port_Behavior
;
;   DESCRIPTION:    Get bit array of valid ports for PM API entry point
;
;   ENTRY:	Client_DX = 3
;		Client_CX = Port Index
;		    0 = COM1
;		    1 = COM2
;		    2 = COM3
;		    ...
;		 Client_BX:Client_AX = Port behavior value
;			   == -1 Always Warn on contention (no auto assign)
;			   ==  0 Never Warn on contention (auto assign)
;			   != 0 or -1, value is auto assign Idle Miliseconds
;				    auto assign after AX msec of no use
;
;   EXIT:	Client Carry Set
;		    ERROR
;		       port value is invalid
;		       behavior value is invalid
;		Client Carry Clear
;		     Port behavior set
;
;   USES:	FLAGS,EAX
;
;==============================================================================
BeginProc VCD_PM_Set_Port_Behavior, RARE

	call	VCD_PM_Get_Data_Ptr
	jc	VCD_PM_API_Failed		; Port is not valid
	movzx	eax,[ebp.Client_BX]
	shl	eax,16
	mov	ax,[ebp.Client_AX]
	mov	[ecx.VCD_Auto_Assign],eax
VCDSPDone:
	ret

EndProc VCD_PM_Set_Port_Behavior


;******************************************************************************
;
;   VCD_PM_Acquire_Port
;
;   DESCRIPTION:
;	Private API for Windows COMM driver to grab a port.  It passes a ptr
;	to its extended Device Control Block, so ring 0 interrupt routines can
;	be used to process interrupts using the Windows data structures
;	directly.  We set a flag in VCD_Flags which VCD_Int checks.  When
;	VCD_Int determines that the port is owned by Windows, then it passes
;	control to the interrupt handler in VCDInt.asm.
;
;   ENTRY:
;	EBX = VM handle
;	Client_DX = 4
;	Client_CX = Port index
;		    0 = COM1
;		    1 = COM2
;		    etc.
;	Client_AX = Flags
;		    Bit 0 = 1 if port should be assigned regardless of current
;			    ownership
;		    Bit 1 = 1 if port has a ring0 callable driver (3.1 driver)
;		    Bit 2 = 1 if port being opened by mouse driver to check for
;				existance of mouse (in this case the client
;				registers DS, SI & DI are ignored and it is
;				assumed that the mouse driver is handling the
;				interrupt.)
;		    All other flags must be zero
;	Client_DS:SI -> Windows COMDEB structure for the port
;	Client_DI = offset from start of COMDEB to flag byte which VCD can use
;		      to flag loss of port to another VM (3.1 driver only)
;
;   EXIT:
;	If Carry clear port is now owned by Windows COM driver
;	    If Bits 1 & 2 of Client_AX were clear then
;		ES:DI -> call back to get transmitting going
;	    Else if Bit 1 was set then
;		CX:EDI -> ring 0 routine in VCD that should be called for
;			    every hardware int so that VCD can detect idleness
;			    of the COM port
;		EBX = reference data that should be passed to VCD in ESI when
;		      calling CX:EDI
;	    AX = base I/O port
;	    DX = irq #
;	else Carry set in Client_Flags, if failed
;	    Z-flag set in Client_Flags, if port owned by a DOS VM
;	    else port doesn't exist
;
;   USES:
;
;==============================================================================
BeginProc VCD_PM_Acquire_Port, RARE

	Client_Ptr_Flat	esi, ds, si		; COMDEB
	movzx	edi, [ebp.Client_DI]		; offset in COMDEB
	movzx	ecx, [ebp.Client_AX]		; flags
	btr	ecx, 1				; ring 0 callable driver
	jnc	VCD_PM_Api_Failed
	movzx	eax, [ebp.Client_CX]		; port #
	inc	eax
	ClrFlag	[Overrides], 1
	SetFlag	[Overrides], 2
	VxDCall	VCD_Acquire_Port_Windows_Style
	jc	VCD_PM_Acquire_Port_Failed
	mov	[ebp.Client_CX], cs
	mov	[ebp.Client_EDI], OFFSET32 VCD_COMMDRV_Int
	mov	[ebp.Client_EBX], eax
	mov	ecx, [eax.VCD_IOBase]
	movzx	edx, [eax.VCD_IRQn]
	mov	[ebp.Client_AX], cx
	mov	[ebp.Client_DX], dx
	ret

VCD_PM_Acquire_Port_Failed:
	jz	@F
	ClrFlag	[ebp.Client_Flags], ZF_Mask
	jmp	VCD_PM_API_Failed
@@:
	SetFlag	[ebp.Client_Flags], ZF_MASK
	jmp	VCD_PM_API_Failed

EndProc VCD_PM_Acquire_Port


;******************************************************************************
;
;   VCD_PM_Free_Port
;
;   DESCRIPTION:
;	Private API for Windows COMM driver to release a port.	We clear the
;	Windows owner bit, so that we stop processing interrupts for this port
;	(we reflect them into the VM instead.)
;
;   ENTRY:
;	EBX = VM handle
;	Client_DX = 5
;	Client_CX = Port index
;		    0 = COM1
;		    1 = COM2
;		    etc.
;
;   EXIT:
;
;   USES:
;
;==============================================================================
BeginProc VCD_PM_Free_Port, RARE

	call	VCD_PM_Get_Data_Ptr
	jc	@F
	mov	eax, ecx
	VxDCall	VCD_Free_Port_Windows_Style
@@:
	ret

EndProc VCD_PM_Free_Port


;******************************************************************************
;
;   VCD_PM_Steal_Port
;
;   DESCRIPTION:
;	Private API for Windows COMM driver to release a port.	We clear the
;	Windows owner bit, so that we stop processing interrupts for this port
;	(we reflect them into the VM instead.)
;
;   ENTRY:
;	EBX = VM handle
;	Client_DX = 6
;	Client_CX = Port index
;		    0 = COM1
;		    1 = COM2
;		    etc.
;
;   EXIT:
;	Client_AL = -1, if successful, else 0
;
;   USES:
;
;==============================================================================
BeginProc VCD_PM_Steal_Port, RARE

	mov	[ebp.Client_AL], 0
	jmp	VCD_PM_API_Failed		; Port is not valid

EndProc VCD_PM_Steal_Port


;******************************************************************************
;
;   VCD_Set_Focus
;
;   DESCRIPTION:    Specify the current owner of a COM port, so that IRQ
;		    interrupts will be simulated into the appropriate VM.
;
;   ENTRY:	    EBX = Handle of VM to receive the keyboard focus
;		    EDX = Device ID or 0 if critical device focus call
;		    ESI = port #
;
;   EXIT:	    Carry clear
;
;   USES:	    flags
;
;==============================================================================
BeginProc VCD_Set_Focus

	Assert_VM_Handle ebx
	pushad
	cmp	edx, VCD_Device_ID
	jne	short not_for_VCD

	cmp	esi, [MaxPort]
	ja	short setf_exit 	    ; return Carry set, if port # > max
	sub	esi, 1
	jb	short setf_exit 	    ; return Carry set, if port # < 1

	shl	esi, 2
	add	esi, [VCDtable]
	mov	esi, [esi]
	or	esi, esi			    ;Q: port valid?
	jz	short not_for_VCD		    ;	N: ignore call

	mov	edx, ebx
	TestMem	[esi.VCD_Flags], VCD_global_port    ;Q: global port?
	jz	short assign_owner		    ;	N:
						    ;	Y: set new owner VM handle

	mov	ebx, [esi.VCD_Owner]
	or	ebx, ebx			    ;Q: port owned?
	jz	short assign_owner		    ;	N: set new owner
	inc	ebx				    ;Q: port owned by a VxD?
	jz	short setf_exit 		    ;	Y: exit
	dec	ebx				    ;	N: restore VM handle

	TestMem	[esi.VCD_Flags], VCD_Owns_IRQ	    ; Q: Does COM own the IRQ?
	jz	SHORT assign_owner		    ;	 N: Can't do anything
	mov	eax, [esi.VCD_IRQ_Handle]
	VxDCall VPICD_Get_Complete_Status
	TestReg	ecx, VPICD_Stat_Virt_Req	    ;Q: int requested for old owner?
	jz	short assign_owner		    ;	N: set new owner
	VxDCall VPICD_Clear_Int_Request 	    ; Clear request
	mov	ebx, edx
	VxDCall VPICD_Set_Int_Request		    ; Request int in new owner

assign_owner:
	mov	ebx, edx
	call	VCD_Set_Owner

setf_exit:
not_for_VCD:
	clc
	popad
	ret

EndProc VCD_Set_Focus

;******************************************************************************
;
;   VCD_Suspend_VM
;
;   DESCRIPTION:    Clear pending COM interrupt, if VM owns the port
;
;   ENTRY:	    EBX = current VM
;
;   EXIT:
;
;   USES:
;
;==============================================================================
BeginProc VCD_Suspend_VM

	mov	edi, [VCDtable]
	mov	ecx, [MaxPort]
	jecxz	suspend_done

suspend_lp:
	push	ecx
	mov	esi, [edi]
	or	esi, esi		;Q: COM port exists?
	jz	short suspend_next	;   N:
	cmp	[esi.VCD_Owner], ebx	;Q: VM owns COM port?
	jne	short suspend_next	;   N:

	TestMem	[esi.VCD_Flags], VCD_Owns_IRQ	; Q: Does COM own the IRQ?
	jz	SHORT suspend_next		;    N: Can't do anything
	mov	eax, [esi.VCD_IRQ_Handle]
	call	VCD_Clear_Int		; clear any outstanding int

suspend_next:
	add	edi, 4
	pop	ecx
	loop	suspend_lp

suspend_done:
	clc
	ret

EndProc VCD_Suspend_VM


;******************************************************************************
;
;   VCD_Set_IRQ_Procs
;
;   DESCRIPTION:
;
;   ENTRY:
;	ESI -> VCD_COM_Struc
;
;   EXIT:
;
;   USES:
;	Flags
;
;==============================================================================
BeginProc VCD_Set_IRQ_Procs

	push	eax
	mov	eax, [VCD_Def_Int_Proc]
	mov	[esi.VCD_Hw_Int_Proc], eax
	mov	[esi.VCD_Virt_Int_Proc], OFFSET32 VCD_Ignore
	mov	eax, [VCD_Def_EOI_Proc]
	mov	[esi.VCD_EOI_Proc], eax
	mov	[esi.VCD_Mask_Change_Proc], OFFSET32 VCD_Mask
	mov	[esi.VCD_IRET_Proc], OFFSET32 VCD_Ignore
	pop	eax

	TestMem	[esi.VCD_Flags], VCD_Virtualized    ;Q: virtualized?
	jz	short VCD_Ignore		    ;	N:
IFDEF test_combuff_handling_mouse
	TestMem	[esi.VCD_Flags], VCD_Windows_Port
ELSE
	TestMem	[esi.VCD_Flags], <VCD_global_port OR VCD_Windows_Port OR VCD_Mouse_Port>
ENDIF
	jnz	short VCD_Ignore		    ; jump if global or windows

	pushad
	mov	ecx, 5
	mov	edi, [esi.VCD_virt_procs]
	lea	edi, [edi.VPS_Hw_Int_Proc]
	lea	ebx, [esi.VCD_Hw_Int_Proc]
irq_chk_lp:
	mov	eax, [edi]
	or	eax, eax		    ;Q: call back defined?
	jz	short no_irq_proc	    ;	N:
	mov	[ebx], eax		    ;	Y: save call back ptr
no_irq_proc:
	add	edi, 4
	add	ebx, 4
	loop	irq_chk_lp

	mov	edi, [esi.VCD_virt_procs]
	cmp	[edi.VPS_EOI_Proc], 0
	jne	short eoi_hooked
	mov	[esi.VCD_EOI_Proc], OFFSET32 VCD_Ignore
eoi_hooked:
	popad

VCD_Ignore:
	ret

EndProc VCD_Set_IRQ_Procs


;******************************************************************************
;
;   VCD_Notify_VxD
;
;   DESCRIPTION:
;
;   ENTRY:
;	ESI -> VCD_COM_Struc
;	EBX = VM Handle, or 0
;	EDX = VM handle or previous owner, or 0
;
;   EXIT:
;
;   USES:
;	Flags
;
;==============================================================================
BeginProc VCD_Notify_VxD

IFDEF DEBUG
	test	ebx, ebx
	jz	SHORT vnv_no_handle
	Assert_VM_Handle ebx
vnv_no_handle:
ENDIF
	TestMem	[esi.VCD_Flags], VCD_Virtualized    ;Q: virtualized?
	jz	short no_notify 		    ;	N:
IFNDEF test_combuff_handling_mouse
	TestMem	[esi.VCD_Flags], VCD_Mouse_Port     ;Q: mouse owns port?
	jnz	short no_notify 		    ;	Y:
ENDIF

	pushad
	mov	eax, VCD_Control_Set_Owner
	mov	edi, [esi.VCD_virt_procs]
	call	[edi.VPS_Control_Proc]
	popad

no_notify:
	ret

EndProc VCD_Notify_VxD



;******************************************************************************
;******************************************************************************
;******************************************************************************
;*************************************
; IN: ESI = COM data structure
;     EBX = VM Handle
;*****************************************

BeginProc VCD_Enable_Trapping

	pushad
	mov	edx, [esi.VCD_IObase]
	mov	ecx, UART_PORTS
VCDET_Loop:
	VMMcall Enable_Local_Trapping
	inc	edx
	loopd	VCDET_Loop
	popad
	ret

EndProc VCD_Enable_Trapping


BeginProc VCD_Disable_Trapping

	pushad
	mov	edx, [esi.VCD_IObase]
	mov	ecx, UART_PORTS
	mov	ebp, [VCD_Disable_Mask] ; 1 = port to disable, 0 = port to ignore
	TestMem	[esi.VCD_Flags], <VCD_Windows_Port OR VCD_Mouse_Port>
	jz	short VCDDT_Loop
	or	ebp, -1 		; disable everything

VCDDT_Loop:
	shr	ebp, 1
	jnc	SHORT VCDDT_ignore
	VMMcall Disable_Local_Trapping
VCDDT_ignore:
	inc	edx
	loopd	VCDDT_Loop
	popad
	ret

EndProc VCD_Disable_Trapping


;******************************************************************************
;
;   NAME:
;	VCD_Create_VM
;
;   DESCRIPTION:
;	This routine is called whenever a new VM is created.  It copies the
;	default register values for all COM ports into the VMs control block.
;
;   ENTRY:
;	EBX = Handle of VM being created
;
;   EXIT:
;	Carry flag set if error
;
;   USES:
;	ECX, ESI, EDI, Flags
;
;------------------------------------------------------------------------------

BeginProc VCD_Create_VM

	mov	ecx, [MaxPort]
	jecxz	init_vm_cb_done

	mov	esi, [VCDtable]
init_CB_port_info:
	push	esi
	mov	esi, [esi]			; ESI -> COM1's data structure
	call	VCD_Init_VM_CB			; Initialize the control block
	pop	esi
	add	esi, 4
	loop	init_CB_port_info

init_vm_cb_done:
        clc
        ret

EndProc VCD_Create_VM


;******************************************************************************
;
;   NAME:
;	VCD_Init_VM_CB
;
;   DESCRIPTION:
;
;   ENTRY:
;	EBX = Handle of VM being created
;	ESI -> COM data structure
;
;   USES:
;	ESI, EDI, Flags
;
;------------------------------------------------------------------------------

BeginProc VCD_Init_VM_CB

	test	esi, esi			; Q: Does COM exist?
	jz	SHORT VCD_IVB_Exit		;    N: Just return
						;    Y: Copy info into CB

	call	FillBiosArea		; Update the bios data area

	mov	edi, [esi.VCD_CB_Offset]	; EDI = Offset in control block
	lea	edi, [edi][ebx.VCD_BAUD_LSB]	; EDI -> Destination address
	TestMem	[esi.VCD_Flags], VCD_global_port ;Q: global port?
	jz	short ivcb_use_defaults 	;   N:
	push	ebx
	mov	ebx, [esi.VCD_Owner]
	inc	ebx				;Q: port owned by a VxD?
	jz	short ivcb_use_sys_VM		;   Y: use SYS VM Handle
	dec	ebx				;   N: restore ebx
	VMMCall Test_Sys_VM_Handle
	jne	short ivcb_use_sys_VM
	push	edi
	lea	edi, [esi.VCD_Def_BAUD_LSB]
	push	eax
	push	edx
	call	VCD_Read_State
	pop	edx
	pop	eax
	pop	edi
	pop	ebx
	jmp	short ivcb_use_defaults

ivcb_use_sys_VM:
	VMMCall Get_Sys_VM_Handle
	add	ebx, [esi.VCD_CB_Offset]
	lea	esi, [ebx.VCD_BAUD_LSB]
	pop	ebx
	jmp	short ivcb_do_copy

ivcb_use_defaults:
	lea	esi, [esi.VCD_Def_BAUD_LSB]	; ESI -> Origin of copy
ivcb_do_copy:
	cld
	movsd					; Copy first 4 bytes
	movsb					; Copy 5th byte
.errnz	VCD_Read_Stat - 5
	mov	BYTE PTR [edi], 0		; Reset Read Status byte

VCD_IVB_Exit:
	ret

EndProc VCD_Init_VM_CB

;******************************************************************************
;
; FillBiosArea
;
; Description:
;		For ports marked as dynamic, update the BDA for this VM.
; Entry:
;		EBX = VM handle
;		ESI -> COM struc
; Exit:
;		None
; Uses:
;		EDI, Flags
;==============================================================================

BeginProc FillBiosArea

	TestMem	[esi.VCD_Flags], VCD_Dynamic_Port	; Q: Dynamic port ?
	jz	FBA_Done				;    N:

	SaveReg	<eax>

	movzx	eax, [esi.VCD_Number]			; Q: COM # <= 4 ?
	cmp	al, 4
	ja	@F					;    N:
	mov	edi, [ebx.CB_High_Linear]
	lea	edi, [edi+eax*2-2+400h]

	cmp	WORD PTR [edi], 0
	jne	@F
	mov	eax, [esi.VCD_IOBase]
	mov	WORD PTR [edi], ax
@@:
	RestoreReg <eax>

FBA_Done:
	ret

EndProc FillBiosArea

;******************************************************************************
;
;   VCD_Clear_Int
;
;   DESCRIPTION:    Check to see if int is requested for owner VM.  If requested
;		    then service the int by reading the require COM port
;		    registers, clear the int request and EOI the int.
;
;   ENTRY:	    EAX = IRQ handle
;		    EBX = Current COM owner
;		    ESI = Com data structure
;
;   EXIT:	    nothing
;
;   USES:	    ECX
;
;==============================================================================
BeginProc VCD_Clear_Int

	VxDCall VPICD_Get_Complete_Status
	TestReg	ecx, VPICD_Stat_Virt_Dev_Req	;Q: we requested int?
	jz	SHORT nothing_to_clear		;   N:
	VxDCall VPICD_Clear_Int_Request 	; Clear request
	call	VCD_Eat_Interrupt
	VxDCall VPICD_Phys_EOI			; throw int away
nothing_to_clear:
	ret

EndProc VCD_Clear_Int


;******************************************************************************
;
;   NAME:
;	VCD_Attach
;
;   DESCRIPTION:
;	This procedure will initialize a physical COM port to the virtual
;	state of the VM.
;
;   ENTRY:
;	EBX = Handle of new owner
;	[ESI] = Com data structure
;	ECX = Offset in control block of com's control block
;	interrupts disabled
;
;   EXIT:
;
;   USES:
;	Flags
;
;   ASSUMES:
;	EBX contains a valid VM handle (is not 0)
;
;------------------------------------------------------------------------------

BeginProc VCD_Attach

	Assert_VM_Handle ebx			; Debugging

	pushad

IFDEF DEBUG
	mov	al, [esi.VCD_Number]
	Queue_Out 'VCD_Attach COM#al to #ebx'
ENDIF

	TestMem	[esi.VCD_Flags], VCD_Virtualized ;Q: virtualized?
	jz	short va_not_virt		 ;  N: disable trapping
IFNDEF test_combuff_handling_mouse
	TestMem	[esi.VCD_Flags], VCD_global_port ;Q: global port?
	jnz	short va_not_virt		 ;  Y: disable trapping
ENDIF
	TestMem	[ebx][ecx.VCD_CB_Flags], <VCD_CB_Windows_Port OR VCD_CB_Mouse_Port>
						 ;Q: windows port?
	jz	short va_virt			 ;  N: leave trapping enabled


va_not_virt:
	call	VCD_Disable_Trapping

	TestMem	[ebx][ecx.VCD_CB_Flags], VCD_CB_Windows_Port
	jz	short va_virt
	TestMem	[ebx][ecx.VCD_CB_Flags], VCD_CB_Windows_30Drvr
	jnz	short va_virt
	push	ecx
	mov	ecx, [esi.VCD_Notify]	; Q: Should we tell someone ?
	jecxz	va_nonotify			;    N:
	cCall	ecx,<[esi.VCD_RefData],1>	;    Y: we return port here.
	pop	ecx
	jmp	va_virt

va_nonotify:
	pop	ecx
	mov	eax, [esi.VCD_COMDEB_Flag]	; if 3.1 COMM.DRV, then
	or	eax, eax
	jz	short va_virt
	and	byte ptr [eax], NOT fCOM_ignore_ints ;clear flag so COMM.DRV
						;   will handle ints again
va_virt:
	TestMem	[esi.VCD_Flags], VCD_Owns_IRQ	; Q: Does COM own the IRQ?
	jz	SHORT attach_skip_mask		;    N: Can't do anything
	mov	eax, [esi.VCD_IRQ_Handle]
	VxDCall VPICD_Physically_Mask
attach_skip_mask:

	mov	[esi.VCD_Owner], ebx		; Set new owner

	add	ecx, ebx			; [ECX] = VC Control Block

	mov	edx, [esi.VCD_IObase]		; base of IO regs
	add	edx, UART_LCR			; DX= Line Control Register
	mov	al,LCR_DLAB			; Set Divisor Latch Access Bit
	out	dx,al				; Set access to BAUD Divisor
	IO_Delay
	add	edx, (UART_BAUD_LSB - UART_LCR)
	mov	al,[ecx].VCD_BAUD_LSB		; AL = LSB of virtual BAUD
	out	dx,al				; Set LSB of BAUD
	IO_Delay
	inc	edx
	mov	al,[ecx].VCD_BAUD_MSB		; AL = MSB of virtual BAUD
	out	dx,al				; Set MSB BAUD
	IO_Delay
	add	edx, (UART_LCR - UART_BAUD_MSB) ; DX = Line Control Register
	mov	al, [ecx].VCD_LCR		; AL = LCR value for this VM
	out	dx,al				; Enable access to IER and RXB
	IO_Delay
	inc	edx
	.errnz	UART_MCR - UART_LCR - 1 	; DX = Modem Control Port
	mov	al,[ecx].VCD_MCR		; AL = Virtual Modem Ctrl value
	out	dx,al				; Set Modem Control Register
	IO_Delay

;
; copy Windows ownership flags from VCD_CB_Struc into VCD_COM_Struc
;
	movzx	eax, [ecx.VCD_CB_Flags]
	and	eax, VCD_CB_Windows_Bits
	or	[esi.VCD_Flags], ax

	add	edx, (UART_IER - UART_MCR)	; DX = Interrupt Enable Reg
	mov	al,[ecx].VCD_IER		; AL = IER value
	out	dx,al				; Set IER
	IO_Delay

	call	VCD_Set_IRQ_Mask

	popad
	ret

EndProc VCD_Attach

;******
;
; VCD_Set_IRQ_Mask
;
; Description:
;	If a VxD is waiting to own the port, unmask the IRQ, else call
;	Set_Auto_Masking.
; Entry:
;	ESI -> VCD_COM_Struc
; Exit:
;	None
; Uses:
;	EAX
;
BeginProc VCD_Set_IRQ_Mask

	TestMem	[esi.VCD_Flags], VCD_Owns_IRQ	; Q: Does COM own the IRQ?
	jz	skip_set_mask			;    N: Can't do anything
	mov	eax, [esi.VCD_IRQ_Handle]
	cmp	[esi.VCD_Notify],0		; Q: pending VxD owner ?
	je	@F				;    N:
	VxDJmp	VPICD_Physically_Unmask
@@:
	VxDJmp VPICD_Set_Auto_Masking

skip_set_mask:

	ret

EndProc VCD_Set_IRQ_Mask

;******************************************************************************
;
;   VCD_Read_State
;
;   DESCRIPTION:    Read the current state of the COM port into a VCD_CB_Struc.
;
;   ENTRY:
;	[ESI] = Com data structure
;	EDI -> CB struc to read data into
;
;   EXIT:	    none
;
;   USES:	    EAX, EDX, Flags
;
;==============================================================================
BeginProc VCD_Read_State

	mov	edx, [esi].VCD_IObase		; DX = Base of IO regs for port
	add	edx, UART_LCR			; DX = Line Control Register
	in	al, dx
	mov	[edi].VCD_LCR, al		; save LCR value for this VM
	or	al, LCR_DLAB			; Set Div Latch Access bit
	out	dx, al				; Set access to Divisor Latch
	IO_Delay
	add	dl, (UART_BAUD_LSB - UART_LCR)
	in	al, dx				; AL = LSB of BAUD
	mov	[edi].VCD_BAUD_LSB, al		; Save LSB of BAUD
	IO_Delay
	inc	edx
	in	al, dx				; AL = MSB of BAUD
	IO_Delay
	mov	[edi].VCD_BAUD_MSB, al		; Save MSB of BAUD
	add	dl, (UART_LCR - UART_BAUD_MSB)	; DX = LCR port
	mov	al, [edi].VCD_LCR		; AL = LCR value for this VM
	out	dx, al				; Set LCR back to proper value

	sub	dl, (UART_LCR - UART_IER)
	in	al, dx
	IO_Delay
	mov	[edi].VCD_IER, al
	add	dl, (UART_MCR - UART_IER)
	in	al, dx
	mov	[edi].VCD_MCR, al
	ret

EndProc VCD_Read_State


;******************************************************************************
;
;   VCD_Detach - Detach COM port from current owner
;
;   ENTRY:
;	[ESI] = Com data structure
;	[ESI.VCD_Owner] = Current COM owner
;	ECX = Offset in control block of com's control block
;	Interrupts are disabled
;
;   EXIT:
;	Port will not be assigned to any VM (Owner will = 0)
;
;   ASSUMES:
;	This procedure will ONLY BE CALLED BY VCD_SET_OWNER
;
;------------------------------------------------------------------------------

BeginProc VCD_Detach

	cmp	[esi.VCD_Owner], 0		; Q: Does any VM own this port?
	jz	short VCD_D_Exit		;    N: Nothing to do here
	cmp	[esi.VCD_Owner], -1		;Q: port owned by a VxD?
	jz	short VCD_D_Exit		;   Y: nothing to do here

	pushad

	mov	ebx, [esi.VCD_Owner]

IFDEF DEBUG
	mov	al, [esi.VCD_Number]
	Queue_Out 'VCD_Detach COM#al from #ebx'
ENDIF

	mov	edi, ebx			; EDI = Old owner's handle
	add	edi, ecx			; [EDI] = Offset to CB

;
; Check in the VCD data structure instead of in the Control block
; to make sure that we are dealing with the flags for the COM port as
; it is currently owned.
;
	TestMem	[esi.VCD_Flags], VCD_Windows_Port
	jz	short @F
	TestMem	[esi.VCD_Flags], VCD_Windows_30Drvr
	jnz	short @F
	mov	ecx, [esi.VCD_Notify]
	jecxz	vd_nonotify
	cCall	ecx,<[esi.VCD_RefData],0>	; inform about loss of port.
	jmp	@F

vd_nonotify:
	mov	eax, [esi.VCD_COMDEB_Flag]
	or	eax, eax
	jz	short @F
	or	byte ptr [eax], fCOM_ignore_ints
@@:
	call	VCD_Enable_Trapping

	call	VCD_Read_State
	mov	edx, [esi].VCD_IObase		; DX = Base of IO regs for port
	inc	edx
.errnz UART_IER-1
	xor	al, al
	out	dx, al				; disable port interrupts
	IO_Delay
	inc	edx				; disable FIFO also
.errnz UART_IIR - UART_IER - 1
	out	dx, al

	xor	ebx, ebx			; EBX = 0
	xchg	ebx, [esi.VCD_Owner]
	ClrFlag	[esi.VCD_Flags], VCD_CB_Windows_Bits
	popad

VCD_D_Exit:
	ret

EndProc VCD_Detach

;******************************************************************************
;
;   VCD_Virtualize_IRQ
;
;   DESCRIPTION:
;
;   ENTRY:
;	EBX = Handle of VM
;	ESI -> COM data structure
;
;   EXIT:
;
;   USES:	EAX, EDI, Flags
;
;==============================================================================
BeginProc VCD_Virtualize_IRQ

	TestMem	[esi.VCD_Flags], VCD_IRQ_Init	; Q: Has IRQ been initialized?
	jnz	VCD_IRQ_Initialized		;    Y: Attach to new owner
	push	ecx				;    N: Virtualize IRQ

	sub	esp, SIZE VPICD_IRQ_Descriptor
	mov	edi, esp			; edi -> VPICD_IRQ_Descriptor

	movzx	eax, [esi.VCD_IRQN]
	mov	[edi.VID_IRQ_Number], ax
	xor	ecx, ecx
	TestMem	[esi.VCD_Flags], VCD_not_sharable_IRQ
	jnz	short vi_IRQ_not_sharable
	SetReg	ecx, VPICD_Opt_Can_Share
vi_IRQ_not_sharable:
	mov	[edi.VID_Options], cx

;
; set PIC virtualization procs to point to thunks in header of VCD_COM_Struc
;
offset_to_previous_thunk = VCD_HwInt_Thunk - VCD_VirtInt_Thunk
.errnz VCD_VirtInt_Thunk - VCD_EOI_Thunk  - offset_to_previous_thunk
.errnz VCD_EOI_Thunk	 - VCD_Mask_Thunk - offset_to_previous_thunk
.errnz VCD_Mask_Thunk	 - VCD_IRET_Thunk - offset_to_previous_thunk

	lea	eax, [esi.VCD_HwInt_Thunk]
	lea	edi, [edi.VID_Hw_Int_Proc]
	cld
	mov	ecx, Virt_PIC_procs		; # of proc addrs to init
vi_set_procs:
	stosd
	sub	eax, offset_to_previous_thunk
	loop	vi_set_procs

	mov	edi, esp
	mov	[edi.VID_IRET_Time_Out], 500	; VID_IRET_Time_Out = 500

	xor	eax, eax
	mov	ecx, [esi.VCD_virt_procs]	;Q: port virtualized?
	jecxz	vi_call_VPICD			;   N:
	cmp	[ecx.VPS_Virt_Int_Proc], eax	;Q: virtualizer wants virt ints?
	jne	short vi_w_virt_int		;   Y:
	mov	[edi.VID_Virt_Int_Proc], eax	;   N: zero virt int adr for VPICD
vi_w_virt_int:
	cmp	[ecx.VPS_IRET_Proc], eax	;Q: virtualizer wants irets?
	jne	short vi_call_VPICD		;   Y:
	mov	[edi.VID_IRET_Proc], eax	;   N: zero iret adr for VPICD
vi_call_VPICD:

IFDEF DEBUG
	mov	al, [esi.VCD_number]
	movzx	ecx, [edi.VID_IRQ_Number]
	Queue_Out "Virtualizing IRQ#al for COM#bl",ecx,eax
ENDIF
	VxDCall VPICD_Virtualize_IRQ		; Virtualize the sucker
	jc	SHORT VCD_Virtualize_Error 	; If error don't save handle
	mov	[esi.VCD_IRQ_Handle], eax	; Save the IRQ handle
	SetFlag	[esi.VCD_Flags], VCD_Owns_IRQ	; The COM port owns it's IRQ

; There may be an int. request lying around. If so, transfer it to
; owner of virtualized int.
	VxDCall VPICD_Get_Complete_Status	; Check status of int.
	TestReg	ecx, VPICD_Stat_Phys_In_Serv	; Q: Is it phys. in Service?
	jz	SHORT @F			;    N:
	VxDCall VPICD_Set_Int_Request		;    Y: pass int to new owner

@@:
	TestMem	[esi.VCD_Flags],VCD_Not_Sharable_IRQ	; Q: IRQ sharable?
	jnz	@F				;   N: cannot be global
	TestReg	ecx,VPICD_STAT_PHYS_MASK	;Q: Was it masked ?
	jnz	@F				;   Y:
	SetFlag	[esi.VCD_Flags], VCD_Global_IRQ	;   N: Say that IRQ is global.
@@:
	movzx	eax, [esi.VCD_IRQN]
	bts	[VCD_IRQs_virtualized], eax	;Q: first IRQ is virtualized?
	jc	short VCD_IRQ_Init_Done 	;   N:
	SetFlag	[esi.VCD_Flags], VCD_1st_on_IRQ ;   Y: flag it
IFDEF DEBUG
	jmp	SHORT VCD_IRQ_Init_Done
ENDIF

VCD_Virtualize_Error:        
IFDEF DEBUG
	mov	al, [esi.VCD_number]
	movzx	ecx, [esi.VCD_IRQN]
	Trace_Out "VCD: could not virtualize IRQ#cl for COM#al"
ENDIF
VCD_IRQ_Init_Done:
	SetFlag	[esi.VCD_Flags], VCD_IRQ_Init	; IRQ has been initialized
	add	esp, SIZE VPICD_IRQ_Descriptor
	pop	ecx
VCD_IRQ_Initialized:
	ret

EndProc VCD_Virtualize_IRQ


page
;******************************************************************************
;
;   VCD_Set_Owner_Event
;
;   DESCRIPTION:
;
;   ENTRY:	    EDX -> COM data structure
;
;   EXIT:
;
;   USES:
;
;==============================================================================
BeginProc VCD_Set_Owner_Event

	mov	esi, edx
	xor	ebx, ebx
	xchg	ebx, [esi.VCD_Owner]
	call	VCD_Set_Owner
	ClrFlag	[esi.VCD_Flags], VCD_SetOwner_Event
	ret

EndProc VCD_Set_Owner_Event


;******************************************************************************
;
;   NAME:
;	VCD_Set_Owner
;
;   DESCRIPTION:
;	~~~~~
;
;   ENTRY:
;	EBX = Handle of VM to assign port to (0 for no owner)
;	ESI -> COM data structure
;
;------------------------------------------------------------------------------
BeginProc VCD_Set_Owner

IFDEF DEBUG					; Debugging
	test	ebx, ebx
	jz	SHORT VCD_SO_Handle_OK
	Assert_VM_Handle ebx
VCD_SO_Handle_OK:
ENDIF

	pushad					; Save registers

	test	esi, esi			; Q: Does port even exist?
	jz	End_VCD_Set_Owner		;    N: Nothing to do

	mov	ecx, [esi.VCD_CB_Offset]	; ECX = Offset in Control Blk

	cmp	ebx, [esi].VCD_Owner		; Q: Set to same owner?
	je	End_VCD_Set_Owner_wTO		;    Y: set use timeout
;
;   Detach COM from current owner
;
	push	[esi.VCD_Owner] 		; save previous owner
	call	VCD_Detach

;
;   Test for no owner & reset if necessary
;
	or	ebx, ebx			; Q: Any new owner?
	jnz	VCD_Set_New_Owner		;    Y: Set up port for VM
						;    N: Reset port to default
	mov	edx, [esi.VCD_IObase]		; Get port address

	TestMem	<DWORD PTR [esp+4]>, IF_Mask	; Q: ints originally enabled?
	jnz	short @F			;   Y:
	inc	edx
.errnz UART_IER-1
	xor	al, al
	out	dx, al				; disable all port ints
	IO_Delay
	add	dl, UART_MCR-UART_IER
	in	al, dx
	IO_Delay
	and	al, MCR_DTR+MCR_RTS		;Leave DTR, RTS high if already so
	out	dx, al				;  but tri-state IRQ line
	jmp	detach_complete

@@:
	add	dl, UART_MCR-UART_THR		;--> Modem Control Register
	in	al, dx
	test	al, MCR_INTEN			;Q: ints enabled?
	jz	detach_complete 		;   N:
;
;   When the OUT2 bit is reset to 0 to disable interrupts, many ports
;   generate an interrupt which can not be identified, because the the
;   interrupt ID register will not be set.  To work around this hardware
;   problem we first mask the IRQ, then set the port into loopback mode
;   and output a NULL to generate a receive interrupt request.	Then we
;   reset OUT2 and unmask the IRQ.  This will cause the interrupt to occur
;   and the interrupt handler will be able to correctly identify the
;   interrupt as coming from the com port.
;
	dec	ebx
	mov	[esi.VCD_Owner], ebx		; Set owner to -1
	inc	ebx
	TestMem	[esi.VCD_Flags], VCD_Owns_IRQ	; Q: Does COM own the IRQ?
	jz	SHORT @F			;    N: Can't do anything
	mov	eax, [esi.VCD_IRQ_Handle]
	VxDCall VPICD_Physically_Mask
@@:
	mov	eax, [VCD_Def_Int_Proc] 	; don't pass loopback receive
	mov	[esi.VCD_Hw_Int_Proc], eax	;   int to any other vxd

	mov	al, IER_DR			; enable only receive ints
	sub	dl, UART_MCR-UART_IER		;--> int enable register
	out	dx, al
	add	dl, UART_LSR-UART_IER		;--> line status register
	IO_Delay

@@:
	in	al, dx				;Wait until xmit is empty
	IO_Delay
	and	al, LSR_TXBITS
	cmp	al, LSR_TXBITS
	jne	@B				;Not empty yet

	dec	edx				;--> Modem Control Register
.errnz UART_LSR - UART_MCR - 1
	in	al, dx
	IO_Delay
	mov	ah, al
	or	al, MCR_Loopback		; turn on loopback
	out	dx, al
	IO_Delay
	sub	dl, UART_MCR-UART_THR
	xor	al, al
	out	dx, al				; output a NULL to generate an int
	IO_Delay
	add	dl, UART_LSR-UART_THR
@@:
	in	al, dx				;Wait until xmit is empty
	IO_Delay
	and	al, LSR_TXBITS
	cmp	al, LSR_TXBITS
	jne	@B				;Not empty yet

	dec	edx				; now clear OUT2 and loopback
.errnz UART_LSR - UART_MCR - 1
	mov	al, ah
	and	al, MCR_DTR+MCR_RTS		;Leave DTR, RTS high if already so
	out	dx, al				;  but tri-state IRQ line

	TestMem	[esi.VCD_Flags], VCD_Owns_IRQ	; Q: Does COM own the IRQ?
	jz	SHORT @F			;    N: Can't do anything
	mov	eax, [esi.VCD_IRQ_Handle]
	VxDCall VPICD_Physically_Unmask 	  ; this will cause the receive int
@@:
	sti					; to occur and be processed
	sub	dl, UART_MCR-UART_IER		; clear the receive int enable
	xor	eax, eax
	out	dx, al
	mov	[esi.VCD_Owner], eax		; Set owner to nil

	call	VCD_Set_IRQ_Mask

detach_complete:

	pop	edx				; previous owner
	call	VCD_Notify_VxD			; notify if necessary

	jmp	SHORT End_VCD_Set_Owner
;
;   Assign to new owner
;
VCD_Set_New_Owner:
	btr	DWORD PTR [Overrides],0
	jc	@F
	call	VCD_Virtualize_IRQ
@@:
	call	VCD_Attach			; Initialize the port

so_notify:
	call	VCD_Set_IRQ_Procs
	pop	edx
	call	VCD_Notify_VxD			; notify if necessary

End_VCD_Set_Owner_wTO:
	VMMCall Get_System_Time
	mov	[esi.VCD_Last_Use], eax
	call	[VCD_not_idle_proc_addr]

End_VCD_Set_Owner:
	popad					; restore registers

	ret					; Return

EndProc VCD_Set_Owner


page
;******************************************************************************
;   VCD_Destroy_VM - called when a VM is destroyed
;
;   ENTRY: Protected Mode Ring 0
;		EBX = VM handle
;
;   EXIT:  Protected Mode Ring 0
;
;   USED: 
;   STACK:
;------------------------------------------------------------------------------

BeginProc VCD_Destroy_VM

	mov	ecx, [MaxPort]
	jecxz	destroy_vm_done

	mov	esi, [VCDTable]
destroy_lp:
	push	esi
	mov	esi, [esi]			; ESI -> COM1's data structure
	call	VCD_Detach_If_Owner		; Initialize the control block
	pop	esi
	add	esi, 4
	loop	destroy_lp

destroy_vm_done:
	clc
	ret

EndProc VCD_Destroy_VM


BeginProc VCD_Detach_If_Owner

	test	esi, esi			; Q: Does port exist?
	jz	SHORT VCD_DIO_Exit		;    N: Forget it

	cmp	ebx, [esi.VCD_Owner]		; Q: Does VM own COM?
	jne	SHORT VCD_DIO_Exit		;    N: Exit
	push	ebx				;    Y: Save handle
	xor	ebx, ebx			; Owner = Nobody
	call	VCD_Set_Owner			; Set to no owner

	cmp	[esi.VCD_Notify],0	; Q: Is there a waiting owner ?
	jz	VCD_DIO_No_Waiting_Owner	;    N:
	VMMCall	Get_Sys_VM_Handle
	SaveReg	<ecx>
	call	VCD_Assign
	RestoreReg <ecx>

VCD_DIO_No_Waiting_Owner:
	pop	ebx				; EBX = Handle of VM to destroy

VCD_DIO_Exit:
	ret

EndProc VCD_Detach_If_Owner


page
;******************************************************************************
;
;   NAME:
;	VCD_Assign
;
;   DESCRIPTION:
;	This procedure will assign the COM port that is indicated by ESI if
;	possible.  There are four possible cases:
;
;	    1: the port is currently unassigned
;	    2: VM already owns the port
;	    3: the port is owned and seems to be active
;	    4: the port is owned, but doesn't seem to be active
;
;   ENTRY:
;	EBX = VM Handle
;	ESI = Port's control block (MUST <> 0)
;
;   EXIT:
;	ECX = 0 if port was assigned
;	      1 if port was NOT assigned (contention)
;	      2 if port was not assigned because VM has already contended
;		for and lost the COM port
;	ECX = 3 if port was not assigned because it is owned by a VxD
;
;   USES:
;	ECX, Flags
;
;------------------------------------------------------------------------------

BeginProc VCD_Assign

	Assert_VM_Handle ebx
	
	pushad					; Save all registers

	cmp	[esi.VCD_Owner], ebx		; Q: Attach to current owner?
	je	SHORT VCD_Assigned_OK		;    Y: Done
	cmp	[esi.VCD_Owner], 0		; Q: Any VM own device?
	je	SHORT VCD_OK_To_Assign		;    N: Assign it
	cmp	[esi.VCD_Owner], -1		;Q: port owned by a VxD?
	je	short VCD_VxD_Owned		;   Y:

	TestMem	[esi.VCD_Flags], VCD_global_port ;Q: normal contention handling?
	jnz	SHORT VCD_Ignore_Contention	;   N: do NOT steal the port
	cmp	[esi.VCD_Auto_Assign], -2	; Q: Fail silently
	je	VCD_Ignore_Contention
	cmp	[esi.VCD_Auto_Assign], 0	;Q: INI specified auto assign?
	je	SHORT VCD_OK_To_Assign		;   Y: steal the port
	jl	SHORT VCD_Contention		;   N: put up contention dialog
						;   N: INI specified timeout
	VMMCall Get_System_Time
	sub	eax, [esi.VCD_Last_Use]
	cmp	eax, [esi.VCD_Auto_Assign]	;      Q: Can we steal port?
	jb	SHORT VCD_Contention		;	  N: put contention dlg

;; auto-assign if owner port currently masked
;;	   Debug_Out 'VCD: VM is not current owner - check mask status'
;;	   push    ebx
;;	   mov	   eax, [esi.VCD_IRQ_Handle]	   ; EAX = IRQ handle for VPICD
;;	   mov	   ebx, [esi.VCD_Owner]
;;	   VxDCall VPICD_Get_Complete_Status
;;	   test    ecx, VPICD_Stat_Virt_Mask	   ;Q: IRQ masked?
;;	   pop	   ebx
;;	   jz	   SHORT VCD_Contention 	   ;	N: Contention for device

;
;   OK to assign physical device to VM
;
VCD_OK_To_Assign:
	call	VCD_Set_Owner			; Set up the port

VCD_Assigned_OK:
	popad
	xor	ecx, ecx
	ret					; Return with port assigned

;
;   Contention for COM port
;
VCD_Contention:
	mov	eax, [esi.VCD_CB_Offset]
	add	eax, ebx			; EAX -> VCD control block data
	bts	[eax.VCD_CB_Flags], VCD_Contended_Bit ; Q: Has VM contended already?
	jc	SHORT VCD_Ignore_Contention	;    Y: Don't ask user again
						;    N: Display a dialogue box
	mov	eax, [esi.VCD_Owner]		; EAX = Current owner
	push	esi				; EBX = Contender
	lea	esi, [esi.VCD_Name]		; ESI -> Device name ("COMx")
	VxDcall SHELL_Resolve_Contention	; EBX = Contention winner
	pop	esi
	jnc	short resolved
	mov	ebx, eax			; resolve failed, so leave
						; original owner
resolved:
	cmp	ebx, eax			; Q: Same owner?
	je	short no_change 		;   Y: new VM failed to get port
	movzx	ecx, [esi.VCD_Number]		; ECX = port number
	mov	edx, [esi.VCD_CB_Offset]
	SetFlag	[edx+eax.VCD_CB_Flags], VCD_Contended	  ; set contended flag in old owner
	ClrFlag	[edx+ebx.VCD_CB_Flags], VCD_Contended ; clear contended flag in new owner
	jmp	VCD_OK_To_Assign		; assign new owner

no_change:
	VMMCall Get_System_Time
	mov	[esi.VCD_Last_Use], eax
	call	[VCD_not_idle_proc_addr]
	popad
	mov	ecx, 1				; Did not assign the port
	ret					; Contention was raised

VCD_Ignore_Contention:
	popad					; Restore registers
	mov	ecx, 2				; Did not assign port but
	ret					; contention not raised.

VCD_VxD_Owned:
	popad
	mov	ecx, 3
	ret

EndProc VCD_Assign

VxD_Pageable_Code_Ends

VxD_Locked_Code_Seg

;******************************************************************************
;
;   VCD_Dispatch_IO
;
;   DESCRIPTION:    Handle trapped I/O
;
;   ENTRY:	    called from thunk in header of VCD_COM_Struc
;		    [ESP] = OFFSET32 VCD_COM_Struc - VCD_IO_COMS_bias
;		    DX = port #
;		    AL = data to output, if output request
;		    ECX = I/O type
;
;   EXIT:	    AL = data from input, if input request
;
;   USES:	    anything
;
;==============================================================================
BeginProc VCD_Dispatch_IO, NO_LOG

	pop	esi
	add	esi, VCD_IO_COMS_bias		; esi -> VCD_COM_Struc
	Emulate_Non_Byte_IO

VCD_Dispatch_IO_Again:				; alt entry point from
						;   VCD_Cant_Virtualize, if
						;   port wasn't owned the 1st
						;   time thru
	mov	edi, [esi.VCD_CB_Offset]
	add	edi, ebx
	SetFlag	[edi.VCD_CB_Flags], VCD_Touched

	cmp	ebx, [esi.VCD_Owner]		; Q: Do we own the device?
	jne	SHORT virtualize_io		;   N:

	TestMem	[esi.VCD_Flags], VCD_Virtualized
	jnz	short virt_owner_io

dont_virt:
	jecxz	phys_read			;Q: read?
	out	dx, al				;   N: do phys write
	ret

phys_read:
	mov	al, dl
	and	al, UART_REG_MASK		; Get register offset
	cmp	al, UART_IIR			;Q: owner reading IIR?
	je	SHORT Virt_in_IIR		;   Y:
	in	al, dx				;   N: do phys read
	ret

Virt_in_IIR:
	mov	al, 1				; set save to 1 to force re-read
	xchg	al, [esi.VCD_Virt_IIR]		;   next time thru
	mov	ah, al
	and	ah, 7
	cmp	ah, 2				;Q: saved id = xmit buf empty?
	je	SHORT Ret_THRE_id		;   Y: return saved id
	in	al, dx				;   N: re-read id
Ret_THRE_id:
	ret


virt_owner_io:
	TestMem	[esi.VCD_Flags], <VCD_global_port OR VCD_Windows_Port OR VCD_Mouse_Port>
	jnz	dont_virt
	push	edi
	mov	edi, edx
	and	edi, UART_REG_MASK		; Get register offset
	shl	edi, 3
	add	edi, [esi.VCD_virt_procs]
	mov	edi, [edi.VPS_In_RxTxB][ecx]	; EDI -> I/O routine
	xchg	edi, DWORD PTR ss:[esp] 	; Restore EDI, put addr on stack
	ret					; "Jump" to routine

virtualize_io:
	push	edi
	mov	edi, edx
	and	edi, UART_REG_MASK		; Get register offset
	mov	edi, cs:VCD_Trap_Tab[edi*8][ecx]; EDI -> I/O routine
	xchg	edi, DWORD PTR ss:[esp] 	; Restore EDI, put addr on stack
	ret					; "Jump" to routine


VCD_Trap_Tab	LABEL DWORD
	dd	OFFSET32 VCD_Virt_In_Out_RxTxB	  ; Receive/Transmit buffer etc.
	dd	OFFSET32 VCD_Virt_In_Out_RxTxB	  ; Receive/Transmit buffer etc.
	dd	OFFSET32 VCD_Virt_In_Out_IER	  ; Interrupt Enable etc.
	dd	OFFSET32 VCD_Virt_In_Out_IER	  ; Interrupt Enable etc.
	dd	OFFSET32 VCD_Virt_In_IIR	  ; Interrupt Identity
	dd	OFFSET32 VCD_Virt_Out_IIR	  ; Interrupt Identity
	dd	OFFSET32 VCD_Virt_In_LCR	  ; Line Control
	dd	OFFSET32 VCD_Virt_Out_LCR	  ; Line Control
	dd	OFFSET32 VCD_Virt_In_MCR	  ; Modem Control
	dd	OFFSET32 VCD_Virt_Out_MCR	  ; Modem Control
	dd	OFFSET32 VCD_Virt_In_LSR	  ; Line Status
	dd	OFFSET32 VCD_Ignore_IO		  ; Line Status
	dd	OFFSET32 VCD_Virt_In_MSR	  ; Modem Status
	dd	OFFSET32 VCD_Ignore_IO		  ; Modem Status

EndProc VCD_Dispatch_IO


;******************************************************************************
;
;   VCD_Virt_RxTxB -- If I/O is to LSB of BAUD then I/O is virtualized,
;	otherwise the physical device must be assigned since read from receive
;	buffer or write to transmit buffer.
;
;------------------------------------------------------------------------------

BeginProc VCD_Virt_In_Out_RxTxB
	test	[edi].VCD_LCR, LCR_DLAB 	; Is access to divisor latch?
	jz	DEBFAR VCD_Cant_Virtualize	;    N: Then assign device
	jecxz	SHORT VCD_In_Baud_LSB		;    Y: If input then jump
	mov	[edi.VCD_BAUD_LSB], al		;	else save LSB of BAUD
	ret

VCD_In_Baud_LSB:
	mov	al, [edi.VCD_BAUD_LSB]		; Return LSB of BAUD
	ret
EndProc VCD_Virt_In_Out_RxTxB


;******************************************************************************
;
;   VCD_Virt_IER -- This will virtually intialize either the IER or BAUD MSB
;
;------------------------------------------------------------------------------

BeginProc VCD_Virt_In_Out_IER

	test	[edi].VCD_LCR, LCR_DLAB 	; Is access to divisor latch?
        jz      SHORT Virt_IER_IO               ;    N: I/O to IER
	lea	edi, [edi].VCD_BAUD_MSB 	;    Y: I/O is to BAUD MSB
        jmp     SHORT Virtual_IO                ;       Do I/O
Virt_IER_IO:
	lea	edi, [edi].VCD_IER		; I/O is to IER

Virtual_IO:
	jecxz	SHORT Virt_Input
	mov	BYTE PTR [edi], al
	ret

Virt_Input:
	mov	al, BYTE PTR [edi]
	ret

EndProc VCD_Virt_In_Out_IER


;******************************************************************************
;
;   VCD_Virt_In_IIR -- Will return default IIR value (output ignored)
;
;------------------------------------------------------------------------------

BeginProc VCD_Virt_In_IIR
	mov	al, 1				; return none pending
	ret					; Done.
EndProc VCD_Virt_In_IIR

;******************************************************************************
;
;   VCD_Virt_Out_IIR -- Assign port, if attempting to enable FIFO
;
;------------------------------------------------------------------------------
BeginProc VCD_Virt_Out_IIR

	or	al, al				;Q: enabling FIFO?
	jnz	short VCD_Cant_Virtualize	;   Y: Then assign device
	ret

EndProc VCD_Virt_Out_IIR



;******************************************************************************
;
;   VCD_Virt_LCR -- The LCR will be virtually read or written to the VM's
;	Control Block.	Output to this register will also reset the read flags
;	so that the LSR and MSR can be virtually "read" again without assigning
;	the physical device.
;
;------------------------------------------------------------------------------

BeginProc VCD_Virt_In_LCR

	mov    al, [edi.VCD_LCR]		; Virtualize the input
	ret

EndProc VCD_Virt_In_LCR


BeginProc VCD_Virt_Out_LCR

	mov	[edi.VCD_Read_Stat], 0		; Reset the read flags
	mov	[edi.VCD_LCR], al		; Virtualize the output
	ret

EndProc VCD_Virt_Out_LCR


;******************************************************************************
;
;   VCD_Virt_MCD -- If a write to this register sets the UART loopback mode
;	then the physical device will be assigned.  Otherwise, IO to this
;	register will be virtualized.
;
;------------------------------------------------------------------------------

BeginProc VCD_Virt_In_MCR

	mov	al, [edi.VCD_MCR]		; Do the virtual input
	ret

EndProc VCD_Virt_In_MCR


BeginProc VCD_Virt_Out_MCR

	test	al, MCR_Loopback+MCR_INTEN	; Q: Loopback set or int enable?
	jnz	SHORT VCD_Cant_Virtualize	;    Y: Assign device to VM
	mov	[edi.VCD_MCR], al		;    N: Virtualize the output
	ret

EndProc VCD_Virt_Out_MCR


;******************************************************************************
;
;   VCD_Virt_LSR -- Only one read to this register will be vitrualized.  A
;	second read of the LSR will force the physical device to be assigned
;	since the VM may be polling the register.  Output is ignored.
;
;------------------------------------------------------------------------------

BeginProc VCD_Virt_In_LSR

	test	[edi.VCD_Read_Stat], VCD_Read_LSR_Mask	; Q: Read already?
	jnz	SHORT VCD_Cant_Virtualize	;   Y: Then assign device
	or	[edi.VCD_Read_Stat], VCD_Read_LSR_Mask ; N: Set read flag
	mov	al, [esi.VCD_Def_LSR]		; "Read" default LSR once
	ret

EndProc VCD_Virt_In_LSR


;******************************************************************************
;
;   VCD_Virt_MSR -- Only one read to this register will be vitrualized.  A
;	second read of the MSR will force the physical device to be assigned
;	since the VM may be polling the register.  Output is ignored.
;
;------------------------------------------------------------------------------

BeginProc VCD_Virt_In_MSR

	test	[edi.VCD_Read_Stat], VCD_Read_MSR_Mask ; Q: Read already?
	jnz	SHORT VCD_Cant_Virtualize	;   Y: Then assign device
	or	[edi.VCD_Read_Stat], VCD_Read_MSR_Mask ; N: Set read flag
	mov	al, [esi].VCD_Def_MSR		; "Read" default MSR once
	ret

EndProc VCD_Virt_In_MSR

;******************************************************************************
;
;   VCD_Cant_Virtualize
;
;   DESCRIPTION:    assign physical device
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================
BeginProc VCD_Cant_Virtualize

	push	ecx
	call	VCD_Take_Port			; Assign the COM port
	test	ecx, ecx			; Q: Successful?
	pop	ecx
	jz	VCD_Dispatch_IO_Again		;    Y: Do normal IO
VCD_Ignore_IO:
	mov	al, -1				; Will return FFh if input
	ret					; IGNORE IT!!!!

EndProc VCD_Cant_Virtualize


;******************************************************************************
;
;   VCD_Take_Port
;
;   DESCRIPTION:
;	Call VCD_Assign and display an error message if the assign fails
;	because the port is owned by a VxD.
;
;   ENTRY:
;	(Same as VCD_Assign)
;
;   EXIT:
;	(Same as VCD_Assign)
;
;   USES:
;	(Same as VCD_Assign)
;
;==============================================================================
BeginProc VCD_Take_Port

	call	VCD_Assign
	cmp	ecx, 3		;Q: failed because VxD owns the port?
	jne	short vtp_exit

	pushad
	mov	edi, [esi.VCD_CB_Offset]
	bts	[ebx+edi.VCD_CB_Flags], VCD_Assign_Failed_Bit
	jc	short @F
;
; message box for assign failure, copy COM port name into message text first
;
        mov     ecx, OFFSET32 Name_Buf
	mov	eax, dword ptr [esi.VCD_Name]
	mov	[ecx], eax
	mov	eax, dword ptr [esi.VCD_Name+4]
	mov	[ecx+4], eax

        PUSH_SPRINTF <MSG_VCD_Assign_Fail_Msg>, <OFFSET32 Name_Buf>
        xchg    eax, ecx                        ; ECX = message on stack
        mov     eax, MB_OK + MB_ASAP + MB_ICONEXCLAMATION + MB_NOWINDOW
        xor     edi, edi
	VxDCall SHELL_SysModal_Message		; send message to Windows
        POP_SPRINTF
@@:
	popad
vtp_exit:
	ret

EndProc VCD_Take_Port



PAGE

;******************************************************************************
;
;   VCD_handler
;
;   DESCRIPTION:    Common entry point from thunks in header before
;		    VCD_COM_Struc.
;		    The header looks like:
;
;				mov	dl, 16
;				jmp	short @F
;				mov	dl, 12
;				jmp	short @F
;				mov	dl, 8
;				jmp	short @F
;				mov	dl, 4
;				jmp	short @F
;				mov	dl, 0
;			@@:	call	VCD_Handler
;  start of VCD_COM_Struc -->	[VCD_CB_Offset]
;
;   ENTRY:	    called from thunks in header of VCD_COM_Struc
;		    [ESP] -> VCD_COM_Struc
;		    EAX = IRQ Handle
;		    DL = 0    for VCD_Hw_Int_Proc
;			 4    for VCD_Virt_Int_Proc
;			 8    for VCD_EOI_Proc
;			 12   for VCD_Mask_Change_Proc
;			 16   for VCD_IRET_Proc
;
;   EXIT:	    indirect jumps to hander with ESI -> VCD_COM_Struc
;
;   USES:	    EAX, ESI, ECX, Flags
;
;==============================================================================
BeginProc VCD_handler, NO_LOG

	pop	esi		; esi -> VCD_COM_Struc
	movzx	edx, dl
	or	edx, edx
	jz	short handle_hw_int
	jmp	[edx][esi.VCD_Hw_Int_Proc]

handle_hw_int:
	push	esi
	call	[edx][esi.VCD_Hw_Int_Proc]
	pop	esi
	jc	short vh_not_serviced	; jump if not serviced
	TestMem	[esi.VCD_Flags],VCD_Global_IRQ	; Q: Is irq global ?
	jz	vh_ret			;  N: return with NC
	stc				;  Y: return with CY
	ret

vh_not_serviced:
	TestMem	[esi.VCD_Flags], VCD_1st_on_IRQ
	stc
	jz	short vh_ret		; not the last so let it go
	cmp	[esi.VCD_Owner],1
	jc	vh_ret
	mov	eax, [esi.VCD_IRQ_Handle]
	VxDCall VPICD_Get_Complete_Status
	bt	ecx, VPICD_Stat_Phys_In_Serv_Bit    ; carry set means that
						    ; it is still in service,
						    ; so we should let it go
vh_ret:
	ret

EndProc VCD_handler


page
;******************************************************************************
;
;   VCD_Int
;
;   DESCRIPTION:    common handler for COM controller interrupts
;
;   ENTRY:	    EAX = IRQ handle
;		    EBX = VM handle
;		    ESI -> COM data structure
;
;   EXIT:
;
;   USES:
;
;==============================================================================
IFDEF SHARE_IRQ
BeginProc VCD_Sharable_Int, High_Freq

	mov	ecx, eax			; save IRQ handle
	mov	edx, [esi.VCD_IObase]		; Get start of ports
	add	edx, UART_IIR
	in	al, dx				; get interrupt identity
	mov	[esi.VCD_Virt_IIR], al

	test	al, IIR_NONE			;Q: Came from this device?
	mov	eax, ecx			; (restore IRQ handle)
	stc
	jnz	short VCD_Int_exit		;   N: carry set - didn't service
	Assumes_Fall_Through VCD_Int
EndProc VCD_Sharable_Int

BeginProc VCD_Int
ELSE
BeginProc VCD_Int, High_Freq
ENDIF

	TestMem	[esi.VCD_Flags], VCD_Windows_Port
	jnz	short VCDI_WinPort

	mov	ebx, [esi.VCD_Owner]
	test	ebx, ebx			; Q: Owned by any VM?
	jz	SHORT VCDI_assign		;   N: assign current VM
	inc	ebx				;Q: int caused during detach?
	jz	short VCDI_ignore		;   Y: consume it
	dec	ebx
VCD_Request_Int:
	TestMem	[ebx.CB_VM_Status], VMStat_Suspended ;Q: VM suspended?
	jnz	short VCDI_ignore		    ;	Y: service int & EOI
	mov	ecx, eax			; save IRQ handle
	mov	edx, [esi.VCD_IObase]		; Get start of ports
	add	dl, UART_MCR
	in	al, dx
	test	al, MCR_INTEN			;Q: master int enable for port?
	mov	eax, ecx
	jz	short VCDI_ignore		;   N: service int
	VxDCall VPICD_Set_Int_Request		;   Y: Request the interrupt

VCD_Int_Serviced:
	clc

VCD_Int_exit:
	ret


VCDI_ignore:
	call	VCD_Eat_Interrupt
	VxDCall VPICD_Phys_EOI
	jmp	VCD_Int_Serviced


VCDI_assign:
	mov	edx, [esi.VCD_IObase]		; Get start of ports
	add	dl, UART_IER
	in	al, dx
	or	al, al				;Q: ints enabled from port?
	stc
	jz	VCD_Int_exit			;   N: return with carry set
	IO_Delay
	add	dl, UART_MCR-UART_IER
	in	al, dx
	test	al, MCR_INTEN			;Q: master int enable for port?
	stc
	jnz	VCD_Int_exit			;   N: return with carry set

	VMMcall Get_Cur_VM_Handle		;   Y: This VM should own port
	bts	[esi.VCD_Flags], VCD_SetOwner_Event_Bit
	jc	VCD_Request_Int
	mov	[esi.VCD_Owner], ebx
	mov	edx, esi			;	schedule event to set
	mov	esi, OFFSET32 VCD_Set_Owner_Event   ;	owner
	VMMCall Schedule_Global_Event
	mov	esi, edx
	jmp	VCD_Request_Int 		;	& request int for VM


VCDI_WinPort:
	TestMem	[esi.VCD_Flags], VCD_Windows_30Drvr
	jz	short not_30_driver ; jump with carry clear, so cmc below will
				    ;	set it indicating that the int hasn't
				    ;	been handled
	push	esi
	call	[VCD_Windows_Int]
	pop	esi
	mov	eax, [esi.VCD_IRQ_Handle]
	VxDCall VPICD_Phys_EOI
	VMMCall Get_Last_Updated_System_Time
	mov	[esi.VCD_Last_Use], eax
	stc			    ; set carry so cmc below will clear
				    ;	it and indicate that the int was handled
not_30_driver:
	cmc
	ret

EndProc VCD_Int


;******************************************************************************
;
;   VCD_COMMDRV_Int
;
;   DESCRIPTION:    Called at Ring 0 from COMM.DRV's Ring 0 interrupt handler.
;		    This routine is needed to update the last used time field
;		    in the COM data structure for handling contention for the
;		    COM port.  (We must do a FAR RET to return!)
;
;   ENTRY:	    ESI -> COM data structure
;		    SS:ESP our flat stack
;		    DS & ES are non-flat data selectors
;
;   EXIT:	    none
;
;   USES:	    Flags
;
;==============================================================================
BeginProc VCD_COMMDRV_Int, NO_LOG, NO_PROLOG

	push	es
	push	ds
	push	eax
	mov	eax, ss
	mov	ds, eax
	mov	es, eax
	VMMCall Get_Last_Updated_System_Time
	mov	[esi.VCD_Last_Use], eax
	pop	eax
	pop	ds
	pop	es
	retf

EndProc VCD_COMMDRV_Int


;------------------------------------------------------------------------------
;
;   ASSUMES:
;	It is OK to call VPICD_Phys_EOI TWICE for an interrupt (even if it is
;	not physically in service).

IFDEF SHARE_IRQ

BeginProc VCD_Sharable_EOI, High_Freq

; PHYS_EOI only if we have set the INT req before.
	VxDCall VPICD_Get_Complete_Status
	TestReg	ecx, VPICD_Stat_Virt_Dev_Req	;Q: we requested int?
	jz	SHORT VCD_E_Exit		;   N:

	VxDCall VPICD_Phys_EOI		  ; OK to call twice!

; Check the H/W for a missed interrupt
	mov	edx, [esi.VCD_IObase] 	  ; Get start of ports
	add	edx, UART_IIR
	in	al, dx			  ; get interrupt identification reg
	mov	[esi.VCD_Virt_IIR], al
	test	al, IIR_NONE		  ;  Q: I/R pending?
	jz	SHORT VCD_E_adj 	  ;    Y: Don't clear Int req
	mov	eax, [esi.VCD_IRQ_Handle]
	jmp	short VCD_E_CIR
EndProc VCD_Sharable_EOI
ENDIF

BeginProc VCD_EOI, High_Freq

	VxDCall VPICD_Phys_EOI		  ; OK to call twice!
VCD_E_CIR:
	VxDCall VPICD_Clear_Int_Request

VCD_E_adj:
	mov	eax, [VCD_Int_Boost_Amount]
	VMMcall Adjust_Execution_Time

	VMMCall Get_Last_Updated_System_Time
	mov	[esi.VCD_Last_Use], eax
	call	[VCD_not_idle_proc_addr]

VCD_E_Exit:
	ret

EndProc VCD_EOI


;******************************************************************************
;
;   VCD_Mask
;
;   DESCRIPTION:    Common handler for COM port interrupt masking.
;		    If unmasking, then check for contention and/or assign
;		    the port to the current VM
;
;   ENTRY:	    EAX = IRQ handle
;		    EBX = VM handle
;		    ESI -> COM data structure
;		    ECX = 0, if unmasking
;
;   EXIT:
;
;   USES:
;
;==============================================================================
BeginProc VCD_Mask

	jecxz	short VCD_unmasking	;Q: unmasking?
	ret				;   N:
VCD_unmasking:
	call	VCD_Take_Port		;   Y: assign ownership
	ret

EndProc VCD_Mask

VxD_Locked_Code_Ends

VxD_Pageable_Code_Seg

;******************************************************************************
;
;   VCD_Soft_Int_14h
;
;   DESCRIPTION:    Reschedule use timeout used to determine if COM port
;		    is being used.
;
;   ENTRY:	    EBX = VM handle
;		    EBP => Client frame
;
;   EXIT:	    nothing
;
;   USES:	    nothing
;
;==============================================================================

BeginProc VCD_Soft_Int_14h

	movzx	edx, [ebp.Client_DX]
	cmp	edx, [MaxPort]		    ;Q: port in range?
	jae	short reflect_swi_14	    ;	N: skip it
	mov	esi, [VCDTable]
	mov	esi, [edx*4][esi]
	test	esi, esi
	jz	short reflect_swi_14
	cmp	ebx, [esi.VCD_Owner]	    ;Q: current owner of the port?
	jne	short reflect_swi_14	    ;	N: don't schedule use timeout
	VMMCall Get_System_Time 	    ;	Y: record last use time
	mov	[esi.VCD_Last_Use], eax
;
; Don't need to call VMPoll here, because VMPoll has its own INT 14h hook
;

reflect_swi_14:
	stc			; reflect the interrupt
	ret

EndProc VCD_Soft_Int_14h

;******************************************************************************
;
; VCD_Contention_Handler
;
; Description:
;	returns the contention handler of VCD.
; Entry:
;	none
; Exit:
;	EAX = contention handler, NC
; Uses:
;	EAX, flags.
;==============================================================================
BeginProc VCD_Get_Contention_Handler

	mov	eax,OFFSET32 _VCD_Contention_Handler
	clc
	ret

EndProc VCD_Get_Contention_Handler

;******************************************************************************
;
; VCD_Contention_Handler
;
; Description:
;		Provides contention services for callers.
; Entry:
;	FunctionCode = function to perform.
; Exit:
;	that of the specific function
; Uses:
;	C style
;==============================================================================
BeginProc VCD_Contention_Handler,CCALL,esp,PUBLIC

ArgVar	FunctionCode,DWORD

	EnterProc
	mov	eax,FunctionCode
	cmp	eax,MAX_CONTEND_FUNCTIONS
	ja	VCH_Done
	jmp	[Contention_Handler+eax*4]

VCH_Done:
	LeaveProc
	return

EndProc VCD_Contention_Handler

;******************************************************************************
;
; VCD_Map_Name_To_Resource
;
; Description: maps a name such as COMx to a resource
;
; Parameters:
;
;	FunctionCode = MAP_NAME_TO_RESOURCE
;	PName -> Name to map
;	hDevNode = devnode corresponding to the device
;	IOBase = Base allocated to the device
;
; Returns:
;	0 if could not be mapped, else VCD_COM_STRUC
;
;==============================================================================
BeginProc VCD_Map_Name_To_Resource,CCALL,esp

ArgVar	FunctionCode,DWORD
ArgVar	PName,DWORD
ArgVar	hDevNode,DWORD
ArgVar	IOBase,DWORD

	EnterProc

	SaveReg	<esi>
	mov	edx,[VCDTable]
	mov	ecx,[MaxPort]
	jecxz	MapBase_None
	mov	eax,IOBase

MapBase_Loop:
	mov	esi,[edx]
	cmp	eax,[esi.VCD_IOBase]
	je	MapBase_Done
	lea	edx,[edx+4]
	loop	MapBase_Loop

MapBase_None:
	xor	esi,esi

MapBase_Done:
	mov	eax,esi

	RestoreReg <esi>
	LeaveProc
	return

EndProc VCD_Map_Name_To_Resource


;***
;
; GetPortNum
;
; Entry:
;	ECX = hDevNode
;
; Exit:
;	Z if success, else failure
;	EAX = port number if success.
; Uses:
;	EAX, ECX, EDX, EDI
;
BeginProc GetPortNum

	mov	edi, OFFSET32 regbuf
	mov	eax, OFFSET32 prd_Temp
	mov	DWORD PTR [eax], 9
	VxDCall	_CONFIGMG_Read_Registry_Value, <ecx, \
			0, OFFSET32 PortName, REG_SZ, \
			edi, eax, CM_REGISTRY_HARDWARE>
	test	eax, eax		; Q: success ?
	jnz	gpn_done		;   N:

	mov	eax, DWORD PTR [edi]
	sub	eax, '0MOC'
	jb	gpn_done
	test	eax, 0FFFFFFh
	jnz	gpn_done
	rol	eax, 8			; get num
	cmp	al,9
	ja	gpn_done
	cmp	al,1
	jb	gpn_done
	movzx	ecx, BYTE PTR [edi+4]	; Q: COMx ?
	test	ecx, ecx
	jz	gpn_done		;    Y:
	cmp	BYTE PTR [edi+5], 0	; Q: COMxx ?
	jnz	gpn_done		;    N:
	imul	eax, eax, 10
	sub	cl, '0'
	jb	gpn_done
	cmp	cl, 9
	ja	gpn_done
	add	eax, ecx
	xor	ecx, ecx

gpn_done:
	Trace_OutNZ "VCD: Could not find portname of devnode #ECX"
	ret

EndProc GetPortNum

VxD_Pageable_Data_Seg

runonce		db	REGSTR_PATH_RUNONCE, 0

EnablePCMCIA	db	'EnablePCMCIA', 0

regtype	dd	REG_BINARY

VxD_Pageable_Data_Ends

;******************************************************************************
;
; VCD_Port_Arrival
;
; Description:
;	Called by VCOMM when it gets the alloc'ed logical config
;	to tell VCD that this is an additional port it needs to keep
;	track of for contention.
; Entry:
;	AllocBase = allocated base
;	AllocIRQ  = alloc'ed irq
;	hDevNode  = devnode for us
; Exit:
;	0 if port doesn't exist, else non-zero
; Uses:
;	C style
;=============================================================================
BeginProc VCD_Port_Arrival, CCALL, PUBLIC, esp, RARE

ArgVar	FunctionCode, DWORD
ArgVar	hDevNode, DWORD
ArgVar	AllocBase, DWORD
ArgVar	AllocIRQ, DWORD

LocalVar AllocPortNum, DWORD

	EnterProc

	SaveReg	<esi, edi, ebx>

	lea	esi, AllocPortNum

	VMMCall	_RegOpenKey, <HKEY_LOCAL_MACHINE, OFFSET32 runonce, esi>
	or	eax, eax			; Q: RUNONCE section present?
	jnz	vpa_norunonce			;    N: no global overrides

	mov	prd_Temp, 1
	VMMCall	_RegQueryValueEx,<[esi], OFFSET32 EnablePCMCIA, \
				  0, OFFSET32 RegType, \
				  OFFSET32 prd_Temp+1, OFFSET32 prd_Temp>
	mov	edi, eax
	VMMCall	_RegCloseKey,<[esi]>

	test	edi, edi
	jz	vpa_test_exist
	cmp	edi, ERROR_MORE_DATA
	jz	vpa_test_exist

vpa_norunonce:
	mov	eax, OFFSET32 prd_Temp
	mov_b	ecx, 4
	mov	DWORD PTR [eax], ecx

	VxDCall	_CONFIGMG_Read_Registry_Value,<hDevNode, \
			0, OFFSET32 verify_base_ini, REG_BINARY, \
			esi, eax, CM_REGISTRY_HARDWARE>
	test	eax, eax			; Q: Does entry exist ?
	jnz	vpa_exists			;    N: skip verification
	cmp	AllocPortNum, eax
	jz	vpa_exists

vpa_test_exist:
	mov	edx, AllocBase
	add	dl, UART_IIR
	in	al, dx
	IO_Delay
	test	al, 30h
	jnz	vpa_failed

vpa_exists:
	mov	ecx, hDevNode		; get port number for the port.
	call	GetPortNum
	jz	vpa_num_found
	mov	eax, AllocBase
	mov_b	ecx, 4
	mov	edi, OFFSET32 KnownBases
	cld
	repne	scasw
	jnz	vpa_failed
	mov_b	eax, 4
	sub	eax, ecx

vpa_num_found:
	mov	AllocPortNum, eax

	test	[device_init_flag], fDevice_Init_Done
	jz	vpa_predevice_init
;
; The tough part. We need to expand the array of ports. Then we need
; to allocate data structs for this port AND call VCD_Init_Port for
; it.
;
	mov	ebx, eax		; save index
	mov	esi, OFFSET32 VCDTable
	cmp	eax, [MaxPort]		; new port > = known ports ?
	jbe	@F
	mov	[MaxPort], eax
	shl	eax, 2			; new size of VCD table.
	VMMCall	_HeapReallocate,<[esi], eax, HEAPZEROINIT>
	test	eax, eax		; Q: Realloc succeeded ?
	jz	vpa_failed		;    N: get out.
	mov	DWORD PTR [esi], eax
@@:
	mov	eax, [esi]
	cmp	DWORD PTR [eax+ebx*4-4], 0
	TRAPNZ
	jne	vpa_success

	mov	ecx, AllocIRQ
	mov	esi, AllocBase
	shl	esi, 16
	movzx	si, cl
	mov	edx, [AllocPortNum]	; EDX = port #.
	xor	eax, eax		; no extra COM area
	xor	ecx, ecx		; no extra CB area
	xor	edi, edi		; no irq procs
	xor	ebx, ebx		; assume irqs are not sharable.
	TestMem	[machine_info], <GMIF_EISA OR GMIF_MCA>
	jz	@F
	or	bl, 1			; they are sharable on MCA/EISA
@@:
	VxDCall	VCD_Virtualize_Port_Dynamic
	jc	vpa_failed

	SaveReg	<esi>
	lea	edi, [esi.VCD_Name]
	mov	esi, OFFSET32 RegBuf
	mov_b	ecx, 8

vpa_copy_name_loop:
	lodsb
	stosb
	dec	ecx
	or	al, al
	jnz	vpa_copy_name_loop
	dec	edi
	inc	ecx
	mov	al, ' '
	rep	stosb

	RestoreReg <esi>

	VMMCall	Get_Cur_VM_Handle

vpa_init_vm_loop:
	SaveReg	<esi>
	SetFlag	[esi.VCD_Flags], VCD_Dynamic_Port
	call	VCD_Init_VM_CB
	RestoreReg <esi>
	VMMCall	Get_Next_VM_Handle
	VMMCall	Test_Cur_VM_Handle
	jne	vpa_init_vm_loop

	jmp	vpa_success

vpa_predevice_init:
	call	VCD_Initialize
	jc	vpa_failed

	mov	esi, [VCD_COMList]
	VMMCall	List_Allocate
	jc	vpa_failed
	VMMCall	List_Attach_Tail
	mov	edi, eax
	mov	ebx, edi			; ebx -> VCD_Init_Data_Struc
	xor	eax, eax
	mov	ecx, SIZE VCD_Init_Data_Struc
	rep	stosb
	mov	eax, AllocIRQ
	mov	[ebx.VCD_id_irq], al
	mov	eax, AllocBase
	mov	[ebx.VCD_id_base], ax
	mov	eax, AllocPortNum
	mov	[ebx.VCD_id_num], eax

vpa_success:
	or	al, 1
	jmp	vpa_done

vpa_failed:
	xor	eax, eax

vpa_done:
	RestoreReg <ebx, edi, esi>
	LeaveProc
	return

EndProc VCD_Port_Arrival

;******************************************************************************
;
; VCD_Port_Departure
;
; Description:
;		Called by VCOMM to tell VCD that a port is going away
;
; Entry:
;
; Exit:
;	None.
; Uses:
;	C style.
;
;==============================================================================
BeginProc VCD_Port_Departure, CCALL, PUBLIC, ESP, RARE

ArgVar	FunctionCode,DWORD
ArgVar	AllocBase,DWORD

	EnterProc

	SaveReg	<esi, edi, ebx>

	test	[VCD_Global_Flags], fVCD_Init_Complete
	TRAPZ
	jz	vpd_done

	mov	esi, [VCDTable]
	mov	eax, [AllocBase]
	mov	ecx, [MaxPort]
	jecxz	vpd_done

vpd_find_port:
	mov	edx, [esi]
	test	edx, edx
	jz	vpd_next_port
	cmp	[edx.VCD_IOBase], eax
	jz	vpd_found_port

vpd_next_port:
	add	esi, 4
	loop	vpd_find_port
	TRAP
	jmp	vpd_done

vpd_found_port:
	mov	DWORD PTR [esi], 0		; remove the port
	mov	esi, edx			; ESI -> VCD_COM_Struc
	movzx	edx, [esi.VCD_Number]
	mov	ecx, [esi.VCD_IOBase]
	VxDCall	VCD_UnVirtualize_Port_Dynamic

	VMMCall	Get_Cur_VM_Handle

vpd_init_vm_loop:

	mov	eax, [ebx.CB_High_Linear]
	cmp	word ptr [eax+edx*2-2+400h], cx
	jne	@F
	mov	word ptr [eax+edx*2-2+400h], 0
@@:
	VMMCall	Get_Next_VM_Handle
	VMMCall	Test_Cur_VM_Handle
	jne	vpd_init_vm_loop

vpd_done:
	RestoreReg <ebx, edi, esi>
	LeaveProc
	return

EndProc VCD_Port_Departure

;******************************************************************************
;
; VCD_Acquire_Resource
;
; Description:
;
;	Blindly calls VCD_Acquire_Port_Windows_Style which will call
;	the current owner, if any, to inform it that someone else wants
;	to open the port. If the current owner is ok with it, the port
;	will be stolen from it and given to the caller.
;
; Entry:
;	FunctionCode = ACQUIRE_RESOURCE
;	Resource = VCD_COM_STRUC
;	NotifyProc -> Address of procedure to call
;	NotifyRefData = reference data to call with.
;	StealFlag = if 0, don't even try to acquire if owned
;		    if 1, acquire it, try to steal if owned.
; Exit:
;	0 if not acquired, else port handle
; Uses:
;	C style.
;==============================================================================
BeginProc VCD_Acquire_Resource,CCALL

ArgVar	FunctionCode,DWORD
ArgVar	Resource,DWORD
ArgVar	NotifyProc,DWORD
ArgVar	NotifyRefData,DWORD
ArgVar	StealFlag,DWORD

	EnterProc

	SaveReg	<esi,edi,ebx>

	mov	esi,Resource		; get VCD_COM_STRUC
	test	esi,esi
	jz	VAR_Failed

	TestMem	[esi.VCD_Flags],<VCD_AltOwnerActive OR VCD_Mouse_Port>
					; Q: Two owners pending ?
	jnz	VAR_Failed		; Y: no more owners allowed.

	mov	ecx,[esi.VCD_Notify]	; Q: Owned port ?
	jecxz	VAR_Acquire		;    N: let it be acquired.
	cmp	StealFlag,0		; Y: Q: Should we try to acquire ?
	je	VAR_Failed		;    N:
	cCall	ecx,<[esi.VCD_RefData],0> ; Someone wants to acquire port.
	test	eax,eax			; Q: Should we allow it ?
	jz	VAR_Done		;    N:
	cmp	[esi.VCD_Notify],0      ; Q: Did owner free the port ?
	jz	VAR_Acquire		;      Y: normal acquire
	mov	eax,[esi.VCD_Notify]
	mov	edx,[esi.VCD_RefData]
	mov	[esi.VCD_Alt_Notify],eax
	mov	[esi.VCD_Alt_RefData],edx
	SetFlag	[esi.VCD_Flags], VCD_AltOwnerActive

VAR_Acquire:
	mov	eax,[esi.VCD_IOBase]	; EAX = io base.
	mov_b	ecx,0Ah			; acquire by base AND notifyProc
	VxDCall	Get_Sys_VM_Handle
	mov	esi,NotifyProc
	mov	edi,NotifyRefData
	VxDCall	VCD_Acquire_Port_Windows_Style
	jnc	VAR_Done

VAR_Failed:

	xor	eax,eax			; failed acquire

VAR_Done:
	RestoreReg <ebx,edi,esi>
	LeaveProc
	return

EndProc VCD_Acquire_Resource

;******************************************************************************
;
; VCD_Steal_Resource
;
; Description:
;		Called to steal a port which was lost due to someone else
;	wanting control.
;
; Parameters:
;	FunctionCode = STEAL_RESOURCE
;	ResourceHnd = resource handle
;	NotifyProc = Address of proc set during Acquire_Resource.
; Exit:
;	EAX != 0 if stolen, else = 0.
; Uses:
;	C style
;==============================================================================
BeginProc VCD_Steal_Resource,CCALL,esp

ArgVar	FunctionCode,DWORD
ArgVar	ResourceHnd,DWORD
ArgVar	NotifyProc,DWORD

	EnterProc

	SaveReg	<esi,edi,ebx>

	mov	eax, ResourceHnd
	call	VCD_Validate_Handle
	jnz	VSR_Failed

	mov	esi,eax
	test	esi,esi
	jz	VSR_Failed
	VMMCall	Get_Sys_VM_Handle
	mov	eax,NotifyProc
	cmp	[esi.VCD_Alt_Notify],eax	; Q: Alternate owner ?
	jne	VSR_Primary_Owner_Steal	;    N: steal it for primary owner
	cmp	ebx,[esi.VCD_Owner]	; Q: Does primary owner own it ?
	jnz	VSR_Switch_Owners	;    N: Switch owners and try stealing.
	cCall	[esi.VCD_Notify],<[esi.VCD_RefData], \
				0>	; Q: Ok to steal from primary owner ?
	test	eax,eax	
	jz	VSR_Done		;    N:
	mov	eax,NotifyProc

VSR_Switch_Owners:
 ;
 ; Some other VM owns it. and alternate owner wants to steal. We will make
 ; it primary owner and try to steal.
 ;
	xchg	eax,[esi.VCD_Notify]	    ; VCD_Notify = alt notify
	xchg	eax,[esi.VCD_Alt_Notify]    ; VCD_Alt_Notify = primary owner
	mov	eax,[esi.VCD_RefData]       ; primary ref data
	xchg	eax,[esi.VCD_Alt_RefData]   ; VCD_Alt_Refdata = primary ref
	mov	[esi.VCD_RefData],eax       ; VCD_RefData = sec ref data.

VSR_Primary_Owner_Steal:
	cmp	ebx,[esi.VCD_Owner]
	jz	VSR_Success
	SetFlag	[overrides],1
	VxDCall	VCD_Steal_Port_Windows_Style ; ESI -> VCD_COM_STRUC, EBX=sysvm
	ClrFlag	[overrides],1
	jmp	VSR_Done

VSR_Failed:
	xor	eax,eax
	jmp	VSR_Done

VSR_Success:
	or	al,1

VSR_Done:
	RestoreReg <ebx,edi,esi>

	LeaveProc
	return

EndProc VCD_Steal_Resource

;******************************************************************************
;
; VCD_Release_Resource
;
; Description:
;	Called to release a resource acquired via Acquire_Resource.
;
; Parameters:
;	FunctionCode = RELEASE_RESOURCE
;	ResourceHnd = handle of the resource
;	NotifyProc = Address of proc set during Acquire_Resource
; Exit:
;	None
;
; Uses:	C Style
;==============================================================================
BeginProc VCD_Release_Resource,CCALL,esp

ArgVar	FunctionCode,DWORD
ArgVar	ResourceHnd,DWORD
ArgVar	NotifyProc,DWORD

	EnterProc
	SaveReg	<ebx,esi>

	mov	eax,ResourceHnd
	call	VCD_Validate_Handle
	jnz	VRR_Done

	mov	esi,eax
	mov	eax,NotifyProc
	cmp	[esi.VCD_Alt_Notify],eax	; Q: Alternate owner ?
	jz	VRR_Alt_Owner		;    Y: Just clear flag and leave

	mov	eax,esi
	VxDCall	Get_Sys_VM_Handle
	VxDCall	VCD_Free_Port_Windows_Style
	mov	[esi.VCD_Notify],0

	btr	[esi.VCD_Flags],VCD_AltOwnerActiveBit ; Q: pending Alt_owner ?
	jnc	VRR_Done		; N:

	cCall	VCD_Acquire_Resource,<ACQUIRE_RESOURCE, esi, \
			[esi.VCD_Alt_Notify], [esi.VCD_Alt_RefData], \
			0>

	cCall	[esi.VCD_Notify],<[esi.VCD_RefData],1>

VRR_Alt_Owner:
	ClrFlag	[esi.VCD_Flags],VCD_AltOwnerActive

VRR_Done:
	RestoreReg <esi,ebx>
	LeaveProc
	return

EndProc VCD_Release_Resource

VxD_Pageable_Code_Ends

VxD_Locked_Code_Seg

IFDEF DEBUG

;******************************************************************************
;
;   VCD_Debug_Dump
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

BeginProc VCD_Debug_Dump

	pushad

	mov	esi, [VCDTable]
	mov	esi, [esi]
	test	esi, esi			    ; Q: do we have Hardware
	jz	SHORT VCD_Debug_no_com1 	    ;	 n: don't dump struct
        call    VCD_Dump2                           ;    y: do dump
	jz	SHORT VCD_Debug_Exit
	jmp	SHORT VCD_Debug_com2

VCD_DEbug_no_com1:

	Trace_Out "No COM1 hardware!!!!!"
        Assumes_Fall_Through VCD_Debug_com2

VCD_Debug_com2:

	Trace_Out " "
	mov	esi, [VCDTable]
	mov	esi, [esi+4]
	test	esi, esi			    ; Q: do we have Hardware
	jz	SHORT VCD_Debug_no_com2 	    ;	 n: don't dump struct
        call    VCD_Dump2                           ;    y: do dump
	jz	SHORT VCD_Debug_Exit
%out add com3, com4 debug dumps here, if necessary
        jmp     SHORT VCD_Debug_Exit

VCD_Debug_no_com2:

	Trace_Out "No COM2 hardware!!!!!"
	Assumes_Fall_Through VCD_Debug_Exit

VCD_Debug_Exit:
	popad
	ret

EndProc VCD_Debug_Dump


;******************************************************************************
;
;   VCD_Dump2
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;	ZF set means user hit ESC
;
;   USES:
;
;==============================================================================

BeginProc VCD_Dump2

	mov al, [esi.VCD_Number]
        Trace_Out "COM#AL:"

	Dump_Struc_Head
	Dump_Struc esi, VCD_CB_Offset
	Dump_Struc esi, VCD_Number
	Dump_Struc esi, VCD_IRQN
	Dump_Struc esi, VCD_IRQ_Desc
	Dump_Struc esi, VCD_Flags
	Dump_Struc esi, VCD_IObase
	Dump_Struc esi, VCD_Owner
	Dump_Struc esi, VCD_IRQ_Handle

	VMMcall In_Debug_Chr
	jz	VCD_D2_Exit
	Trace_Out " "

	Dump_Struc esi, VCD_Def_BAUD_LSB
	Dump_Struc esi, VCD_Def_BAUD_MSB
	Dump_Struc esi, VCD_Def_IER
	Dump_Struc esi, VCD_Def_LCR
	Dump_Struc esi, VCD_Def_MCR
	Dump_Struc esi, VCD_Def_LSR
	Dump_Struc esi, VCD_Def_MSR

	VMMcall In_Debug_Chr
	jz	VCD_D2_Exit
	Trace_Out " "

	VMMcall Get_Sys_VM_Handle
	Trace_Out "STATE OF VM #EBX:"
	add	ebx, [esi.VCD_CB_Offset]

	Dump_Struc_Head
	Dump_Struc ebx, VCD_BAUD_LSB
	Dump_Struc ebx, VCD_BAUD_MSB
	Dump_Struc ebx, VCD_IER
	Dump_Struc ebx, VCD_LCR
	Dump_Struc ebx, VCD_MCR
	Dump_Struc ebx, VCD_Read_Stat
	Dump_Struc ebx, VCD_CB_Flags
	VMMcall In_Debug_Chr
	Trace_Out " "

VCD_D2_Exit:
	ret

EndProc VCD_Dump2

ENDIF

VxD_Locked_CODE_ENDS


	END
