;	SCCSID	=  @(#)abinit.asm	5.15 87/03/18
	page	,132
	title	ABINIT - MS OS/2 1.1 disk device driver
	name	ABINIT
	comment !
********************* Start of Specifications *************************
*								      *
*  Source File Name: ABINIT.ASM 				      *
*								      *
*  Descriptive Name: INIT 0 command and initialization		      *
*								      *
*  Copyright:							      *
*								      *
*  Status: Version 1.1						      *
*								      *
*  Function: Provides the DOS command 0 (INIT) command handler	      *
*								      *
*  Notes:							      *
*								      *
*    Dependencies: N/A						      *
*								      *
*    Restrictions: N/A						      *
*								      *
*  Entry Points:  DriveInit					      *
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
	extrn	Disk_IO:near

; Routines located in ABDSK2.ASM/ATDSK2.ASM
	extrn	DriveExecute:near
  CodeInit segment
	extrn	Floppy_Init:near
  CodeInit ends

; Routines located in ABDSK3.ASM/ATDSK3.ASM
	extrn	FixedExecute:near
  CodeInit segment
	extrn	Fixed_Init:near
  CodeInit ends

; Routines located in ABSUBR.ASM
	extrn	CmdErr:near
	extrn	StatusComplete:near
	extrn	VirtToPhysDSBX:near
	extrn	Get_BDS_Addr:near

; Routines located in ABDSK4.ASM
	extrn	TimerInit:near

; Routines located in ATINT13.ASM/ABINT13.ASM
	extrn	Int13Init:near


Code	ends

	BREAK <External data references>
BioData segment

; Data located in ABDSK1.ASM
	extrn	Start_BDS:word		; BDS chain anchor
	extrn	NumDrv:byte		; device count in driver header
	extrn	FDInfo:word		; BPB offset array
	extrn	DevHlpFunc:dword	; device helper routine address
	extrn	LDrive80:near,LDrive81:near,LDriveB:near ; BDS chains
	extrn	LDriveA:near				 ; BDS chains
	extrn	Drive80:word,Drive81:word,DriveB:word, ; BDS chains
	extrn	DriveC:word, DriveA:word	       ; BDS chains
	extrn	ScratchBuffer:word	; 512 byte I/O buffer
	extrn	ScratchBufPhys:dword	; 32 bit address of ScratchBuffer
	extrn	dtable:word		; DriveRequest dispatch table

; Data located in ABDSK2.ASM/ATDSK2.ASM
	extrn	NumFlop:byte		; count of real/pseudo drives
	extrn	Drive:byte		; floppy IOStruc

; Data located in ABDSK3.ASM/ATDSK3.ASM
	extrn	Fixed:byte		; fixed disk IOStruc
	extrn	NumFix:byte		; count of number of hard disks

; Data located in ABDSK4.ASM
	extrn	TimerActive:byte

; Data located in ABDSK5.ASM		;
	extrn	NumPart:byte		; number of partiotions
	extrn	NextFreeBDS:word	; offset to next available BDS
	extrn	LastLogicalBDS:word

BioData ends

	.xlist				;
	include struc.inc		; structured macros
if1
	include abmac.inc		; private macros
endif
	include ibmrom.inc		; AT low core ROM data area
	include abint13.inc		; special ROM/BIOS equates
	include devhlp.inc		; Device helper definitions.
	include error.inc		; DOS error codes
	include bpb.inc 		; BPB layout
	include abdskequ.inc		; general equates
	include abddrv.inc		; IOStruc
	include atstate.inc		; state machine definitions
	include dconfig.inc		; configuration data struc


	ListLevel %ablistlevel,%abdebug

	BREAK	<Data Segment>
BioData segment 		; FloppyDevice must be 1st

	public InitTime 	; will call spins on I/O when 0
InitTime	db	0	; set to 0FFH after initialization

	public fHaveDRV2	; SYSINIT looks at this
fHaveDRV2	db	0	; default to no external 3.5" drive

ScratchMoved	db	0	; indicates whether the scratch buffer moved
	public	SysInfoSeg
SysInfoSeg	dw	0	; sysInfoSeg selector

BioData ends			;   ...

DataInit segment		; throw away data


BDS_size equ size DriveStruc	; size of BDS	!!.. go to ABDDRV.INC ...!!!


; The following are the recommended BPBs for supported media.

	public BPB48T
BPB48T		dw	512		    ; 48 tpi diskettes
		db	2		    ; sectors/cluster
		dw	1		    ; reserved sectors
		db	2		    ; number of FATs
		dw	112		    ; number of directory entries
		dw	2*9*40		    ; total sectors
		db	MB_48_9Sect	    ; media byte
		dw	2		    ; sectors/FAT
		dw	9		    ; sectors/track
		dw	2		    ; heads
		dd	0		    ; hidden sectors

	public BPB35L
BPB35L		dw	512		    ; 3.5 inch 1.44 MB diskette
		db	1		    ; sectors/cluster
		dw	1		    ; reserved sectors
		db	2		    ; number of FATs
		dw	224		    ; number of directory entries
		dw	2*18*80 	    ; total sectors
		db	MB_35_18Sect	    ; media byte
		dw	9		    ; sectors/FAT
		dw	18		    ; sectors/track
		dw	2		    ; heads
		dd	0		    ; hidden sectors

	public BPB96T
BPB96T		dw	512		    ; 96 tpi diskettes
		db	1		    ; sectors/cluster
		dw	1		    ; reserved sectors
		db	2		    ; number of FATs
		dw	224		    ; number of directory entries
		dw	2*15*80 	    ; total sectors
		db	MB_96		    ; media byte
		dw	7		    ; sectors/FAT
		dw	15		    ; sectors/track
		dw	2		    ; heads
		dd	0		    ; hidden sectors

	public BPB35S
BPB35S		dw	512		    ; 3.5 inch 720 KB diskette
		db	2		    ; sectors/cluster
		dw	1		    ; reserved sectors
		db	2		    ; number of FATs
		dw	112		    ; num dir entries
		dw	2*9*80		    ; total sectors
		db	MB_35_9Sect	    ; media byte
		dw	3		    ; sectors/FAT
		dw	9		    ; sectors/track
		dw	2		    ; heads
		dd	0		    ; hidden sectors

NumExtraBDSs	db	2		    ; # of extra BDSs for aliasing

DataInit ends

	BREAK	<Command 0 - INIT>
CodeInit segment

	assume	cs:BiosSeg,ds:BiosGroup,es:nothing

;=======================================================================
; Miscellaneous notes:
;
; 1. Device types/sizes:    Size     Cylinders	Heads  Sectors/Track
;
;    320/360 KB diskettes   5.25"        40       2         8/9
;    1.2 MB	    "       5.25"        80       2         15
;    720 KB	    "       3.5"         80       2          9
;    1.4 MB	    "       3.5"         80       2         18
;
;    20 MB fixed disk			615	  4	    17
;    30 MB   "    "                     733       5         17
;
;=======================================================================

	BREAK <DriveInit>
;**  DriveInit - Disk device driver initialization
;		    This routine runs in protect mode only as a single
;		    thread at level 3 with IOPL.
;
;	ENTRY	ES:BX  request packet
;
;	EXIT	return to SYSINIT

	public DriveInit
DriveInit proc

if abdebug
  INT3					  ; debug break point
endif

  mov ax,word ptr es:[bx].InitpEnd	  ; save the DevHlp
  mov word ptr DevHlpFunc,ax		  ;  routine address
  mov ax,word ptr es:[bx].InitpEnd+2	  ;  for the DEVHLP macro
  mov word ptr DevHlpFunc+2,ax		  ;   ...


;=============================================================================;
;=  If there is a 64k boundary in our scratch buffer then we must move it.   =;
;=									     =;
;=									     =;
;=	  before		     during		     after	     =;
;=     initialization		  initialization	  initialization     =;
;=									     =;
;=									     =;
;=  |-----------------| 	|-----------------|	|-----------------|  =;
;=  |  ScratchBuffer  | 	|  BDS chain	  |	|  BDS chain	  |  =;
;=  |		      | 	|	 .	  |	|	 .	  |  =;
;=  |		      | 	|	 .	  |	|	 .	  |  =;
;=  |		      | 	|	 .	  |	|-----------------|  =;
;=  |-----------------| 	|		  |	|  ScratchBuffer  |  =;
;=  |		      | 	|		  |	|		  |  =;
;=  | BDS chain       | 	|		  |	|		  |  =;
;=  |	    .	      | 	|-----------------|	|		  |  =;
;=  |	    .	      | 	|  ScratchBuffer  |	|-----------------|  =;
;=  |	    .	      | 	|		  |			     =;
;=  |		      | 	|		  |	     fig 3.	     =;
;=  |		      | 	|		  |			     =;
;=  |-----------------| 	|-----------------|			     =;
;=  | init data       | 	| init data	  |			     =;
;=  |		      | 	|		  |			     =;
;=  |-----------------| 	|-----------------|			     =;
;=									     =;
;=	 fig 1. 		      fig 2.				     =;
;=									     =;
;=									     =;
;=  step 1. - Before initialization (fig 1.) if the ScratchBuffer lies on    =;
;=	      64k boundary we must switch the BDS Chain with the	     =;
;=	      ScratchBuffer (fig 2.)					     =;
;=									     =;
;=  step 2. - After initialization when we get rid of the unecessary data    =;
;=	      area (i.e - the invalid BDS entries) we must move up the	     =;
;=	      ScratchBuffer (fig 3.).  We must make sure that we do not      =;
;=	      move it up onto the boundary again!			     =;
;=									     =;
;=============================================================================;

  SaveReg bx				  ; save request packet offset
  mov bx, ScratchBuffer 		  ; save the 32 bit physical
  call VirtToPhysDSBX			  ; ax:bx = 32 address
  mov word ptr ScratchBufPhys,bx	  ; save as a dword
  mov word ptr ScratchBufPhys+2,ax	  ;   ...
  RestoreReg bx 			  ; restore packet offset

  cmp	WORD PTR ScratchBufPhys, -(SCRATCHSIZE)
  ja	on_boundary
  jmp	not_on_boundary
on_boundary:
    pusha
    SaveReg es

    mov   ScratchMoved, 1			; indicate that scratched buffer
						; moved
    push  ds
    pop   es
    mov   di, ScratchBuffer			; mov bds chain up
    mov   Start_BDS, di 			; reinit Start_BDS
    mov   si, DataOffset LDriveA
    mov   cx, (SIZE DriveStruc)*28		; for A-Z, 80, and 81
    rep   movsb
    add   ScratchBuffer, (SIZE DriveStruc)*28	; adjust offset to scratchbuffer

    mov   dx, word ptr ScratchBufPhys+2 	; Adjust physical address of
    mov   ax, word ptr ScratchBufPhys		; scratch buffer.
    add   ax, (SIZE DriveStruc)*28		;	 .
    adc   dx, 0 				;	 .
    mov   word ptr ScratchBufPhys+2, dx 	;	 .
    mov   word ptr ScratchBufPhys  , ax 	;	 .

    mov   si, Start_BDS 			; recalculate BDS chain
						; offsets and links
;======================     offsets	  ===========================;
    mov   DriveA, si

    mov   DriveB, si
    add   DriveB, SIZE DriveStruc

    mov   Drive80, si
    add   Drive80, (SIZE DriveStruc) * 2

    mov   Drive81, si
    add   Drive81, (SIZE DriveStruc) * 3

    mov   DriveC, si
    add   DriveC,  (SIZE DriveStruc) * 4

;========================   links  ==================================;
    mov   ax, DriveB					;  A:BDS ->B:BDS
    mov   [si].Link, ax

    mov   ax, Drive80					;  B:BDS ->80:BDS
    mov   [si].Link + ((SIZE DriveStruc) * 1) , ax

    mov   ax, Drive81					; 80:BDS ->81:BDS
    mov   [si].Link + ((SIZE DriveStruc) * 2) , ax

    ; Drive81 is already -1 terminated			; 81:BDS ->FFFFh

;====================================================================;

    mov   si, DriveB			; last logical BDS so far
    mov   LastLogicalBDS, si		; last logical BDS so far
    mov   si, DriveC			; next available BDS
    mov   NextFreeBDS, si		; next available BDS

    mov   si,DataOffset FDInfo		; return pointer to the BPBs
    mov   ax, driveA
    add   ax, BPBsecsiz
    mov   [si], ax			; and re_initialize
    mov   ax, driveB
    add   ax, BPBsecsiz
    mov   [si+2], ax

    RestoreReg	es
    popa
not_on_boundary:


; initialize disk device driver trace
;
  call TraceInit			  ; initialize trace

; initialize timer
;
  call TimerInit			  ; initialize timer

; Setup BDSs
;
  call SetROMCfg			  ; set h/w config data for Flop/Fix BDS
  call Floppy_Init			  ; diskette initialization
  call Fixed_Init			  ; fixed disk initialization

; initialize Int 13 emulation
;
  call Int13Init			  ; initialize INT 13

; return number of units
;
  mov ah,NumFlop			  ; return number of units
  .IF <ah eq 1> 			  ; add the pseudo-drive if necessary
     inc ah				  ;   ...
  .ENDIF				  ;   ...
  add ah,NumPart			  ;  = total number of logical devices
  mov NumDrv,ah 			  ; update device driver header
  mov es:[bx].InitcUnit,ah		  ; update request packet

; return pointer to BPBs
;
  .IF <ah ne 0> near			  ; any units supported ?
     mov si,DataOffset FDInfo		  ; return pointer to the BPBs
     mov word ptr es:[bx].InitpBPB,si	  ;   ...
     mov word ptr es:[bx].InitpBPB+2,ds   ;   ...
     .IF <fHaveDrv2 eq 0>		  ; need external drive INT 13H code?
	mov si,CodeOffset BiosExtDrv	  ;    no
     .ELSE				  ;  ...
	mov si,CodeOffset CodeInit	  ;   yes, so just return INIT stuff
     .ENDIF				  ;   ...

; return offset ending address of Code
;
     mov word ptr es:[bx].InitpEnd,si	  ;   ...


; Set the address of BPB in BPB array.
;
;   On return of INIT command, pointer to BPB entry array (FDInfo) is set
;   in INIT command packet.
;   As default, the 1st entry is BPB of drive A and 2nd is of drive B.
;   Even if there is only one floppy dirve in the system, BPB for drive B
;   is still in FDInfo to support pseudo drive.
;   In addtition to the above two BPB entries, we have to add the BPBs
;   for all logical drives. (drive C, D,....).
;   And in addtion, we need one extra BPB entry per physical floppy drive,
;   and also another two BPB when external 3.5" drive exists.
;

; calculate total # of extra BPB entries
;
     mov al,NumFlop			 ;
     .IF <fHaveDRV2 ne 0>		 ; external drive exist?
       add al,2 			 ;   then -> two more BPB entries
     .ENDIF				 ;
     mov NumExtraBDSs,al		 ; total # of extra BPBs

; calculate total # of BPBs  (-> AL)
;
     mov al,NumDrv			 ;
     add al,NumExtraBDSs		 ; AL -> total # of BDS entries to keep
     mov ah,al				 ; save to AH

; fill BPB array with BPB address for logical fixed drive (C,D,E..),
; aliasing function and external drive if necessary.
;
     mov si,DataOffset FDInfo+4 	 ; SI -> BPB pointer entry following
					 ;	 Drive B BPB pointer in FDInfo.
     mov di,Start_BDS			 ;
     .IF <NumFix eq 0>			 ; No fixed disk?
       mov di, Drive80			 ;  then BPB for physical, logical
     .ELSE					 fixed drive not needed.
       .WHILE <[di].DriveUnit ne 2>	 ;  else find 1st logical fixed drive(c:)
	 add di,BDS_size		 ;
       .ENDWHILE			 ;
     .ENDIF				 ;
     add di,BPBsecsiz			 ;
     .FOR al = 3 to ah			 ; set BPB address other than Drive A,B
       mov [si],di			 ; BPBs.
       add si,2 			 ;
       add di,BDS_size			 ; DI -> point to next Drive BPB
     .NEXT al				 ;

; retrun offset ending address of Data
;
     sub di,BPBsecsiz			 ; DI -> first released BDS
     dec di				 ; decrement DI (-> end of valid BDSs)

     .IF <ScratchMoved eq  1>		 ; if the scratch buffer moved then
	mov  ScratchBuffer, di		 ; mov the scratchbuffer back up
	SaveReg bx			 ; save request packet offset
	mov bx, ScratchBuffer		 ; save the 32 bit physical
	call VirtToPhysDSBX		 ; ax:bx = 32 address
	mov word ptr ScratchBufPhys,bx	 ; save as a dword
	mov word ptr ScratchBufPhys+2,ax ;   ...
	RestoreReg bx			 ; restore packet offset
	add  di, SCRATCHSIZE		 ; it will be at the bottom of the

	cmp   WORD PTR ScratchBufPhys, -512  ; Check to see if a 64k boundary
	jna   not_on_boundary2		     ; is still on the scratchbuffer.
	mov   ax, WORD PTR ScratchBufPhys    ; If it is then mov the scratch
	neg   ax			     ; buffer to just past the boundary.
	mov   WORD PTR ScratchBufPhys, 0     ; Adjust the physical address.
	inc   WORD PTR ScratchBufPhys+2      ;
	add   ScratchBuffer, ax 	     ;
	add   di, ax			     ; Adjust the end of data segment
       not_on_boundary2:		     ; address.
     .ENDIF				 ; data segment

     mov word ptr es:[bx].InitpEnd+2,di  ; point to end of the data segment
					 ; or scratch

  .ELSE 				 ;   ...
     mov word ptr es:[bx].InitpBPB,0	 ; if no units, release all storage
     mov word ptr es:[bx].InitpBPB+2,0	 ;   and don't return a BPB array
     mov word ptr es:[bx].InitpEnd,0	 ;   ...
     mov word ptr es:[bx].InitpEnd+2,0	 ;   ...
  .ENDIF				 ;   ...

; don't allow another INIT command, spin on I/O
;
  mov si,CodeOffset CmdErr		 ; don't allow another INIT command
  mov dtable,si 			 ;   by patching the dispatch table
  mov InitTime,-1			 ; don't spin on I/O any more
  xor al,al				 ; zero error return code

; return to SYSINT
;
  jmp StatusComplete			 ; return to SYSINIT

DriveInit endp

	BREAK <Spin_IO>
;**  Spin_IO - perform state machine requests during initialization
;
;    Spin_IO is used during INIT processing because the DevHlp Block and
;    Run routines cannot be called. Spin_IO goes into a hard loop waiting
;    for the STDON bit in PktStatus to be set. If an error occurred, STERR
;    will also be set. Spin_IO is "returned" after INIT, so do not call it
;    from the mainline code: use Do_DiskIO or Disk_IO
;
;    ENTRY  ES:BX  contains a hard coded request packet in DataInit
;	    DS:DI  BDS entry for the device
;
;    EXIT   status is in PktStatus - STDON and maybe STERR
;
;    USES   flags

	public Spin_IO
Spin_IO proc

  pusha 					;   ...
  SaveReg es					;   ...
  mov es:[bx].PktStatus,0			; zero status
  test word ptr [di].DrvTypFlag,fNon_Removable	; floppy or hard disk ?

;=======================================================================
;			   floppy disk I/O
;=======================================================================

  .IF z 				   ; floppy disk handling
     mov  word ptr Drive.Queue,bx	   ;   insert packet in queue
     mov  word ptr Drive.Queue+2,es	   ;   ...
     mov  TimerActive,0 		   ; clear any active floppy timers
     mov  Drive.State,Start		   ; want to start I/O
     mov  Drive.Active,-1		   ; set the semaphore
     ENABLE				   ;   ...
     call DriveExecute			   ; start up the state machine

;=======================================================================
;			     hard disk I/O
;=======================================================================

  .ELSE 				   ; fixed disk processing
     mov  word ptr Fixed.Queue,bx	   ;   insert packet in queue
     mov  word ptr Fixed.Queue+2,es	   ;   ...
     mov  Fixed.State,Start		   ; want to start I/O
     mov  Fixed.Active,-1		   ; set the semaphore
     ENABLE				   ;   ...
     call FixedExecute			   ; start up the state machine
  .ENDIF				   ;   ...

  RestoreReg es 			   ;   ...
  popa					   ;   ...
  .REPEAT				   ; spin here until the I/O is
     test es:[bx].PktStatus,STDON	   ; complete
  .UNTIL nz				   ;   ...
  ret					   ;   ...

Spin_IO endp


;** SetROMCfg - copy H/W configuration parameters passed in INIT command
;		packet to BDS blocks for each drive.
;		And also, set  # of physical floppy drives (NumFlop) and
;		fixed disks (NumFix).
;
;		When only one floppy drive exits, create the BDS for
;		Drive B (pseudo drive) by copying ROMCfg for Drive A.
;
;		H/W configuration data is passed from System Initialization.
;
; Entry:  ES:BX -> point to INIT Command packet
;
; Exit:   set the following parameters
;
;	  DriveStruc variables:
;
;	       cClyn	     -	 # of cylinders
;	       TotalPhysCyln -	 Total # of cylinders in partition
;	       DriveNum      -	 0-based floppy drive #,80h-based fixed drive #
;	       BPBsecsiz     -	 # of sectors per track
;	       BPBnhead      -	 # of heads
;	       WrtPrCmp      -	 Write precompensation factor
;	       DrvTypFlag    -	 bit 0 : non-removable media
;				 bit 1 : changeline is supported
;
;	    (only for Fixed Drive)
;
;	       FormFactor    -	 form factor
;
;	  other variables:
;
;	       NumFlop	     -	 # of floppy drives
;	       NumFix	     -	 # of fixed drives
;
      DebugPublic SetROMCfg
SetROMCfg proc
  pusha
  SaveReg <es>
  push	 word ptr es:[bx].InitpBPB	;
  push	 word ptr es:[bx].InitpBPB+2	;
  pop	 es				;ES:SI -> point to initial parameter
  pop	 si				;	  pointer
  push	 es:[si+2]			;ES:SI -> point to H/W configuration
  pop	 si				;	  data (ROMCfg chain)
  mov	 di,Start_BDS			;DS:DI -> start of BDS block

  SaveReg si
CountDrive:				; Save pointer to ROM data
  test es:[si].ROMflags,fNon_Removable	; Fixed Drive's ROM data?
  .IF nz				;
    inc NumFix				; then, count up NumFix
  .ELSE 				;
    inc NumFlop 			; else, for Flop Drive's ROM data,
  .ENDIF				; count up NumFlop.
  mov si,es:[si].ROMLink		; SI -> point to next ROM data
  .IF <si ne 0> 			; end of data?
    jmp CountDrive			;  no, get next
  .ENDIF				;
  RestoreReg si 			; Restore pointer to ROM data

SetCfg:
  mov ax,es:[si].ROMcyls		; maximum number of cylinders
  mov [di].cCyln,ax			;
  mov [di].TotalPhysCyln,ax		;
  mov ax,es:[si].ROMsecptrk		; sectors per track
  mov [di].BPBtrksiz,ax 		;
  mov al,es:[si].ROMdevnbr		; 0-based floppy/80-based fixed disk #
  mov [di].DriveNum,al			;
  mov ax,es:[si].ROMheads		; number of heads
  mov [di].BPBnhead,ax			;
  mov ax,es:[si].ROMwpf 		; Write precompensation factor
  mov [di].WrtPrCmp,ax			;
  mov ax,es:[si].ROMflags		; DrvTypflag
  or  [di].DrvTypFlag,ax		; (bit 0:fixed disk, bit 1:changeline)

  test [di].DrvTypFlag,fNon_Removable	;
  .IF nz				; fixed drive data?
     mov [di].FormFactor,ffHardFile	;  set Hard disk FormFactor
  .ELSEIF <NumFlop eq 1>		;  one-drive system?
    .IF <[di].DriveUnit eq 0>		;    make pseudo drive
      mov di,[di].Link			;    DS:DI -> next BDS to set
      jmp SetCfg			;    set up pseudo Drive B:
    .ENDIF				;
  .ENDIF				;

  mov si,es:[si].ROMLink		; ES:SI -> next ROMdata to set
  .IF <si ne 0> 			; end of ROM data?
    mov di,[di].Link			;  DS:DI -> next BDS to set
    jmp SetCfg				;  set up next BDS
  .ENDIF

  RestoreReg <es>			;
  popa					;
  ret					;
SetROMCfg endp				;

;** AllocBDSChain - delete BDSs for non-existing physical fixed disks
;		    which are defined as default in ABDSK1.ASM.
;
; Called by: Fixed_Init
; Entry:     NumFix	 -  # of fixed drives
; Exit:      NexFreeBDS  -  point to offset of next free BDS
;
;      Terminate BDS chain by seting -1 to link pointer of the last
;      BDS in linkage.	 (not physical address in memory).
;
;	     # of fixed disks		   terminated BDS
;		    0			     DriveB
;		    1			     Drive80
;		    2			     Drive81
;
;      When only one fixed disk exists, BDS defined for Drive81
;      will not be used any more. So, in this case, the data area
;      for BDS after Drive80 is reassgined to each drive to
;      minimize returned BDS data area.
;
;			       link -1 --+    +---- NextFreeBDS
;	       BDS chain		 V    V
;		   +---------------------------------------------
;    (default)	   |  A   |  B	|  80  |  81  |  C  |  D  |  E	| ....
;		   +---------------------------------------------
;
;			 link -1 --+   +---- NextFreeBDS
;				   V   V
;		   +---------------------------------------------
;    (1 fixed	   |  A   |  B	|  80  |  C   |  D  |  E  |  F	| ....
;     disk exits)  +---------------------------------------------
;
;
public AllocBDSChain
AllocBDSChain proc
  pusha
  .IF <NumFix e 0>		  ; no fixed disk exists?
;    mov LDriveB.Link,-1	  ;  then, terminate BDS chain at DriveB
     mov si, DriveB		  ;  then, terminate BDS chain at DriveB
     mov [si].link, -1		  ;  then, terminate BDS chain at DriveB
  .ELSEIF <NumFix e 1>		  ; only one fixed disk?
;    mov LDrive80.Link,-1	  ;  then terminate BDS chain at Drive80
     mov si, Drive80		  ;  then terminate BDS chain at Drive80
     mov [si].Link, -1		  ;  then terminate BDS chain at Drive80
     mov di, Drive81		  ;  DI -> start of DriveC BDS (new)
     mov NextFreeBDS,di 	  ;  initialize pointer to next free BDS
    .FOR dl = 'C' to 'Z'          ;
       mov si,di		  ;  SI -> BDS to set up
       add di,BDS_size		  ;  DI -> offset to next BDS
       mov [si].Link,di 	  ;  set pointer to next BDS
       mov al,dl		  ;
       sub al,'A'                 ;  calculate 0-based unit # (->AL)
       mov [si].DriveUnit,al	  ;  set drive Unit #  (0-based)
    .NEXT dl			  ;
  .ENDIF			  ;
  popa				  ;
  ret				  ;
AllocBDSChain endp

;** TraceInit	  - Initialize Trace Information
;
;
;      Find out where our trace activation bit is so we will trace only
;      when needed.
;
;
TraceInit     proc
  pusha
  push	 es
  mov	 al,1			  ; Get SysInfoSeg
  DEVHLP DevHlp_GetDOSVar
  mov	 es,ax			  ; AX:BX => SysInfoSeg Selector
  mov	 ax,es:[bx]
  mov	 SysInfoSeg,ax		  ; Save for TRACEDD macro
  pop	 es
  popa
  ret
TraceInit    endp

CodeInit ends

	end
