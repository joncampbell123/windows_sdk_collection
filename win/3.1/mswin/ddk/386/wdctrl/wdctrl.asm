PAGE 58,132
;******************************************************************************
TITLE WDCTRL.ASM -- Block Device VxD for Western Digital Controlers
;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1990
;
;   Title:	WDCTRL.ASM -- Block Device VxD for Western Digital Controlers
;
;   Version:	3.10
;
;   Date:	29-Aug-1990
;
;   Author:	RAL
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;   29-Aug-1990 RAL Original -- Taken from FastDisk.386
;   04-Dec-1990 RAL Added scatter/gather, bug fixes
;   12-Sep-1991 RAL Read alternate status port since it does not clear int
;		    request + add IO_Delay everywhere -- Old controllers need
;		    these delays!  Don't remove them!
;   11-Oct-1991 RAL Undo the alternate status port stuff!
;   07-Nov-1991 RAL Retry on failed operations
;
;==============================================================================

	.386p

;******************************************************************************
;			      I N C L U D E S
;******************************************************************************

	.XLIST
	INCLUDE VMM.Inc
	INCLUDE Debug.Inc
	INCLUDE OptTest.Inc
	INCLUDE BlockDev.Inc
	INCLUDE Int13.Inc
	INCLUDE VPICD.Inc
	INCLUDE WDLocal.Inc
	INCLUDE Shell.Inc
	.LIST

;******************************************************************************
;	       V I R T U A L   D E V I C E   D E C L A R A T I O N
;******************************************************************************

Declare_Virtual_Device WDCTRL, WD_Major_Ver, WD_Minor_Ver, WDCtrl_Control, Undefined_Device_ID, Undef_Touch_Mem_Init_Order



VxD_IDATA_SEG

EXTRN WD_Init_Error_String:BYTE
EXTRN WD_Time_Out_Ini_String:BYTE

VxD_IDATA_ENDS

;******************************************************************************
;			    L O C K E D   D A T A
;******************************************************************************

VxD_DATA_SEG

;
;   Error messages
;
EXTRN WD_Time_Out_Error_Title:BYTE
EXTRN WD_Time_Out_Error_Message:BYTE
EXTRN WD_IO_Access_Error:BYTE

PUBLIC	WD_Drive_80h_BDD
PUBLIC	WD_Drive_81h_BDD
PUBLIC	WDCtrl_Name_0
PUBLIC	WDCtrl_Name_1
PUBLIC	WD_Current_Command
PUBLIC	WD_Current_BDD
PUBLIC	WD_Pending_Command
PUBLIC	WD_Pending_BDD
PUBLIC	WD_Cur_Xfer_Ptr
PUBLIC	WD_Remaining_Sec_Count
PUBLIC	WD_Cur_Region_Count
PUBLIC	WD_Cur_Region_Ptr
PUBLIC	WD_Next_Start_Sector
PUBLIC	WD_Rem_Sec_This_Xfer
PUBLIC	WD_Accum_Status
PUBLIC	WD_Num_Retries
PUBLIC	WD_Last_Cmd_Time
PUBLIC	WD_Max_Cmd_Time
PUBLIC	WD_IO_Warn_Msg_Disp

;
;   Device desctiptors for two drives
;
WD_Drive_80h_BDD	WDCtrl_Private_BDD<>
WD_Drive_81h_BDD	WDCtrl_Private_BDD<>

;
;   Names for drive 0 and 1
;
WDCtrl_Name_0		db	"WDCTRL0", 0
WDCtrl_Name_1		db	"WDCTRL1", 0

;
;   Variables for maintaining state of commands in progress
;
WD_Current_Command	dd	0
WD_Current_BDD		dd	0
WD_Pending_Command	dd	0
WD_Pending_BDD		dd	0
WD_Last_Cmd_Time	dd	0
WD_Max_Cmd_Time 	dd	20000

WD_Cur_Xfer_Ptr 	dd	?
WD_Remaining_Sec_Count	dd	?
WD_Cur_Region_Count	dd	?
WD_Cur_Region_Ptr	dd	?
WD_Next_Start_Sector	dd	?
WD_Rem_Sec_This_Xfer	db	?
WD_Accum_Status 	db	0
WD_Num_Retries		db	0
WD_IO_Warn_Msg_Disp	db	FALSE

IFDEF DEBUG
PUBLIC WD_Total_Retries
PUBLIC WD_Total_Failed_Reads
PUBLIC WD_Total_Failed_Writes

WD_Total_Retries	dd	0
WD_Total_Failed_Reads	dd	0
WD_Total_Failed_Writes	dd	0
ENDIF

VxD_DATA_ENDS



;******************************************************************************
;	       D E V I C E   C O N T R O L   P R O C E D U R E
;******************************************************************************

VxD_CODE_SEG

;******************************************************************************
;
;   WDCtrl_Control
;
;   DESCRIPTION:
;	Device control procedure for the WDCTRL Virtual Device.
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

BeginProc WDCtrl_Control

	Control_Dispatch Sys_Critical_Init, WDCtrl_Sys_Crit_Init

IFDEF DEBUG
	EXTRN WDCtrl_Debug_Query:NEAR
	Control_Dispatch Debug_Query, WDCtrl_Debug_Query
ENDIF

	clc
	ret


EndProc WDCtrl_Control

VxD_CODE_ENDS


;******************************************************************************
;		    I N I T I A L I Z A T I O N   C O D E
;******************************************************************************

VxD_ICODE_SEG

;******************************************************************************
;
;   WDCtrl_Sys_Crit_Init
;
;   DESCRIPTION:
;	This control handler will initialize a Block Device Desctiptor for
;	one or two drives on a standard AT type controller.
;
;   ENTRY:
;	EBX = System VM handle (not used)
;	EDX = Reference data from real mode init portion
;
;   EXIT:
;	If successful then
;	    Carry flag is clear
;	else
;	    Carry flag is set to indicate an error -- Device not initialized
;	    Note that currently this device will generate a fatal error
;	    if it can not initialize.
;
;   USES:
;	EAX, EBX, ECX, EDX, ESI, EDI, Flags
;
;==============================================================================

BeginProc WDCtrl_Sys_Crit_Init

	TestReg edx, RF_Drive_80h_Ours		; Q: First Int 13h drive ours?
	jz	SHORT WD_SCI_Try_Drive_81h	;    N: Check out 2nd drive

	push	edx				; Save reference data
	mov	eax, DWORD PTR ds:[41h*4]	; EAX -> Seg:Offset of params
	mov	edi, OFFSET32 WD_Drive_80h_BDD	; EDI -> BDD
	mov	cl, 80h 			; Drive 0 for this controller
	mov	esi, OFFSET32 WDCtrl_Name_0	; Name for drive
	and	edx, RF_USE_ALT_STAT_80 	; EDX = 0 if use normal status
	call	WDCtrl_Init_BDD 		; Initialize the drive
	pop	edx				; Restore refernce data
	jc	SHORT WD_SCI_Error

WD_SCI_Try_Drive_81h:
	TestReg edx, RF_Drive_81h_Ours		; Q: Second Int 13h drive ours?
	jz	SHORT WD_SCI_Trap_IO_Ports	;    N: Done with this part

	mov	eax, DWORD PTR ds:[46h*4]	; EAX -> Seg:Offset of params
	mov	edi, OFFSET32 WD_Drive_81h_BDD	; EDI -> BDD
	mov	cl, 81h 			; Drive 1 for this controller
	mov	esi, OFFSET32 WDCtrl_Name_1	; Name for drive
	and	edx, RF_USE_ALT_STAT_81 	; EDX = 0 if use normal status
	call	WDCtrl_Init_BDD 		; Initialize the drive
	jc	SHORT WD_SCI_Error

;------------------------------------------------------------------------------
;
;   Trap all hard disk I/O ports to make it appear that there is no western
;   digital controller on this machine.
;
;------------------------------------------------------------------------------

WD_SCI_Trap_IO_Ports:
	mov	ecx, 8
	mov	esi, OFFSET32 WDCtrl_IO_Port_Trap
	mov	edx, WDIO_Def_Base_Port
WD_SCI_Trap_Loop1:
	VMMcall Install_IO_Handler
	jc	SHORT WD_SCI_Error
	inc	edx
	loopd	WD_SCI_Trap_Loop1

	mov	edx, WDIO_Def_Base_Port+WDIO_Alt_Stat_Off
	VMMcall Install_IO_Handler
	jc	SHORT WD_SCI_Error

;
;   NOTE:  We do NOT want to trap I/O to the WDIO_Drive_Address port
;	   (port 3F7h) since it is used primairly by the floppy disk and
;	   since no data can be written to it that would alter the
;	   state of the fixed disk controller
;

;
;   Read System.Ini to get a user defined time-out value for WDCTRL commands.
;
	mov	eax, [WD_Max_Cmd_Time]
	xor	esi, esi
	mov	edi, OFFSET32 WD_Time_Out_Ini_String
	VMMcall Get_Profile_Decimal_Int
	mov	[WD_Max_Cmd_Time], eax

;
;   Schedule a preiodic time-out so we can put up a horrible, ugly message
;   if a command times out.
;
	test	eax, eax
	jz	SHORT WD_SCI_Success
	mov	esi, OFFSET32 WDCtrl_Periodic_Time_Out
	VMMcall Set_Global_Time_Out

;
;   Success!  We're installed.
;
WD_SCI_Success:
	clc
	ret


;
;   Error during initialization.
;
WD_SCI_Error:
	Debug_Out "ERROR DURING WDCTRL SYS CRITICAL INIT!"
	Fatal_Error <OFFSET32 WD_Init_Error_String>

EndProc WDCtrl_Sys_Crit_Init


;******************************************************************************
;
;   WDCtrl_Init_BDD
;
;   DESCRIPTION:
;
;   ENTRY:
;	EAX = Seg:Offset of Int 13h BIOS parameter table for this drive
;	EDX = 0 if normal status register to be used
;	   != 0 if alternate status register to be used
;	EDI -> Block Device Descriptor
;	CL = 80h or 81h to indicate drive 80 or drive 81
;
;   EXIT:
;	If carry flag set then BlockDev did not install device
;
;   USES:
;	EAX, EBX, ECX, EDX, ESI, EDI, Flags
;
;==============================================================================

BeginProc WDCtrl_Init_BDD

;------------------------------------------------------------------------------
;
;   Fill in the easy parts first
;
;------------------------------------------------------------------------------

	mov	[edi.BDP_Base_IO_Port], WDIO_Def_Base_Port
	mov	[edi.BDP_Status_Port], WDIO_Def_Base_Port + WDIO_Status_Off
	test	edx, edx
        jz      SHORT WD_IBDD_No_Alt_Status
	mov	[edi.BDP_Status_Port], WDIO_Def_Base_Port + WDIO_Alt_Stat_Off
WD_IBDD_No_Alt_Status:

	mov	[edi.BDD_BD_Major_Ver], BD_Major_Version
	mov	[edi.BDD_BD_Minor_Ver], BD_Minor_Version
	mov	[edi.BDD_Flags], BDF_Int13_Drive OR BDF_Writeable OR BDF_Serial_Cmd
	mov	[edi.BDD_Name_Ptr], esi
	mov	[edi.BDD_Sector_Size], 512
	mov	[edi.BDD_Int_13h_Number], cl
	mov	[edi.BDD_Device_Type], BDT_Fixed_Disk
	mov	[edi.BDD_Sync_Cmd_Proc], OFFSET32 WDCtrl_Sync_Command
	mov	[edi.BDD_Command_Proc], OFFSET32 WDCtrl_Command
	mov	[edi.BDD_Hw_Int_Proc], OFFSET32 WDCtrl_Hw_Int


;------------------------------------------------------------------------------
;
;   Now suck information out of the Fixed Disk Parameter structure in the
;   BIOS and fill in the rest.
;
;------------------------------------------------------------------------------

	movzx	ecx, ax
	shr	eax, 16
	shl	eax, 4
	add	eax, ecx			; EAX -> Fixed disk parameters

	movzx	ecx, [eax.FDPT_Max_Cyl]
	mov	[edi.BDD_Num_Cylinders], ecx
	movzx	ecx, [eax.FDPT_Max_Heads]
	mov	[edi.BDD_Num_Heads], ecx
	movzx	ecx, [eax.FDPT_Sec_Per_Track]
	mov	[edi.BDD_Num_Sec_Per_Track], ecx
	mov	cx, [eax.FDPT_Write_Precom_Cyl]
	cmp	cx, -1
	jne	SHORT WD_IBDD_Set_Precom
	xor	cx, cx
WD_IBDD_Set_Precom:
	shr	cx, 2
	mov	[edi.BDP_Write_Precom], cl
	mov	cl, [eax.FDPT_Drive_Control]
	mov	[edi.BDP_Drive_Control], cl

	mov	eax, [edi.BDD_Num_Cylinders]
	imul	eax, [edi.BDD_Num_Heads]
	imul	eax, [edi.BDD_Num_Sec_Per_Track]
	dec	eax

	mov	DWORD PTR [edi.BDD_Max_Sector], eax
	mov	DWORD PTR [edi.BDD_Max_Sector][4], 0

	VxDjmp	BlockDev_Register_Device

EndProc WDCtrl_Init_BDD


VxD_ICODE_ENDS


VxD_CODE_SEG

;******************************************************************************
;
;   WDCtrl_Sync_Command
;
;   DESCRIPTION:
;	This procedure is called for synchronous commands.  It currently
;	only supports the get physical drive characteristics call and get
;	version.
;
;   ENTRY:
;	AX = Command (equates begin with BD_SC_xxx)
;	Other registers defined by specific API call
;
;   EXIT:
;	If function is successful:
;	Carry flag is clear
;	Registers modified as specified by API call
;
;   USES:
;	Flags, any register defined as modified by API
;
;==============================================================================

BeginProc WDCtrl_Sync_Command

	.ERRNZ BD_SC_Get_Version

	or	ax, ax
	jnz	SHORT WD_SC_Return_Error
;
;   Get version call
;
	mov	ax, (WD_Major_Ver*100h)+WD_Minor_Ver
	clc
	ret

;
;   Command not supported by this device
;
WD_SC_Return_Error:
	mov	ax, BD_SC_Err_Invalid_Cmd
	stc
	ret

EndProc WDCtrl_Sync_Command


;******************************************************************************
;
;   WDCtrl_Command
;
;   DESCRIPTION:
;	This is the entry point called by BlockDev when an async command is
;	sent to one of the drives.  Since this VxD supports only serialized
;	commands, it will only have to deal with one command per drive
;	at a time.  BlockDev will be responsible for queueing commands.
;	This also means that BlockDev will handle trivial error cases
;	such as sector out of range errors, so this procedure will not
;	need to check for that error.
;
;   IMPORTANT NOTE:
;	This procedure is also called wby WDCTRL_HW_INT to process queued
;	commands and to retry commands that fail.  In the case of a command
;	retry, the WC_Current_Command pointer will be set to 0 and the
;	command will be re-submitted.  Note that the HW_Int procedure is
;	responsible for incrementing the WD_Num_Retries count before calling
;	this procedure.  This procedure will never set or modify the retry
;	count.
;
;   ENTRY:
;	EDI -> Block Device Descriptor
;	ESI -> Command block
;
;   EXIT:
;	None
;
;   USES:
;	Flags
;
;==============================================================================

BeginProc WDCtrl_Command

	Assert_Ints_Disabled

	pushad

	cmp	[esi.BD_CB_Command], BDC_Write
	ja	WD_C_Not_Supported

	cmp	[WD_Current_Command], 0 	; Q: Processing a command?
	jne	SHORT WD_C_Queue_Command	;    Y: Queue this one

;------------------------------------------------------------------------------
;
;   We aren't currently processing a command, so we'll start on this one.
;
;------------------------------------------------------------------------------

WD_C_Make_Cur_Cmd:
	mov	[WD_Current_Command], esi	; Remember the command
	mov	[WD_Current_BDD], edi		; and which BDD it was for

	mov	[WD_Accum_Status], 0		; Reset accumulated status flags

	mov	eax,DWORD PTR [esi.BD_CB_Sector]; Suck out the starting sector
	mov	[WD_Next_Start_Sector], eax	; need this to split up xfers
	mov	eax, [esi.BD_CB_Buffer_Ptr]	; Same thing with the buffer

	TestMem [esi.BD_CB_Flags], BDCF_Scatter_Gather
	jz	SHORT WC_C_Not_Scatter_Gather
	mov	edx, DWORD PTR [eax]
	mov	[WD_Cur_Region_Count], edx
	mov	ecx, DWORD PTR [eax+4]
	mov	[WD_Cur_Xfer_Ptr], ecx
	mov	[WD_Cur_Region_Ptr], eax
WD_C_Sum_Xfer_Size:
	add	eax, 8
	mov	ecx, DWORD PTR [eax]
	jecxz	WC_C_Have_Xfer_Size
	add	edx, ecx
	jmp	WD_C_Sum_Xfer_Size

WC_C_Not_Scatter_Gather:
	mov	[WD_Cur_Xfer_Ptr], eax		; pointer
	mov	[WD_Cur_Region_Count], 0	; Set to 4Gig remaining
	mov	edx, [esi.BD_CB_Count]		; And the sector count too

WC_C_Have_Xfer_Size:
	mov	[WD_Remaining_Sec_Count], edx

	call	WDCtrl_Send_Cmd_To_Ctrl 	; Send it to the controller now

WD_C_Exit:
	sti
	popad
	ret


;------------------------------------------------------------------------------
;
;   Since we're handling a max of two drives, and each one can only get one
;   command at a time, we can just stick this command in a static variable
;   until the current command completes.
;
;------------------------------------------------------------------------------


WD_C_Queue_Command:
	cmp	[WD_Current_Command], -1
	je	SHORT WD_C_Call_At_Cmd_Cplt

	mov	[WD_Pending_Command], esi
	mov	[WD_Pending_BDD], edi
	jmp	WD_C_Exit

;------------------------------------------------------------------------------
;
;   There is a pending command already.  We are being called during the
;   completion of another command.  If the pending command is high priority
;   or if the new command is low priority, do the pending command first.
;   Otherwise leave the pending command alone and use the new command.
;
;------------------------------------------------------------------------------

WD_C_Call_At_Cmd_Cplt:
	TestMem [esi.BD_CB_Flags], BDCF_High_Priority
	jz	SHORT WD_C_Use_Pending
	mov	eax, [WD_Pending_Command]
	TestMem [eax.BD_CB_Flags], BDCF_High_Priority
	jz	WD_C_Make_Cur_Cmd

WD_C_Use_Pending:
	xchg	esi, [WD_Pending_Command]
	xchg	edi, [WD_Pending_BDD]
	jmp	WD_C_Make_Cur_Cmd

;------------------------------------------------------------------------------
;
;   This command is not supported by WDCtrl.
;
;------------------------------------------------------------------------------

WD_C_Not_Supported:
	mov	[esi.BD_CB_Cmd_Status], BDS_Invalid_Command
	VxDcall BlockDev_Command_Complete
	jmp	WD_C_Exit

EndProc WDCtrl_Command


;******************************************************************************
;
;   WDCtrl_Send_Cmd_To_Ctrl
;
;   DESCRIPTION:
;	This procedure will program the controller to read/write a block
;	of sectors up to the maximum sector transfer size.  It may be called
;	repeatedly for a single command to transfer sector counts that
;	are larger than the maximum transfer size allowed.
;
;	This code assumes that the caller has previously initialized:
;
;	    WD_Next_Start_Sector
;	    WD_Cur_Xfer_Ptr
;	    WD_Remaining_Sec_Count
;
;	These variables are used to maintain information about the current
;	state of a disk transfer.  They are updated at interrupt time and
;	by this procedure.
;
;	This procedure will be called initially by WDCtrl_Command which is
;	responsible for initializing the variables listed above as well as
;	setting the current command and current BDD variables.	This procedure
;	then will program the controller to read/write the largest allowable
;	number of sectors.  Once that transfer is complete, if more sectors
;	must be transfered then the hardware interrupt handler will call this
;	procedure again to begin another transfer where the previous one
;	left off.  This continues until all sectors requested by a command
;	have been transfered.
;
;	Only read and write commands are supported.
;
;   ENTRY:
;	Interrupts disabled
;	EDI -> Current BDD
;	ESI -> Current Command
;
;   EXIT:
;	None
;
;   USES:
;	EAX, ECX, EDX, Flags
;
;==============================================================================

BeginProc WDCtrl_Send_Cmd_To_Ctrl

	Assert_Ints_Disabled

	VMMcall Get_Last_Updated_System_Time
	mov	[WD_Last_Cmd_Time], eax

;
;   Wait for the controller to be ready
;
	movzx	edx, [edi.BDP_Status_Port]
	in	al, dx				; Get the status
	test	al, WDStat_Busy 		; Q: Is DRQ asserted?
	jz	SHORT WD_SCTC_Ctrl_Ready
	VMMcall Get_Last_Updated_System_Time
	mov	ecx, eax
WD_SCTC_Wait_Out_Busy:
	sti
	IO_Delay
	IO_Delay
	in	al, dx				; Get the status
	test	al, WDStat_Busy 		; Q: Is DRQ asserted?
	cli
	jz	SHORT WD_SCTC_Ctrl_Ready
	VMMcall Get_Last_Updated_System_Time
	sub	eax, ecx
	cmp	eax, WD_Busy_Timeout
	jb	WD_SCTC_Wait_Out_Busy
	jmp	WD_SCTC_Cmd_Timed_Out

;
;   The controller is ready to accept a command
;
WD_SCTC_Ctrl_Ready:
	mov	ecx, [WD_Remaining_Sec_Count]	; ECX = # sectors remaining
	cmp	ecx, WD_Max_Sector_Xfer 	; Q: Larger than max xfer?
	jbe	SHORT WD_SCTC_Count_OK		;    N: Good!  Do it all
	mov	ecx, WD_Max_Sector_Xfer 	;    Y: Restrict to this size

WD_SCTC_Count_OK:
	mov	[WD_Rem_Sec_This_Xfer], cl	; Set xfer count for this block

	mov	al, [edi.BDP_Drive_Control]	; AL = Drive control
	mov	ah, [edi.BDP_Write_Precom]	; AH = Write precompensation

        movzx   edx, [edi.BDP_Base_IO_Port]
	add	edx, WDIO_Drive_Control_Off
	out	dx, al				; Set drive control

	add	edx, WDIO_Precomp_Off-WDIO_Drive_Control_Off
	mov	al, ah
	IO_Delay
	IO_Delay
	out	dx, al				; Set write procompensation

	inc	edx				; EDX -> Sector count port
	mov	al, cl
	IO_Delay
	IO_Delay
	out	dx, al				; Set sector count

	mov	ebx, edx			; Save I/O port

	mov	eax, [WD_Next_Start_Sector]	; EAX = Sector to begin xfer at
	add	ecx, eax			; ECX = Base+Total this xfer
	mov	[WD_Next_Start_Sector], ecx	; Remember this info for later

	xor	edx, edx			; Zero high dword for idiv
	idiv	[edi.BDD_Num_Sec_Per_Track]	; Remainder+1 = Starting sector
	mov	ecx, edx
	xor	edx, edx			; Zero high dword for idiv
	idiv	[edi.BDD_Num_Heads]		; EAX = Cylinder number
						; EDX = Starting head
	xchg	eax, ecx			; Move stuff around to make
	xchg	ebx, edx			; it easier to do I/O

;------------------------------------------------------------------------------
;
;   At this point values in registers are as follows:
;	EAX = Sector # (0 BASED!)
;	EBX = Starting head
;	ECX = Cylinder number
;	EDX = Port number for sector count
;	EDI -> Current BDD
;	ESI -> Current Command block
;
;------------------------------------------------------------------------------

	inc	edx				; EDX -> Sector number port
	inc	al				; Sectors start at 1
	out	dx, al				; Set starting sector for xfer

	inc	edx				; EDX -> Cylinder LOW port
	mov	al, cl
	IO_Delay
	IO_Delay
	out	dx, al				; Set low cylinder value

	inc	edx				; EDX -> Cylinder HIGH port
	mov	al, ch
	IO_Delay
	IO_Delay
	out	dx, al				; Set high cylinder value

	inc	edx				; EDX -> Drive select/head port
	mov	al, bl
	or	al, 10100000b			; 512 bytes, drive 0
	cmp	edi, OFFSET32 WD_Drive_80h_BDD
	je	SHORT WD_SCTC_Output_Head
	or	al, 00010000b			; Drive 1!
WD_SCTC_Output_Head:
	IO_Delay
	IO_Delay
	out	dx, al				; Output head/drive select value

	inc	edx				; EDX -> Command port

	cmp	[esi.BD_CB_Command], BDC_Read	; Q: Is this a read command?
	jne	SHORT WD_SCTC_Write_Command	;    N: Must be a write!

	mov	al, 20h
	IO_Delay
	IO_Delay
	out	dx, al				; Start the read
	ret


WD_SCTC_Write_Command:
	mov	al, 30h
	IO_Delay
	IO_Delay
	out	dx, al				; Start the write

        movzx   edx,[edi.BDP_Status_Port]
	IO_Delay
	IO_Delay
	in	al, dx				; Get the status
	test	al, WDStat_DRQ			; Q: Is DRQ asserted?
	jnz	SHORT WDCtrl_Write_Sector

	VMMcall Get_Last_Updated_System_Time
	mov	ecx, eax
WD_SCTC_Wait_Til_Ready:
	sti
	IO_Delay
	IO_Delay
	in	al, dx				; Get the status
	test	al, WDStat_DRQ			; Q: Is DRQ asserted?
	cli
	jnz	SHORT WDCtrl_Write_Sector
	VMMcall Get_Last_Updated_System_Time
	sub	eax, ecx
	cmp	eax, WD_Write_Ready_Timeout
	jb	WD_SCTC_Wait_Til_Ready

;
;   This command has timed out.  Return error.
;
WD_SCTC_Cmd_Timed_Out:
	Debug_Out "WARNING:  Command #ESI for WDCTRL drive #EDI timed-out"

	pushad
	mov	[esi.BD_CB_Cmd_Status], BDS_Device_Error
	call	WDCtrl_Cur_Cmd_Cplt
	popad
	ret

EndProc WDCtrl_Send_Cmd_To_Ctrl


;******************************************************************************
;
;   WDCtrl_Write_Sector
;
;   DESCRIPTION:
;	This procedure is called to write a single sector of data to the
;	hard disk conrtoller.  It updates the global variables:
;	    WD_Cur_Xfer_Ptr
;	    WD_Remaining_Sec_Count
;	    WD_Remaining
;
;   ENTRY:
;	EDI -> Current BDD
;
;   EXIT:
;	None
;
;   USES:
;	ECX, EDX, ESI, Flags
;
;==============================================================================

BeginProc WDCtrl_Write_Sector

	mov	esi, [WD_Cur_Xfer_Ptr]		; ESI -> Buffer of data to write
	mov	ecx, 100h			; Write 256 words
	mov	dx, [edi.BDP_Base_IO_Port]	; DX = Data port
	.ERRNZ	WDIO_Data_Off			; (Paranoia...)
	cld					; Don't forget this!
	rep outsw				; Pump data out to controller

	dec	[WD_Cur_Region_Count]		; Q: At end of scatter/gather?
	jnz	SHORT WD_WS_Have_Xfer_Ptr	;    N: Use ESI
	mov	esi, [WD_Cur_Region_Ptr]	;    Y: ESI -> Region list
	add	esi, 8				;	ESI -> Next region
	mov	ecx, DWORD PTR [esi]		;	ECX = Region size
	jecxz	SHORT WD_WS_End_Of_List 	;	If 0 then end of list
	mov	[WD_Cur_Region_Count], ecx	;	else save count
	mov	[WD_Cur_Region_Ptr], esi	;	Save new region list ptr
	mov	esi, DWORD PTR [esi+4]		;	ESI = Linear addr of xfer

WD_WS_Have_Xfer_Ptr:
WD_WS_End_Of_List:
	mov	[WD_Cur_Xfer_Ptr], esi		; Save new xfer pointer
	dec	[WD_Remaining_Sec_Count]	; One less sector
	dec	[WD_Rem_Sec_This_Xfer]		; One less this xfer block
	ret

EndProc WDCtrl_Write_Sector


;******************************************************************************
;
;   WDCtrl_Hw_Int
;
;   DESCRIPTION:
;	BlockDev will call this procedure whenever a hard disk interrupt
;	occurs.
;
;   ENTRY:
;	Interrupts disabled
;	EAX = IRQ handle for hard disk interrupt
;	EBX = Current VM handle
;	EDI -> BDD
;
;   EXIT:
;	If carry flag is set then
;	    Interrupt handled by this procedure -- Do not chain to next device
;	else
;	    Interrupt was not generated by this device -- Chain to next device
;
;   USES:
;	EAX, EBX, ECX, EDX, ESI, EDI, Flags
;
;==============================================================================


;------------------------------------------------------------------------------
;
;   Interrupt was not handled by this device.  Return with carry set to
;   indicate that the interrupt should be presented to the next device.
;   Note that this code is placed here so that the jump can be short.
;
;------------------------------------------------------------------------------


BeginProc WD_Hw_Int_Not_Mine

	stc
	ret

EndProc WD_Hw_Int_Not_Mine


;==============================================================================


BeginProc WDCtrl_Hw_Int

	Assert_Ints_Disabled

	cmp	edi, [WD_Current_BDD]		; Q: BDD of current command?
	jne	SHORT WD_Hw_Int_Not_Mine	;    N: Can't be for me.

;------------------------------------------------------------------------------
;
;   Since WD_Current_Command can be set to -1 while the current command
;   is being terminated, we check for a value of either -1 or 0 to indicate
;   that no command is being processed.
;
;------------------------------------------------------------------------------

	mov	ecx, [WD_Current_Command]	; ECX = Command pointer
	jecxz	SHORT WD_Hw_Int_Not_Mine	; If 0 no cmd in progress
	inc	ecx				; Q: Is it -1?
	jz	SHORT WD_Hw_Int_Not_Mine	;    Y: Forget it

	mov	ecx, eax			; Save IRQ handle

	movzx	edx, [edi.BDP_Status_Port]      ; Probably is alt status
	in	al, dx				; Read the drive status

	test	al, WDStat_Busy 		; Q: Is controller busy?
	jnz	WD_Hw_Int_Not_Mine		;    Y: Not my interrupt

        IO_Delay
        IO_Delay
	movzx	edx, [edi.BDP_Base_IO_Port]     ; Now read real status to clear
	add	edx, WDIO_Status_Off
	in	al, dx				; Read the drive status

	xchg	ecx, eax			; Save status in CL / Get IRQ
	VxDcall VPICD_Phys_EOI			; End of this interupt

	mov	esi, [WD_Current_Command]

	test	cl, WDStat_Error
	jnz	WD_HI_Error

	or	[WD_Accum_Status], cl		; Accumulate status bits

	cmp	[esi.BD_CB_Command], BDC_Read	; Q: Is this a read command?
	jne	SHORT WD_HI_Write		;    N: Must be a write

;------------------------------------------------------------------------------
;
;   Read command
;   At this point EDX = Status I/O port
;
;------------------------------------------------------------------------------

	add	edx, WDIO_Data_Off-WDIO_Status_Off

	mov	edi, [WD_Cur_Xfer_Ptr]		; EDI -> Buffer to read into
	mov	ecx, 100h			; Read 256 words
	cld					; Don't forget this!
	rep insw				; Read the data

	dec	[WD_Cur_Region_Count]		; Q: At end of scatter/gather?
	jnz	SHORT WD_HI_Have_Xfer_Ptr	;    N: Use EDI
	mov	edi, [WD_Cur_Region_Ptr]	;    Y: EDI -> Region list
	add	edi, 8				;	EDI -> Next region
	mov	ecx, DWORD PTR [edi]		;	ECX = Region size
	jecxz	SHORT WD_HI_Have_Xfer_Ptr	;	If 0 then end of list
	mov	[WD_Cur_Region_Count], ecx	;	else save count
	mov	[WD_Cur_Region_Ptr], edi	;	Save new region list ptr
	mov	edi, DWORD PTR [edi+4]		;	EDI = Linear addr of xfer

WD_HI_Have_Xfer_Ptr:
	mov	[WD_Cur_Xfer_Ptr], edi		; Set new xfer pointer

	mov	ecx, [WD_Remaining_Sec_Count]	; ECX = Count of remaing sectors
	dec	ecx				; One less sector to read
	mov	[WD_Remaining_Sec_Count], ecx	; Set new value
	dec	[WD_Rem_Sec_This_Xfer]		; Q: End of this block?
	jnz	SHORT WD_HI_Eat_This_Int	;    N: Just eat the int
						;    Y: May be end of command

	mov	edi, [WD_Current_BDD]		; Reset EDI to point to BDD
	jecxz	SHORT WD_HI_Command_Complete	; If 0 remaining then done
						; else process next block
;
;   This code will also be jumped to by the write command code to process
;   the next data block of a command.
;
WD_HI_Proc_Next_Block:
	call	WDCtrl_Send_Cmd_To_Ctrl 	; Send new block cmd to ctrl
	jmp	SHORT WD_HI_Eat_This_Int	; and eat the interrupt


;------------------------------------------------------------------------------
;
;   Write command
;
;------------------------------------------------------------------------------

WD_HI_Write:
	xor	ecx, ecx
	cmp	[WD_Rem_Sec_This_Xfer], cl	; Q: Still writing?
	je	SHORT WD_HI_End_Of_Write_Block	;    N: End of this block

	call	WDCtrl_Write_Sector		; Send data to controller
	jmp	SHORT WD_HI_Eat_This_Int	; and we're done


;
;   No more sectors to transfer
;
WD_HI_End_Of_Write_Block:
	cmp	[WD_Remaining_Sec_Count], ecx	; Q: Any sectors left in cmd?
	jne	SHORT WD_HI_Proc_Next_Block	;    Y: Process next block
						;    N: Command is complete

;------------------------------------------------------------------------------
;
;   Command processing is complete.  Set the command status and call
;   WDCtrl_Cur_Cmd_Cplt to complete processing of this command
;
;------------------------------------------------------------------------------

WD_HI_Command_Complete:
	mov	esi, [WD_Current_Command]
	.ERRNZ BDS_Success
	xor	eax, eax
	cmp	[WD_Num_Retries], al
	je	SHORT WD_HI_Test_ECC
	mov	al, BDS_Success_With_Retries
	jmp	SHORT WD_HI_Finish_It_Now
WD_HI_Test_ECC:
	test	[WD_Accum_Status], WDStat_ECC_Corrected ; Q: Assumpt correct?
	jz	SHORT WD_HI_Finish_It_Now
	mov	al, BDS_Success_With_ECC
WD_HI_Finish_It_Now:
	mov	[esi.BD_CB_Cmd_Status], ax
	call	WDCtrl_Cur_Cmd_Cplt

;------------------------------------------------------------------------------
;
;   Done!  Return with carry clear to indicate that we handled the interrupt.
;
;------------------------------------------------------------------------------

WD_HI_Eat_This_Int:
	clc
	ret

;------------------------------------------------------------------------------
;
;   Ack!  Gag!	Error, error, error!  Does not compute!
;   Status is in the CL register.  Interrupt has been EOIed.
;   EDI -> Current BDD.  ESI -> Current command.  CL = Status.
;
;------------------------------------------------------------------------------

WD_HI_Error:
	cmp	[esi.BD_CB_Command], BDC_Read	; Q: Is this a read?
	jne	SHORT WD_HI_Do_Retry		;    N: Don't clear data buffer

	test	cl, WDStat_DRQ			; Q: Data avaliable
	jz	SHORT WD_HI_Do_Retry		;    N: Don't clear data buffer
						;    Y: Read garbage
	pushad
	mov	edi, [WD_Cur_Xfer_Ptr]		; EDI -> Buffer to read into
	mov	ecx, 100h			; Read 256 words
	cld					; Don't forget this!
	rep insw				; Read the data
	popad

;
;   We'll retry this instruction if we haven't already retried too many times
;   already.
;
WD_HI_Do_Retry:
IFDEF DEBUG
	Assert_Ints_Disabled			; Must be disabled to retry
	inc	[WD_Total_Retries]
ENDIF
	inc	[WD_Num_Retries]
	cmp	[WD_Num_Retries], WD_Max_Retries
	ja	SHORT WD_HI_Fail_It
	mov	[WD_Current_Command], 0
	call	WDCtrl_Command
	jmp	WD_HI_Eat_This_Int

WD_HI_Fail_It:
IFDEF DEBUG
	pushad
	movzx	edx, [edi.BDP_Base_IO_Port]     ; Now read real status to clear
	add	edx, WDIO_Error_Off
	Trace_Out "WDCTRL I/O OPERATION #ESI ON DRIVE #EDI FAILED"
	in	al, dx
	Debug_Out "Status port is #CL, error condition #AL"
	cmp	[esi.BD_CB_Command], BDC_Read
	jne	SHORT WD_HI_Debug_Write_Failed
	inc	[WD_Total_Failed_Reads]
	jmp	SHORT WD_HI_Debug_Exit
WD_HI_Debug_Write_Failed:
	inc	[WD_Total_Failed_Writes]
WD_HI_Debug_Exit:
	popad
ENDIF
	mov	[esi.BD_CB_Cmd_Status], BDS_Media_Error
	call	WDCtrl_Cur_Cmd_Cplt
	jmp	WD_HI_Eat_This_Int

EndProc WDCtrl_Hw_Int


;******************************************************************************
;
;   WDCtrl_Cur_Cmd_Cplt
;
;   DESCRIPTION:
;	Due to the fact that commands can be queued up for more than one
;	drive, this procedure must be used instead of calling BlockDev_-
;	Command_Complete.
;
;	The caller is responsible for filling the BD_CB_Cmd_Status field
;	of the current command.
;
;	This procedure calls BlockDev_Command_Complete and processes any queued
;	command.  We will set the Current_Command and Current_BDD variables to
;	-1 to indicate that a command completion is in progress.  If another
;	command is sent during this call-back and there is currently a pending
;	command then the WDCtrl_Command procedure will automatically decide
;	which command to make the current command.  If the pending command has
;	the same or greater priority as the new command then the pending
;	command will be executed first.
;
;	This procedure will zero the retry count byte before calling
;	BlockDev_Command_Complete.  This is done because the WDCTRL_Command
;	procedure is used for retrying I/O operations that fail (and therefore
;	could not zero this counter).
;
;   ENTRY:
;	EDI = WD_Current_BDD
;	Interrupts disabled
;
;   EXIT:
;	Interrupts disabled (although they will be enabled during this call)
;
;   USES:
;	EAX, EBX, ECX, EDX, ESI, and EDI may be modified.
;
;==============================================================================

BeginProc WDCtrl_Cur_Cmd_Cplt

IFDEF DEBUG
	Assert_Ints_Disabled
	cmp	edi, [WD_Current_BDD]
	je	SHORT WD_CCC_EDI_Is_OK
	Debug_Out "ERROR:  Invalid internal state in WDCtrl_Cur_Cmd_Cplt"
WD_CCC_EDI_Is_OK:
ENDIF

	xor	esi, esi
	mov	[WD_Last_Cmd_Time], esi
	cmp	[WD_Pending_Command], esi	; Q: Any pending command
	je	SHORT WD_CCC_Finish_Cmd_Off	;    N: Just clear cur cmd
	or	esi, -1 			;    Y: ESI = FFFFFFFFh

WD_CCC_Finish_Cmd_Off:
	xchg	esi, [WD_Current_Command]	; Get command ptr and BDD for

	mov	[WD_Num_Retries], 0		; Reset this when any cmd done!
	VxDcall BlockDev_Command_Complete	; Call command complete for cmd
						; ENABLES INTERRUPTS!
	cli					; Ints back off

	cmp	[WD_Current_Command], -1	; Q: Process pending cmds?
	jne	SHORT WD_CCC_Exit		;    N: We're done
	xor	esi, esi
	mov	[WD_Current_Command], esi	; No current command
	xchg	esi, [WD_Pending_Command]	; Zero pending command
	mov	edi, [WD_Pending_BDD]
	CallRet WDCtrl_Command			; Send command to self

WD_CCC_Exit:
	ret

EndProc WDCtrl_Cur_Cmd_Cplt

;******************************************************************************
;
;   WDCtrl_Periodic_Time_Out
;
;   DESCRIPTION:
;	This procedure is a fail-safe measure to present an error message to
;	the user if a command does not complete in a reasonable amount of
;	time (default of 5 seconds).  This procedure will be called periodically
;	even if no disk activity is taking place.  If the amount of time
;	since the last command was submitted to the controller exceeds the
;	time-out limit then an error message will be displayed.
;
;   ENTRY:
;	(From global time-out call-back)
;	EBX = Current VM handle
;
;   EXIT:
;	None
;
;   USES:
;	EAX, EBX, ECX, EDX, ESI, EDI, Flags
;
;==============================================================================

BeginProc WDCtrl_Periodic_Time_Out

	mov	ecx, [WD_Last_Cmd_Time]
	jecxz	WD_PTO_Reset_Time_Out

	VMMcall Get_Last_Updated_System_Time
	sub	eax, ecx
	jb	SHORT WD_PTO_Reset_Time_Out	; Wrap!  Happens every 32 days!
	cmp	eax, [WD_Max_Cmd_Time]
	jbe	SHORT WD_PTO_Reset_Time_Out

	mov	edi, OFFSET32 WD_Time_Out_Error_Title
	mov	eax, MB_SYSTEMMODAL OR MB_ASAP OR MB_ICONEXCLAMATION OR MB_NOWINDOW OR MB_OK
	mov	ecx, OFFSET32 WD_Time_Out_Error_Message
	Assert_Cur_VM_Handle ebx
	VxDcall SHELL_SYSMODAL_Message

WD_PTO_Reset_Time_Out:
	mov	eax, [WD_Max_Cmd_Time]
	mov	esi, OFFSET32 WDCtrl_Periodic_Time_Out
	VMMjmp	Set_Global_Time_Out

EndProc WDCtrl_Periodic_Time_Out

;******************************************************************************
;
;   WDCtrl_IO_Port_Trap
;
;   DESCRIPTION:
;	This procedure is called whenever a VM attempts to read or write
;	one of the hard disk controller's I/O ports.  Whenever this happens
;	this procedure will return a value of 0FFh for input and ignore
;	all output.
;
;   ENTRY:
;	EBX = Current VM handle
;	ECX = Type of I/O
;	EDX = Port number
;	EBP -> Client register structure
;	If output then
;	    EAX/AX/AL = Data output to port
;
;   EXIT:
;	AL = 0FFh (simulate bus noise if input)
;
;   USES:
;	EAX, Flags
;
;==============================================================================

BeginProc WDCtrl_IO_Port_Trap

	Debug_Out "WDCTRL ERROR -- I/O ATTEMPTED TO PORT #DX, TYPE #ECX, EAX=#EAX"

	Emulate_Non_Byte_IO

	mov	al, TRUE
	xchg	al, [WD_IO_Warn_Msg_Disp]
	cmp	al, TRUE
	je	SHORT WD_IPT_Exit

	xor	edi, edi
	mov	eax, MB_SYSTEMMODAL OR MB_ASAP OR MB_ICONEXCLAMATION OR MB_NOWINDOW OR MB_OK
	mov	ecx, OFFSET32 WD_IO_Access_Error
	Assert_Cur_VM_Handle ebx
	VxDcall SHELL_SYSMODAL_Message

WD_IPT_Exit:
	mov	al, 0FFh			; Ignored if output, 0FFh ret
	ret					; for input.

EndProc WDCtrl_IO_Port_Trap


VxD_CODE_ENDS


	END
