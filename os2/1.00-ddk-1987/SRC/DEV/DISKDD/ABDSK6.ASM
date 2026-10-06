;	SCCSID = @(#)abdsk6.asm	10.4 87/05/18
	page	,132
	title	ABDISK6 - MS OS/2  Category 8/9 IOCTL routines
	name	ABDISK6
	comment !
********************* Start of Specifications *************************
*								      *
*  Source File Name: ABDSK6.ASM 				      *
*								      *
*  Descriptive Name: Category 8/9 IOCTL routines and IOCTL function   *
*		     router					      *
*								      *
*  Copyright:							      *
*								      *
*  Status: Version 1.1						      *
*								      *
*  Function: IOCTL command router and category 8/9 function routines  *
*								      *
*  Notes:	     N/A					      *
*    Dependencies:   N/A					      *
*    Restrictions:   N/A					      *
*								      *
*  Entry Points:  DriveGenIOCTL 				      *
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
	extrn	Do_DiskIO:near

; Routines located in ABSUBR.ASM
	extrn	Get_BDS_Addr:near
	extrn	VirtToPhysESBX:near
	extrn	Get_Request_Packet:near
	extrn	Release_Request_Packet:near
	extrn	SetStatus:near
	extrn	StatusError:near
	extrn	SWait:near
	extrn	SSig:near
	extrn	ReadSecInScratch:near
	extrn	BPBFromBoot:near
	extrn	BPBFromScratch:near
	extrn	CHS_to_RBA:near
	extrn	Add_Partition_Offset:near
	extrn	SectorIndexToSectorSize:near
	extrn	SectorSizeToSectorIndex:near
	extrn	FloppyChange:near
; Routines located in ABDSK2.ASM and ATDSK2.ASM
	extrn	DmaIoctl:near		    ;  @p2463

Code	ends

	BREAK <External data references>
BioData segment

; Data located in ABDSK1.ASM
	extrn	Start_BDS:word
	extrn	ScratchBufSem:byte
	extrn	DevHlpFunc:dword
	extrn	NumDrv:word
	extrn	FDInfo:word

BioData ends

	.xlist				;
	include dosmac.inc		; public DOS macros
	include struc.inc		; structured macros
if1
	include abmac.inc		; private macros
endif
	include bpb.inc 		; bios parameter block
	include abdskequ.inc		; general equates
	include abddrv.inc		; IOStruc
	include abioctl.inc		; special IOCTL equates
	include devhlp.inc		; device helper equates
	include error.inc		; DOS error codes

	ListLevel %ablistlevel,%abdebug

	BREAK	<Data Segment>
BioData segment

		even
		dw	5				; highest allowed
IOREADJumpTbl	dw	CodeOffset IOCTL_Func_Err	; unsupported
		dw	CodeOffset IOCTL_Func_Err	; unsupported
		dw	CodeOffset IOCTL_Func_Err	; unsupported
		dw	CodeOffset GetDevParm		; 63
		dw	CodeOffset ReadTrack		; 64
		dw	CodeOffset VerifyTrack		; 65

		dw	5				; highest allowed
IOWriteJumpTbl	dw	CodeOffset IOCTL_Func_Err	; unsupported
		dw	CodeOffset IOCTL_Func_Err	; unsupported
		dw	CodeOffset IOCTL_Func_Err	; unsupported
		dw	CodeOffset SetDevParm		; 43
		dw	CodeOffset WriteTrack		; 44
		dw	CodeOffset FmtVerifyTrack	; 45



DummyBPB    dw	    0			; used by InstallBDS
DmaIoctlflag db     0			; @p2463

VirtParaPacketAddr DD	?		; Virtual Address of parm pkt p4027

BioData ends

	BREAK	<Code Segment>
Code	segment

	assume	cs:BiosSeg,ds:BiosGroup,es:nothing

	BREAK	<DriveGenIOCTL - function router>
;**  DriveGenIOCTL
;
;    Perform Generic IOCTL request for the drive
;
;   ENTRY   AL	  - UNIT #
;	    ES:BX - Packet Address
;	    Note: DS:DI does NOT point to a BDS entry and the unit
;		  number in AL has NOT been validated
;
;   EXIT    'C' clear if no error
;	    'C' set if error, AX contains error code

Procedure DriveGenIOCTL,near

LocalVar  ParaLockHandle,DWORD		   ; Parameter Packet Lock Handle
LocalVar  DataLockHandle,DWORD		   ; Data Packet Lock Handle

  .IF <es:[bx].GIOCategory eq IOC_PC>	   ; if it's category 9
     add  al,80H			   ; make unit 80H based
     mov  es:[bx].PktUnit, al		   ; update in packet
  .ELSEIF <es:[bx].GIOCategory ne IOC_DC>  ; if it's not category 8
     jmp  IOCTL_Func_Err2		   ; not category 8 or 9, exit
  .ENDIF
  call Get_BDS_Addr			   ; ds:di -> BDS entry for unit in al
  .IF c 				   ; carry set if not found
     mov al,ERROR_I24_BAD_UNIT		   ;   invalid unit number
     jmp StatusError			   ;   ...
  .ENDIF				   ;   ...

  .IF <DmaIoctlflag eq 0> and		    ; @p2463 begin
  .IF <es:[bx].GIOCategory eq IOC_DC> and   ; if it's category 8 and
  .IF <es:[bx].GIOFunction eq IOCTL_DMAOVERRRUN> ; dma overrun function code
     inc DmaIoctlflag
     SaveReg <es,bx>
     les  bx,es:[bx].GIOParaPack	   ;   ...
     mov  byte ptr es:[bx],0		   ; signal harderror daemon to close
     RestoreReg <bx,es> 		   ; handle
     jmp DmaIoctl
  .ENDIF				   ; @p2463 end

  ; Make sure data and parm packet segments are accessible
  ; and that the request will not go past the limit of the segment.
  ; This must be done before we lock the segments.

  call CheckAddresses			   ; check addresses in packet
  .IF c 				   ; was something invalid?
      mov al, ERROR_I24_INVALID_PARAMETER  ; yes, error return
      jmp StatusError			   ;  ...
  .ENDIF

  EnterProc <0,0,0,0>			   ; initialize stack frame

  SaveReg bx				   ; save request packet offset
  mov ax,word ptr es:[bx].GIODataPack+2    ; lock data packet segment
  xor bx,bx				   ; short term lock and block
  DEVHLP DevHlp_Lock			   ; lock the segment
  .IF c 				   ; exit if error
     RestoreReg bx			   ; restore request packet offset
     push STDON + STERR + ERROR_I24_BAD_COMMAND
     jmp  Unlock_Neither		   ; no data packet
  .ENDIF				   ;   ...
  mov  DataLockHandleh,ax		   ; save data packet handle
  mov  DataLockHandlel,bx		   ;   ...
  RestoreReg bx 			   ; restore request packet offset

  mov ax,word ptr es:[bx].GIOParaPack+2    ; now do the parameter packet
  SaveReg	ax
  or  ax, word ptr es:[bx].GIOParaPack	   ; check for and ignore null pointers
  RestoreReg	ax
  .IF nz				   ;   ...
     SaveReg bx 			   ; save request packet offset
     xor bx,bx				   ; short term lock/block until locked
     DEVHLP DevHlp_Lock 		   ; lock the segment
     .IF c				   ; check for errors
	RestoreReg bx			   ; restore request packet offset
	push STDON + STERR + ERROR_I24_BAD_COMMAND
	jmp  Unlock_Data		   ; unlock only the data packet
     .ENDIF
     mov ParaLockHandleh,ax		   ; save the parmater packet handle
     mov ParaLockHandlel,bx		   ;   ...
     RestoreReg bx			   ; restore request packet offset
  .ENDIF

  .IF <es:[bx].GIOFunction eq IOCTL_ExtDrive>
     call InstallBDS			   ; service function 22H
     .IF c				   ; treat an error as bad command
	mov ah,(STDON+STERR)/256	   ; set error return code
	push ax 			   ; al set already
     .ELSE				   ;   ...
	push STDON			   ; good status
     .ENDIF				   ;   ...
  .ELSE 				   ; normal IOCTL command
     xor ch,ch				   ; search the jump table
     mov  cl,es:[bx].GIOFunction	   ; get the function code
     test cl,GEN_IOCTL_FN_TST		   ; read or write type ?
     .IF z				   ;   ...
	mov si,DataOffset IOWriteJumpTbl   ; write type function - 40's
     .ELSE				   ;   ...
	mov si,DataOffset IOReadJumpTbl    ; read type function  - 60's
     .ENDIF				   ;   ...
     and cl,0FH 			   ; isolate function code
     cmp cx,[si-2]			   ; function in range ?
     .IF be				   ; error exit
	shl  cx,1			   ; form word index
	add  si,cx			   ;   ...
	mov  al, es:[bx].PktUnit	   ; put unit back in al
	call [si]			   ; call the function handler
	.IF nc				   ; carry set if error
	   mov ax,STDON 		   ; normal completion
	.ELSE				   ;   ...
	   mov ah,(STDON+STERR)/256	   ; error completion
	.ENDIF				   ;   ...
     .ELSE				   ;
	mov ax,STDON + STERR + ERROR_I24_BAD_COMMAND
     .ENDIF				   ;
     push ax				   ; save status word
  .ENDIF				   ;   ...

  mov ax,ParaLockHandleh		   ; unlock parameter packet
  or  ax,ax				   ; don't unlock if high word is zero
  .IF nz				   ;   ...
     mov bx,ParaLockHandlel		   ;   ...
     DEVHLP DevHlp_UnLock		   ; unlock parameter packet
  .ENDIF				   ;   ...

Unlock_Data:				   ; here if only data locked
  mov  ax,DataLockHandleh		   ; unlock data packet
  mov  bx,DataLockHandlel		   ;   ...
  DEVHLP DevHlp_UnLock			   ; unlock data packet

Unlock_Neither: 			   ; here if neither packet locked
  pop ax				   ; restore status word
  LeaveProc				   ; restore stack
  jmp SetStatus 			   ; return to kernel

EndProc DriveGenIOCTL, NoCheck

	BREAK <CheckAddresses>
;**  CheckAddresses - Checks the user addresses in the request packet
;
;	This routine checks the user addresses in the request packet
;	to make sure the user has access to the segments and that the
;	request will not go past the limit of the segments.
;
;	ENTRY	ES:BX => request packet (containing virtual addresses)
;		DS:DI => BDS for the drive
;
;	EXIT	'C' clear if access and limit were ok
;		'C' set if access should be denied
;
;	USES	Flags

DebugPublic CheckAddresses
CheckAddresses proc

    pusha				    ; save regs
    mov  al, es:[bx].GIOFunction	    ; get IOCTL function code
    and  al, 0FH			    ; get low 4 bits

    .IF  <al eq 4> or			    ; if Read or Write Track
    .IF  <al eq 5>			    ; or Verify or FmtVerify
	; first check parameter packet
	; setup regs for DevHlp_VerifyAccess

	mov  cx, size DDI_RWVPacket_param   ; cx = size of a parm packet
	mov  ax, word ptr es:[bx].GIOParaPack+2 ; ax = parm pack segment
	push di 			    ; save BDS address
	mov  di, word ptr es:[bx].GIOParaPack	; di = parm pack offset
	mov  dh, 0			    ; verify for read access
	DEVHLP DevHlp_VerifyAccess	    ; 'C' set if access denied
	pop  di 			    ; restore BDS address
	.IF c				    ; if access was denied
	    jmp StopChecking		    ; stop checking
	.ENDIF

	; now check data packet; only for Read Track or Write Track
	; for Verify or FmtVerify just clear 'C'

	mov  al, es:[bx].GIOFunction	    ; get IOCTL function code
	and  al, 0FH			    ; get low 4 bits
	.IF  <al eq 4>			    ; if Read or Write
	    mov  cx, ds:[di].BPBsecsiz	    ; cx = bytes per sector
	    push ds			    ; save our ds
	    lds  si, es:[bx].GIOParaPack    ; ds:si => user parameters
	    mov  ax, ds:[si].DDI_RWV_NumSec ; ax = # of sectors requested
	    mul  cx			    ; ax = total bytes requested
	    pop  ds			    ; restore ds

	    ; setup regs for DevHlp_VerifyAccess
	    mov  cx, ax 		    ; cx = total bytes requested
	    mov  ax, word ptr es:[bx].GIODataPack+2 ; ax = data packet seg
	    mov  di, word ptr es:[bx].GIODataPack   ; di = data packet offset
	    test es:[bx].GIOFunction, GEN_IOCTL_FN_TST	; read or write type?
	    .IF z			    ; if Write Track
		mov dh, 0		    ; verify for mem read access
	    .ELSE			    ; if Read Track
		mov dh, 1		    ; verify for mem r/w access
	    .ENDIF
	    DEVHLP DevHlp_VerifyAccess	    ; 'C' set if access denied
	.ELSE				    ; Verify or FmtVerify
	    clc 			    ; no need to check data packet
	.ENDIF

    .ELSEIF  <al eq 3>			    ; if Get or Set DevParms
	; first check parameter packet
	; setup regs for DevHlp_VerifyAccess

	mov  cx, size DDI_DeviceParameters_param ; cx = size of a parm packet
	mov  ax, word ptr es:[bx].GIOParaPack+2 ; ax = parm packet segment
	mov  di, word ptr es:[bx].GIOParaPack	; di = parm packet offset
	mov  dh, 0			    ; verify for read access
	DEVHLP DevHlp_VerifyAccess	    ; 'C' set if access denied
	.IF c				    ; if access was denied
	    jmp StopChecking		    ; stop checking
	.ENDIF

	; now check data packet
	; cat 8 and cat 9 data packets are different sizes

	.IF <es:[bx].GIOCategory eq IOC_DC> ; if it's category 8
	    mov  cx, size DDI_DeviceParameters_data
	.ELSE				    ; must be category 9
	    mov  cx, size DDI_PhysDeviceParameters_data
	.ENDIF
	mov  ax, word ptr es:[bx].GIODataPack+2 ; ax = data packet seg
	mov  di, word ptr es:[bx].GIODataPack	; di = data packet offset
	test es:[bx].GIOFunction, GEN_IOCTL_FN_TST  ; read or write type?
	.IF z				    ; if SetDevParms (cat 8 only)
	    .IF <es:[bx].GIOCategory ne IOC_DC> ; if it's not cat 8
		stc			    ; bad request (cat 9 func 43H)
		jmp  StopChecking	    ; stop checking
	    .ENDIF
	    mov dh, 0			    ; verify for mem read access
	.ELSE				    ; if GetDevParms
	    mov dh, 1			    ; verify for mem r/w access
	.ENDIF
	DEVHLP DevHlp_VerifyAccess	    ; 'C' set if access denied

    .ELSE
	clc				    ; clear 'C' for all others
    .ENDIF

StopChecking:				    ; 'C' set if invalid parameter
    popa				    ; restore regs
    ret 				    ; return

CheckAddresses endp


;**   IOCTL_Func_Err - set bad command status and exit

IOCTL_Func_Err proc

  mov al,ERROR_I24_BAD_COMMAND		; error code
  stc					; indicate error
  ret					; error exit

IOCTL_Func_Err endp

;**   IOCTL_Func_Errc - set bad command status and jmp to StatusError

IOCTL_Func_Err2 proc

  mov al,ERROR_I24_BAD_COMMAND		   ; error code
  jmp StatusError			   ; error exit

IOCTL_Func_Err2 endp


    BREAK <Get Device Parameters>
;**  GetDevParm - Get device parameters - function 63H
;
;	GetDevParm gets the device parameters for the drive specified.
;	This is the only IOCTL routine that works differently between
;	category 8 and category 9.
;
;	ENTRY	ES:BX	DOS request packet
;		AL	Unit #
;		DS:DI	BDS address
;
;	EXIT	'C' set if any error occurs, AX has status
;		Relevant fields in user packet are set

	DebugPublic GetDevParm
GetDevParm proc

    .IF <es:[bx].GIOCategory eq IOC_DC> 	; if it's category 8

	; get command information out of users parameter packet
	; to determine if they want recommended or media BPB

	call GetCommandInfo			; ah = command info
	.IF <ah eq 0>				; 0 = recommended, 1 = media
	    lea si,[di].BPBRsecsiz		; return recommended BPB
	.ELSE					;   ...
	    test [di].DrvTypFlag,RETURN_FAKE_BPB ; this bit set by SetDevParm
	    .IF z				; Return BPB already in BDS
		call BuildNewBPB		;   ...
		RETC				; ax has error code
	    .ENDIF				;   ...
	    lea si,[di].BPBsecsiz		; return media BPB
	.ENDIF					;   ...

	SaveReg <es,bx> 			; save request packet
	SaveReg <di>				; and BDS
						; ds:si -> BPB to return

	les  di,es:[bx].GIODataPack		; es:di -> data packet
	push di 				; mov orginal user offset to si
						; (see pop si below)
	lea  di,es:[di].DDI_DP_BPB		; es:di -> user BPB area
	cld					;   ...
	mov  cx,size a_BPB			; copy the BPB to user packet
	rep  movsb				; ds:[si] -> es:[di]
	pop  si 				; es:si -> data packet
	RestoreReg <di> 			; ds:di -> BDS for drive
	mov  al,byte ptr ds:[di].FormFactor	; device type
	mov  byte ptr es:[si].DDI_DP_DeviceType,al  ;	...
	mov  ax,word ptr ds:[di].DrvTypFlag	; device attributes
	and  ax,fNon_Removable+fChangeline	; mask off other bits
	mov  word ptr es:[si].DDI_DP_DeviceAttr,ax  ;	...
	mov  ax,word ptr ds:[di].TotalPhysCyln	; number of cylinders
	mov  word ptr es:[si].DDI_DP_Cyln,ax	;   ...

    .ELSE					; it's category 9

	 call GetCommandInfo			; ah = command info
	.IF <ah ne 0>				; must be 0
	   mov al,ERROR_I24_BAD_COMMAND 	; error code
	   stc					; set error code
	   ret
	.ENDIF
	SaveReg <es,bx> 			; save request packet
						; ds:di -> BDS
	les  si,es:[bx].GIODataPack		; es:si -> data packet
	mov  ax,ds:[di].TotalPhysCyln		; get # of cylinders
	mov  es:[si].DDI_PDP_Cylinders, ax	; put in user data
	mov  ax,ds:[di].BPBnhead		; get # of heads
	mov  es:[si].DDI_PDP_Heads, ax		; put in user data
	mov  ax,ds:[di].BPBtrksiz		; get sectors per track
	mov  es:[si].DDI_PDP_Sectors, ax	; put in user data

    .ENDIF

    RestoreReg <bx,es>				; restore request packet
    xor  ax, ax 				; zero ax
    clc 					; good return code
    ret 					; return

GetDevParm endp

	BREAK <Set Device Parameters>
;**  SetDevParm - Set device parameters - function 43H
;
;	SetDevParm sets the device parameters for the drive specified.
;
;	ENTRY	ES:BX	DOS request packet
;		DS:DI	BDS entry address
;		AL	Unit #
;
;	EXIT	Relevant fields in the BDS for the drive are set

	DebugPublic SetDevParm
SetDevParm proc

  call GetCommandInfo				; ah = command info
  .IF <ah eq 0> 				; revert to using media
     and [DI].DrvTypFlag,NOT RETURN_FAKE_BPB	;  for BUILDBPB
     ret					;
  .ENDIF					; ah=command info

  SaveReg <es,ds,di>				; save regs
  push	  ds					; copy ds to es (pop later)
  lds	  si,es:[bx].GIODataPack		; ds:si -> virtual address of
						;	   data packet
  pop	  es					; es:di -> BDS
  .IF <ah eq 1> 				; Change the default BPB
     mov ax, [si].DDI_DP_Cyln			; change device related parms
     mov es:[di].TotalPhysCyln, ax		;
     mov ax, [si].DDI_DP_DeviceAttr		;
     and ax, fNon_Removable+fChangeline 	;
     and es:[di].DrvTypFlag, NOT (fNon_Removable + fChangeline)
     or  es:[di].DrvTypFlag, ax 		;
     mov ah, byte ptr [si].DDI_DP_DeviceType	;
     mov es:[di].FormFactor, ah 		;
     lea di,[di].BPBRsecsiz			; es:di -> default BPB
  .ELSE 					; Change the media BPB
     or  es:[di].DrvTypFlag,RETURN_FAKE_BPB	; BUILDBPB over ride
     lea di,[di].BPBsecsiz			; es:di -> media BPB
  .ENDIF					;
						; es:di -> BDS BPB to change
  lea	  si,[si].DDI_DP_BPB			; ds:si -> users BPB
  cld						;
  mov	  cx,size a_BPB 			; number of bytes to transfer
  rep	  movsb 				; copy BPB to user area
  RestoreReg <di,ds,es>

  clc
  ret

SetDevParm endp

	BREAK <Verify Track>
;**  VerifyTrack - Verify a single track
;
;     VerifyTrack verifies the track specified. It uses the track layout
;     that is passed by the user.
;
;     ENTRY
;	    ES:BX points to device request packet
;	    DS:DI BDS entry address
;	    AL	  unit #
;
;     EXIT
;	    'C' set if error, AL has error code
;	    'C' clear if no error
;
;     USES  AX, CX, SI, Flags

VerifyTrack proc

	mov	cl,IOCTL_Verify
	call	TrackRWV
	ret

VerifyTrack endp

	BREAK <Read Track>
;**  ReadTrack - Read a single track - function 64h
;
;     ReadTrack reads a track off the disk. The layout described in
;     the user track table is assumed.
;
;     ENTRY
;	 ES:BX points to read packet
;	 DS:DI BDS entry address
;	 AL - unit #
;
;     EXIT
;	    'C' set if error, AL has error code
;	    'C' clear if no error
;
;     USES  AX, CX, SI, Flags

ReadTrack proc

	mov	cl,IOCTL_Read
	call	TrackRWV
	ret

ReadTrack endp

	BREAK <Write Track>
;**  WriteTrack - Write a single track
;
;     WriteTrack writes out the data onto a single track. The track
;     layout described in the user track table is assumed. This is a
;
;     ENTRY
;	 ES:BX points to write packet
;	 DS:DI BDS entry address
;	 AL - unit #
;
;     EXIT
;	    'C' set if error, AL has error code
;	    'C' clear if no error
;
;     USES  AX, CX, SI ,Flags
DebugPublic WriteTrack
WriteTrack proc

	mov	cl,IOCTL_Write
	call	TrackRWV
	ret

WriteTrack endp

	BREAK <TrackRWV>
;** TrackRWV - Read/Write/Verify the track
;
;	TrackRWV reads/writes/verifies the track specified with the
;	layout specified in the packet.
;
;	ENTRY	DS:DI	BDS
;		ES:BX	Request packet
;		AL	Unit #
;		CL	Function code
;
;	EXIT
;		'C' set if error, AX contains status word
;		'C' clear of no error
;
;	USES	AX, CX, DX, SI, BP, Flags

	DebugPublic TrackRWV
procedure   TrackRWV, NEAR

localVar	RPkt,DWORD		; Original request packet
localVar	PPkt,DWORD		; Original parm packet
localVar	DataSeg,WORD		; Device driver data segment
localVar	LoopCnt,WORD		; Loop counter

  mov  ch,al				; save unit number

; check for wrong pseudo-drive mounted

  SaveReg cx				; save unit and function #
  call CheckFloppy
  RestoreReg cx
  RETC

; Initialize local varibles

  EnterProc
  mov  DataSeg,ds
  mov  word ptr RPkt, bx
  mov  word ptr RPkt+2, es

; Verify user parameters

  les  bx,es:[bx].GIOParaPack		; es:bx -> parm packet
  mov  word ptr PPkt, bx		; intialize local varibles
  mov  word ptr PPkt+2, es		;   ...
  call VerifyParameters 		; return 'c'with ax=error if error
  .IF c
    les bx,RPkt
    leaveproc
    ret
  .ENDIF

; Allocate a new request packet to send to state machine

  call Get_Request_Packet		; es:bx -> new request packet
  mov  es:[bx].PktLen,LENSpecPkt	;
  mov  es:[bx].PktUnit,ch		;
  mov  es:[bx].PktCmd,DISKOP_IOCTL	;
  mov  es:[bx].IOC_Fun,cl		;

; Pass physical address of data packet to state machine

  SaveReg <es,bx>			; save new request packet
  les  bx,RPKT				; es:bx -> request packet
  les  bx,es:[bx].GIODataPack		; es:bx -> data packet
  call VirtToPhysESBX			; ax:bx -> data packet (phys addr)
  mov  cx,bx				;
  RestoreReg <bx,es>			; restore new request packet
  mov  word ptr es:[bx].IOpData,cx	;
  mov  word ptr es:[bx].IOpData+2,ax	;

; Set up new requst packet

  lds si,PPkt				; ds:si -> parm packet
  mov ax,ds:[si].DDI_RWV_Cyln		; ax = cylinder
  mov es:[bx].IOC_Cyln,ax		;
  mov ax,ds:[si].DDI_RWV_Head		; al = head
  mov es:[bx].IOC_Head,ax		;
  mov cx,ds:[si].DDI_RWV_NumSec 	; number of sectors
  mov ax,ds:[si].DDI_RWV_Sec		; logical starting sector
  mov dx,1				;
  test ds:[si].DDI_RWV_Command,1	; simple track layout table ?
  .IF nz				; nz = simple table
     xchg cx,dx 			;
  .ENDIF				;   ...
  mov es:[bx].IOcount,dx		; save number of sectors to do
  lea si,ds:[si].DDI_RWV_TTable 	; ds:si -> track table
  shl ax,2				; X 4 for dword offset
  add si,ax				; ds:si -> first track table entry


; cx	=  loop counter
; es:bx -> new request packet
; ds:si -> track layout table
;    di -> offset of BDS

  mov  LoopCnt,cx			; initialize loop counter
; .REPEAT				;
TRWV:					;

; set up sector id to do

     mov cx,ds:[si]			; cl = sector (1 based)
     dec cx				;   convert to 0 based
     mov es:[bx].IOC_StartSec,cx	;   and save for state machine

; Set up sector size

     mov ax,ds:[si + 2] 		; sector size
     call SectorSizeToSectorIndex	; convert to index
     mov es:[bx].IOC_SecSiz,al		;   and save for state machine

; set up diskette characterists for non 512 byte sectors

    mov ds,DataSeg			; ds -> device driver data segment
    assume ds:BiosGroup 		; ds OK again

; Set up starting RBA

     mov cl,1				; cl = first sector
     mov ch,byte ptr es:[bx].IOC_Head	; ch = head
     mov ax,es:[bx].IOC_Cyln		; ax = cylinder
     call CHS_to_RBA			; convert CHS to logical RBA
     mov word ptr es:[bx].IOstart,ax	; dx,ax = logical RBA
     mov word ptr es:[bx].IOstart+2,dx	;    ...
     call Add_Partition_Offset		; adjust to logical partition

; Do the IO: IOCTLs pass the starting sector in two parts. IOPhysRBA points to
; the first sector on the track. IOC_StartSec has the physical starting sector
; id (0-based). This allows non-standard track layout (copy-protect).

     or  [di].DrvFlag,FForceRdWrt	; force the operation
     and [di].DrvFlag,NOT FMedInd	;   and reset known media indicator
     mov cx,es:[bx].IOcount		; set up for DISK_IO
     SaveReg si 			; save track layout indes
     call Do_DiskIO			; Do_DiskIO does not return until
     RestoreReg si

     test ax,STERR			; quit if an error occurred
     jnz Finished

; Increment for next iteration

     mov  ds,word ptr PPkt+2		; ds:si-> track table
     assume ds:nothing			; ds not pointing to BiosGroup
     mov ax,ds:[si + 2] 		; sector size
     add word ptr es:[bx].IOpData,ax	; adjust data buffer
     adc word ptr es:[bx].IOpData+2,0	;   ...
     add si,4				; ds:si -> entry for next sector

     dec  LoopCnt			; Decrement loop counter
     jz   Finished			;   ... 		       p4389
     jmp  TRWV				;   ... 		       p4389

Finished:
  mov  ds,DataSeg			;   ...
  assume ds:BiosGroup			; ds is good again
  call Release_Request_Packet		; finished with new request packet
  test ax,STERR 			; set carry if an error occurred
  .IF nz				;   ...
     stc				;   ...
  .ELSE 				;   ...
     clc				;   ...
  .ENDIF				;   ...
  les  bx,Rpkt				; es:bx -> request packet
  LeaveProc
  ret					;   ...

EndProc TrackRWV


	BREAK <VerifyParameters>
;**  VerifyParameters - Verify user parameters for an IOCTL read/write/verify
;
;
;     ENTRY	ds:di -> BDS for the drive
;		es:bx -> user parameters
;
;     EXIT
;		'C' set if error, AX contains status word
;		'C' clear if parameters are all ok
;
;     USES	AX, Flags
     Public VerifyParameters
VerifyParameters proc

  mov  ax, es:[bx].DDI_RWV_Head 	; get requested head (0-based)
  .IF <ax ge ds:[di].BPBnhead>		; if head is too big
      jmp InvalidParm			; get out
  .ENDIF

  mov  ax, es:[bx].DDI_RWV_Sec		; get starting sector (0-based)
  add  ax, es:[bx].DDI_RWV_NumSec	; add # of sectors requested (1-based)
  .IF <ax gt ds:[di].BPBtrksiz> 	; if sector is too big
      jmp InvalidParm			; get out
  .ENDIF

  .IF <<byte ptr es:[bx].DDI_RWV_Command> eq 1>     ;if standard layout
      .IF <<word ptr es:[bx].DDI_RWV_TTable> ne 1>  ;if 1st entry not 1
	    jmp InvalidParm
      .ENDIF
  .ELSEIF <<byte ptr es:[bx].DDI_RWV_Command> ne 0> ;must be non-standard
	jmp InvalidParm
  .ENDIF
					; parameters are ok
  clc					; clear 'C'
  ret					; return

InvalidParm:
  mov  ax, STERR + ERROR_I24_INVALID_PARAMETER	; error return
  stc					; set 'C'
  ret					; return

VerifyParameters endp


	BREAK <Format/Verify>
;**  FmtVerifyTrack - Format and Verify a Track - function 45
;
;	FmtVerifyTrack formats and verifies a track based on the
;	track layout table passed by the user.	If the given unit
;	resides on a hard disk, the format portion is skipped.
;
;	ENTRY	ES:BX	DOS request packet
;		AL	Unit #
;		DS:DI	BDS entry address
;
;	EXIT	'C' set if any error, AX contains status
;		'C' clear if no error

	DebugPublic FmtVerifyTrack
procedure	FmtVerifyTrack, NEAR

localVar	DataSeg,WORD	  ; Device Driver Data Segment
localVar	RPkt,DWORD	  ; Request packet
localVar	LoopCnt,Word	  ; Loop Counter
localVar	PPkt,DWORD	  ; Parm packet

; check for wrong pseudo-drive mounted

    mov  cx,ax					; move unit number
    SaveReg cx
    call CheckFloppy
    RestoreReg cx
    RETC

; Initialize local varibles

    EnterProc
    mov  DataSeg,ds
    mov  word ptr RPkt, bx
    mov  word ptr RPkt+2, es
    mov  ax,word ptr es:[bx].GIOParaPack
    mov  word ptr PPkt, ax
    mov  ax,word ptr es:[bx].GIOParaPack+2
    mov  word ptr PPkt+2, ax

; Get a new request packet and copy parms from original request packet

    call Get_Request_Packet			; es:bx -> req pkt  p4238
    mov  byte ptr es:[bx].PktUnit,cl		; unit number
    mov  es:[bx].PktCmd,DISKOP_IOCTL		; IOCTL command code
    mov  es:[bx].PktLen,LENSpecPkt		;
    lds  si,PPkt				; ds:si -> parm packet
    mov ax,[si].DDI_FP_Cyln			; cylinder number
    mov es:[bx].IOC_Cyln,ax			;   ...
    mov ax,[si].DDI_FP_Head			; head number
    mov es:[bx].IOC_Head,ax			; fill in new request packet
    mov ax,[si].DDI_FP_NumSec			; number of sectors
    mov es:[bx].IOcount,ax			;   ...
    xor ax,ax					; get a zero
    mov es:[bx].IOC_StartSec,ax 		; starting sector = 0
						;  passed to state machine
    mov ah,byte ptr [si].DDI_FP_TTable+3	; Get sector size index
    mov es:[bx].IOC_Secsiz,ah			;
    mov ds,DataSeg				; Restore DS, ds:di -> BDS

; Only format diskettes, disks are just verified

    test [di].DrvTypFlag, fNon_Removable	; see if it's a hard disk
    .IF z near					; if not a hard disk; format
	mov byte ptr es:[bx].IOC_Fun,IOCTL_Format ; format

; Pass physical address of data packet to state machine

	SaveReg <es,bx> 		      ; save new request packet
	les  bx,PPkt			      ; es:bx -> parm packet
	call VirtToPhysESBX		      ; ax:bx -> parm packet (phys addr)
	mov  cx,bx			      ;
	RestoreReg <bx,es>		      ; restore new request packet
	add  cx, DDI_FP_TTable		      ; get address of track table
	adc  ax, 0			      ; ax:cx -> track table
	mov  word ptr es:[bx].IOpData,cx      ;
	mov  word ptr es:[bx].IOpData+2,ax    ;

; Pass RBA of first sector to state machine

	mov  ax, es:[bx].IOC_Cyln		; ax = cylinder
	mov  ch, byte ptr es:[bx].IOC_Head	; ch = head
	mov  cl, 1				; cl = sector
	call CHS_to_RBA 			; convert CHS to logical RBA
	mov  word ptr es:[bx].IOPhysRBA, ax	; put RBA in new req packet
	mov  word ptr es:[bx].IOPhysRBA+2, dx	;    ...

; Set index for request and do it

	or   [di].DrvFlag,FForceRdWrt		;
	call Set_DASD				;
	xor  dx,dx				; Starting sector = 0
	mov  al, byte ptr es:[bx].PktUnit	; al = unit
	mov  cx, es:[bx].IOcount		; cx = sector count
	call Do_DiskIO				; format
	or   [di].DrvTypFlag,fChanged_By_Format ; Indicate media changed

; Check for error

	test ax,STERR				;
	.IF nz					;
	    call Release_Request_Packet 	;		   p4027
	    les  bx,RPkt			; restore request packet
	    stc 				;
	    LeaveProc
	    ret 				;
	.ENDIF					;
    .ENDIF

;----------------------------------------------------------------------
; Verify for both hard disks and floppies
;
; es:bx -> new request packet
; ds:di -> BDS
;----------------------------------------------------------------------

; Set up new request packet for format

    mov  es:[bx].IOC_Fun,IOCTL_Verify
    mov  word ptr es:[bx].IOpData,0	       ; No data address
    mov  word ptr es:[bx].IOpData+2,0

; Figure out how many sectors can be done in one IO request

    lds  si,PPkt				; ds:si -> parm packet
    mov  cx,[si].DDI_FP_NumSec			; get num requested sectors
    mov  dx,1					; 1 sector at a time
    test [si].DDI_FP_Command,1			; Consecutive sectors in tbl?
    .IF nz					; If consecutive sectors
       xchg cx,dx				; process all sectors in 1 loop
    .ENDIF
    mov     es:[bx].IOcount, dx 		; Save sector per IO count
    mov     word ptr LoopCnt,cx 		; Set up loop count

    lea  si,[si].DDI_FP_TTable			; DS:SI -> track table

fvt3:
;----------------------------------------------------------------------
; loop for non-standard layouts, or one pass for standard
;
; ES:BX -> new	request packet
; DS:SI -> track table
;----------------------------------------------------------------------

; set up sector id and sector size

    mov     ds,word ptr PPkt+2			; ds:si -> track table
						; (needed for loop)
    inc     si
    inc     si					; skip cyl and head entries
    lodsb					; al = sector #, 1-based
    cbw
    dec     ax					; ax = sector #, 0-based
    mov     es:[bx].IOC_StartSec,ax		;  passed to state machine
    lodsb					; al = sector size
    mov     es:[bx].IOC_SecSiz,al
    mov     ds, DataSeg 			; ds:di -> BDS

; Set up RBA of first sector

    mov cl,1					; cl = first sector
    mov ch,byte ptr es:[bx].IOC_Head		; ch = head
    mov ax,es:[bx].IOC_Cyln			; ax = cylinder
    call CHS_to_RBA				; convert CHS to RBA
    mov word ptr es:[bx].IOPhysRBA,ax		; dx,ax = RBA
    mov word ptr es:[bx].IOPhysRBA+2,dx 	;    ...
    call Add_Partition_Offset			; adjust to logical partition

; Do the IO

    mov     al,es:[bx].PktUnit
    mov     cx, es:[bx].IOcount
    or	    [di].DrvFlag,FForceRdWrt
    and     [di].DrvFlag,NOT FMedInd
    SaveReg si					; save track layout index
    call    DO_DiskIO
    RestoreReg si

; Check for error

    test    ax,STERR
    jnz     fvt7

; Check "until" condition

    dec     LoopCnt
    jz	    fvt9
    jmp     fvt3				;		    p4389


; Good ending

fvt9:						;		    p4389
    call    Release_Request_Packet
    clc
    jmp     fvt8

; Error exit

fvt7:
    call    Release_Request_Packet
    stc
fvt8:
    les  bx,RPkt				; es:bx -> request packet
    LeaveProc
    ret

EndProc FmtVerifyTrack, NoCheck


PAGE

	BREAK <Install BDS>
;**  InstallBDS - Install the BDS for an external drive - function 22H
;
;	InstallBDS copies over the BDS passed into an empty BDS that
;	is available in the data area. It is an error if all the BDSs
;	have been used up.
;
;	ENTRY	ES:BX	DOS request packet
;
;	EXIT	'C' set if any error
;		'C' clear if no error

	DebugPublic InstallBDS
InstallBDS proc

  mov ax,NumDrv 			; number of logical drives
  shl ax,1				; word index
  mov di,DataOffset FDInfo		; start of BDS offset array
  add di,ax				; offset of next free one
  .IF <<word ptr [di]> eq 0>		; zero if storage freed
     stc				; signal error
     ret				; error exit
  .ENDIF				;   ...
  mov di,word ptr [di]			; DriveStruc offset + BPB offset
  sub di,BPBsecsiz			; back to start of BDS
					; ds:di -> free BDS
  SaveReg <es,bx>			; save request packet
  push ds				; copy ds to es (later pop)
  lds  si,es:[bx].GIOParaPack		; ds:si -> passed BDS
  pop  es				; es:di -> new BDS
  SaveReg <di>				; Save pointer to new BDS
  cld					;   ...
  mov cx,size DriveStruc		;   ...
  repe movsb				; copy over the BDS
  push es				; mov ds,es (if there was one)
  pop  ds				;   ...
  RestoreReg <di>			; ds:di -> new BDS
  RestoreReg <bx,es>			; es:bx -> request packet
  assume ds:BiosGroup,es:nothing	;   ...
  mov ah,byte ptr NumDrv		; assume unit number is next one
  mov [di].DriveUnit,ah 		; 0-based unit number

; Perform aliasing

  mov ah,[di].DriveNum			; caller's DriveNum
  mov si,Start_BDS			; chase the BDS chain to see if
  .WHILE <si ne -1>			;   we have an entry for this
     .IF <[si].DriveNum eq ah>		;   and "alias" it if we do
	or  [si].DrvTypFlag,fI_Am_Mult	;   set fI_Am_Mult in both
	or  [di].DrvTypFlag,fI_Am_Mult	;   reset I_Own_Physical in new
	and [di].DrvTypFlag,NOT fI_Own_Physical
     .ENDIF				;   ...
      mov si,[si].Link			;   ...
  .ENDWHILE				;   ...

; Adjust a few things

  add [di].DriveLet,1			; convert unit to letter
  mov word ptr [di].PartitionOffset,0	; Partition starts at 0
  mov word ptr [di].PartitionOffset+2,0

; Link in the new BDS

  mov si,di				; ds:si -> new BDS
  mov ax,NumDrv 			; get a 0 based highest
  dec ax				;   unit in al
  call Get_BDS_Addr			; ds:di -> BDS to add after
  DISABLE				;   ...
  mov ax,[di].Link			; old link
  mov [di].Link,si			; point at new one
  mov [si].Link,ax			; now inserted
  ENABLE				;   ...
  inc NumDrv				; count the one just added

  mov ah,[si].DriveUnit 		; new unit
  mov es:[bx].PktUnit,ah		; return unit number for this drive
  lea di,[si].BPBsecsiz 		; and a dummy array of BPB's
  mov DummyBPB,di			;  ...
  mov di,DataOffset DummyBPB		;  ...
  mov word ptr es:[bx].InitpBPB,di	; return pointer to BPB "array"
  mov word ptr es:[bx].InitpBPB+2,ds	;   ...
  clc					; good return code
  ret					;   ...

InstallBDS endp

	BREAK <BuildNewBPB>
;**  BuildNewBPB - Build a new BPB for the medium in the drive
;
;	BuildNewBPB builds the BPB for the drive by first reading
;	the boot sector and passing it on to BPBFromBoot. The
;	sequence of events is the same as for the DOS BUILD BPB command.
;
;	ENTRY	    AL	    unit #
;		    ES:BX   DOS request packet address
;		    DS:DI   BDS entry address
;
;	EXIT	    'C' set if error, AX contains status.
;
;	REGISTERS USED	AX,CX,Flags

	DebugPublic BuildNewBPB
BuildNewBPB proc

  call CheckFloppy		      ; handle the pseudo-drive
  RETC				      ; we need to switch diskettes
  SaveReg <es,bx,ds,di> 	      ;   ...
  mov  bx,DataOffset ScratchBufSem    ; get scratch buffer and
  call SWait			      ;   wait for it to free
  xor  cl,cl			      ; read in the boot sector
  call ReadSecInScratch 	      ; es:bx -> boot sector
  .IF nc			      ; return if error in reading sector
     mov si,di			      ; set up for BPBFromBoot
     RestoreReg <di,ds> 	      ; restore BDS address
     call BPBFromBoot		      ; try to use boot sector BPB
     .IF c			      ; boot sector had an invalid BPB
	mov  bx,DataOffset ScratchBufSem
	call SSig		      ; release the scratch buffer
	RestoreReg <bx,es>	      ; es:bx -> DOS request packet
	call BPBFromScratch	      ; BPBFromScratch also gets ScratchBuf
     .ELSE			      ; boot sector BPB was good
	mov  bx,DataOffset ScratchBufSem
	call SSig		      ; release the scratch buffer
	RestoreReg <bx,es>	      ; es:bx -> DOS request packet
	clc			      ; set good status
     .ENDIF			      ;   ...
  .ELSE 			      ;   ...
     mov  bx,DataOffset ScratchBufSem ;
     call SSig			      ; release the scratch buffer
     RestoreReg <di,ds,bx,es>	      ; clean the stack
     mov al,ERROR_I24_READ_FAULT      ; error code
     stc			      ; set bad status
  .ENDIF			      ;   ...
  ret				      ;   ...

BuildNewBPB	endp

	BREAK <Set_DASD>
;**  Set_DASD - Set the type of medium in the drive for formatting
;
;     Set_DASD sets the relevant fields in the BDS to tell the driver
;     what type of medium is in the drive. This *MUST* be done before
;     trying to format the medium because the controller cannot determine
;     the parameters for the drive otherwise.
;
;     ENTRY   DS:DI -> BDS for the drive
;
;     EXIT    NONE
;
;     EFFECTS The relevant fields in the BDS are set appropriately.

Set_DASD proc

  TEST [di].DrvTypFlag,fNon_Removable	; fixed disk ?
  .IF z 				; do nothing if a fixed disk
     SaveReg <ax,bx>			;   ...
     mov bl,[di].FormFactor		; get form factor
     .IF <bl eq ff48tpi>		; 360 KB ?
	mov al,INDEX44			;   ...
     .ELSEIF <bl eq ff96tpi>		; 1.2 MB ?
	.IF <[di].BPBtrksiz eq 15>	;   see if a lower density medium
	   mov al,INDEX99		;   is in the drive
	.ELSE				;   ...
	   mov al,INDEX49		;   ...
	.ENDIF				;   ...
     .ELSEIF <bl eq ff35_9>		; 720 KB ?
	mov al,INDEX71			;   ...
     .ELSEIF <bl eq ff35_18>		; 1.44 MB ?
	.IF <[di].BPBtrksiz eq 18>	;   see if a lower density medium
	   mov al,INDEX11		;   is in the drive
	.ELSE				;   ...
	   mov al,INDEX71		;   ...
	.ENDIF				;   ...
     .ELSE				; not an expected form factor
	mov al,INDEX11			; default to 1.44 MB in 1.44 MB drive
     .ENDIF				;   ...
     mov [di].CurIndex,al		; update BDS
     or  [di].DrvFlag,fMedInd		;   ...
     RestoreReg <bx,ax> 		;   ...
  .ENDIF				;   ...
  ret					;   ...

Set_DASD endp


	BREAK <GetCommandInfo>
;**	GetCommandInfo - Get command byte from parameter packet
;
;	ENTRY	ES:BX	DOS request packet
;		AL	Unit #
;
;	EXIT	ES:BX	Virtual address of parameter packet.
;		AH	command byte of request packet
;		CX	destroyed

GetCommandInfo	proc

	SaveReg <es, bx>		; save request packet
	les  bx,es:[bx].GIOParaPack	; es:bx -> parm packet
	mov  ah, es:[bx].DDI_DP_Command ; 0, 1, or 2
	RestoreReg <bx, es>	    ; ES:BX is virtual address of packet
	ret

GetCommandInfo	endp

	BREAK <CheckFloppy>
;*****	CheckFloppy - Check to ensure that correct floppy is in the drive
;
;	CheckFloppy checks to ensure that the correct floppy disk is in
;	the drive. This would be needed for drives that have more than one
;	logical drive assigned to them.
;
;	ENTRY
;	    AL - Unit #
;	    ES:BX - Request Packet (this **MUST** be the one that will be
;		    used for the same request once the DOS has prompted for
;		    the correct disk).
;	    CX - Sector count (if relevant)
;
;	EXIT
;	    If disk change required (Carry Set)
;		AX - Status to return to DOS to cause prompt
;		CX = 0
;	    If no change required (Carry Clear)
;		AH = 0
;		AL - Unit #
;
;	REGISTERS AFFECTED AX,CX,Flags

CheckFloppy proc

  SaveReg di				       ;   ...
  call Get_BDS_Addr			       ; ds:di -> BDS for passed unit
  test word ptr [di].DrvTypFlag,fNon_Removable ; ignore fixed disks
  .IF z 				       ; no check for fixed drives
     call FloppyChange			       ; see if should change disks
     .IF <ah lt 0>			       ; ah = -1 if change required
	 and [di].DrvFlag,NOT FMedInd	       ; media has or may have changed
	 mov ax,STERR+STDON+error_I24_disk_change
	 xor cx,cx			       ; zero sectors transferred
	 stc				       ; ERROR - disk switch required
     .ENDIF				       ;   ...
  .ENDIF				       ;   ...
  RestoreReg di 			       ;   ...
  ret					       ;   ...

CheckFloppy endp

Code	ends

	end
