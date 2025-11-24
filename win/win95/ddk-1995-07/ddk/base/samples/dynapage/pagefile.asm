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
TITLE PageFile.Asm - Demand Paging File Device
;******************************************************************************
;
;   Title:	PageFile.Asm - Demand Paging File Device
;
;	This virtual device implements the paging file services for the
;	PageFile device.  The paging file is resizeable and uses the IFSMgr
;	or int 21h to perform the disk access.
;
;	Note that all the routines for paging through Int 21h are in the
;	pageable RARE segment, even the routines for actually paging
;	things in and out.  This is OK because whenever we are paging though
;	int 21h we are considered to be "paging through dos" and all the
;	VxD code and data segments are locked down by VMM in that case.
;
;==============================================================================

	.386p

;******************************************************************************
;			      I N C L U D E S
;******************************************************************************

	.XLIST
	INCLUDE VMM.Inc
	INCLUDE Debug.Inc
	INCLUDE dosmgr.inc
	INCLUDE Smartdrv.inc
	INCLUDE PDB.Inc
	INCLUDE Int2FAPI.Inc
	INCLUDE Int2FAPI.Inc
	INCLUDE OptTest.Inc
DOS_VXD equ 1		; temporary until fixed in ifs.inc
MASM	equ 1		; temporary until fixed in ifs.inc

	INCLUDE ifs.inc
	INCLUDE ifsmgr.inc
	INCLUDE ios.inc
	INCLUDE irs.inc

	Create_PageFile_Service_Table EQU VMM_TRUE

	INCLUDE PageFile.Inc
	INCLUDE PageSwap.Inc

	.LIST

;******************************************************************************
;		V I R T U A L	D E V I C E   D E C L A R A T I O N
;******************************************************************************

Declare_Virtual_Device PAGEFILE, 4, 00h, PageFile_Control, PageFile_Device_ID, \
		       PageFile_Init_Order, , PageFile_Svc_Call

;******************************************************************************
;			      E Q U A T E S
;******************************************************************************


PF_V86_Stack_Size   EQU     400 	; 400 byte stack while paging

;******************************************************************************
;******************************************************************************


VxD_IDATA_SEG

EXTRN PF_Enable_Ini:BYTE
EXTRN PF_Swap_File_Ini:BYTE
EXTRN PF_Swap_Drive_Ini:BYTE
EXTRN PF_Max_Size_Ini:BYTE
EXTRN PF_Min_Size_Ini:BYTE
EXTRN PF_Min_Free_Ini:BYTE


PF_pAsyncMgr	dd	OFFSET32 PageFile_Default_Async_Manager

PF_Swap_File_Name	db	"WIN386.SWP", 0
PF_Swap_File_Name_Len	EQU	$-PF_Swap_File_Name

VxD_IDATA_ENDS


VxD_PAGEABLE_DATA_SEG
	ALIGN 4

public PF_Our_File_Name
PF_Our_File_Name	db	128 dup (0)

PF_Max_File_Pages	dd	0		; If 0 then paging is disabled
PF_Min_File_Pages	dd	0
PF_Cur_File_Pages	dd	0
PF_Drive_Number 	dd	0		; 1-based drive number of swap file
PF_Min_Free_Pages	dd	?		; number of pages to try to keep
						; free on the swap drive

public PF_Our_File_Name
public PF_Max_File_Pages
public PF_Min_File_Pages
public PF_Cur_File_Pages
public PF_Drive_Number
public PF_Min_Free_Pages


; variables for int 21 paging.	These are all usually ignored.

PF_Orig_DOS_Vector	dd	?
PF_Lin_Page_Num 	dd	-1
PF_V86_Stack_Seg_Off	dd	?
PF_Cache_Lock_Ptr	dd	0
PF_Save_User_Stack	dd	?
PF_Client_PSP		dw	?
PF_Our_PSP		dw	?
PF_DOS_IO_Count 	db	0		; If <> 0 then we called DOS

public PF_Orig_DOS_Vector
public PF_Lin_Page_Num
public PF_V86_Stack_Seg_Off
public PF_Cache_Lock_Ptr
public PF_Save_User_Stack
public PF_Client_PSP
public PF_Our_PSP
public PF_DOS_IO_Count

VxD_PAGEABLE_DATA_ENDS

VxD_LOCKED_DATA_SEG
	ALIGN 4
PF_File_Handle		dd	0
PF_Pager_Type		db	0		; 1 if off, 2 if dos, 3 if smart
PF_Int21_Pager		db	0		; 1 if paging through int21

public PF_File_Handle
public PF_Pager_Type
public PF_Int21_Pager


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
;	edx = real-mode pointer to cache lock byte (set this byte to turn
;	      off real-mode disk cache) or 0 if no real mode cache found
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
;   (Historical note: Bimbo was the code name for SmartDrv version something)

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


BeginDoc
;******************************************************************************
;
;   PageFile_Init_File
;
;   DESCRIPTION:
;	Note: This service can only be called at initialization time!
;
;	This service will initialize the paging file.  It should only be called
;	from the PageSwap virtual device.
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

	trace_out	"PageFile: can't set swap file to minimum size"

;   Test for paging enabled

	mov	eax, VMM_TRUE			; Default value = ON
	xor	esi, esi			; Use Win386 section
	mov	edi, OFFSET32 PF_Enable_Ini	; Look for this string
	VMMcall Get_Profile_Boolean		; Get the value
	test	eax, eax			; Q: Has user disabled it
	jz	PF_Init_No_Paging		;    Y: Done!
						;    N: Try to open swap file

;   Paging is enabled.	Create the swap file.

	call	PageFile_Open_Swap_File 	;    N: Try to use a DOS file

	mov	ebx, [PF_Max_File_Pages]
	test	ebx, ebx
	jz	PF_Init_No_Paging

PF_Init_Exit:
	popad					; Restore caller's regs
	mov	eax,[PF_Cur_File_Pages] 	;   and setup return values
	mov	ebx,[PF_Max_File_Pages]
	clc
	ret


;   Paging could not be intialized for some reason.  Set Max_File_Pages to 0.

PF_Init_No_Paging:
	mov	[PF_Max_File_Pages], 0		; Make sure it's really off
	mov	[PF_Pager_Type],PF_Paging_None	; paging off
	jmp	SHORT PF_Init_Exit

EndProc PageFile_Init_File



;******************************************************************************
;
;   PageFile_Open_Swap_File
;
;   DESCRIPTION:
; How we pick where the swap file will go:
;
; if a pagingfile= or pagingdrive= is specified, use it
;   if the drive specified is invalid or full, fall through to the default below
; else, use the windows directory
;   if the windows directory is compressed with a real mode driver, use the host
;      if the host is full, back to the windows dir
;
;   ENTRY:
;
;   EXIT:
;	Success is indicated by setting PF_Max_File_Pages
;	Failure by not modifying PF_Max_File_Pages
;
;   USES:
;
;==============================================================================

BeginProc PageFile_Open_Swap_File

LocalVar  po_di, <SIZE DemandInfoStruc>
LocalVar  po_kbmindefault, DWORD	; computed needed minimum kb for file
LocalVar  po_comp, <SIZE IRS_drv_comp>	; for ios to tell us if drive compressed
LocalVar  po_getdrv, <SIZE IRS_drv_get> ; for ios to tell us if RMM
LocalVar  po_fusedefault, BYTE		; non-zero if we are using the default
					; file name (c:\windows\win386.swp)
LocalVar  po_fusehost, BYTE		; non-zero if we have moved swap file
					; to host drive of compressed volume
LocalVar  po_fforcepath, BYTE		; non-zero if we are some error path
					; and really want to try a given filespec

        EnterProc
	pushad

	mov	[po_fusedefault],0
	mov	[po_fusehost],0
	mov	[po_fforcepath],0

;	See if the user has a requested minimum size.  If not, we default to
;	max(0, 9meg - RAM)

	lea	esi,[po_di]			; (esi) = GetDemandPageInfo struc
	VMMCall _GetDemandPageInfo, <esi, 0>
	mov	ecx,[esi].DIPhys_Count		; (ecx) = pages of RAM
	shl	ecx,2				; (ecx) = kb of RAM
	mov	eax,9*1024			; (eax) = 9meg in kb
	sub	eax,ecx 			; (eax) = 9meg - RAM in kb
	jns	osf02				; jump if positive value
	xor	eax,eax 			; (eax) = else use 0 minimum
osf02:	mov	[po_kbmindefault],eax		; save default minimum size

	xor	esi,esi 			; (esi) = use [386enh]
	mov	edi, offset PF_Min_Size_Ini	; (edi) = "MinPagingFileSize"
	VMMCall Get_Profile_Decimal_Int 	; (eax) = minimum size in k
	shr	eax,2				; (eax) = min size in pages
	mov	[PF_Min_File_Pages],eax 	; save it

;	See if the user requested a certain amount of space to be left free
;	on the drive.  If not, default to 512k so we can be sure that we can
;	save registry changes, etc.  Note that minimum sizes over-ride this
;	value, so you could see less free space than this initially.

	mov	eax,512 			; (eax) = default 512k
	xor	esi,esi 			; (esi) = use 386enh section
	mov	edi,OFFSET32 PF_Min_Free_Ini	; (edi) = "MinUserDiskSpace"
	VMMcall Get_Profile_Decimal_Int 	; (eax) = # of K to leave free
	shr	eax,2				; (eax) = pages to leave free
	mov	[PF_Min_Free_Pages],eax 	; save it

;	See if user has a requested maximum size

	mov	eax,2*1024*1024 		; (eax) = default max 2 gig
	xor	esi, esi			; Nil means use [Win386] section
	mov	edi, OFFSET32 PF_Max_Size_Ini	; EDI -> "MaxPagingFileSize"
	VMMcall Get_Profile_Decimal_Int 	; EAX = Maximum file size
	shr	eax, 2				; EAX = our max size in pages
	mov	[PF_Max_File_Pages], eax	; save it

;
;   Check for a user-specified file name (pagingfile=).  If so, ignore any
;   pagingdrive= parameters.
;
	xor	esi, esi
	mov	edi, OFFSET32 PF_Swap_File_Ini
	VMMcall Get_Profile_String
	jc	SHORT PF_OSF_Create_File_Name
	mov	esi, edx
	mov	edi, OFFSET32 PF_Our_File_Name
	cld
PF_OSF_Copy_To_Our_Buff:
	lodsb
	stosb
	test	al, al
	jnz	PF_OSF_Copy_To_Our_Buff
	jmp	SHORT PF_OSF_Have_File_Name

;
;   If we get here, there was no pagingfile= line, we have to create
;   the file name ourselves.
;   Check to see if the user specified a particular drive
;
PF_OSF_Create_File_Name:
	xor	esi, esi
	mov	edi, OFFSET32 PF_Swap_Drive_Ini
	VMMcall Get_Profile_String
	mov	edi, OFFSET32 PF_Our_File_Name
	cld
	jc	SHORT PF_OSF_Use_Config_Dir
	mov	bl,[edx]		; (bl) = drive letter

PF_OSF_Have_Drive:
	mov	al, bl
	stosb
	mov	ax, "\:"
	stosw
	jmp	SHORT PF_OSF_Copy_File_Name

PF_OSF_Use_Config_Dir:
	mov	[po_fusedefault],1
	VMMcall Get_Config_Directory
	mov	esi, edx
PF_OSF_Copy_Loop:
	lodsb
	test	al, al
	jz	SHORT PF_OSF_Copy_File_Name
	stosb
	jmp	PF_OSF_Copy_Loop

PF_OSF_Copy_File_Name:
	mov	esi, OFFSET32 PF_Swap_File_Name
	mov	ecx, PF_Swap_File_Name_Len
	rep movsb

PF_OSF_Have_File_Name:

;	Save drive number for later

	movzx	edx,byte ptr [PF_Our_File_Name] ; (edx) = drive letter
	and	dl, NOT ("a"-"A")		; Make drive letter UPPER case
	sub	dl, "A"-1			; (dl) = 1 based drive letter
	mov	[PF_Drive_Number],edx		; save


;	If we are using the default path, make sure the drive isn't compressed
;	by some evil real-mode compression driver.  If so, move the swap
;	file to the host drive.

	dec	edx				; (edx) = 0 based drive letter
	lea	eax,[po_comp]			; (eax) = packet for ios
	mov	[eax].IRS_i_d_c_drive,dl
	mov	[eax].IRS_func,IRS_IS_DRVCOMPRESSED
	push	eax				; push parameter for ios
	VxDCall IOS_Requestor_Service
	pop	eax				; clean stack and restore ptr
	cmp	[eax].IRS_result,0		; error?
	jnz	osf08				; jump if error, assume no compression

	TestMem [eax].IRS_i_d_c_flags,(IRS_I_D_C_COMP+IRS_I_D_C_PM_COMP)
	jz	osf08				; jump if no compression

	TestMem [eax].IRS_i_d_c_flags,IRS_I_D_C_PM_COMP
	jnz	osf08				; or if prot mode compression

;	If system.ini or previous errors are forcing a particular path, put the
;	swap file on the compressed volume even if we don't like the compression

	cmp	[po_fusedefault],0		; default path or given one?
	jz	osf08				; jump if given a path
	cmp	[po_fforcepath],0		; forced path because of errors?
	jnz	osf08				; jump if forced to use

;	Here we have a real-mode compression driver so we have to swap to the
;	host drive instead.

osf05:	mov	bl,[eax].IRS_i_d_c_drive	; (bl) = host drive number
	add	bl,"A"				; (bl) = host drive letter
	mov	edi, OFFSET32 PF_Our_File_Name	; reset file name
	mov	[po_fusehost],1
	jmp	PF_OSF_Have_Drive		; jump up to reform file name

;	Find out if ifsmgr is in control of this drive

osf08:	mov	[PF_Pager_Type],PF_Paging_DOS	; assume using dos

	mov	edx,[PF_Drive_Number]		; (edx) = 1 based drive number
	dec	edx				; (edx) = 0 based drive number
	VxDCall IFSMgr_Ring0GetDriveInfo	; are we dragon?
	jc	osf100				; jump if no IFS for drive

	test	eax,IFS_DRV_RMM 		; using real mode mapper?
	jnz	osf10				; jump if we are

;	Find out if this drive has a nice safe protected mode driver or
;	if a nasty driver that might block with events enabled or have
;	pageable code (which we will have to treat like an RMM drive).

	mov	edx,[PF_Drive_Number]		; (edx) = 1 based drive number
	add	dl,'A' - 1			; (edx) = 0 based drive letter
	lea	eax,[po_getdrv] 		; (eax) = packet for ios
	mov	[eax].IRS_DrvLetter,dl
	mov	[eax].IRS_func,IRS_GET_DRVINFO
	push	eax				; push parameter for ios
	VxDCall IOS_Requestor_Service
	pop	eax				; clean stack and restore ptr
	cmp	[eax].IRS_result,0		; error?
	jnz	osf09				; error is odd, but if so trust IFSMgr

	TestMem [eax].IRS_DrvFlags,IRS_DRV_PAGEABLE ; bad driver?
	jnz	osf10				; if bad dos-like driver, jump

osf09:	mov	[PF_Pager_Type],PF_Paging_HW	; else we are using dragon
osf10:

;	Open or Create the file

	mov	eax,0d500h			; (eax) = open function code
	mov	bx,0110000010010010b		; (bx) = flags: r/w, deny all,
						;	no inherit, no int 24,
						;	no caching
	xor	ecx,ecx 			; (ecx) = attributes (nothing)
	mov	dl,00010001b			; (dl) = open mode, open or create
	mov	dh,((R0_SWAPPER_CALL or R0_NO_CACHE or OPEN_FLAGS_NO_COMPRESS) SHR 8)
						; (dh) = special flags
	mov	esi, offset32 PF_Our_File_Name	; (esi) = file name
	VxDCall IFSMgr_Ring0_FileIO		; do the open  (eax) = handle
	jc	osf110				; jump if error

	mov	[PF_File_Handle],eax		; save file handle

;	File is open.  Set its size to the minimum.  We want to do this even
;	if the minimum is 0 since we might have a large left-over file from the
;	last boot.

osf20:	mov	edx,[PF_Min_File_Pages] 	; (edx) = requested minimum
	call	PageFile_Set_File_Size		; do the resize
	jnc	osfx				; jump if sizing worked
	trace_out	"PageFile: can't set swap file to minimum size"

;	If we can't get the requested minimum size, try the computed absolute
;	minimum value.

	mov	edx,[po_kbmindefault]		; (edx) = minimum minimum in k
	shr	edx,2				; (edx) = in pages
	mov	[PF_Min_File_Pages],edx
	call	PageFile_Set_File_Size		; do the resize
	jc	osf90				; jump if sizing failed
						;   otherwise, we are done
osfx:	popad
	LeaveProc
	Return
	debug_out	"PageFile: can't set swap file to very minimum size"

;	If we can't get even the very minimum size, try putting the swap file
;	in the default directory, if we haven't already.

osf90:	VxDCall PageFile_Clean_Up		; close swap file
	jmp	osf110				; try default


;	If are trying to open a swap file on a drive not under the control of
;	ifsmgr, see if we can access it from Dos.

osf100: call	PageFile_Int21_Open
	jnc	osf20

;	If we get an error from the int 21 method, fall through into the
;	error path below.


;	If we get an error trying to open the swap file, try again but this
;	time use the default file name (maybe the user set the swap file to
;	an invalid drive.

osf110: mov	[PF_Int21_Pager],0

ifdef DEBUG
	Trace_Out	"PageFile: unable to open swap file: ", NOEOL
	mov	esi, OFFSET32 PF_Our_File_Name
	VMMcall Out_Debug_String
endif

	mov	edi, OFFSET32 PF_Our_File_Name	; setup to try again
	cmp	[po_fusedefault],0	; did we already use default?
	Trace_OutZ	".  Attempting to use default path"
	jz	PF_OSF_Use_Config_Dir	; jump to try default if we haven't yet

;	If we were already trying the default directory, but because of
;	real-mode compression we were using the host, as a last straw try
;	paging to the evil compressed drive.

	mov	[po_fforcepath],1	; force onto compressed drive
	cmp	[po_fusehost],0 	; on host?
	mov	[po_fusehost],0 	; (not any more)
	jnz	PF_OSF_Use_Config_Dir	; if host, jump to try default again

;	If we were unable to get any swap file at all, don't even bother
;	to try to boot.  We could make an attempt, but it is so sure to fail
;	that we will probably just make things worse (especially since so
;	few components report out of memory errors well).  Unless of course
;	the user has enough RAM to boot without a swap file.

	mov	[PF_Max_File_Pages],0	; turn swapping off
	cmp	[po_kbmindefault],0	; need any swap file?
	jz	osfx			; jump to exit if we don't need swapping

	VMMCall Fatal_Memory_Error	; otherwise, die a horrible death

EndProc PageFile_Open_Swap_File


BeginDoc
;******************************************************************************
;
;   PageFile_Int21_Open
;
;   DESCRIPTION:
;	Called by PageFile_Open_Swap_File to try to open the swap file
;	via int 21s.
;
;   ENTRY:
;	PF_Our_File_Name setup
;
;   EXIT:
;	carry set if couldn't open the swap file using int 21
;
;   USES:
;	Flags.
;
;==============================================================================
EndDoc

BeginProc PageFile_Int21_Open

	pushad

;   Get the REAL DOS Int 21h vector so we can call it directly at all times.

	mov	eax, 21h
	VMMcall Get_V86_Int_Vector
	shl	ecx, 10h
	mov	cx, dx				; ECX = Seg:Offset of Int 21h
	mov	[PF_Orig_DOS_Vector], ecx

;
;   Get the Win386 PSP segment so we can switch back to it.
;
	VMMcall Get_PSP_Segment
	mov	[PF_Our_PSP], ax


;	Open the new file by creating a new file, closing it, then re-open it.
;	We go through this pain so we can use the nice low numbered int 21
;	functions which are more likely to work on more platforms

	mov	ah,3ch				; (ah) = create function code
	xor	ecx,ecx 			; (ecx) = no attributes
	mov	edx,OFFSET32 PF_Our_File_Name	; (edx) = name of swap file
	VxDInt	21h
	jc	piox				; jump to exit if error

	mov	ebx,eax 			; (ebx) = file handle
	mov	ah,3eh				; (ah) = close file function
	VxDInt	21h
	Debug_OutC	"PageFile_Int21_Open: close failed"

	mov	ah,3dh				; (ah) = open function
	mov	al,10010010b			; (al) = r/w, deny all, no inherit
	VxDInt	21h
	jc	piox

	mov	[PF_File_Handle],eax		; save file handle

;  Allocate a page we can map our paging i/o to

	VMMCall _Allocate_Global_V86_Data_Area,<1000h, GVDAPageAlign+GVDAReclaim>
	test	eax,eax
	jz	pio10
	shr	eax,PAGESHIFT
	mov	[PF_Lin_Page_Num], eax
	call	PageFile_Map_Null


;   Allocate V86 memory for our stack.

	VMMCall _Allocate_Global_V86_Data_Area, <PF_V86_Stack_Size, <GVDADWordAlign OR GVDAZeroInit>>
	test	eax, eax
	jz	pio20
	shl	eax, 12
	shr	ax, 12				; EAX = Seg:Off of base of stack
	add	ax, PF_V86_Stack_Size		; AX = SP to start at
	mov	[PF_V86_Stack_Seg_Off], eax	; Save pointer for later

;   Swapping to a DOS file.  Hook Int 24h so we can fail it while paging.

	mov	esi, OFFSET32 PageFile_Int_24h
	mov	eax, 24h
	VMMcall Hook_V86_Int_Chain

;   Swapping to a DOS file.  Hook Int 23h so we can fail it while paging.

	mov	esi, OFFSET32 PageFile_Int_23h
	mov	eax, 23h
	VMMcall Hook_V86_Int_Chain

	inc	[PF_Int21_Pager]
	clc					; success
piox:	popad
	ret


;   Paging could not be intialized for some reason.  Set Max_File_Pages to 0.

pio10:
pio20:
	VxDcall PageFile_Clean_Up		; Try to remove the swap file
	stc					; indicate error
	jmp	piox				; jump to common exit

EndProc PageFile_Int21_Open


BeginDoc
;******************************************************************************
;
;   PageFile_Set_Async_Manager
;
;   DESCRIPTION:
;	Note: This service can only be called at initialization time!
;
;	An FSD offering async page-outs stores an internal entry
;	which can be called to register a call-back that is invoked
;	at FSD idle time.
;
;   ENTRY:
;	EAX = Entry in FSD
;
;   EXIT:
;	Carry: success iff clear, else error
;
;   USES: Flags
;
;==============================================================================
EndDoc
BeginProc PageFile_Set_Async_Manager, Service
	cmp	[PF_pAsyncMgr], OFFSET32 PageFile_Default_Async_Manager
	jne	PageFile_Default_Async_Manager
	mov	[PF_pAsyncMgr], eax
	clc
	ret
EndProc PageFile_Set_Async_Manager

BeginDoc
;******************************************************************************
;
;   PageFile_Call_Async_Manager
;
;   DESCRIPTION:
;	Note: This service can only be called at initialization time!
;
;	Call the FSD function to register a call-back that is
;	called at FSD-idle time and return the FSD entry for async
;	pageouts.
;
;   ENTRY:
;	EAX = address of call-back function
;
;   EXIT:
;	if carry clear
;		EAX = FSD async pageout entry
;	else carry set (error)
;
;   USES: Flags
;
;==============================================================================
EndDoc
BeginProc PageFile_Call_Async_Manager, Service
	jmp	[PF_pAsyncMgr]
PageFile_Default_Async_Manager LABEL NEAR
	stc
	ret
EndProc PageFile_Call_Async_Manager

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
IFDEF DEBUG
	Control_Dispatch Debug_Query, PageFile_Debug_Query
ENDIF

	clc
	ret

EndProc PageFile_Control

VxD_LOCKED_CODE_ENDS
VxD_PAGEABLE_CODE_SEG

BeginDoc
;******************************************************************************
;
;   PageFile_Clean_Up
;
;   DESCRIPTION:
;	This service will "clean-up" the paging file.  We shink the file down
;	to its minimum size.
;	This code is normally called
;	by the PageSwap device at System Exit time.
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

BeginProc PageFile_Clean_Up, Service, SYSEXIT

	mov	edx,[PF_Min_File_Pages]
	call	PageFile_Set_File_Size
ifdef DEBUG
	Debug_OutC	"PageFile: clean-up shrink failed 1"
endif

	mov	ebx,[PF_File_Handle]		; (ebx) = file handle
	cmp	[PF_Int21_Pager],0		; use int 21?
	jnz	pcu20				; jump if int 21

	mov	eax,0d700h			; (ax) = close function
	VxDCall IFSMgr_Ring0_FileIO		; do the resize
pcu10:
ifdef DEBUG
	Debug_OutC	"PageFile: clean-up shrink failed 2"
endif
	ret

pcu20:	mov	ah,3eh				; (ah) = close function
	VxDInt	21h
	jmp	pcu10

EndProc PageFile_Clean_Up


BeginDoc
;******************************************************************************
;
;   PageFile_Set_File_Size
;
;   DESCRIPTION:
;	Set the paging file to the size specified.
;	This routine does not perform any checking about whether the specified
;	size is greater than or less than the minimum or maximum size.
;	If the call fails, the file size should remain unchanged (unless
;	we are paging through int 21).
;
;   ENTRY:
;	(edx) = new size, in pages
;
;   EXIT:
;	carry clear if success or set if failure
;
;   USES:
;	eax, ebx, ecx, flags
;
;==============================================================================
EndDoc

BeginProc PageFile_Set_File_Size, W16

	cmp	[PF_Int21_Pager],0
	jnz	PageFile_Int21_Set_File_Size	; jump if using int 21

	mov	ebx,[PF_File_Handle]		; (ebx) = file handle

	xor	ecx,ecx 			; (ecx) = 0 size
	mov	eax,0d601h			; (eax) = write
	push	edx
	shl	edx,PAGESHIFT			; (edx) = size of file in bytes
	VxDCall IFSMgr_Ring0_FileIO		; do the resize
	pop	edx
	jc	PSFS_Exit			; jump if error

ifdef DEBUG

;	Unlike DOS, Ring0_FileIO is supposed to always allocate the requested
;	amount of space when the call succeeds.  Verify that here.

psfs20: mov	eax, 0d800h			; (eax) = get file size
	push	edx
	VxDCall IFSMgr_Ring0_FileIO		; get the file size
	pop	edx
	Debug_OutC "PageFile_Set_File_Size: error getting size"

	shr	eax, PAGESHIFT			; (eax) = filesize in pages
	cmp	eax, edx			; new size = asked size?
	Debug_OutNZ "PageFile_Set_File_Size: IFSMgr_Ring0_FileIO returned success but wrong size"
endif

	mov	[PF_Cur_File_Pages],edx 	; save new (requested) size

PSFS_Exit:
	ret

EndProc PageFile_Set_File_Size


BeginDoc
;******************************************************************************
;
;   PageFile_Int21_Set_File_Size
;
;   DESCRIPTION:
;	Set the paging file to the size specified using int 21.
;	Unlike the normal PageFile_Set_File_Size, if a grow fails, this function
;	may leave the file larger than it was on entry.  But why waste the
;	code (which will never be tested properly) to shrink it back.
;	3.1 did this also.
;
;   ENTRY:
;	(edx) = new size, in pages
;
;   EXIT:
;	carry clear if success or set if failure
;
;   USES:
;	flags
;
;==============================================================================
EndDoc

BeginProc PageFile_Int21_Set_File_Size, RARE
	pushad
	xor	esi,esi 		; (esi) = useful zero value
	mov	edi,edx 		; (edi) = (edx) = new file size in pages

	Push_Client_State
	VMMcall Begin_Nest_V86_Exec
	inc	[PF_DOS_IO_Count]

	shl	edx,PAGESHIFT		; (edx) = new size in bytes

	VMMcall Get_Cur_VM_Handle
	mov	ebp, [ebx.CB_Client_Pointer]

	mov	eax, [PF_V86_Stack_Seg_Off]
	mov	[ebp.Client_SP],ax
	shr	eax, 16
	mov	[ebp.Client_SS],ax

;
;   Set PSP to Win386.Exe PSP to make DOS calls
;
	call	PageFile_Set_Our_PSP

;
;   Set the file handle in BX to the paging file
;
	mov	eax,[PF_File_Handle]
	mov	[ebp.Client_EBX],eax

;
;   Seek to new desired size and do a 0 byte write to grow the file.  Note that
;   the DOS call will NOT fail if the disk is full.
;
	mov	[ebp.Client_EDX],edx		; low word of size
	shr	edx, 16
	mov	[ebp.Client_ECX],edx		; high word of size
	mov	[ebp.Client_AX], 4200h
	call	PageFile_Call_DOS
	jc	PF_IGF_Cant_Grow

	mov	[ebp.Client_AH], 40h
	mov	[ebp.Client_ECX],esi		; zero
	call	PageFile_Call_DOS
	jc	PF_IGF_Cant_Grow


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
	mov	eax,[PF_File_Handle]		; Must restore file handle in
	mov	[ebp.Client_EBX],eax		; client's BX

	mov	[ebp.Client_AX], 4202h		; seek to end
	mov	[ebp.Client_ECX],esi		; zero offset from end
	mov	[ebp.Client_EDX],esi		; zero offset from end
	call	PageFile_Call_DOS		; Seek to end of file
	jc	SHORT PF_IGF_Cant_Grow		; If error then just give up

	mov	eax, [ebp.Client_EDX]
	shl	eax, 16
	mov	ax, [ebp.Client_AX]		; (eax) = new size in bytes
	shr	eax,PAGESHIFT			; (eax) = new size in pages

	mov	[PF_Cur_File_Pages],eax 	; New size of file
	cmp	eax, edi			; Q: Desired size?
	jnz	PF_IGF_Cant_Grow		;    N: jump to error

	mov	esi,esp 			; indicate success, random non-0
						;  will clear carry in sub esi,1
;   Done!  Restore original PSP and return.

PF_IGF_Cant_Grow:				; if we jump here, esi=0 which
						;  will set carry in sub esi,1
GF_Exit:
	call	PageFile_Set_Client_PSP

	dec	[PF_DOS_IO_Count]
	VMMcall End_Nest_Exec
	Pop_Client_State
	sub	esi,1				; set carry if error

	popad
	ret
EndProc   PageFile_Int21_Set_File_Size


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

BeginProc PageFile_Get_Version, Service, RARE

	mov	bl,[PF_Pager_Type]

Internal_Get_Version LABEL NEAR

PF_GV_Exit:
	mov	eax, 400h			; Version 4.0
	clc
	ret

EndProc PageFile_Get_Version

BeginDoc
;******************************************************************************
;
;   PageFile_Grow_File
;
;   DESCRIPTION:
;	Attempts to grow or shrink the size of the paging file.
;
;   ENTRY:
;	ECX = Number of pages to change paging file size by (may be negative)
;
;   EXIT:
;	EAX = Current (new) # file pages
;	EBX = Maximum # file pages
;
;   USES: Flags
;
;==============================================================================
EndDoc

BeginProc PageFile_Grow_File, Service, esp, W16

LocalVar	pgf_locktype,DWORD		; used for IFSMgr_GetLockState

	EnterProc

	SaveReg <edx, ecx>	; WARNING - using [esp] below to get orig ecx

;	Compute the new size and put it in edx

	mov	edx,[PF_Cur_File_Pages] 	; (edx) = current size in pages
	add	edx,ecx 			; (edx) = requested size
ifdef DEBUG
	Debug_OutS	"PageFile_Grow_File: request for negative size"
endif
	js	pgfx				; jump to exit if negative


;	If the requested size is less than the minumum, set the size to the min

	cmp	edx,[PF_Min_File_Pages]
	ja	pgf10				; jump if bigger than min

	mov	edx,[PF_Min_File_Pages] 	; if below min, set to min
	cmp	edx,[PF_Cur_File_Pages] 	; if file already at min...
	jz	pgfx				; ...jump to exit (carry clear)


;	If the request is bigger than the maximum, return error

pgf10:	cmp	edx,[PF_Max_File_Pages]
	ja	pgf20				; jump to error if above max

;	The request is now between the max and the min.

;	If this is a shrink of the swap file, ignore it if we are in a volume
;	lock so we don't force disk utilities to restart for no good reason.

	or	ecx,ecx 			; negative value?
	jns	pgf15				; jump if positive (a grow)

	mov	ecx,[PF_Drive_Number]	; (ecx) = 1-based drive number
	dec	ecx			; (ecx) = 0-based drive number
	lea	eax,[pgf_locktype]	; (eax) = addr of lock return value

	SaveReg <edx>
	VxDCall IFSMgr_GetLockState, <ecx, eax, 0, 0>
	RestoreReg <edx>

	cmp	[pgf_locktype],0ffffffffh	; no lock?
	jnz	pgfx				; jump to exit if there's a lock
	jmp	pgf19				; if no lock, jump to do shink

;	If we get here, we are trying to grow the swap file.

pgf15:
	xor	eax,eax 			; zero high words for get free
	mov	ebx,eax 			;   space call below
	mov	ecx,ecx 			;

	mov	ax,3601h			; (ax) = get free space noblock
	mov	edx,[PF_Drive_Number]		; (edx) = 1 based drive number

	cmp	[PF_Int21_Pager],0
	jz	pgf16				; jump if not using int 21

	VxDInt	21h				; get free space using int 21
	jmp	pgf17				; back to common path

pgf16:	VxDCall IFSMgr_Ring0_FileIO		; get free space using ifsmgr
pgf17:	mov	edx,[esp]			; (edx) = size delta of grow
	Debug_OutC "PageFile_Grow_File: can't query free space" ; I don't think this can happen legitimately
	jc	pgf18				; if error, skip space check

ifdef DEBUG
;	The computations below assume that the high word of eax, ebx and ecx
;	are still zero from the xor and movs above.

	push	edx
	mov	edx,eax
	or	edx,ebx
	or	edx,ecx
	test	edx,0ffff0000h
	Debug_OutNZ	"PageFile_Get_Size_Info: high word trashed"
	pop	edx
endif

	imul	ecx,eax 		; (ecx) = bytes per cluster
	imul	ecx,ebx 		; (ecx) = bytes free disk space
	shr	ecx,PAGESHIFT		; (ecx) = page of free space
	sub	ecx,edx 		; (ecx) = space left (may be negative)
	cmp	ecx,[PF_Min_Free_Pages] ; enough free space left after grow?
	jl	pgf20			; if not enough space, jump to error

;	OK, we have enough space for the grow.	Go for it.

pgf18:	add	edx,[PF_Cur_File_Pages] ; (edx) = new file size

pgf19:	call	PageFile_Set_File_Size

;	All done, common exit code

pgfx:	RestoreReg <ecx, edx>
	mov	eax,[PF_Cur_File_Pages] 	; Return current (new) size
	mov	ebx,[PF_Max_File_Pages] 	;   and maybe diff max size
	LeaveProc
	Return

;	Common error path

pgf20:	stc
	jmp	pgfx

EndProc PageFile_Grow_File


BeginDoc
;******************************************************************************
;
;   PageFile_Cancel
;
;   DESCRIPTION:
;	This service is unused in this version of pagefile and should never
;	be called
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

BeginProc PageFile_Cancel, Service, RARE
	Debug_Out "PageFile_Cancel: should never be used"
	ret

EndProc   PageFile_Cancel

BeginDoc
;******************************************************************************
;
;   PageFile_Get_Size_Info
;
;   DESCRIPTION:
;	Returns information about the size of the paging file.
;
;   ENTRY:
;	None
;
;   EXIT:
;	EAX = Current size of the paging file, in pages
;	EBX = Absolute maximum size of paging file, in pages, as set by
;	      the "MaxPagingFile=" line of system.ini or 2 gigabytes if no
;	      MaxPagingFile value was specified.
;	      0 if paging is off.
;	ECX = Current maximum possible size of paging file, in pages, given
;	      current disk free space.
;	EDX = Minimum size of paging file, in pages, as set by the
;	      "MinPagingFile=" line of system.ini or 0 if no value was specified.
;	ESI = Pointer to null terminated string containing the full path
;	      name of the swap file.  Do not write to this pointer.
;	      Undefined if EBX == 0.
;
;   USES: Flags
;
;==============================================================================
EndDoc

BeginProc PageFile_Get_Size_Info, Service, W16

	xor	eax,eax 			; zero high words for get free
	mov	ebx,eax 			;   space call below
	mov	ecx,eax 			;

	mov	ah,36h				; (ah) = get free space
	mov	edx,[PF_Drive_Number]		; (edx) = 1 based drive number
	VxDInt	21h
	Debug_OutC "PageFile_Get_Size_Info: can't get free space"

ifdef DEBUG
;	The computations below assume that the high word of eax, ebx and ecx
;	are still zero from the xor and movs above.

	mov	edx,eax
	or	edx,ebx
	or	edx,ecx
	test	edx,0ffff0000h
	Debug_OutNZ	"PageFile_Get_Size_Info: high word trashed"
endif

	imul	ecx,eax 			; (ecx) = bytes per cluster
	imul	ecx,ebx 			; (ecx) = bytes free disk space
	shr	ecx,PAGESHIFT			; (ecx) = page of free space

	mov	esi,OFFSET32 PF_Our_File_Name	; (esi) = name of swap file
	mov	eax,[PF_Cur_File_Pages] 	; (eax) = current pages
	mov	ebx,[PF_Max_File_Pages] 	; (ebx) = absolute maximum pages
	add	ecx,eax 			; (ecx) = current maximum pages
	mov	edx,[PF_Min_File_Pages] 	; (edx) = minimum pages

;	If there is more free disk space than the absolute maximum, return
;	the absolute maximum as the current maximum.  If free disk space is
;	the limiting factor, return that.

	cmp	ecx,ebx 			; which is bigger?
	jb	pgsix				; if less disk space, jump
	mov	ecx,ebx 			; if more disk, use absolute max

pgsix:	ret
EndProc PageFile_Get_Size_Info

VxD_PAGEABLE_CODE_ENDS

VxD_PAGEABLE_DATA_SEG

PSC_Call_Table LABEL DWORD
	dd	offset32 PSC_Get_Version	; AX = 0000
	dd	offset32 PSC_Get_Paging_Info	; AX = 0001
	dd	offset32 PSC_Paging_Info_Change ; AX = 0002
	dd	offset32 PSC_Get_Cur_Tmp_Sz	; AX = 0003

MAX_PSCFUNC	EQU ($-PSC_Call_Table)/4

VxD_PAGEABLE_DATA_ENDS

VxD_PAGEABLE_CODE_SEG

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
BeginProc PageFile_Svc_Call, RARE

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

BeginProc PSC_Get_Version, RARE

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
;			(ignored by this version of pagefile)
;	Client_DS:SI -> 128 byte file name buffer for paging file name
;
;   EXIT:
;	Client_Carry = clear
;	    Client_DS:DI -> filled in (if approp) - never by this version
;	    Client_DS:SI -> filled in (if approp)
;	    Client_EAX = Flag Low two bits are the pager type currently in
;			 operation. NOTE that you must mask to these bits
;			 before checking value.
;
;		      bit 8000h is set if a corrupt permanent swapfile
;			was detected during pagefile init.
;			(never set by this version of pagefile)
;
;		      bit 4000h is set if the pager
;			is not paging to a full dragon device.
;			If this bit is set, it means that paging is going
;			to a non-dragon device (like a net drive), or is
;			going to the real mode mapper. This bit being clear
;			means paging access to the swap file does
;			not involve running any virtual mode code.
;
;
;		      Low two bits:
;
;			== 0 if paging is OFF
;			== 1 if paging using temporary swap file DS:SI is file
;			   name Client_ECX = MAX size of the paging file in
;			   bytes.  Client_EDX = MIN size of paging file in
;			   bytes (new to this version of pagefile).
;			== 2 if paging using permanent swap file non-Dragon
;			   DS:DI is name of SPART.PAR, DS:SI is name of
;			   partition file extracted from SPART.PAR
;			   Client_ECX = MAX size of the paging file in bytes.
;			   (never set by this version of pagefile)
;			== 3 if paging using permanent swap file AND Dragon
;			   DS:DI is name of SPART.PAR, DS:SI is name of
;			   partition file extracted from SPART.PAR
;			   Client_ECX = MAX size of the paging file in bytes.
;			   (never set by this version of pagefile)
;
;	Client_Carry = set
;	    Invalid buffer pointer passed in
;
;   USES:
;
;==============================================================================

BeginProc PSC_Get_Paging_Info, RARE

ifdef DEBUG

; code assumes client carry is clear on entry (cleared by PageFile_Svc_Call)

	TestMem [ebp.Client_EFLAGS], CF_Mask
	Debug_OutNZ	"PSC_Get_Paging_Info: client carry set"
endif

	xor	eax,eax
	cmp	[PF_Max_File_Pages], eax
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

	mov	eax,[PF_Min_File_Pages]
	shl	eax, 12
	mov	[ebp].Client_EDX,eax		; set min size in client edx

	mov	eax,1				; (eax) = 1 = temporary swap file
	cmp	[PF_Pager_Type],PF_Paging_HW	; paging through dragon?
	jz	PSC_GPISetDone			; jump if not
	or	ah,40h				; indicate smart pager

PSC_GPISetDone:
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

BeginProc PSC_Paging_Info_Change, RARE

;	This service used to be used to delete a permanent swap file, but
;	we don't need to do that any more.

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

BeginProc PSC_Get_Cur_Tmp_Sz, RARE

	xor	eax,eax
	mov	[ebp.Client_DX], ax		; Init return to 0
	mov	[ebp.Client_AX], ax
	cmp	[PF_Max_File_Pages], eax	; Paging off?
	je	short PSC_GTSzDone		; Yes, invalid call
	mov	eax, [PF_Cur_File_Pages]
	shl	eax, 12 			; Convert pages to bytes
	mov	[ebp.Client_AX],ax
	shr	eax,16
	mov	[ebp.Client_DX],ax
PSC_GTSzDone:
	ret

EndProc PSC_Get_Cur_Tmp_Sz

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

BeginProc PageFile_Int_24h, RARE

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

BeginProc PageFile_Int_23h, RARE

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

BeginProc PageFile_Map_Null, RARE

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

BeginProc PageFile_Set_Our_PSP, RARE

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

	VxDCall DOSMGR_MMGR_PSP_Change_Notifier

	mov	[ebp.Client_EBX],eax

	shl	eax, 4

	Begin_Touch_1st_Meg
	push	[eax.PDB_User_stack]
	End_Touch_1st_Meg

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

BeginProc PageFile_Set_Client_PSP, RARE

	Assert_Ints_Enabled

	mov	ax, [PF_Client_PSP]

	VxDCall DOSMGR_MMGR_PSP_Change_Notifier

	mov	[ebp.Client_EBX],eax
	mov	[ebp.Client_AH], 50h
	call	PageFile_Call_DOS


;
;   Restore the user stack dword on the Win386 PSP
;
	movzx	eax, [PF_Our_PSP]
	shl	eax, 4
	push	[PF_Save_User_Stack]

	Begin_Touch_1st_Meg
	pop	[eax.PDB_User_stack]
	End_Touch_1st_Meg

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

BeginProc PageFile_Call_DOS, RARE

        push    eax                     ; Part of HACK
	push	ecx
	push	edx
	mov	ecx, [PF_Orig_DOS_Vector]
	movzx	edx, cx
	shr	ecx, 10h
	VMMcall Build_Int_Stack_Frame
	ClrFlag [ebp.Client_EFlags], DF_Mask	; WORK AROUND BUSTED SOFTWARE

        VMMcall _EnterMustComplete

        ; Call the simulated interrupt
        VMMcall _ResumeExecMustComplete

        pushfd
        VMMcall _LeaveMustComplete      ; May never return, because of pending termination
        popfd

        jc      PFCD_Resume_Failed

	bt	[ebp.Client_Flags], CF_Bit
PFCD_Resume_Failed:
	pop	edx
	pop	ecx
        pop     eax
	ret

EndProc PageFile_Call_DOS

VxD_PAGEABLE_CODE_ENDS
VxD_LOCKED_CODE_SEG

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

	cmp	[PF_Pager_Type],PF_Paging_DOS	; dos pager?
	jnz	PF_TIV_Paging_OK		; if not, paging is always ok

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
;   PageFile_Read_Or_Write
;
;   DESCRIPTION:
;	This service will read or write a page from/to the paging file.
;
;   ENTRY:
;	EBX = pointer to first PageSwapBufferDesc node
;	Ints enabled
;
;   EXIT:
;	Fills in status in PageSwapBufferDesc
;
;   USES:
;	Flags
;
;==============================================================================
EndDoc

BeginProc PageFile_Read_Or_Write, Service

	Assert_Ints_Enabled
	cmp	[PF_Int21_Pager],0
	jnz	PageFile_Int21_Read_Or_Write	; jump if int 21 pager

	SaveReg <eax, ecx, edx, esi>

	mov	eax,0d600h			; (eax) = read/write
	.errnz	PF_Read_Data			; make sure they match
	.errnz	PF_Write_Data - 1		;   ifsmgr's values
	mov	al,[ebx].PS_BD_Cmd		; (al) = read or write
	xor	ecx,ecx
	mov	edx,[ebx].PS_BD_File_Page	; (edx) = page seek position
	mov	cl,[ebx].PS_BD_nPages		; (ecx) = number of pages in i/o
	shl	edx,PAGESHIFT			; (edx) = byte seek position
	shl	ecx,PAGESHIFT			; (ecx) = bytes in i/o
	mov	esi,[ebx].PS_BD_Buffer_Ptr	; (esi) = transfer address
	push	ebx
	mov	ebx,[PF_File_Handle]		; (ebx) = file handle
	VxDCall IFSMgr_Ring0_FileIO		; do the i/o
	Debug_OutC	"PageFile_Read_Or_Write: error #eax from file i/o"
	pop	ebx

ifdef DEBUG
; If there was no error, we better have transfered the whole amount
	jc	pfrw10				; skip check if error
	xor	ecx,ecx
	mov	cl,[ebx].PS_BD_nPages		; (ecx) = pages we tried to do
	shl	ecx,PAGESHIFT			; (ecx) = bytes we tried to do
	cmp	eax,ecx 			; did we get it right?
	Debug_OutNZ	"PageFile_Read_Or_Write: wrong amount transfered"
pfrw10:
endif
	.errnz	PFS_Success
	.errnz	PFS_Failure - 1
	mov	eax,0				; set error or success code
	adc	eax,0				;   from carry flag

	mov	[ebx].PS_BD_Status,al		; Inform caller of completion

	RestoreReg <esi, edx, ecx, eax>
	ret

EndProc PageFile_Read_Or_Write

BeginDoc
;******************************************************************************
;
;   PageFile_Int21_Read_Or_Write
;
;   DESCRIPTION:
;	Called by PageFile_Read_Or_Write to do the work if we are int 21
;	condition before calling Dragon.
;
;   ENTRY:
;	EBX = pointer to first PageSwapBufferDesc node
;	Ints enabled
;
;   EXIT:
;	Fills in status in PageSwapBufferDesc
;
;   USES:
;	Flags
;
;==============================================================================
EndDoc

BeginProc PageFile_Int21_Read_Or_Write, RARE

	pushad
	mov	di,PFS_Failure			; (di) = result code, assume fail
	mov	esi,ebx 			; (esi) = PageSwapBufferDescNode

;   Get parameters from structure
	mov	eax,[esi.PS_BD_Buffer_Ptr]	; (eax) = transfer address

;   Map the appropriate page into V86 address space

	VMMcall Get_Cur_VM_Handle		; (ebx) = current VM handle

	shr	eax,PAGESHIFT			; (eax) = page number to read/write
	VMMcall _LinMapIntoV86, <eax, ebx, [PF_Lin_Page_Num], 1, 0>
	test	eax, eax
	Debug_OutZ "PageFile ERROR:  Unable to map page"
	jz	PF_ROW_Invalid_Page

;   If we have a smartdrv we can turn off, do so here

	mov	ch,[esi.PS_BD_Cmd]		; (ch) = read or write command
	mov	eax,[PF_Cache_Lock_Ptr] 	; (eax) = pointer to lock
	test	eax, eax			; jump if no lock
	jz	SHORT PF_ROW_Nest_Exec
	Begin_Touch_1st_Meg
	inc	BYTE PTR [eax]
	End_Touch_1st_Meg

PF_ROW_Nest_Exec:
	Push_Client_State
	VMMcall Begin_Nest_V86_Exec

	mov	ebp,[ebx.CB_Client_Pointer]	; (ebp) = client regs

	mov	eax, [PF_V86_Stack_Seg_Off]
	mov	[ebp.Client_SP],ax
	shr	eax, 16
	mov	[ebp.Client_SS],ax

	inc	[PF_DOS_IO_Count]

;   Set PSP to Win386.Exe PSP to make DOS calls

	call	PageFile_Set_Our_PSP


;   Move file pointer to appropriate position

	mov	[ebp.Client_AX], 4200h
	mov	eax, [PF_File_Handle]
	mov	[ebp.Client_EBX],eax
	mov	eax, [esi.PS_BD_File_Page]	; (eax) = page # file offset
	shl	eax,PAGESHIFT			; (eax) = byte file offset
	mov	[ebp.Client_EDX],eax		; low word
	shr	eax, 16
	mov	[ebp.Client_ECX],eax		; high word
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

;   Everything worked, set success code

	mov	di,PFS_Success


;   Restore original PSP

PF_ROW_DOS_Error:

	call	PageFile_Set_Client_PSP

	dec	[PF_DOS_IO_Count]

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
PF_ROW_Invalid_Page:
	mov	eax,edi 			; (ax) = result code
	mov	[esi].PS_BD_Status,al		; Inform caller of completion

	popad
	ret

EndProc PageFile_Int21_Read_Or_Write,NOCHECK ; two Pop_Client_States

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

	Trace_Out "Swap file = ", No_EOL
	pushad
	mov	esi, OFFSET32 PF_Our_File_Name
	VMMcall Out_Debug_String
	popad
	Trace_Out " "
	mov	edi, [PF_Max_File_Pages]
	mov	eax, [PF_Cur_File_Pages]
	mov	edx, [PF_Min_File_Pages]
	Trace_Out "File contains #EAX pages of a possible #EDI"
	Trace_Out "Min size #EDX pages"
	Trace_Out " "
	mov	eax, [PF_File_Handle]
	Trace_Out "File handle	      = #EAX"
	Trace_Out " "

PF_DQ_Exit:
	clc
	ret

EndProc PageFile_Debug_Query

ENDIF

VxD_LOCKED_CODE_ENDS

	END PageFile_Real_Init
