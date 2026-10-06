;	SCCSID = @(#)abdsk1.asm	5.9 87/03/10
	page	,132
	title	ABDISK1 - MS OS/2 disk device driver
	name	ABDISK1
	comment !
********************* Start of Specifications *************************
*								      *
*  Source File Name: ABDSK1.ASM 				      *
*								      *
*  Descriptive Name: Strategy Routine and Command Handlers	      *
*								      *
*  Copyright:							      *
*								      *
*  Status: Version 1.1						      *
*								      *
*  Function: Provides the strategy routine and command processors     *
*		      for the disk device driver		      *
*								      *
*  Notes: Since this source file contains the device driver header    *
*	  control block, it must be the 1st file input to the linker  *
*								      *
*    Dependencies: N/A						      *
*								      *
*    Restrictions: N/A						      *
*								      *
*  Entry Points:  DriveRequest					      *
*								      *
*  External References:  See EXTRN statements below		      *
*								      *
********************** End of Specifications ************************** !

; @@ This source file has been totally reworked from the Version 1.0 base
; @@ code. It has been so heavily modified and reformatted that it is not
; @@ practical to determine what lines of code are new or old on a line
; @@ by line basis.

	.286p

	include abdebug.inc		; debug and listing control

	include absegs.inc		; code and data segment definitions

	BREAK <External code entry points>
Code	segment

; Routines located in ABINIT.ASM
  CodeInit segment
	extrn	DriveInit:near
  CodeInit ends

; Routines located in ABDSK2.ASM/ATDSK2.ASM
	extrn	DriveExecute:near
	extrn	FloppyChange:near
	extrn	Do_Check:near
	extrn	Update_Owner:near
	extrn	Do_ResetUM:near
	extrn	Media_Check:near
	extrn	CheckWrap:near

; Routines located in ABDSK3.ASM/ATDSK3.ASM
	extrn	FixedExecute:near

; Routines located in CACHDSK1.ASM
	extrn	CacheIO:near

; Routines located in ABDSK5.ASM
	extrn	DrivePartNbr:near
	extrn	DriveUnitMap:near

; Routines located in ABDSK6.ASM
	extrn	DriveGenIOCTL:near

; Routines located in ABINT13.ASM
	extrn	Int13Handler:far
	extrn	INT13:near
	extrn	RealModeDS:word 	; Word in code seg to keep RM DS

; Routines located in ABSUBR.ASM
	extrn	CmdErr:near
	extrn	PBlock:near
	extrn	PRun:near
	extrn	StatusDevReady:near
	extrn	StatusError:near
	extrn	StatusComplete:near
	extrn	SetStatus:near
	extrn	SSig:near
	extrn	SWait:near
	extrn	SWClr:near
	extrn	Get_BDS_Addr:near
	extrn	Get_Request_Packet:near
	extrn	Release_Request_Packet:near
	extrn	BPBFromBoot:near
	extrn	BPBFromScratch:near
	extrn	Add_Partition_Offset:near


Code	ends

	BREAK <External data references>
BioData segment

; Data located in ABDSK1.ASM
	extrn	CacheActive:word

; Data located in ABDSK2.ASM
	extrn	NumFlop:byte		; count of real/pseudo drives
	extrn	Drive:byte		; floppy IOStruc

; Data located in ABDSK3.ASM
	extrn	Fixed:byte
	extrn	NumFix:byte		; count of number of hard disks

; Data located in ABDSK4.ASM
	extrn	TimerActive:byte

; Data located in ABDSK4.ASM
	extrn	InitTime:byte		; 0 during INIT processing

; Data located in ABINT13.ASM
	extrn	SemInt13:byte
	extrn	RealInt13Vector:dword

BioData ends

	.xlist				;
	include struc.inc		; structured macros
if1
	include abmac.inc		; private macros
endif
	include ibmrom.inc		; AT low core ROM data area
	include devhlp.inc		; Device helper definitions.
	include error.inc		; DOS error codes
	include abdskequ.inc		; general equates
	include abddrv.inc		; IOStruc
	include atstate.inc		; state machine definitions

	ListLevel %ablistlevel,%abdebug

	BREAK	<Data Segment>
BioData segment 			    ;

	public DevHlpFunc
DevHlpFunc dd ? 			    ; Address of DevHelp routine

	even					  ;   ...
	public Start_BDS			  ;   ...
Start_BDS dw DataOffset LDriveA 		   ; BDS chain anchor

	public DriveA,DriveB,DriveC,Drive80,Drive81
DriveA		dw	LDriveA
DriveB		dw	LDriveB
DriveC		dw	LDriveC
Drive80 	dw	LDrive80
Drive81 	dw	LDrive81

	public	FDInfo
FDInfo	label	word				  ; array of BPB addresses
						  ; return on INIT
	irpc	x,AB				  ;
	dw	DataOffset LDrive&x + BPBsecsiz   ; BPB addr of drive A,B is
	endm					  ; set as default

	irpc	x,CDEFGHIJKLMNOPQRSTUVWXYZ	  ; BPB addr after drive B
	dw	0				  ; will be set at INIT time.
	endm					  ; Initialize with 0.
						  ;
						  ;
;**	IOActive - Disk I/O Active Semaphore
;
;	Some routines want to wait until disk I/O is inactive, so we
;	maintain the IOActive semaphore.  It is set/locked when disk I/O
;	is active and is cleared when the work list goes empty.  Processes
;	that want to wait until the disks are idle can block on setting
;	this semaphore.  They should then immediately release it.  Note
;	that setting this semaphore does not prevent other disk I/O from
;	being started up or put on the work queue.  This semaphore is only
;	intended to indicate when the drives have gone idle, not to prevent
;	the drives from becoming active.

	even
	public	IOActive
IOActive db	0,0		; set while BIOS disk I/O is in progress

BioData ends

ScratchData	segment
;**	ScratchBufSem - Protect Scratch Buffer

;	This driver contains a 512-byte scratch buffer that may be
;	allocated by KERNEL-TASK mode processes.  The user of the buffer
;	must protect it with ScratchBufSem.  Note that the driver may need
;	ScratchBuffer if it is to do a disk transfer which straddles a
;	65K physical memory boundary, so processes which reserve ScratchBufSem
;	must be sure to request no such transfers.  This is normally the
;	case because ScratchBuffer uses normally want to read/write from
;	the buffer itself which does not, of course, straddle a boundary.

	even
	public	ScratchBufSem,ScratchBuffer,ScratchBufPhys
ScratchBufSem	db 0,0		; semaphore controlling ScratchBuffer
ScratchBufPhys	dd 0		; physical 32 bit address of ScratchBuffer
ScratchBuffer	dw LScratchBuffer  ; offset to scratchbuffer
LScratchBuffer	 db SCRATCHSIZE dup (?)  ; scratch buffer for use when DMA fails

ScratchData	ends


	BREAK	<BDS Chain - DriveStruc>
;    DriveA/DriveB/DriveC/DriveD contain status information (BPB's and
;    other related information) about each of the 4 possible drives on the
;    system.

BDSChain segment

even
;		public DriveA,DriveB,DriveC,Drive80,Drive81
		public LDriveA,LDriveB,LDriveC,LDrive80,LDrive81
LDriveA 	DriveStruc	<DataOffset LDriveB,0>	   ; floppy drive 0
LDriveB 	DriveStruc	<DataOffset LDrive80,1>    ; floppy drive 1 or 0
LDrive80	DriveStruc	<DataOffset LDrive81,80H>  ; fixed disk 80H
LDrive81	DriveStruc	<-1,81H>		  ; fixed disk 81H
		ListLevel	0
?x	=		2
		irpc x,CDEFGHIJKLMNOPQRSTUVWXYZ 	 ; C to Z for fixed disk
LDrive&x	DriveStruc	<-1,?x> 		  ;   partitions
?x	=		?x + 1			  ;   ...
		endm					  ;   ...
		ListLevel	%ablistlevel

BDSChain ends


		    BREAK <Disk Device Driver Header>
 ;** Disk Device Driver Header -
 ;
 ;   This header replaces "FloppyDevice" which is for 1.0 driver.
 ;   NumDrv (# of drives) is set at DriveInit in INIT command process
 ;   and other parameter will be set at system initialization.
 ;   This header is put at the begining of Data.
 ;

DDDHeader segment
	  DebugPublic header
	  public NumDrv
 header   label   word
	   dw	-1, -1			 ; Pointer to next device header
	   dw	DEV_NON_IBM+DEVLEV_1	 ; device attribute
	   dw	CodeOffset DriveRequest  ; strategy routine offset
	   dw	0			 ; Interrupt entry point
 NumDrv    db	0			 ; 1 Unit
	   db	7 dup (0)		 ; Remaining Name Chars
	   dw	0			 ; Protect mode CS selctor of strategy
	   dw	0			 ; Protect-mode DS selector
	   dw	0			 ; Real-mode CS segment of strategy
	   dw	0			 ; Real-mode DS segment

DDDHeader ends


	BREAK	<Code Segment>
Code	segment

	assume	cs:BiosSeg,ds:BiosGroup,es:nothing

	comment !
************************ Start of Specifications *********************
*								     *
* Subroutine Name: DriveRequest 				     *
*								     *
* Descriptine Name: Strategy Routine				     *
*								     *
* Function: route the request to the proper command handler based on *
*	    on the request packet command code			     *
*								     *
* Notes: entered in real and protect mode			     *
*	 entered in kernel mode context 			     *
*	 the kernel has saved its registers			     *
*	 entered enabled					     *
*								     *
* Entry Point: DriveRequest					     *
*								     *
* Linkage:  FAR call						     *
*	    ES:BX - Request Packet				     *
*	    DS	  - BiosGroup					     *
*	    CS	  - BiosSeg					     *
*								     *
* Input: Standard DOS Request Packet				     *
*								     *
* Exit-Normal: indirect jump to the command handler		     *
*								     *
* Exit-Error:  jump to CmdErr					     *
*								     *
* Effects: sets the following registers for the command handlers     *
*								     *
*	      AL    unit number 				     *
*	   DS:DI    BDS entry address (except for the IOCTL command) *
*	   ES:BX    still points to the request packet		     *
*								     *
* Internal References:	see command handlers in dispatch table	     *
*								     *
* External References:	see command handlers in dispatch table	     *
*								     *
************************* End of Specifications ********************** !

	BREAK	<Strategy Routine>
	DebugPublic DriveRequest
DriveRequest proc far

  SaveReg <es,bx>		     ; SetStatus retores these regs
  xor ch,ch			     ; zero ch
  mov cl,es:[bx].PktCmd 	     ; get device command
  TRACEREQUEST			     ; trace deive command if required.
  mov ah,cl			     ; remember command
  cmp cx,ndtable		     ; check command against table limit
  LJA CmdErr			     ;	 command code too large
  shl cx,1			     ; X 2 to make a word index
  mov si,DataOffset dtable	     ; index into dispatch table
  add si,cx			     ;	  ...
  mov al,es:[bx].PktUnit	     ; get unit number
  call Get_BDS_Addr		     ; ds:di -> BDS
  .IF c and			     ; carry set if not found
  .IF <ah ne CMDGenIOCTL> or	     ; IOCTLs can have physical unit numbers
  .IF <ah ne CMDInit>		     ; INIT command does not need a unit
     mov  al,ERROR_I24_BAD_UNIT      ; error return code
     jmp  StatusError		     ; error exit
  .ENDIF			     ;	 ...
  jmp [si]			     ; jump to the command handler

DriveRequest endp

	BREAK <Strategy Routine Dispatch Table>
BioData segment

	public dtable
dtable	label	word			    ; dispatch table
	dw	CodeOffset DriveInit	    ;  0  initialize
	dw	CodeOffset DriveCheck	    ;  1  check media
	dw	CodeOffset DriveBuild	    ;  2  build BPB
	dw	CodeOffset CmdErr	    ;  3  reserved
	dw	CodeOffset DriveRead	    ;  4  read
	dw	CodeOffset StatusDevReady   ;  5  non-destructive read
	dw	CodeOffset StatusComplete   ;  6  input status
	dw	CodeOffset StatusComplete   ;  7  input flush
	dw	CodeOffset DriveWrite	    ;  8  write
	dw	CodeOffset DriveWriteV	    ;  9  write with verify
	dw	CodeOffset CmdErr	    ; 10  get output status
	dw	CodeOffset CmdErr	    ; 11  flush output
	dw	CodeOffset CmdErr	    ; 12  reserved
	dw	CodeOffset StatusComplete   ; 13  open
	dw	CodeOffset StatusComplete   ; 14  close
	dw	CodeOffset DriveRemovable   ; 15  removable media
	dw	CodeOffset DriveGenIOCTL    ; 16  generic IOCTL
	dw	CodeOffset DriveReset	    ; 17  reset uncertain media
	dw	CodeOffset DriveGetMap	    ; 18  get Logical Drive Map
	dw	CodeOffset DriveSetMap	    ; 19  set Logical Drive Map
	dw	CodeOffset CmdErr	    ; 20  de-Install this device
	dw	CodeOffset CmdErr	    ; 21  reserved
	dw	CodeOffset DrivePartNbr     ; 22  get number of partitions
	dw	CodeOffset DriveUnitMap     ; 23  get unit map
	dw	CodeOffset DriveRead	    ; 24  no caching read
	dw	CodeOffset DriveWrite	    ; 25  no caching write
	dw	CodeOffset DriveWriteV	    ; 26  no caching write/verify

ndtable equ	($ - offset dtable - type dtable)/type dtable

BioData ends

	BREAK <Command 1 - MEDIA CHECK>
;**	DriveCheck - check to see if the disk may have been changed.
;
;	ENTRY	AL	unit number
;		ES:BX	Request packet
;		DS:DI	BDS entry address
;
;	EXIT	Return value in request header set to one of:
;			1	Media not changed
;			0	Media may have been changed
;		       -1	Media was probably changed

	DebugPublic DriveCheck
DriveCheck proc

  call	  Do_Check			  ; Perform floppy check
  mov	  byte ptr es:[bx].MedChkflag,dl  ; update request packet
  jmp	  StatusComplete		  ; return status

DriveCheck endp

	BREAK <Command 2 - DriveBuild>
;**	DriveBuild - Build a BPB for the floppy
;
;	ENTRY	ES:BX	Packet address
;		DS:DI	BDS entry address
;		AL	Unit number
;
;	EXIT	ES:BX	Virtual address of request packet
;		BldBPBmedia set to media byte
;		BldBPBpBPB set to a valid BPB for that drive

	DebugPublic DriveBuild
DriveBuild proc

  push es				     ; save ES
  les  si,dword ptr es:[bx].IOpData	     ; passed as virtual ??????
  call BPBFromBoot			     ; DS:DI -> BDS for the drive
  pop  es				     ; restore ES
  .IF c 				     ; valid boot sector BPB ?
     call  BPBFromScratch		     ; no
     LJC StatusError			     ; valid now ?
  .ENDIF				     ;	 ...
  mov ah,[di].BPBmediab 		     ; update request packet
  mov es:[bx].BldBPBmedia,ah		     ;	 ...
  lea di,[di].BPBsecsiz 		     ; ds:di -> BPB in the BDS
  mov word ptr es:[bx].BldBPBpBPB,di	     ;	 ...
  mov word ptr es:[bx].BldBPBpBPB+2,ds	     ;	 ...
  jmp StatusComplete			     ; return to kernel

DriveBuild endp

	BREAK <Commands 4, 8, and 9 - Read, Write, Write verify>
;**	DriveRead, DriveWrite, DriveWriteV - Basic I/O entry points
;
;	DriveRead, DriveWrite and DriveWriteV are the basic I/O
;	routines used by the DOS.  They really do not do much except
;	queue the request and start the device if it is idle.
;
;	For single drive floppy systems, they also handle the
;	switching of disks when I/O changes from A to B or vice-versa.
;
;	ENTRY	ES:BX	packet address
;		DS:DI	BDS entry address
;		AL	Unit #
;
;	EXIT	ES:BX	Packet Addr
;		CX	# of sectors left to do

DriveRead	label	 near
DriveWrite	label	 near
	DebugPublic DriveWriteV
DriveWriteV proc

  test [di].FatSiz,fTOOBig		     ;	 ...
  .IF nz or				     ; invalid partition on drive!!
  test [di].DrvTypFlag,fNoDOSPartition	     ;	 ...
  .IF nz or				     ; invalid partition on drive!!
  mov si,word ptr es:[bx].IOStart	     ; starting sector
  mov cx,es:[bx].IOCount		     ; number of sectors for Disk_IO
  add si,cx				     ; compute last sector + 1
  .IF c or				     ;	  ...
; .IF <word ptr es:[bx].IOStart+2 ne 0> or   ; only do 32 MB stuff here
  .IF <si a [di].BPBnsec >		     ; besure sector in the partition
     mov es:[bx].IOCount,0		     ; zero sectors transferred
     mov al,ERROR_I24_SECTOR_NOT_FOUND
     jmp StatusError			     ; error - sector not found
  .ENDIF

; See if it's for a pseudo-drive and if we need to swap floppies.
;	If we have to swap floppies, FloppyChange will set a semaphore
;	to block further IO requests, and register THIS request as the
;	one to service when the swap is complete.  We return to DOS with
;	a ``Swap Needed'' error code and have DOS prompt the user.

  test word ptr [di].DrvTypFlag,fNon_Removable
  .IF z 				      ; if a floppy disk do
     call FloppyChange			      ; see if should change disks
     .IF <ah eq -1>			      ; ah = -1 if change required
	and  [di].DrvFlag,NOT FMedInd	      ; media has or may have changed
	mov  es:[bx].IOCount,0		      ; zero sectors transferred
	mov  al,ERROR_I24_DISK_CHANGE	      ; error code
	jmp  StatusError		      ; error - disk swap needed
     .ENDIF				      ;   ...
     test [di].DrvFlag,FForceRdWrt	      ; skip check if force on
     .IF z				      ; else check for media change
	test [di].DrvFlag,FUncert_Media       ; check uncertain media flag
	jnz flrw3			      ; uncertain media flag set?
	call Media_Check		      ; Do_Check on AT
	dec  dl
	jz   flrw5
flrw3:
	   mov es:[bx].IOCount,0	      ; zero sectors transferred
	   mov al,ERROR_I24_UNCERTAIN_MEDIA   ; error code
	   jmp StatusError		      ; error - uncertain media
     .ENDIF				      ;   ...
  .ENDIF				      ;   ...
flrw5:
  call Add_Partition_Offset		      ; convert to physical sector
  push dx				      ;* save it
  mov ax,[di].BPBhidsec			      ; add hidden sectors
  mov dx,[di].BPBhidsecHi		      ;* add MSW of hidden sectors
  add word ptr es:[bx].IOPhysRBA,ax	      ;   not treated as double word
;*  adc word ptr es:[bx].IOPhysRBA+2,0	      ;   ...
  adc word ptr es:[bx].IOPhysRBA+2,dx	      ;* TREAT AS DOUBLE WORD
  pop dx				      ;* restore dx

  and  [di].DrvFlag,NOT FForceRdWrt	      ; only one shot at this
  call Disk_IO				      ; Perform the operation
  jmp  SetStatus			      ; Disk_IO put status in ax

DriveWriteV endp

	BREAK <Command 15 - REMOVABLE MEDIA>
;**	DriveRemovable - Removable Media call for floppy drive.
;
;	DriveRemovable returns an indication that the media in
;	the specified drive is removable.  This is indicated by
;	the BUSY bit in the status word.  We set the bit for media
;	that is non-removable.
;
;	ENTRY	AL	unit number
;		ES:BX	request packet
;		DS:DI	BDS entry address
;
;	EXIT	ES:BX	request packet

	DebugPublic DriveRemovable
DriveRemovable proc

  test	word ptr [di].DrvTypFlag,fNon_Removable  ; floppy or hard disk ?
  LJNZ	StatusDevReady				 ; set STDON and STBUI
  jmp	StatusComplete				 ; floppy disk

DriveRemovable endp

	BREAK	<Command 17 - RESET UNCERTAIN MEDIA>
;**	DriveReset - Reset the Uncertain Media flag.
;
;	Reset the uncertain media flag
;
;	ENTRY	ES:BX	request packet
;		DS:DI	BDS entry address
;		   AL	unit number
;
;	EXIT	StatusComplete

	DebugPublic DriveReset
DriveReset proc

  and  [di].DrvFlag,NOT FUncert_Media	   ; Zero out uncertain flag
  and  [di].DrvFlag,NOT FMedInd 	   ; Medium may have changed
  or   [di].DrvFlag,FForceRdWrt 	   ; let next request fly
  and  [di].DrvTypFlag,NOT (fChanged_By_Format + fChanged)
  test [di].DrvTypFlag,fNon_Removable	   ; is it a diskette ?
  .IF z 				   ;   ...
     call Do_ResetUM			   ; tell state the state machine
  .ENDIF				   ;   ...
  jmp  StatusComplete			   ;   ...

DriveReset endp

    BREAK <Command 18 - Get Logical Drive Map>
;**   DriveGetMap - Get the logical drive mapped on this physical drive
;
;     DriveGetMap returns the logical drive that is currently mapped on to
;     the physical drive represented by the logical drive in AL. If there is
;     only one drive mapped, then a zero is returned.
;
;     Returns DriveLet which is DriveNum + 1
;
;     ENTRY	AL	Unit#
;		ES:BX	Request Packet
;		DS:DI	BDS entry address
;
;     EXIT	PktUnit field in Request Packet is set to the correct drive #.

	DebugPublic DriveGetMap
DriveGetMap proc

  mov  cl,[di].DriveNum 		     ; get physical drive number
  mov  si,Start_BDS			     ; BDS chain anchor
  .WHILE <si ne -1> and 		     ; -1 = end of chain
  .WHILE <<byte ptr [si].DriveNum> ne cl> and
  test	[si].DrvTypFlag,fI_Own_Physical      ; want this on
  .WHILE nz				     ;	 ...
     mov  si,[si].Link			     ; try the next BDS entry
  .ENDWHILE				     ;	 ...

; If there is only one drive mapped onto this physical drive, we must return
; zero instead of the drive number.

  .IF <si ne -1> and			     ; be sure we have a BDS entry
  test	[si].DrvTypFlag,fI_Am_Mult	     ; is it mapped?
  .IF nz				     ; more than one drive mapped
     mov  ch,[si].DriveLet		     ; return DriveLet
     mov  es:[bx].PktUnit,ch		     ;	 if mapped
  .ELSE 				     ;	 ...
     mov  es:[bx].PktUnit,0		     ; return 0 if not mapped
  .ENDIF				     ;	 ...
  jmp  StatusComplete			     ; return to DOS

DriveGetMap endp

	BREAK <Command 19 - Set Logical Drive Map>
;**   DriveSetMap - Map the drive specfied onto the physical drive
;
;     DriveSetMap makes the drive specified the current drive mapped onto
;     the physical drive represented by the drive in AL.
;
;     ENTRY	AL	Unit#
;		ES:BX	Request Packet
;		DS:DI	BDS entry address
;
;     EXIT	PktUnit field in Request Packet is set to the correct drive #.
;		Set to 0 if there is only one drive mapped onto the physical
;		drive.

	DebugPublic DriveSetMap
DriveSetMap proc

  call	Update_Owner			     ; update fI_Own_Physical
  test	[di].DrvTypFlag,fI_Am_Mult	     ; is it mapped?
  .IF nz				     ; more than one drive mapped
     mov  ah,[di].DriveLet		     ; return DriveLet
     mov  es:[bx].PktUnit,ah		     ;	 if mapped
  .ELSE 				     ;	 ...
     mov  es:[bx].PktUnit,0		     ; return 0 if not mapped
  .ENDIF				     ;	 ...
  jmp  StatusComplete			     ; return to DOS

DriveSetMap endp

	BREAK <Disk_IO - common disk access routine>
;**	Disk_IO - Common disk access routine
;
;	Disk_IO is the common routine for all operations that need
;	to be performed on a device. It really does not do much except
;	to queue the request and start the device if it is idle.
;
;	For single drive floppy systems, they also handle the
;	switching of disks when I/O changes from A to B or vice-versa.
;
;	ENTRY	ES:BX	Packet address
;		CX	Number of sectors
;		DS:DI	Pointer to BDS for drive
;
;	EXIT	ES:BX	Packet Addr
;		AX	Status of operation

	public Disk_IO
Disk_IO proc

; Check to see if the request is for zero sectors, and if
; so simply return.  We must do this as the LAST check to
; make sure that various status flags are properly set (if
; we had a floppy change error, and the retry was for
; zero bytes in length, we would not release the floppy
; change semaphore).

  .IF <cx eq 0> 			       ; if zero sectors
     mov ax,STDON			       ; treat as a finished request
     or  es:[bx].PktStatus,STDON	       ; treat as a finished request
     ret				       ; quit right here
  .ENDIF				       ;   ...

; If SemInt13 is set then someone is wanting to do a direct INT 13H.
; We block until SemInt13 clears so we don't keep filling the queue
; while he's waiting.

  .IF <SemInt13 ne 0>			 ; is it held ?
     push    bx 			 ; yes
     mov     bx,DataOffset SemInt13	 ;   ...
     call    SWClr			 ; Wait for semaphore to clear
     pop     bx 			 ;   ...
  .ENDIF				 ;   ...

; The request is in range (on the disk) and if we're single floppy
; the proper pseudo-drive is active.
; Determine if floppy or hard disk request and put the request on the
; appropriate queue.

  SaveReg <es,bx>				;   ...
  test word ptr [di].DrvTypFlag,fNon_Removable	; floppy or hard disk ?

;=======================================================================
;			   floppy disk I/O
;=======================================================================

  .IF z 				   ; floppy disk handling
     .IF <es:[bx].PktCmd ne DISKOP_CHKCHNG>; special state machine command
	mov  si,word ptr es:[bx].IOpData   ; si is low word of phys address
	call CheckWrap			   ; reserve scratch buffer if needed
     .ENDIF				   ;   ...
     mov si,DataOffset Drive.Queue	   ; ds:si -> head of queue
     DISABLE				   ; interrupts off while testing state
     DEVHLP DevHlp_SortRequest		   ; add request to list
     .IF <Drive.Active eq 0>		; is driver active?
	mov  TimerActive,0		; clear any active floppy timers
	mov  Drive.State,Start		; want to start I/O
	SaveReg bx			; save request packet address
	mov  bx,DataOffset Drive.Active ; get the semaphore
	call SWait			; We know it's clear, set it.
	RestoreReg bx
	ENABLE				;   ...
	call DriveExecute		; Start up the state machine
     .ENDIF				;   ...

;=======================================================================
;			     hard disk I/O
;=======================================================================

  .ELSE 	    ; ** fixed disk processing ** ;

     cmp  CacheActive, 1		; check to see if cache is active
     jne  cache_not_active
					; yes so call CacheIO
     call  CacheIO			; returns 0 = request is not complete
					;     non-0 = request is complete
     cmp   ax, 0			;
     je    request_not_satisfied	;
					;
     or    es:[bx].PktStatus,STDON	; set done bit
     jmp   short request_satisfied	;

 request_not_satisfied:
 cache_not_active:

     mov si,DataOffset Fixed.Queue	; put request packet in the queue
     DISABLE				; interrupts off while testing state
     DEVHLP DevHlp_SortRequest		;   ...
     .IF <Fixed.Active eq 0>		; Is driver active?
	mov Fixed.State,Start		; Want to start I/O
	SaveReg bx			; save request packet address
	mov  bx,DataOffset Fixed.Active ; get the semaphore
	call SWait			; we know it's clear, set it.
	RestoreReg bx			;   ...
	ENABLE				;    ...
	call FixedExecute		; start up the state machine
     .ENDIF				;   ...
  .ENDIF				;   ...

request_satisfied:

  RestoreReg <bx,es>			; es:bx -> request packet

  DISABLE				; prevent races on completion check
  test es:[bx].PktStatus,STDON		; is the request complete ?
  .IF z 				;   ...
     mov es:[bx].PktStatus,STBUI	; Request not done
  .ENDIF				;   ...
  mov  ax,es:[bx].PktStatus		; Need AX = status
					; the ENABLE is done in "SetStatus"
  ret					;   ...

Disk_IO endp

	Break <Do_DiskIO>
;**	Do_DiskIO - Perform the I/O, blocking until done
;
;	Do_DiskIO makes a call to DISK_IO, and blocks this process until the
;	operation is complete, i.e. the STDON bit is set in the request packet
;
;	ENTRY	ES:BX	Packet address
;		AL	Unit #
;		CX	Sector count
;		DS:DI	Pointer to BDS for drive
;
;	EXIT	ES:BX	Packet Addr
;		AX	Status of operation

	public Do_DiskIO
Do_DiskIO proc

  mov  es:[bx].PktStatus,0		; zero packet status = not done
  call Disk_IO				; perform the I/O operation
  DISABLE				; prevent completion check races
  .REPEAT				; block until done
     test es:[bx].PktStatus,STDON	; is it finished ?
     .LEAVE nz				; IO completed
     mov ax,es				; ax:bx = request packet
     SaveReg bx 			;    ...
     call PBlock			; block until IO is done
     DISABLE				; prevent a race - see BLOCK in spec
     RestoreReg bx			; BLOCK does an ENABLE
  .UNTIL				; repeat until leave takes
  mov ax,es:[bx].PktStatus		; get packet status
  ENABLE				;   ...
  ret					;   ...

Do_DiskIO endp

	BREAK <DoneRequest>
;**	DoneRequest - Mark a request complete, setup to start next one
;
;	DoneRequest does common processing needed when a request
;	has been completed.  It will reset the device state,
;	dequeue the request, mark it complete, restart the
;	process and restart any process waiting on ScratchBuffer
;	if this request had reserved it.
;
;	ENTRY	DS:DI IOStruc for completing operation
;
;	EXIT	ES:BX set to request packet
;
;	USES	AX, BX, ES

	public DoneRequest
DoneRequest proc

  SaveReg <dx,si>			;   ...
  mov [di].ErrCnt,0			; reset error count
  .IF <InitTime ne 0>			; can't use DevHlp during INIT
     lea si,[di].Queue			; get offset to queue anchor
     DEVHLP DevHlp_PullRequest		; carry set if no request
     .REPEAT				; loop on error
	nop				; need error recovery here
     .UNTIL nc				;   ...
  .ELSE 				; during INIT, take the queue
     les bx,[di].Queue			; anchor
     mov word ptr [di].Queue,0		; set the queue to empty
     mov word ptr [di].Queue+2,0	;   ...
  .ENDIF				;   ...

  mov ax,[di].Count			; get I/O left to do
  sub es:[bx].IOcount,ax		; adjust requested count by residual
  or  es:[bx].PktStatus,STDON		; set done bit
  .IF <InitTime ne 0>			; spinning in Spin_IO during INIT
     test [di].Flags,fIOCTL+fINTERNAL	; who did the block, DOS or us?
     .IF z				; z = DOS did the block
	TRACERESULT2   ;; command sequence tracing macro for debug.
	DEVHLP DevHlp_DevDone		; start up blocked processes
     .ELSE				; nz = we did the block
	mov  ax,es			; issue a PRun for our blocks
	call PRun			; ax:bx = request completed
     .ENDIF				;   ...
  .ENDIF				; InitTime loops in Spin_IO
  .IF <<word ptr [di].Queue+2> ne 0>	; is the queue empty ?
     mov [di].State,START		; queue not empty, start next request
  .ELSE 				;   ...
     mov [di].State,IDLE		; queue is empty
     .IF <InitTime ne 0>		; bypass durint INIT
	SaveReg bx			;   ...
	lea  bx,[di].Active		; ds:bx -> request semaphore
	call SSig			; release the semaphore
	RestoreReg bx			;   ...
     .ELSE				; InitTime does no need semaphore
	mov [di].Active,0		;   just zero out active flag
     .ENDIF				;   ...
  .ENDIF				;   ...
  RestoreReg <si,dx>			;   ...
  ret					;   ...

DoneRequest	endp

;** JustReturn - a far return
;
; The address of JustReturn is placed in the device header as the
; interrupt handler routine.

JustReturn proc far			; do a far return
  ret					;   ...
JustReturn endp 			;   ...

Code	ends

	end
