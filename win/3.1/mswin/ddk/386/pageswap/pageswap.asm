PAGE 58,132
;******************************************************************************
TITLE PageSwap.Asm - Demand Paging Swap Device
;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1988-1990
;
;   Title:	PageSwap.Inc - Demand Paging Swap Device
;
;   Version:	2.10
;
;   Date:	09-Oct-1988
;
;   Author:	RAL
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
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
;   04-Oct-1990 JEM Split out file functions to PageFile device.
;   10-Oct-1990 JEM Removed liked lists for locating pages in swap file, now
;		    uses simpler first guess, sequential search method.
;   04-Mar-1991 RAL Added read ahead/write behind
;   30-Jul-1991 FJ  Rewrote scheduling of Write-Behinds and Read-Aheads
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
	INCLUDE PageFile.Inc
	INCLUDE OptTest.Inc
	.LIST

	Create_PageSwap_Service_Table EQU TRUE

	INCLUDE BlockDev.Inc
	INCLUDE PageSwap.Inc

	.ERRNZ	(SIZE_PS_BD_RESERVED - size BlockDev_Command_Block)

;******************************************************************************
;		V I R T U A L	D E V I C E   D E C L A R A T I O N
;******************************************************************************

Declare_Virtual_Device PageSwap, 2, 10, PageSwap_Control, PageSwap_Device_ID, \
		       PageSwap_Init_Order

;******************************************************************************
;			      E Q U A T E S
;******************************************************************************


PS_Read_Ahead	EQU	3
		.ERRE	PS_Read_Ahead-PF_Read_Data
		.ERRE	PS_Read_Ahead-PF_Write_Data

PS_Link_Head	EQU	010000h
PS_Link_Head_Bit EQU	16
PS_Link_Tail	EQU	0

;   Flag for page entries (used for LRU)

PSF_Recent_Swap_In  EQU     80000000h
PSF_Recent_Swap_In_Bit EQU  31

PSF_Next_Pg_Delta   EQU     0FF00000h	    ; Not really flags -- Byte 'o data
PSF_Next_Pg_Delta_Start EQU 20		    ; First bit in field

PSF_Entry_Flags     EQU     (PSF_Recent_Swap_In OR PSF_Next_Pg_Delta)


;   Flag equates for PS_Idle_Flags

PS_IF_Writing	    EQU     0002h
PS_IF_Writing_Bit   EQU     1
PS_IF_Test_Dirty    EQU     0004h
PS_IF_Test_Dirty_Bit EQU    2
PS_IF_Restart	    EQU     0008h
PS_IF_Restart_Bit   EQU     3
PS_IF_File_Full     EQU     0010h
PS_IF_File_Full_Bit EQU     4
PS_IF_Prepaging     EQU     0020h
PS_IF_Prepaging_Bit EQU     5
PS_IF_2nd_Pass	    EQU     0040h
PS_IF_2nd_Pass_Bit  EQU     6


PS_Idle_Dirty_Test_Time EQU 5000	    ; Test dirty count every 5 seconds


;
;   Buffer equates
;
PS_Default_Buffers  EQU     4		    ; Default to 4 buffers
PS_Max_Buffers	    EQU     32		    ; Maximum of 32 buffers allowed

PS_Max_Delta	    EQU     100 	    ; Max delta to next page for
					    ; prepage (must be <=127)

;******************************************************************************
;******************************************************************************


VxD_IDATA_SEG

EXTRN PS_Page_Buffers_Ini:BYTE

VxD_IDATA_ENDS

VxD_LOCKED_DATA_SEG
	ALIGN 4

PS_Idle_Flags		dd	0
PS_Last_Idle_Time	dd	0

PS_Reenter_Sem		dd	?

PS_Max_File_Pages	dd	0	; If 0 then paging is disabled
PS_Cur_File_Pages	dd	?

PS_Base_Lin_Page	dd	?

PS_Page_Entry_Base	dd	?
PS_Buff_Descr_Base	dd	?

PS_Free_Page_Count	dd	0	; # free pages in paging file
PS_Orig_Free_Count	dd	0	; Approx # PHYSICAL pgs avail for paging

PS_Next_Possible_Victim dd	0

PS_Last_Page_In 	dd	-1
PS_Last_Index_In	dd	0


PS_Num_Buffers		dw	0

PS_Pager_Type		db	0

			ALIGN	4

PS_Static_CB		PageSwapBufferDesc	<>	; static CB for immed.IO
PS_Free_List		dd	0
PS_Pending_WB_List	dd	0
PS_Submitted_RA_List	dd	0
PS_Locked_Buffer_List	dd	0

PS_Writes_Submitted	db	False	; True when chain of Writes in progress
PS_Wait_Cancel_CB	dd	0
PS_Immediate_IO_Sem	dd	?
PS_Buff_Avail_Sem	dd	?

;
;   Debugging data
;
IFDEF	DEBUG
	PUBLIC	PS_Pager_Type, PS_Buff_Descr_Base


			ALIGN	4

PS_Buff_Desc_Start	dd	?
PS_Buff_Desc_End	dd	?

PS_Deb_Struc_Base	dd	?

PS_Deb_Struc_Size	EQU 18	    ; 18 bytes for debug structure
PS_Deb_TimeIn_Offset	EQU 2
PS_Deb_TimeOut_Offset	EQU 6
PS_Deb_CountIn_Offset	EQU 10
PS_Deb_CountOut_Offset	EQU 12
PS_Deb_LastSwap_Offset	EQU 14

PS_Deb_Checksum_Size	EQU    400h	; 400h DWORDS for checksum

PS_DQ_Total_In		dd	0
PS_DQ_Read_In		dd	0
PS_DQ_Read_Time 	dd	0
PS_DQ_Total_Out 	dd	0
PS_DQ_Written_Out	dd	0
PS_DQ_Write_Time	dd	0
PS_DQ_PrePg_Total_Out	dd	0
PS_DQ_PrePg_Written_Out dd	0
PS_DQ_PrePg_Write_Time	dd	0

PS_Read_Ahead_Count	dd	0
PS_Read_Ahead_Hits	dd	0
PS_Read_Ahead_Canceled	dd	0

PS_Read_Race_Cond	dd	0
PS_Write_Race_Cond	dd	0


PS_Start_Time		dd	0

PS_DS_Idle		EQU	0
PS_DS_Reading		EQU	1
PS_DS_Writing		EQU	2

PS_Debug_State		dd	PS_DS_Idle


PS_DQ_State_Tab LABEL DWORD
	dd	OFFSET32 PS_DS0
	dd	OFFSET32 PS_DS1
	dd	OFFSET32 PS_DS2


PS_DS0	db  "Idle", 0
PS_DS1	db  "Paging in", 0
PS_DS2	db  "Paging out", 0

PS_Debug_Queuing	db	False
PS_Debug_Level2 	db	False

ENDIF



VxD_LOCKED_DATA_ENDS



;******************************************************************************
;		     D E B U G G I N G	 M A C R O S
;******************************************************************************

Assert_Buff_Desc MACRO BD_Ptr
	LOCAL BozoBuffer, OKBuffer
IFDEF DEBUG
	pushfd
	cmp	BD_Ptr, [PS_Buff_Desc_Start]
	jb	SHORT BozoBuffer
	cmp	BD_Ptr, [PS_Buff_Desc_End]
	jbe	SHORT OKBuffer
BozoBuffer:
	Debug_Out "PAGESWAP ERROR:  Invalid buffer pointer #&BD_Ptr"
OKBuffer:
	popfd
ENDIF
	ENDM


PS_Queue_Message MACRO S, V1, V2
	LOCAL	Str_Off
	LOCAL	No_Msg
IFDEF DEBUG

_LDATA SEGMENT
Str_Off db S, 0Ah,0Dh, 0
_LDATA ENDS

	pushfd
	cmp	[PS_Debug_Queuing], True
	jne	SHORT No_Msg

	push	esi
IFNB <V1>
    IF	TYPE V1 GT 0
	push	dword ptr V1
    ELSE
	push	V1
    ENDIF
ELSE
	push	eax		; dummy value1
ENDIF
IFNB <V2>
    IF	TYPE V2 GT 0
	push	dword ptr V2
    ELSE
	push	V2
    ENDIF
ELSE
	push	ebx		; dummy value2
ENDIF
	mov	esi, OFFSET32 Str_Off
	VMMcall Queue_Debug_String
	pop	esi

No_Msg:
	popfd
ENDIF
	ENDM


PS_Queue_Level2 MACRO S, V1, V2
	LOCAL	Str_Off
	LOCAL	No_Msg
IFDEF DEBUG

_LDATA SEGMENT
Str_Off db S, 0Ah,0Dh, 0
_LDATA ENDS

	pushfd
	cmp	[PS_Debug_Level2], True
	jne	SHORT No_Msg

	push	esi
IFNB <V1>
    IF	TYPE V1 GT 0
	push	dword ptr V1
    ELSE
	push	V1
    ENDIF
ELSE
	push	eax		; dummy value1
ENDIF
IFNB <V2>
    IF	TYPE V2 GT 0
	push	dword ptr V2
    ELSE
	push	V2
    ENDIF
ELSE
	push	ebx		; dummy value2
ENDIF
	mov	esi, OFFSET32 Str_Off
	VMMcall Queue_Debug_String
	pop	esi

No_Msg:
	popfd
ENDIF
	ENDM

;******************************************************************************

	.SALL


;******************************************************************************
;		  R E A L   M O D E   I N I T	C O D E
;******************************************************************************

;******************************************************************************
;
;   PageSwap_Real_Init
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

VxD_REAL_INIT_SEG

BeginProc PageSwap_Real_Init

;   If another pageswap device is loaded then don't load -- Just abort our load

	test	bx, Duplicate_From_INT2F OR Duplicate_Device_ID
	jnz	SHORT PageSwap_RI_Abort_Load

	xor	edx, edx
	xor	bx, bx
	xor	si, si
	mov	ax, Device_Load_Ok
	ret

PageSwap_RI_Abort_Load:
	xor	bx, bx
	xor	si, si
	mov	ax, Abort_Device_Load + No_Fail_Message
	ret

EndProc PageSwap_Real_Init

VxD_REAL_INIT_ENDS



;******************************************************************************
;	       P R O T E C T E D   M O D E   I N I T   C O D E
;******************************************************************************

VxD_ICODE_SEG

;******************************************************************************
;
;   PageSwap_Sys_Critical_Init
;
;   DESCRIPTION:
;
;   ENTRY:
;	EBX = System VM handle
;
;   EXIT:
;	Carry clear
;
;   USES:
;	EAX, EDI
;
;==============================================================================

BeginProc PageSwap_Sys_Critical_Init

;   Get information for overcommit.  We're attempting to calculate the
;   amount of PHYSICAL memory available for paging--something that isn't
;   directly reported by _GetDemandPageInfo.

	.ERRNZ DemandInfoStruc MOD 4
	sub	esp, SIZE DemandInfoStruc
	mov	edi, esp
	VMMcall _GetDemandPageInfo, <edi, 0>

	mov	eax, [edi.DILin_Total_Count]
	sub	eax, [edi.DILin_Total_Free]
	add	eax, [edi.DIFree_Count]
	add	eax, 40h			; Force overcommit fudge factor
	mov	[PS_Orig_Free_Count], eax

;   Get the base page of the linear address space for allocating file pages

	mov	eax, [edi.DILinear_Base_Addr]	; We'll need this lager, so
	shr	eax, 12 			;   might as well save it
	mov	[PS_Base_Lin_Page], eax 	;   while we already have it

	add	esp, SIZE DemandInfoStruc

IFDEF	DEBUG
	or	eax,eax
	jnz	SHORT PS_SCI_Base_Ok
	Debug_Out "Paging Linear Base == 0!  Will break free page search!"
PS_SCI_Base_Ok:
ENDIF

	clc
	ret

EndProc PageSwap_Sys_Critical_Init


;******************************************************************************
;
;   PageSwap_Device_Init
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

BeginProc PageSwap_Device_Init

	VxDcall PageFile_Get_Version		; Verify PageFile device is
	jc	PS_Init_No_Paging		;   loaded

	VxDcall PageFile_Init_File		; Initialize the paging file
	test	ebx,ebx 			; returns max file pages in EBX
	jz	PS_Init_No_Paging		;     and cur file pages in EAX

	mov	[PS_Max_File_Pages],ebx
	mov	[PS_Cur_File_Pages],eax

	mov	[PS_Free_Page_Count], eax	; All file pages free initially


;   Allocate enough memory for our data structures.  Each page requires 4 bytes.
;   Allocation is based on max file size, not current.

	lea	edx, [ebx*4]			; EDX = # entries * 4

IFDEF DEBUG

; For debug, 2 extra bytes per page are allocated to hold page checksum values.
; Total bytes per entry = 6.

	lea	edx, [edx][ebx*2]		; EDX = # entries * 6
ENDIF

	add	edx, 0FFFh			; Round up to next page
	shr	edx, 12 			; EDX = # pages needed
	VMMcall _PageAllocate,<edx,PG_SYS,0,0,0,0,0,<PageLocked + PageZeroInit>>
	test	eax, eax
	jz	PS_Init_No_Paging
	mov	[PS_Page_Entry_Base], edx

IFDEF DEBUG
	push	ebx
	lea	ebx, [edx][ebx*4]		; EBX = base + entries * 4
	mov	[PS_Deb_Struc_Base], ebx
	pop	ebx
ENDIF


;   Create a semaphore to prevent reentering pageswap operations

	mov	ecx, 1				; One token
	VMMcall Create_Semaphore		; Get a semaphore
	jc	PS_Cant_Get_Semaphore		; If failed then no paging
	mov	[PS_Reenter_Sem], eax

;   Create another semaphore for waiting for immediate IO to complete
;   (used both with and without smart pager)

	xor	ecx, ecx
	VMMcall Create_Semaphore
	jc	PS_Init_Skip_Smart_Init
	mov	[PS_Immediate_IO_Sem], eax

;   Now that we (and PageFile) have mostly initialized, call PageFile again to
;   find out what type of paging is supported.

	VxDcall PageFile_Get_Version
	mov	[PS_Pager_Type], bl

	cmp	bl, PF_Paging_HW
	jb	PS_Init_Skip_Smart_Init

;   Read SYSTEM.INI to see how many async page buffers to use.

	mov	eax, PS_Default_Buffers 	    ; Default # 'o buffers
	xor	esi, esi			    ; 386 Enhanced mode section
	mov	edi, OFFSET32 PS_Page_Buffers_Ini   ; This keyword
	VMMcall Get_Profile_Decimal_Int 	    ; EAX = # buffers

	cmp	eax, PS_Max_Buffers		; Lets get real!
	jb	SHORT PS_Init_Reasonable
	mov	eax, PS_Max_Buffers

PS_Init_Reasonable:
	mov	[PS_Num_Buffers], ax

	or	eax, eax			; Skip buffer init if unwanted
	jz	PS_Init_Skip_Smart_Init

;   Create a semaphore for available (free-list) buffers. Initialize it to
;   the number of buffers

	mov	ecx, eax
	VMMcall Create_Semaphore
	jc	PS_Init_Skip_Smart_Init
	mov	[PS_Buff_Avail_Sem], eax

;   Allocate space for the buffer descriptors from the heap

	movzx	eax, [PS_Num_Buffers]
	imul	eax, SIZE PageSwapBufferDesc

	VMMcall _HeapAllocate,<eax,HeapZeroInit>
	or	eax, eax
	jz	DEBFAR PS_Init_Skip_Smart_Init	; NOTE: Semaphore still around

	mov	[PS_Free_List], eax

IFDEF DEBUG
	mov	[PS_Buff_Desc_Start], eax
ENDIF



;   Allocate the page buffers

	movzx	ecx, [PS_Num_Buffers]
	VMMcall _PageAllocate,<ecx,PG_SYS,0,0,0,0,0,PageLocked>
	test	eax, eax
	jz	SHORT PS_Cant_Get_Buffers

;   Initialize buffer descriptors

	mov	ebx, OFFSET32 PS_Static_CB
	mov	[ebx.PS_BD_Next], 0		; no next ptr in static CB

	mov	ebx, [PS_Free_List]
	mov	edi, OFFSET32 PageSwap_CallBack

PS_Init_Desc_Loop:

	mov	[ebx.PS_BD_Buffer_Ptr], edx
	mov	[ebx.PS_BD_Page_Number], PS_BDP_Invalid_Data
	mov	[ebx.PS_BD_List], PS_BDL_Free
	mov	[ebx.PS_BD_Call_Back], edi
	lea	esi, [ebx.PS_BD_Next]

	add	ebx, SIZE PageSwapBufferDesc
	mov	[esi], ebx			; link into free list
	add	edx, 1000h
	loopd	PS_Init_Desc_Loop

	mov	DWORD PTR [esi], 0		; NULL ptr at end of list

IFDEF DEBUG
	mov	[PS_Buff_Desc_End], esi
ENDIF

	jmp	SHORT PS_Init_Finish


PS_Cant_Get_Buffers:

	VMMcall _HeapFree,<PS_Buff_Descr_Base,0> ; Clean-up the descriptors

PS_Init_Skip_Smart_Init:

	xor	eax, eax			; So there is no confusion
	mov	[PS_Num_Buffers], ax

PS_Init_Finish:

;   Have time-slicer call us back when all VMs are idle so we can pre-page.

	mov	esi, OFFSET32 PageSwap_Idle
	VMMcall Call_When_Idle

PS_Init_Exit:

	clc
	ret


;   Paging could not be intialized for some reason.

PS_Cant_Get_Semaphore:

	call	PageSwap_System_Exit		; Clean up after ourselves

PS_Init_No_Paging:

	mov	[PS_Max_File_Pages], 0		; Make sure it's really off
	jmp	SHORT PS_Init_Exit

EndProc PageSwap_Device_Init


VxD_ICODE_ENDS

VxD_LOCKED_CODE_SEG

;******************************************************************************
;
;   PageSwap_Control
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

BeginProc PageSwap_Control

	Control_Dispatch Sys_Critical_Init, PageSwap_Sys_Critical_Init
	Control_Dispatch Device_Init,	    PageSwap_Device_Init
	Control_Dispatch System_Exit,	    <SHORT PageSwap_System_Exit>
	Control_Dispatch VM_Resume,	    PageSwap_Test_Dirty_Count
	Control_Dispatch VM_Suspend,	    PageSwap_Test_Dirty_Count

IFDEF DEBUG
	Control_Dispatch Debug_Query, PageSwap_Debug_Query
ENDIF

	clc
	ret

EndProc PageSwap_Control


;******************************************************************************
;
;   PageSwap_System_Exit
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

BeginProc PageSwap_System_Exit

	cmp	[PS_Max_File_Pages], 0		; Q: Is paging enabled?
	je	SHORT PS_SE_Exit		;    N: Quit right away

	VxDcall PageFile_Clean_Up		; Let PageFile clean up the file

PS_SE_Exit:
	clc
	ret

EndProc PageSwap_System_Exit


;******************************************************************************
;			     S E R V I C E S
;******************************************************************************


BeginDoc
;******************************************************************************
;
;   PageSwap_Get_Version
;
;   DESCRIPTION:
;	This service returns the PageSwap device version number, the type
;	of paging in effect, and the maximum size of the paging file in
;	pages.
;
;   ENTRY:
;	No entry parameters
;
;   EXIT:
;	EAX = Version number (0 if not installed)
;	 BL  = Pager Type (1 = No pager, 2 = DOS pager, 3 = Direct hardware pg)
;	ECX = Maximum size of swap file in pages
;	Carry flag clear if page-swapper device installed
;
;	Note: The max swap file size will not be valid until after Device Init!
;
;   USES: Flags
;
;==============================================================================
EndDoc

BeginProc PageSwap_Get_Version, Service

	VxDcall PageFile_Get_Version		; Pass through to PageFile to
						;   setup paging type in BL

	%OUT Change the version number(s) to 3.1 soon

	mov	eax, 200h			; Set PageSwap version #
	mov	ecx, [PS_Max_File_Pages]	; Return max paging file size

	clc
	ret

EndProc PageSwap_Get_Version


BeginDoc
;******************************************************************************
;
;   PageSwap_Test_Create
;
;   DESCRIPTION:
;	This service is called by the memory manager to determine if it can
;	satisfy a memory request.  For this version of pageswap, the formula
;	used to determine this is:
;
;	IF (TotalLinSpace-FreeLinSpace)+ReqestSize <= FileSize+PhysMemory THEN
;	    Request can be satisfied
;	ELSE
;	    Memory can not be allocated
;
;   ENTRY:
;	ECX = Page count
;	EAX = Flags
;
;   EXIT:
;	If carry clear then OK to create memory handle/realloc
;
;   USES:
;	Flags
;
;==============================================================================
EndDoc

BeginProc PageSwap_Test_Create, Service

	bt	eax, PS_Fixed_Page_Bit		; Always success on fixed
	jc	SHORT PS_TC_Quick_Exit		;   page create

	pushad

;   Make sure we have grown the file to a proper size

	mov	ebx, [PS_Free_Page_Count]
	sub	ebx, ecx
	jge	SHORT PS_TC_Grow_Done
	push	ecx
	mov	ecx, ebx
	neg	ecx
	call	PageSwap_Grow_File
	pop	ecx
PS_TC_Grow_Done:


;   Test for

	.ERRNZ DemandInfoStruc MOD 4
	sub	esp, SIZE DemandInfoStruc
	mov	edi, esp
	mov	esi, ecx			; ESI = Size of request
	VMMcall _GetDemandPageInfo, <edi, 0>
	mov	eax, [edi.DILin_Total_Count]
	sub	eax, [edi.DILin_Total_Free]
	add	eax, esi
	mov	ecx, [PS_Cur_File_Pages]
	add	ecx, [PS_Orig_Free_Count]
	add	esp, SIZE DemandInfoStruc

	sub	ecx, eax			; Carry if EAX > ECX
	popad
	ret

PS_TC_Quick_Exit:
	clc
	ret

EndProc PageSwap_Test_Create


;******************************************************************************
;
;   PageSwap_Create
;
;   DESCRIPTION:
;
;   ENTRY:
;	EAX = Flags
;	ESI = Memory handle (ignored)
;	EDX = New linear start
;	ECX = New size
;	EBX = Old linear start (0 if none)  <-- These parameters are used
;	EDI = Old size (0 if new handle)    <-- when a handle moves
;
;   EXIT:
;	None.
;
;   USES:
;	Flags
;
;==============================================================================

BeginProc PageSwap_Create, Service

	cmp	[PS_Max_File_Pages], 0		; Q: Paging on?
	je	DEBFAR PSC_Exit_Out		;    N: Ignore this call

	bt	eax, PS_Fixed_Page_Bit		; Nothing to do if fixed
	jc	SHORT PSC_Exit_Out		;   page allocation

	pushad

	shr	edx, 12 			; Convert linear addresses to
	shr	ebx, 12 			; linear page numbers

;   Make sure we have grown the file to a proper size

	push	ecx
	lea	ecx, [edx+ecx-1]
	sub	ecx, [PS_Base_Lin_Page]
	sub	ecx, [PS_Cur_File_Pages]	; ECX = # extra pages needed
	jle	SHORT PSC_Grow_Done		; Signed compare is correct!
	call	PageSwap_Grow_File
PSC_Grow_Done:
	pop	ecx

;   Test for

	test	ebx, ebx
	jz	SHORT PSC_New_Handle

;   Handle is moving or changing size.	 If shrinking then free pages past
;   end of new size.

	cmp	ecx, edi			; Q: Is handle shrinking
	jae	SHORT PSC_Move_Pages		;    N: Move pages around
						;    Y: Free ones at end
	pushad

	add	edx, edi			; Add old size
	sub	ecx, edi			; ECX = -Delta of shrink
	neg	ecx				; ECX = # pages to shrink

	sub	edx, ecx			; 1st page to free

	call	PageSwap_Free_Page_Range	; Free the extra pages

	popad

PSC_Move_Pages:
	sub	edx, ebx			; Q: Has lin address changed?
	jz	SHORT PSC_Exit			;    N: Nothing more to do
						;    Y: EDX = Delta of move
	mov	ecx,edi 			; Count = old size
	xchg	ebx,edx				; EBX=delta move,EDX=old page

PSC_Move_Loop:
	call	PageSwap_Find_Page
	jc	SHORT PSC_Next_Page

;   EDI -> Page node

	push	edx
	add	edx, ebx
	mov	eax, [PS_Page_Entry_Base]
	mov	esi, DWORD PTR [eax][edi*4]
	and	esi, PSF_Entry_Flags
	or	esi, edx
	mov	DWORD PTR [eax][edi*4], esi
	pop	edx

PSC_Next_Page:
	inc	edx				; Next page
	loopd	PSC_Move_Loop
	jmp	SHORT PSC_Exit


PSC_New_Handle:
PSC_Exit:
	popad

PSC_Exit_Out:
	clc
	ret

EndProc PageSwap_Create


;******************************************************************************
;
;   PageSwap_Destroy
;
;   DESCRIPTION:
;
;   ENTRY:
;	ESI = Memory handle (NOT USED)
;	EDX = Linear start
;	ECX = Number of pages
;	EAX = Flags
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc PageSwap_Destroy, Service

	bt	eax, PS_Fixed_Page_Bit		; Nothing to do if fixed page
	jc	SHORT PSD_Exit

	jecxz	PSD_Exit			; Zero page allocation?

	cmp	[PS_Max_File_Pages], 0		; Paging enabled?
	je	SHORT PSD_Exit

	pushad

	shr	edx, 12
	call	PageSwap_Free_Page_Range

	popad

PSD_Exit:
	clc
	ret

EndProc PageSwap_Destroy


;******************************************************************************
;
;   PageSwap_Free_Page_Range
;
;   DESCRIPTION:
;
;   ENTRY:
;	EDX = Starting page number
;	ECX = Number of pages to free
;
;   EXIT:
;
;   USES:
;	Can trash EAX, EBX, ECX, EDX, ESI, EDI, Flags
;
;==============================================================================

BeginProc PageSwap_Free_Page_Range

	lea	ebx, [edx+ecx-1]		; Last page to free
	mov	ecx, [PS_Max_File_Pages]	; # table entries to examine
	mov	edi, [PS_Page_Entry_Base]	; EDI -> Base of table entries

PS_FPR_Loop:
	mov	eax, DWORD PTR [edi]
	and	eax, NOT PSF_Entry_Flags	; EAX = Page number

	cmp	eax, edx			; Q: Lower than 1st page?
	jb	SHORT PS_FPR_Next		;    Y: Ignore it

	cmp	eax, ebx			; Q: Above last page?
	ja	SHORT PS_FPR_Next		;    Y: Ignore it
						;    N: Free it
	mov	DWORD PTR [edi], 0
	inc	[PS_Free_Page_Count]

PS_FPR_Next:
	add	edi, 4
	loopd	PS_FPR_Loop

	ret

EndProc PageSwap_Free_Page_Range



;******************************************************************************
;
;   PageSwap_In
;
;   DESCRIPTION:
;
;   ENTRY:
;	EDX = Linear address of page to read in (not present page)
;	EDI = Linear address of memory that pageswap can touch
;	EAX = Flags
;
;   EXIT:
;	If carry set then error reading page
;
;   USES:
;	Flags
;
;==============================================================================

BeginProc PageSwap_In, Service

	cmp	[PS_Max_File_Pages], 0		; Paging enabled?
	jz	PSI_Quick_Exit

;   Nothing to do if zero init, fixed page, or never dirty

	test	eax, (PS_Zero_Init_Mask OR PS_Fixed_Page_Mask)
	jnz	PSI_Quick_Exit

	bt	eax, PS_Ever_Dirty_Bit
	jnc	PSI_Quick_Exit

	pushad

	push	eax				; Prevent reentry of swap in
	mov	eax, [PS_Reenter_Sem]		;   code
	mov	ecx, (Block_Svc_If_Ints_Locked OR Block_Enable_Ints)
	VMMcall Wait_Semaphore
	pop	eax

IFDEF	DEBUG
	pushad
	mov	[PS_Debug_State], PS_DS_Reading
	VMMcall Get_System_Time
	mov	[PS_Start_Time], eax
	inc	[PS_DQ_Total_In]
	popad
ENDIF

	mov	esi, edi			; ESI = Linear addr we can touch

	shr	edx, 12 			; EDX = Page NUMBER to read

	call	PageSwap_Find_Page		; Q: Is page in file?

IFDEF	DEBUG
	jnc	SHORT PSI_Found_Page
	Debug_Out "PageSwap_In called for page not in file!"
PSI_Found_Page:
ENDIF
	jc	PSI_Error_Exit			;    N: Not OK for 3.10!

IFDEF DEBUG
	inc	[PS_DQ_Read_In]
ENDIF

	mov	ch, PF_Read_Data		; Want to read data
	call	PageSwap_Read_Or_Write		; Try to do it
	jc	PSI_Error_Exit			; If error then return error
						;   (CY already set)

;   In the debugging version we checksum the page to make sure we read
;   the right data.

IFDEF	DEBUG
	pushad					; Don't change ANYTHING!
	mov	ecx, PS_Deb_Checksum_Size	; Only check this many dwords
	xor	eax, eax			; Start with 0 checksum
PSI_Checksum:
	add	eax, DWORD PTR [esi][ecx*2]
	loopd	PSI_Checksum
	mov	esi, [PS_Deb_Struc_Base]
	cmp	WORD PTR [esi][edi*2], ax	; base + EDI * 2
	je	SHORT PSI_Checksums_Matched
	mov	bx, WORD PTR [esi][edi*2]
	lea	ecx, [esi][edi*2]
	Trace_Out "ERROR:  Checksum on PageSwap_In did not match out checksum!"
	Trace_Out "        Lin page = #EDX, PageSwap file entry =#DI"
	Debug_Out "        Checksum loc = #ECX, is #AX, should be #BX"
PSI_Checksums_Matched:
	popad
ENDIF


;   Mark the page entry as swapped in.

	mov	eax, [PS_Page_Entry_Base]
	mov	ecx, DWORD PTR [eax][edi*4]	; Get current entry
	and	ecx, PSF_Entry_Flags
	or	ecx, PSF_Recent_Swap_In
	or	ecx, edx
	mov	[eax][edi*4], ecx

;   Get smart!	Try to read the next page in anticipation...

	cmp	[PS_Pager_Type], PF_Paging_HW	; Direct to hardware pager?
	jb	SHORT PSI_Exit			;   if not, don't read ahead

	mov	ebx, edx
	xchg	ebx, [PS_Last_Page_In]
	xchg	edi, [PS_Last_Index_In]

	mov	esi, DWORD PTR [eax][edi*4]
	and	esi, NOT PSF_Entry_Flags
	cmp	esi, ebx			; Q: Entry still point to page?
	jne	SHORT PSI_Read_Ahead		;    N: Forget it

	neg	esi
	add	esi, edx			; ESI = Delta from prev to this
	cmp	esi, -PS_Max_Delta
	jl	SHORT PSI_Too_Darn_Far
	cmp	esi, PS_Max_Delta
	jle	SHORT PSI_Set_Delta
PSI_Too_Darn_Far:
	xor	esi, esi
PSI_Set_Delta:
	and	DWORD PTR [eax][edi*4], NOT PSF_Next_Pg_Delta
	shl	esi, PSF_Next_Pg_Delta_Start
	and	esi, PSF_Next_Pg_Delta
	or	DWORD PTR [eax][edi*4], esi


PSI_Read_Ahead:
	and	ecx, PSF_Next_Pg_Delta
	jz	SHORT PSI_Exit
	shr	ecx, PSF_Next_Pg_Delta_Start
	movsx	ecx, cl

	add	edx, ecx			; EDX -> Most likely next pg
	push	edx
	mov	ecx, edx
	call	PageSwap_Test_Page_Present	; Q: Is page already present?
	pop	edx
	jnz	SHORT PSI_Exit			;    Y: Nothing to do

	call	PageSwap_Find_Page
	jc	SHORT PSI_Exit

	mov	ch, PS_Read_Ahead		; Yes, try to read it in
	call	PageSwap_Read_Or_Write		;   during background

PSI_Exit:
	clc

PSI_Error_Exit:

IFDEF DEBUG
	pushfd
	pushad
	VMMcall Get_System_Time
	sub	eax, [PS_Start_Time]
	add	[PS_DQ_Read_Time], eax
	mov	[PS_Debug_State], PS_DS_Idle
	popad
	popfd
ENDIF
	pushfd
	mov	eax, [PS_Reenter_Sem]		; Okay to reenter swap in now
	VMMcall Signal_Semaphore
	popfd

	popad
	ret

PSI_Quick_Exit:
	clc
	ret

EndProc PageSwap_In



;******************************************************************************
;
;   PageSwap_Out
;
;   DESCRIPTION:
;
;   ENTRY:
;	EDX = Linear address of page to write out
;	ESI = Linear address of memory to write from
;	EAX = Flags
;
;   EXIT:
;	If carry set then error writing page
;
;   USES:
;	Flags
;
;==============================================================================


BeginProc PageSwap_Out, Service

	cmp	[PS_Max_File_Pages], 0		; Paging enabled?
	jz	DEBFAR PSO_Error_Out

;   Nothing to do if page is fixed or never dirty

	bt	eax, PS_Fixed_Page_Bit
	jc	DEBFAR PSO_Success_Out

	bt	eax, PS_Ever_Dirty_Bit
	jnc	DEBFAR PSO_Success_Out

	or	[PS_Idle_Flags], PS_IF_Test_Dirty	; Restart check for
							; dirty pgs at idle
	pushad

	push	eax				; Prevent reentry of swap out
	mov	eax, [PS_Reenter_Sem]		;   code
	mov	ecx, (Block_Svc_If_Ints_Locked OR Block_Enable_Ints)
	VMMcall Wait_Semaphore
	pop	eax

IFDEF DEBUG
	pushad
	mov	[PS_Debug_State], PS_DS_Writing
	VMMcall Get_System_Time
	mov	[PS_Start_Time], eax
	test	[PS_Idle_Flags], PS_IF_Prepaging
	jz	SHORT PSO_DB1_Not_PrePg
	inc	[PS_DQ_PrePg_Total_Out]
	jmp	SHORT PSO_DB1_Done

PSO_DB1_Not_PrePg:
	inc	[PS_DQ_Total_Out]
PSO_DB1_Done:
	popad
ENDIF

	shr	edx, 12 			; EDX = Page NUMBER to write

	call	PageSwap_Find_Page
	jnc	SHORT PSO_Got_Page

IFDEF DEBUG
	bt	eax, PS_Dirty_Bit
	jc	SHORT PSO_not_clean
	Trace_Out "Optimize this?  swap out clean page needs free page!"
PSO_not_clean:
ENDIF

	call	PageSwap_Find_Free_Page
	jc	DEBFAR PSO_Error
	jmp	SHORT PSO_Write_Page

;
;   The page is in the file, but if it is dirty we still need to write
;   it again.  If it's not dirty then we are done.
;
PSO_Got_Page:
	.ERRNZ P_DIRTY-40h
	bt	eax, PS_Dirty_Bit		; Q: Is the page dirty?
	jnc	SHORT PSO_Success		;    N: YAHOO!	It's out!

PSO_Write_Page:

IFDEF DEBUG
	test	[PS_Idle_Flags], PS_IF_Prepaging
	jz	SHORT PSO_DB2_Not_PrePg
	inc	[PS_DQ_PrePg_Written_Out]
	jmp	SHORT PSO_DB2_Done

PSO_DB2_Not_PrePg:
	inc	[PS_DQ_Written_Out]
PSO_DB2_Done:
ENDIF


	mov	ch, PF_Write_Data
	call	PageSwap_Read_Or_Write
	jc	SHORT PSO_Error


;   In the debugging version we checksum the page so that we can verify the
;   data when the page is read in again.

IFDEF DEBUG
	pushad					; Don't change ANYTHING!
	mov	ecx, PS_Deb_Checksum_Size	; Only check this many dwords
	xor	eax, eax			; Start with 0 checksum
PSO_Checksum:
	add	eax, DWORD PTR [esi][ecx*2]
	loopd	PSO_Checksum
	mov	esi, [PS_Deb_Struc_Base]
	mov	WORD PTR [esi][edi*2], ax	; base + EDI * 2
	popad
ENDIF


;   Finished!

PSO_Success:
	mov	eax, [PS_Reenter_Sem]
	VMMcall Signal_Semaphore
	popad

IFDEF DEBUG
	pushad
	VMMcall Get_System_Time
	sub	eax, [PS_Start_Time]
	test	[PS_Idle_Flags], PS_IF_Prepaging
	jz	SHORT PSO_DB3_Not_PrePg
	add	[PS_DQ_PrePg_Write_Time], eax
	jmp	SHORT PSO_DB3_Done

PSO_DB3_Not_PrePg:
	add	[PS_DQ_Write_Time], eax
PSO_DB3_Done:
	mov	[PS_Debug_State], PS_DS_Idle
	popad
ENDIF

PSO_Success_Out:
	clc
	ret

PSO_Error:
	Debug_Out "ERROR PAGING OUT PAGE #EDX"
	mov	eax, [PS_Reenter_Sem]
	VMMcall Signal_Semaphore
	popad

IFDEF DEBUG
	pushad
	VMMcall Get_System_Time
	sub	eax, [PS_Start_Time]
	test	[PS_Idle_Flags], PS_IF_Prepaging
	jz	SHORT PSO_DB4_Not_PrePg
	add	[PS_DQ_PrePg_Write_Time], eax
	jmp	SHORT PSO_DB4_Done

PSO_DB4_Not_PrePg:
	add	[PS_DQ_Write_Time], eax
PSO_DB4_Done:
	mov	[PS_Debug_State], PS_DS_Idle
	popad
ENDIF

PSO_Error_Out:
	stc
	ret

EndProc PageSwap_Out


;******************************************************************************
;
;   PageSwap_Find_Free_Page
;
;   DESCRIPTION:
;	Code to locate position in swap file for pages that are currently
;	not in the file.
;
;   ENTRY:
;	EDX = Page number to locate slot for
;
;   EXIT:
;	if carry clear then successful
;	EDI = Free page slot in file
;
;	If carry set then unable to locate slot for page
;
;   USES:
;	Flags
;
;==============================================================================


BeginProc PageSwap_Find_Free_Page

	pushad

	xor	eax, eax			; Calculate 'best' position
	xchg	eax, edx			;   for page
	sub	eax, [PS_Base_Lin_Page]
	idiv	[PS_Max_File_Pages]		; EDX = page num MOD file size

	mov	eax, [PS_Page_Entry_Base]	; Is 'best' position available?
	cmp	DWORD PTR [eax][edx*4], 0	;   if so, go setup the page
	je	DEBFAR PS_Add_New_Page


;   Page can't be put in 'best' position, find first free page after 'best'.

	mov	ebx, [PS_Max_File_Pages]	; EBX = # pages in paging file
	mov	ecx, ebx
	dec	ecx				; ECX = max # entries to check

IFDEF DEBUG
	cmp	ebx, [PS_Cur_File_Pages]
	jz	SHORT PS_FF_maxed
	Debug_Out "PageSwap_FF: Best isn't with max != cur"
PS_FF_maxed:
ENDIF

PS_Find_Free_Loop:

	inc	edx				; Bump page entry index,
	cmp	edx, ebx			;   watch for wrap
	jb	SHORT PS_FFL_No_Wrap
	xor	edx, edx
PS_FFL_No_Wrap:

	cmp	DWORD PTR [eax][edx*4], 0	; Is this page free?
	je	SHORT PS_Add_New_Page		;   yes, go use it...

	loopd	PS_Find_Free_Loop


;   No free pages, locate a victim page to replace.  A victim page is
;   one present in both the paging file and physical memory.  If it is
;   in memory, it doesn't need to be in the file.

PS_Find_Victim_Page:

	or	[PS_Idle_Flags], PS_IF_File_Full

;   Scan the page entry list twice (worse case).  The first time we'll reject
;   pages with the PSF_Recent_Swap_In bit set, the 2nd time we'll take them.

	mov	ecx, [PS_Max_File_Pages]	; Scan this many pages

	add	ecx, ecx			; TWICE!

	mov	edi, [PS_Next_Possible_Victim]	; EDI = Index of next victim
	mov	esi, [PS_Page_Entry_Base]	; ESI -> Page entry base

PS_Find_Victim_Loop:
	btr	DWORD PTR [esi][edi*4], PSF_Recent_Swap_In_Bit	; test & reset
	jc	SHORT PS_Find_Not_This_One

	push	ecx
	mov	ecx, DWORD PTR [esi][edi*4]
	and	ecx, NOT PSF_Entry_Flags
	call	PageSwap_Test_Page_Present
	pop	ecx
	jnz	SHORT PS_Found_Victim

PS_Find_Not_This_One:
	inc	edi
	cmp	edi, [PS_Cur_File_Pages]
	jb	SHORT PS_Find_Try_Next
	xor	edi, edi
PS_Find_Try_Next:
	loopd	PS_Find_Victim_Loop

;   ERROR:  Could not locate a victim page!  Return error.

	Debug_Out "ERROR:  Could not locate a victim page for replacement in swapfile"
	popad
	stc
	ret


PS_Found_Victim:
	lea	edx, [edi+1]
	cmp	edx, [PS_Cur_File_Pages]
	jb	SHORT PS_Set_Next_Victim
	xor	edx, edx
PS_Set_Next_Victim:
	mov	[PS_Next_Possible_Victim], edx

	mov	eax, esi			; EAX = PS_Page_Entry_Base
	mov	edx, edi			; EDX = free page index
	jmp	SHORT PS_Took_A_Victim		; Don't dec free page count


;   Located a free page, now use it.
;	EAX = PS_Page_Entry_Base
;	EDX = page entry index

PS_Add_New_Page:

IFDEF DEBUG
	cmp	edx, [PS_Cur_File_Pages]
	jb	SHORT PS_FF_range_ok
	Debug_Out "PS_FF returning page > Cur_File_Pages!"
PS_FF_range_ok:
ENDIF

	dec	[PS_Free_Page_Count]		; One less free page

PS_Took_A_Victim:
	mov	ebx, [esp.Pushad_EDX]		; Put original page # into
	mov	[eax][edx*4], ebx		;   free slot
	mov	[esp.Pushad_EDI], edx		; Return slot # in EDI

	popad
	clc
	ret

EndProc PageSwap_Find_Free_Page


;******************************************************************************
;
;   PageSwap_Test_Page_Present
;
;   DESCRIPTION:
;
;   ENTRY:
;	ECX = Page number to test
;
;   EXIT:
;	If zero flag is set then page is not present
;	(jz Page_Not_Present)
;
;   USES:
;	EAX, ECX, EDX, Flags
;
;==============================================================================

BeginProc PageSwap_Test_Page_Present

	push	ebp
	sub	esp, 4
	mov	ebp, esp			; EBP -> Page table copy addr

	VMMcall _CopyPageTable, <ecx, 1, ebp, 0>
	test	BYTE PTR [ebp], P_PRES		; Q: Is the page present?

	pop	ebp				; Don't change flags
	pop	ebp				; Restore EBP
	ret

EndProc PageSwap_Test_Page_Present


BeginDoc
;******************************************************************************
;
;   PageSwap_Test_IO_Valid
;
;   DESCRIPTION:
;	Determines if paging to/from disk can be performed now.  Simply passes
;	the call through to the PageFile device.
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

BeginProc PageSwap_Test_IO_Valid, Service

	VxDjmp	PageFile_Test_IO_Valid		; Just pass through to the
						;   PageFile device

EndProc PageSwap_Test_IO_Valid


;******************************************************************************
;		     L O C A L	 P R O C E D U R E S
;******************************************************************************



;******************************************************************************
;
;   PageSwap_Idle
;
;   DESCRIPTION:
;
;   ENTRY:
;	EBX = System VM Handle
;
;   EXIT:
;	If carry clear then
;	    This procedure "ate" the idle call -- We paged something.
;	else
;	    VM is still considered idle
;
;   USES:
;	All except EBP
;
;==============================================================================




BeginProc PageSwap_Idle

	cmp	[PS_Writes_Submitted], True	; Paging right now?
	jz	PS_VI_Do_Nothing		;   yes, do nothing

	cmp	[PS_Free_Page_Count], 0 	; Q: Any pages free?
	jne	SHORT PS_VI_Test_Write		;    Y: Try to do it
	test	[PS_Idle_Flags], PS_IF_File_Full;    N: Q: File completely full?
	jnz	PS_VI_Do_Nothing

PS_VI_Test_Write:
	cmp	[PS_Pending_WB_List], 0 	; Q: anything to submit?
	je	SHORT PS_VI_No_Pending_Writes	;   N: may want to page out
						;   Y: Do writes now
	mov	ecx, (Block_Svc_Ints OR Block_Enable_Ints)
	VMMcall Begin_Critical_Section
	cli
	PS_Queue_Level2 "[PS] PageSwap_Idle calling flush writes..."
	call	PageSwap_Flush_Writes
	sti
	VMMcall End_Critical_Section
	jmp	PS_VI_Do_Nothing

PS_VI_No_Pending_Writes:
	and	[PS_Idle_Flags], NOT PS_IF_File_Full

	test	[PS_Idle_Flags], PS_IF_Writing OR PS_IF_Restart
	jnz	SHORT PS_VI_Write_Pages

	btr	[PS_Idle_Flags], PS_IF_Test_Dirty_Bit
	jc	SHORT PS_VI_Get_Dirty_Count
	VMMcall Get_System_Time
	mov	ecx, eax
	sub	ecx, [PS_Last_Idle_Time]
	cmp	ecx, PS_Idle_Dirty_Test_Time
	jb	PS_VI_Do_Nothing
	mov	[PS_Last_Idle_Time], eax


;   Check the dirty page count.  Since this happens fairly infrequently and
;   since the call to _PageOutDirtyPages can take a long time, we'll always
;   "eat" this idle call even if we decide not to page.  We won't eat the next
;   one.

PS_VI_Get_Dirty_Count:
	VMMcall _PageOutDirtyPages, <0, PagePDPQueryDirty>
	test	eax, eax
	jz	PS_VI_Ate_It
	mov	edi, eax
	sub	esp, SIZE DemandInfoStruc
	mov	esi, esp
	VMMcall _GetDemandPageInfo, <esi, 0>
	mov	eax, [esi.DIUnlock_Count]
	shr	eax, 1
	cmp	[esi.DIFree_Count], 0
	ja	SHORT PS_VI_Test_Unlock
	shr	eax, 1
PS_VI_Test_Unlock:
	add	esp, SIZE DemandInfoStruc
	cmp	edi, eax
	jb	SHORT PS_VI_Ate_It
	or	[PS_Idle_Flags], PS_IF_Restart


;   It's OK to page stuff out!  Check flags and start writing

PS_VI_Write_Pages:
	xor	edx, edx
	btr	[PS_Idle_Flags], PS_IF_Restart_Bit
	jnc	SHORT PS_VI_Do_Write
	and	[PS_Idle_Flags], NOT PS_IF_2nd_Pass
	or	[PS_Idle_Flags], PS_IF_Writing
	or	edx, PagePDPSetBase

PS_VI_Do_Write:
	mov	ecx, (Block_Svc_Ints OR Block_Enable_Ints)
	VMMcall Begin_Critical_Section

PS_VI_Write_Again:
	or	[PS_Idle_Flags], PS_IF_Prepaging
	VMMcall _PageOutDirtyPages, <1, edx>
	and	[PS_Idle_Flags], NOT PS_IF_Prepaging
	test	eax, eax
	jnz	SHORT PS_VI_End_Crit
	mov	edx, PagePDPSetBase
	bts	[PS_Idle_Flags], PS_IF_2nd_Pass_Bit
	jnc	SHORT PS_VI_Write_Again
	or	[PS_Idle_Flags], PS_IF_Test_Dirty
	and	[PS_Idle_Flags], NOT (PS_IF_Writing OR PS_IF_2nd_Pass)

PS_VI_End_Crit:
	VMMcall End_Critical_Section


;   We did something that took a long time.  Return with carry clear to
;   indicate that the system VM should NOT be considered idle.

PS_VI_Ate_It:
	clc
	ret


;   We don't care about this one.  Pass idle call on to next handler.

PS_VI_Do_Nothing:
	stc
	ret

EndProc PageSwap_Idle


;******************************************************************************
;
;   PageSwap_Test_Dirty_Count
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;	Carry flag clear
;
;   USES:
;
;==============================================================================

BeginProc PageSwap_Test_Dirty_Count

	or	[PS_Idle_Flags], PS_IF_Test_Dirty
	clc
	ret

EndProc PageSwap_Test_Dirty_Count


;******************************************************************************
;
;   PageSwap_Grow_File
;
;   DESCRIPTION:
;
;   ENTRY:
;	ECX = Number of pages to grow paging file by
;
;   EXIT:
;	None.
;
;   USES: Flags
;
;==============================================================================

BeginProc PageSwap_Grow_File

;  Must own the critical section before calling PageFile_Grow_File

	pushad

	push	ecx
	mov	ecx, (Block_Svc_Ints OR Block_Enable_Ints)
	VMMcall Begin_Critical_Section
	pop	ecx

	VxDcall PageFile_Grow_File	; Returns new file size in EAX
					;   and max size in EBX
IFDEF DEBUG
	cmp	ebx, [PS_Max_File_Pages]
	jz	SHORT PS_GF_Same_Size
	Trace_Out "PS_Grow_File, max file size changed, now #EBX"
PS_GF_Same_Size:
ENDIF

	mov	[PS_Max_File_Pages], ebx	; May change if error growing

	mov	ecx, eax			; ECX = # pages in file
	sub	ecx, [PS_Cur_File_Pages]	; ECX = # NEW pages in file
	jz	SHORT GF_Exit			; If none then just quit

	mov	[PS_Cur_File_Pages], eax	; Set new current file size
	add	[PS_Free_Page_Count], ecx	; Add new pgs to free count


;   Done, release critical section & return.

GF_Exit:
	VMMcall End_Critical_Section

	popad
	ret

EndProc PageSwap_Grow_File


;******************************************************************************
;
;   PageSwap_Read_Or_Write
;
;   DESCRIPTION:
;
;	Note: the buffering code assumes that reads are never put into the
;	I/O request queue.
;
;   ENTRY:
;	CH  = PF_Read_Data, PF_Write_Data, PS_Read_Ahead
;	EDX = Page NUMBER to read/write
;	EDI = PAGE offset in file to read/write
;	ESI = Linear address of memory we can touch
;
;   EXIT:
;	If carry set then
;	    ERROR:  Could not read/write page
;	else
;	    Page read/written successfully
;
;   USES:
;	Flags
;
;==============================================================================

BeginProc PageSwap_Read_Or_Write

	Assert_Ints_Enabled

; Restart dirty page check when idle if not already prepaging

	test	[PS_Idle_Flags], PS_IF_Prepaging
	jnz	SHORT PS_ROW_Do_Rd_Wt
	SetFlag [PS_Idle_Flags], PS_IF_Restart
PS_ROW_Do_Rd_Wt:

	pushad

; If using a stupid pager, skip all the asynchronus stuff

	cmp	[PS_Pager_Type], PF_Paging_HW
	jb	SHORT PS_ROW_Stupid_Pager

; walk the lists, looking for a buffer with the same page number.
; If found, EBX will point to the Control Block for this buffer

	cli
	xor	ebx, ebx
	mov	eax, [PS_Free_List]
	call	PageSwap_Find_Buffer
	jc	SHORT PS_ROW_Found_Buffer
	mov	eax, [PS_Pending_WB_List]
	call	PageSwap_Find_Buffer
	jc	SHORT PS_ROW_Found_Buffer
	mov	eax, [PS_Submitted_RA_List]
	call	PageSwap_Find_Buffer
	jc	SHORT PS_ROW_Found_Buffer
	mov	eax, [PS_Locked_Buffer_List]
	call	PageSwap_Find_Buffer

PS_ROW_Found_Buffer:

; at this point, EBX -> CB of buffer for this page number, or EBX = NULL if no
; such buffer found

	cmp	ch, PF_Read_Data
	jne	SHORT PS_ROW_Not_Read
	call	PageSwap_Read_Now
	jmp	SHORT PS_ROW_Exit

PS_ROW_Not_Read:
	cmp	ch, PF_Write_Data
	jne	SHORT PS_ROW_Read_Ahead
	call	PageSwap_Write
	jmp	SHORT PS_ROW_Exit

PS_ROW_Read_Ahead:
	call	PageSwap_Read_Ahead
	jmp	SHORT PS_ROW_Exit

PS_ROW_Stupid_Pager:
	PS_Queue_Message "[PS] Read Or Write: no blockdev, issuing Immediate IO operation"
	call	PageSwap_Immediate_IO

PS_ROW_Exit:
	sti
	popad
	ret

EndProc  PageSwap_Read_Or_Write



;******************************************************************************
;
;   PageSwap_Read_Now
;
;   DESCRIPTION:
;
;	Actually does the read (read_now as opposed to read-ahead)
;
;   ENTRY:
;	CH  = PF_Read_Data
;	EBX -> CB of buffer for this page #, or NULL if no such buffer
;	EDX = Page NUMBER to read/write
;	EDI = PAGE offset in file to read/write
;	ESI = Linear address of memory we can touch
;
;   EXIT:
;	If carry set then
;	    ERROR:  Could not read
;	else
;	    Page read
;
;   USES:
;	Flags, EAX, ECX, EDI, ESI
;
;==============================================================================

BeginProc PageSwap_Read_Now

	Assert_Ints_Disabled

	or	ebx, ebx		    ; Q: was buffer found on some list?
	jz	DEBFAR PS_RN_Read_Immed     ;	N: must read in immediate mode

	mov	cx, [ebx.PS_BD_List]

	cmp	cx, PS_BDL_Free 	    ; Q: on free list?
	jne	SHORT PS_RN_Not_Free_List   ;	N:
					    ;	Y: get data from buffer
; this is a hit on a read-ahead buffer; move the buffer to the head of the
; free list (won't be needed again)

IFDEF DEBUG
	inc	[PS_Read_Ahead_Hits]
ENDIF

	PS_Queue_Message "[PS] Read: satisfied from read-ahead buffer #EBX"

	mov	eax, [PS_Free_List]
	call	PageSwap_Unlink_CB
	mov	ecx, PS_Link_Head + PS_BDL_Free
	call	PageSwap_Link_CB
	mov	[PS_Free_List], eax

	jmp	DEBFAR PS_RN_Copy_Page

PS_RN_Not_Free_List:
	cmp	cx, PS_BDL_Submitted_RA     ; Q: on read-ahead list?
	jne	DEBFAR PS_RN_Copy_Page_Msg  ;	N: all other lists: copy page

; command is already submitted on the read-ahead list: POSSIBLE RACE CONDITION
; if we submit our read now. So first we must ensure the read-ahead is
; completed or cancelled

IFDEF DEBUG
	inc	[PS_Read_Race_Cond]
ENDIF
	PS_Queue_Level2 "[PS] Read: Race condition with buffer #EBX"
	call	PageSwap_Ensure_Command_Done
	cli
	mov	ecx, PS_Link_Head + PS_BDL_Free
	mov	eax, [PS_Free_List]
	call	PageSwap_Link_CB
	mov	[PS_Free_List], eax

; added a buffer to the free list, must signal the semaphore to keep count
; up to date

	mov	eax, [PS_Buff_Avail_Sem]
	VMMcall Signal_Semaphore

; now check the buffer for valid data. If data is valid, then read-ahead
; had time to complete and we can use it. Otherwise, we will have to
; submit an immediate read.

	cmp	[ebx.PS_BD_Page_Number], PS_BDP_Invalid_Data
IFDEF DEBUG
	je	SHORT PS_RN_Read_Immed
	inc	[PS_Read_Ahead_Hits]
	PS_Queue_Message "[PS] Read: satisfied from buffer #EBX after Ensure_Command_Done"
	jmp	SHORT PS_RN_Copy_Page
ELSE
	jne	SHORT PS_RN_Copy_Page
ENDIF

PS_RN_Read_Immed:

; The data is not in any buffer, so we must read NOW, and wait for the
; operation to complete. Note: no cancelling of other commands which are
; potentially executing now; the read will complete with any submitted
; writes (high priority operations), but before any read-aheads (low-
; priority ops).

	PS_Queue_Message "[PS] Read: Submitted an immediate Read"

	mov	ch, PF_Read_Data
	call	PageSwap_Immediate_IO
	jmp	SHORT PS_RN_Exit

PS_RN_Copy_Page_Msg:
	PS_Queue_Message "[PS] Read: satisfied from buffer #EBX"

PS_RN_Copy_Page:
	mov	edi, esi			; EDI = destination of copy
	mov	esi, [ebx.PS_BD_Buffer_Ptr]	; ESI = source of copy
	cld
	mov	ecx, 1000h / 4
	rep movsd
	clc

PS_RN_Exit:
	ret

EndProc  PageSwap_Read_Now


;******************************************************************************
;
;   PageSwap_Read_Ahead
;
;   DESCRIPTION:
;
;	Actually does the read-ahead
;
;   ENTRY:
;	CH  = PS_Read_Ahead
;	EBX -> CB of buffer for this page #, or NULL if no such buffer
;	EDX = Page NUMBER to read/write
;	EDI = PAGE offset in file to read/write
;	ESI = Linear address of memory we can touch
;
;   EXIT:
;	Always carry clear. Page may or may not have been read.
;
;   USES:
;	Flags, EAX, EBX, ECX, EDX
;
;==============================================================================

BeginProc PageSwap_Read_Ahead

	Assert_Ints_Disabled

IFDEF DEBUG
	inc	[PS_Read_Ahead_Count]
ENDIF

	or	ebx, ebx
	jz	SHORT PS_RA_No_Buffer

	cmp	[ebx.PS_BD_List], PS_BDL_Free	; Q: on free list?

; If buffer is on some other list: locked list (can't read, disk error),
; pending write-behind (data already in a buffer) or submitted read-
; ahead (don't duplicate read-ahead) => then ignore this read-ahead request.

	jne	DEBFAR PS_RA_Ignore_Req 	;   N: ignore request

; Buffer with desired data found on free list => don't perform the read
; again. Simply move buffer to tail of free list, as if read-ahead had
; just been completed.

	PS_Queue_Message "[PS] Read-Ahead: data already in buffer #EBX on free list"

	mov	eax, [PS_Free_List]
	call	PageSwap_Unlink_CB
	mov	ecx, PS_Link_Tail + PS_BDL_Free
	call	PageSwap_Link_CB
	mov	[PS_Free_List], eax
	jmp	DEBFAR PS_RA_Exit

PS_RA_No_Buffer:
	cmp	[PS_Free_List], 0
	je	DEBFAR PS_RA_No_Free_Buf	;   N: can't read-ahead then

;
;   This wait should never block since we know that there is a buffer
;   in the free list.  This just keeps the counts up to date.
;
	mov	eax, [PS_Buff_Avail_Sem]
	mov	ecx, (Block_Svc_If_Ints_Locked OR Block_Enable_Ints)
	VMMcall Wait_Semaphore
	cli

;
;   Now actually remove a buffer from the free list
;
	mov	eax, [PS_Free_List]
	mov	ebx, eax
	call	PageSwap_Unlink_CB
	mov	[PS_Free_List], eax


; Now fill in control block with this read-ahead request.
; Must translate Read-Ahead to normal read with low priority (PageFile
; doesn't understand read-ahead)

	mov	[ebx.PS_BD_Cmd], PF_Read_Data
	mov	[ebx.PS_BD_Priority], PF_Low_Priority
	mov	[ebx.PS_BD_Page_Number], edx
	mov	[ebx.PS_BD_File_Page], edi
	mov	[ebx.PS_BD_Next], 0		; NULL pointer

; We must link the request before submitting it: it could actually complete
; (and call the CallBack) before returning, and then the lists would be all
; screwed up.
; But if we link before submitting, we must link at the tail, otherwise we
; are submitting a chain of commands.

	mov	eax, [PS_Submitted_RA_List]
	mov	ecx, PS_Link_Tail + PS_BDL_Submitted_RA
	call	PageSwap_Link_CB
	mov	[PS_Submitted_RA_List], eax

	PS_Queue_Message "[PS] Read-Ahead: submitted buffer #EBX"

	VxDcall PageFile_Read_Or_Write

IFDEF DEBUG
	jmp	SHORT PS_RA_Exit
ENDIF

PS_RA_Ignore_Req:
IFDEF DEBUG
	PS_Queue_Message "[PS] Read-Ahead: data already in buffer #EBX, NOT on free list"
	jmp	SHORT PS_RA_Exit
ENDIF

PS_RA_No_Free_Buf:
	PS_Queue_Message "[PS] Read-Ahead: couldn't read, no free buffer"

PS_RA_Exit:
	clc
	ret

EndProc PageSwap_Read_Ahead



;******************************************************************************
;
;   PageSwap_Write
;
;   DESCRIPTION:
;
;	Actually does the write
;
;   ENTRY:
;	CH  = PS_Write
;	EBX -> CB of buffer for this page #, or NULL if no such buffer
;	EDX = Page NUMBER to read/write
;	EDI = PAGE offset in file to read/write
;	ESI = Linear address of memory we can touch
;
;   EXIT:
;	Always carry clear.
;
;   USES:
;	Flags, EAX, EBX, ECX, EDI
;
;==============================================================================

BeginProc PageSwap_Write

	or	ebx, ebx			; Q: buffer found on any list?
	jz	DEBFAR PS_W_No_Buffer		;   N:

	mov	cx, [ebx.PS_BD_List]

	cmp	cx, PS_BDL_Free 	    ; Q: on free list?
	jne	SHORT PS_W_Not_Free_List    ;	N:

; Buffer was found on free list. We will use this buffer, so unlink it from
; the free list, and wait on the semaphore, to keep the count up to date.
; Note: we can't just jump to PS_W_Get_Buffer, because we don't want to use
; the first buffer on the free list; we want to use THIS buffer

	mov	eax, [PS_Buff_Avail_Sem]
	mov	ecx, (Block_Svc_If_Ints_Locked OR Block_Enable_Ints)
	VMMcall Wait_Semaphore
	cli

	mov	eax, [PS_Free_List]
	jmp	DEBFAR PS_W_Unlink

PS_W_Not_Free_List:

; Check the Pending Write-behind list. If buffer not there, assume (for now -
; debug code will verify assumption) that buffer is on locked list. In that
; case, we simply want to copy data from VMM page to buffer.

	cmp	cx, PS_BDL_Pending_WB		; Q: is it on write-behind?
	jne	SHORT PS_W_Locked_List		;   N: assume locked list then

	Trace_Out "PageSwap_Write called, buffer already on Write-Behind List"
	PS_Queue_Message "PageSwap_Write called, buffer already on Write-Behind List"

; If Buffer is on Pending Write-Behind list, but the operation has not yet been
; submitted. So we can simply copy the new data to the buffer.

	cmp	[PS_Writes_Submitted], True
	jne	SHORT PS_W_Copy_Data

; Buffer is on Pending Write-Behind list, AND writes have been submitted.
; POSSIBLE RACE CONDITION. We must ensure the pending write-behind is
; cancelled or completed before proceeding with the new write.

IFDEF DEBUG
	inc	[PS_Write_Race_Cond]
ENDIF
	PS_Queue_Level2 "[PS] Write: Race condition with buffer #EBX"
	call	PageSwap_Ensure_Command_Done
	cli
	cmp	[ebx.PS_BD_List], PS_BDL_Locked
	jne	DEBFAR PS_W_Have_Buffer

; Very rare case: cancelled command actually failed due to disk error.
; Buffer is in locked list, simply copy data over; ie: fall through
IFDEF DEBUG
	jmp	SHORT PS_W_Copy_Data
ENDIF

PS_W_Locked_List:
	PS_Queue_Message "[PS] Write: buffer #EBX found on Locked List"

PS_W_Copy_Data:

IFDEF DEBUG
; Sanity Checking: make sure buffer isn't actually on Submitted Read-ahead
; list. THIS CAN NOT BE!!!! If a buffer is in the Read-Ahead, it has not yet
; been read (because READs purge the Read-Ahead list), and it is not in
; memory. So, HOW CAN IT BE WRITTEN???
; Debug-only checking; print error message, but don't handle case.

	cmp	[ebx.PS_BD_List], PS_BDL_Submitted_RA
	jne	SHORT PS_W_Debug_Continue

	Debug_Out "Pageswap SEVERE ERROR: Write request before read-ahead is done"

PS_W_Debug_Continue:
ENDIF

; Copy data from VMM page to buffer
; ESI = lin. addr. = source of copy

	mov	edi, [ebx.PS_BD_Buffer_Ptr]	; EDI = destination of copy
	cld
	mov	ecx, 1000h / 4
	rep movsd

	jmp	PS_W_Exit

PS_W_No_Buffer:
	cmp	[PS_Free_List], 0		; Q: any free buffers?
	jnz	SHORT PS_W_Get_Buffer		;   Y: go get one

	mov	ebx, [PS_Submitted_RA_List]
	or	ebx, ebx			; Q: any submitted R-A?
	jz	SHORT PS_W_Get_Buffer		;   N: oh well, go wait

; No free buffers, but there is a buffer on the submitted read-ahead list.
; Attempt to cancel this request, and then go wait for the buffer to free.

IFDEF DEBUG
	inc	[PS_Read_Ahead_Canceled]
ENDIF
	PS_Queue_Message "[PS] Write: Cancelling Read-Ahead #EBX to get buffer"
	VxDcall PageFile_Cancel

PS_W_Get_Buffer:

; First we must wait on the buffer available semaphore, 2 reasons:
; - keeps the count up to date
; - we may actually really have to wait - in some cases, no buffer is
;   immediately available.
; Note: Possibility of Deadlock here:
;   if all buffers end up on the locked list while we are blocked waiting,
;   the semaphore will never be signalled (never any free buffer). This
;   case is too rare to be handled (and if all buffers are on locked list,
;   we have bigger problems anyway!)

	PS_Queue_Level2 "[PS] Write: preparing to wait for free buffer..."
	mov	eax, [PS_Buff_Avail_Sem]
	mov	ecx, (Block_Svc_If_Ints_Locked OR Block_Enable_Ints)
	VMMcall Wait_Semaphore
	PS_Queue_Level2 "[PS] Write: ...got free buffer"
	cli

	mov	eax, [PS_Free_List]
	mov	ebx, eax

PS_W_Unlink:
	call	PageSwap_Unlink_CB
	mov	[PS_Free_List], eax

PS_W_Have_Buffer:

; EBX -> buffer we can use for write operation

	mov	[ebx.PS_BD_Cmd], PF_Write_Data
	mov	[ebx.PS_BD_Priority], PF_High_Priority
	mov	[ebx.PS_BD_Page_Number], edx
	mov	[ebx.PS_BD_File_Page], edi

; copy the data over to buffer (yes, some code duplication with PS_W_Copy_Data)

	mov	edi, [ebx.PS_BD_Buffer_Ptr]	; EDI = destination of copy
	cld
	mov	ecx, 1000h / 4
	rep movsd

	mov	eax, [PS_Pending_WB_List]
	mov	ecx, PS_Link_Tail + PS_BDL_Pending_WB
	call	PageSwap_Link_CB
	mov	[PS_Pending_WB_List], eax

	cmp	[PS_Writes_Submitted], True
	jne	SHORT PS_W_Not_Submitted

; Writes have been submitted, so submit this request too. Note that NEXT
; pointer must be NULL (no next request), which is why we linked at the tail.

	PS_Queue_Message "[PS] Write: Submitted Buffer #EBX"
	VxDcall PageFile_Read_Or_Write
	jmp	SHORT PS_W_Exit

PS_W_Not_Submitted:
	PS_Queue_Message "[PS] Write: Queued Buffer #EBX on Write-Behind list"
	cmp	[PS_Free_List], 0
	jne	SHORT PS_W_Exit

; Writes are not submitted yet, and the free list is empty. (Does NOT mean
; that all buffers are pending writes: some may be in the submitted read-
; ahead list).
; Now is a good time to submit a batch of writes.

	PS_Queue_Level2 "[PS] Write: calling Flush Writes"
	call	PageSwap_Flush_Writes

PS_W_Exit:
	clc
	ret

EndProc PageSwap_Write


;******************************************************************************
;
;   PageSwap_Ensure_Command_Done
;
;   DESCRIPTION:
;
;	Attempts to cancel the command, and waits for the cancel to take
;	effect or the command to complete normally.
;
;   ENTRY:
;	interrupts disabled
;	EBX -> CB of command to cancel
;
;   EXIT:
;	interrupts enabled
;
;   USES:
;	Flags, EAX, ECX
;
;==============================================================================

BeginProc PageSwap_Ensure_Command_Done

	Assert_Ints_Disabled
	Assert_Buff_Desc ebx

IFDEF DEBUG
	cmp	[ebx.PS_BD_List], PS_BDL_Free	    ; Q: Done now?
	jne	SHORT PS_ECD_State_OK		    ;	 N: Good!
	Debug_Out "PAGESWAP STATE ERROR:  Page on free list at Ensure Command Done"
PS_ECD_State_OK:
ENDIF


; Command did not complete yet. Prepare to submit a cancel (and set the
; global flag indicating we are waiting for this cancel to complete).

	mov	[PS_Wait_Cancel_CB], ebx

	VxDcall PageFile_Cancel
	sti

	PS_Queue_Level2 "[PS] ECD: preparing to wait for #EBX to complete"
	mov	eax, [PS_Immediate_IO_Sem]
	mov	ecx, (Block_Svc_If_Ints_Locked OR Block_Enable_Ints)
	VMMcall Wait_Semaphore
	PS_Queue_Level2 "[PS] ECD: #EBX has completed"

PS_ECD_Exit:
	ret

EndProc PageSwap_Ensure_Command_Done



;******************************************************************************
;
;   PageSwap_Immediate_IO
;
;   DESCRIPTION:
;
;	Perform an immediate IO operation (ie: wait for it to terminate)
;	(Read or Write)
;
;   ENTRY:
;	CH = PF_Read_Data, PF_Write_Data
;	EDX = Page NUMBER to read/write
;	EDI = PAGE offset in file to read/write
;	ESI = Linear address of memory we can touch
;
;   EXIT:
;	If carry set then
;	    ERROR:  Could not read/write
;	else
;	    Page read/written
;
;   USES:
;	Flags,
;
;==============================================================================

BeginProc PageSwap_Immediate_IO

	push	eax
	push	ebx
	push	ecx
	push	edx

	mov	ebx, OFFSET32 PS_Static_CB
	mov	[ebx.PS_BD_Call_Back], OFFSET32 PageSwap_CallBack
	mov	[ebx.PS_BD_Cmd], ch
	mov	[ebx.PS_BD_Priority], PF_High_Priority
	mov	[ebx.PS_BD_Page_Number], edx
	mov	[ebx.PS_BD_Buffer_Ptr], esi
	mov	[ebx.PS_BD_File_Page], edi

	VxDcall PageFile_Read_Or_Write

	mov	eax, [PS_Immediate_IO_Sem]
	mov	ecx, (Block_Svc_If_Ints_Locked OR Block_Enable_Ints)
	VMMcall Wait_Semaphore

	cmp	[ebx.PS_BD_Page_Number], PS_BDP_Invalid_Data
	clc
	jne	SHORT PS_Immed_Exit
	stc

PS_Immed_Exit:
	pop	edx
	pop	ecx
	pop	ebx
	pop	eax
	ret
EndProc PageSwap_Immediate_IO


;******************************************************************************
;
;   PageSwap_CallBack
;
;   DESCRIPTION:
;
;	Called when an IO request has completed
;
;   ENTRY:
;	Interrupts disabled
;	EBX -> PageSwap Command Block
;	AX  = PFS_Success, PFS_Canceled, PFS_Failure
;
;   EXIT:
;
;   USES:
;	Flags,
;
;==============================================================================

BeginProc PageSwap_CallBack

	Assert_Ints_Disabled

	pushad

	mov	edx, eax			    ; edx = success flags

	cmp	ebx, OFFSET32 PS_Static_CB	    ; Q: static control block?
	jne	SHORT PS_CB_Not_Immediate	    ;	N: then not Immed. IO

; Called with pointer to static control block: this was an immediate IO
; operation. We must check the status of the operation (mark a failure
; by placing PS_BDP_Invalid_Data in the Page # field) and signal the
; semaphore

	PS_Queue_Message "[PS] Call Back: Immediate IO operation complete"
	cmp	dx, PFS_Success
	je	SHORT PS_CB_Immed_Signal
	mov	[ebx.PS_BD_Page_Number], PS_BDP_Invalid_Data
	PS_Queue_Message "[PS]            Immediate IO operation FAILED!"

PS_CB_Immed_Signal:
	mov	eax, [PS_Immediate_IO_Sem]
	VMMcall Signal_Semaphore
	jmp	PS_CB_Exit

PS_CB_Not_Immediate:
	cmp	[ebx.PS_BD_List], PS_BDL_Pending_WB ; Q: Was it Write?
	jne	DEBFAR PS_CB_Read_Ahead 	    ;	N: Must be Read-Ahead

	PS_Queue_Message "[PS] Call Back: Write-Behind (buffer #EBX) complete"

	mov	eax, [PS_Pending_WB_List]
	call	PageSwap_Unlink_CB
	mov	[PS_Pending_WB_List], eax
	or	eax, eax			    ; Q: Last submitted write?
	jnz	SHORT PS_CB_Check_Success	    ;	N: next step

; this is the last submitted write => must indicate that writes no longer
; submitted

	PS_Queue_Message "[PS] Call Back: no more submitted Write-Behinds"
	mov	[PS_Writes_Submitted], False

PS_CB_Check_Success:
	cmp	dx, PFS_Success 		    ; Q: Success?
	jne	SHORT PS_CB_Failed_Write	    ;	N:

; Write was successful. Prepare to place buffer at head of free list. Don't
; actually link it there yet, though: if Ensure_Command_Done is waiting on
; this particular buffer, we don't want it in the free list

	mov	ecx, PS_Link_Head + PS_BDL_Free
	jmp	DEBFAR PS_CB_Check_Cancel

PS_CB_Failed_Write:
IFDEF DEBUG

; sanity checking: make sure the write wasn't cancelled. Only read-aheads
; should get cancelled

	cmp	dx, PFS_Canceled
	jne	SHORT PS_CB_Debug_Done

	Debug_Out "PageSwap: attempt to cancel a write request"
PS_CB_Debug_Done:
ENDIF

	PS_Queue_Message "[PS] Call Back: Write failed. Placing buffer #EBX on Locked List"

	mov	eax, [PS_Locked_Buffer_List]
	mov	ecx, PS_Link_Head + PS_BDL_Locked
	call	PageSwap_Link_CB
	mov	[PS_Locked_Buffer_List], eax
	cmp	ebx, [PS_Wait_Cancel_CB]
	je	DEBFAR PS_CB_Signal_Cancel
	jmp	DEBFAR PS_CB_Exit

PS_CB_Read_Ahead:
	mov	eax, [PS_Submitted_RA_List]
	call	PageSwap_Unlink_CB
	mov	[PS_Submitted_RA_List], eax

	cmp	dx, PFS_Success
	jne	SHORT PS_CB_Failed_Read

; Read-ahead was successful. Prepare to place buffer at tail of free list

	PS_Queue_Message "[PS] Call Back: Read-Ahead (buffer #EBX) successful"
	mov	ecx, PS_Link_Tail + PS_BDL_Free
	jmp	SHORT PS_CB_Check_Cancel

PS_CB_Failed_Read:

; Failed or cancelled read. Buffer is invalid, so place an invalid page #
; in the control block and prepare to place at head of free list

	PS_Queue_Message "[PS] Call Back: Read-Ahead (buffer #EBX) complete but data invalid."
	mov	[ebx.PS_BD_Page_Number], PS_BDP_Invalid_Data
	mov	ecx, PS_Link_Head + PS_BDL_Free

PS_CB_Check_Cancel:

; we are ready to place the buffer in the free list (at head or tail, depending
; on ECX). But first, we must check if Ensure_Command_Done is waiting on this
; buffer. If it is, we must not place it in the free list.

	cmp	ebx, [PS_Wait_Cancel_CB]    ; Q: Cancel waiting for this CB?
	je	SHORT PS_CB_Signal_Cancel   ;	Y: Go signal semaphore

	mov	eax, [PS_Free_List]
	call	PageSwap_Link_CB
	mov	[PS_Free_List], eax
	mov	eax, [PS_Buff_Avail_Sem]
	VMMcall Signal_Semaphore
	jmp	SHORT PS_CB_Exit

PS_CB_Signal_Cancel:
	mov	[PS_Wait_Cancel_CB], 0	    ; reset variable
	mov	eax, [PS_Immediate_IO_Sem]  ; same semaphore for cancel
	VMMcall Signal_Semaphore

PS_CB_Exit:
	popad
	ret

EndProc PageSwap_CallBack



;******************************************************************************
;
;   PageSwap_Flush_Writes
;
;   DESCRIPTION:
;
;	Submits the current chain of Writes
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;	Flags
;
;==============================================================================

BeginProc PageSwap_Flush_Writes

	Assert_Ints_Disabled

IFDEF DEBUG
	cmp	[PS_Writes_Submitted], True	; Q: Already submitted?
	jne	SHORT PS_FW_Not_Submitted	;   N: skip error message

	Debug_Out "PageSwap_Flush_Writes called but writes already submitted"

PS_FW_Not_Submitted:
	cmp	[PS_Pending_WB_List], 0 	; Q: anything to submit?
	jne	SHORT PS_FW_List_Not_Empty	;   Y: skip error message

	Debug_Out "Pageswap_Flush_Writes called but write list is empty"

PS_FW_List_Not_Empty:
ENDIF

	PS_Queue_Message "[PS] Flush Writes: Write-Behind queue submitted"

	mov	[PS_Writes_Submitted], True
	push	ebx
	mov	ebx, [PS_Pending_WB_List]
	VxDcall PageFile_Read_Or_Write

	pop	ebx

	ret
EndProc PageSwap_Flush_Writes



;******************************************************************************
;
;   PageSwap_Find_Buffer
;
;   DESCRIPTION:
;
;	Searches the specified list for a buffer containing a given page
;
;   ENTRY:
;	EAX -> list to search
;	EDX = Page Number to search for
;
;   EXIT:
;	if carry set
;	    buffer found,
;	    EBX -> command block
;	else carry clear
;	    buffer not found
;	    EBX not modified
;
;   USES:
;	Flags, EBX
;
;==============================================================================

BeginProc PageSwap_Find_Buffer

	Assert_Ints_Disabled

	push	eax

PS_FB_Loop:
	or	eax, eax			    ; Q: end of list?
	jz	SHORT PS_FB_Not_Found		    ;	Y: Buffer not found

	cmp	edx, [eax.PS_BD_Page_Number]	    ; Q: same page number?
	je	SHORT PS_FB_Found		    ;	Y: Found buffer

	mov	eax, [eax.PS_BD_Next]		    ; next node
	jmp	SHORT PS_FB_Loop

PS_FB_Found:
	mov	ebx, eax
	Assert_Buff_Desc ebx
	stc
	jmp	SHORT PS_FB_Exit

PS_FB_Not_Found:
	clc

PS_FB_Exit:
	pop	eax
	ret

EndProc PageSwap_Find_Buffer



;******************************************************************************
;
;   PageSwap_Unlink_CB
;
;   DESCRIPTION:
;
;	Removes the given command block from the specified list
;
;   ENTRY:
;	EAX -> list in which cb is to be found
;	EBX -> CB to be unlinked
;
;   EXIT:
;	CB no longer in list
;	EAX is new head to list
;
;   USES:
;	Flags, EAX
;
;==============================================================================

BeginProc PageSwap_Unlink_CB

	Assert_Ints_Disabled
	Assert_Buff_Desc ebx

	cmp	eax, ebx		    ; Q: Is CB at head of list?
	je	SHORT PS_UCB_Head	    ;	Y: Special case

	push	eax
	push	ecx

PS_UCB_Loop:
	or	eax, eax		    ; Q: End of list?
	jz	SHORT PS_UCB_Not_Found	    ;	Y: can't find CB !!

	mov	ecx, [eax.PS_BD_Next]
	cmp	ecx, ebx		    ; Q: is it next buffer in list?
	je	SHORT PS_UCB_Found	    ;	Y:

	mov	eax, ecx
	jmp	SHORT PS_UCB_Loop

PS_UCB_Found:
; found CB: eax -> previous CB, ebx -> CB
	mov	ecx, [ebx.PS_BD_Next]
	mov	[eax.PS_BD_Next], ecx
	pop	ecx
	pop	eax
	jmp	SHORT PS_UCB_Exit

PS_UCB_Not_Found:
IFDEF DEBUG
	Debug_Out "PageSwap_Unlink_CB called but CB not in specified list"
ENDIF
	pop	ecx
	pop	eax
	ret

PS_UCB_Head:
	mov	eax, [ebx.PS_BD_Next]

PS_UCB_Exit:
	mov	[ebx.PS_BD_Next], 0
	ret

EndProc PageSwap_Unlink_CB



;******************************************************************************
;
;   PageSwap_Link_CB
;
;   DESCRIPTION:
;
;	Add the given buffer to a specified list, either at the head or the
;	tail
;
;   ENTRY:
;	EAX -> list in which cb is to be linked
;	EBX -> CB to be linked
;	ECX: high-word: PS_Link_Head or PS_Link_Tail
;	     low-word: PS_BD_List value
;
;   EXIT:
;	EAX is new head to list
;
;   USES:
;	Flags, EAX
;
;==============================================================================

BeginProc PageSwap_Link_CB

	Assert_Ints_Disabled
	Assert_Buff_Desc ebx

	mov	[ebx.PS_BD_List], cx
	or	eax, eax		    ; Q: empty list?
	jz	SHORT PS_LCB_Head	    ;	Y: must link at head then

	test	ecx, PS_Link_Head	    ; Q: Link at head?
	jz	SHORT PS_LCB_Tail	    ;	N: at tail then

PS_LCB_Head:
	mov	[ebx.PS_BD_Next], eax
	mov	eax, ebx
	jmp	SHORT PS_LCB_Exit

PS_LCB_Tail:

; link at tail - first, must find tail. We know the list has at least 1 elmt.

	push	eax
	push	ecx

PS_LCB_Loop:
	mov	ecx, [eax.PS_BD_Next]
	jecxz	SHORT PS_LCB_Found_Tail     ; ecx = 0 => end of list

	mov	eax, ecx		    ;	N: next element
	jmp	SHORT PS_LCB_Loop

PS_LCB_Found_Tail:
	mov	[ebx.PS_BD_Next], ecx	    ; note ecx is conveniently 0
	mov	[eax.PS_BD_Next], ebx
	pop	ecx
	pop	eax

PS_LCB_Exit:
	ret

EndProc PageSwap_Link_CB


;******************************************************************************
;
;   PageSwap_Find_Page
;
;   DESCRIPTION:
;
;   ENTRY:
;	EDX = Page number
;
;   EXIT:
;	If carry clear then
;	    EDI = Offset in file of page
;	else
;	    Page not in file
;
;   USES:
;
;
;==============================================================================

BeginProc PageSwap_Find_Page, High_Freq

	push	eax
	push	ebx
	push	ecx

	mov	ecx, edx			; Save page # in ECX
	xor	eax, eax			; Calculate 'best' position
	xchg	eax, edx			;   for page
	sub	eax, [PS_Base_Lin_Page]
	idiv	[PS_Max_File_Pages]		; EDX = page num MOD file size
	mov	edi, edx			;     = target index of pg ent
	mov	edx, ecx			; EDX = page num to find

	mov	ebx, [PS_Page_Entry_Base]
	mov	ecx, [PS_Max_File_Pages]

;   Scan the Page_Entry list looking for the desired page.  Hopefully we'll
;   find it on the first compare, but if not, check Max_File_Pages # of
;   entries.

PS_FP_Loop:
	mov	eax, [ebx][edi*4]		; Scan the Page_Entry list
	and	eax, NOT PSF_Entry_Flags	;   looking for the target pg.

	cmp	eax, edx
	jz	SHORT PS_FP_Exit		; Got it!  (CY also clr if eq)

	inc	edi				; Bump index, watch for wrap
	cmp	edi, [PS_Max_File_Pages]
	jb	SHORT PS_FP_Next
	xor	edi, edi
PS_FP_Next:
	loopd	PS_FP_Loop

PS_FP_Failed:

	%OUT Enable this Debug_Out after MMGR change

	;;; Debug_Out "PS_Find_Page failed"

	stc

PS_FP_Exit:

	pop	ecx
	pop	ebx
	pop	eax
	ret


EndProc PageSwap_Find_Page


;******************************************************************************
;			D E B U G G I N G   C O D E
;******************************************************************************


IFDEF DEBUG

;******************************************************************************
;
;   PageSwap_Debug_Query
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


BeginProc PageSwap_Debug_Query

	cmp	[PS_Max_File_Pages], 0
	ja	SHORT PS_DQ_Enabled
	Trace_Out "Demand paging is disabled"
	clc
	ret

PS_DQ_Enabled:
	Trace_Out "PageSwap Debug Routines:"
	Trace_Out " "
	Trace_Out "1. Print PageSwap statistics"
	Trace_Out " "
	Trace_Out "2. Reset PageSwap statistics"
	Trace_Out " "
	Trace_Out "3. Enable level 1 Queue_Out of PageSwap tracing messages"
	Trace_Out " "
	Trace_Out "4. Enable level 2 Queue_Out of PageSwap tracing messages"
	Trace_Out " "
	Trace_Out "5. Disable Queue_Out of messages"
	Trace_Out " "
	Trace_Out "6. Print page list"
	Trace_Out " "
	Trace_Out "[ESC] Exit PageSwap Debug Routines"
PS_DQ_Get_Choice:
	VMMcall In_Debug_Chr
	Trace_Out " "
	jz	PS_DQ_Exit
	cmp	al, '1'
	je	PS_DQ_Stats
	cmp	al, '2'
	je	PS_DQ_Reset_Stats
	cmp	al, '3'
	je	SHORT PS_DQ_Enable_Queue_Out
	cmp	al, '4'
	je	SHORT PS_DQ_Enable_Level_2
	cmp	al, '5'
	je	short PS_DQ_Disable_Queue_Out
	cmp	al, '6'
	je	PS_DQ_P_List
	jmp	PS_DQ_Get_Choice

PS_DQ_Disable_Queue_Out:
	mov	[PS_Debug_Queuing], False
	mov	[PS_Debug_Level2], False
	Trace_Out "Queuing of PageSwap tracing messages DISABLED"
	Trace_Out " "
	jmp	PS_DQ_Exit

PS_DQ_Enable_Queue_Out:
	mov	[PS_Debug_Queuing], True
	mov	[PS_Debug_Level2], False
	Trace_Out "Queuing of PageSwap tracing messages LEVEL 1 ENABLED"
	Trace_Out " "
	jmp	PS_DQ_Exit

PS_DQ_Enable_Level_2:
	mov	[PS_Debug_Queuing], True
	mov	[PS_Debug_Level2], True
	Trace_Out "Queuing of PageSwap tracing messages LEVEL 2 ENABLED"
	Trace_Out " "
	jmp	PS_DQ_Exit

PS_DQ_Stats:
	Trace_Out " "
	Trace_Out "---- Throughput ----"
	Trace_Out " "

	mov	edi, [PS_DQ_Total_In]
	Trace_Out "Total calls to pageswap_in    = #EDI"
	test	edi, edi
	jz	SHORT Skip_Read_Perf
	mov	ecx, [PS_DQ_Read_In]
	mov	eax, ecx
	xor	edx, edx
	imul	eax, 100
	idiv	edi
	VMMcall Debug_Convert_Hex_Decimal
	Trace_Out "Total number of pages read    = #ECX (#AX%)"
	xor	edx, edx
	mov	eax, [PS_DQ_Read_Time]
	mov	ebx, 1000
	idiv	ebx
	VMMcall Debug_Convert_Hex_Decimal
	xchg	eax, edx
	VMMcall Debug_Convert_Hex_Decimal
	shl	eax, 4
	Trace_Out "Total time for page in        = #EDX.#AX (decimal)"
	jecxz	SHORT Skip_Read_Perf
	mov	eax, ecx
	xor	edx, edx
	shl	eax, 12
	mov	ebx, 1000
	imul	ebx
	idiv	[PS_DQ_Read_Time]
	VMMcall Debug_Convert_Hex_Decimal
	Trace_Out "Bytes per second              = #EAX (decimal)"

Skip_Read_Perf:
	Trace_Out " "

	mov	edi, [PS_DQ_Total_Out]
	Trace_Out "Total calls to pageswap_out   = #EDI"
	test	edi, edi
	jz	SHORT Skip_Write_Perf
	mov	ecx, [PS_DQ_Written_Out]
	mov	eax, ecx
	xor	edx, edx
	imul	eax, 100
	idiv	edi
	VMMcall Debug_Convert_Hex_Decimal
	Trace_Out "Total number of pages written = #ECX (#AX%)"
	xor	edx, edx
	mov	eax, [PS_DQ_Write_Time]
	mov	ebx, 1000
	idiv	ebx
	VMMcall Debug_Convert_Hex_Decimal
	xchg	eax, edx
	VMMcall Debug_Convert_Hex_Decimal
	shl	eax, 4
	Trace_Out "Total time for page out       = #EDX.#AX (decimal)"
	jecxz	Skip_Write_Perf
	mov	eax, ecx
	xor	edx, edx
	shl	eax, 12
	mov	ebx, 1000
	imul	ebx
	idiv	[PS_DQ_Write_Time]
	VMMcall Debug_Convert_Hex_Decimal
	Trace_Out "Bytes per second              = #EAX (decimal)"

Skip_Write_Perf:
	Trace_Out " "

	mov	edi, [PS_DQ_PrePg_Total_Out]
	Trace_Out "PrePaging: Total calls to pageswap_out   = #EDI"
	test	edi, edi
	jz	SHORT Skip_PrePg_Write_Perf
	mov	ecx, [PS_DQ_PrePg_Written_Out]
	mov	eax, ecx
	xor	edx, edx
	imul	eax, 100
	idiv	edi
	VMMcall Debug_Convert_Hex_Decimal
	Trace_Out "Prepaging: Total number of pages written = #ECX (#AX%)"
	xor	edx, edx
	mov	eax, [PS_DQ_PrePg_Write_Time]
	mov	ebx, 1000
	idiv	ebx
	VMMcall Debug_Convert_Hex_Decimal
	xchg	eax, edx
	VMMcall Debug_Convert_Hex_Decimal
	shl	eax, 4
	Trace_Out "Prepaging: Total time for page out       = #EDX.#AX (decimal)"
	jecxz	Skip_PrePg_Write_Perf
	mov	eax, ecx
	xor	edx, edx
	shl	eax, 12
	mov	ebx, 1000
	imul	ebx
	idiv	[PS_DQ_PrePg_Write_Time]
	VMMcall Debug_Convert_Hex_Decimal
	Trace_Out "Prepaging: Bytes per second              = #EAX (decimal)"

Skip_PrePg_Write_Perf:
	Trace_Out "Press any key to continue, [ESC] to quit...", NO_EOL
	VMMcall In_Debug_Chr
	Trace_Out " "
	jz	PS_DQ_Exit
	Trace_Out " "
	Trace_Out "---- I/O operation scheduling ----"
	Trace_Out " "

	mov	ax, [PS_Num_Buffers]
	Trace_Out "Number of buffers used        = #AX"
	Trace_Out " "

	mov	ecx, [PS_Read_Ahead_Count]
	jecxz	PS_DQ_Skip_Rd_Ahead
	mov	ebx, [PS_Read_Ahead_Hits]
	mov	eax, ebx
	xor	edx, edx
	imul	eax, 100
	idiv	ecx
	VMMcall Debug_Convert_Hex_Decimal
	Trace_Out "Read Ahead Calls/Hits         = #ECX / #EBX (#AX%)"
	mov	ecx, [PS_DQ_Read_In]
	mov	eax, ebx
	xor	edx, edx
	imul	eax, 100
	idiv	ecx
	VMMcall Debug_Convert_Hex_Decimal
	Trace_Out "Reads satisfied by read-ahead hits = #AX%"

	mov	eax, [PS_Read_Ahead_Canceled]
	Trace_Out "Read-Aheads Canceled          = #EAX"

	Trace_Out " "

	mov	eax, [PS_Read_Race_Cond]
	Trace_Out "Cancel & Wait, READ           = #EAX"

PS_DQ_Skip_Rd_Ahead:
	mov	eax, [PS_Write_Race_Cond]
	Trace_Out "Cancel & Wait, WRITE          = #EAX"

	Trace_Out "Press any key to continue, [ESC] to quit...", NO_EOL
	VMMcall In_Debug_Chr
	Trace_Out " "
	jz	PS_DQ_Exit
	Trace_Out " "
	Trace_Out "---- Swap File information ----"
	Trace_Out " "

	mov	edi, [PS_Max_File_Pages]
	mov	eax, [PS_Cur_File_Pages]
	Trace_Out "File contains #EAX pages of a possible #EDI"
	mov	eax, [PS_Free_Page_Count]
	mov	ecx, eax
	xor	edx, edx
	imul	eax, 100
	idiv	edi
	VMMcall Debug_Convert_Hex_Decimal
	Trace_Out "#ECX pages are free (#AX%)"
	Trace_Out " "
	mov	eax, [PS_Page_Entry_Base]
	Trace_Out "Page entry lin base  = #EAX"
	mov	eax, [PS_Next_Possible_Victim]
	Trace_Out "Next possible victim = #EAX"
	Trace_Out " "
	Trace_Out "Current state: ", NO_EOL
	mov	esi, [PS_Debug_State]
	mov	esi, PS_DQ_State_Tab[esi*4]
	pushad
	VMMcall Out_Debug_String
	popad
	Trace_Out " "
	jmp	SHORT PS_DQ_Exit

PS_DQ_Reset_Stats:
	xor	eax, eax
	mov	[PS_DQ_Total_In], eax
	mov	[PS_DQ_Read_In], eax
	mov	[PS_DQ_Read_Time], eax
	mov	[PS_DQ_Total_Out], eax
	mov	[PS_DQ_Written_Out], eax
	mov	[PS_DQ_Write_Time], eax
	mov	[PS_DQ_PrePg_Total_Out], eax
	mov	[PS_DQ_PrePg_Written_Out], eax
	mov	[PS_DQ_PrePg_Write_Time], eax
	mov	[PS_Read_Ahead_Count], eax
	mov	[PS_Read_Ahead_Hits], eax
	mov	[PS_Read_Ahead_Canceled], eax
	mov	[PS_Read_Race_Cond], eax
	mov	[PS_Write_Race_Cond], eax

	Trace_Out " "
	Trace_Out "Pageswap statistics reset"
	Trace_Out " "

PS_DQ_Exit:
	clc
	ret


PS_DQ_P_List:
	mov	ebx, [PS_Page_Entry_Base]
	mov	ecx, [PS_Max_File_Pages]
	xor	edi, edi
PS_DQ_Loop:
	trace_out "#EDI-",NO_EOL
	mov	eax, [ebx][edi*4]		; Scan the Page_Entry list
	and	eax, NOT PSF_Entry_Flags	;   looking for the target pg.
	jnz	SHORT PS_DQ_NotFr
	trace_out "FREE     ",NO_EOL
	jmp	short PS_DQ_Done

PS_DQ_NotFr:
	trace_out "#EAX ",NO_EOL
PS_DQ_Done:
	inc	edi
	test	edi,00000003h
	jnz	short NoDQNl
	trace_out " "
NoDQNl:
	loopd	PS_DQ_Loop
	jmp	short PS_DQ_Exit

EndProc PageSwap_Debug_Query


ENDIF

VxD_LOCKED_CODE_ENDS

	END PageSwap_Real_Init
