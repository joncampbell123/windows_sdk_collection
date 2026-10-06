;	SCCSID	=  @(#)absubr.asm	5.33 87/03/28
	page	,132
	title	ABSUBR - MS OS/2 disk device driver
	name	ABSUBR
	comment !
********************* Start of Specifications *************************
*								      *
*  Source File Name: ABSUBR.ASM 				      *
*								      *
*  Descriptive Name: disk device driver common subroutines	      *
*								      *
*  Copyright:							      *
*								      *
*  Status: Version 1.1						      *
*								      *
*  Function: Provides miscellaneous common subroutines		      *
*								      *
*  Notes:	      N/A					      *
*    Dependencies:    N/A					      *
*    Restrictions:    N/A					      *
*								      *
*  Entry Points: numerous (will be completed for the I2)	      *
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

; Routines located in ABDSK1.ASM
	extrn Do_DiskIO:near

; Located in Atdsk2.asm and Abdsk2.asm
	extrn TMotorOff:near

Code	ends

	BREAK <External data references>
BioData segment

; Data located in ABDSK1.ASM
	extrn	Start_BDS:word
	extrn	DevHlpFunc:dword
	extrn	ScratchBuffer:word
	extrn	ScratchBufSem:byte
	extrn	ScratchBufPhys:dword

; Data located in ABDSK2.ASM
	extrn	Drive:byte
; Data located in ABDSK4.ASM
	extrn	TimerActive:byte
BioData ends

	.xlist				;
	include struc.inc		; structured macros
if1					;
	include abmac.inc		; private macros
endif					;
	include abdskequ.inc		; general equates
	include abddrv.inc		; IOStruc
	include bpb.inc 		; bios parameter block
	include abioctl.inc		; IOCTL equates
	include devhlp.inc		; device helper definitions

	include dosmac.inc		;
	include error.inc		;
	include ibmrom.inc		;

	ListLevel %ablistlevel,%abdebug

	BREAK	<Data Segment>
BioData segment

; Minimum BPB. This is the BPB set up in the BDS of a fixed disk which has
; no partition. This is done to allow access to the drive so that it may be
; formatted and made useable.
; NOTE: We only need the first 13 bytes. The rest are physical characteristics!

MinimumBPB  dw	    512 		; 512 bytes per sector
	    db	    1			; sectors/cluster
	    dw	    1			; reserved sectors
	    db	    1			; number of FATs
	    dw	    16			; root directory entries
	    dw	    4			; total sectors
	    db	    0F0H		; media descriptor
	    dw	    1			; sectors/FAT
MinBPBSiz   equ     $-MinimumBPB

	public fBigFAT
fBigFAT db	0			; Flags for drives (16 bit fats, etc).

	public	NumFlop
NumFlop 	db	0		; number of diskette drives
	public harderrreqblk		; ptm 3005
harderrreqblk	dd	?		; address of harderr request blk p3005
	public	dmaoverrunflag		; ptm 3005
dmaoverrunflag	db	0ffh		; dmaoverrunflag PTM 3005

; Table of parameters for an unknown hard disk.

;			Number		Sec/Clus	 # of root	Flags
;			Of Sectors			dir entries

;DiskTable2	 DW	 32680, 	 0803H, 	 512,		 0
DiskTable2	DW	20740,		0803H,		512,		0
		DW	65535,		0402H,		512,		fBIG

BioData ends

	BREAK	<Code Segment>
Code	segment

	assume	cs:BiosSeg,ds:BiosGroup,es:nothing


;**	Get_BDS_Addr - Return pointer to BDS for drive
;
;	Get_BDS_Addr returns a pointer to the BDS for drive for the
;	selected unit.
;
;	ENTRY	(AL) = Drive number
;
;	EXIT	DS:DI = Pointer to BDS for the drive
;		'C' set if no BDS found for this drive
;
;	USES	DI, Flags
;
;	ASSUMPTIONS
;		All the BDSs are in the same segment

	public	Get_BDS_Addr
Get_BDS_Addr proc

   mov di,Start_BDS			   ; BDS chain anchor
   .WHILE <di ne -1>			   ; chase the linked list
      .IF <[di].DriveUnit eq al>	   ; is this the unit ?
	 clc				   ; good return code
	 ret				   ; normal exit
      .ENDIF				   ; DS:DI now set
      mov     di,[di].Link		   ; try the next one
   .ENDWHILE				   ;   ...
   stc					   ; BDS entry not found
   ret					   ; error exit

Get_BDS_Addr endp

;**	GetDrivePtr - compute ptr to per drive info.
;
;	GetDrivePtr returns a pointer to the per-drive information
;	for the current drive.	Should not be called before the
;	current drive is set up by Sel765  in state CALC.
;
;	ENTRY	DS = BiosGroup
;
;	EXIT	BX = pointer to per drive table
;		'C' set if no BDS available for this drive
;
;	USES	BX, Flags

	public GetDrivePtr
GetDrivePtr proc

  SaveReg <di,ax>		;   ...
  mov  al,Drive.Current 	; get unit number
  call Get_BDS_Addr		; di -> BDS for drive
  mov  bx,di			; return result in bx
  RestoreReg <ax,di>		;   ...
  ret				;   ...

GetDrivePtr endp

	BREAK <CHS to/from RBA conversions>
;** Convert_CHS_RBA
;
;   Calls CHS_to_RBA to convert the CHS address in IOStruc to RBA and
;   saves the result in the IOStruc.First.
;
;   ENTRY  DS:SI IOStruc with Cyl, Head, and Sector fields as input
;	   DS:DI DriveStruc for device
;
;   EXIT   IOStruc First is updated
;
;   USES   flags

	public Convert_CHS_RBA
Convert_CHS_RBA proc

  pusha 				 ;   ...
  mov ax,[si].Cyl			 ; Cyl * heads
  mov ch,[si].Head			 ; head * sectors/track
  mov cl,[si].Sector			 ;   ...
  call CHS_to_RBA			 ;   ...
  mov word ptr [si].First,ax		 ; lsb
  mov word ptr [si].First+2,dx		 ; msb
  popa					 ;   ...
  ret					 ;   ...

Convert_CHS_RBA endp

;** CHS_to_RBA
;
;   Converts a Cylinder/Head/Sector address to a double word
;   Relative Block Address using this formula:
;
;	((cyl * BPBnheads) + head) * BPBtrksize) + (sector - 1) = RBA
;
;
;   ENTRY  DS:DI BDS entry containing media characteristics
;	   AX	 Cylinder (zero based)
;	   CH	 Head (zero based)
;	   CL	 Sector (one based)
;
;   EXIT   AX	 Low half of RBA
;	   DX	 High half of RBA
;
;   USES   flags

	public CHS_to_RBA
CHS_to_RBA proc

  SaveReg bx				; save before trashing
  mul [di].BPBnhead			; cylinder * heads
  xor bh,bh				; zero bh
  mov bl,ch				; put head in bx
  add ax,bx				; add head #
  mul [di].BPBtrksiz			; mult by trksize -> dx ax
  mov bl,cl				; put sector in bx
  dec bx				; make sector 0 based
  add ax,bx				; add sector
  adc dx,0				; allow for carry
  RestoreReg bx 			; dx ax now has RBA
  ret

CHS_to_RBA endp

	BREAK <Add_Partition_Offset>
;** Add_Partition_Offset - convert logical RBA to physical RBA for
;			   partitions
;
;   Add_Partition_Offset will convert the request packet starting sector
;   value to a physical sector address for fixed disk logical partitions
;
;   ENTRY  DS:DI - BDS entry
;	   ES:BX - request packet
;
;   EXIT   updates/changes starting sector field in the request packet
;	   restores all registers, changes the flags

	public Add_Partition_Offset
Add_Partition_Offset proc

  SaveReg ax				     ;	 ...
  mov ax,word ptr es:[bx].IOstart	     ; add lsb of logical sector
  add ax,word ptr [di].PartitionOffset	     ;	 to physical offset
  mov word ptr es:[bx].IOPhysRBA,ax	     ;	 ...
  mov ax,word ptr es:[bx].IOstart+2	     ; add msb + carry
  adc ax,word ptr [di].PartitionOffset+2     ;	 ...
  mov word ptr es:[bx].IOPhysRBA+2,ax	     ;	 ...
  RestoreReg ax 			     ;	 needs another field in the
  ret					     ;	 packet

Add_Partition_Offset endp

	BREAK <Request packet routines>
;**	Get_Request_Packet - Allocate a device driver Request Packet
;
;	This routine will allocate a request packet from the pool
;	of available request packets.
;
;	ENTRY	NONE
;
;	EXIT	(ES:BX) = Address of a request packet in the DOS
;
;	USES	BX, ES,  Flags

	public Get_Request_Packet
Get_Request_Packet proc

    SaveReg dx
    xor     dh,dh		      ; Wait if none available
    DEVHLP  DevHlp_AllocReqPacket
    RestoreReg dx
    ret

Get_Request_Packet endp

;***	Release_Request_Packet - Release a DOS Request packet
;
;	This routine will release a request packet that had been
;	previously allocated by the DOS using the DevHlp_AllocReqPacket.
;
;	ENTRY	(ES:BX) = Address of a request packet
;
;	USES	Flags

	public Release_Request_Packet
Release_Request_Packet proc

   SaveReg dx
   DEVHLP  DevHlp_FreeReqPacket
   RestoreReg dx
   ret

Release_Request_Packet endp

	BREAK <BPBFromBoot>
;**	BPBFromBoot - Get pointer to Floppy BPB
;
;	BPBFromBoot builds a BPB for the disk/media type from the boot
;	sector passed to it. If the boot sector does not contain a valid
;	BPB, it returns a Carry.
;
;	ENTRY	AL	= Unit number
;		ES:SI	= Copy of the boot sector on the current drive
;		DS:DI	= BDS entry address
;
;	EXIT	'C' clear
;		DS:DI = Pointer to BPB
;		'C' set if boot sector does not contain a valid BPB
;
;	USES	Flags
;
;	NOTE	This code assumes that the media you are attempting to
;		build on is
;		    a)	a DOS disk
;		    b)	if it is a >2.x disk, the boot sector
;			lies at sector 1 on track 0 (head 0).  The first
;			3 bytes are a JMP, the next 8 bytes are a media
;			ID ASCII string, and the 11 bytes following that
;			are the BPB for that disk.
;		    c)	if the above conditions (3 byte jump, 8 ascii ID
;			string) are not met, then we assume the media is
;			a DOS 1.x disk.  On a 1.x disk sector 2 on
;			track 0 (head 0) contains the first FAT sector.
;			The first byte of the first FAT sector contains
;			the "FAT ID byte" which is the media ID byte for
;			this disk.

	public	BPBFromBoot
BPBFromBoot proc

  test [di].DrvTypFlag,RETURN_FAKE_BPB	 ; do nothing if this is set
  .IF z 				 ; build a new BPB
     call  Is_BPB_Boot			 ; test boot sector
     RETC				 ; boot sector has invalid BPB
     SaveReg <es,si,ds,di>		 ;   ...
     push es				 ; swap es and ds
     push ds				 ;   ...
     pop  es				 ; es:di -> BDS entry
     lea  di,[di].BPBsecsiz		 ; es:di -> BDS entry media BPB
     pop  ds				 ; ds:si -> passed boot record
     call BootBPB_To_BDSBPB		 ; copy boot rec BPB to media BPB
     RestoreReg <di,ds,si,es>		 ;   ...

; In pre-DOS 3.20 versions of Format, there was a bug that caused the sectors
; per cluster field in the BPB in the boot sector to be set to 2 even when
; the medium was single-sided. We "fix" this by setting the field to 1 on the
; diskettes that may have been affected. This will ensure that whatever has
; been recorded on those media will remain intact.

     test [di].DrvTypFlag,fNon_Removable       ;   ...
     .IF z				and    ; only do for floppies
     .IF <<byte ptr es:[si].8>	be '3'> and    ; older than 3.2
     .IF <<byte ptr es:[si].10> be '2'> and    ; single sided
     .IF <<byte ptr [di].BPBMediab> eq MB_48_8Sect1S> or
     .IF <<byte ptr [di].BPBMediab> eq MB_48_9Sect1S>
	mov byte ptr [di].BPBsecpau,1	       ; do the update
     .ENDIF				       ;   ...
  .ENDIF				       ;   ...
  clc					       ; return no error
  ret					       ;   ...

BPBFromBoot endp



	BREAK <BootBPB_To_BDSBPB>
;**  BootBPB_To_BDSBPB - Copies the BPB from a boot sector to BDS BPB
;
;    Copies the BPB from the passed boot sector to the media BPB.
;    If the boot record is older than DOS 3.2, the 12 byte
;    extended BPB is not copied because it may contain garbage.
;
;*   Modified for support of 32-bit Hidden Sector value
;
;	ENTRY:	ES:DI -> media BPB in BDS
;		DS:SI -> start of boot sector
;
;	EXIT:	Boot BPB copied to media BPB
;
;	REGISTERS MODIFIED: CX, DI, SI

	public	BootBPB_To_BDSBPB
BootBPB_To_BDSBPB  proc

BPBoffset equ 11				; offset in boot rec to BPB
						;
    push es:[di.BPBhidsec-BPBsecsiz]	;* save 32-bit hidden sector value that
    push es:[di.BPBhidsecHi-BPBsecsiz]	;*   was set up by Process_Partition 
    mov  cx, size a_BPB 			; size of a whole BPB
    .IF <<byte ptr ds:[si].7>  eq ' '> and      ; older than 3.2 ?
    .IF <<byte ptr ds:[si].8>  be '3'> and      ;  ...
    .IF <<byte ptr ds:[si].10> be '1'>          ;  ...
	sub  cx, 12				; we will ignore extended BPB
						;   and put zeros in ours
	SaveReg <ax,cx,di>			; save regs
	xor  ax, ax				; get a zero clearing ext. BPB
	add  di, 19				; es:di -> 12 byte extended BPB
	mov  cx, 6				; # words to copy
	cld					; clear direction flag
	rep  stosw				; wipe out extended BPB
	RestoreReg <di,cx,ax>			; restore regs
;* *******************
;* BEGIN INSERTED CODE
;* 
    .ELSE	; It's 3.2 or later
	;
	;   Usually, 3.20 and later boot sectors can be trusted to 
	;   have valid Extended BPBs.  However, if the boot sector 
	;   of a partition that starts in the first 32 Meg of the 
	;   disk was written by the COMPAQ 3.20 Rev A SYS utility, 
	;   the high word of HiddenSectors will contain trash 
	;   (always 0D474h).  Since the normal BPB is still OK, 
	;   this case is fixed here by clearing the Extended part 
	;   of the BDS Extended BPB...
	;
	.IF <<ds:[si.BPBoffset.BPB_HiddenSectors+2]> eq 0D474h>
	    sub  cx, 12			; we will ignore extended BPB
					;   and put zeros in ours
	    SaveReg <ax,cx,di>		; save regs
	    xor  ax, ax			; get a zero clearing ext. BPB
	    add  di, 19			; es:di -> 12 byte extended BPB
	    mov  cx, 6			; # words to copy
	    cld				; clear direction flag
	    rep  stosw			; wipe out extended BPB
	    RestoreReg <di,cx,ax>	; restore regs
	.ELSE 
	;
	;   Normally, an extended BPB contains a 16-bit value 
	;   for "Total number of sectors in the partition" at
	;   BPB offset 8.  If this value is 0000, it indicates 
	;   that the 32-bit value for TotalSectors (at BPB offset
	;   15h) should be used.  For now, the rest of the driver
	;   does not comprehend the BPB if TotalSectors = 0000.
	;   This is fixed here by moving the lower 16-bits of 
	;   the 32-bit TotalSectors value to the old location
	;   at offset 8.  The assumption of course is that the
	;   value will never be more than 16-bits, which is 
	;   true for now.  Note that the result is still a valid 
	;   Extended BPB.This hack can be removed when the rest
	;   of the driver is updated to handle Extended BPBs.
	; 
	    .IF <<ds:[si.BPBoffset.BPB_TotalSectors]> eq 0> ; If extended BPB...
	        push ds:[si.BPBoffset.BPB_BigTotalSectors]  ; update lower
	        pop  ds:[si.BPBoffset.BPB_TotalSectors]	    ; totalsectors value
	    .ENDIF
	.ENDIF
;* 
;* END INSERTED CODE
;* *****************

    .ENDIF					;
    push di					;* save original BDSBPB pointer
    add  si, BPBoffset				; ds:si -> BPB in boot sector
    cld 					; clear direction flag
    rep  movsb					; copy ds:si to es:di
    pop  di					;* get pointer back
    .IF <<byte ptr es:[di.BPBmediab-BPBsecsiz]> eq 0F8h> ;* Q: fixed disk ?
	pop  es:[di.BPBhidsecHi-BPBsecsiz]	;*   Y: restore original 32-bit
	pop  es:[di.BPBhidsec-BPBsecsiz]	;*      HiddenSectors value 
    .ELSE					;* 
	pop  cx					;*   N: throw away the saved
	pop  cx					;*      values (CX = don't care)
    .ENDIF					;* 
    ret 					; return

BootBPB_To_BDSBPB  endp






	BREAK <BPBFromScratch>
;**	BPBFromScratch - Build a BPB for the disk
;
;	BPBFromScratch builds a BPB for the drive where the boot sector
;	contains an invalid BPB in it.
;	For floppy drives, no action is performed.
;	For fixed disks, the partition table is read and from it the
;	location of the boot sector is obtained. The boot sector is then
;	read and the BPB built from there. An error is returned if the
;	partition table is invalid or is too small (< 32K bytes).
;
;	ENTRY	AL  Unit #
;		ES:BX -> DOS request packet
;		DS:DI -> BDS entry address
;
;	EXIT	'C' not set
;		'C' set
;		    AX contains error code
;
;	Registers used Flags

	public BPBFromScratch
BPBFromScratch proc

; pusha 			       ;   ...	  @p3142d
  SaveReg <es,ds,di,bx> 	       ;   ...	  @p3142change
  test [di].DrvTypFlag,fNon_Removable  ; hard disk or floppy ?
  .IF z near			       ; handle floppy
;    mov al,ERROR_I24_NOT_DOS_DISK     ; @p1828 add @p2692 delete
;    stc			       ; @p2xxx add @p2692 delete
;    jmp BPBExit		       ; @p1828 add @p2692 delete
     mov cl,1			       ; read FAT sector for 1.0 diskettes
     mov  bx,DataOffset ScratchBufSem  ; get a buffer
     call SWait 		       ; wait for ScratchBuffer to be free
     call ReadSecInScratch	       ; read the sector
     LJC BPBbad 		       ; quit on I/O error
     mov ah,byte ptr es:[di]	       ; get 1.0 media byte

; We can not build a bpb for two different medias with the same Media
; Descriptor byte. They sometimes have different sectors/track values,
; sectors/fat, total sectors. We however do the build bpbs for unique Media.

    .IF <ah eq MB_96> or	       ; high density or 720k @p2692 add
      .IF <ah eq MB_35_18Sect>	       ; 1.44 media	      @p2692 add
	mov al,ERROR_I24_NOT_DOS_DISK  ;		      @p2692 add
	stc			       ;		      @p2692 add
	jmp BPBbad			  ; quit on I/O error
    .ENDIF			       ;		      @p2692 add
     mov cl,ah			       ; be sure it starts with 0F8H
     or  cl,7			       ;  ...
     inc cl			       ;  ...
     .IF nz			       ; was not a valid
;	mov ah,MB_48_8Sect1S	       ; make it 8 sector, single sided 2273d
	mov al,ERROR_I24_GEN_FAILURE   ; compatible with pcdos@p2273 add
	stc			       ;		      @p2273 add
	jmp BPBbad			  ; quit on I/O error
     .ENDIF			       ;   ...
     call Get_BDS_Addr		       ; ds:di -> BDS
     .IF <[di].BPBMediab ne ah>        ; all done if it matches already
	.IF <ah eq MB_96>	       ; high density
	   mov al,7		       ; seven sectors/FAT
	   mov bx,224*256+0Fh	       ; 224 Root dir entries, &0f sector max
	   mov cx,80*15*2	       ; 80 tracks, 15 sectors/track,2 sides
	   mov dx,01*256+2	       ; sectors/allocation unit &Head max.
	.ELSE
	   mov al,1		       ; Assume will have 1 FAT sector
	   mov bx,64*256+8	       ; Assume # dir = 64,  8 sector
	   mov cx,40*8		       ; Assume 320 sectors/disk
	   mov dx,1*256+1	       ; Assume 1 head, 1 sector/allocate
	   test ah,2		       ; Is drive 8 or 9 sector?
	   .IF z		       ; It's 8, we assumed right
	      inc  al		       ; 9 sector, incr # of FAT sectors
	      inc  bl		       ; Set we have 9 sectors/cylinder
	      add  cx,40	       ; Increase size to 360 sectors
	   .ENDIF
	   test ah,1		       ; Is disk double sided?
	   .IF ne		       ; No, we guessed right
	      add cx,cx 	       ; Double size of disk
	      mov bh,112	       ; Increase # of directory entries
	      inc dh		       ; Set 2 sectors/allocation unit
	      inc dl		       ; Set 2 heads
	   .ENDIF
	.ENDIF			       ; @p1828 move
;	(AH) = Media Byte
;	(AL) = # of FAT sectors
;	(BH) = # of directory entries
;	(BL) = # of sectors/track
;	(CX) = # of sectors on disk
;	(DH) = Sectors/Cluster
;	(DL) = # of heads

	   mov [di].BPBsecpau,dh	       ; Set sectors/allocation unit
	   mov byte ptr [di].BPBndir,bh        ; Set # of directory entries
	   mov [di].BPBnsec,cx		       ; Set size of disk in sectors
	   xchg ah,al
	   mov word ptr [di].BPBmediab,ax      ; Set media byte & # of FAT sectors
	   mov byte ptr [di].BPBtrksiz,bl      ; Set sectors/track
	   mov byte ptr [di].BPBnhead,dl       ; Set # of heads
     .ENDIF
  .ELSE 				       ; hard disk case

; We have a fixed disk. We read the partition table to determine the location
; of the boot sector on the disk. We then read in the boot sector and build
; the BPB from there. If the boot sector does not have a valid BPB, we build
; a default one from DiskTable2.

     xor     ax,ax			; cylinder 0
     mov     cx,1			; sector 1
     xor     dx,dx			; head 0
     mov     bx,DataOffset ScratchBufSem  ; get the ScratchBuffer
     call    SWait			  ; wait for ScratchBuffer to be free
     call    ReadPhysSec		; read master boot record
     jc      BPBbad			; error on read
     call    Process_Partition		; ax=cylinder #, ch=head#, cl=boot sector
					; bx=partition size in sectors
     .IF nc				; valid partition
	push	bx			; save partition size
	xor	dh,dh			;   ...
	mov	dl,ch			; dx  = head #
	xor	ch,ch			; cx  = sector # (1 based)
	call	ReadPhysSec		; read partition boot record
	pop	cx			; ax=partition size in sectors p3142chg
	jc	BPBbad			; Error from READ
	mov	ax,cx			; @p3142 add
	call	Process_Boot		; Build a BPB for the drive
     .ELSE

; We have an invalid partition on the fixed disk. We return a "minimum BPB" so
; that the disk can still be accessed in order to place a valid partition table
; and master boot record onto the disk.

	call	SetMinBPB		   ; Set up "minimum BPB"
	call	BPB_To_Rec		   ; Move this BPB into the recommended
     .ENDIF				   ;   ...
  .ENDIF

BPBdone:
  mov bx,DataOffset ScratchBufSem	; Release scratch buffer
  call SSig				;   ...
  clc
  jmp BPBExit
BPBbad: 				; AX has error code
  mov bx,DataOffset ScratchBufSem	; Release scratch buffer
  call SSig				;   ...
  stc					; Return with carry set
BPBExit:
  RestoreReg <bx,di,ds,es>		; these were saved on entry @p3142chg
; popa					;   ... @p3142 delete
  ret					;   ...

BPBFromScratch endp

	BREAK <Is_BPB_Boot>
;**	Is_BPB_Boot - Is this a valid 286CP/DOS boot sector ?
;
;	ENTRY	ES:SI = Address of a boot sector (Virtual)
;
;	EXIT	'C' Set if Not a 2.x/3.x/4.x boot sector
;
;	USES	Flags
;
;	ES:[ScratchBuffer] points to the boot sector.  In theory, the BPB in
;	this thing is correct.	We can, therefore, suck out all the relevant
;	statistics on the media (those that we cannot get from the partition
;	table) if we can recognise it.	A valid boot sector has a near
;	JMP (opcode 0E9H) followed by a NOP, or a short JMP (opcode 0EBH) as
;	its first word.  It is followed by a  string of 8 ASCII characters.
;	If we find this, its a valid boot sector.

	public Is_BPB_Boot
Is_BPB_Boot proc

  pusha 				 ; save all regs
  .IF <<byte ptr es:[si]>   eq 0E9H> or  ; is it a near jump
  .IF <<byte ptr es:[si]>   eq 0EBH> and ; or a short jump ?
  .IF <<byte ptr es:[si]+2> eq 090H>	 ; is this a NOP ?

; Note for PTM 1828 we commented out the following lines up to the second else
; because there was a vendor that produce 720K diskettes in which the 8th byte
; of the boot id was invalid. We could not try to build a BPB from scratch
; because a 1.2 meg. disktette has the same Media Desciptor byte as the 720K
; disktette, so there would be no way to distinguish the two medias. So we only
; checked for a valid jump instruction. However Ptm 2692 demonstrated that even
; if a Disktette has a valid jump instruction in the boot sector, the BPB still
; may be invalid !!!.

; In order to satisfy everyone we will only check the first 7 bytes of the boot
; id and if it is invalid we will try to build a bpb from scratch for those
; medias that do not have conflicting Media Descriptor Bytes.

;     clc				 ; @p1828 add @p2692 delete
     add si,03H 			 ; es:si[3] = BOOT ID #   @p1828 delete
     mov cx,7				 ; loop counter @p2692 chg@p1828 delete
ibb2:					 ; check the next 7 bytes.@p1828 delete
     mov al,es:[si]			 ; get the character	  @p1828 delete
     .IF <al ae ' '> and                 ; is it less than ` '?   @p1828 delete
     .IF <al be '~'>                     ; Greater than `~'?      @p1828 delete
	inc si				 ; bump to next character @p1828 delete
	loop	ibb2			 ; repeat the test	  @p1828 delete
	clc				 ; valid boot sector	  @p1828 delete
     .ELSE				 ; one char >= 080H	  @p1828 delete
	stc				 ; invalid boot sector	  @p1828 delete
     .ENDIF				 ;   ...
  .ELSE 				 ;   ...
     stc				 ; invalid boot sector
  .ENDIF				 ;   ...
   popa 				 ; restore all regs
   ret					 ; return

Is_BPB_Boot endp

	BREAK <Process_Partition>
;** Process_Partition - Process the partition table of the fixed disk
;
;	Process_Partition processes the partition table obtained from the
;	fixed disk and determines where the DOS boot sector is found on the
;	disk.
;
;	ENTRY	DS:DI -> DS
;		ES:BX -> ScratchBuffer (Physical sector 0 read off the disk)
;
;* This routine has been modified to remove its dependence on ScratchBuffer.
;* ES:BX can point to any arbitrary 512-byte buffer in which a partition
;* table exists (at offset 1BEh).
;
;	EXIT	CH    Head for boot sector
;		CL    Sector # for boot sector
;		AX    Cylinder # for boot sector
;		BX    # of sectors on hard disk
;		DL    Drive number
;		'C' set if some error (invalid partition, partition too small)

	public Process_Partition
Process_Partition proc

  cmp word ptr es:[bx+1FEH],0AA55H
  jne pp2			       ; This is an invalid partition sector
;* 	REMOVE DEPENDENCE ON SCRATCHBUFFER OFFSET...
  mov ax, bx			       ;* save original offset of buffer
  add bx,1C2h			       ; BX -> first OS indicator
;*  mov ax, ScratchBuffer 	       ; get offset of ScratchBuffer
  add ax, SCRATCHSIZE + 2	       ; for checking past end of buffer

  mov cl, 0			       ;* initialize primary partition counter

pp1:
  cmp byte ptr es:[bx],1	       ; Look for a 12 or 16 bit FAT
  je  pp3
  cmp byte ptr es:[bx],4
  je  pp3
pp1a:				       ;* (added loop entry point)
  add bx,16
  cmp bx, ax
  jnz pp1

pp2:
  or  [di].DrvTypFlag,fNoDOSPartition
  stc					; Not found. Partition invalid.
  ret

pp3:

  cmp  [di].PartitionNumber, cl		;* Q: found requested partition yet ?
  je   pp3a				;*   Y: go process it
  inc  cl				;*   N: no, bump partition counter
  jmp  short pp1a			;* and loop to look for next
pp3a:					;* 

  SaveReg dx
  mov ax,es:[bx+8]		; get total # of sectors (low word)
  dec ax			; decrement by 1
;* should dec dx/ax, but only partitions < 64k are currently supported
  mov dx,word ptr es:[bx+10]	; get total # of sectors (high word)
  add ax,word ptr es:[bx+4]	; add relative sectors (low word)
  adc dx,word ptr es:[bx+6]	; add relative sectors (high word)

;* ENHDISK partitions (including the first) can be anywhere on the disk - 
;*   they are not limited to the first 32 Meg of the disk.
;*  jz  pp4
  jnc pp4			;* Error if overflow 32 bits (VERY doubtful)
  or  fBigFat,fTOOBIG

pp4:
  RestoreReg dx
  mov ax,es:[bx+4]
  mov ds:[di].BPBhidsec,ax		  ;SET HIDDEN SECTOR COUNT
  mov ax,es:[bx+6]			  ;* Get MSW of Rel Sect field
  mov ds:[di].BPBhidsecHi,ax		  ;* Set MSW of Hidden Sector Count
  cmp word ptr es:[bx+10],0000		  ;* DOS Partitions must be <64K
  jnz pp2				  ;* If >64K sectors - invalid DOS part
  mov ax,es:[bx+8]
  cmp ax,64				  ;HAS TO BE AT LEAST 32K
  jc  pp2				  ; Indicate no DOS partition present
  mov [di].BPBnsec,ax			  ; BPB->MaxSec = P->PartitionLength
  SaveReg <ax,dx>			  ; Save # sectors.
  mov ax,[dI].BPBHidSec 		  ; Get Boot sector #
;*  xor dx,dx
;*  mov bh,dh
  mov dx,[di].BPBhidsecHi		  ;* get MSW of sector count
  mov bh,00				  ;* 
  mov bl,byte ptr [di].BPBTrkSiz
  div bx
  mov cl,dl				  ; CL is sector number
  inc cl				  ; sectors are 1 based
  cwd
  mov bl,byte ptr [di].BPBnhead
  div bx				  ; DL is head, AX is cyl.
  mov ch,dl				  ; Save head # in CH
  and [di].DrvTypFlag,NOT fNoDOSPartition ; Indicate valid DOS partition
  RestoreReg <dx,bx>			  ; BX = total # of sectors on disk
  clc
  ret

Process_Partition endp

	BREAK <Process_Boot>
;*****	Process_Boot - Process the boot sector of the fixed disk
;
;	Process_Boot examines the boot sector read off the fixed disk
;	and builds a BPB for it in the BDS for the drive. If the boot
;	sector is not valid, it assumes a default BPB from a table.
;
;	ENTRY	DI -> BDS for the drive
;		ES:BX -> boot sector (in ScratchBuffer)
;		AX = total # of sectors in partition
;
;	EXIT	BPB set up in the BDS

	public Process_Boot
Process_Boot proc

  push si				;   ...
  mov  si,bx				; es:si -> boot sector
  call Is_BPB_Boot			; does it look like a boot sector ?
  pop  si				;   ...
  LJC  Unknown				; 'C' set if not a boot sector

; We have a valid boot sector.	Use the BPB in it to build
; the BPB in BIOS.  It is assumed that ONLY SecPerClus, cDIR,
; cSecFat, and BPBmediab need to be set (all other values in already).
; fBigFat is also set.

	MOV	AH,byte ptr ES:[BX+11+BPBmediab-BPBsecsiz]
	MOV	[DI].BPBmediab,AH		; store media byte.
	MOV	AX,WORD PTR ES:[BX+11+BPBnsec-BPBsecsiz]   ; TotalSectors
	DEC	AX
	MOV	DX,WORD PTR ES:[BX+11+BPBnFATsec-BPBsecsiz] ; Sectors for 1 fat
	MOV	[DI].BPBnFATsec,DX			; Set in BIOS BPB
	SHL	DX,1				; Always 2 fats
	SUB	AX,DX				; SUB # FAT sectors
	MOV	DX,WORD PTR ES:[BX+11+BPBndir-BPBsecsiz]   ; # of root entries
	MOV	[DI].BPBndir,DX 		; Set in BIOS BPB
	MOV	CL,4
	SHR	DX,CL				; Divide by 16 ents/sect
	SUB	AX,DX				; SUB # dir sectors

;	AX now contains the # of data sectors

	MOV	CL,BYTE PTR ES:[BX+11+BPBSecPAU-BPBsecsiz]  ; Sectors per cluster
	MOV	[DI].BPBSecPAU,CL		; Set in BIOS BPB
	XOR	DX,DX
	MOV	CH,DH
	DIV	CX

;	AX now contains the # clusters.

;;	CMP	AX,4096-10			; Is this a 16 bit fat? ;#####
	cmp	WORD PTR ES:[BX+11+BPBnsec],20740   ; TotalSectors
;;	JB	GoodRetj			; NO			;#####
	jbe	GoodRetj			; NO
	OR	fBigFat,FBIG			; 16 Bit fat
GoodRetj:JMP   GoodRet

;	We were unable to recognise the boot sector.  Use a table
;	of values saved internally to figure out what the values
;	in the BPB that we would ordinarily have gotten from the boot
;	sector.
;
;	(AX) = # of sectors on the disk
;	(DS) = BiosGroup

Unknown:
	MOV	SI,DataOffset DiskTable2
Scan:	CMP	AX,[SI]
	JBE	GotParm
	ADD	SI,4*2
	JMP	Scan

GotParm:
	MOV	CL,Byte Ptr [SI+6]		; Get flags stored in table.
	MOV	fBigFat,CL
	MOV	CX,[SI+2]			; Get #Sec/Clus | Log_2 CH
	MOV	DX,[SI+4]			; Get number of dir. entries.

;	AX = number of sectors on disk drive
;	DX = number of dir entries,
;	CH = number of sectors per cluster
;	CL = log base 2 of ch
;	NOW calculate size of fat table

	MOV	WORD PTR [DI].BPBndir,DX ; Save # dir entries
	MOV	BYTE PTR [DI].BPBsecpau,CH ; Save Sectors per cluster
	MOV	BYTE PTR [DI].BPBmediab,0F8H ; Stick in a hard disk media byte
	TEST	fBigFat,FBIG		; if (bigfat)
	JNZ	DoBig			;	Goto (doBig);
	XOR	BX,BX
	mov	BL,CH
	DEC	BX
	ADD	BX,AX
	SHR	BX,CL			; BX = 1+(BPB->MaxSec+SecPerClus-1)/
	INC	BX			;	SecPerClus
	AND	BL,11111110B			; Bx &=~1 ( = # of clusters)

	MOV	SI,BX
	SHR	BX,1
	ADD	BX,SI
	ADD	BX,511			; BX+=511+BX/2
	SHR	BH,1			; BH = BX / 512
	MOV	BYTE PTR [DI].BPBnfatsec,BH ; Save Number of FAT sectors.
	JMP	short GoodRet

DoBig:
	MOV	CL,4			; 16 directory entries/sector
	SHR	DX,CL
	SUB	AX,DX			; AX -= cSecDir;
	DEC	AX			; AX = t - r -d
	MOV	BL,2
	MOV	BH,[DI].BPBSecPAU
	XOR	DX,DX
	ADD	AX,BX
	ADC	DX,0
	SUB	AX,1
	SBB	DX,0
	DIV	BX
	MOV	WORD PTR [DI].BPBnFATSec,AX
GoodRet:
	MOV	BL,fBigFat
	MOV	[DI].FatSiz,BL		; Set size of FAT on media
	CALL	BPB_To_Rec		; Update recommended BPB
	ret

Process_Boot endp

	BREAK <ReadSecInScratch>
;**  ReadSecInScratch - Read a sector from device
;
;	Reads the sector specified in CL from track 0 into Scratchbuffer.
;
;	ENTRY	CL - logical sector to be read
;		AL - Unit #
;
;	EXIT	ES:DI - Sector just read
;		If Carry set, AX has error code
;
;	USES	ES, DI, BX, Flags
;
;	NOTE	ScratchBufSem MUST be set and released around the call
;		to this routine to ensure the availability of ScratchBuffer.

	public ReadSecInScratch
ReadSecInScratch proc

  SaveReg <ax,bx,cx,dx,di,es>		      ;   ...
  call	  Get_Request_Packet		      ; es:bx -> request packet
  mov	  es:[bx].PktCmd,DISKOP_INTERNREAD    ; internal read request
  mov	  es:[bx].PktUnit,al		      ; store unit number
; mov	  es:[bx].IOmedia,ah		      ; store media ID byte
  mov	  di,word ptr ScratchBufPhys+2	      ; get 32 bit physucal address
  mov	  word ptr es:[bx].IOpData+2,di       ;   of ScratchBuffer
  mov	  di,word ptr ScratchBufPhys	      ;   ...
  mov	  word ptr es:[bx].IOpData,di	      ;   ...
  xor	  ch,ch 			      ; cx = starting sector number
  mov	  word ptr es:[bx].IOPhysRBA,cx       ; starting sector
  mov	  word ptr es:[bx].IOPhysRBA+2,0      ;   ...
  mov	  es:[bx].IOcount,1		      ; read one sector.
  call	  Get_BDS_Addr			      ; DS:DI -> BDS
  or	  [di].DrvFlag,FForceRdWrt	      ; force next read to succeed
  and	  word ptr [di].DrvFlag,NOT FMedInd   ; assume medium unknown

  call	  Add_Partition_Offset		      ; convert to physical sector
  mov	  ax,[di].BPBhidsec		      ; add hidden sectors
  add	  word ptr es:[bx].IOPhysRBA,ax       ;   not treated as double word
  mov     ax,[di].BPBhidsecHi		      ;* Get MSW of hidden sector count
  adc     word ptr es:[bx].IOPhysRBA+2,ax     ;* Treat as Double Word
;*  adc	  word ptr es:[bx].IOPhysRBA+2,0      ;   ...

; Now the fake device driver packet has been loaded up.
; We can go ahead and call the appropriate read/write routine
; to handle the request. The registers are also set up for the
; call to Do_DiskIO.
; AL = Unit #, ES:BX-> Request packet, CX = Sector count
; ds:di -> BDS

  mov  cx,1			       ; set up for Disk_IO
  call Do_DiskIO		       ; AX has returned status
  call Release_Request_Packet	       ; Release reserved packet
  RestoreReg <es,di,dx,cx,bx>	       ; restore saved regs
  test ax,STERR 		       ; Do_DiskIO error ?
  .IF z 			       ; no
     push ds			       ; set up ES:DI to point to
     pop  es			       ;   sector read
     mov  di, ScratchBuffer	       ;   ...
     RestoreReg ax		       ; restore unit #
     clc			       ; no error return code
  .ELSE 			       ; Do_DiskIO had an error
     pop bx			       ; clean up stack - AX has error status
     stc			       ; error return code
  .ENDIF			       ;   ...
  ret				       ;   ...

ReadSecInScratch  endp

	BREAK <ReadPhysSec>
;**  ReadPhysSec - Read the sector from the disk into ScratchBuffer
;
;	ReadPhysSec reads the sector specified by the head, cylinder,
;	sector combination into ScratchBuffer.
;
;	ENTRY	AX	Cylinder #
;		CX	Sector #      (ch = 0, cl = 1 based sector #)
;		DX	Head #	      (dh = 0, dl = head #)
;		DS:DI	Points to the BDS for the drive
;
;	EXIT	Sector specified is read into ScratchBuffer
;		ES:BX -> sector just read in (ScratchBuffer)
;		If 'C' set, AX has status from the READ
;
;	NOTE	ScratchBufSem must be set and released around the call to
;		this routine in order to assure the availability of
;		ScratchBuffer for this read.

	DebugPublic ReadPhysSec
ReadPhysSec proc

  SaveReg ax
  call Get_Request_Packet		   ; es:bx -> new request packet
  mov  es:[bx].PktLen,LENSpecPkt	   ; fill in new request packet
  mov  al,[di].DriveUnit		   ; unit from request packet
  mov  es:[bx].PktUnit,al		   ; store unit number
  mov  es:[bx].PktCmd,DISKOP_IOCTL	   ; IOCTL request
  mov  ax,word ptr ScratchBufPhys	   ; use physical scratch buffer
  mov  word ptr es:[bx].IOpData,ax	   ;   address
  mov  ax,word ptr ScratchBufPhys+2	   ;   ...
  mov  word ptr es:[bx].IOpData+2,ax	   ;   ...
  RestoreReg ax 			   ; restore cylinder number
  mov  es:[bx].IOC_Cyln,ax		   ; ax = cylinder
  mov  es:[bx].IOC_Head,dx		   ; head number
  dec  cx				   ; convert to 0 based and
  mov  es:[bx].IOC_StartSec,cx		   ;   pass to state machine
  mov  es:[bx].IOC_Secsiz,2		   ; 512-byte sector
  mov  es:[bx].IOC_Fun,IOCTL_Read	   ; read operation
  mov  es:[bx].IOcount,1		   ; read one sector
  mov  ch,dl				   ; ch = head
  mov  cl,1				   ; cl = first sector on track
					   ;	  StartSec has sector offset
  call CHS_to_RBA			   ; ax:dx = RBA
  mov word ptr es:[bx].IOstart,ax	   ; fill in starting RBA low half
  mov word ptr es:[bx].IOstart+2,dx	   ; fill in starting RBA high half
  call Add_Partition_Offset		   ; adjust to partition start
  or   [di].DrvFlag,FForceRdWrt 	   ; force next read to succeed
  and  word ptr [di].DrvFlag,NOT FMedInd   ; assume medium unknown
  mov  cx,1				   ; set up count for Disk_IO
  call Do_DiskIO			   ; ax has returned status
  call Release_Request_Packet		   ; release reserved packet
  push ds				   ; set es:bx to I/O buffer
  pop  es				   ;   ...
  mov  bx, ScratchBuffer		   ;   ...
  clc					   ; good return code
  test ax,STERR 			   ; Do_DiskIO error ?
  .IF nz				   ; no error
     stc				   ; error return code
  .ENDIF				   ;   ...
  ret

ReadPhysSec ENDP

	BREAK <BPB_to_Rec>
;**  BPB_To_Rec - Move the BPB into the recommended BPB in the BDS
;
;	BPB_To_Rec moves the BPB from the actual media BPB in the BDS
;	into the recommended BPB.
;
;	ENTRY	DS:DI -> BDS
;
;	EXIT	NONE

BPB_To_Rec  proc

  SaveReg <es,di,si,cx>
  push	  ds
  pop	  es
  mov	  si,di 		  ; DS:SI, ES:DI -> BDS
  lea	  di,[di].BPBRsecsiz
  lea	  si,[si].BPBsecsiz
  mov	  cx,size a_BPB
  cld
  repe	  movsb
  RestoreReg <cx,si,di,es>
  ret

BPB_To_Rec  endp

	BREAK <SetMinBPB>
;**  SetMinBPB - Set up minimum BPB for the medium
;
;	SetMinBPB sets up a "minimum BPB" for the hard disk so that it
;	can be read and written to.
;
;	ENTRY	DS:DI -> BDS for drive
;
;	EXIT	BPB in the BDS is set up

	public SetMinBPB
SetMinBPB   proc

	SaveReg <cx,es,si,di>		    ;
	push	ds			    ; move ds to es
	pop	es			    ;
	lea	di,[di].BPBsecsiz	    ;
	mov	si,DataOffset MinimumBPB    ;
	mov	cx,MinBPBSiz		    ;
	rep	movsb			    ; Copy over the minimum BPB
	RestoreReg <di,si,es,cx>	    ;
	mov	[di].BPBhidsec,0	    ; No hidden sectors
	mov	[di].BPBhidsecHi,0	    ;* 
	ret				    ;

SetMinBPB   endp

	BREAK <Sector size/index conversions>
;**  SectorSizeToSectorIndex - Convert Sector Size to Index
;
;	SectorSizeToSectorIndex converts a sector size from bytes to an index
;	that can be used by the NEC disk controller.
;	(0=>128, 1=>256, 2=>512, 3=>1024)
;
;	ENTRY:	  AX contains sector size in bytes
;
;	EXIT:	  AL contains index
;
;	REGISTERS MODIFIED:   AX, Flags

	public SectorSizeToSectorIndex
SectorSizeToSectorIndex proc

	mov	al,3
	.IF <ah na 2>			; examine upper byte only
	   mov	   al,ah		; value in ah is the index!
	.ENDIF
	ret

SectorSizeToSectorIndex endp

;**  SectorIndexToSectorSize - Convert index to sector size
;
;	ENTRY	AL - Sector size Index
;
;	EXIT	AX - Sector size

	public SectorIndexToSectorSize
SectorIndexToSectorSize proc

	push	cx
	mov	cl,al
	mov	ax,128
	shl	ax,cl
	pop	cx
	ret

SectorIndexToSectorSize endp

	BREAK <Address conversions>
;** PhysToVirtESDI - Convert physical address to virtual address
;
;   PhysTOVirtESDI converts the 32-bit address in AX:BX into a virtual
;   address in ES:DI.
;
;   ENTRY   DS = BiosGroup
;	    AX:BX = 32-bit address to be converted.
;
;   EXIT    ES:DI = Virtual address for the current mode of the driver.

	public PhysToVirtESDI
PhysToVirtESDI proc

	SaveReg <si,dx,cx,bx,ax,bp>
	mov	dh,1
	mov	si,-1
	DEVHLP	DevHlp_PhysToVirt
	RestoreReg <bp,ax,bx,cx,dx,si>
	return

PhysToVirtESDI endp

;** PhysToVirtDSSI - Convert physical address to virtual address
;
;   PhysTOVirtDSSI converts the 32-bit address in AX:BX into a virtual
;   address in DS:SI.
;
;   ENTRY   DS = BiosGroup
;	    AX:BX = 32-bit address to be converted.
;
;   EXIT    DS:SI = Virtual address for the current mode of the driver.

	public PhysToVirtDSSI
PhysToVirtDSSI proc

  SaveReg <di,dx,cx,bx,ax,bp>
  xor	  dh,dh
  mov	  si,-1
  DEVHLP  DevHlp_PhysToVirt
  RestoreReg <bp,ax,bx,cx,dx,di>
  ret

PhysToVirtDSSI	endp

;**  VirtToPhysESBX - Convert virtual address to physical address
;
;   VirtToPhysESBX converts the virtual address in ES:BX into a 32-bit
;   address in AX:BX.
;
;   ENTRY   DS = BiosGroup
;	    ES:BX = Virtual address for the current mode of the driver.
;
;   EXIT    AX:BX = 32-bit address to be converted.
;
;   REGISTERS MODIFIED AX,BX

	public VirtToPhysESBX
VirtToPhysESBX proc

    SaveReg <ds,es,si,dx>	     ;	 ...
    push ds			     ; swap ES and DS
    push es			     ;	 ...
    pop  ds			     ; DS:BX = address to be converted
    pop  es			     ;	 ...
    mov  si,bx			     ; set up for device helper
    mov  dl,DevHlp_VirtToPhys	     ; can't use macro because ds is bad
    call es:[DevHlpFunc]	     ; call device helper
    RestoreReg <dx,si,es,ds>	     ; return ax:bx
    ret 			     ;	 ...

VirtToPhysESBX endp

;**  VirtToPhysDSBX - Convert virtual address to physical address
;
;	VirtToPhysDSBX converts the virtual address in DS:BX into a 32-bit
;	address in AX:BX.
;
;	ENTRY	DS:BX = Virtual address for the current mode of the driver.
;
;	EXIT	AX:BX = 32-bit address to be converted.
;
;	REGISTERS MODIFIED AX,BX,Flags

	public VirtToPhysDSBX
VirtToPhysDSBX proc

    SaveReg <si,dx>
    mov     si,bx		; DS:SI = virtual address to convert
    DEVHLP  DevHlp_VirtToPhys
    RestoreReg <dx,si>
    ret

VirtToPhysDSBX endp

;**  UN_PhysToVirt - Clean up after PhysToVirt(s)
;
;	UNPhysToVirt will convert register back to the other mode If and
;	only if the mode changed on a previous PhysToVirt.
;
;	ENTRY	none
;
;	EXIT	none
;
;	REGISTERS MODIFIED Flags

	public UN_PhysToVirt
UN_PhysToVirt	 proc	 near

    SaveReg	dx
    DEVHLP  DevHlp_UNPhysToVirt
    RestoreReg	dx

    ret

UN_PhysToVirt	 endp

;*
;*************** copied from ROUTINT.INC, BIOSINC.INC, 286BIO.ASM *********
;*

	BREAK	<Routines used by device routines>

;    All routines on this page are various exits for device functions.
; They each return different information in the request packet for the
; DOS. The routines are as follows:
;
;	StatusDevReady
;		  The busy and done bits are set in the packet. This
;		means that the device has input to be read or can
;		do output without any waiting.
;
;	StatusPartialXfer
;		  The device was unable to do the I/O for the requested
;		number of bytes/blocks. CX contains the number that are
;		left to do. Fall into StatusError to set error has
;		happened.
;
;	StatusError
;		  Set the error and done bits in the status.
;
;	StatusNoXfer
;		  The device couldn't do the read or write, set the
;		number of bytes transferred to 0, but don't set the
;		error bit.
;
;	StatusComplete
;		  The device actually completed the request and every-
;		thing was just fine, so just set the done bit in the
;		request status.
;
;	StatusWait
;		  The device driver is for 3.0 and saw that it would
;		have to wait in a loop to do the request, so instead
;		it will not set the done bit which tells the DOS to
;		put the task into the I/O wait queue.
;
; Entry parameters:	( for all of the above routines )
;	DS:BX	Address of device request packet
;	CX	Count of bytes/blocks left to transfer if applicable
;	AL	Error code if applicable
;
; Exit parameters:
;	ES:BX	Pointer to packet
;	AX	Destroyed
;	SI	Destroyed
;	All other registers preserved
;

	public	StatusDevReady
StatusDevReady	proc			; Device has data or can send

	DISABLE
	ERRNZ	<((STBUI+STDON) AND 255)>	; Bits must be in high byte
	mov	Ah,(STDON+STBUI)/256	; Done + busy
	jmp	short SetStatus
StatusDevReady	endp

	public	CmdErr
CmdErr	proc				; Bad device command number
	DISABLE
	mov	al,error_I24_bad_command ; Unknown command error
;IF2
;	 ERRNZ	 <$-StatusPartialXFer> ; Fall through
;ENDIF
CmdErr	endp

;	 DebugPublic  StatusPartialXfer
;StatusPartialXfer  proc
;	 sub	 [bx].IOcount,cx	 ;# of successful I/O's
;IF2
;	 ERRNZ	 <$-StatusError> ; Fall through
;ENDIF
;StatusPartialXfer	 endp

	public	StatusError
StatusError	proc	near
	DISABLE
	ERRNZ	<((STERR+STDON) AND 255)>	; Bits must be in high byte
	mov	ah,(STDON+STERR)/256	; Done + busy
	jmp	short SetStatus
StatusError	endp

;	 DebugPublic  StatusNoXfer
;StatusNoXfer	 proc	 near
;	 xor	 ax,ax
;	 mov	 [bx].IOcount,ax	 ; No chars read
;StatusNoXfer	 endp

	public	StatusComplete
	public	SetStatus
StatusComplete	proc	near
	DISABLE
	ERRNZ	<((STDON) AND 255)>	; Bits must be in high byte
	mov	ah,(STDON)/256	; Done + busy
SetStatus:
	pop	bx
	pop	es
	mov	es:word ptr [bx].PktStatus,ax	;mark operation complete
	ENABLE					;
	TRACERESULT1	  ;; command sequence tracing macro for debug.
XXX	proc	far
	ret				;restore regs and return
XXX	endp
StatusComplete	endp

;	 DebugPublic  StatusWait
;StatusWait	 proc	 near
;	 DISABLE
;	 mov	 ah,0			 ; Don't set done bit
;	 mov	 [bx].IOcount,cx	 ; Set number completed
;	 mov	 word ptr [bx].IOpData,di ; Set new offset
;	 jmp	 SetStatus
;StatusWait	 endp

	DebugPublic  JustReturn
	BREAK	<Device Driver Secondary Entry Point>
;**	JustReturn - Simply perform a FAR return
;

JustReturn	proc	far
	ret
JustReturn	endp




BIOS_BLOCK_ID	equ	0fffch

BREAK	<Semaphore Subroutines>

;**	SWait - Wait for a semaphore
;
;	This is the same routine as the DOS "ram semaphore" routine.
;	It operates on a semaphore made up of a pair of bytes- an "in
;	use" flag and a "someone is wait" flag.
;
;	USABLE IN SYSTEM and USER MODE.
;
;	WARNING - Enables Interrupts
;
;	DB	busyflag
;	DB	waitingflag
;
;	If busyflag is clear then
;		set busyflag
;		return
;	else
;		set waitingflag (to notify owner to do a wakeup when he's done)
;		ProcBlock on address of busyflag
;
;	Note that when ProcBlock returns interrupts will be enabled.
;	Since the entire procblock interval was an "interrupts enabled"
;	window we figure we must be called with interrupts on and
;	thus don't mess with PUSHF/POPFF but just jam the interrupts
;	on and off as necessary.  (In keeping with good practice we
;	keep them on as much as possible.)
;
;	ENTRY	(DS:BX) = address of semaphore word
;
;	EXIT	When semaphore owned by caller
;		semaphore set
;		enables interrupts
;
;	USES	I-flag, Flags
;

	public SWait
SWait	proc

	;;DBPRT   80H,01H,<SW: $x:$x >,<ds,bx>
	push	ax
;	retest semaphore
;
;	(ds:bx) = address
;	(TOS) = caller's AX

swa0:	ENABLE
	mov	al,1
	xchg	al,BYTE PTR ds:[bx]	; set semaphore

;	If busyflag was set then its still set and (al) = 1
;	If busyflag was clear then its set now and (al) = 0

	and	al,al
	jnz	swa1			; foo - it was already set
	pop	ax
	ret				; done	- we've got it

;	The thing is set.  We're going to have to sleep on it
;	We have to do it under "cli" so that we don't have a race with
;	the semaphore owner releasing it while we're blocking
;
;	(al) = 1

swa1:	DISABLE
	test	BYTE PTR ds:[bx],0ffh
	jz	swa0			    ; just unlocked!  try again
	mov	BYTE PTR ds:1[bx],al	    ; set waiting
	push	bx
	push	cx
	mov	ax,BIOS_BLOCK_ID
	call	PBlock			    ; wait till semaphore released
	pop	cx
	pop	bx
	;;DBPRT     80H,1,< - C($x:$x) >,<ds,bx>
	jmp	swa0				; Re-Test this.

SWait	endp

;**	SSig - Signal a disk semaphore
;
;	SSig releases a semaphore (it is presumed that the caller indeed
;	owns it!) and, if anyone was waiting, wakes them up.
;
;	Usable in SYSTEM-TASK, USER and INTERRUPT mode.
;	Callable in BOOT mode if we're sure no one is "waiting" on the
;		semaphore (which should always be the case)
;
;	ENTRY	(DS:BX) = address of semaphore word (see Swait, above)
;
;	EXIT	NONE
;
;	USES	Flags

;**	SWClr - Wait for a Semaphore to clear
;
;	SWClr waits for a semaphore to become clear.  NOTE that SWClr
;	does not reserve the semaphore for the caller, nor guarantee the
;	semaphore REMAINS clear - as soon as the semaphore is seen to be
;	clear SWClr returns to the caller.  It's up to the caller to
;	rule out or deal with the possibility of someone getting the
;	semaphore.
;
;	If we're called in kernel mode then the semaphore will still be
;	clear when SWClr returns to the caller.  If we're in user mode
;	it might be busy again by the time SWClr returns.
;
;	Usable in SYSTEM-TASK and USER mode.
;
;	ENTRY	(DS:BX) = address of semaphore word (see Swait, above)
;
;	EXIT	NONE
;
;	USES	Flags
;
	public SWClr
SWClr:
	;;DBPRT     80h,1,<SWClr >,<>
	call	Swait

IF2
    ERRNZ	<SSig-$>		; Fall through to SSig
ENDIF

public SSig
SSig	proc

	;;DBPRT     80H,1,<SS: $x:$x >,<DS,BX>
	push	ax
	xor	ax,ax
	xchg	ax,ds:[bx]		; clear semaphore and wait flag

;	(al) = semaphore flag
;	(ah) = wait flag

	and	al,al
	jz	SSig8			; TROUBLE! Not SET!
	and	ah,ah
	jz	SSig2			; nobody waiting, don't ProcRun

;	Someone is waiting on this semaphore.  Wake him/them up.
;
;	(AX:BX) = block/run code value

	push	bx
	push	cx
	mov	ax,BIOS_BLOCK_ID
	call	PRun			; Yes, let them run.
	pop	cx
	pop	bx
SSig2:
	pop	ax
	ret

;	Jeez... we're clearing a semaphore which was never set!

SSig8:	InternalError	<Semaphore cleared that was never set!!>

SSig	endp


;**	SBlock - Return if someone is blocked on a semaphore
;
;	ENTRY	(DS:BX) = Address of semaphore
;
;	EXIT	'Z' set if no one is blocked on the semaphore
;
;	USES	NONE
;
	DebugPublic SBlock
SBlock	proc
	test	byte ptr ds:[bx],0ffh
	return
SBlock	endp


BREAK	<Block and unblock a process>

;**	PBlock	- Do Proc_Block DevHlp Call
;
;	These routines simply call  ProcBlock via the DevHlp interface.
;	We've split them out to save space.  The overhead of the extra
;	call/ret is small compared to the ProcBlock.
;
;	ENTRY	(AX,BX) = Block/Run Code
;
;	EXIT	Upon awakening
;
;	USES	AX, BX, CX, Flags
;

	public	PBlock
PBlock proc
	SaveReg <cx,dx,di>
	mov	di,-1
	mov	cx,-1			; Never time out
	DEVHLP DevHlp_ProcBlock 	; Block until Proc_Run
	RestoreReg <di,dx,cx>
	ret
PBlock endp


;**	PRun - Do Proc_Run DevHlp Call
;
;	This routine simply calls ProcRun via the DevHlp interface.
;	We've split it out to save space.  The overhead of the extra call/ret
;	is small compared to the ProcRun.
;
;	ENTRY	(AX,BX) = Block/Run Code
;
;	EXIT	Upon awakening
;
;	USES	AX, BX, CX, Flags
;

	public PRun
PRun	proc
	;;DBPRT   080H,8,<PRun ($x:$x) >,<AX,BX>
	push	dx
	DEVHLP	DevHlp_ProcRun		   ; Block until Proc_Run
	pop	dx
	ret
PRun	endp

;**	IntErr - InternalError support: HALT the system.
;**	IntErrNull - Entry point for default NULL string.
;
;	We HALT the system (enable interrupts and loop forever).
;
;	ENTRY	NONE
;
;	EXIT	NEVER
;
;	EFFECTS     SYSTEM HANGS (ALWAYS)
;
	DebugPublic  IntErrNull
IntErrNull proc
entry IntErr
	ENABLE				; Enable interrupts for Ctrl-Alt-Del
ielab:	jmp	ielab

IntErrNull  endp

	BREAK <Set_Changed>
;**	Set_Changed - Set changed status for drive
;
;	ENTRY	DL = Physical Drive Number
;
;	OUTPUT	NONE
;
;	USES	Flags.
;
;	ALGORITHM  We scan through the list of BDSs and set the relevant
;		   flag bits to indicate that the medium may have changed.
;		   On a system with logical drives, we *MUST* flag the other
;		   drives as being changed since all drive numbers really
;		   refer to the same drive.

	public Set_Changed
Set_Changed proc

  SaveReg di					  ;   ...
  mov di,Start_BDS				  ; chase the BDS chain
  .WHILE <di ne -1>				  ;   ...
     .IF <[di].DriveNum e dl>			  ; compare on physical drive
	or  [di].DrvFlag,FUncert_Media		  ;   number
	and [di].DrvFlag,NOT FMedInd		  ; turn off media known bits
	or  [di].DrvTypFlag,fChanged		  ;
     .ENDIF					  ;   ...
     mov di,[di].Link				  ; next entry
  .ENDWHILE					  ;   ...
  RestoreReg di 				  ;   ...
  ret						  ;   ...

Set_Changed endp

	BREAK <Update_Owner>
;***** Update_Owner - Update the Owner of the logical drive
;
;      Update_Owner maps the current drive onto the physical drive by
;      adjusting the fI_Own_Physical flag bits in the approporiate BDSs.
;      It also adjusts the ROMs data area.
;
;      ENTRY	DS:DI	BDS for current drive
;
;      EXIT	NONE

	public Update_Owner
Update_Owner proc

  SaveReg di					;   ...
  mov al,[di].DriveNum				; reset fI_Own_Physical for
  mov di,[Start_BDS]				;   all BDS entries mapped
  .WHILE <di ne -1>				;   onto this physical
     .IF <al e [di].DriveNum >			;   drive
	and [di].DrvTypFlag,NOT fI_Own_Physical ;   ...
     .ENDIF					;   ...
     mov di,[di].Link				; get next BDS in the link
  .ENDWHILE					;   ...
  RestoreReg di 				;   ...
  or [di].DrvTypFlag,fI_Own_Physical + fChanged ; set it for this logical drive
  .IF <NumFlop e 1> and 			; update the ROM data
  .IF <[di].DriveUnit be 1>			;   for diskettes only
     SaveReg <es,ax>				;   ...
     mov ax,RomData				;   ...
     MOV es,ax					;   ...
     assume es:RomData				;   ...
     mov al,[di].DriveUnit			;   ...
     mov [ActPDrv],al				;   ...
     assume es:nothing				;   ...
     RestoreReg <ax,es> 			;   ...
  .ENDIF					;   ...
  ret						;   ...

Update_Owner endp

BREAK	<FloppyChange - check whether floppy disk must be changed>

;***	FloppyChange - check whether floppy disk must be changed
;
;	The DOS/BIOS conspire so that on single-floppy-drive systems there
;	appear to be two floppy drives, "A" and "B".  The Bios keeps track
;	of which of these "pseudo drives" is currently active and if it
;	sees a request to the other drive causes the user to be prompted
;	to "insert drive ?? disk".
;
;	FloppyChange is called when we're doing floppy I/O on a single
;	drive system.  It takes one of two actions:
;
;	1) the request is for the pseudo-drive thats active
;		- FloppyChange returns, no action
;
;	2) the request is for the other pseodo-drive:
;		- Return disk change error to dos and let it map the
;		  the logical drive.
;
;	Called in KERNEL-TASK mode only.
;
;	ENTRY	ES:BX	Pointer to I/O request
;		DS:DI	Pointer to BDS for drive
;		AL	Unit #
;		DS	BiosGroup
;
;	EXIT	Interrupts Enabled
;		AH  -1 if pseudo drive must be changed
;		     0 if request for current pseudo drive
;
;	USES	Flags,AX
;
;
	ASSUME	DS:BiosGroup,ES:NOTHING,SS:NOTHING

	PUBLIC	FloppyChange
FloppyChange	PROC
	;;DBPRT 8H,4H,<FloppyChange $x:$x>,<DS,BX>
	SaveReg <cx>
	xor	ah,ah			; Set return status to 'no change'
	test	[di].DrvTypFlag,fI_Am_Mult  ; Is this 1 of several logical drvs?
	jz	flcha2			; No, don't check for drive change

;	A pseudo-drive change is not in progress. Check to see if this
;	request is for the current pseudo-drive.
;	AL has unit number from request packet

	test	[di].DrvTypFlag,fI_Own_Physical
	jnz	flcha2			; This unit is mapped onto the drive
	cmp	NumFlop,1
	jne	flcha1
;	test	[Single],0FFH
;	jz	flcha1			; Do not check ActPDrv if not a single
					; drive system.
	push	es
	mov	cx,ROMData
	mov	es,cx
	cmp	es:ActPDrv,al		; Switching A and B drive?
	pop	es
	jne	flcha1			; Request is not for current pseudodrive

	call	Update_Owner		; Update drive ownership status
	jmp	short flcha2		; Return to caller
flcha1:
	dec	ah			; Disk switch required.
flcha2:
	RestoreReg <cx>
	return
FloppyChange	ENDP

	BREAK <Do_Check>
;**	Do_Check - Perform the work involved in media check
;
;	ENTRY	AL = Unit #
;		DS:DI -> BDS for drive
;
;	EXIT	DL = Return value for DOS
;			1	Media unchanged.
;			0	Media may have been changed (unknown)
;		       -1	Media has changed
;
;	USES	DX, Flags

	public Do_Check
Do_Check proc

  SaveReg <cx,di,ax>
  mov  dl,1			      ; Assume not changed
  test [di].DrvTypFlag,fNon_Removable ;
  LJNZ flchk9			      ; If hard disk, media not changed

  call FloppyChange		      ; Check if this unit is the current
  or   ah,ah			      ;  physical drive.
  jnz  flchk5			      ; If not currently mapped, media has
				      ;  changed
  test [DI].DrvTypFlag,(fChanged_By_Format+fChanged) ; p2273 logic change
  jnz  flchk5			      ; Medium has changed

  test [di].DrvFlag,Fmotoron	      ;
  Jnz  flchk9			      ; If motor on, media not changed

  test [di].DrvTypFlag,fChangeline    ;
  jz   flchk4			      ; No changeline, so don't know if changed

  call Check_Change		      ; The drive has changeline support
  test al,80H			      ; See if the changeline is active
  jnz  flchk5			      ; Changeline is active - assume changed


; Changeline is low. This does not mean that it was not triggered!!!! It could
; be the case that it was high, but an access to the second floppy drive turned
; it off (this was a problem reported during DOS 3.20). We therefore check to
; see if this unit was the last one accessed if we are on a system with a single
; floppy drive. If it was, then we assume the medium has not changed.

  cmp NumFlop,1 		      ;
  jne flchk9			      ; Medium was unchanged
  cmp [di].DriveUnit,1		      ;
  ja  flchk9			      ; Unchanged if unit > 1
  SaveReg <es,dx>		      ;
  mov dx,RomData		      ;
  mov es,dx			      ;
  assume es:RomData		      ;
  mov al,ActPDrv		      ;
  RestoreReg <dx,es>		      ;
  assume es:nothing		      ;
  cmp al,[di].DriveUnit 	      ;
  jz flchk9			      ; Same as last drive accessed - medium
				      ;   has not changed
; We were unable to tell if the floppy is changed from the
; status of the drive. We return a 'maybe' to the DOS.

flchk4: 			      ;
  xor dl,dl			      ; Might have been changed
  jmp short flchk6		      ;

; The medium has changed

flchk5: 			      ;
  AND	  [DI].DrvTypFlag,NOT (fChanged_By_Format+fChanged)  ; Reset flags
  mov dl,-1			      ;

; The medium may have changed or has changed. We note this in the BDS for later
; use. We *MUST* flag these changes in ALL the logical drives that are mapped
; onto this physical drive.
; We also inform the ROM that the medium may have changed. This must be done
; so that it retries I/O on the medium with different data rates, and does not
; assume a certain rate for the particular drive. This is only done for physical
; drives 0 and 1 since the ROM only has data areas for these drives.

flchk6: 				;
  mov al,[di].DriveNum			;
  mov di,Start_BDS			;
  .IF <al be 1> 			;
     SaveReg <es,bx>			;
     mov bx,RomData			;
     mov es,bx				;
     assume es:RomData			;
     xor bh,bh				;
     mov bl,al				; Get drive # (= index into ROM data)
     mov byte ptr DSK_STATE[bx],0	; Zero out all state bits
     RestoreReg <bx,es> 		;
     assume es:nothing			;
  .ENDIF				;
  .WHILE <di ne -1>			;
     .IF <[di].DriveNum eq al>		;
	or  [di].DrvFlag,FUncert_Media	;
     .ENDIF				;
      mov di,[di].Link			;
  .ENDWHILE				;

flchk9: 				;
  RestoreReg <ax,di,cx> 		;
  ret					;

Do_Check endp

	BREAK <Check_Change>
;*****	Check_Change - Check to see if changeline is active
;
;	Check_Change check the status of the changeline.
;
;	ENTRY	DI  BDS for the drive
;		AL  Unit #
;
;	EXIT	AX contains status
;		(High bit of AL is set if changeline active)
;
;	Registers changed: AX, Flags

Check_Change proc

  SaveReg <di,es,bx,dx> 	     ;	 ...
  call Get_Request_Packet	     ; Get a request packet
  mov  es:[bx].PktCmd,DISKOP_CHKCHNG ; Check changeline request.
  mov  es:[bx].PktUnit,al	     ; Save users unit
  call Do_DiskIO		     ; return status in ax
  call Release_Request_Packet	     ; release the request packet
  RestoreReg <dx,bx,es,di>	     ;	  ...
  ret				     ;	  ...

Check_Change endp


	BREAK <Flag_UM>
;**  Flag_UM - Walk device driver packet list, and return UM.
;
;	This routine walks the device driver packet list and forces
;	the driver requests to return Uncertain Media.
;
;	We mark the PktDevLink field of the packet passed in as NIL to
;	prevent the BIOS from attempting to pull off more packets.
;
;	ENTRY	(ES:DI) = Head of floppy queue (Virtual address)
;
;	EXIT	NONE
;
;	USES	Flags

	Public Flag_UM
Flag_UM proc

  SaveReg <ax,bx,cx,di,es>
  .REPEAT
     les di,es:[di].PktDevLink	  ; es:di = Next request
     mov ax,es			  ; ax = Segment address of next req
     or  ax,ax			  ; ax = 0 ?
     .LEAVE z			  ; yes - End of list.
     mov es:[di].PktStatus,STERR+STDON+error_I24_Uncertain_Media
     mov  ax,es 		  ;
     mov  bx,di 		  ; ax:bx = Request packet
     call PRun			  ; Force run on this request
  .UNTIL			  ; Go back and get next packet
  RestoreReg <es,di>		  ;   ...

;  We have told all the other requests to wake up when we exit
;  kernel mode, we now empty out the request queue by setting the
;  next field of the current packet to NIL (we cannot simply
;  set [DriveQueue]+2 to 0, because DoneRequest will call
;  PullRequest on the DriveQueue to get the current packet).

  mov word ptr es:[di].PktDevLink+2,0	  ;
  mov al,es:[di].PktUnit		  ;
  SaveReg di				  ;   ...
  call Get_BDS_Addr			  ; ds:di -> drive parameters
  mov  [di].DrvFlag, FUncert_Media+FRestor+Fmotoron
  RestoreReg <di,cx,bx,ax>		  ;   ...
  ret					  ;   ...

Flag_UM endp


	BREAK <MotorOff>
;** MotorOff - turn off drive motor
;
;   MotorOff is called by timer services to turn off the diskette drive motor
;   when the timer has expired.  Turn off the drive motor and if the device
;   is active, call the state machine to handle a timeout error.
;
;   ENTRY CS and DS set by timer services
;
;   EXIT  DriveStruc updated
;
;   USES  flags

	public MotorOff
MotorOff proc

  pusha 				       ;   ...
  xor ch,ch				       ; ch ne 0 if a timer is active
  xor bl,bl
  mov di,Start_BDS			       ; ds:di -> BDS chain
  .WHILE <di ne -1> near		       ; chase the whole chain
     test [di].DrvFlag,Fmotoron 	       ; motor on?
     .IF nz and 			       ; no
     .IF <[di].Timer ne 0> and		       ; is timer active for drive?
     inc ch				       ; count the timer as still running
     dec [di].Timer			       ; another tick has passed
     .IF z				       ; has it expired ?
	dec ch				       ; adjust count since it expired
	and  [di].DrvFlag,NOT Fmotoron	       ; stop drive motor
	mov  cl,[di].DriveNum		       ; get physical drive number
	test [di].DrvTypFlag,fI_Am_Mult        ; is it a mapped drive ?
	.IF nz				       ; nz means there are others
;==============================================================================;
;=  Here we decide whether or not to turn off the motor. There is a	      =;
;=  problem  with 1 drive systems and alias.  The problem occurs when	      =;
;=  a request comes in for A and a motoroff timer is set and goes off during  =;
;=  the request for another PSEUDO drive (which realy is the same drive).     =;
;==============================================================================;
	   cmp	   Drive.Active, 1	; Are we trying to turn off someone
	   Jne	   callmotoroff 	; elses motor?
					; "                               "
	   cmp	   cl, Drive.PhysDrv	; "                               "
	   jne	   callmotoroff 	;

;	   cmp	   bl, Drive.Current	; is it a TIMEOUT timer
;	   jne	   no_motoroff		;

	   callmotoroff:
	      SaveReg di			  ;   ...
	      mov di,Start_BDS			  ; beginning of BDS chain
	      .WHILE <di ne -1> 		  ; chase the chain
		 .IF <cl e [di].DriveNum>	  ;   ...
		    and [di].DrvFlag,NOT Fmotoron ; stop the drive motor
		 .ENDIF 			  ;   ...
		 mov di,[di].Link		  ; next BDS
	      .ENDWHILE 			  ;    ...
	      RestoreReg di			  ; restore original BDS
	      Call TMotorOff
	.ELSE				       ;   ...
	   Call TMotorOff		       ;   ...
	.ENDIF				       ;   ...
     .ENDIF				       ;   ...
     no_motoroff:
     inc bl
     mov di,[di].Link			       ; check next BDS entry
  .ENDWHILE				       ;   ...
  .IF <ch e 0>				       ; ch = 0 if all timers were off
;    and TimerActive,NOT 2		       ; clear timeout is active
     mov TimerActive,0			       ; clear timeout is active
  .ENDIF				       ;   ...
  popa					       ;   ...
  ret					       ;   ...

MotorOff endp

; This subroutine is moved from ABDSK2.ASM so that ATDSK2.ASM could use it
; Ptm 3005

public DmaIoctl
DmaIoctl  proc near

	mov	dl,DevHlp_FreeReqPacket ; Free Request Packet
	CALL	DWORD PTR [DevHlpFunc]

	mov	bx,DataOffset harderrreqblk
	mov	word ptr harderrreqblk,bx
	mov	word ptr harderrreqblk+2,DS
D0:
	mov	ax,ds
	call	pblock
D1:
	; yield to time critical thread
	DEVHLP DevHlp_TCYield		; Block until Proc_Run
	test	dmaoverrunflag,0ffh
	jz	D1
	jmp	short D0
	ret
DmaIoctl  endp

Code	ends

	end
