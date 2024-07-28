PAGE 58,132
;******************************************************************************
TITLE INT13.ASM -- Int 13h API translation VxD
;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1990
;
;   Title:	INT13.ASM -- Int 13h API translation VxD
;
;   Version:	1.00
;
;   Date:	24-Aug-1990
;
;   Author:	RAL
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;   24-Aug-1990 RAL Original
;   07-Oct-1990 RAL Added more documentation -- Support get params call
;   08-Oct-1990 RAL Made job of this VxD much easier by moving hook code to
;		    BlockDev.386
;
;==============================================================================
;
;   This VxD contains all code related to translating Int 13h
;   calls from V86 mode programs and DOS into Windows/386 block device
;   driver calls.
;
;------------------------------------------------------------------------------

	.386p


;******************************************************************************
;			      I N C L U D E S
;******************************************************************************

	.XLIST
	INCLUDE VMM.Inc
	INCLUDE Debug.Inc
	INCLUDE OptTest.Inc
	INCLUDE BlockDev.Inc
	.LIST


;******************************************************************************
;	    I N C L U D E   C R E A T E S   S E R V I C E   T A B L E
;******************************************************************************

	Create_Int13_Service_Table EQU TRUE

	INCLUDE Int13.Inc


;******************************************************************************
;	       V I R T U A L   D E V I C E   D E C L A R A T I O N
;******************************************************************************

Declare_Virtual_Device INT13, 3, 0Ah, Int13_Control, Int13_Device_ID, \
		       Int13_Init_Order

;******************************************************************************
;		   I N I T I A L I Z A T I O N	 D A T A
;******************************************************************************

VxD_IDATA_SEG

EXTRN INIIndosPoll:BYTE
EXTRN INIOverlappedIO:BYTE

VxD_IDATA_ENDS


;******************************************************************************
;			   L O C K E D	 D A T A
;******************************************************************************

VxD_DATA_SEG

;
;   NOTE:  The following tables contain entries for all BIOS supported
;	   floppy and fixed disk drives.  The table entries are in the
;	   following order.
;
;		Offset 0 = Data for drive 00h
;		Offset 4 = Data for drive 80h
;		Offset 8 = Data for drive 01h
;		Offset C = Data fro drive 81h
;
;	   This is done for efficency so that the following code can be
;	   used to access the tables:
;
;		movzx	eax, [Int_13_Drive_Number]
;		rol	al, 1
;		mov	eax, Table_Name[eax*4]
;
;
;	    Although this VxD does not support floppy drives, the tables
;	    have been set up so that they could be supported in the future.
;

;
;   This table is used to store handles for up to two fixed disks and two
;   floppy drives.
;
I13_Table_Size EQU 4
I13_BDD_Table	    dd	    I13_Table_Size dup (0)


;
;   Table of status bytes for each possible drive
;
I13_Last_Stat_Table db	    4 dup (0)


;
;   Buffer for one sector to be used when can't lock caller's RAM.
;
I13_Panic_Xfer_Buff db	200h dup (?)


;
;   Semaphore used for Int 13h synchronization
;
I13_Disk_Ready_Sem  dd	?

;
;   Flags used to call Wait_Semaphore with.  If overlapped IO is disabled
;   then we will set the Block_Poll flag to prevent task switches except
;   to handle hardware interrupts (same behavior as normal BIOS poll loop).
;
I13_Wait_Flags	    dd	(Block_Svc_Ints OR Block_Enable_Ints)

VxD_DATA_ENDS




;******************************************************************************
;	       D E V I C E   C O N T R O L   P R O C E D U R E
;******************************************************************************

VxD_CODE_SEG

;******************************************************************************
;
;   Int13_Control
;
;   DESCRIPTION:
;
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc Int13_Control

	Control_Dispatch Sys_Critical_Init, Int13_Sys_Critical_Init
	Control_Dispatch Init_Complete, OEM_Specific_Hack

IFDEF DEBUG
	EXTRN Int13_Debug_Init:NEAR
	Control_Dispatch Device_Init,	    Int13_Debug_Init
	EXTRN Int13_Debug_Query:NEAR
	Control_Dispatch Debug_Query,	    Int13_Debug_Query
ENDIF
	clc
	ret

EndProc Int13_Control




VxD_CODE_ENDS

;******************************************************************************
;	       D E V I C E   C O N T R O L   H A N D L E R S
;******************************************************************************

VxD_ICODE_SEG


;******************************************************************************
;
;   Int13_Sys_Critical_Init
;
;   DESCRIPTION:
;
;   ENTRY:
;	EBX = System VM handle
;	EBP -> Client register structure
;
;   EXIT:
;	Carry flag clear to if successful
;
;   USES:
;
;==============================================================================

BeginProc Int13_Sys_Critical_Init

;
;   Check for a specific user selection of OverlappedIO
;
	xor	esi, esi			; Use 386ENH section
	.ERRNZ True+1
	lea	eax, [esi-1]			; Default is true
	mov	edi, OFFSET32 INIOverlappedIO	; EDI -> String to find
	VMMcall Get_Profile_Boolean		; Q: Was entry found?
	jnc	SHORT I13_SCI_Test_Poll 	;    Y: Use this value
						;    N: Default to opposite
						;	of InDOSPolling
	xor	eax, eax			; Default to false
	mov	edi, OFFSET32 INIInDOSPoll	; EDI -> String to find
	VMMcall Get_Profile_Boolean		; Get the value
	not	eax				; We'll do the opposite

I13_SCI_Test_Poll:
	test	eax, eax			; Q: Is overlapped IO enabled
	jnz	SHORT I13_SCI_Alloc_Sem 	;    Y: Don't set the flag
	SetFlag [I13_Wait_Flags], Block_Poll	;    N: Turn on poll flag

;
;   Initialize semaphore to be used by the Int 13h BIOS hook
;
I13_SCI_Alloc_Sem:
	xor	ecx, ecx			; Zero tokens
	VMMcall Create_Semaphore		; Semaphore used to wait until
						; disk I/O completed
IFDEF DEBUG
	jnc	SHORT I13_SCI_No_Msg
	Debug_Out "INT13.386 UNABLE TO CREATE SEMAPHORE"
I13_SCI_No_Msg:
ENDIF
	mov	[I13_Disk_Ready_Sem], eax

	ret					; Carry already set/clear

EndProc Int13_Sys_Critical_Init


VxD_ICODE_ENDS

VxD_CODE_SEG

;******************************************************************************
;			     S E R V I C E S
;******************************************************************************

;******************************************************************************
;
;   Int13_Get_Version
;
;   DESCRIPTION:
;	Returns the version number of the Int13 virtual device
;
;   ENTRY:
;	None
;
;   EXIT:
;	AH=Major version number
;	AL=Minor version number
;
;   USES:
;
;==============================================================================

BeginProc Int13_Get_Version, Service

	mov	eax, 030Ah
	clc
	ret

EndProc Int13_Get_Version


VxD_CODE_ENDS


VxD_ICODE_SEG

BeginDoc
;******************************************************************************
;
;   Int13_Device_Registered (INITIALIZATION ONLY)
;
;   DESCRIPTION:
;	This service is called by BlockDev.386 whenever a block device driver
;	identifies itself as an Int 13h drive.
;
;   ENTRY:
;	EDI -> Block Device Desctiptor
;
;   EXIT:
;	If carry flag is clear then
;	    Success
;	else
;	    Error:  Unable to allocate handle/Duplicate Int 13h drive
;
;   USES:
;	Flags
;
;==============================================================================
EndDoc

BeginProc Int13_Device_Registered, Service

	pushad

	movzx	edx, [edi.BDD_Int_13h_Number]
	cmp	dl, 80h
	jb	SHORT I13_DR_Error
	cmp	dl, 81h
	ja	SHORT I13_DR_Error

	rol	dl, 1
	cmp	[I13_BDD_Table][edx*4], 0
	jne	SHORT I13_DR_Error
	mov	[I13_BDD_Table][edx*4], edi

	popad
	clc
	ret

I13_DR_Error:
	Debug_Out "Int 13h rejecting FastDisk #DL, DDB=#EDI"
	popad
	stc
	ret

EndProc Int13_Device_Registered


;******************************************************************************
;
;   OEM_Specific_Hack (Called at Init_Complete)
;
;   DESCRIPTION:
;	This procedure is used for Dell computers so that people won't
;	complain about the disk cylinder number not blinking.  It is a
;	hack that looks for the word "Dell" at F000:003Eh.  If it is found
;	then it will display a 4 character string on the blinky light
;	display so long as bit 5 in the byte at 40:B4h is zero (when this
;	bit is set, the BIOS will not blink the blinky lights).
;
;	NOTE:  Since this routine touches ROM memory, it must be called
;	       *after* all sys_critical_init processing has been done
;	       so that limulators can set up page tables properly.
;
;   ENTRY:
;	None
;
;   EXIT:
;	Carry clear (always succeeds)
;
;   USES:
;	Can use EAX, EBX, ECX, EDX, ESI, EDI, or EBP
;
;==============================================================================

VxD_IDATA_SEG

OEM_Hack_String db "WIN3"

VxD_IDATA_ENDS

BeginProc OEM_Specific_Hack

;
;   Check for any Int 13h drives registered.
;
	mov	esi, OFFSET32 I13_BDD_Table
	xor	ecx, ecx
	.ERRNZ I13_Table_Size-4
	cld
	lodsd
	or	ecx, eax
	lodsd
	or	ecx, eax
	lodsd
	or	ecx, eax
	lodsd
	or	ecx, eax
	jecxz	OEM_Hack_Exit

	cmp	DWORD PTR ds:[0F003Eh], "lleD"
	jne	SHORT OEM_Hack_Exit

	test	BYTE PTR ds:[4B4h], 20h
	jnz	SHORT OEM_Hack_Exit

	cld
	mov	esi, OFFSET32 OEM_Hack_String
	mov	dx, 5Bh
	mov	ecx, 4
OEM_Hack_Loop:
	lodsb
	out	dx, al
	IO_Delay
	IO_Delay
	dec	edx
	loopd	OEM_Hack_Loop

OEM_Hack_Exit:
	clc
	ret

EndProc OEM_Specific_Hack


VxD_ICODE_ENDS


VxD_CODE_SEG


BeginDoc
;******************************************************************************
;
;   Int13_Hooking_BIOS_Int
;
;   DESCRIPTION:
;	This service is called by BlockDev whenever the BIOS Int 13h interrupt
;	is being hooked.  Note that it may be called more than once, but that
;	it is guaranteed to be called at least once during initialization.
;	This service MUST remain resident since it can be called at system
;	exit time (it can't be in the VxD_ICODE segment).
;
;	Note that is service is called AFTER the interrupt has been hooked.
;	Therefore, this service should not do an Exec_Int of an Int 13h as
;	it would call the Int13_Translate_VM_Int service.  Instead, use
;	the Build_Int_Stack_Frame/Resume_Exec services to call the BIOS.
;
;	The value passed to this service in ECX can be called by the
;	Int13_Translate_VM_Int service if it needs to chain to the original
;	ROM BIOS at any point in time.	Since this VxD always replaces
;	the ROM BIOS, you would normally ignore the value in EAX.
;
;   ENTRY:
;	EAX = CS:IP of original Int 13h BIOS handler
;	ECX = CS:IP of original ROM BIOS Int 13h hook
;
;   EXIT:
;	None
;
;   USES:
;	Can use EAX, EBX, ECX, EDX, ESI, EDI, and Flags
;
;==============================================================================
EndDoc

BeginProc Int13_Hooking_BIOS_Int, Service

	ret					; Not interested in this data!

EndProc Int13_Hooking_BIOS_Int

BeginDoc
;******************************************************************************
;
;   Int13_Unhooking_BIOS_Int
;
;   DESCRIPTION:
;	This service is called when exiting BEFORE the Int 13h chain is
;	unhooked.
;
;   ENTRY:
;	EAX = CS:IP of original DOS BIOS Int 13h hook
;	ECX = CS:IP of original ROM BIOS Int 13h hook
;
;   EXIT:
;	None
;
;   USES:
;	Can use EAX, EBX, ECX, EDX, ESI, EDI, and Flags
;
;==============================================================================
EndDoc

BeginProc Int13_Unhooking_BIOS_Int, Service

	ret					; Not real interesting

EndProc Int13_Unhooking_BIOS_Int


BeginDoc
;******************************************************************************
;
;   Int13_Translate_VM_Int
;
;   DESCRIPTION:
;	This service is called by BlockDev when an Int 13h is intercepted.
;
;   ENTRY:
;	EBX = Current VM handle
;	EBP -> Client register structure
;
;   EXIT:
;
;   USES:
;
;==============================================================================
EndDoc

;------------------------------------------------------------------------------
;
;   Jump table used by this procedure
;
;------------------------------------------------------------------------------

VxD_DATA_SEG

I13_Dispatch_Table LABEL DWORD
	dd	OFFSET32 I13_TV13_Reset 	 ; AH = 00h -- Reset drive
	dd	OFFSET32 I13_TV13_Read_Last_Stat ; AH = 01h -- Read stat last cmd
	dd	OFFSET32 I13_TV13_Read		 ; AH = 02h -- Read sectors
	dd	OFFSET32 I13_TV13_Write 	 ; AH = 03h -- Write sectors
	dd	OFFSET32 I13_TV13_Verify	 ; AH = 04h -- Verify sectors
	dd	OFFSET32 I13_TV13_Not_Supported  ; AH = 05h -- Format cylinder
	dd	OFFSET32 I13_TV13_Not_Supported  ; AH = 06h -- Format & mark bad
	dd	OFFSET32 I13_TV13_Not_Supported  ; AH = 07h -- Format drive
	dd	OFFSET32 I13_TV13_Read_Parameters; AH = 08h -- Read drive params
	dd	OFFSET32 I13_TV13_Not_Supported  ; AH = 09h -- Init drive params
	dd	OFFSET32 I13_TV13_Not_Supported  ; AH = 0Ah -- Read Long
	dd	OFFSET32 I13_TV13_Not_Supported  ; AH = 0Bh -- Write Long
	dd	OFFSET32 I13_TV13_Seek		 ; AH = 0Ch -- Seek
	dd	OFFSET32 I13_TV13_Alt_Reset	 ; AH = 0Dh -- Alternate reset
	dd	OFFSET32 I13_TV13_Not_Supported  ; AH = 0Eh -- Diagnostic
	dd	OFFSET32 I13_TV13_Not_Supported  ; AH = 0Fh -- Diagnostic
	dd	OFFSET32 I13_TV13_Test_Ready	 ; AH = 10h -- Test drive ready
	dd	OFFSET32 I13_TV13_Recalibrate	 ; AH = 11h -- Recalibrate
	dd	OFFSET32 I13_TV13_Not_Supported  ; AH = 12h -- Diagnostic
	dd	OFFSET32 I13_TV13_Not_Supported  ; AH = 13h -- Diagnostic
	dd	OFFSET32 I13_TV13_Diagnostic	 ; AH = 14h -- Diagnostic
	dd	OFFSET32 I13_TV13_Read_DASD_Type ; AH = 15h -- Read DASD Type
	dd	OFFSET32 I13_TV13_Not_Supported  ; AH = 16h -- Diskette only
	dd	OFFSET32 I13_TV13_Not_Supported  ; AH = 17h -- Diskette only
	dd	OFFSET32 I13_TV13_Not_Supported  ; AH = 18h -- Diskette only
	dd	OFFSET32 I13_TV13_Park_Heads	 ; AH = 19h -- Park heads
	dd	OFFSET32 I13_TV13_Not_Supported  ; AH = 1Ah -- Format unit

	Max_I13_Command EQU ($-I13_Dispatch_Table)/4-1

VxD_DATA_ENDS

;------------------------------------------------------------------------------


BeginProc Int13_Translate_VM_Int, High_Freq, Service

        ;** In case we get a reset call to a fixed disk, we want to just
        ;**     reset drive 0.  We cannot allow reset calls to go through
        ;**     for hard disks that we do not own as the BIOS will
        ;**     try to reprogram disks we DO own if a reset goes through
        ;**     to a non-FastDisk drive.
        xor     edx, edx
        cmp     [ebp.Client_AH], dl             ;Reset is AH=0
        jne     SHORT I13_TVI_Not_Reset
        cmp     [ebp.Client_DL], 80h            ;Fixed disk?
        jb      SHORT I13_TVI_Not_Reset         ;  No.
        mov     [ebp.Client_DL], dl             ;  Yes, set to drive zero

I13_TVI_Not_Reset:
	mov	dl, [ebp.Client_DL]
	test	dl, 01111110b
	jnz	DEBFAR I13_TV13_Chain
	rol	dl, 1
	mov	edi, [I13_BDD_Table][edx*4]
	test	edi, edi
	jz	SHORT I13_TV13_Chain

	VMMcall Simulate_Iret

	sub	esp, SIZE BlockDev_Command_Block
	mov	esi, esp

	movzx	ecx, [ebp.Client_AH]
	cmp	ecx, Max_I13_Command
	ja	SHORT I13_TV13_Invalid_Command

	mov	[esi.BD_CB_Next], 0
	mov	[esi.BD_CB_Flags], BDCF_High_Priority
	mov	[esi.BD_CB_Cmd_Cplt_Proc], OFFSET32 Int13_Command_Call_Back
	jmp	I13_Dispatch_Table[ecx*4]

;------------------------------------------------------------------------------
;
;   Command is not supported!
;
;------------------------------------------------------------------------------

I13_TV13_Invalid_Command:
	Debug_Out "WARNING!  Invalid command #CX being failed by Int13.386"
	mov	[ebp.Client_AH], I13Stat_Bad_Command
	jmp	SHORT I13_TV13_Return_Error

;------------------------------------------------------------------------------
;
;   All handlers will jump to one of these two common exit points.
;   Both handlers must be jumped to with EDI -> Block Device Descriptor
;
;------------------------------------------------------------------------------

I13_TV13_Return_Success:
	mov	[ebp.Client_AH], I13Stat_Success
	ClrFlag [ebp.Client_EFlags], CF_Mask

I13_TV13_Common_Exit:
	movzx	edx, [edi.BDD_Int_13h_Number]
	rol	dl, 1
	mov	al, [ebp.Client_AH]
	mov	I13_Last_Stat_Table[edx], al
I13_TV_Dont_Set_Stat:
	add	esp, SIZE BlockDev_Command_Block
	clc
	ret

I13_TV13_Return_Error:
	SetFlag [ebp.Client_EFlags], CF_Mask
	jmp	I13_TV13_Common_Exit


I13_TV13_No_Status_Exit:
	ClrFlag [ebp.Client_EFlags], CF_Mask
	jmp	I13_TV_Dont_Set_Stat


;------------------------------------------------------------------------------
;
;   This Int 13h was not for a virtualized drive.  Chain to BIOS.
;
;------------------------------------------------------------------------------

I13_TV13_Chain:
	stc
	ret


;------------------------------------------------------------------------------
;
;   Reset Disk System (AH=00h)
;
;   This command is ignored and always succeeds.  It is inappropriate for
;   one VM to reset the state of a piece of hardware that is global.
;
;   IN:
;	Client_AH=00h
;	Client_DL=Drive number
;   OUT:
;	Client carry flag clear
;	Client_AH=Status (0 -- Always succeeds)
;
;------------------------------------------------------------------------------

IFDEF DEBUG
I13_TV13_Reset:
	Debug_Out "WARNING:  Disk reset for drive #EDI being ignored"
	jmp	I13_TV13_Return_Success
ELSE
	I13_TV13_Reset EQU I13_TV13_Return_Success
ENDIF


;------------------------------------------------------------------------------
;
;   Read Status of Last Operation (AH=01h)
;
;   IN:
;	Client_AH=01h
;	Client_DL=Drive number
;   OUT:
;	Client carry flag clear
;	Client_AH=Status (0 -- Always succeeds)
;	Client_AL=Status of last operation for drive
;
;------------------------------------------------------------------------------

I13_TV13_Read_Last_Stat:
	movzx	edx, [ebp.Client_DL]
	rol	dl, 1
	mov	al, I13_Last_Stat_Table[edx]
	mov	[ebp.Client_AL], al
	jmp	I13_TV13_Return_Success


;------------------------------------------------------------------------------
;
;   Read Desired Sectors into Memory (AH=02h)
;
;------------------------------------------------------------------------------

I13_TV13_Read:
	mov	[esi.BD_CB_Command], BDC_Read
	jmp	SHORT I13_TV13_Read_Write_Common

;------------------------------------------------------------------------------
;
;   Write Desired Sectors from Memory (AH=03h)
;
;------------------------------------------------------------------------------

I13_TV13_Write:
	mov	[esi.BD_CB_Command], BDC_Write

;------------------------------------------------------------------------------
;
;   Common entry point for read and write commands.
;
;------------------------------------------------------------------------------

I13_TV13_Read_Write_Common:
	VMMcall Enable_VM_Ints			; Enable ints so page lock will
						; be able to service ints

	call	Int13_Calc_Sector_Params

IFDEF DEBUG
	EXTRN	Int13_Debug_Start_IO:NEAR
	call	Int13_Debug_Start_IO
ENDIF


;
;   Get a pointer to the buffer and save it in the command block
;
	movzx	eax, [ebp.Client_ES]
	movzx	edx, [ebp.Client_BX]
	shl	eax, 4
	add	eax, [ebx.CB_High_Linear]
	add	eax, edx
	mov	[esi.BD_CB_Buffer_Ptr], eax

;
;   If it's only one sector then the copy will be faster than locking the
;   page down anyway.
;
	cmp	ecx, 1
	je	SHORT I13_TV13_Chop_It_No_Problem

;
;   Lock the memory
;
	shl	ecx, 9				; * 512
	dec	ecx
	mov	edx, eax
	and	edx, 0FFFh
	add	ecx, edx
	shr	ecx, 12
	inc	ecx				; ECX = # pages to lock

	shr	eax, 12 			; EAX = Starting page to lock

	xor	edx, edx

	push	edx				; Push params once for unlock
	push	ecx
	push	eax

	VMMcall _LinPageLock, <eax, ecx, edx>	; Lock the pages
	test	eax, eax			; Q: Did the lock work
	jz	SHORT I13_TV13_Break_It_Up	;    N: Chop transfer up
						;    Y: Do I/O right now
	call	Int13_Send_Command

	VMMcall _LinPageUnlock
	add	esp, 3*4

	jmp	SHORT I13_TV13_IO_Complete

;
;   We have to chop this command into single sector reads!
;
I13_TV13_Break_It_Up:
	Debug_Out "INT13 warning: Unable to lock memory for disk I/O.  Buffering command #ESI"

	add	esp, 3*4			; Throw away unlock params

I13_TV13_Chop_It_No_Problem:
	mov	ebx, 1
	xchg	ebx, [esi.BD_CB_Count]
	mov	edx, OFFSET32 I13_Panic_Xfer_Buff
	xchg	edx, [esi.BD_CB_Buffer_Ptr]

I13_TV13_Chop_Loop:
	mov	[esi.BD_CB_Next], 0

	cmp	[esi.BD_CB_Command], BDC_Read	; Q: Read operation?
	je	SHORT I13_TV13_Do_1_IO		;    Y: Will read into buffer
						;    N: Copy data to buffer
	xchg	edx, esi			; ESI -> Caller's buffer
	push	edi
	mov	edi, OFFSET32 I13_Panic_Xfer_Buff;EDI -> Buffer we will write
	mov	ecx, 80h			; Copy 128 DWORDs
	cld
	rep movsd
	pop	edi				; Restore EDI and ESI
	xchg	edx, esi			; and update pointer

I13_TV13_Do_1_IO:
	call	Int13_Send_Command		; DO I/O now

	cmp	[esi.BD_CB_Cmd_Status], BDS_First_Error_Code
	jae	SHORT I13_TV13_IO_Complete

	cmp	[esi.BD_CB_Command], BDC_Read
	jne	SHORT I13_TV13_Next_Sector

	xchg	edx, edi
	push	esi
	mov	esi, OFFSET32 I13_Panic_Xfer_Buff
	mov	ecx, 80h
	cld
	rep movsd
	pop	esi
	xchg	edx, edi

I13_TV13_Next_Sector:
	inc	DWORD PTR [esi.BD_CB_Sector]
	dec	ebx
	jnz	I13_TV13_Chop_Loop


;------------------------------------------------------------------------------
;
;   Read or write operation is complete.  Now determine what status to return
;   for the operation.
;
;------------------------------------------------------------------------------

I13_TV13_IO_Complete:
IFDEF DEBUG
	EXTRN	Int13_Debug_End_IO:NEAR
	call	Int13_Debug_End_IO
ENDIF


;
;   NOTE:  Verify Sectors command jumps to this point to return the
;	   appropriate Int 13h operation status
;
I13_TV13_Determine_Op_Status LABEL NEAR
	xor	al, al				; No error
	mov	cx, [esi.BD_CB_Cmd_Status]

	cmp	cx, BDS_Success_With_ECC
	jne	SHORT I13_TV13_Check_Error_Status
	mov	al, I13Stat_ECC_Corrected
	jmp	SHORT I13_TV13_Set_Op_Status
I13_TV13_Check_Error_Status:
	cmp	cx, BDS_First_Error_Code
	jb	SHORT I13_TV13_Set_Op_Status
	.ERRNZ	BDS_First_Error_Code-BDS_Invalid_Sector_Number
	je	SHORT I13_TV13_Bad_Sector
	mov	al, I13Stat_Bad_Block
	jmp	SHORT I13_TV13_Set_Op_Status

I13_TV13_Bad_Sector:
	mov	al, I13Stat_Sec_Not_Found

I13_TV13_Set_Op_Status:
	test	al, al
	jnz	SHORT I13_TV13_Error_Status

;
;   Read operations return a western digital compatible status byte in AL
;   only if the operation succeeds.  Otherwise, AL is zeroed.  For write
;   operations, AL is not changed.  A successful status is 50h.
;
	cmp	[esi.BD_CB_Command], BDC_Read
	jne	I13_TV13_Return_Success
	mov	[ebp.Client_AL], 50h
	jmp	I13_TV13_Return_Success

I13_TV13_Error_Status:
	mov	[ebp.Client_AH], al
	cmp	[esi.BD_CB_Command], BDC_Read
	jne	I13_TV13_Return_Error
	mov	[ebp.Client_AL], 0
	jmp	I13_TV13_Return_Error


;------------------------------------------------------------------------------
;
;   AH = 04H -- Verify Desired Sectors from Memory
;
;------------------------------------------------------------------------------

I13_TV13_Verify:

	mov	[esi.BD_CB_Command], BDC_Verify

	call	Int13_Calc_Sector_Params
	call	Int13_Send_Command

;
;   Some fastdisk device drivers may not support the verify command.  If not
;   then this Int 13h will simply return success.
;
	cmp	[esi.BD_CB_Cmd_Status], BDS_Invalid_Command
	je	I13_TV13_Return_Success
	jmp	I13_TV13_Determine_Op_Status


;------------------------------------------------------------------------------
;
;   AH = 08H -- Read Drive Parameters
;   IN:
;	Client_DL = Drive number (80h or 81h)
;   OUT:
;	Client_DL = Number of consecutive drives
;	Client_DH = Maximum value for head number
;	Client_CH = Maximum value for cylinder number
;	Client_CL = Maximum value for sector and high order 2 bits of cyl #
;
;------------------------------------------------------------------------------

I13_TV13_Read_Parameters:

	mov	al, 1
	mov	ecx, edx
	xor	ecx, 10b
	cmp	I13_BDD_Table[ecx*4], 0
	je	SHORT I13_RP_Get_Other_Params
	inc	al

I13_RP_Get_Other_Params:
	mov	ah, BYTE PTR [edi.BDD_Num_Heads]
	dec	ah				; Does this in the ROM!

	mov	[ebp.Client_DX], ax

	mov	ax, WORD PTR [edi.BDD_Num_Cylinders]
	sub	ax, 2				; DOES THIS IN THE ROM TOO!
	shl	ah, 6
	or	ah, BYTE PTR [edi.BDD_Num_Sec_Per_Track]
	xchg	ah, al
	mov	[ebp.Client_CX], ax

	jmp	I13_TV13_Return_Success

;------------------------------------------------------------------------------
;
;   Get Type of Drive (AH=15h)
;
;------------------------------------------------------------------------------

I13_TV13_Read_DASD_Type:
	mov	[ebp.Client_AX], 300h		; Type = Fixed disk
	mov	eax, [edi.BDD_Num_Cylinders]
	dec	eax				; Remove landing zone cyl
	imul	eax, [edi.BDD_Num_Heads]
	imul	eax, [edi.BDD_Num_Sec_Per_Track]
	mov	[ebp.Client_DX], ax
	shr	eax, 16
	mov	[ebp.Client_CX], ax

	jmp	I13_TV13_No_Status_Exit


;==============================================================================
;
;   Commands below this point are ignored and will always return success.
;   The different entry points are only provided so that the debug version
;   will present useful information about what programs are doing with Int 13h.
;
;==============================================================================


;------------------------------------------------------------------------------
;
;   Seek Cylinder (AH=0Ch)
;
;------------------------------------------------------------------------------

IFDEF DEBUG
I13_TV13_Seek:
	Debug_Out "Int 13h Seek call being ignored (will return success)"
	jmp	I13_TV13_Return_Success
ELSE
I13_TV13_Seek EQU I13_TV13_Return_Success
ENDIF

;------------------------------------------------------------------------------
;
;   Alternate Disk Reset (AH=0Dh)
;
;------------------------------------------------------------------------------

IFDEF DEBUG
I13_TV13_Alt_Reset:
	Debug_Out "Int 13h Alt_Reset call being ignored (will return success)"
	jmp	I13_TV13_Return_Success
ELSE
I13_TV13_Alt_Reset EQU I13_TV13_Return_Success
ENDIF

;------------------------------------------------------------------------------
;
;   Test Drive Ready (AH=10h)
;
;------------------------------------------------------------------------------

IFDEF DEBUG
I13_TV13_Test_Ready:
	Debug_Out "Int 13h Test_Ready call being ignored (will return success)"
	jmp	I13_TV13_Return_Success
ELSE
I13_TV13_Test_Ready EQU I13_TV13_Return_Success
ENDIF

;------------------------------------------------------------------------------
;
;   Recalibrate Drive (AH=11h)
;
;------------------------------------------------------------------------------

IFDEF DEBUG
I13_TV13_Recalibrate:
	Debug_Out "Int 13h Recalibrate call being ignored (will return success)"
	jmp	I13_TV13_Return_Success
ELSE
I13_TV13_Recalibrate EQU I13_TV13_Return_Success
ENDIF

;------------------------------------------------------------------------------
;
;   Controller Diagnostic (AH=14h)
;
;------------------------------------------------------------------------------

IFDEF DEBUG
I13_TV13_Diagnostic:
	Debug_Out "Int 13h diagnostic call being ignored (will return success)"
	jmp	I13_TV13_Return_Success
ELSE
I13_TV13_Diagnostic EQU I13_TV13_Return_Success
ENDIF


;------------------------------------------------------------------------------
;
;   Park drive heads (AH=19h)
;
;------------------------------------------------------------------------------

IFDEF DEBUG
I13_TV13_Park_Heads:
	Debug_Out "Int 13h Park_Heads call being ignored (will return success)"
	jmp	I13_TV13_Return_Success
ELSE
I13_TV13_Park_Heads EQU I13_TV13_Return_Success
ENDIF


;==============================================================================
;
;   UNSUPPORTED COMMANDS.
;   These commands are valid Int 13h calls, but they are either used for
;   diagnostics or to format the hard disk.  They will all return with a
;   bad command status.
;
;==============================================================================

I13_TV13_Not_Supported:
	Debug_Out "Int13 WARNING:  Unsupported command #CL returning bad command error"
	mov	[ebp.Client_AH], I13Stat_Bad_Command
	jmp	I13_TV13_Return_Error

EndProc Int13_Translate_VM_Int


;******************************************************************************
;
;   Int13_Calc_Sector_Params
;
;   DESCRIPTION:
;
;
;   ENTRY:
;	ESI -> Command block to fill in sector number for.
;	EBP -> Client register structure for current VM
;	Client registers contain head/cyl/sector data for Int 13h.
;
;   EXIT:
;	[esi.BD_CB_Sector] filled in with appropriate sector number
;	[esi.BD_CB_Count] filled in with transfer size in sectors
;	EAX contians sector number
;	ECX contains number of sectors to transfer
;
;   USES:
;	EAX, ECX, Flags
;
;==============================================================================

BeginProc Int13_Calc_Sector_Params

	xor	eax, eax
	mov	DWORD PTR [esi.BD_CB_Sector][4], eax	; High dword always 0

;
;   Calculate the sector to read using:
;	(((CylNum * NumHeads) + StartHead) * SecPerTrack) + StartSector - 1
;
	movzx	eax, [ebp.Client_CX]
	mov	ecx, eax
	shr	al, 6
	xchg	al, ah				; EAX = Starting Cylinder

	imul	eax, [edi.BDD_Num_Heads]	; * Number of heads

	movzx	edx, [ebp.Client_DH]
	add	eax, edx			; + Starting head

	imul	eax, [edi.BDD_Num_Sec_Per_Track]; * Sectors per track

	and	ecx, 111111b
	lea	eax, [eax][ecx][-1]		; + Starting sector - 1

	mov	DWORD PTR [esi.BD_CB_Sector], eax

;
;   Get the number of sectors and save it in the command block
;
	movzx	ecx, [ebp.Client_AL]
	mov	[esi.BD_CB_Count], ecx

	ret

EndProc Int13_Calc_Sector_Params



;******************************************************************************
;
;   Int13_Send_Command
;
;   DESCRIPTION:
;	This procedure calls BlockDev_Send_Command and then blocks until
;	the command has completed.
;
;   ENTRY:
;	ESI -> Command block
;	EDI -> BDD for FastDisk to send command to
;
;   EXIT:
;
;   USES:
;	EAX, ECX, Flags
;
;==============================================================================

BeginProc Int13_Send_Command

	VxDcall BlockDev_Send_Command

	mov	eax, [I13_Disk_Ready_Sem]
	mov	ecx, [I13_Wait_Flags]
	VMMjmp	Wait_Semaphore

EndProc Int13_Send_Command



;******************************************************************************
;
;   Int13_Command_Call_Back
;
;   DESCRIPTION:
;	This procedure is called when a command completes.  It simply signals
;	a semaphore to wake up the waiting VM and returns.
;
;   ENTRY:
;	Interrupts disabled
;	ESI -> Command block
;	EDI -> BDD for drive that completed
;
;   EXIT:
;	No return values
;
;   USES:
;	EAX, Flags
;
;==============================================================================

BeginProc Int13_Command_Call_Back, High_Freq

	mov	eax, [I13_Disk_Ready_Sem]
	VMMcall Signal_Semaphore

	ret

EndProc Int13_Command_Call_Back


VxD_CODE_ENDS


	END
