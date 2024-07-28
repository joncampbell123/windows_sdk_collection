	PAGE 58,132
;******************************************************************************
TITLE PageFile.Asm - Demand Paging File Device
;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1988-1990
;
;   Title:	PageFile.Asm - Demand Paging File Device
;
;   Version:	1.1
;
;   Date:	4-Oct-90
;
;   Author:	JEM
;
;	This virtual device implements the paging file services for the
;	PageSwap device.  The paging file may either be a DOS file
;	accessed via Int 21h (worse), a 'partition' file accessed via
;	Int 13h (better), or a block device (best).
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;   04-Oct-1990 JEM Original, adapted from PageSwap.Asm by RAL
;
;   Change log from PageSwap.Asm:
;
;   09-Oct-1988 RAL Original
;   02-Feb-1989 RAL Complete re-design (version 2.0)
;   07-Feb-1989 RAL Victim page finding works
;   17-Feb-1989 RAL Lots of debugging code
;   27-Feb-1989 RAL Fixed TONS of bugs.  Works with a partition file.
;   28-Feb-1989 RAL Checksum pages in debug version to verify reads.
;   05-Mar-1989 RAL Improved debugging report
;   10-Apr-1989 RAL Removed anoying trace_outs
;   27-Apr-1989 RAL Grow DOS swap file by one byte and flush to disk to avoid
;		    strange overwrite FAT bug of doom.
;   30-May-1989 RAL Added "MaxPagingFileSize" system.ini entry
;   06-Jun-1989 RAL Works with new partion file format
;   31-Jul-1989 RAL Uses V86MMGR to get page to map paging buffer
;   18-Sep-1989 RAL Direct to hardware paging enabled
;   02-Oct-1989 RAL Put contig lin pages in adjcent disk sectors
;   13-Nov-1989 RAL New service + calls Call_When_Idle service + Lock cache
;   04-Dec-1989 RAL Allocates private V86 stack/Fixed bugs in Grow File code
;   13-Jan-1990 RAL Enter critical section on page in/out services
;
;==============================================================================

	.386p

;******************************************************************************
;			      I N C L U D E S
;******************************************************************************

	.XLIST
	INCLUDE VMM.Inc
	INCLUDE Debug.Inc
	INCLUDE DOSMgr.Inc
	INCLUDE V86MMGR.Inc
	INCLUDE Shell.Inc
	INCLUDE BlockDev.Inc
	INCLUDE SmartDrv.Inc
	INCLUDE PDB.Inc
	INCLUDE Int2FAPI.Inc
	INCLUDE OptTest.Inc

	Create_PageFile_Service_Table EQU TRUE

	INCLUDE PageFile.Inc
	INCLUDE PageSwap.Inc

	INCLUDE SPART.INC
	INCLUDE SPOEM.INC
	.LIST

;******************************************************************************
;		V I R T U A L	D E V I C E   D E C L A R A T I O N
;******************************************************************************

Declare_Virtual_Device PageFile, 2, 0, PageFile_Control, PageFile_Device_ID, \
		       PageFile_Init_Order, , PageFile_Svc_Call

;******************************************************************************
;			      E Q U A T E S
;******************************************************************************


PF_Sector_Size	    EQU     200h

PF_Min_File_Grow    EQU     100h	    ; Grow file in 1Mb chunks
PF_Min_File_Pages   EQU     80h 	    ; Must have at least 512K to page

PF_Default_Reserve  EQU     2000	    ; reserve 2Meg on disk

PF_V86_Stack_Size   EQU     400 	    ; 400 byte stack while paging

;******************************************************************************
;******************************************************************************

VxD_IDATA_SEG

EXTRN PF_Enable_Ini:BYTE
EXTRN PF_Swap_File_Ini:BYTE
EXTRN PF_Swap_Drive_Ini:BYTE
EXTRN PF_Min_Free_Ini:BYTE
EXTRN PF_Max_Size_Ini:BYTE
EXTRN PF_Invalid_Part_Msg:BYTE
EXTRN PF_Caption_Title_Msg:BYTE


PF_Init_Last_Sector	dd	?
PF_Init_DOS_Start	dd	?
PF_Init_DOS_End 	dd	?

PF_Spart_File_Name	db	"SPART.PAR", 0

PF_Error_Msg_Ptr	dd	0

PF_Swap_File_Name	db	"WIN386.SWP", 0
PF_Swap_File_Name_Len	EQU	$-PF_Swap_File_Name

PF_Corrpt_File_Name	db	128 dup (0)

VxD_IDATA_ENDS

VxD_DATA_SEG
;
; In the case where PF_Have_Partition is TRUE, this buffer contains the
;   WFP name of SPART.PAR.
;
; In the case where PF_Have_Partition is FALSE, this buffer contains
;   nothing unless PF_Spart_Was_Corrupt is TRUE in which case it is the
;   WFP name of the corrupt SPART.PAR.
;
PF_Spart_File_Buffer	db	128 dup(0)

PF_Spart_Was_Corrupt	db	False

VxD_DATA_ENDS

VxD_LOCKED_DATA_SEG
	ALIGN 4

PF_BD_Descriptor	dd	?		; Ptr to BlockDev_Device_Dscr

PF_Cancel_Sem		dd	?

PF_Orig_DOS_Vector	dd	?

PF_Max_File_Pages	dd	0		; If 0 then paging is disabled
PF_Cur_File_Pages	dd	0

PF_Lin_Page_Num 	dd	-1

PF_V86_Stack_Seg_Off	dd	?

PF_Cache_Lock_Ptr	dd	0

PF_BD_Cancel	BlockDev_Command_Block	<>	; Cancel BlockDev Cmd Block

PF_Save_User_Stack	dd	?
PF_Client_PSP		dw	?
PF_Our_PSP		dw	?
PF_File_Handle		dw	?

;
; In the case where PF_Have_Partition is TRUE, this buffer contains the
;   WFP name of the actual swap partition file extracted from SPART.PAR.
;
; In the case where PF_Have_Partition is FALSE, this buffer contains the
;   WFP name of the temporary DOS swap file.
;
PF_Our_File_Name	db	128 dup (0)

PF_Int13_Num_Heads	dd	?
PF_Int13_Sec_Per_Track	dd	?
PF_Int13_Base_Sector	dd	?

PF_Int13_Drive_Num	db	?


PF_Have_Partition	db	False
IFDEF DEBUG
PF_IO_In_Progress	db	False
ENDIF
PF_Using_BlockDev	db	False
PF_PermDel		db	False

PF_DOS_IO_Count 	db	0		; If <> 0 then we called DOS

VxD_LOCKED_DATA_ENDS


;******************************************************************************

	.SALL

	PAGE
;******************************************************************************
;		  R E A L   M O D E   I N I T	C O D E
;******************************************************************************

;******************************************************************************
;
;   PageFile_Real_Init
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;	edx = pointer to cache lock byte (Bimbo or smartdrive)
;
;   USES:
;
;==============================================================================

VxD_REAL_INIT_SEG

Smart_Drv_Name db SD_DEV_NAME
Smart_Drv_Read_Buff db SIZE SD_IOCTL_Read dup (?)

BeginProc PageFile_Real_Init

;   If another PageFile device is loaded then don't load -- Just abort our load

	test	bx, Duplicate_From_INT2F OR Duplicate_Device_ID
	jnz	SHORT PageFile_RI_Abort_Load


;   No other PageFile is loaded. Get Bimbo info about cache lock pointer

	mov	ax, (W386_Int_Multiplex SHL 8) OR W386_Device_Broadcast
	mov	bx, PageFile_Device_ID
	xor	cx, cx
	int	2Fh
	or	ax, ax			    ; if ax not 0, then...
	jnz	SHORT PF_RI_No_Bimbo	    ; ...Bimbo did not respond

; Bimbo responded to int2Fh - pointer to cach lock byte is in es:di.

	mov	dx, es
	shl	edx, 16
	mov	dx, di
	jmp	SHORT PageFile_RI_Exit

PF_RI_No_Bimbo:
;   Bimbo not around, get SmartDrv info about cache lock pointer

	mov	ax, 3D00h		    ; Open file DOS function
	mov	dx, OFFSET Smart_Drv_Name
	int	21h
	jc	SHORT PageFile_RI_No_SmartDrv

	mov	bx, ax			    ; BX = File handle
	mov	ax, 4400h		    ; Get Device Data IOCTL
	int	21h
	jc	SHORT PageFile_RI_Failed
	test	dx, 80h 		    ; Q: Is it a device?
	jz	SHORT PageFile_RI_Failed	  ;    N: Not Smart Drive
					    ;	 Y: Read control strings
	mov	dx, OFFSET Smart_Drv_Read_Buff
	mov	cx, SIZE SD_IOCTL_Read
	mov	ax, 4402h
	int	21h
	jc	SHORT PageFile_RI_Failed
	cmp	ax, SIZE SD_IOCTL_Read	    ; Q: Get it all?  (New Smartdrv?)
	jne	SHORT PageFile_RI_Failed    ;	 N: Darn Darn Darn

	mov	ah, 3Eh
	int	21h
	xor	edx, edx		    ; Assume no cache lock
	cmp	[Smart_Drv_Read_Buff.SD_IR_Major_Ver], 3
	jb	SHORT PageFile_RI_Exit	    ; Must be ver 3 or above for this
	mov	edx, [Smart_Drv_Read_Buff.SD_IR_Cache_Lock_Ptr]
	jmp	SHORT PageFile_RI_Exit

PageFile_RI_Failed:
	mov	ah, 3Eh 		    ; Close file handle
	int	21h			    ; And abort operation

PageFile_RI_No_SmartDrv:
	xor	edx, edx

PageFile_RI_Exit:
	xor	bx, bx
	xor	si, si
	mov	ax, Device_Load_Ok
	ret

PageFile_RI_Abort_Load:
	xor	bx, bx
	xor	si, si
	mov	ax, Abort_Device_Load + No_Fail_Message
	ret

EndProc PageFile_Real_Init

VxD_REAL_INIT_ENDS


	PAGE
;******************************************************************************
;	       P R O T E C T E D   M O D E   I N I T   C O D E
;******************************************************************************

VxD_ICODE_SEG

;******************************************************************************
;
;   PageFile_Sys_Critical_Init
;
;   DESCRIPTION:
;
;   ENTRY:
;	EBX = System VM handle
;	EDX = Reference data
;	      (Real mode seg:offset of smartdrv lock byte.  0 if no smartdrv)
;
;   EXIT:
;	Carry clear
;
;   USES:
;
;==============================================================================

BeginProc PageFile_Sys_Critical_Init

;   Convert real mode SEG:OFF to a linear address for smart drive cache lock

	movzx	eax, dx
	shr	edx, 16
	shl	edx, 4
	add	edx, eax
	mov	[PF_Cache_Lock_Ptr], edx

	clc
	ret

EndProc PageFile_Sys_Critical_Init


;******************************************************************************
;
;   PageFile_Init_Complete
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

BeginProc PageFile_Init_Complete

	cmp	[PF_Max_File_Pages], 0
	je	SHORT PF_IC_Print_Error_Msg


;   Allocate a page in V86 global memory for mapping.

	cmp	[PF_Using_BlockDev], True
	jz	SHORT PF_IC_Print_Error_Msg

	xor	esi, esi
	mov	ecx, 1000h
	VxDcall V86MMGR_Map_Pages
	jc	SHORT PF_IC_Cant_Get_Map_Page
	shr	edi, 12
	mov	[PF_Lin_Page_Num], edi
	call	PageFile_Map_Null
	jmp	SHORT PF_IC_Print_Error_Msg


;   STRANGE!  Unable to allocate a mapping page.

PF_IC_Cant_Get_Map_Page:
	Trace_Out "ERROR:  Unable to allocate mapping page for PageFile device"
	Debug_Out "This will probably be fatal (the MMGR thinks paging is enabled)"
	VxDcall PageFile_Clean_Up
	mov	[PF_Max_File_Pages], 0


;   If the partition is corrupted then we may need to display an error message.

PF_IC_Print_Error_Msg:
	mov	ecx, [PF_Error_Msg_Ptr]
	or	ecx,ecx
	jz	DEBFAR PF_IC_Exit
IFDEF DEBUG
	cmp	ecx, OFFSET32 PF_Invalid_Part_Msg
	je	short PFD60
	debug_out "BUG BUG PF_Invalid_Part_Msg is only error reported PageFile_Init_Complete"
PFD60:
ENDIF
	mov	[PF_Spart_Was_Corrupt],True

	VMMCall Get_SYS_VM_Handle

	mov	eax, MB_YESNO+MB_ICONEXCLAMATION+MB_SYSTEMMODAL+MB_ASAP+MB_NOWINDOW
	mov	edi, OFFSET32 PF_Caption_Title_Msg
	xor	esi, esi
	VxDcall Shell_SYSMODAL_Message
	cmp	eax,IDNO
	je	short PF_IC_Exit
	cmp	[PF_PermDel],True
	je	short PF_IC_Exit
	mov	esi, OFFSET32 PF_Spart_File_Buffer

	VMMCall Set_Delete_On_Exit_File
IFDEF DEBUG
	jnc	short PFD40
	debug_out "Could not set DELETE ON EXIT for permanent swap file 2a"
PFD40:
ENDIF
	mov	esi, OFFSET32 PF_Corrpt_File_Name

	VMMCall Set_Delete_On_Exit_File
IFDEF DEBUG
	jnc	short PFD50
	debug_out "Could not set DELETE ON EXIT for permanent swap file 2b"
PFD50:
ENDIF
	mov	[PF_PermDel],True
PF_IC_Exit:
	clc
	ret

EndProc PageFile_Init_Complete


BeginDoc
;******************************************************************************
;
;   PageFile_Init_File
;
;   DESCRIPTION:
;	Note: This service can only be called at initialization time!
;
;	This service will initialize the paging file.  It should only be called
;	from the PageSwap virtual device.  It will attempt to locate a paging
;	partition.  If no partition exists it will create a DOS paging file.
;
;   ENTRY:
;	None.
;
;   EXIT:
;	EAX = Current size of swap file in pages.
;	EBX = Maximum size of swap file in pages.
;	Note: EBX will be 0 if the swap file couldn't be initialized
;
;   USES:
;	Flags.
;
;==============================================================================
EndDoc

BeginProc PageFile_Init_File, Service

	pushad

	mov	ecx, [PF_Cache_Lock_Ptr]
	jecxz	PF_IF_Dont_Lock_Cache
	Begin_Touch_1st_Meg
	inc	BYTE PTR [ecx]
	End_Touch_1st_Meg
PF_IF_Dont_Lock_Cache:

	Push_Client_State
	VMMcall Begin_Nest_V86_Exec


;   Test for paging enabled

	mov	eax, True			; Default value = ON
	xor	esi, esi			; Use Win386 section
	mov	edi, OFFSET32 PF_Enable_Ini	; Look for this string
	VMMcall Get_Profile_Boolean		; Get the value
	test	eax, eax			; Q: Has user disabled it
	jz	PF_Init_No_Paging		;    Y: Done!
						;    N: Try to open swap file

;   Get the REAL DOS Int 21h vector so we can call it directly at all times.

	mov	eax, 21h
	VMMcall Get_V86_Int_Vector
	shl	ecx, 10h
	mov	cx, dx				; ECX = Seg:Offset of Int 21h
	mov	[PF_Orig_DOS_Vector], ecx


;   Open the swap partition or file.  If the procedure returns with PF_Max_File
;   Pages = 0 then the file could not be opened or created and paging is
;   disabled.  Otherwise, PF_Max_File_Pages contains the maximum number of pages
;   that can be written to the swap file.

	call	PageFile_Open_Partition 	; Try to open the partition
	test	eax, eax			; Q: Did we get Temp V86 area?
	jz	PF_Init_No_Paging		;    N: HORRIBLE ERROR!
	cmp	[PF_Max_File_Pages], 0		; Q: Do we have a partition?
	jne	SHORT PF_Init_Free_V86_Area	;    Y: Good!  We'll use it.
	call	PageFile_Open_DOS_Swap_File	;    N: Try to use a DOS file

PF_Init_Free_V86_Area:
	VMMcall _Free_Temp_V86_Data_Area

	mov	ebx, [PF_Max_File_Pages]
	test	ebx, ebx
	jz	PF_Init_No_Paging


;  If we're using the BlockDev device for paging, then iniitialization is done.

	cmp	[PF_Using_BlockDev], True
	jz	SHORT PF_Init_Exit

;  Reserve a mapping page in the V86MMGR map region

	xor	eax, eax
	mov	bx, 0001h
	xor	ecx, ecx
	VxDcall V86MMGR_Set_Mapping_Info


;   Allocate V86 memory for our stack.

	VMMCall _Allocate_Global_V86_Data_Area, <PF_V86_Stack_Size, <GVDADWordAlign OR GVDAZeroInit>>
	test	eax, eax
	jz	SHORT PF_Cant_Get_Stack
	shl	eax, 12
	shr	ax, 12				; EAX = Seg:Off of base of stack
	add	ax, PF_V86_Stack_Size		; AX = SP to start at
	mov	[PF_V86_Stack_Seg_Off], eax	; Save pointer for later


;   Now do different stuff depending on wether we have a partition or are
;   swapping to a DOS file.

	cmp	[PF_Have_Partition], True	; Q: Do we have a partition?
	je	SHORT PF_Init_Exit		;    Y: Done!
						;    N: Do DOS type things

;   Swapping to a DOS file.  Hook Int 24h so we can fail it while paging.

	mov	esi, OFFSET32 PageFile_Int_24h
	mov	eax, 24h
	VMMcall Hook_V86_Int_Chain

;   Swapping to a DOS file.  Hook Int 23h so we can fail it while paging.

	mov	esi, OFFSET32 PageFile_Int_23h
	mov	eax, 23h
	VMMcall Hook_V86_Int_Chain

;   The end!

PF_Init_Exit:
	VMMcall End_Nest_Exec
	Pop_Client_State

	mov	ecx, [PF_Cache_Lock_Ptr]
	jecxz	PF_IF_Dont_Unlock_Cache
	Begin_Touch_1st_Meg
	dec	BYTE PTR [ecx]
	End_Touch_1st_Meg
PF_IF_Dont_Unlock_Cache:

	popad					; Restore caller's regs
	mov	eax,[PF_Cur_File_Pages] 	;   and setup return values
	mov	ebx,[PF_Max_File_Pages]
	clc
	ret


;   Paging could not be intialized for some reason.  Set Max_File_Pages to 0.

PF_Cant_Get_Stack:
	VxDcall PageFile_Clean_Up		; Try to remove the swap file

PF_Init_No_Paging:
	mov	[PF_Max_File_Pages], 0		; Make sure it's really off
	jmp	SHORT PF_Init_Exit

EndProc PageFile_Init_File


;******************************************************************************
;
;   PageFile_Open_Partition
;
;   DESCRIPTION:
;
;   ENTRY:
;	None
;
;   EXIT:
;	If [PF_Max_File_Pages] != 0 then
;	    Partition was found and can be used
;	else
;	    Partition file not found or corrupted
;	EAX = Linear Address of Temp_V86_Data_Area buffer (== 0 if couldn't alloc)
;
;   USES:
;
;==============================================================================

BeginProc PageFile_Open_Partition

	pushad

;   Try to open the SPART.PAR partition description file

	mov	edi, OFFSET32 PF_Spart_File_Buffer
	mov	edx, OFFSET32 PF_Spart_File_Name

	VMMCall OpenFile

	push	eax				; Save file handle
	pushfd					; And carry setting

	VMMcall _Allocate_Temp_V86_Data_Area, <400h, 0>

	pop	ecx				; Flags from OpenFile Call
	pop	[ebp.Client_EBX]		; File handle to BX register
	mov	[esp.Pushad_EAX],eax		; Return addr of Temp V86 buffer
	TestReg ecx, CF_MASK			; Did OpenFile Work?
	jnz	PF_OP_Exit			; If carry then not found
						; but NOT an error, just RET
	or	eax,eax 			; Did we get Temp_V86 area?
	jnz	SHORT ProcessSpart		; Yes
	mov	[ebp.Client_AX], 3E00h		; Close file
	call	PageFile_Call_DOS
	jmp	PF_OP_Exit

ProcessSpart:

;   We opened SPART.PAR successfully.  Now read the file's contents.

	shl	eax, 12 			; EAX = Seg:Offset for V86
	shr	ax, 12				; temp data area
	mov	[ebp.Client_DX], ax		; Point client's DS:DX
	shr	eax, 16 			; to the buffer
	mov	[ebp.Client_DS], ax		; DS:DX points to buffer

;   We opened SPART.PAR successfully.  Now read the file's contents.

	mov	[ebp.Client_CX], SIZE PFileForm ; Read this many bytes
	mov	[ebp.Client_AH], 3Fh		; DOS read
	call	PageFile_Call_DOS
	jc	SHORT PF_OP_Close_Part_File	; If carry invalid SPART.PAR
	cmp	[ebp.Client_AX], SIZE PFIleForm ;    Y: Q: Read all the data?
	jne	SHORT PF_OP_Close_Part_File	;	   N: Corrupted
	mov	[PF_Have_Partition], True	;	   Y: Looks like we've
						;	      got a partition
PF_OP_Close_Part_File:
	mov	[ebp.Client_AH], 3Eh		; DOS close
	call	PageFile_Call_DOS		; (BX still contains handle)

	cmp	[PF_Have_Partition], True
	jne	PF_OP_Invalid_Partition

	mov	edi, [esp.Pushad_EAX]		; EDI -> V86 buffer again
	VMMcall Get_Cur_VM_Handle
	add	edi, [ebx.CB_High_Linear]	; EDI -> High lin addr to touch
	lea	esi, [edi].PFileName		; ESI -> File name
	cmp	BYTE PTR [esi], 0		; Q: Null file?
	je	PF_OP_Invalid_Partition 	;    Y: No partition exists
						;    N: GOOD!  We have one!
;
; Copy the name of the perm swap partition into our buffer
;
	push	edi
	mov	edi, OFFSET32 PF_Our_File_Name
	mov	ecx, 127
	cld
CpPartNm:
	lodsb
	stosb
	or	al,al
	jz	short CpDn
	loop	CpPartNm
	xor	al,al
	stosb
CpDn:
	pop	edi
;
;   Make sure this is a partition we understand.
;
	cmp	[edi.PFileVersion], PARTCURVERSION
	jne	PF_OP_Invalid_Partition

;
;   We have read the contents of the SPART.PAR file into memory.  Now copy
;   the data that we will need to use into local variables.
;

	lea	esi, [edi].OEMField

	mov	ax, [esi.INT13DrvNum]
	mov	[PF_Int13_Drive_Num], al
	movzx	eax, [esi.Int13NumHeads]
	mov	[PF_Int13_Num_Heads], eax
	movzx	eax, [esi.INT13SecPerTrk]
	mov	[PF_Int13_Sec_Per_Track], eax

;
;   Calculate the first sector of our partition and save it.
;
	movzx	eax, [esi.StartCyln]
	movzx	ebx, [esi.HeadNumStart]
	imul	eax, [PF_Int13_Num_Heads]
	add	eax, ebx
	imul	eax, [PF_Int13_Sec_Per_Track]
	mov	[PF_Int13_Base_Sector], eax

;
;   Calculate the last sector of the partition and compute the size of the
;   partition in pages.
;
	movzx	eax, [esi.EndCyln]
	movzx	ebx, [esi.HeadNumEnd]
	imul	eax, [PF_Int13_Num_Heads]
	add	eax, ebx
	inc	eax				; Last track is ours too!
	imul	eax, [PF_Int13_Sec_Per_Track]	; EAX = First sector, Last trace
	dec	eax				; Back up to last sector
	mov	[PF_Init_Last_Sector], eax	; Save for use later
	sub	eax, [PF_Int13_Base_Sector]	; EAX = Total # sectors in part
	shr	eax, 3				; EAX = # pages possible (/ 8)
	mov	[PF_Max_File_Pages], eax
	mov	[PF_Cur_File_Pages], eax

;
;   Move some data we will need for initilization only into some variables
;   in the init data segment.
;
	mov	eax, [edi.DOSStartOffset]
	mov	[PF_Init_DOS_Start], eax
	mov	eax, [edi.DOSEndOffset]
	mov	[PF_Init_DOS_End], eax

;
;   Now we know where everything SHOULD be.  Now we have to make sure it really
;   is there or else we might start doing Int 13s to bad places.  To verify
;   that the file has not been moved by some random compaction utility we will
;   write to it through DOS and read it back using Int 13h.  If the last sector
;   and first sector match what we write with DOS then we will assume that the
;   file has not moved.

;
;   First try to open the file.  If this fails then were hosed.
;
	mov	[ebp.Client_AX], 3D02h		; DOS Open file
	lea	eax, [edi].PFileName		; EAX = Linear ptr to file name
	shl	eax, 12 			; EAX = Seg:Offset for part
	shr	ax, 12				; file name
	mov	[ebp.Client_DX], ax		; Point client's DS:DX
	shr	eax, 16 			; to the file name
	mov	[ebp.Client_DS], ax
	call	PageFile_Call_DOS		; Call Mr. Operating system
	jc	PF_OP_Invalid_Partition 	; If error then no partition
	mov	ebx, [ebp.Client_EAX]		; Else move file handle to BX
	mov	[ebp.Client_EBX], ebx		; for further DOS calls

;
;   We opened it!  Now read the last sector into the buffer.
;
	mov	eax, [PF_Init_DOS_End]
	mov	[ebp.Client_DX], ax
	shr	eax, 16
	mov	[ebp.Client_CX], ax
	mov	[ebp.Client_AX], 4200h		; Set file pointer
	call	PageFile_Call_DOS		; (BX still contains handle)
	jc	PF_OP_Invalid_Partition 	; If error then exit

	mov	[ebp.Client_AH], 3Fh		; DOS Read File
	mov	eax, edi			; EAX = Linear ptr to buffer
	shl	eax, 12 			; EAX = Seg:Offset of buffer
	shr	ax, 12				; to read sector into
	mov	[ebp.Client_DX], ax		; Point client's DS:DX
	shr	eax, 16 			; to the file name
	mov	[ebp.Client_DS], ax
	mov	[ebp.Client_CX], PF_Sector_Size ; Read one sector (512 bytes)
	call	PageFile_Call_DOS		; Call Mr. Operating system
	jc	SHORT PF_OP_Close_Partition	; If error then no partition
	cmp	[ebp.Client_AX], PF_Sector_Size ; Q: Did we read all 512 bytes?
	jne	SHORT PF_OP_Close_Partition	;    N: Error!

;
;   We successfully read the last sector of the file.  Now change it so
;   we will be sure of writing something that is not currently there.
;
	mov	ecx, PF_Sector_Size/4
PF_OP_Alter_Loop:
	xor	DWORD PTR [edi][ecx*4][-4], ecx ; Change the data
	loopd	PF_OP_Alter_Loop


;   Now make sure the partition is still in the right place.

	mov	ecx, [PF_Init_Last_Sector]
	mov	edx, [PF_Init_DOS_End]
	call	PageFile_Test_Correct_Sector
	jc	SHORT PF_OP_Close_Partition

	mov	ecx, [PF_Int13_Base_Sector]
	mov	edx, [PF_Init_DOS_Start]
	call	PageFile_Test_Correct_Sector

PF_OP_Close_Partition:
	pushfd
	mov	[ebp.Client_AH], 3Eh		; DOS close
	call	PageFile_Call_DOS		; (BX still contains handle)
	popfd

	jc	SHORT PF_OP_Invalid_Partition
	TestMem [ebp.Client_EFlags], CF_Mask
	jnz	SHORT PF_OP_Invalid_Partition


;   SUCCESS!  THE PARTITION IS VALID!
;
;   Check to see if the BlockDev device is available to do intelligent paging.

	VxDcall BlockDev_Get_Version		; Is BlockDev around?
	jc	SHORT PF_OP_Exit		;   no, do it the old way

	mov	al, [PF_Int13_Drive_Num]	; Locate the Device Descriptor
	VxDcall BlockDev_Find_Int13_Drive	;   for the paging drive
	jc	SHORT PF_OP_Exit
	mov	[PF_BD_Descriptor], edi

;   Create a semaphore to synchronize cancel I/O with BlockDev

	mov	ecx, 1
	VMMcall Create_Semaphore
	jc	SHORT PF_OP_Exit
	mov	[PF_Cancel_Sem], eax


	Trace_Out "***** Using BlockDev for paging! *****"

	mov	[PF_Using_BlockDev], True	; Going to use BlockDev for I/O

PF_OP_Exit:
	popad
	ret


;   For some reason the partition is not useable.  Return with Max_Pages=0.

PF_OP_Invalid_Partition:

	mov	[PF_Error_Msg_Ptr], OFFSET32 PF_Invalid_Part_Msg
	xor	ecx, ecx
	mov	[PF_Max_File_Pages], ecx
	mov	[PF_Cur_File_Pages], ecx
	.ERRNZ False
	mov	[PF_Have_Partition], cl
    ;
    ; Copy the name of the corrupt partition file into a new buffer
    ;	so that it does not get overlayed with the TEMP swap file name.
    ;
	mov	esi,offset32 PF_Our_File_Name
	mov	edi,offset32 PF_Corrpt_File_Name
	mov	ecx,128/4
	cld
	rep	movsd
	jmp	SHORT PF_OP_Exit

EndProc PageFile_Open_Partition


;******************************************************************************
;
;   PageFile_Test_Correct_Sector
;
;   DESCRIPTION:
;
;   ENTRY:
;	ECX = Sector number (0 based) for Int 13h read
;	EDX = DOS starting position
;	EDI -> Linear address of V86 memory to use.
;	       First 512 bytes contain data to write.
;	       Second 512 bytes are buffer to read into.
;	Client_BX = Handle of open partition file
;
;   EXIT:
;	If carry set then
;
;   USES:
;	All client registers except EBX
;
;==============================================================================

BeginProc PageFile_Test_Correct_Sector

	pushad

	mov	[ebp.Client_DX], dx
	shr	edx, 16
	mov	[ebp.Client_CX], dx
	mov	[ebp.Client_AX], 4200h		; Set file pointer
	call	PageFile_Call_DOS				 ; (BX contains handle)
	jc	PF_TCS_Error			; If error then exit

	mov	[ebp.Client_AH], 40h		; DOS Write File
	mov	eax, edi			; EAX = Linear ptr to buffer
	shl	eax, 12 			; EAX = Seg:Offset of buffer
	shr	ax, 12				; to read sector into
	mov	[ebp.Client_DX], ax		; Point client's DS:DX
	shr	eax, 16 			; to the file name
	mov	[ebp.Client_DS], ax
	mov	[ebp.Client_CX], PF_Sector_Size ; Read one sector (512 bytes)
	call	PageFile_Call_DOS				 ; Call Mr. Operating system
	jc	PF_TCS_Error			; If error then no partition
	cmp	[ebp.Client_AX], PF_Sector_Size ; Q: Did we write all 512 bytes?
	jne	PF_TCS_Error			;    N: Error!

;
;   Flush the DOS buffers to disk so we can read it back with Int 13h
;
	mov	[ebp.Client_AH], 0Dh
	call	PageFile_Call_DOS		; No possible error so must
						; ignore the carry flag
;
;   Do other calls necessary to flush other OEM's caches
;
	call	PageFile_OEM_Cache_Flush

;
;   Read back data
;
	push	[ebp.Client_EBX]
	mov	eax, ecx
	xor	edx, edx
	idiv	[PF_Int13_Sec_Per_Track]
	mov	ecx, edx			; Remainder = Starting sector
	xor	edx, edx
	idiv	[PF_Int13_Num_Heads]
	;   ECX = Sector # (0 BASED)!
	;   EDX = Starting head
	;   EAX = Cylinder number
	mov	ch, al
	inc	cl
	and	cl, 00111111b
	shl	ah, 6
	or	cl, ah
	mov	[ebp.Client_CX], cx		; Set cylinder and sector number
	mov	[ebp.Client_DH], dl
	mov	al, [PF_Int13_Drive_Num]
	mov	[ebp.Client_DL], al

	lea	eax, [edi+PF_Sector_Size]
	shl	eax, 12
	shr	ax, 12
	mov	[ebp.Client_BX], ax
	shr	eax, 16
	mov	[ebp.Client_ES], ax

	mov	[ebp.Client_AX], 0201h		; Read 1 sector
	mov	eax, 13h
	ClrFlag [ebp.Client_EFlags], DF_Mask	; WORK AROUND BUSTED SOFTWARE
	VMMcall Exec_Int
	pop	[ebp.Client_EBX]
	TestMem [ebp.Client_EFlags], CF_Mask
	jnz	SHORT PF_TCS_Error

;
;   Compare the data to make sure it is identical
;
	lea	esi, [edi+PF_Sector_Size]
	mov	ecx, PF_Sector_Size / 4
	cld
	repe cmpsd
	jne	SHORT PF_TCS_Error

	clc
	popad
	ret

PF_TCS_Error:
	stc
	popad
	ret

EndProc PageFile_Test_Correct_Sector


;******************************************************************************
;
;   PageFile_OEM_Cache_Flush
;
;   DESCRIPTION:
;	This procedure is used to force cache programs to write all data to
;	thd hard disk before performing our read-back validation.
;
;   ENTRY:
;	In nested execution mode (can do an Exec_Int)
;
;   EXIT:
;	None
;
;   USES:
;	Flags, Client registers are *NOT* modified
;
;==============================================================================

BeginProc PageFile_OEM_Cache_Flush

	push	eax
	Push_Client_State

;
;   Specific flush for NCACHE
;
	mov	[ebp.Client_AX], 0FE03h 	; Force delayed writes to disk
	mov	[ebp.Client_SI], "CF"
	mov	[ebp.Client_DI], "NU"
	SetFlag [ebp.Client_EFlags], CF_Mask
	mov	eax, 2Fh
	ClrFlag [ebp.Client_EFlags], DF_Mask	; WORK AROUND BUSTED SOFTWARE
	VMMcall Exec_Int

	Pop_Client_State
	pop	eax
	ret

EndProc PageFile_OEM_Cache_Flush


;******************************************************************************
;
;   PageFile_Open_DOS_Swap_File
;
;   DESCRIPTION:
;
;   ENTRY:
;	EAX -> TEMP V86 DATA AREA
;
;   EXIT:
;	Success is indicated by setting PF_Max_File_Pages
;	Failure by not modifying PF_Max_File_Pages
;
;   USES:
;
;==============================================================================

BeginProc PageFile_Open_DOS_Swap_File

	pushad

;
;   Check for a user-specified file name.  If one exists, use it and ignore
;   all other .INI parameters
;
	xor	esi, esi
	mov	edi, OFFSET32 PF_Swap_File_Ini
	VMMcall Get_Profile_String
	jc	SHORT PF_ODSF_Create_File_Name
	mov	esi, edx
	mov	edi, OFFSET32 PF_Our_File_Name
	cld
PF_ODSF_Copy_To_Our_Buff:
	lodsb
	stosb
	test	al, al
	jnz	PF_ODSF_Copy_To_Our_Buff
	jmp	SHORT PF_ODSF_Have_File_Name

;
;   Create the paging file name
;
PF_ODSF_Create_File_Name:
	xor	esi, esi
	mov	edi, OFFSET32 PF_Swap_Drive_Ini
	VMMcall Get_Profile_String
	mov	edi, OFFSET32 PF_Our_File_Name
	cld
	jc	SHORT PF_ODSF_Use_Config_Dir

	mov	bl, BYTE PTR [edx]
	and	bl, NOT ("a"-"A")	; make drive letter UPPER case
	cmp	bl, "A"
	jb	SHORT PF_ODSF_Use_Config_Dir
	cmp	bl, "Z"
	ja	SHORT PF_ODSF_Use_Config_Dir

;
;   Make sure the specified paging drive is valid.  If not, use the config
;   directory.
;
	mov	[ebp.Client_AH], 36h
	mov	al, bl
	sub	al, "A"-1
	mov	[ebp.Client_DL], al
	call	PageFile_Call_DOS
	cmp	[ebp.Client_AX], 0FFFFh
	je	SHORT PF_ODSF_Use_Config_Dir

	mov	al, bl
	stosb
	mov	ax, "\:"
	stosw
	jmp	SHORT PF_ODSF_Copy_File_Name

PF_ODSF_Use_Config_Dir:
	VMMcall Get_Config_Directory
	mov	esi, edx
PF_ODSF_Copy_Loop:
	lodsb
	test	al, al
	jz	SHORT PF_ODSF_Copy_File_Name
	stosb
	jmp	PF_ODSF_Copy_Loop

PF_ODSF_Copy_File_Name:
	mov	esi, OFFSET32 PF_Swap_File_Name
	mov	ecx, PF_Swap_File_Name_Len
	rep movsb

PF_ODSF_Have_File_Name:
;
;   If the swap file is left around from a previous run of Win386 nuke it.
;
	mov	edi, [esp.Pushad_EAX]
	call	PageFile_Nuke_Swap_File

;
;   Find out how much memory is available on swap disk.
;
	mov	al, [PF_Our_File_Name]
	and	al, NOT ("a"-"A")		; Make drive letter UPPER case
	sub	al, ("A"-1)
	mov	[ebp.Client_DL], al		; For swap drive
	mov	[ebp.Client_AH], 36h		; Get Disk Free Space
	call	PageFile_Call_DOS				 ; AX * BX * CX = Free space

	movzx	eax, [ebp.Client_AX]
	cmp	eax, 0FFFFh			; Q: Error?
	je	PF_ODSF_Exit			;    Y: Invalid drive

	movzx	ebx, [ebp.Client_BX]
	movzx	ecx, [ebp.Client_CX]
	imul	ebx, eax
	imul	ebx, ecx			; EBX = Total free space

	mov	eax, PF_Default_Reserve 	; Default value
	xor	esi, esi			; Use Win386 section
	mov	edi, OFFSET32 PF_Min_Free_Ini	; Look for this string
	VMMcall Get_Profile_Decimal_Int 	; EAX = # K to reserve on disk
	shl	eax, 10 			; EAX = # bytes to reserve

	sub	ebx, eax			; Q: Is there ANY free space?
	jbe	PF_ODSF_Exit			;    N: Nothing to do
						;    Y: Allocate data struc mem
	mov	eax, 10000h			; Never bigger than 64 meg
	xor	esi, esi			; Nil means use [Win386] section
	mov	edi, OFFSET32 PF_Max_Size_Ini	; EDI -> "MaxPagingFileSize"
	VMMcall Get_Profile_Decimal_Int 	; EAX = Maximum file size

	shr	ebx, 10 			; EBX = Max size in K
	cmp	ebx, eax			; Q: More than desired max
	jb	SHORT PF_ODSF_Have_Max_Size	;    N: Use this value
	mov	ebx, eax			;    Y: DOS max file = User's
PF_ODSF_Have_Max_Size:
	shr	ebx, 2				; EBX = Ideal file size in pages
	cmp	ebx, PF_Min_File_Pages		; Q: Enough pages available?
	jb	PF_ODSF_Exit			;    N: Don't page




	.ERRNZ DemandInfoStruc MOD 4
	sub	esp, SIZE DemandInfoStruc
	mov	edi, esp
	VMMcall _GetDemandPageInfo, <edi, 0>
	mov	edi, [edi.DILin_Total_Count]
	add	esp, SIZE DemandInfoStruc

	cmp	ebx, edi
	jbe	SHORT PF_ODSF_Fixed_Max_Size
	mov	ebx, edi

PF_ODSF_Fixed_Max_Size:
	mov	[PF_Max_File_Pages], ebx


;
;   Get the Win386 PSP segment so we can switch back to it.
;
	VMMcall Get_PSP_Segment
	mov	[PF_Our_PSP], ax

	call	PageFile_Set_Our_PSP

;
;   Copy the file name into V86 address space.
;
	mov	edi, [esp.Pushad_EAX]		; EDI -> Temp data area
	mov	eax, edi			; EAX -> Temp data area
	mov	esi, OFFSET32 PF_Our_File_Name
	mov	ecx, 128/4
	cld
	Begin_Touch_1st_Meg
	rep movsd
	End_Touch_1st_Meg

  ;
  ;   Open the file by creating it
  ;
	  mov	  [ebp.Client_AH], 3Ch
	  mov	  [ebp.Client_CX], 0
	  shl	  eax, 12			  ; Cvt temp data ptr to SEG:OFF
	  shr	  ax, 12
	  mov	  [ebp.Client_DX], ax
	  shr	  eax, 16
	  mov	  [ebp.Client_DS], ax
	  call	  PageFile_Call_DOS
	  jc	  PF_ODSF_Cant_Open_File
  ;
  ; Tell loader that we want this file deleted when we exit.
  ;
	  mov	  esi, OFFSET32 PF_Our_File_Name

	  VMMCall Set_Delete_On_Exit_File
IFDEF DEBUG
	  jnc	  short PFD10
	  debug_out "Could not set DELETE ON EXIT for temporary swap file"
PFD10:
ENDIF
	  mov	  eax, [ebp.Client_EAX]
  ;
  ;   Strange hack for strange DOS bug.  DO NOT REMOVE THIS CODE!
  ;
  ;   This code writes one byte to the swap file and then flushes it to disk
  ;   by closing the file handle.  For some reason, DOS will sometimes write
  ;   over the first FAT entry if you grow a file from zero length to an
  ;   enormous size.  It uses FAT entry zero as a temporary pointer to a fake
  ;   cluster and it can sometimes be written to disk, causing CHKDSK to
  ;   report "Probable non-DOS disk".
  ;
	  mov	  [ebp.Client_CX], 1
	  mov	  [ebp.Client_AH], 40h
	  mov	  [ebp.Client_EBX], eax
	  call	  PageFile_Call_DOS
	  jc	  PF_ODSF_Cant_Open_File
  ;
  ;   Close the handle to flush the one byte file.
  ;
	  mov	  [ebp.Client_EBX], eax
	  mov	  [ebp.Client_AH], 3Eh
	  call	  PageFile_Call_DOS
	  jc	  PF_ODSF_Cant_Open_File
  ;
  ;   Now re-open the file in "don't inherit" mode so that the open handle
  ;	is not passed across EXECs.
  ;
	  mov	  [ebp.Client_AH], 3Dh
	  mov	  [ebp.Client_AL], 82h ; Read/write, not inherited
	  call	  PageFile_Call_DOS
	  jc	  PF_ODSF_Cant_Open_File
	  mov	  eax, [ebp.Client_EAX]
	  mov	  [PF_File_Handle], ax

;
;   There!  It's flushed and DOS won't do strange things with the FAT anymore.
;   Grow the file by the minimum allowable amount (or to max size)
;
	mov	eax, [PF_Max_File_Pages]
	cmp	eax, PF_Min_File_Grow
	jbe	SHORT PF_ODSF_Grow_File
	mov	eax, PF_Min_File_Grow
PF_ODSF_Grow_File:
	mov	[PF_Cur_File_Pages], eax
	shl	eax, 12 			; Convert pages to bytes
	mov	[ebp.Client_DX], ax
	shr	eax, 16
	mov	[ebp.Client_CX], ax
	mov	ax, [PF_File_Handle]
	mov	[ebp.Client_BX],ax		; Put the file handle back in BX
	mov	[ebp.Client_AX], 4200h
	call	PageFile_Call_DOS
	jc	DEBFAR PF_ODSF_Cant_Grow

	mov	[ebp.Client_AH], 40h
	mov	[ebp.Client_CX], 0
	call	PageFile_Call_DOS
	jc	DEBFAR PF_ODSF_Cant_Grow

;
;   Seek to the end of the file to make sure the file size is correct.
;
	mov	[ebp.Client_AX], 4202h
	mov	[ebp.Client_CX], 0
	mov	[ebp.Client_DX], 0
	call	PageFile_Call_DOS
	jc	SHORT PF_ODSF_Cant_Grow
	mov	eax, [ebp.Client_EDX]
	shl	eax, 16
	mov	ax, [ebp.Client_AX]
	TestReg eax, 00FFFh
	jnz	SHORT PF_ODSF_Cant_Grow
	shr	eax, 12
	cmp	eax, [PF_Cur_File_Pages]
	jne	SHORT PF_ODSF_Cant_Grow

;
;   Flush the file to disk by duplicating the file handle and closing the
;   duplicate.	If either of these calls fails the error is ignored since
;   the file has been grown successfully (we just couldn't flush it).
;
	; Client_BX already contains handle
	mov	[ebp.Client_AH], 45h
	call	PageFile_Call_DOS
IFDEF DEBUG
	jnc	SHORT PF_ODSF_Dup_Worked
	Debug_Out "WARNING:  Unable to duplicate file handle for paging file"
PF_ODSF_Dup_Worked:
ENDIF
	jc	SHORT PF_ODSF_Restore_PSP

;
;   Close the duplicate handle
;
	mov	eax, [ebp.Client_EAX]
	mov	[ebp.Client_EBX], eax
	mov	[ebp.Client_AH], 3Eh
	call	PageFile_Call_DOS
IFDEF DEBUG
	jnc	SHORT PF_ODSF_Flush_Worked
	Debug_Out "WARNING:  Unable to close duplicate file handle to flush paging file"
PF_ODSF_Flush_Worked:
ENDIF

PF_ODSF_Restore_PSP:
	call	PageFile_Set_Client_PSP

PF_ODSF_Exit:
	popad
	ret

;
;   Unable to grow the file.  Close file handle and destroy the file.
;
PF_ODSF_Cant_Grow:
	mov	ax, [PF_File_Handle]
	mov	[ebp.Client_BX], ax
	mov	[ebp.Client_AH], 3Eh
	call	PageFile_Call_DOS
	mov	edi, [esp.Pushad_EAX]
	call	PageFile_Nuke_Swap_File

;
;   Set Max_File_Pages to 0 to indicate that paging is disabled.
;
PF_ODSF_Cant_Open_File:
	mov	[PF_Max_File_Pages], 0
	jmp	PF_ODSF_Restore_PSP

EndProc PageFile_Open_DOS_Swap_File


;******************************************************************************
;
;   PageFile_Nuke_Swap_File
;
;   DESCRIPTION:
;
;   ENTRY:
;	Caller must have called Begin_Nest_V86_Exec
;	EDI -> Linear V86 address to use for DOS calls
;	EBP -> Client register structure
;
;   EXIT:
;	If carry set then could not delete file
;
;   USES:
;	All CLIENT registers and Flags (caller's registers preserved)
;
;==============================================================================

BeginProc PageFile_Nuke_Swap_File

	pushad

	mov	eax, edi			; Save this for later
;
;   Copy our file name into the V86 temp data area.
;
	mov	esi, OFFSET32 PF_Our_File_Name
	mov	ecx, 128/4
	cld
	Begin_Touch_1st_Meg
	rep movsd
	End_Touch_1st_Meg

;
;   Change the attributes so we can delete it.
;
	mov	[ebp.Client_AX], 4301h		; Set file attributes
	shl	eax, 12
	shr	ax, 12
	mov	[ebp.Client_DX], ax
	shr	eax, 16
	mov	[ebp.Client_DS], ax
	mov	[ebp.Client_CX], 0
	call	PageFile_Call_DOS
	jc	SHORT PF_NSF_Exit

;
;   Now just delete it.  Client's DS:DX already point to the file name.
;
	mov	[ebp.Client_AH], 41h
	call	PageFile_Call_DOS

PF_NSF_Exit:
	popad
	ret

EndProc PageFile_Nuke_Swap_File


VxD_ICODE_ENDS

	PAGE
;******************************************************************************
;	      P R O T E C T E D   M O D E   R E S I D E N T   C O D E
;******************************************************************************

VxD_LOCKED_CODE_SEG

;******************************************************************************
;
;   PageFile_Control
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

BeginProc PageFile_Control

	Control_Dispatch Sys_Critical_Init, PageFile_Sys_Critical_Init
	Control_Dispatch Init_Complete,     PageFile_Init_Complete

IFDEF DEBUG
	Control_Dispatch Debug_Query, PageFile_Debug_Query
ENDIF

	clc
	ret

EndProc PageFile_Control


BeginDoc
;******************************************************************************
;
;   PageFile_Clean_Up
;
;   DESCRIPTION:
;	This service will "clean-up" the paging file.  If a DOS swap file is
;	being used, the file will be deleted.  This code is normally called
;	by the PageSwap device at System Exit time, but may be called by
;	internal initialization code if something goes wrong.
;
;	NOTE that we really don't need this anymore because we have set the
;	temp swap file as a Set_Delete_On_Exit_File.
;
;   ENTRY:
;	None.
;
;   EXIT:
;	None.
;
;   USES:
;	Flags.
;
;==============================================================================
EndDoc

BeginProc PageFile_Clean_Up, Service

;;;;	    pushad
;;;;
;;;;	    cmp     [PF_Max_File_Pages], 0	    ; Q: Is paging enabled?
;;;;	    je	    SHORT PF_SE_Exit		    ;	 N: Quit right away
;;;;						    ;	 Y: Map back the right mem
;;;;	    cmp     [PF_Have_Partition], True	    ; Q: Using a disk partition?
;;;;	    je	    SHORT PF_SE_Exit		    ;	 Y: GOOD!  Do nothing else!
;;;;
;;;;	    Push_Client_State
;;;;	    VMMcall Begin_Nest_V86_Exec
;;;;
;;;;	    call    PageFile_Set_Our_PSP
;;;;
;;;;	    mov     ax, [PF_File_Handle]
;;;;	    mov     [ebp.Client_BX], ax
;;;;	    mov     [ebp.Client_AH], 3Eh
;;;;	    call    PageFile_Call_DOS
;;;;
;;;;	    mov     ax, 4301h
;;;;	    mov     edx, OFFSET32 PF_Our_File_Name
;;;;	    xor     cx, cx
;;;;	    VxDint  21h
;;;;
;;;;	    mov     ah, 41h
;;;;	    VxDint  21h
;;;;
;;;;	    call    PageFile_Set_Client_PSP
;;;;
;;;;	    VMMcall End_Nest_Exec
;;;;	    Pop_Client_State
;;;;
;;;;PF_SE_Exit:
;;;;	    popad
	clc
	ret

EndProc PageFile_Clean_Up


;******************************************************************************
;			     S E R V I C E S
;******************************************************************************


BeginDoc
;******************************************************************************
;
;   PageFile_Get_Version
;
;   DESCRIPTION:
;
;   ENTRY:
;	No entry parameters
;
;   EXIT:
;	EAX = Version number (0 if not installed)
;	 BL  = Pager Type (1 = No pager, 2 = DOS pager, 3 = Direct hardware pg)
;	Carry flag clear if page-file device installed
;
;	Note: The pager type in BL is not valid until PageFile_Init_File has
;	      been called!
;
;   USES:
;
;==============================================================================
EndDoc

BeginProc PageFile_Get_Version, Service

	.ERRNZ	PF_Paging_None-1
	.ERRNZ	PF_Paging_DOS-2
	.ERRNZ	PF_Paging_HW-3

	mov	bl, 1				; Assume no pager (type 1)

	cmp	[PF_Max_File_Pages], 0		; if # pages in file = 0
	jz	SHORT PF_GV_Exit		;   then paging is off
	inc	bl				; Assume DOS pager (type 2)

	cmp	[PF_Using_BlockDev], True	; Q: Async hard disk?
	jnz	SHORT PF_GV_Exit		;    N: Done
	inc	bl				;    Y: Smart pager (type 3)

Internal_Get_Version LABEL NEAR

PF_GV_Exit:
	%OUT Change version # to 3.1 soon

	mov	eax, 10Ah			; Version 1.10
	clc
	ret

EndProc PageFile_Get_Version



BeginDoc
;******************************************************************************
;
;   PageFile_Test_IO_Valid
;
;   DESCRIPTION:
;	Determines if paging to/from disk can be performed now.
;
;   ENTRY:
;	None.
;
;   EXIT:
;	If carry flag set then
;	    Do not do anything that may cause paging
;	else
;	    OK to cause paging now
;
;   USES:
;
;==============================================================================
EndDoc

BeginProc PageFile_Test_IO_Valid, Service

	cmp	[PF_Using_BlockDev], True	; Okay if using BlockDev...
	jz	SHORT PF_TIV_Paging_OK

	VxDcall DOSMGR_Get_DOS_Crit_Status	; Otherwise check critical
	jne	SHORT PF_TIV_Dont_Page		;   section status

	push	eax
	VxDcall DOSMGR_Get_IndosPtr
	cmp	WORD PTR ds:[eax], 0
	pop	eax
	jne	SHORT PF_TIV_Dont_Page

PF_TIV_Paging_OK:
	clc
	ret

PF_TIV_Dont_Page:
	stc
	ret

EndProc PageFile_Test_IO_Valid


BeginDoc
;******************************************************************************
;
;   PageFile_Grow_File
;
;   DESCRIPTION:
;	Attempts to grow the size of the paging file.
;
;	Note: This service may not be able to the file by the desired size
;	if the disk is full), and in fact may not have grown it at all.
;
;   ENTRY:
;	The critical section must be owned by the caller!
;	ECX = Number of pages to grow paging file by
;
;   EXIT:
;	EAX = Current (new) # file pages
;	EBX = Maximum # file pages
;
;	Note: This service may return a different max file size than the one
;	      returned by PageFile_Init_File.  This will happen if an error
;	      occurs while trying to grow the file (disk full for example).
;
;   USES: Flags
;
;==============================================================================
EndDoc

BeginProc PageFile_Grow_File, Service

	Assert_Ints_Enabled

IFDEF DEBUG
	cmp	ecx, 10000h
	jb	SHORT PF_GF_Reasonable
	Debug_Out "PageFile_Grow_File by #ECX pages???"
PF_GF_Reasonable:
ENDIF

;  Make a quick exit if the file is already at the maximum size.

	mov	eax, [PF_Cur_File_Pages]
	cmp	eax, [PF_Max_File_Pages]
	jae	PF_GF_Quick_Exit

;  Attempt to grow the file

	pushad

	mov	edi, PF_Min_File_Grow
	cmp	edi, ecx
	jae	SHORT PF_GF_Enter_Crit
	mov	edi, ecx			; EDI = # pages to really grow

PF_GF_Enter_Crit:

	Push_Client_State
	VMMcall Begin_Nest_V86_Exec
	inc	[PF_DOS_IO_Count]


	VMMcall Get_Cur_VM_Handle
	mov	ebp, [ebx.CB_Client_Pointer]

	mov	eax, [PF_V86_Stack_Seg_Off]
	mov	[ebp.Client_SP], ax
	shr	eax, 16
	mov	[ebp.Client_SS], ax

;
;   Set PSP to Win386.Exe PSP to make DOS calls
;
	call	PageFile_Set_Our_PSP

;
;   Set the file handle in BX to the paging file
;
	mov	ax, [PF_File_Handle]
	mov	[ebp.Client_BX], ax

;
;   Position the file pointer to the proper position and truncate file to
;   grow it to the new size.
;
	add	edi, [PF_Cur_File_Pages]
	cmp	edi, [PF_Max_File_Pages]
	jbe	SHORT PF_GF_Grow_Now
	mov	edi, [PF_Max_File_Pages]

;
;   Seek to new desired size and do a 0 byte write to grow the file.  Note that
;   the DOS call will NOT fail if the disk is full.  A later test will shrink
;   the max file size if the file is not grown to the desired size.
;
PF_GF_Grow_Now:
	mov	eax, edi
	shl	eax, 12 			; Convert pages to bytes
	mov	[ebp.Client_DX], ax
	shr	eax, 16
	mov	[ebp.Client_CX], ax
	mov	[ebp.Client_AX], 4200h
	call	PageFile_Call_DOS
	jc	PF_GF_Cant_Grow

	mov	[ebp.Client_AH], 40h
	mov	[ebp.Client_CX], 0
	call	PageFile_Call_DOS
	jc	PF_GF_Cant_Grow


;
;   Duplicate our file handle and close the duplicate to flush the FAT entries
;   to disk.  Note that we ignore errors here since not being able to do this
;   is not fatal (we may just be out of handles).
;
	; Client_BX already contains handle
	mov	[ebp.Client_AH], 45h
	call	PageFile_Call_DOS
IFDEF DEBUG
	jnc	SHORT PF_GF_Dup_Worked
	Debug_Out "WARNING:  PageFile unable to duplicate file handle -- Can't flush file"
PF_GF_Dup_Worked:
ENDIF
	jc	SHORT PF_GF_Test_Size

	mov	eax, [ebp.Client_EAX]
	mov	[ebp.Client_EBX], eax
	mov	[ebp.Client_AH], 3Eh
	call	PageFile_Call_DOS
IFDEF DEBUG
	jnc	SHORT PF_GF_Test_Size
	Debug_Out "WARNING:  PageFile unable to close duplicate file handle"
ENDIF

;
;   Seek to the end of the file to make sure the file size is correct.	If we
;   were unable to extend the file (disk is full) then set a new Max_File_-
;   Pages value so we won't thrash.
;
PF_GF_Test_Size:
	mov	ax, [PF_File_Handle]		; Must restore file handle in
	mov	[ebp.Client_BX], ax		; client's BX

	mov	[ebp.Client_AX], 4202h
	mov	[ebp.Client_CX], 0
	mov	[ebp.Client_DX], 0
	call	PageFile_Call_DOS		; Seek to end of file
	jc	SHORT PF_GF_Cant_Grow		; If error then just give up

	mov	eax, [ebp.Client_EDX]
	shl	eax, 16
	mov	ax, [ebp.Client_AX]
	shr	eax, 12 			; EAX = File size in pages

	cmp	eax, edi			; Q: Desired size?
	je	SHORT GF_Grew_Successfully	;    Y: Good!
	mov	[PF_Max_File_Pages], eax	;    N: Set new maximum to
						;	prevent growing any more
	Trace_Out "WARNING:  Out of disk space.  Unable to grow paging file."


;   Add the new pages to the free list.

GF_Grew_Successfully:
	mov	[PF_Cur_File_Pages],eax 	; New size of file


;   Done!  Restore original PSP and return.

GF_Exit:
	call	PageFile_Set_Client_PSP

	dec	[PF_DOS_IO_Count]
	VMMcall End_Nest_Exec
	Pop_Client_State

	clc
	popad

PF_GF_Quick_Exit:
	mov	eax,[PF_Cur_File_Pages] 	; Return current (new) size
	mov	ebx,[PF_Max_File_Pages] 	;   and maybe diff max size
	ret

;   DOS returned an error from some call.  Set max=current size so we won't
;   try to grow the file again.

PF_GF_Cant_Grow:
	Debug_Out "ERROR:  DOS Error while attempting to grow paging file"
	mov	eax, [PF_Cur_File_Pages]
	mov	[PF_Max_File_Pages], eax
	jmp	GF_Exit

EndProc PageFile_Grow_File


BeginDoc
;******************************************************************************
;
;   PageFile_Read_Or_Write
;
;   DESCRIPTION:
;	This service will read or write a page from/to the paging file.
;
;	If DOS or Int 13h is being used for paging, the call back will always
;	be called before this routine return, regardless of whether the I/O
;	is successful or not.  If BlockDev is being used, the call back may
;	or may not have been called when this routine returns--it depends on
;	when BlockDev calls us back, and whether or not we detect an error
;	condition before calling BlockDev.
;
;   ENTRY:
;	Current VM is in critical section
;	EBX = pointer to first PageSwapBufferDesc node
;
;   EXIT:
;	Interrupts are ENABLED
;
;   USES:
;	Flags
;
;==============================================================================
EndDoc

BeginProc PageFile_Read_Or_Write, Async_Service

IFDEF DEBUG
; ensure we are in critical section
	pushad
	VMMcall Get_Crit_Section_Status
	or	ecx, ecx
	jz	SHORT PF_ROW_Not_Crit_Sect
	VMMcall Test_Cur_VM_Handle
	je	SHORT PF_ROW_Done_Test

PF_ROW_Not_Crit_Sect:
	Debug_Out "PageFile_Read_Or_Write: VM not in critical section!"

PF_ROW_Done_Test:
	popad
ENDIF

	pushad

	cmp	[PF_Using_BlockDev], True	; Using BlockDev for I/O?
	jnz	PF_ROW_Stupid_Pager

;--------------------------------------------------------------------------
;   Direct to hardware paging!
;--------------------------------------------------------------------------

	cli				    ; Make sure list not updated by
					    ; command completion

; check that first command's page # falls in correct range
PF_ROW_Check_Range:
	mov	edi, [ebx.PS_BD_File_Page]
	cmp	edi, [PF_Cur_File_Pages]
	jb	SHORT PF_ROW_Do_IO

	mov	ax, PFS_Failure
	call	[ebx.PS_BD_Call_Back]	    ; Inform caller of completion

	mov	ebx, [ebx.PS_BD_Next]	    ; next node in list
	or	ebx, ebx		    ; Q: null pointer?
	jz	PF_ROW_Exit		    ;	Y: no valid command in list

	jmp	PF_ROW_Check_Range


;   Build the BlockDev command block for this operation

PF_ROW_Do_IO:

; ebx -> first valid command in list

	lea	esi, [ebx.PS_BD_Reserved]	; pointer to BlockDev struct
	push	esi				; save it for later

	.ERRNZ	PF_Read_Data-BDC_Read
	.ERRNZ	PF_Write_Data-BDC_Write

PF_ROW_Build_Loop:

	movzx	ax, [ebx.PS_BD_Cmd]
	mov	[esi.BD_CB_Command], ax

	movzx	eax, [ebx.PS_BD_Priority]
	mov	[esi.BD_CB_Flags], eax

	.ERRNZ	PF_High_Priority-BDCF_High_Priority

	lea	eax, [edi*8]
	add	eax, [PF_Int13_Base_Sector]	; EAX = Starting sector
	mov	DWORD PTR [esi.BD_CB_Sector], eax
	mov	DWORD PTR [esi.BD_CB_Sector][4], 0

	mov	[esi.BD_CB_Count], 8	    ; Read/Write 8, 512 byte sectors
	mov	eax, [ebx.PS_BD_Buffer_Ptr]
	mov	[esi.BD_CB_Buffer_Ptr], eax ; Read/write to/from here

	mov	[esi.BD_CB_Cmd_Cplt_Proc], OFFSET32 PageFile_Call_Back

PF_ROW_Next_Command:
	mov	ebx, [ebx.PS_BD_Next]	    ; next node in list
	or	ebx, ebx		    ; Q: null pointer?
	jz	SHORT PF_ROW_Issue	    ;	Y: done building list

	mov	edi, [ebx.PS_BD_File_Page]
	cmp	edi, [PF_Cur_File_Pages]    ; Q: this entry OK?
	jae	SHORT PF_ROW_Bad_Page	    ;	N: don't add it, don't point to it

	lea	eax, [ebx.PS_BD_Reserved]   ; pointer to next BlockDev struc
	mov	[esi.BD_CB_Next], eax	    ; chain the BD_CB's together
	mov	esi, eax
	jmp	PF_ROW_Build_Loop

PF_ROW_Bad_Page:
	mov	ax, PFS_Failure
	call	[ebx.PS_BD_Call_Back]	    ; Inform caller of completion

	jmp	PF_ROW_Next_Command

;   Issue the command.

PF_ROW_Issue:
	mov	[esi.BD_CB_Next], ebx	    ; null-pointer to terminate list

	pop	esi			    ; restore pointer to 1st command
	mov	edi, [PF_BD_Descriptor]

	sti				    ; Interrupts back on
	VxDcall BlockDev_Send_Command

	jmp	PF_ROW_Exit

;--------------------------------------------------------------------------
;   Need to use either DOS or Int 13h for paging...
;--------------------------------------------------------------------------

PF_ROW_Stupid_Pager:

	sti				    ; Make sure ints are enabled

	mov	edi, [ebx.PS_BD_File_Page]
	cmp	edi, [PF_Cur_File_Pages]
	jae	PF_ROW_Invalid_Page

;   Get parameters from structure
	mov	esi, [ebx.PS_BD_Buffer_Ptr]

;   Map the appropriate page into V86 address space

	push	ebx
	VMMcall Get_Cur_VM_Handle
	shr	esi, 12
	push	ecx
	VMMcall _LinMapIntoV86, <esi, ebx, [PF_Lin_Page_Num], 1, 0>
	pop	ecx
	pop	ebx
	test	eax, eax

IFDEF DEBUG
	jnz	SHORT PF_ROW_Mapped_OK
	Debug_Out "PageFile ERROR:  Unable to map page"
PF_ROW_Mapped_OK:
ENDIF
	jz	PF_ROW_Invalid_Page

	mov	ch,  [ebx.PS_BD_Cmd]
	mov	eax, [PF_Cache_Lock_Ptr]	; Lock the smartdrv cache
	test	eax, eax			;   if necessary
	jz	SHORT PF_ROW_Nest_Exec
	Begin_Touch_1st_Meg
	inc	BYTE PTR [eax]
	End_Touch_1st_Meg

PF_ROW_Nest_Exec:
	Push_Client_State
	VMMcall Begin_Nest_V86_Exec


	push	ebx
	VMMcall Get_Cur_VM_Handle
	mov	ebp, [ebx.CB_Client_Pointer]
	pop	ebx

	mov	eax, [PF_V86_Stack_Seg_Off]
	mov	[ebp.Client_SP], ax
	shr	eax, 16
	mov	[ebp.Client_SS], ax

	cmp	[PF_Have_Partition], True	; Int 13h or DOS?
	jne	DEBFAR PF_ROW_Use_DOS


;------------------------------------------------------------------------------
;   Read/Write using Int 13h
;------------------------------------------------------------------------------

	add	ch, 2
	mov	[ebp.Client_AH], ch
	mov	[ebp.Client_AL], 1000h / PF_Sector_Size ; 8 Secs = 4K read/write

	lea	eax, [edi*8]			; EAX = Offset in file
	add	eax, [PF_Int13_Base_Sector]	; EAX = Off from start of disk
	xor	edx, edx
	idiv	[PF_Int13_Sec_Per_Track]
	mov	ecx, edx			; Remainder = Starting sector
	xor	edx, edx
	idiv	[PF_Int13_Num_Heads]
	;   ECX = Sector # (0 BASED!)
	;   EDX = Starting head
	;   EAX = Cylinder number
	inc	cl				; Make sector 1 based
	mov	ch, al				; CH = Cylinder number
	shl	ah, 6				; Move bits 6 and 7 up
	or	cl, ah				; Set high bits of cylinder
	mov	[ebp.Client_CX], cx		; Set cylinder and sector number
	mov	[ebp.Client_DH], dl
	mov	al, [PF_Int13_Drive_Num]
	mov	[ebp.Client_DL], al

	mov	eax, [PF_Lin_Page_Num]
	shl	eax, 8				; * 4K / 10h
	mov	[ebp.Client_ES], ax
	mov	[ebp.Client_BX], 0

	mov	edx, [ebp.Client_EAX]		; Save for possible retry
	mov	ecx, 3				; Retry up to 3 times

PF_ROW_Retry_Loop:
;
;   NOTE:  Some BIOS's have bugs in their Int 13h handler so that if they
;	   are called with the direction flag set, THEY WILL BLOW UP!  So
;	   we always clear the direction flag before calling any software
;	   int in the pagefile VxD.
;
	mov	eax, 13h
	ClrFlag [ebp.Client_EFlags], DF_Mask	; WORK AROUND BUSTED SOFTWARE
	VMMcall Exec_Int

	TestMem [ebp.Client_EFlags], CF_Mask
	jz	DEBFAR PF_ROW_End_Exec

	Trace_Out "WARNING:  PageFile failed to read/write page through Int 13h.  Retry."

	mov	[ebp.Client_EAX], edx		; Restore original AH/AL
	loopd	PF_ROW_Retry_Loop		; Try up to 3 times

	Debug_Out "PageFile read/write via Int 13h failed!"

	jmp	PF_ROW_Int13_Error

;------------------------------------------------------------------------------
;   Read/Write to DOS file
;------------------------------------------------------------------------------

PF_ROW_Use_DOS:

	inc	[PF_DOS_IO_Count]

;   Set PSP to Win386.Exe PSP to make DOS calls

	call	PageFile_Set_Our_PSP


;   Move file pointer to appropriate position

	mov	[ebp.Client_AX], 4200h
	mov	ax, [PF_File_Handle]
	mov	[ebp.Client_BX], ax
	shl	edi, 12
	mov	[ebp.Client_DX], di
	shr	edi, 16
	mov	[ebp.Client_CX], di
	call	PageFile_Call_DOS
	jc	SHORT PF_ROW_DOS_Error


;   Set up for read or write.

	add	ch, 3Fh
	mov	[ebp.Client_AH], ch
	mov	[ebp.Client_CX], 1000h
	mov	eax, [PF_Lin_Page_Num]
	shl	eax, 8
	mov	[ebp.Client_DS], ax
	mov	[ebp.Client_DX], 0
	call	PageFile_Call_DOS
	jc	SHORT PF_ROW_DOS_Error
	cmp	[ebp.Client_AX], 1000h
	jne	SHORT PF_ROW_DOS_Error


;   Restore original PSP

PF_ROW_DOS_Exit:

	call	PageFile_Set_Client_PSP

	dec	[PF_DOS_IO_Count]

;   Common DOS/Int 13h exit -----------------------------------------------

PF_ROW_End_Exec:

;   Map null memory over the page

	call	PageFile_Map_Null

	VMMcall End_Nest_Exec
	Pop_Client_State

	mov	eax, [PF_Cache_Lock_Ptr]	; Release the smartdrv cache
	test	eax, eax			;   lock if used
	jz	SHORT PF_ROW_Success_Exit
	Begin_Touch_1st_Meg
	dec	BYTE PTR [eax]
	End_Touch_1st_Meg

PF_ROW_Success_Exit:
	mov	ax, PFS_Success
	cli					; ints. off for callback
	call	[ebx.PS_BD_Call_Back]		; Inform caller of completion

;   Common BlockDev/DOS/Int 13h exit --------------------------------------
	jmp	SHORT PF_ROW_Exit

;--------------------------------------------------------------------------
;	Error exits -- clean up and return with carry set
;--------------------------------------------------------------------------

PF_ROW_DOS_Error:
	Debug_Out "PageFile ERROR READING OR WRITING PAGES!  Type .VR to see status"

	call	PageFile_Set_Client_PSP
	dec	[PF_DOS_IO_Count]

PF_ROW_Int13_Error:

	call	PageFile_Map_Null

	VMMcall End_Nest_Exec
	Pop_Client_State

	mov	eax, [PF_Cache_Lock_Ptr]	; Release the smartdrv cache
	test	eax, eax			;   lock if used
	jz	SHORT PF_ROW_Invalid_Page
	Begin_Touch_1st_Meg
	dec	BYTE PTR [eax]
	End_Touch_1st_Meg

PF_ROW_Invalid_Page:

	mov	ax, PFS_Failure
	cli					; ints off for callback
	call	[ebx.PS_BD_Call_Back]		; Inform caller of completion

PF_ROW_Exit:
	popad
	ret

EndProc PageFile_Read_Or_Write


BeginDoc
;******************************************************************************
;
;   PageFile_Cancel
;
;   DESCRIPTION:
;	This service will cancel the specified read or write operation.  I/O
;	can only be canceled when a direct hardware paging is in effect
;	(pager type PF_Paging_HW).
;
;   ENTRY:
;	EBX -> PageSwapBufferDesc describing command to cancel
;
;   EXIT:
;
;   USES:
;
;==============================================================================
EndDoc

BeginProc PageFile_Cancel, Service

	push	ebx
	push	esi
	push	edi

IFDEF DEBUG
; Sanity check: make sure this is not called when using a stupid pager

	cmp	[PF_Using_BlockDev], True
	je	SHORT PF_C_Debug_OK

	Debug_Out "Pagefile: Cancel called, not using BlockDev for paging"
PF_C_Debug_OK:
ENDIF

; Build the BlockDev command block to cancel current I/O and send it off...

	mov	esi, OFFSET32 PF_BD_Cancel
	mov	[esi.BD_CB_Next], 0
	mov	[esi.BD_CB_Command], BDC_Cancel
	lea	ebx, [ebx.PS_BD_Reserved]
	mov	[esi.BD_CB_Buffer_Ptr], ebx
	mov	[esi.BD_CB_Cmd_Cplt_Proc], OFFSET32 PageFile_Cancel_Call_Back

	mov	edi, [PF_BD_Descriptor]
	VxDcall BlockDev_Send_Command

	pop	edi
	pop	esi
	pop	ebx

	ret

EndProc   PageFile_Cancel


	PAGE
;******************************************************************************
;		     L O C A L	 P R O C E D U R E S
;******************************************************************************


;******************************************************************************
;
;   PageFile_Int_24h
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

BeginProc PageFile_Int_24h

	Assert_Ints_Enabled

	cmp	 [PF_DOS_IO_Count], 0
	jne	 SHORT PF_I24_Fail
	stc
	ret

PF_I24_Fail:
	Debug_Out "ERROR:  Int 24h while paging"
	mov	[ebp.Client_AL], 3
	clc
	ret

EndProc PageFile_Int_24h


;******************************************************************************
;
;   PageFile_Int_23h
;
;   DESCRIPTION:
;	do the same with int23h (break) as with int24h
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc PageFile_Int_23h

	Assert_Ints_Enabled

	cmp	 [PF_DOS_IO_Count], 0
	jne	 SHORT PF_I23_Fail
	stc
	ret

PF_I23_Fail:
	Debug_Out "WARNING:  Int 23h while paging, ignored"
	clc
	ret

EndProc PageFile_Int_23h


;******************************************************************************
;
;   PageFile_Call_Back
;
;   DESCRIPTION:
;	CallBack function for all blockdev commands.
;	BlockDev calls us, we call the appropriate PageSwap Call Back.
;	Must first map success/failure codes to PageSwap success/failure.
;
;   ENTRY:
;	EDI -> Block Device Descriptor
;	ESI -> Command Block
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc PageFile_Call_Back

	Assert_Ints_Disabled

	push	eax

	.ERRNZ	BDS_Canceled-PFS_Canceled

	mov	ax, [esi.BD_CB_Cmd_Status]	; Map BlockDev return status
	cmp	ax, BDS_First_Error_Code	;   to PageFile status
	jb	SHORT PF_RWCB_Success

	cmp	ax, PFS_Canceled
	jz	SHORT PF_RWCB_Call_Up

	mov	ax, PFS_Failure
	jmp	SHORT PF_RWCB_Call_Up

PF_RWCB_Success:
	mov	ax, PFS_Success

PF_RWCB_Call_Up:
	push	ebx
	lea	ebx, [esi - PS_BD_Reserved]	; get ptr to PageSwap structure
	call	[ebx.PS_BD_Call_Back]		; Inform caller of completion

	pop	ebx
	pop	eax

	ret

EndProc PageFile_Call_Back


;******************************************************************************
;
;   PageFile_Cancel_Call_Back
;
;   DESCRIPTION:
;	CallBack function for cancel commands.
;
;   ENTRY:
;	EDI -> Block Device Descriptor
;	ESI -> Command Block
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc PageFile_Cancel_Call_Back

	Assert_Ints_Disabled
	ret

EndProc PageFile_Cancel_Call_Back


;******************************************************************************
;
;   PageFile_Map_Null
;
;   DESCRIPTION:
;
;   ENTRY:
;	EBX = Cur_VM_Handle
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc PageFile_Map_Null

	Assert_Ints_Enabled

	pushad

	VMMcall _GetNulPageHandle
	VMMcall Get_Cur_VM_Handle
	VMMcall _MapIntoV86, <eax, ebx, [PF_Lin_Page_Num], 1, 0, PageDEBUGNulFault>

	popad

	ret

EndProc PageFile_Map_Null


;******************************************************************************
;
;   PageFile_Set_Our_PSP
;
;   DESCRIPTION:
;
;   ENTRY:
;	None
;
;   EXIT:
;
;   USES:
;	EAX, Flags, All client registers and flags
;
;==============================================================================

BeginProc PageFile_Set_Our_PSP

	Assert_Ints_Enabled

;
;   Get current PSP
;
	mov	[ebp.Client_AH], 51h
	call	PageFile_Call_DOS
	mov	eax, [ebp.Client_EBX]
	mov	[PF_Client_PSP], ax

;
;   Set PSP to Win386.Exe PSP to make DOS calls
;
	movzx	eax, [PF_Our_PSP]

	mov	[ebp.Client_BX], ax

	shl	eax, 4
	push	[eax.PDB_User_stack]
	pop	[PF_Save_User_Stack]

	mov	[ebp.Client_AH], 50h
	call	PageFile_Call_DOS

	ret

EndProc PageFile_Set_Our_PSP


;******************************************************************************
;
;   PageFile_Set_Client_PSP
;
;   DESCRIPTION:
;
;   ENTRY:
;	None (PageFile_Set_Our_PSP must have been called previously)
;
;   EXIT:
;	None
;
;   USES:
;	EAX, Client registers, Flags
;
;==============================================================================

BeginProc PageFile_Set_Client_PSP

	Assert_Ints_Enabled

	mov	ax, [PF_Client_PSP]
	mov	[ebp.Client_BX], ax
	mov	[ebp.Client_AH], 50h
	call	PageFile_Call_DOS

;
;   Restore the user stack dword on the Win386 PSP
;
	movzx	eax, [PF_Our_PSP]
	shl	eax, 4
	push	[PF_Save_User_Stack]
	pop	[eax.PDB_User_stack]

	ret

EndProc PageFile_Set_Client_PSP

;******************************************************************************
;
;   PageFile_Call_DOS
;
;   DESCRIPTION:
;
;   ENTRY:
;	Must be in nested exec mode
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc PageFile_Call_DOS, High_Freq

	Assert_Ints_Enabled

	push	ecx
	push	edx
	mov	ecx, [PF_Orig_DOS_Vector]
	movzx	edx, cx
	shr	ecx, 10h
	VMMcall Build_Int_Stack_Frame
	ClrFlag [ebp.Client_EFlags], DF_Mask	; WORK AROUND BUSTED SOFTWARE
	VMMcall Resume_Exec
	bt	[ebp.Client_Flags], CF_Bit
	pop	edx
	pop	ecx
	ret

EndProc PageFile_Call_DOS


	PAGE
;******************************************************************************
;			D E B U G G I N G   C O D E
;******************************************************************************

IFDEF DEBUG

;******************************************************************************
;
;   PageFile_Debug_Query
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


BeginProc PageFile_Debug_Query

	cmp	[PF_Max_File_Pages], 0
	ja	SHORT PF_DQ_Enabled
	Trace_Out "Demand paging is disabled"
	clc
	ret

PF_DQ_Enabled:

	Trace_Out "SPART.PAR file = ", No_EOL
	pushad
	mov	esi, OFFSET32 PF_Spart_File_Buffer
	VMMcall Out_Debug_String
	popad
	Trace_Out " "
	Trace_Out "Swap file = ", No_EOL
	pushad
	mov	esi, OFFSET32 PF_Our_File_Name
	VMMcall Out_Debug_String
	popad
	Trace_Out " "
	mov	edi, [PF_Max_File_Pages]
	mov	eax, [PF_Cur_File_Pages]
	Trace_Out "File contains #EAX pages of a possible #EDI"
	Trace_Out " "
	mov	ax, [PF_Our_PSP]
	Trace_Out "Win386 startup PSP = #AX"
	mov	ax, [PF_File_Handle]
	Trace_Out "File handle        = #AX"
	Trace_Out " "
	mov	eax, [PF_Orig_DOS_Vector]
	mov	ebx, eax
	shr	eax, 10h
	Trace_Out "Original DOS vector  = #AX:#BX"
	Trace_Out " "
	mov	eax, [PF_Lin_Page_Num]
	Trace_Out "V86 lin mapping page = #AX"

PF_DQ_Exit:
	clc
	ret

EndProc PageFile_Debug_Query

ENDIF

VxD_LOCKED_CODE_ENDS

VxD_DATA_SEG

PSC_Call_Table LABEL DWORD
	dd	offset32 PSC_Get_Version	; AX = 0000
	dd	offset32 PSC_Get_Paging_Info	; AX = 0001
	dd	offset32 PSC_Paging_Info_Change ; AX = 0002
	dd	offset32 PSC_Get_Cur_Tmp_Sz	; AX = 0003

MAX_PSCFUNC	EQU ($-PSC_Call_Table)/4

VxD_DATA_ENDS

VxD_CODE_SEG

;******************************************************************************
;
;   PageFile_Svc_Call
;
;   DESCRIPTION:
;	This is a PM API entry point which is used by the Control Panel/
;	SwapFile guy to find out and set things about paging.
;
;	This API is only valid from the SYS VM
;
;   ENTRY:
;	EBP -> client frame
;	EBX = VM handle that called
;
;   EXIT:
;
;   USES:
;
;==============================================================================
BeginProc PageFile_Svc_Call

	VMMcall Test_Sys_VM_Handle
IFDEF DEBUG
	jz	short PSC_DOk
	debug_out "PageFile_Svc_Call from non VM1 VM!!!!!"
PSC_DOk:
ENDIF
	jnz	short PSC_BadCall
	movzx	eax,[ebp.Client_AX]
	cmp	ax,MAX_PSCFUNC
	jae	short PSC_BadCalla
	ClrFlag [ebp.Client_EFLAGS], CF_Mask	; clear carry
	Call	PSC_Call_Table[eax*4]		; Call appropriate code
PSC_Done:
	ret

PSC_BadCalla:
	debug_out "Function #EAX out of range PageFile_Svc_Call"
PSC_BadCall:
	SetFlag [ebp.Client_EFLAGS], CF_Mask	; Set carry
	jmp	short PSC_Done

EndProc PageFile_Svc_Call

;******************************************************************************
;
;   PSC_Get_Version
;
;   DESCRIPTION:
;
;   ENTRY:
;	EBP -> client frame
;	EBX = VM handle that called
;	Client_AX = 0000h
;
;   EXIT:
;	Client_EAX = PageFile version #
;	Client_Carry = clear
;
;   USES:
;
;==============================================================================

BeginProc PSC_Get_Version

	call	Internal_Get_Version
	mov	[ebp.Client_EAX],eax
	ret

EndProc PSC_Get_Version

;******************************************************************************
;
;   PSC_Get_Paging_Info
;
;   DESCRIPTION:
;
;   ENTRY:
;	EBP -> client frame
;	EBX = VM handle that called
;	Client_AX = 0001h
;	Client_DS:DI -> 128 byte file name buffer for SPART.PAR name
;	Client_DS:SI -> 128 byte file name buffer for paging file name
;
;   EXIT:
;	Client_Carry = clear
;	    Client_DS:DI -> filled in (if approp)
;	    Client_DS:SI -> filled in (if approp)
;	    Client_EAX = Flag Low two bits are the pager type currently in
;			 operation. NOTE that you must mask to these bits
;			 before checking value.
;
;		      bit 8000h is set if a corrupt permanent swapfile
;			was detected during pagefile init.
;
;		      Low two bits:
;
;			== 0 if paging is OFF
;			== 1 if paging using temporary swap file DS:SI is file
;			   name Client_ECX = MAX size of the paging file in
;			   bytes.
;			== 2 if paging using permanent swap file non-blockdev
;			   DS:DI is name of SPART.PAR, DS:SI is name of
;			   partition file extracted from SPART.PAR
;			   Client_ECX = MAX size of the paging file in bytes.
;			== 3 if paging using permanent swap file AND BLOCKDEV
;			   DS:DI is name of SPART.PAR, DS:SI is name of
;			   partition file extracted from SPART.PAR
;			   Client_ECX = MAX size of the paging file in bytes.
;
;	Client_Carry = set
;	    Invalid buffer pointer passed in
;
;   USES:
;
;==============================================================================

BeginProc PSC_Get_Paging_Info

	xor	eax,eax
	cmp	[PF_Max_File_Pages], 0
	je	short PSC_GPISetDone

	Client_Ptr_Flat edi,ds,si

	inc	edi
	jz	short PSC_GPIErr
	dec	edi
	mov	esi,OFFSET32 PF_Our_File_Name
	mov	ecx,128/4
	cld
	rep	movsd
    ;
    ; Need to get Max file size in ECX.
    ;
	mov	eax, [PF_Max_File_Pages]
	shl	eax, 12 			; Convert pages to bytes
	mov	[ebp.Client_ECX],eax
	cmp	[PF_Have_Partition], True	; Permanent swap file?
	je	short PSC_GPIFinishPerm 	; Yes, go do spart name
	mov	eax,1
	jmp	short PSC_GPISetDone

PSC_GPIFinishPerm:

	Client_Ptr_Flat edi,ds,di

	inc	edi
	jz	short PSC_GPIErr
	dec	edi
	mov	esi,OFFSET32 PF_Spart_File_Buffer
	mov	ecx,128/4
	cld
	rep	movsd
	mov	eax,2
	cmp	[PF_Using_BlockDev], True	; Using BlockDev for I/O?
	jne	short PSC_GPISetDone		; No, 2 is correct
	inc	eax				; Change 2 to 3
PSC_GPISetDone:
	cmp	[PF_Spart_Was_Corrupt],True
	jne	short PSC_GPISetDone2
	or	ax,8000h			; Set high bit, corrupt swap
PSC_GPISetDone2:
	mov	[ebp.Client_EAX],eax
PSC_GPIDone:
	ret

PSC_GPIErr:
	debug_out "One of the client buffer pointers is invalid PSC_Get_Paging_Info"
	SetFlag [ebp.Client_EFLAGS], CF_Mask	; Set carry
	jmp	short PSC_GPIDone

EndProc PSC_Get_Paging_Info

;******************************************************************************
;
;   PSC_Paging_Info_Change
;
;   DESCRIPTION:
;
;   ENTRY:
;	EBP -> client frame
;	EBX = VM handle that called
;	Client_AX = 0002h
;
;   EXIT:
;	Client_Carry = clear
;
;   USES:
;
;==============================================================================

BeginProc PSC_Paging_Info_Change

	cmp	[PF_Have_Partition], True	; Valid?
	jne	short PSC_PIF_Done		; No just ignore
	cmp	[PF_PermDel],True		; Already marked for delete?
	je	short PSC_PIF_Done		; Yes, just ignore
	mov	esi, OFFSET32 PF_Spart_File_Buffer

	VMMCall Set_Delete_On_Exit_File
IFDEF DEBUG
	jnc	short PFD80
	debug_out "Could not set DELETE ON EXIT for permanent swap file 3a"
PFD80:
ENDIF
	mov	esi, OFFSET32 PF_Our_File_Name

	VMMCall Set_Delete_On_Exit_File
IFDEF DEBUG
	jnc	short PFD90
	debug_out "Could not set DELETE ON EXIT for permanent swap file 3b"
PFD90:
ENDIF
	mov	[PF_PermDel],True
PSC_PIF_Done:
	ret

EndProc PSC_Paging_Info_Change


;******************************************************************************
;
;   PSC_Get_Cur_Tmp_Sz
;
;   DESCRIPTION:
;	Return the current size of the temp paging file.
;
;   ENTRY:
;	EBP -> client frame
;	EBX = VM handle that called
;	Client_AX = 0003h
;
;   EXIT:
;	Client_Carry = clear
;	Client_DX:AX = Size in bytes, will be 0 if not tmp setting.
;
;   USES:
;
;==============================================================================

BeginProc PSC_Get_Cur_Tmp_Sz

	xor	eax,eax
	mov	[ebp.Client_DX], ax		; Init return to 0
	mov	[ebp.Client_AX], ax
	cmp	[PF_Max_File_Pages], eax	; Paging off?
	je	short PSC_GTSzDone		; Yes, invalid call
	cmp	[PF_Have_Partition], True	; Permanent swap file?
	je	short PSC_GTSzDone		; Yes, invalid call
	mov	eax, [PF_Cur_File_Pages]
	shl	eax, 12 			; Convert pages to bytes
	mov	[ebp.Client_AX],ax
	shr	eax,16
	mov	[ebp.Client_DX],ax
PSC_GTSzDone:
	ret

EndProc PSC_Get_Cur_Tmp_Sz

VxD_CODE_ENDS

	END PageFile_Real_Init
