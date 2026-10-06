;	SCCSID = @(#)abdsk5.asm	5.4 87/03/02
	page	,132
	title	ABDISK5 - MS OS/2 Partitioning support for > 32MB
	name	ABDISK5
	comment !
********************* Start of Specifications *************************
*								      *
*  Source File Name: ABDSK5.ASM 				      *
*								      *
*  Descriptive Name: Partitioning support for > 32MB		      *
*								      *
*  Copyright:							      *
*								      *
*  Status: Version 1.1						      *
*								      *
*  Function: Provides initialization of logical BDSs for	      *
*	     DOS partitions (primary and extended).		      *
*								      *
*	     Also contains CMD 22, return # of partitionable	      *
*	     fixed disks, and CMD 23, return logical unit map	      *
*	     for a given fixed disk.				      *
*								      *
*  Notes:							      *
*    Dependencies:						      *
*    Restrictions:						      *
*								      *
*  Entry Points:  Setup_Partition_BDSs - initialize logical BDSs      *
*		  DrivePartNbr - CMD 22, return # of fixed disks      *
*		  DriveUnitMap - CMD 23, return logical unit map      *
*								      *
*  External References:  See EXTRN statements below		      *
*								      *
********************** End of Specifications ************************** !

	.286p

	include absegs.inc	; code and data segment definitions



	BREAK <External code entry points>

; Routines located in ABSUBR.ASM

Code	segment
	extrn	Process_Partition:near
	extrn	Process_Boot:near
	extrn	VirtToPhysESBX:near
	extrn	CHS_to_RBA:near
	extrn	Is_BPB_Boot:near
	extrn	BootBPB_To_BDSBPB:near
	extrn	StatusComplete:near
	extrn	StatusError:near
Code	ends

; Routines located in ABINIT.ASM

CodeInit    segment
	extrn	Spin_IO:near
CodeInit    ends




	BREAK <External data references>
BioData segment

; Data located in ABDSK1.ASM
	extrn	Start_BDS:word
	extrn	LDriveB:word
	extrn	LDriveC:word
	extrn	Drive80:word
	extrn	Drive81:word
	extrn	ScratchBuffer:word
	extrn	ScratchBufPhys:dword	    ; physical address of ScratchBuffer

; Data located in ABDSK3.ASM/ATDSK3.ASM
	extrn	Fixed:byte			; Fixed IOStruc
	extrn	NumFix:byte			; Number of fixed disks

BioData ends

	.xlist
	include struc.inc			; structured macros
if1
	include abmac.inc			; private macros
endif
	include abdskequ.inc			; general equates
	include abddrv.inc			; IOStruc
	include abdebug.inc			; debug and listing control
	include bpb.inc 			; bpb structure
	include error.inc			; error codes

	ListLevel %ablistlevel,%abdebug



	BREAK	<Data Segment>
BioData segment

	public	NumPart
NumPart db	0				; number of partitions found

	public	LastLogicalBDS
LastLogicalBDS	dw   DataOffset LDriveB 	; last logical BDS so far

	public	NextFreeBDS
NextFreeBDS	dw  DataOffset LDriveC		; next available BDS

BioData ends



	BREAK	<DataInit Segment - throw away data>
DataInit segment

MBR_PartTable_80  db   66 dup (?)		; to save partition table 80H
MBR_PartTable_81  db   66 dup (?)		; to save partition table 81H
Next_EBR	  dd	0			; RBA ptr to next EBR

InitReadPkt	  Packet <26,,CMDINPUT> 	; dummy request packet

DataInit ends



	BREAK	<CodeInit Segment>

CodeInit    segment
	assume	cs:BiosSeg,ds:BiosGroup,es:nothing


	BREAK <Setup_Partition_BDSs>
	comment !
************************ Start of Specifications **********************
*								      *
* Subroutine Name: Setup_Partition_BDSs 			      *
*								      *
* Descriptive Name: Installs a BDS for each DOS partition found       *
*		    on the fixed disks 80H and 81H.		      *
*								      *
* Function: This routine searches the fixed disks 80H and 81H	      *
*	    looking for a primary partition and extended DOS volumes  *
*	    on each disk.					      *
*								      *
*	    A BDS is installed first for the primary partition on     *
*	    80H then for the primary partition on 81H.	Then BDSs     *
*	    are installed for extended volumes found on 80H and       *
*	    81H.  If 80H did not have a primary partition, it	      *
*	    will not be searched for extended volumes.	81H will      *
*	    always be searched for extended volumes whether it	      *
*	    had a primary partition or not.			      *
*								      *
* Notes:    Entered in protected mode during initialization.	      *
*	    The physical BDSs for 80H and 81H must be set up	      *
*	    and NumFix must be set to the number of fixed	      *
*	    disks prior to calling this routine.		      *
*								      *
* Entry Point: Setup_Partition_BDSs				      *
*								      *
* Linkage:  CALL						      *
*	    DS	  -   BiosGroup 				      *
*								      *
* Input: None							      *
*								      *
* Exit-Normal: BDSs installed for all partitions found		      *
*								      *
* Exit-Error:							      *
*								      *
* Effects:  Adds BDSs into BDS chain after DriveB and before Drive80  *
*	    Modifies NextFreeBDS pointer			      *
*	    Modifies NumPart					      *
*								      *
* Internal References:						      *
*								      *
* External References:						      *
*								      *
************************* End of Specifications *********************** !

	Public	   Setup_Partition_BDSs
Setup_Partition_BDSs proc

    push es					; save es
    pusha					; save registers

    .IF <NumFix ge 1> near			; is there a fixed disk?

	; Call Read_Sector to read the MBR (RBA 0) off disk 80H
	; into ScratchBuffer.
	;
	; Input:    DS:SI -> Physical BDS
	;	    DXAX  =  RBA to read
	;
	; Output:   'C' clear if read was ok
	;	    ES:BX -> ScratchBuffer containing sector read

	mov  si, Drive80			; si -> physical BDS
	xor  ax, ax				; low half of RBA 0
	xor  dx, dx				; high half of RBA 0
	call Read_Sector			; read in the MBR

	.IF nc					; was the read ok?

	    ; Read was ok, so save MBR partition table for future use

	    mov  si, bx 			; load source offset
	    add  si, 1BEH			; offset to part table
	    mov  di, DataOffset MBR_PartTable_80 ; load dest offset
	    mov  cx, 66 			; will move 66 bytes
	    cld 				; DF = 0, to move forward
	    rep  movsb				; copy the data

	    ; Call Build_Next_BDS to build a BDS for the primary
	    ; partition on drive 80H, if there is one.
	    ;
	    ; Input:	DS:SI -> Physical BDS
	    ;		DXAX  =  RBA partition sector offset
	    ;		ES:BX -> partition sector read from disk
	    ;
	    ; Output:	'C' clear if BDS was installed

	    mov  si, Drive80			; si -> physical BDS
	    xor  ax, ax 			; low half of RBA 0
	    xor  dx, dx 			; high half of RBA 0
	    xor  cl, cl				;* first primary partition
	    call Build_Next_BDS 		; set up primary partition

	.ENDIF

	; 'C' will be clear if 80H had a primary partition

	pushf					; save the 'C' flag

	.IF <NumFix eq 2>			; is there another disk?

	    ; Call Read_Sector to read the MBR (RBA 0) off disk 81H
	    ; into ScratchBuffer.
	    ;
	    ; Input:	DS:SI -> Physical BDS
	    ;		DXAX  =  RBA to read
	    ;
	    ; Output:	'C' clear if read was ok
	    ;		ES:BX -> ScratchBuffer containing sector read

	    mov  si, Drive81			; si -> physical BDS
	    xor  ax, ax 			; low half of RBA 0
	    xor  dx, dx 			; high half of RBA 0
	    call Read_Sector			; read in the MBR

	    .IF nc				; was the read ok?

		; Read was ok, so save MBR partition table for future use

		mov  si, bx			; load source offset
		add  si, 1BEH			; offset to part table
		mov  di, DataOffset MBR_PartTable_81 ; load dest offset
		mov  cx, 66			; will move 66 bytes
		cld				; DF = 0, to move forward
		rep  movsb			; copy the data

		; Call Build_Next_BDS to build a BDS for the primary
		; partition on drive 81H, if there is one.
		;
		; Input:    DS:SI -> Physical BDS
		;	    DXAX  =  RBA partition sector offset
		;	    ES:BX -> partition sector read from disk
		;
		; Output:   'C' clear if BDS was installed

		mov  si, Drive81		; si -> physical BDS
		xor  ax, ax			; low half of RBA 0
		xor  dx, dx			; high half of RBA 0
		xor  cl, cl			;* first primary partition
		call Build_Next_BDS		; set up primary partition

	    .ELSE				; read failed
		dec  NumFix			; ignore this disk
	    .ENDIF

	.ENDIF

	popf					; restore flags

	.IF nc					; did 80H have a primary part?

	    ; Call Setup_Extended_Volumes to search disk 80H for extended
	    ; volumes and install BDSs for any found.
	    ;
	    ;  Input:	DS:SI -> physical BDS of disk to search
	    ;		ES:BX -> Master Boot Record read and saved earlier
	    ;
	    ;  Output:	None

	    mov  si, Drive80			 ; si -> physical BDS
	    mov  bx, DataOffset MBR_PartTable_80 ; bx -> MBR part table on 80H
	    sub  bx, 1BEH			; to look like a whole MBR
	    call Setup_Extended_Volumes 	; install BDSs for ext. vols

	.ENDIF

	.IF <NumFix eq 2>			; is there a second disk?

	    ; Call Setup_Extended_Volumes to search disk 81H for extended
	    ; volumes and install BDSs for any found.
	    ;
	    ;  Input:	DS:SI -> physical BDS of disk to search
	    ;		ES:BX -> Master Boot Record read and saved earlier
	    ;
	    ;  Output:	None

	    mov  si, Drive81			; si -> physical BDS
	    mov  bx, DataOffset MBR_PartTable_81 ; bx -> MBR part table on 80H
	    sub  bx, 1BEH			; to look like a whole MBR
	    call Setup_Extended_Volumes 	; install BDSs for ext. vols

	.ENDIF

;* ****** BEGIN INSERTED CODE ******
;* 
;*  The following code searches for additional primary partitions
;*  on each disk and sets up BDSs for any that are found.  This 
;*  allows a fixed disk that has been set up with COMPAQ's DOS 3.x
;*  ENHDISK scheme to be fully utilized under OS/2.  Note that this 
;*  partition search is done after the Extended Partition(s) have 
;*  been set up.  Assuming two fixed disks, the drive letter ordering
;*  is thus:  (first) Primary DOS Partition on drive 80h is drive C:,
;*  (first) Primary DOS Partition on drive 81h is drive D:, next
;*  are any Extended DOS Volumes in an Extended DOS Partition on
;*  drive 80h, then any Extended DOS Volumes on drive 81h.  Finally,
;*  drive 80h, then drive 81h are searched for ENHDISK partitions.
;*  The main reason for this sequence is for compatibility with 
;*  DOS 3.x.  If the COMPAQ ENHDISK driver is used under DOS 3.30,
;*  this drive ordering will result, since DOS 3.30 assigns all the
;*  Extended Partition drives before ENHDISK gets a chance to look
;*  for extra DOS partitions in the Master Boot Record.
;*

	; Call Setup_Other_Partitions to search disk 80H for additional 
	; primary partitions and install BDSs for any found.
	;
	;  Input:	DS:SI -> physical BDS of disk to search
	;		ES:BX -> Master Boot Record read and saved earlier
	;
	;  Output:      None
        
	mov  si, Drive80			; si -> physical BDS
	mov  bx, DataOffset MBR_PartTable_80	; bx -> MBR part table on 80H
	sub  bx, 1BEH				; make it look like a whole MBR
	call Setup_Other_Partitions		; setup BDSs for any additional
						;   primary partitions

	.IF <NumFix eq 2>			; is there a second disk?

	    ; Call Setup_Other_Partitions to search disk 81H for additional 
	    ; primary partitions and install BDSs for any found.
	    ;
	    ;  Input:	DS:SI -> physical BDS of disk to search
	    ;		ES:BX -> Master Boot Record read and saved earlier
	    ;
	    ;  Output:  None

	    mov  si, Drive81			; si -> physical BDS
	    mov  bx, DataOffset MBR_PartTable_81 ; bx -> MBR part table on 80H
	    sub  bx, 1BEH			; make it look like a whole MBR
	    call Setup_Other_Partitions		; setup BDSs for any additional
						;   primary partitions
	.ENDIF
;* 
;* 
;* ****** END INSERTED CODE ******

    .ENDIF

    popa					; restore registers
    pop  es					; restore es
    ret

Setup_Partition_BDSs endp


;* ****** BEGIN INSERTED CODE ******
;* 
	BREAK <Setup_Other_Partitions>
;** Setup_Other_Partitions
;
;   Sets up the BDS entries for extra (i.e. other than the first one) 
;   primary partitions found on a given fixed disk.
;
;   ENTRY  DS:SI -> physical BDS of fixed disk to search
;          ES:BX -> buffer containing Master Boot Record
;
;   EXIT   BDSs installed for extra primary partitions found
;
;   USES   flags

	DebugPublic     Setup_Other_Partitions
Setup_Other_Partitions proc
    pusha				; save registers

    xor  cl, cl				; ((second primary partition) - 1)
Sop_1:
    inc  cl				; next primary partition
    cmp  cl,3				; Q: fourth partition yet ?
    ja   Sop_exit			;   Y: exit
    xor  ax, ax				; low half of RBA 0
    xor  dx, dx				; high half of RBA 0
    call Build_Next_BDS			; set up primary partition
    jnc  Sop_1				; loop to try next if one has been found
Sop_Exit:
    popa
    ret					; *** RETURN ***
Setup_Other_Partitions endp
;* 
;* ****** END INSERTED CODE ******


	BREAK <Build_Next_BDS>
	comment !
************************ Start of Specifications **********************
*								      *
* Subroutine Name: Build_Next_BDS				      *
*								      *
* Descriptive Name: Build a BDS for next logical partition	      *
*								      *
* Function: This routine attempts to build a BDS entry for a logical  *
*	    fixed disk partition.  It is passed a pointer to the      *
*	    physical BDS (80H or 81H) of the disk containing the      *
*	    partition, a pointer to the buffer containing the	      *
*	    Master or Extended boot record (partition sector) read    *
*	    off the disk, and the offset in sectors of that	      *
*	    partition sector from the begining of the disk.	      *
*								      *
*	    It first gets the next available BDS and sets up	      *
*	    the BPBtrksiz and BPBnhead fields from the physical BDS.  *
*	    Process_Partition is then called to validate the	      *
*	    partition sector and see if there is a DOS partition.     *
*	    If one is found, this BDS is put in the chain and	      *
*	    several fields are set up.	The DOS boot sector for       *
*	    the partition is then read in and Process_Boot is	      *
*	    called to examine it and build the BPB in the BDS.	      *
*								      *
*	    If there was no valid DOS partition, the BDS will	      *
*	    remain available.					      *
*								      *
* Notes: Entered in protected mode during initialization	      *
*								      *
* Entry Point: Build_Next_BDS					      *
*								      *
* Linkage:  CALL						      *
*	    DS	  -   BiosGroup 				      *
*	    AX	  -   Low half of partition sector offset (RBA)       *
*	    DX	  -   High half of partition sector offset (RBA)      *
* ;*	    CL    -   Number of primary partition to build (0-based) ;*
*	    SI	  ->  Physical disk BDS (Drive80 or Drive81)	      *
*	    ES:BX ->  Partition sector read from disk		      *
*								      *
* Input: Physical BDS, partition sector offset, partition sector,     *
*								      *
* Exit-Normal: Partition valid; BDS set up;  'C' clear                *
*								      *
* Exit-Error:  Partition invalid; BDS not set up;  'C' set            *
*								      *
* Effects:  May add a BDS into BDS chain			      *
*	    May modify NextFreeBDS pointer			      *
*	    May increment NumPart				      *
*								      *
* Internal References:						      *
*								      *
* External References:						      *
*								      *
************************* End of Specifications *********************** !

	DebugPublic	Build_Next_BDS
Build_Next_BDS proc

    pusha					; save registers

    mov  di, NextFreeBDS			; get next free BDS
    mov  word ptr [di].PartitionOffset, ax	; RBA low half
    mov  word ptr [di].PartitionOffset+2, dx	; RBA high half
    mov  byte ptr [di].PartitionNumber, cl	;* setup partition number

    ; Call Process_Partition to examine the partition table and
    ; find a DOS partition.  The partition's size and location, relative
    ; to its' partition sector, are returned.  BPBhidsec and
    ; BPBnsec are also filled in.
    ;
    ; Input:	DS:DI -> new BDS being built, must have BPBtrksiz and BPBnhead
    ;		ES:BX -> ScratchBuffer containing partition sector
    ;
    ; Output:	'C' clear if a valid partition was found
    ;		AX    =  Cylinder
    ;		CH    =  Head
    ;		CL    =  Sector
    ;		BX    =  partition size in sectors

    mov  ax, [si].BPBtrksiz			; copy sectors/track
    mov  [di].BPBtrksiz, ax			;   into new BDS
    mov  ax, [si].BPBnhead			; copy number of heads
    mov  [di].BPBnhead, ax			;   into new BDS
    call Process_Partition			; look for a DOS partition

    .IF nc near 				; was there a partition?
	mov  ax, bx				; Yes, first figure # of cyls
	xor  dx, dx				; for double word divide
	div  [di].BPBtrksiz			; divide by sectors/track
	xor  dx, dx				; ax now has total tracks
	div  [di].BPBnhead			; divide by number of heads
						; ax now has # of cylinders
	.IF <dx ne 0>				; was there a remainder
	    inc  ax				; round up to next cylinder
	.ENDIF
	mov  [di].cCyln, ax			; put logical cylinders in BDS
	mov  [di].TotalPhysCyln, ax		; physical and logical are the
						;   same for partition BDSs

	mov  ax, Drive80
	mov  [di].Link, ax			; link this new
	mov  bx, LastLogicalBDS 		;   BDS into chain after
	mov  [bx].Link, di			;   the last logical BDS and
	mov  LastLogicalBDS, di 		;   before phys BDS Drive80

	xor  ax, ax				; zero ax
	mov  al, [di].DriveUnit 		; get DriveUnit, 'C:' = 2
	inc  al 				; make 1 based
	mov  [di].DriveLet, al			; store DriveLet, 'C:' = 3

	mov  al, [si].DriveNum			; get int 13H drive number
	mov  [di].DriveNum, al			; save in new BDS

	mov  ax, [si].WrtPrCmp			; get write precom cylinder
	mov  [di].WrtPrCmp, ax			; save in new BDS

	mov  al, [si].RetryCnt			; get retry count
	mov  [di].RetryCnt, al			; save in new BDS

	or   [di].DrvTypFlag, fNon_Removable	; set up DrvTypFlag
	mov  byte ptr [di].BPBmediab, MB_HardDrive ; put media byte in BDS
	mov  byte ptr [di].FormFactor, ffHardFile ; put form factor in BDS
	mov  [di].ParentDisk, si		; save offset to physical BDS
	mov  [di].IOSoffset, DataOffset Fixed	; save offset to Fixed IOStruc

	; Call Read_Sector to read the DOS boot record into ScratchBuffer
	;
	; Input:    DS:SI -> Physical BDS
	;	    DXAX  =  RBA to read
	;
	; Output:   'C' clear if read was ok
	;	    ES:BX -> ScratchBuffer containing sector read

	mov  ax, word ptr [di].PartitionOffset	; RBA low half
	mov  dx, word ptr [di].PartitionOffset+2 ; RBA high half
	add  ax, [di].BPBhidsec 		; add hidden sectors
	adc  dx, [di].BPBhidsecHi		;* (full 32-bit value)
;* 	adc  dx, 0				; allow for carry
	call Read_Sector			; read in boot record

	mov  si, bx				; es:si -> boot sector
	call Is_BPB_Boot			; see if boot sector is good
	.IF nc					; it's a good boot sector
	    SaveReg <es,di>			; save before trashing
	    push ds				; es = ds
	    pop  es				;   ...
	    lea  di,[di].BPBsecsiz		; es:di -> BPB in BDS
	    call BootBPB_To_BDSBPB		; copy boot BPB to media BPB
	    RestoreReg <di,es>			; di -> new BDS
	.ENDIF					; es:bx -> ScratchBuffer

	; Call Process_Boot to examine boot sector and build the BPB.
	;
	; Input:    DS:DI -> new BDS being built
	;	    ES:BX -> ScratchBuffer
	;	    AX	  =  partition size in sectors
	;
	; Output:   BPB set up in the BDS

	mov  ax, [di].BPBnsec			; partition size
	call Process_boot			; fill in BPB in the BDS

	; copy media BPB to recommended BPB

	SaveReg es				; save old es
	push ds 				; es = ds
	pop  es 				;   ...
	lea  si,[di].BPBsecsiz			; ds:si -> media BPB
	lea  di,[di].BPBRsecsiz 		; es:di -> recommended BPB
	mov  cx,size a_BPB			; # bytes to move
	cld					; DF = 0, to move forward
	repe movsb				; copy the BPB
	RestoreReg es				; restore es

	inc  NumPart				; increment NumPart
	add  NextFreeBDS, size DriveStruc	; point to next free BDS
	clc					; BDS was installed; 'C' clear

    .ENDIF

    popa					; restore registers
    ret

Build_Next_BDS endp



	BREAK <Setup_Extended_Volumes>
;** Setup_Extended_Volumes
;
;   Sets up the BDS entries for all extended volumes found
;   on a given fixed disk.
;
;   ENTRY  DS:SI -> physical BDS of fixed disk to search
;	   ES:BX -> ScratchBuffer containing Master Boot Record
;
;   EXIT   BDSs installed for all extended volumes found
;
;   USES   flags

	DebugPublic	Setup_Extended_Volumes
Setup_Extended_Volumes proc

    pusha					; save registers

    ; Call Find_Extended_Boot_Record to look for the
    ; the first EBR and return its' RBA position.
    ;
    ;  Input:	ES:BX -> partition sector to search
    ;		DS:DI -> physical BDS with media characteristics
    ;
    ;  Output:	'C' clear if EBR was found
    ;		DXAX  =  RBA sector offset of EBR

    mov  di, si 				; di -> physical BDS
    call Find_Extended_Boot_Record		; find the first EBR

    .WHILE nc					; while there is another EBR
						;   and disk reads are ok
	; Call Read_Sector to read the EBR into ScratchBuffer
	;
	; Input:    DS:SI -> Physical BDS
	;	    DXAX  =  RBA to read
	;
	; Output:   'C' clear if read was ok
	;	    ES:BX -> ScratchBuffer containing sector read

	call Read_Sector			; read next EBR

	.IF nc					; was read ok?

	    ; Call Find_Extended_Boot_Record to look for the
	    ; the next EBR and return its' RBA position.  Note
	    ; that the previous EBR RBA must be saved before calling
	    ; Find_Extended_Boot_Record and restored afterwards.
	    ;
	    ;  Input:	ES:BX -> partition sector to search
	    ;
	    ;  Output:	'C' clear if EBR was found
	    ;		DXAX  =  RBA sector offset of EBR

	    push ax				; save prev RBA low half
	    push dx				; save prev RBA high half
	    call Find_Extended_Boot_Record	; find the next EBR
	    mov  word ptr Next_EBR, ax		; save next RBA low half
	    mov  word ptr Next_EBR+2, dx	; save next RBA high half
	    pop  dx				; restore prev RBA high half
	    pop  ax				; restore prev RBA low half

	    ; 'C' is clear if another EBR was found

	    pushf				; save 'C' flag

	    ; Call Build_Next_BDS to build a BDS for the extended
	    ; volume, if there is one.
	    ;
	    ; Input:	DS:SI -> Physical BDS
	    ;		DXAX  =  RBA partition sector offset
	    ;		ES:BX -> partition sector read from disk
	    ;
	    ; Output:	'C' clear if BDS was installed

	    push cx				;* 
	    mov  cl, 0				;* first partition
	    call Build_Next_BDS			; set up extended volume BDS
	    pop  cx				;* 

	    mov  ax, word ptr Next_EBR		; get next RBA low half
	    mov  dx, word ptr Next_EBR+2	; get next RBA high half
	    popf				; restore 'C' flag

	.ENDIF

    .ENDWHILE

    popa					; restore registers
    ret

Setup_Extended_Volumes endp



	BREAK <Find_Extended_Boot_Record>
;** Find_Extended_Boot_Record
;
;   Searches a partition table for an extended boot record
;   and returns its' RBA position relative to the
;   Master Boot Record.
;
;   ENTRY  ES:BX -> partition sector to search	    ES = DS
;	   DS:DI -> physical BDS with media characteristics
;
;   EXIT   'C' clear if an EBR was found
;	   AX	 =  low half of RBA
;	   DX	 =  high half of RBA
;
;   USES   flags

	DebugPublic	Find_Extended_Boot_Record
Find_Extended_Boot_Record proc

    SaveReg <bx,cx>				; save registers

    add  bx, 1BEH				; go to first partition entry

    .FOR cx = 1 to 4				; look at 4 partition entries
	.IF <<byte ptr es:[bx]+4> eq 5> 	; is this a type 5 (EBR)

	    ; Get CHS address from this entry and convert to RBA

	    mov  ch, es:[bx]+1			; ch = head
	    mov  cl, es:[bx]+2			; cl = sector + 2 bits of cyl
	    mov  al, cl 			; al = sector + 2 bits of cyl
	    and  cl, 3FH			; zero 2 high bits; cl = sector
	    and  ax, 00C0H			; keep only 2 high bits
	    shl  ax, 2				; move these bits left
	    mov  al, es:[bx]+3			; get low 8 bits of cyl; ax=cyl

	    call CHS_to_RBA			; DXAX = RBA address

	    RestoreReg <cx,bx>			; restore registers
	    clc 				; EBR found, clear 'C'
	    ret 				; return to caller
	.ENDIF
	add  bx, 16				; go to next partition entry
    .NEXT cx

    RestoreReg <cx,bx>				; restore registers
    stc 					; EBR not found, set 'C'
    ret 					; return to caller

Find_Extended_Boot_Record endp



	BREAK <Read_Sector>
;** Read_Sector - performs disk reads during initialization
;
;   Reads a sector from a fixed disk into ScratchBuffer.
;   This routine sets up a hard coded read request packet
;   and calls Spin_IO to perform the read.
;
; ENTRY:    DS:SI -> physical BDS of disk to read
;	    DXAX  -> RBA address of sector to read
;
; EXIT:     ES:BX -> ScratchBuffer address
;	    'C' clear if read was ok

	DebugPublic Read_Sector
Read_Sector proc

    SaveReg di					; save before trashing
    SaveReg <ax,dx>				; save RBA address
    mov  dl, [si].DriveNum			; dl = int 13 number
    mov  InitReadPkt.Pktunit, dl		; block device unit
    mov  InitReadPkt.IOmedia,MB_HardDrive	; media byte

    ; get physical address of ScratchBuffer in ax:bx

    mov  bx, word ptr ScratchBufPhys		; low half of physical address
    mov  ax, word ptr ScratchBufPhys+2		; high half of physical address

    mov  word ptr InitReadPkt.IOpData,bx	; put address in packet
    mov  word ptr InitReadPkt.IOpData+2,ax;	;   ...
    mov  InitReadPkt.IOcount,1			; read one sector
    RestoreReg <dx,ax>				; restore RBA address
    mov  word ptr InitReadPkt.IOstart,ax	; RBA low half
    mov  word ptr InitReadPkt.IOstart+2,dx	; RBA high half
    push ds					; es = ds
    pop  es					;   ...
    mov  bx, DataOffset InitReadPkt		; es:bx -> init read packet
    mov  cx,[bx].IOCount			; # of sectors for Disk_IO
    mov  di, si 				; ds:di -> BDS

    and  [di].DrvFlag,NOT FForceRdWrt		; only one shot at this

    call Spin_IO				; do the read

    clc 					; assume no error
    test es:[bx].PktStatus,STERR		; error ?
    .IF nz					; if error
	stc					;   set carry
    .ENDIF
    mov  bx, ScratchBuffer			; es:bx -> ScratchBuffer
    RestoreReg di				; restore callers di
    ret 					; return

Read_Sector endp

CodeInit    ends

Code	segment


	BREAK <DrivePartNbr>
;** DrivePartNbr
;
;   Puts the number of partitionable fixed disks (NumFix)
;   in the request packet PktData.
;
;   ENTRY  ES:BX -> request packet
;
;   EXIT   number of fixed disks put in request packet PktData
;
;   USES   flags

	public	DrivePartNbr
DrivePartNbr proc

    SaveReg  ax 				; save ax

    mov  al, NumFix				; put NumFix in al
    mov  es:[bx].PktData, al			; store in request packet

    RestoreReg	ax				; restore ax
    jmp  StatusComplete 			; return to DOS

DrivePartNbr endp



	BREAK <DriveUnitMap>
;** DriveUnitMap
;
;   Looks at all the logical BDS entries and builds a
;   double word bit map that indicates what logical drives
;   exist on the given physical drive.	The low order bit of the
;   map corresponds to logical unit 0 (A:).  Bits are set for
;   those units that exist on the given physical drive.
;
;   ENTRY  ES:BX -> request packet
;	   AL	 =  physical drive (0 = first fixed disk)
;
;   EXIT   Double word bit map put in request packet PktData.
;	   The map is stored low word, low address.
;
;   USES   flags

	public DriveUnitMap
DriveUnitMap proc

    .IF <NumFix eq 0>				; if there are no fixed disks
	mov  ax,ERROR_I24_BAD_UNIT		; error return code
	jmp  StatusError			; error exit
    .ENDIF

    SaveReg <ax,cx,dx,di>			; save registers

    add  al, 80H				; make disk 80H based
    xor  dx, dx 				; zero dx for building map
    mov  cx, 0001H				; put initial OR mask in cx

    ; put a -1 in PktData to check later to see if the
    ; 1st word of the map was put in the packet

    mov  word ptr es:[bx].PktData, -1		; put in -1 to check later
    mov  di, Start_BDS				; BDS chain anchor
    .WHILE <[di].DriveUnit ne 80H>		; stop when we get to phys BDSs
	.IF <cx ne 0>				; not at the end of 1st word

	    .IF <[di].DriveNum eq al>		; this unit is on given drive
		or   dx, cx			; turn on the bit
	    .ENDIF

	    shl  cx, 1				; adjust OR mask for next unit
	    mov  di, [di].Link			; get next BDS

	.ELSE					; at the end of 1st word

	    mov  word ptr es:[bx].PktData, dx	; put 1st word in packet
	    xor  dx, dx 			; zero dx for mapping 2nd word
	    mov  cx, 0001H			; reset initial OR mask

	.ENDIF

    .ENDWHILE

    .IF <<word ptr es:[bx].PktData> eq -1>	; did not put 1st word in yet

	mov  word ptr es:[bx].PktData, dx	; put 1st word in packet
	mov  word ptr es:[bx].PktData+2, 0	; put 0 in 2nd word

    .ELSE					; already put in 1st word
	mov  word ptr es:[bx].PktData+2, dx	; put 2nd word in packet
    .ENDIF

    RestoreReg <di,dx,cx,ax>			; restore registers
    jmp  StatusComplete 			; return to DOS

DriveUnitMap endp


Code	ends

	end
