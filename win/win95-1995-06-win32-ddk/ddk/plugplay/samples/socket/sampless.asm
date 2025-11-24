PAGE 58,132
;******************************************************************************
TITLE SAMPLESS.ASM - Sample Socket Services Driver
;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1993
;
;   Title:	SAMPLESS.ASM - Sample Socket Services Driver
;
;==============================================================================
;
;   DESCRIPTION:
;
;******************************************************************************

	.386p

;******************************************************************************
;			      I N C L U D E S
;******************************************************************************

	.XLIST
	INCLUDE vmm.inc
	INCLUDE debug.inc
	INCLUDE opttest.inc
	INCLUDE configmg.inc
	INCLUDE pccard.inc
	INCLUDE sslocal.inc
	INCLUDE ssdefs.inc
	INCLUDE regstr.inc
	.LIST


;******************************************************************************
;		 V I R T U A L	 D E V I C E   D E C L A R A T I O N
;******************************************************************************

Declare_Virtual_Device SAMPLESS, 1, 0, SS_Control, , UNDEFINED_INIT_ORDER


;******************************************************************************
;				 E Q U A T E S
;******************************************************************************

%OUT Make most of this VxD pageable!

VxD_PAGEABLE_DATA_SEG

szOptions	db	REGSTR_VAL_PCICOPTIONS, 0

irq_msk 	dw	PCIC_DEFAULT_IRQMASK	; Inverting ints avail (15-0)

VxD_PAGEABLE_DATA_ENDS


VxD_DATA_SEG

;------------------------------------------------------------------------------

;
;   Socket Services Function Dispatch Table
;
;   NOTE:  The table ends with Reset_Socket since EDC functions,
;   Prior_Handler, SS_Addr, and Get_Access_Offsets are not supported.
;   The functions Acknowledge_Interrupt, Get_Vendor_Info, and Vendor_Specific
;   are special-cased.	This saves about 100 bytes of space.
;

ALIGN 4

BASE_TABLE_FUNCTION EQU SSFn_GET_SS_INFO

Function_Table	LABEL DWORD
	dd	OFFSET32 Get_Socket_Services_Info   ; 83h
	dd	OFFSET32 Inquire_Adapter	    ; 84h
	dd	OFFSET32 Get_Adapter		    ; 85h
	dd	OFFSET32 Set_Adapter		    ; 86h
	dd	OFFSET32 Inquire_Window 	    ; 87h
	dd	OFFSET32 Get_Window		    ; 88h
	dd	OFFSET32 Set_Window		    ; 89h
	dd	OFFSET32 Get_Page		    ; 8ah
	dd	OFFSET32 Set_Page		    ; 8bh
	dd	OFFSET32 Inquire_Socket 	    ; 8ch
	dd	OFFSET32 Get_Socket		    ; 8dh
	dd	OFFSET32 Set_Socket		    ; 8eh
	dd	OFFSET32 Get_Status		    ; 8fh
	dd	OFFSET32 Reset_Socket		    ; 90h

FUNCTION_TABLE_ENTRIES	EQU ($-Function_Table)/4

;
;   State flags for socket serices
;
SS_Flags	dd	0

SSF_BUSY	EQU	00000001b
SSF_BUSY_BIT	EQU	0

Vendor_Specific     EQU SSErr_Invalid_Function


VxD_DATA_ENDS


;******************************************************************************
;	       D E V I C E   C O N T R O L   P R O C E D U R E
;******************************************************************************

VxD_CODE_SEG

;******************************************************************************
;
;   SS_Control
;
;   DESCRIPTION:
;	Control procedure for socket services driver.
;
;   ENTRY:
;	EAX = Control call ID
;
;   EXIT:
;	If carry clear then
;	    Successful
;	else
;	    Control call failed
;
;   USES:
;	EAX, EBX, ECX, EDX, ESI, EDI, Flags
;
;==============================================================================

BeginProc SS_Control

	Control_Dispatch PnP_New_Devnode, SS_New_Devnode

	clc				; Ignore other control calls
	ret

EndProc SS_Control


;******************************************************************************
;
;   SS_New_Devnode
;
;   DESCRIPTION:
;	Handles system control calls for PnP_NEW_DEVNODE.  This routine
;	allocates the reference data for the adapter, and registers as the
;	driver for the devnode.
;
;   ENTRY:
;	EBX = Devnode handle
;	EDX = Subfunction (should always be LOAD_DRIVER)
;
;   EXIT:
;	EAX = CR_RESULT
;
;   USES:
;	Can use all registers and flags
;
;==============================================================================

BeginProc SS_New_Devnode

	cmp	edx, DLVXD_LOAD_DRIVER	; Q: Is this the driver?
	jne	SSND_Error		;    N: Strange!!!

	mov	edi, ebx		; Save devnode in EDI

;   Allocate reference data area for this adapter.  The pointer to this
;   data will be registered with the PnP config manager and with Card
;   Services.  It will then be passed to this driver on each call from
;   those components.

	VMMcall _HeapAllocate, <SIZE Adapter_Handle, HeapZeroInit>
	test	eax, eax
	jz	SSND_Error

	mov	esi, eax		; Init the adapter reference data
	mov	[esi.AH_Devnode], edi
	mov	[esi.AH_Signature], ADAPT_SIG

	VxDcall _CONFIGMG_Register_Device_Driver, <edi, OFFSET32 SS_PnP_Handler, esi, 0>
	.ERRNZ	CR_SUCCESS
	test	eax, eax
	jnz	SSND_Couldnt_Reg_Driver

;   EAX contains CR_SUCCESS (0) here, so we just clear carry and return

	clc
	ret


;   Unable to register driver.	Free the adapter reference data

SSND_Couldnt_Reg_Driver:
	Debug_Out "SAMPLESS:  Register device driver failed for devnode #EDI"

	VMMcall _HeapFree, <esi, 0>

SSND_Error:
	Debug_Out "SAMPLESS:  Socket Services PnP_New_Devnode failed"
	mov	eax, CR_FAILURE
	stc
	ret

EndProc SS_New_Devnode


;******************************************************************************
;
;   SS_PnP_Handler
;
;   DESCRIPTION:
;	This is the plug and play driver entry point for socket services.
;	This routine is called by the config manager to notify this driver
;	of configuration related actions.
;
;   ENTRY:
;	cfFuncName = The function to perform.
;	sbfSubFuncName = The subfunction to perform.
;	dnToDevNode = Handle of the devnode being called.
;	dnAboutDevNode = Handle of the subject of the event.
;	ulFlags = Flags value.
;
;   EXIT:
;	EAX = CR_RESULT
;
;   USES:
;	EAX, ECX, EDX
;
;==============================================================================

BeginProc SS_PnP_Handler, CCALL

ArgVar	cfFuncName, DWORD
ArgVar	scfSubFuncName, DWORD
ArgVar	pdnDevNode, DWORD
ArgVar	dwRefData, DWORD
ArgVar	ulFlags, DWORD

	EnterProc
	push	ebx
	push	esi
	push	edi

	cmp	[cfFuncName], CONFIG_FILTER
	jne	SSPnP_Chk_Start

	;> Filter logical configurations for controller.
	;> This is called before start and may not be required.

	jmp	SSPnP_Success

SSPnP_Chk_Start:
	cmp	[cfFuncName], CONFIG_START
	jne	SSPnP_Chk_Stop

	;> Get the allocated logical configuration and init the controller.

	;> The following is a typical implementation to get the allocated
	;> logical configuration from the config manager.

	.ERRNZ	SIZE Config_Buff_s MOD 4
	sub	esp, SIZE Config_Buff_s
	mov	edi, esp

	VxDcall _CONFIGMG_Get_Alloc_Log_Conf, <edi, [pdnDevNode], CM_GET_ALLOC_LOG_CONF_ALLOC>
	test	eax, eax
	jz	@F
	Debug_Out "SAMPLESS:  Unexpected config manager error #EAX"
	jmp	SSPnP_Exit
@@:
	mov	esi, [dwRefData]	; ESI -> adapter reference data

	movzx	eax, [edi.wIOPortBase][0]

	;> EAX = I/O base address for the controller,
	;> save it here typically in reference data pointed to by ESI.

	mov	al, [edi.bIRQRegisters][0]

	;> AL = IRQ number for controller status change,
	;> save it here typically in reference data pointed to by ESI.

	add	esp, SIZE Config_Buff_s

	call	SS_Init_Adapter

	cmp	[esi.AH_NumSockets], 0	; Q: Are there any sockets?
	jne	SSPnP_Start

	Debug_Out "SAMPLESS:  Could not find any sockets"
	mov	eax, CR_DEVICE_NOT_THERE  ;  N: Return no device
	jmp	SSPnP_Exit

SSPnP_Start:

;   If this is the first time start then register with card services.

	cmp	[scfSubFuncName], CONFIG_START_FIRST_START
	jne	SSPnP_Success

;   This is the first start call for this adapter.  Register with card
;   services.

	.ERRNZ	SIZE AddSocketServices_s - 6  ; Reserve 2 dwords of stack
	sub	esp, 8
	mov	ebx, esp

	mov	[ebx.AddSocketServices_s.Attributes], SSA_FLAT_PROTECT_MODE
	mov	[ebx.AddSocketServices_s.DataPointer], esi

	mov	al, F_ADD_SOCKET_SERVICES
	lea	esi, Socket_Services_Entry
	xor	edx, edx
	mov	ecx, SIZE AddSocketServices_s
	VxDcall PCCARD_Card_Services

	add	esp, 8

	.ERRNZ R_SUCCESS
	jmp	SSPnP_Exit

SSPnP_Chk_Stop:
	cmp	[cfFuncName], CONFIG_STOP
	jne	SSPnP_Chk_Test

	;> The controller devnode is being stopped, this driver must stop
	;> using the allocated configuration.  It will be called again with
	;> a start call to receive a new allocated configuration.

	jmp	SSPnP_Success

SSPnP_Chk_Test:
	cmp	[cfFuncName], CONFIG_TEST
	jne	SSPnP_Chk_Remove

	;> Check to see if any sockets are currently being used by the
	;> controller, if not return CR_SUCCESS.

	jmp	SSPnP_Success

SSPnP_Chk_Remove:
	cmp	[cfFuncName], CONFIG_REMOVE
	jne	SSPnP_Chk_Apm

	;> The controller devnode is being removed, this driver must stop
	;> using the allocated configuration.  Any allocated data for this
	;> devnode should be freed.  If the driver is dynamically loaded it
	;> will be unloaded by the config manager.  This driver will not be
	;> called again after this call.

	VMMcall _HeapFree, <dwRefData, 0>
	jmp	SSPnP_Success

SSPnP_Chk_Apm:
	cmp	[cfFuncName], CONFIG_APM
	jne	SSPnP_Chk_Test_Failed

	;> Respond to power management requests passed in scfSubFuncName.

	jmp	SSPnP_Success

SSPnP_Chk_Test_Failed:
	cmp	[cfFuncName], CONFIG_TEST_FAILED
	jne	SSPnP_Chk_Test_Succeeded

	;> Continue as before the CONFIG_TEST call.

	jmp	SSPnP_Success

SSPnP_Chk_Test_Succeeded:
	cmp	[cfFuncName], CONFIG_TEST_SUCCEEDED
	jne	SSPnP_Dont_Handle

	;> Prepare for a CONFIG_STOP or CONFIG_REMOVE call.

;;	jmp	SSPnP_Success

SSPnP_Success:
	.ERRNZ CR_SUCCESS
	xor	eax, eax
	jmp	SSPnP_Exit

SSPnP_Dont_Handle:
	mov	eax, CR_DEFAULT

SSPnP_Exit:
	pop	edi
	pop	esi
	pop	ebx
	LeaveProc
	Return

EndProc SS_PnP_Handler



;******************************************************************************
;
;   SS_Init_Adapter
;
;   DESCRIPTION:
;
;   ENTRY:
;	ESI -> Adapter reference data
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc SS_Init_Adapter

	pushad

;   The generic property sheet for PCMCIA devices allows the number of
;   sockets and the valid IRQ mask for the sockets to be defined.  The
;   following code reads these values from the registry if they exist.

	push	4
	mov	edx, esp		; EDX -> 4 (size of buffer)
	sub	esp, 4			; Scratch dword for registry stuff
	mov	ebp, esp		; EBP -> Buffer for read

	VxDcall _CONFIGMG_Read_Registry_Value, <[esi.AH_Devnode], 0, <OFFSET32 szOptions>, REG_BINARY, ebp, edx, CM_REGISTRY_SOFTWARE>

	pop	ebx			; Get flags in EBX
	pop	edx			; Trash size field

	cmp	eax, CR_SUCCESS 	; Q: Reg data returned?
	jne	SSIA_Find_Sockets	;    N: Do default stuff

	mov	[irq_msk], bx		; Set IRQ mask
	shr	ebx, 16 		; EBX = High word of flags
	jz	SSIA_Find_Sockets	; If 0 then automatic search
	cmp	ebx, MAX_SOCKETS	; Q: Is value reasonable?
	ja	SSIA_Find_Sockets	;    N: Ignore it
	mov	[esi.AH_NumSockets], bl ;    Y: Skip the search
	jmp	SSIA_No_More_Sockets	;	and init socket.

SSIA_Find_Sockets:
	xor	ebx, ebx
	mov	[esi.AH_NumSockets], bl

	;> Determine the number of sockets on the controller and
	;> set the [esi.AH_NumSockets].

SSIA_No_More_Sockets:

	;> Initialize the sockets on the controller

	popad
	ret

EndProc SS_Init_Adapter


;******************************************************************************
;
;   Socket_Services_Entry
;
;   DESCRIPTION:
;	This is the main service entry point for socket services.
;	This routine is called by the Card Services.
;
;   ENTRY:
;	AH  =  Socket service function number
;	ESI -> Adapter reference data
;	Other registers unique to service being called
;
;   EXIT:
;	Carry flag set if error, AH = Status
;	else carry flag clear,	 AH = 0
;
;   USES:
;	Can modify EAX, EBX, ECX, EDX, ESI, EDI, and flags as specified for
;	the given function.
;
;==============================================================================

BeginProc Socket_Services_Entry

	pushad
	mov	ebp, esp		; Set up param frame

	cmp	ah, SSFn_ACK_INTERRUPT
	jne	SSE_Chk_Busy

;   Special code for Acknowledge_Interrupt handling.  We allow this function
;   to be called even when the busy status is set.

	call	Acknowledge_Interrupt
	jmp	SSE_Success_Dont_Reset_Busy

SSE_Chk_Busy:
	bts	[SS_Flags], SSF_BUSY_BIT  ; Q: Have we been reentered?
	jc	SSErr_Reentered 	  ;    Y: May be an error!

;   Since GetAdapterCount is not supported in Microsoft's socket services,
;   we can eleminate all calls below 83h as invalid functions.

	sub	ah, BASE_TABLE_FUNCTION   ; Convert to 0 base
	jb	SSErr_Invalid_Function
	cmp	ah, FUNCTION_TABLE_ENTRIES
	ja	SSE_Test_Special_Case

	movzx	eax, ah
	mov	eax, Function_Table[eax*4]  ; EAX -> Function to call

;   Make sure the adapter reference data is valid by looking for a signature.
;   If it's OK then call the function pointed to by EAX.

SSE_Validate_Adapter_Data:
	cmp	[esi.AH_Signature], ADAPT_SIG  ; Q: Is this an OK signature?
	jne	SSErr_Bad_Adapter	       ;    N: Not valid

	call	eax			; Call the function
	jc	SS_Error_Exit		; Carry set if error

SSE_Success_Exit:
	ClrFlag [SS_Flags], SSF_BUSY

SSE_Success_Dont_Reset_Busy:
	popad
	.ERRNZ SUCCESS
	xor	ah, ah			; Set SUCCESS and CLC too!
	ret


;   The function number is > the dispatch table length, but that may simply
;   mean that this is one of the special-case functions.  We'll check for
;   them here before returning BAD_ADAPTER.

SSE_Test_Special_Case:
	cmp	ah, SSFn_GET_VENDOR_INFO-BASE_TABLE_FUNCTION
	jne	SSE_Test_Vend_Spec
	lea	eax, Get_Vendor_Info
	jmp	SSE_Validate_Adapter_Data

SSE_Test_Vend_Spec:
	cmp	ah, SSFn_VEND_SPECIFIC-BASE_TABLE_FUNCTION
	jne	SSErr_Invalid_Function
	lea	eax, Vendor_Specific
	jmp	SSE_Validate_Adapter_Data

;------------------------------------------------------------------------------
;
;   Error Handlers.  These entry points can be jumped to with any number of
;   arguments pushed on the stack, and EBP -> Socket Services Parameter frame.
;
;------------------------------------------------------------------------------

;   Socket services reentered.	Return Busy.

SSErr_Reentered:
	mov	ah, BUSY
	jmp	SS_Error_Exit_Still_Busy

;   Bad function number passed.

SSErr_Invalid_Function:
	mov	ah, BAD_FUNCTION
	jmp	SS_Error_Exit

;   Invalid adapter number

SSErr_Bad_Adapter:
	mov	ah, BAD_ADAPTER
	Assumes_Fall_Through SS_Error_Exit

;   All error handlers exit through here

SS_Error_Exit:
	and	[SS_Flags], NOT SSF_BUSY

SS_Error_Exit_Still_Busy:
IFDEF DEBUG
	mov	al, [ebp.Pushad_AH]
	Debug_Out "SAMPLESS:  Returning error #AH from function #AL"
ENDIF
	mov	[SSP_AH], ah
	mov	esp, ebp
	popad
	stc
	ret

EndProc Socket_Services_Entry


;******************************************************************************
;
;   Get_Socket_Services_Info
;
;   DESCRIPTION:
;	This function returns information about the compliance level of the
;	Socket Services interface supporting the adapter specified.
;
;   ENTRY:
;	EBP -> Socket Services Parameter frame
;	ESI -> Adapter reference data
;
;   EXIT:
;	Carry flag set if error, AH = Status
;	else carry flag clear,	 AH = 0
;	SSP_BX = BCD Version supported
;	SSP_CH = Number of adapters (always 1)
;	SSP_CL = First adapter (always 0)
;
;   USES:
;	Can use any standard reg and flags
;
;==============================================================================

BeginProc Get_Socket_Services_Info

	mov	[SSP_BX], VERSION_NUM_SS
	mov	[SSP_CX], 0100h
	xor	ah,ah
	ret

EndProc Get_Socket_Services_Info


;******************************************************************************
;
;   Inquire_Adapter
;
;   DESCRIPTION:
;	Returns capabilities information about the specified adapter.
;
;   ENTRY:
;	EBP -> Socket Services Parameter frame
;	ESI -> Adapter reference data
;	EDI -> Caller supplied buffer for adapter characteristics and
;	       power management tables.
;
;   EXIT:
;	Carry flag set if error, AH = Status
;	else carry flag clear,	 AH = 0
;	SSP_BH = Number of windows
;	SSP_BL = Number of sockets
;	SSP_CX = Number of EDCs (Always 0.  EDCs not supported).
;	Caller buffer filled in.
;
;   USES:
;	Can use any standard reg and flags
;
;==============================================================================

BeginProc Inquire_Adapter

	;> Fill in adapter information structure.
	;> Set [SSP_BL] to the number of sockets provided by the adapter.
	;> Set [SSP_BH] to the number of windows provided by the adapter.
	;> Set [SSP_CX] to the number of EDCs provided by the adapter.

	xor	ah,ah
	ret

EndProc Inquire_Adapter


;******************************************************************************
;
;   Get_Adapter
;
;   DESCRIPTION:
;	Returns the power state and interrupt routing for the specified
;	adapter.
;
;   ENTRY:
;	EBP -> Socket Services Parameter frame
;	ESI -> Adapter reference data
;
;   EXIT:
;	Carry flag set if error, AH = Status
;	else carry flag clear,	 AH = 0
;	SSP_DH = Adapter state
;	    Bit 0	Reduced power consumption
;		1	State information preserved
;		2..7	(reserved - set to 0)
;	SSP_DI = Status Change IRQ information
;	    Bit 0..4	IRQ line used
;		0-15 = IRQ0-15, 16 = NMI, 17 = I/O Chk, 18 = Bus Error
;	    Bit 5	reserved - set to 0
;	    Bit 6	Status change interrupt level (1 = active high)
;	    Bit 7	Status change interrupts enabled
;
;   USES:
;	Can use any standard reg and flags
;
;==============================================================================

BeginProc Get_Adapter

	;> Set [SSP_DH] to the adapter state.
	;> Set [SSP_DI] to the status change IRQ status.

	xor	ah,ah
	ret

EndProc Get_Adapter


;******************************************************************************
;
;   Set_Adapter
;
;   DESCRIPTION:
;	Set current configuration of specified adapter.
;
;   ENTRY:
;	EBP -> Socket Services Parameter frame
;	ESI -> Adapter reference data
;	DH  =  Adapter state
;	    Bit 0	Reduced power consumption
;		1	State information preserved
;		2..7	(reserved - set to 0)
;	DI  =  Status Change IRQ control
;	    Bit 0..4  IRQ line to use for Status Change interrupts
;		0-15 = IRQ0-15, 16 = NMI, 17 = I/O Chk, 18 = Bus Error
;	    Bit 5	reserved - set to 0
;	    Bit 6	Status change interrupt level (1 = active high)
;	    Bit 7	Enable Status Change interrupts
;
;   EXIT:
;	Carry flag set if error, AH = Status
;	else carry flag clear,	 AH = 0

;	Possible errors:  BAD_IRQ
;
;   USES:
;	Can use any standard reg and flags
;
;==============================================================================

BeginProc Set_Adapter

	;> Check for valid level.
	;> Check that IRQ number matches currently set IRQ.
	;> Enable or disable IRQ for sockets.
	;> Set the power managment state for the adapter.

	xor	ah,ah
	ret

EndProc Set_Adapter


;******************************************************************************
;
;   Inquire_Window
;
;   DESCRIPTION:
;	Return information for particular window and adapter.
;
;   ENTRY:
;	EBP -> Socket Services Parameter frame
;	ESI -> Adapter reference data
;	EDI -> Client's buffer for window characterstics table
;	BH  =	Window number (0..maxwindow)
;
;   EXIT:
;	Carry flag set if error, AH = Status
;	else carry flag clear,	 AH = 0
;	SSP_BL = Capabilities
;	    Bit 0	Common memory
;		1	Attribute memory
;		2	I/O space
;		3..6	Reserved - set to 0
;		7	WAIT supported
;	SSP_CX = Bit-map of assignable sockets
;	User buffer pointed to by EDI filled in

;	Possible errors:  BAD_WINDOW
;
;   USES:
;	Can use any standard reg and flags
;
;==============================================================================

BeginProc Inquire_Window

	;> Validate the window number for the adapter.
	;> Set [SSP_BL] to the window capability.
	;> Set [SSP_CX] to the bit mask of sockets.
	;> Fill in the window information structure.

	xor	ah,ah
	ret

EndProc Inquire_Window


;******************************************************************************
;
;   Get_Window
;
;   DESCRIPTION:
;	Get the specified window's configuration.
;
;   ENTRY:
;	EBP -> Socket Services Parameter frame
;	ESI -> Adapter reference data
;	BH  =  Window number
;
;   EXIT:
;	Carry flag set if error, AH = Status
;	else carry flag clear,	 AH = 0
;	SSP_BL = Socket number window is assigned to
;	SSP_CX = Window size (bytes if i/o, 4kb if memory)
;	    (0 => max size - i/o = 64kb, memory = 256mb)
;	SSP_DH = Window attributes
;	    Bit 0	Memory .vs. i/o (1 == i/o)
;		1	Enabled/disabled
;		2	Data path width (1 == 16 bits)
;		3	Paged (1 == yes) (memory only)
;		3	EISA-mapped (1 == true) (I/O only)
;		4	Non-specific access slot enable
;			(1 == true) (EISA-mapped i/o only)
;		5..7	(Reserved - set to 0)
;	SSP_DL = Access speed
;	    Bits 0..2  Device speed code, if mantissa is 0
;		 0..2  Speed exponent, if mantissa is not 0
;		 3..6  Speed mantissa
;		 7     (Reserved - set to 0)
;	SSP_DI = Window base address (bytes if i/o, 4kb if memory)
;
;	Possible errors:  BAD_WINDOW
;
;   USES:
;	Can use any standard reg and flags
;
;==============================================================================

BeginProc Get_Window

	;> Validate the window number for the adapter.
	;> Set [SSP_BL] to the socket number window is assigned to.
	;> Set [SSP_CX] to the window size.
	;> Set [SSP_DH] to the window state.
	;> Set [SSP_DL] to the window speed.
	;> Set [SSP_DI] to the window base address.

	xor	ah,ah
	ret

EndProc Get_Window


;******************************************************************************
;
;   Set_Window
;
;   DESCRIPTION:
;	Set specified window's configuration
;
;   ENTRY:
;	EBP -> Socket Services Parameter frame
;	ESI -> Adapter reference data
;	BH  =  Window number
;	BL  =  Socket number
;	CX  =  Window size (bytes if i/o, 4kb if memory)
;	    (0 => max size - i/o = 64kb, memory = 256mb)
;	DH  =  Window attributes
;	    Bit 0	memory .vs. i/o (1 == i/o)
;		1	enabled/disabled
;		2	data path width (1 == 16 bits)
;		3	paged (1 == yes) (memory only)
;		3	EISA-mapped (1 == true) (I/O only)
;		4	non-specific access slot enable
;			(1 == true) (EISA-mapped i/o only)
;		5..7	(reserved - set to 0)
;	DL  =  Access speed
;	    Bits 0..2	Device speed code, if mantissa is 0
;		 0..2	Speed exponent, if mantissa is not 0
;		 3..6	Speed mantissa
;		 7	(reserved - set to 0)
;	DI  =  Window base address (bytes if i/o, 4kb if memory)
;
;   EXIT:
;	Carry flag set if error, AH = Status
;	else carry flag clear,	 AH = 0

;	Possible errors:  BAD_ATTRIBUTE, BAD_BASE, BAD_SIZE, BAD_SOCKET
;			  BAD_SPEED, BAD_TYPE, BAD_WINDOW
;
;   USES:
;	Can use any standard reg and flags
;
;==============================================================================

BeginProc Set_Window

	;> Validate the parameters.
	;> Set the window based on the parameters.

	xor	ah,ah
	ret

EndProc Set_Window


;******************************************************************************
;
;   Get_Page
;
;   DESCRIPTION:
;	Get current configuration for page/window/adapter.
;
;   ENTRY:
;	EBP -> Socket Services Parameter frame
;	ESI -> Adapter reference data
;	BH  =  Window number
;	BL  =  Page number
;
;   EXIT:
;	Carry flag set if error, AH = Status
;	else carry flag clear,	 AH = 0
;	SSP_DL = Page state
;	    Bit 0	Common/attribute memory (1 = attribute)
;	    Bit 1	Disabled/enabled (1 = enabled)
;	    Bit 2	Write protected (1 = protected)
;		3..7	(Reserved - set to 0)
;	SSP_DI = Memory card offset (in 4Kb increments)

;	Possible errors:  BAD_PAGE, BAD_WINDOW
;
;   USES:
;	Can use any standard reg or flags
;
;==============================================================================

BeginProc Get_Page

	;> Validate the window and page numbers.
	;> Set [SSP_DL] to the page state.
	;> Set [SSP_DI] to the memory card offset.

	xor	ah,ah
	ret

EndProc Get_Page


;******************************************************************************
;
;   Set_Page
;
;   DESCRIPTION:
;	Set the base for the specified page/window/adapter.
;
;   ENTRY:
;	EBP -> Socket Services Parameter frame
;	ESI -> Adapter reference data
;	BH  =  Window number
;	BL  =  Page number
;	DL  =  Page control
;	    Bit 0	Common/attribute memory (1 = attribute)
;	    Bit 1	Disabled/enabled (1 = enabled)
;	    Bit 2	Write protected (1 = protected)
;		3..7	(Reserved - set to 0)
;	DI  =  Memory card offset (4kb)
;
;   EXIT:
;	Carry flag set if error, AH = Status
;	else carry flag clear,	 AH = 0

;	Possible errors:  BAD_ATTRIBUTE, BAD_OFFSET, BAD_PAGE, BAD_WINDOW
;
;   USES:
;	Can use any standard reg or flags
;
;==============================================================================

BeginProc Set_Page

	;> Validate the parameters.
	;> Set the page based on the parameters.

	xor	ah,ah
	ret

EndProc Set_Page


;******************************************************************************
;
;   Inquire_Socket
;
;   DESCRIPTION:
;	Return information for a particular socket/adapter.
;
;   ENTRY:
;	EBP -> Socket Services Parameter frame
;	ESI -> Adapter reference data
;	BL  =  Socket number
;	EDI -> Caller supplied buffer for characteristics table
;
;   EXIT:
;	Carry flag set if error, AH = Status
;	else carry flag clear,	 AH = 0
;	SSP_BH = Status change interrupt capabilities (SCIntCaps)
;	    Bit 0	Write Protect Change
;		1	Card Lock Change
;		2	Ejection Request
;		3	Insertion Request
;		4	Battery Dead Change
;		5	Battery Warning Change
;		6	Ready Change
;		7	Card Detect Change
;	SSP_DH =  Socket capabilities (SCRptCaps)
;	    Bit 0	Card change
;		1	Card lock
;		2	Insert card (motor control)
;		3	Eject card (motor control)
;		4..7	(Reserved - set to 0)
;	SSP_DL =  Hardware indicators (CtlIndCaps)
;	    Bit 0	Busy status
;		1	Write protect status
;		2	Battery status
;		3	Card lock status
;		4	XIP status
;		5..7	(Reserved - set to 0)
;	Clients buffer filled with socket characteristics table
;	    Offset Size Contents
;	    0	    W	    supported card types
;		Bit 0	    memory card
;		    1	    i/o card
;		    2..15 (reserved - set to 0)
;	    2	    W	    steerable IRQ levels - bit 0 = IRQ0,
;		bit 1 = IRQ1, ... , bit 15 = IRQ15
;	    4	    W	    additional steerable IRQ levels
;		bit 0 = NMI, bit 1 = I/O, bit 2 =
;		    bus error, bit 3 = vendor-unique
;
;	Possible errors:  BAD_SOCKET
;
;   USES:
;	Can use any standard reg or flags
;
;==============================================================================

BeginProc Inquire_Socket

	;> Validate the socket number.
	;> Set [SSP_BH] to the status change interrupt capability.
	;> Set [SSP_DH] to the socket event capability.
	;> Set [SSP_DL] to the socket control or indicator capability.
	;> Fill in the socket information structure.

	xor	ah,ah
	ret

EndProc Inquire_Socket


;******************************************************************************
;
;   Get_Socket
;
;   DESCRIPTION:
;	Get Current Socket Configuration.
;
;   ENTRY:
;	EBP -> Socket Services Parameter frame
;	ESI -> Adapter reference data
;	BL  =  Socket number
;
;   EXIT:
;	Carry flag set if error, AH = Status
;	else carry flag clear,	 AH = 0
;	SSP_BH = Status change interrupt enable mask
;	    Bit 0	Write protect change
;		1	Card lock change
;		2	Ejection request
;		3	Insertion request
;		4	Battery-dead change
;		5	Battery-warning change
;		6	Ready/busy change
;		7	Card-detect change
;	SSP_CH = Vcc level (lower nybble) (index into PwrMgmt Tbl)
;	SSP_CL = Vpp1/Vpp2 level (upper/lower nybble) (ditto)
;	SSP_DH = Socket state
;	    Bit 0 Write protect change
;		1	card lock change
;		2	card ejection request pending
;		3	card insertion request pending
;		4	Battery Dead Change
;		5	Battery Warning Change
;		6	Ready Change
;		7	Card Detect Change
;	SSP_DL = Control and Indicator State
;	    Bit 0	Write Protect State (indicator)
;		1	Card Lock State (indicator)
;		2	Motorized Card Ejection (control)
;		3	Motorized Card Insertion (control)
;		4	Card Lock (control)
;		5	Battery State (indicator)
;		6	Busy State (indicator)
;		7	XIP State (indicator)
;	SSP_DI = IRQ steering level and interface type
;	    if Bit 9 == 1,
;		Bit 0..4    IRQ level (0..15 = IRQ 0..15, 16 = NMI,
;			    17 = I/O Chk, 18 = BusErr
;	    else
;		Bit 0..4    reserved, set to 0
;		    5	    reserved, set to 0
;		    6	    IREQ inverter enabled
;		    7	    IREQ steering enabled
;		    8	    Memory-only interface
;		    9	    Memory and I/O interface
;		    10..15  reserved, set to 0
;
;	Possible errors:  BAD_SOCKET
;
;   USES:
;	Can use any standard reg or flags
;
;==============================================================================

BeginProc Get_Socket

	;> Validate the socket number.
	;> Set [SSP_BH] to the status change interrupt event mask.
	;> Set [SSP_CH] to the socket Vcc level.
	;> Set [SSP_CL] to the socket Vpp1 and Vpp2 level.
	;> Set [SSP_DH] to the socket state.
	;> Set [SSP_DL] to the socket control or indicator state.
	;> Set [SSP_DI] to the IRQ steering level and interface type.

	xor	ah,ah
	ret

EndProc Get_Socket


;******************************************************************************
;
;   Set_Socket
;
;   DESCRIPTION:
;	Set Socket Configuration.
;
;   ENTRY:
;	EBP -> Socket Services Parameter frame
;	ESI -> Adapter reference data
;	BL  =  Socket number
;	BH  =  Status change interrupt mask
;	    Bit 0	Write protect change
;		1	Card Lock change
;		2	Ejection request
;		3	Insertion request
;		4	Battery-dead change
;		5	Battery-warning change
;		6	Ready/busy change
;		7	Card-detect change
;	CH  =  Vcc level (lower nybble) (index into PwrMgmt Tbl)
;	CL  =  Vpp1/Vpp2 level (upper/lower nybble) (ditto)
;	DH  =  Socket state control (1 = reset state)
;	    Bit 0	Write Protect Change
;		1	Card Lock Change
;		2	Ejection Request
;		3	Insertion Request
;		4	Battery Dead Change
;		5	Battery Warning Change
;		6	Ready Change
;		7	Card Detect Change
;	DL  =  Controls and Indicators
;	    Bit 0	Write Protect state (I)
;		1	Card Lock state (I)
;		2	Motorized Card Ejection (C)
;		3	Motorized Card Insertion (C)
;		4	Card Lock (C)
;		5	Battery State (I)
;		6	Busy State (I)
;		7	XIP state (I)
;	DI  =  IRQ steering and interface type
;	    Bit 0..4	IRQ level (0..15 = IRQ 0..15,
;			16 = NMI, 17 = I/O Chk, 18 = BusErr)
;		5	reserved - must be 0
;		6	Enable IREQ Inverter
;		7	Enable IREQ Steering
;		8	Memory-only interface
;		9	I/O and Memory interface
;		10..15	reserved - must be 0
;
;   EXIT:
;	Carry flag set if error, AH = Status
;	else carry flag clear,	 AH = 0

;	Possible errors:  BAD_IRQ, BAD_SOCKET, BAD_TYPE, BAD_VCC, BAD_VPP
;
;   USES:
;	Can use any standard reg or flags
;
;==============================================================================

BeginProc Set_Socket

	;> Validate the parameters.
	;> Set the socket based on the parameters.

	xor	ah,ah
	ret

EndProc Set_Socket


;******************************************************************************
;
;   Get_Status
;
;   DESCRIPTION:
;	Return Status Of Card and Socket
;
;   ENTRY:
;	EBP -> Socket Services Parameter frame
;	ESI -> Adapter reference data
;	BL  =  Socket number
;
;   EXIT:
;	Carry flag set if error, AH = Status
;	else carry flag clear,	 AH = 0
;	SSP_BH	=  Current Card State
;	    Bit 0	Write Protected
;		1	Card Locked
;		2	Ejection Request
;		3	Insertion Request
;		4	Battery Dead
;		5	Battery Low
;		6	Card Ready
;		7	Card Detected
;	SSP_DH	=  Socket state (saved)
;	    Bit 0	Write protect change
;		1	Card lock change
;		2	Card ejection request pending
;		3	Card insertion request pending
;		4	Battery Dead Change
;		5	Battery Warning Change
;		6	Ready Change
;		7	Card Detect Change
;	SSP_DL	=  Control and Indicator State
;	    Bit 0	Write Protect State (indicator)
;		1	Card Lock State (indicator)
;		2	Motorized Card Ejection (control)
;		3	Motorized Card Insertion (control)
;		4	Card Lock (control)
;		5	Battery State (indicator)
;		6	Busy State (indicator)
;		7	XIP State (indicator)
;	SSP_DI	=  IRQ steering level and interface type
;	    if Bit 9 == 1,
;		Bit 0..4    IRQ level (0..15 = IRQ 0..15, 16 = NMI,
;			    17 = I/O Chk, 18 = BusErr
;	    else
;		Bit 0..4    reserved, set to 0
;		    5	    reserved, set to 0
;		    6	    IREQ inverter enabled
;		    7	    IREQ steering enabled
;		    8	    Memory-only interface
;		    9	    Memory and I/O interface
;		    10..15  reserved, set to 0
;
;	Possible errors:  BAD_SOCKET
;
;   USES:
;	Can use any standard reg or flags
;
;==============================================================================

BeginProc Get_Status

	;> Validate the socket number.
	;> Set [SSP_BH] to the status change interrupt event mask.
	;> Set [SSP_DH] to the socket state.
	;> Set [SSP_DL] to the socket control or indicator state.
	;> Set [SSP_DI] to the IRQ steering level and interface type.

	xor	ah,ah
	ret

EndProc Get_Status


;******************************************************************************
;
;   Reset_Socket
;
;   DESCRIPTION:
;	Reset card and socket
;
;   ENTRY:
;	EBP -> Socket Services Parameter frame
;	ESI -> Adapter reference data
;	BL  =  Socket number
;
;   EXIT:
;	Carry flag set if error, AH = Status
;	else carry flag clear,	 AH = 0

;	Possible errors:  BAD_SOCKET, NO_CARD
;
;   USES:
;	Can use any standard reg and flags
;
;==============================================================================

BeginProc Reset_Socket

	;> Validate the socket number.
	;> Disable all windows for the socket.
	;> Disable IREQ routing.
	;> Set all power levels to 5V.
	;> Reset any card in the socket.

	xor	ah,ah
	ret

EndProc Reset_Socket


;******************************************************************************
;
;   Get_Vendor_Info
;
;   DESCRIPTION:
;	Get information about the Socket Services vendor
;
;   ENTRY:
;	EBP -> Socket Services Parameter frame
;	ESI -> Adapter reference data
;	BL  =  Function code
;		0 = Get vendor release number and string
;	EDI -> Client's buffer for ASCIIZ string describing implementor
;
;   EXIT:
;	Carry flag set if error, AH = Status
;	else carry flag clear,	 AH = 0
;	SSP_DX = Vendor's release number in BCD
;	Client buffer contains implementor descrition string

;	Possible errors:  BAD_FUNCTION
;
;   USES:
;	Can use any standard reg and flags
;
;==============================================================================

BeginProc Get_Vendor_Info

	;> Validate the function code (must be 0 for now).
	;> Set [SSP_DX] to the vendor release number.
	;> Fill in the vendor information structure.

	xor	ah,ah
	ret

EndProc Get_Vendor_Info


;******************************************************************************
;
;   Acknowledge_Interrupt
;
;   DESCRIPTION:
;	Acknowledge a status change interrupt
;
;   ENTRY:
;	EBP -> Socket Services Parameter frame
;	ESI -> Adapter reference data
;
;   EXIT:
;	Carry flag set if error, AH = Status
;	else carry flag clear,	 AH = 0
;	SSP_CX = Bit vector of sockets reporting status change
;		(bit 0 = socket 0, bit 1 = socket 1, etc.)
;
;   USES:
;	Can use any standard reg and flags
;
;==============================================================================

BeginProc Acknowledge_Interrupt

	;> Set [SSP_CX] to the mask of sockets reporting a status change.
	;> Acknowledge the adapter interrupt at the hardware.

	xor	ah,ah
	ret

EndProc Acknowledge_Interrupt


VxD_CODE_ENDS

	END
