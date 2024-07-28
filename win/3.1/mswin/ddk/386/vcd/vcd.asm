PAGE 58,132
;******************************************************************************
TITLE VCD.ASM - Virtual COM Device
;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1989-1991
;
;   Title:	VCD.ASM - Virtual COM Device
;
;   Version:	3.10
;
;   Date:	10-Apr-1989
;
;   Author:	RAP
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;   10-Apr-1989 RAP started re-write
;   16-Nov-1990 RAP Changed the API handling for COMM.DRV to acquire and free
;		    a COM port
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

	Create_VCD_Service_Table EQU 1	    ; VCD service table created
	INCLUDE VCD.INC
	INCLUDE VCDSYS.INC
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
VCD_Init_Data_Struc ENDS

Def_IRQs	db  4, 3, 4, 3

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

device_init_flag    db	0
fDevice_Init_Done   equ 00000001b
fMCA_machine	    equ 00000010b

verify_flag	    db	0

VxD_IDATA_ENDS


;==============================================================================
VxD_DATA_SEG

EXTRN VCD_Setup_Ptrs_Addr:DWORD
EXTRN VCD_Xmit_CallBack_Addr:DWORD
EXTRN VCD_IRQ_Conflict_Msg:BYTE
EXTRN VCD_30_Comm_Driver_Msg:BYTE

VCD_not_idle_proc_addr	dd  OFFSET32 VCD_Bogus_Idle

VCD_Xmit_CallBack_CSIP	dd  0

Max_Port	dd  4

VCD_table	dd  ?		    ; Pointer to array of pointers to VCD_COM_Struc
				    ;	Before Device_Init the table is an
				    ;	array of VCD_Init_Data_Struc entries
				    ;	which hold additional info about each
				    ;	port that is needed while initialization
				    ;	is in progress.

;
; WARNING: THESE FOUR VARIABLES ARE TREATED AS A 4 ELEMENT DWORD ARRAY.
;	Do not shuffle them around.
;
VCD_COM1	dd	0		; Null pointer if port dosen't exist
VCD_COM2	dd	0		; Null pointer if port dosen't exist
VCD_COM3	dd	0		; Null pointer if port dosen't exist
VCD_COM4	dd	0		; Null pointer if port dosen't exist


Default_Assign = 2000		    ; auto-assign if port is unused after 2 sec

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


;******************************************************************************

VxD_ICODE_SEG

EXTRN VCD_Relocate_COMM_Int:NEAR


;******************************************************************************
;
;   VCD_verify_base
;
;   DESCRIPTION:    Validate COM port from base I/O port address by
;		    first reading Interrupt Identification Register and
;		    verifying that reserved bits 4 & 5 are clear.  Then
;		    performing a write/read cycle on the Line Control
;		    Register.  Verify can be disabled with an INI switch.
;
;   ENTRY:	    EAX = base I/O port address
;		    EBX = dword index (0, 4, 8, ... for COM1, COM2, ...)
;
;   EXIT:	    Carry clear if base ok
;
;   USES:	    EDX, Flags
;
;==============================================================================
BeginProc VCD_verify_base

	cmp	[verify_flag], 0	;Q: need to verify user base?
	jz	short no_verify 	;   N:

	push	eax
	mov	edx, eax
	add	edx, UART_IIR
	in	al, dx
	IO_Delay
	test	al, 030h		;Q: any reserved bits set?
	jnz	short bad_base		;   Y: assume bad port
	inc	edx
	.errnz	UART_LCR-UART_IIR-1
	in	al, dx
	IO_Delay
	mov	ah, al
test_pattern = 00101010b
	mov	al, test_pattern
	out	dx, al
	IO_Delay
	in	al, dx
	IO_Delay
	xchg	al, ah
	out	dx, al			; restore original
	cmp	al, test_pattern	;Q: were we able to change the value?
	jne	short bad_base		;   N: assume bad port
	pop	eax

no_verify:
	clc
	ret

bad_base:
	pop	eax
IFDEF DEBUG
	shr	ebx, 2
	Debug_Out 'VCD:  bad base (#ax) specified for COM#bl'
	shl	ebx, 2
ENDIF
	stc
	ret

EndProc VCD_verify_base


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


;******************************************************************************
;
;   VCD_Sys_Crit_Init
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
BeginProc VCD_Sys_Crit_Init

	xor	esi, esi		    ; Win386 section
	mov	eax, [Max_Port]
	mov	edi, OFFSET32 Com_Ports_Ini
	VMMCall Get_Profile_Decimal_Int
	mov	[Max_Port], eax

	mov	edx, SIZE VCD_Init_Data_Struc
	mul	edx
	VMMCall _HeapAllocate, <eax, HeapZeroInit>
	test	eax, eax		    ; Q: Did we get the memory?
	jz	sci_failed_alloc	    ;	 N:
	mov	[VCD_table], eax

	call	VCD_Relocate_COMM_Int
	mov	eax, [VCD_COMM_Int_Addr]
	mov	[VCD_Windows_Int], eax

	VMMCall Get_Machine_Info
	test	ebx, GMIF_MCA		    ; Q: Micro channel?
	jz	short @F		    ;	N:
	mov	[Def_IRQs+2], 3 	    ;	Y: default IRQ for COM3 is 3
	or	[device_init_flag], fMCA_machine
@@:

	BEGIN_Touch_1st_Meg
	cmp	ds:[VCD_Comm_0_Base_Addr], 2F8h ; Q: COM2 in COM1 slot?
	END_Touch_1st_Meg
	jne	short @F			;   N: leave default IRQ as 4
	mov	[Def_IRQs], 3			;   Y: change default IRQ to 3
@@:

IFDEF SHARE_IRQ
;
; Check the INI file for IRQ sharability
;
	xor	eax, eax		    ; default = FALSE
	test	ebx, GMIF_MCA OR GMIF_EISA  ; Q: Micro channel or EISA?
	jz	SHORT not_MCA_EISA	    ;	N:
	or	eax, -1 		    ;	Y: default = TRUE
not_MCA_EISA:
	mov	edi, OFFSET32 COM_IRQs_Sharable
	VMMCall Get_Profile_Boolean
	or	eax, eax
	jz	short not_sharable

	mov	[VCD_Disable_Mask], Mask_For_Sharing
	mov	[VCD_Def_Int_Proc], OFFSET32 VCD_Sharable_Int
	mov	[VCD_Def_EOI_Proc], OFFSET32 VCD_Sharable_EOI
	mov	eax, [VCD_Sharable_COMM_Int_Addr]
	mov	[VCD_Windows_Int], eax
not_sharable:
ENDIF

;
; Check the INI file for interrupt boost time specifications
;
	mov	eax, Int_Boost_Amount
	mov	ecx, 3
	xor	esi, esi
	mov	edi, OFFSET32 Com_Boost_Time
	VMMCall Get_Profile_Fixed_Point
	mov	[VCD_Int_Boost_Amount], eax
;
; Check the INI file for base and/or irq specifications
;
	xor	eax, eax
	xor	esi, esi
	mov	edi, OFFSET32 verify_base_ini
	VMMCall Get_Profile_Boolean
	mov	[verify_flag], al

	xor	ebx, ebx
	mov	esi, [VCD_table]
sci_scan_lp:
	mov	edi, OFFSET32 VCD_INI_Num
	call	VCD_add_portnum

	cmp	ebx, 4
	jae	short sci_chk_ini
	BEGIN_Touch_1st_Meg
	movzx	eax, word ptr [ebx*2][VCD_Comm_0_Base_Addr]
	END_Touch_1st_Meg
	or	eax, eax
	jnz	short sci_got_base
	test	[device_init_flag], fMCA_machine
	jnz	short sci_chk_ini
	cmp	ebx, 2
	jne	short sci_chk_ini
	mov	eax, 3E8h		; default COM3 to 3E8h
sci_chk_ini:
	push	esi
	xor	esi, esi
	mov	edx, OFFSET32 VCD_Base_Ini
	call	VCD_copy_ini_string
	push	edi			; save next pos in ini buf
	mov	edi, OFFSET32 VCD_INI_Buf
	VMMCall Get_Profile_Hex_Int
	pop	edi
	pop	esi
	or	eax, eax
	jz	short sci_next
	call	VCD_verify_base
	jc	short sci_next

sci_got_base:
	mov	[esi.VCD_id_base], ax

	xor	eax, eax
	cmp	ebx, 4
	jae	short sci_no_def_irq
	movzx	eax, [ebx+Def_IRQs]
sci_no_def_irq:
	push	esi
	xor	esi, esi
	mov	edx, OFFSET32 VCD_IRQ_Ini
	call	VCD_copy_ini_string
	mov	edi, OFFSET32 VCD_INI_Buf
	VMMCall Get_Profile_Decimal_Int
	pop	esi
	or	eax, eax
	jz	short sci_invalid_irq
	cmp	eax, 15
	ja	short sci_invalid_irq
	mov	[esi.VCD_id_irq], al

sci_next:
	add	esi, SIZE VCD_Init_Data_Struc
	inc	ebx
	cmp	ebx, [Max_Port]
	jb	sci_scan_lp
	ret

sci_invalid_irq:
IFDEF DEBUG
	inc	ebx
	Debug_Out "VCD:  invalid IRQ (##al) specified for COM#bl"
	dec	ebx
ENDIF
	mov	[esi.VCD_id_base], 0	    ; invalidate base
	jmp	short sci_next

sci_failed_alloc:
	Debug_Out 'VCD: failed to get memory for initialization'
%OUT fatal mem error?
	stc
	ret

EndProc VCD_Sys_Crit_Init


;******************************************************************************
;
;   NAME:
;	VCD_Device_Init
;
;   DESCRIPTION:
;	This procedure will initialize all COM ports.  It converts the table
;	pointed to by [VCD_table] from a table of VCD_Init_Data_Struc to
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
	mov	ecx, [Max_Port]
	mov	esi, [VCD_table]
	mov	edi, esi
init_ports:
	cmp	[edi.VCD_id_base], 0		;Q: COM exists?
	je	short skip_port 		;   N:
	cmp	[edi.VCD_id_coms], 0		;Q: COM exists & not alloc'ed?
	jne	short skip_port 		;   N:
	pushad
	movzx	eax, [edi.VCD_id_base]		; eax = base port
	movzx	ebx, [edi.VCD_id_irq]		; ebx = irq #
	neg	ecx
	add	ecx, [Max_Port] 		; ecx = hard com #
	inc	ecx
	call	VCD_Init_Port
	jnc	short @F
	mov	[edi.VCD_id_base], 0
@@:
	popad

skip_port:
	mov	eax, [edi.VCD_id_coms]
	mov	[esi], eax
	add	esi, 4
	add	edi, SIZE VCD_Init_Data_Struc
	loop	init_ports

	mov	eax, [Max_Port]
	shl	eax, 2
	VMMCall _HeapReAllocate, <[VCD_table], eax, 0>
	or	eax, eax
	jz	sci_failed_alloc
	mov	[VCD_table], eax	    ; in case address changes!

;
;   Hook BIOS interrupt 14h
;
	mov	eax, 14h
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

	mov	ecx, [Max_Port]
	mov	edi, [VCD_table]
instance_port_device:
	mov	esi, [edi]
	or	esi, esi
	jz	short @F
	lea	esi, [esi.VCD_Name]
	VxDCall DOSMGR_Instance_Device
@@:
	add	edi, 4
	loop	instance_port_device
	clc
	ret

EndProc VCD_Init_Complete


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

	mov	edx, eax		; as a simple test of com port
	add	dl, UART_IIR		;   existance, verify that a couple
	in	al, dx			;   of reserved bits in the interrupt
	IO_Delay			;   id register are 0
	test	al, 30h
	jz	SHORT VCD_Looks_Like_A_UART
	popad
	jmp	VCD_Dont_Virt_Port

VCD_Looks_Like_A_UART:

;;	  Trace_Out "VCD INFO:  COM #AX exists"

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
	or	[esi.VCD_Flags], VCD_IRQ_Init	;   Y: flag IRQ as initialized
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
	test	[device_init_flag], fDevice_Init_Done
	jnz	short no_com_port
	cmp	eax, [Max_Port]
	ja	short no_com_port
	sub	eax, 1
	jb	short no_com_port

	cmp	[esi.VPS_Hw_Int_Proc], 0
	je	short no_com_port

	imul	eax, eax, SIZE VCD_Init_Data_Struc
	add	eax, [VCD_table]

	cmp	[eax.VCD_id_base], 0	    ;Q: COM port exists?
	je	short no_com_port	    ;	N:
	cmp	[eax.VCD_id_virt], 0	    ;Q: COM port virtualized?
	jne	short com_virtualized	    ;	Y:

	mov	[eax.VCD_id_extra], ecx
	mov	[eax.VCD_id_CBextra], edx

	mov	edi, eax
	mov	ecx, [esp.Pushad_EAX]	    ; ecx = hard com #
	movzx	ebx, [edi.VCD_id_irq]	    ; ebx = irq #
	movzx	eax, [edi.VCD_id_base]	    ; eax = base port
	call	VCD_Init_Port
	jc	short no_com_port

	mov	edi, [edi.VCD_id_coms]
	test	byte ptr [esp.Pushad_EBX], 1
	jnz	short IRQ_sharable
	or	[edi.VCD_Flags], VCD_not_sharable_IRQ
IRQ_sharable:

	mov	esi, [esp.Pushad_ESI]
	mov	[edi.VCD_virt_procs], esi
	or	[edi.VCD_Flags], VCD_Virtualized    ; flag as virtualized
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



VxD_CODE_SEG


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
	test	[VCD_Global_Flags], fVCD_Init_Complete
	jz	short @F
	VxDCall VMPoll_Reset_Detection
@@:
VCD_Bogus_Idle LABEL NEAR
	ret

EndProc VCD_not_idle


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

	mov	eax, 30Ah
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
	cmp	eax, [Max_Port]
	ja	short VSPG_no_port	; return Carry set, if port # > max
	sub	eax, 1
	jb	short VSPG_invalid_port ; return Carry set, if port # < 1
	shl	eax, 2
	add	eax, [VCD_table]
	mov	eax, [eax]
	or	eax, eax
	jz	short VSPG_no_port
	or	edx, edx		; Q: global port?
	jnz	short VSPG_not_global	;   N:
	or	[eax.VCD_Flags], VCD_global_port
	push	ebx
	VMMCall Get_Cur_VM_Handle	; set owner to cur VM
	jmp	short VSPG_exit

VSPG_not_global:
	and	[eax.VCD_Flags], NOT VCD_global_port
	push	ebx
	mov	ebx, [eax.VCD_Owner]	; set owner to current owner
	or	ebx, ebx
	jz	short VSPG_no_owner

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
	cmp	eax, [Max_Port]
	ja	short gf_no_port	; return Carry set, if port # > max
	sub	eax, 1
	jb	short gf_invalid_port	; return Carry set, if port # < 1
	mov	ebx, [VCD_table]
	mov	ebx, [eax*4][ebx]
	or	ebx, ebx
	jz	short gf_no_port
	mov	ebx, [ebx.VCD_Owner]
	stc
gf_no_port:
	cmc
gf_invalid_port:
	pop	eax
	ret

EndProc VCD_Get_Focus


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

	Control_Dispatch Sys_Critical_Init,	VCD_Sys_Crit_Init
	Control_Dispatch Device_Init,		VCD_Device_Init
	Control_Dispatch Init_Complete, 	VCD_Init_Complete
	Control_Dispatch Create_VM,		VCD_Create_VM
	Control_Dispatch Destroy_VM,		VCD_Destroy_VM
	Control_Dispatch Set_Device_Focus,	VCD_Set_Focus
	Control_Dispatch VM_Suspend,		VCD_Suspend_VM
IFDEF DEBUG
	Control_Dispatch Debug_Query,		VCD_Debug_Dump
ENDIF
	clc
	ret

EndProc VCD_Control


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

BeginProc VCD_PM_Svc_Call

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
	and	[ebp.Client_EFLAGS], NOT CF_Mask; Clear carry
	Call	VCD_PM_Call_Table[eax*4]	; Call appropriate code
	ret

VCDCall_Bad:
IFDEF DEBUG
	Debug_Out "VCD ERROR: Invalid function #EAX on VCD_PM_Svc_Call"
ENDIF
VCDCall_Bad2:
	mov	[ebp.Client_EAX], 0FFFFFFFFh

VCD_PM_API_Failed:
	or	[ebp.Client_EFLAGS], CF_Mask	; Set carry
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
BeginProc VCD_PM_Get_Port_Array

	xor	eax,eax
	mov	esi, [VCD_table]
	mov	ecx, [Max_Port]
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
BeginProc VCD_PM_Get_Data_Ptr

	movzx	ecx,[ebp.Client_CX]
	cmp	ecx, [Max_Port] 		; Port index in range?
	jae	short VCD_gdp_bad		; No
	shl	ecx, 2
	add	ecx, [VCD_table]
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
BeginProc VCD_PM_Get_Port_Behavior

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
BeginProc VCD_PM_Set_Port_Behavior

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
BeginProc VCD_PM_Acquire_Port

	call	VCD_PM_Get_Data_Ptr
	jc	short ap_no_port		; Port is not valid

	mov	esi, ecx

	cmp	[esi.VCD_IObase], 0		;Q: non-zero base?
	je	short ap_no_port		;   N: can't assign port
	cmp	[esi.VCD_IRQN], -1		;Q: irq disabled?
	je	short ap_no_port		;   Y: can't assign port

	call	VCD_Virtualize_IRQ
	test	[esi.VCD_Flags], VCD_Owns_IRQ	;Q: The COM port owns it's IRQ?
	jz	short ap_irq_busy		;   N: can't assign port

;
; If COM1 or COM2 and current BIOS specified base is 0 and it has been
; declared as a global port, then don't allow it to be acquired.
;
	movzx	eax, [ebp.Client_CX]
	cmp	al, 2
	jae	short @F
	test	[esi.VCD_Flags], VCD_global_port ;Q: global port?
	jz	short @F			;   N:
	BEGIN_Touch_1st_Meg
	cmp	word ptr [eax*2+VCD_Comm_0_Base_Addr], 0 ; Q: COM1 or COM2 base zeroed?
	END_Touch_1st_Meg
	je	short ap_no_port
@@:

;
; This has to be done in this order!  We first check for no current owner,
; if true, then go ahead and assign.  Then check for "forced assignment", if
; true, then jump down to see if owner is different.  If we check for "forced
; assignment" first, then we do the wrong thing on the "is owner different"
; check when the port doesn't have an owner (0 is different than cur VM handle)
;
	mov	ecx, [esi.VCD_Owner]
	jecxz	SHORT VCD_PM_OK_To_Assign	; jump if no current owner

	test	[ebp.Client_AL], 1		;Q: forced assignment?
	jz	SHORT VCD_PM_Not_Forced 	;   N:

	add	ecx, [esi.VCD_CB_Offset]
	or	[ecx.VCD_CB_Flags], VCD_Contended ; flag failed contention
	jmp	SHORT VCD_PM_OK_To_Assign

ap_irq_busy:
;
; message box here to state IRQ conflict
;
	mov	eax, MB_OK + MB_ASAP + MB_ICONEXCLAMATION + MB_NOWINDOW
	mov	ecx, OFFSET32 VCD_IRQ_Conflict_Msg
	xor	edi, edi			; Use VM name for caption
	xor	esi, esi			; No call back
	VxDCall SHELL_SysModal_Message		; send message to Windows

ap_no_port:
	and	[ebp.Client_EFLAGS], NOT ZF_Mask    ; clear Z-flag
	jmp	VCD_PM_API_Failed

VCD_PM_Not_Forced:
	or	[ebp.Client_EFLAGS], ZF_Mask	; set Z-flag - assume port owned

	cmp	[esi.VCD_Owner], ebx		;Q: Attach to current owner?
	je	SHORT VCD_PM_OK_To_Assign	;   Y: Done

	test	[esi.VCD_Flags], VCD_global_port ;Q: normal contention handling?
	jnz	SHORT VCD_PM_OK_To_Assign	;   N: steal the port
	cmp	[esi.VCD_Auto_Assign], 0	;Q: INI specified auto assign?
	je	SHORT VCD_PM_OK_To_Assign	;   Y: steal the port
	jl	VCD_PM_API_Failed		;   N:
						;   N: INI specified timeout

	VMMCall Get_System_Time
	sub	eax, [esi.VCD_Last_Use]
	cmp	eax, [esi.VCD_Auto_Assign]	;      Q: Can we steal port?
	jb	VCD_PM_API_Failed		;	  N: put contention dlg

VCD_PM_OK_To_Assign:

IFDEF DEBUG_verbose
	mov	al, [esi.VCD_Number]
	Trace_Out 'Windows acquiring COM#al'
ENDIF
	cmp	ebx, [esi.VCD_Owner]		;Q: VM already owns port?
	jne	short VPAP_new_owner		;   N:
	push	ebx
	xor	ebx, ebx			;   Y: clear owner
	call	VCD_Set_Owner
	pop	ebx
VPAP_new_owner:
	mov	edi, ebx
	add	edi, [esi.VCD_CB_Offset]

	mov	dword ptr [edi.VCD_IER], 0	; zero IER, LCR, MCR & Read_Stat
						;   let COMM.DRV do the programming
.errnz VCD_LCR - VCD_IER - 1
.errnz VCD_MCR - VCD_LCR - 1
.errnz VCD_Read_Stat - VCD_MCR - 1

;
; check for 3.10 driver or greater which supports a ring0 callable int handler
;
	test	[ebp.Client_AL], 110b		;Q: 3.0 driver?
	jz	short VPAP_3_00_drvr		;   Y: finish init for
						;	version 3.00 driver

	test	[ebp.Client_AL], 100b		;Q: acquire for mouse driver
	jnz	short VPAP_mouse_port		;   Y: no COMDEB

	or	[edi.VCD_CB_Flags], VCD_CB_Windows_Port

	Client_Ptr_Flat eax, ds, si		; edi = linear address of COMDEB
	movzx	ecx, [ebp.Client_DI]
;
; pass callback & reference data back to COMM.DRV
;
	mov	[ebp.Client_CX], cs
	mov	[ebp.Client_EDI], OFFSET32 VCD_COMMDRV_Int
	mov	[ebp.Client_EBX], esi

VPAP_set_owner:
	mov	[esi.VCD_COMDEB], eax
	add	eax, ecx
	mov	[esi.VCD_COMDEB_Flag], eax

	call	VCD_Set_Owner			; Set owner to cur VM

	mov	eax, [esi.VCD_IObase]
	mov	[ebp.Client_AX], ax
	movzx	eax, [esi.VCD_IRQN]
	mov	[ebp.Client_DX], ax
	ret

VPAP_mouse_port:
	or	[edi.VCD_CB_Flags], VCD_CB_Mouse_Port
	xor	eax, eax
	xor	ecx, ecx
	jmp	VPAP_set_owner

VPAP_3_00_drvr:
	cmp	[VCD_Setup_Ptrs_Addr], 0	;Q: 3.0 drivers supported?
	jne	SHORT ap_30_supported		;   Y:

; 3.0 comm driver installed, but COMMDRV30 is not set to true in SYSTEM.INI.
; Tell user to set it!

	mov	eax, MB_OK+MB_ASAP+MB_ICONHAND
	mov	ecx, OFFSET32 VCD_30_Comm_Driver_Msg
	xor	edx, edx
	xor	esi, esi
	xor	edi, edi
	VxDcall SHELL_Message
	jnc	SHORT @F			; jump if successful
	xor	edi, edi
	mov	eax, MB_OK+MB_ASAP+MB_ICONHAND+MB_SYSTEMMODAL
	VxDCall SHELL_SYSMODAL_Message		;   Do sysmodal msg NOW
@@:
	jmp	ap_no_port

ap_30_supported:
	or	[edi.VCD_CB_Flags], VCD_CB_Windows_Port OR VCD_CB_Windows_30Drvr
	call	VCD_Set_Owner			; Set owner to cur VM
	mov	eax, [VCD_Xmit_CallBack_CSIP]
	or	eax, eax
	jnz	SHORT callback_allocated

	push	esi
	xor	edx, edx
	mov	esi, [VCD_Xmit_CallBack_Addr]
	VMMCall Allocate_PM_Call_Back
	pop	esi
	mov	[VCD_Xmit_CallBack_CSIP], eax

callback_allocated:
	mov	[ebp.Client_DI], ax
	shr	eax, 16
	mov	[ebp.Client_ES], ax
	callret [VCD_Setup_Ptrs_Addr]

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
BeginProc VCD_PM_Free_Port

	call	VCD_PM_Get_Data_Ptr
	jc	VCD_PM_API_Failed		; Port is not valid
	mov	esi, ecx

IFDEF DEBUG_verbose
	mov	al, [esi.VCD_Number]
	Trace_Out 'Windows freeing COM#al'
ENDIF

	mov	edi, ebx
	add	edi, [esi.VCD_CB_Offset]
	test	[edi.VCD_CB_Flags], VCD_CB_Windows_Port OR VCD_CB_Mouse_Port
	jz	short free_exit

	and	[edi.VCD_CB_Flags], NOT VCD_CB_Windows_Bits

	cmp	[esi.VCD_Owner], ebx		; Q: Attached to current owner?
	jne	short free_exit 		;   N:

	xor	ebx, ebx			; Owner = Nobody
	call	VCD_Set_Owner			; Set to no owner

free_exit:
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
BeginProc VCD_PM_Steal_Port

	mov	[ebp.Client_AL], 0
	call	VCD_PM_Get_Data_Ptr
	jc	VCD_PM_API_Failed		; Port is not valid

	mov	esi, ecx
	call	VCD_Assign
	or	ecx, ecx
	jnz	VCD_PM_API_Failed		; can't steal
	mov	[ebp.Client_AL], -1
	ret

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

	cmp	esi, [Max_Port]
	ja	short setf_exit 	    ; return Carry set, if port # > max
	sub	esi, 1
	jb	short setf_exit 	    ; return Carry set, if port # < 1

	shl	esi, 2
	add	esi, [VCD_table]
	mov	esi, [esi]
	or	esi, esi			    ;Q: port valid?
	jz	short not_for_VCD		    ;	N: ignore call

	mov	edx, ebx
	test	[esi.VCD_Flags], VCD_global_port    ;Q: global port?
	jz	short assign_owner		    ;	N:
						    ;	Y: set new owner VM handle

	mov	ebx, [esi.VCD_Owner]
	or	ebx, ebx			    ;Q: port owned?
	jz	short assign_owner		    ;	N: set new owner

	test	[esi.VCD_Flags], VCD_Owns_IRQ	    ; Q: Does COM own the IRQ?
	jz	SHORT assign_owner		    ;	 N: Can't do anything
	mov	eax, [esi.VCD_IRQ_Handle]
	VxDCall VPICD_Get_Complete_Status
	test	ecx, VPICD_Stat_Virt_Req	    ;Q: int requested for old owner?
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

	mov	edi, [VCD_table]
	mov	ecx, [Max_Port]
suspend_lp:
	push	ecx
	mov	esi, [edi]
	or	esi, esi		;Q: COM port exists?
	jz	short suspend_next	;   N:
	cmp	[esi.VCD_Owner], ebx	;Q: VM owns COM port?
	jne	short suspend_next	;   N:

	test	[esi.VCD_Flags], VCD_Owns_IRQ	; Q: Does COM own the IRQ?
	jz	SHORT suspend_next		;    N: Can't do anything
	mov	eax, [esi.VCD_IRQ_Handle]
	call	VCD_Clear_Int		; clear any outstanding int

suspend_next:
	add	edi, 4
	pop	ecx
	loop	suspend_lp
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

	test	[esi.VCD_Flags], VCD_Virtualized    ;Q: virtualized?
	jz	short VCD_Ignore		    ;	N:
	test	[esi.VCD_Flags], VCD_global_port OR VCD_Windows_Port OR VCD_Mouse_Port
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
	test	[esi.VCD_Flags], VCD_Virtualized    ;Q: virtualized?
	jz	short no_notify 		    ;	N:
	test	[esi.VCD_Flags], VCD_Mouse_Port     ;Q: mouse owns port?
	jnz	short no_notify 		    ;	Y:

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
	test	[esi.VCD_Flags], VCD_Windows_Port OR VCD_Mouse_Port
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

	mov	ecx, [Max_Port]
	mov	esi, [VCD_table]
init_CB_port_info:
	push	esi
	mov	esi, [esi]			; ESI -> COM1's data structure
	call	VCD_Init_VM_CB			; Initialize the control block
	pop	esi
	add	esi, 4
	loop	init_CB_port_info

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
	mov	edi, [esi.VCD_CB_Offset]	; EDI = Offset in control block
	lea	edi, [edi][ebx.VCD_BAUD_LSB]	; EDI -> Destination address
	test	[esi.VCD_Flags], VCD_global_port ;Q: global port?
	jz	short ivcb_use_defaults 	;   N:
	push	ebx
	mov	ebx, [esi.VCD_Owner]
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
	test	ecx, VPICD_Stat_Virt_Dev_Req	;Q: we requested int?
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
	Assert_Ints_Disabled

	pushad

IFDEF DEBUG
	mov	al, [esi.VCD_Number]
	Queue_Out 'VCD_Attach COM#al to #ebx'
ENDIF

	test	[esi.VCD_Flags], VCD_Virtualized ;Q: virtualized?
	jz	short va_not_virt		 ;  N: disable trapping
	test	[esi.VCD_Flags], VCD_global_port ;Q: global port?
	jnz	short va_not_virt		 ;  Y: disable trapping
	test	[ebx][ecx.VCD_CB_Flags], VCD_CB_Windows_Port OR VCD_CB_Mouse_Port
						 ;Q: windows port?
	jz	short va_virt			 ;  N: leave trapping enabled


va_not_virt:
	call	VCD_Disable_Trapping

	test	[ebx][ecx.VCD_CB_Flags], VCD_CB_Windows_Port
	jz	short va_virt
	test	[ebx][ecx.VCD_CB_Flags], VCD_CB_Windows_30Drvr
	jnz	short va_virt
	mov	eax, [esi.VCD_COMDEB_Flag]	; if 3.1 COMM.DRV, then
	or	eax, eax
	jz	short va_virt
	and	byte ptr [eax], NOT fCOM_ignore_ints ;clear flag so COMM.DRV
						;   will handle ints again
va_virt:

	test	[esi.VCD_Flags], VCD_Owns_IRQ	; Q: Does COM own the IRQ?
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

	test	[esi.VCD_Flags], VCD_Owns_IRQ	; Q: Does COM own the IRQ?
	jz	SHORT attach_skip_auto		;    N: Can't do anything
	mov	eax, [esi.VCD_IRQ_Handle]
	VxDCall VPICD_Set_Auto_Masking
attach_skip_auto:

	popad
	ret

EndProc VCD_Attach


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
	test	[esi.VCD_Flags], VCD_Windows_Port
	jz	short @F
	test	[esi.VCD_Flags], VCD_Windows_30Drvr
	jnz	short @F
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
	and	[esi.VCD_Flags], NOT VCD_CB_Windows_Bits
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

	test	[esi.VCD_Flags], VCD_IRQ_Init	; Q: Has IRQ been initialized?
	jnz	VCD_IRQ_Initialized		;    Y: Attach to new owner
	push	ecx				;    N: Virtualize IRQ

	sub	esp, SIZE VPICD_IRQ_Descriptor
	mov	edi, esp			; edi -> VPICD_IRQ_Descriptor

	movzx	eax, [esi.VCD_IRQN]
	mov	[edi.VID_IRQ_Number], ax
	xor	ecx, ecx
	test	[esi.VCD_Flags], VCD_not_sharable_IRQ
	jnz	short vi_IRQ_not_sharable
	or	ecx, VPICD_Opt_Can_Share
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
	or	[esi.VCD_Flags], VCD_Owns_IRQ	; The COM port owns it's IRQ

; There may be an int. request lying around. If so, transfer it to
; owner of virtualized int.
	VxDCall VPICD_Get_Complete_Status	; Check status of int.
	test	ecx, VPICD_Stat_Phys_In_Serv	; Q: Is it phys. in Service?
	jz	SHORT @F			;    N:
	VxDCall VPICD_Set_Int_Request		;    Y: pass int to new owner

@@:
	movzx	eax, [esi.VCD_IRQN]
	bts	[VCD_IRQs_virtualized], eax	;Q: first IRQ is virtualized?
	jc	short VCD_IRQ_Init_Done 	;   N:
	or	[esi.VCD_Flags], VCD_1st_on_IRQ ;   Y: flag it
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
	or	[esi.VCD_Flags], VCD_IRQ_Init	; IRQ has been initialized
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
	and	[esi.VCD_Flags], NOT VCD_SetOwner_Event
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
	pushfd					; Critical section
	cli

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

	test	[esp+4], IF_Mask		; Q: ints originally enabled?
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
	test	[esi.VCD_Flags], VCD_Owns_IRQ	; Q: Does COM own the IRQ?
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

	test	[esi.VCD_Flags], VCD_Owns_IRQ	; Q: Does COM own the IRQ?
	jz	SHORT @F			;    N: Can't do anything
	mov	eax, [esi.VCD_IRQ_Handle]
	VxDCall VPICD_Physically_Unmask 	  ; this will cause the receive int
@@:
	sti					; to occur and be processed
	sub	dl, UART_MCR-UART_IER		; clear the receive int enable
	xor	eax, eax
	out	dx, al
	cli
	mov	[esi.VCD_Owner], eax		; Set owner to nil
	test	[esi.VCD_Flags], VCD_Owns_IRQ	; Q: Does COM own the IRQ?
	jz	SHORT detach_complete		;    N: Can't do anything
	mov	eax, [esi.VCD_IRQ_Handle]
	VxDCall VPICD_Set_Auto_Masking
detach_complete:

	pop	edx				; previous owner
	call	VCD_Notify_VxD			; notify if necessary

	jmp	SHORT End_VCD_Set_Owner
;
;   Assign to new owner
;
VCD_Set_New_Owner:
	call	VCD_Virtualize_IRQ
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
	popfd					; Restore flags
	popad					; And registers

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

	mov	ecx, [Max_Port]
	mov	esi, [VCD_table]
destroy_lp:
	push	esi
	mov	esi, [esi]			; ESI -> COM1's data structure
	call	VCD_Detach_If_Owner		; Initialize the control block
	pop	esi
	add	esi, 4
	loop	destroy_lp

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
;
;   USES:
;	ECX, Flags
;
;------------------------------------------------------------------------------

BeginProc VCD_Assign

	pushad					; Save all registers

	cmp	[esi.VCD_Owner], ebx		; Q: Attach to current owner?
	je	SHORT VCD_Assigned_OK		;    Y: Done
	cmp	[esi.VCD_Owner], 0		; Q: Any VM own device?
	je	SHORT VCD_OK_To_Assign		;    N: Assign it

	test	[esi.VCD_Flags], VCD_global_port ;Q: normal contention handling?
	jnz	SHORT VCD_Ignore_Contention	;   N: do NOT steal the port
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
	or	[edx+eax.VCD_CB_Flags], VCD_Contended	  ; set contended flag in old owner
	and	[edx+ebx.VCD_CB_Flags], NOT VCD_Contended ; clear contended flag in new owner
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

EndProc VCD_Assign


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
	or	[edi.VCD_CB_Flags], VCD_Touched

	cmp	ebx, [esi.VCD_Owner]		; Q: Do we own the device?
	jne	SHORT virtualize_io		;   N:

	test	[esi.VCD_Flags], VCD_Virtualized
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
	cmp	al, 2				;Q: saved id = xmit buf empty?
	je	SHORT Ret_THRE_id		;   Y: return saved id
	in	al, dx				;   N: re-read id
Ret_THRE_id:
	ret


virt_owner_io:
	test	[esi.VCD_Flags], VCD_global_port OR VCD_Windows_Port OR VCD_Mouse_Port
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
	dd	OFFSET32 VCD_Ignore_IO		  ; Interrupt Identity
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
	call	VCD_Assign			; Assign the COM port
	test	ecx, ecx			; Q: Successful?
	pop	ecx
	jz	VCD_Dispatch_IO_Again		;    Y: Do normal IO
VCD_Ignore_IO:
	mov	al, -1				; Will return FFh if input
	ret					; IGNORE IT!!!!

EndProc VCD_Cant_Virtualize


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
	jnc	short vh_ret		; jump if serviced
	bt	[esi.VCD_Flags], VCD_1st_on_IRQ_Bit
	cmc
	jc	short vh_ret	; not the last so let it go
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
	test	[esi.VCD_Flags], VCD_Windows_Port
	jnz	VCDI_WinPort

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

	test	[esi.VCD_Flags], VCD_Windows_Port
	jnz	short VCDI_WinPort

	mov	ebx, [esi.VCD_Owner]
	test	ebx, ebx			; Q: Owned by any VM?
	jz	SHORT VCDI_assign		;   N: assign current VM
	inc	ebx				;Q: int caused during detach?
	jz	short VCDI_ignore		;   Y: consume it
	dec	ebx
VCD_Request_Int:
	test	[ebx.CB_VM_Status], VMStat_Suspended ;Q: VM suspended?
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
	test	[esi.VCD_Flags], VCD_Windows_30Drvr
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
BeginProc VCD_COMMDRV_Int, NO_LOG

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
	test	ecx, VPICD_Stat_Virt_Dev_Req	;Q: we requested int?
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
	call	VCD_Assign		;   Y: assign ownership
	ret

EndProc VCD_Mask


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
	cmp	edx, 4			    ;Q: port in range?
	jae	short reflect_swi_14	    ;	N: skip it
	mov	esi, [VCD_table]
	mov	esi, [edx*4][esi]
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

	mov	esi, [VCD_table]
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
	mov	esi, [VCD_table]
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

VxD_CODE_ENDS


	END
