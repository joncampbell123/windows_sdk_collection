;
; Copyright (C) Microsoft, 1988
;
	page	,132
	title	ATDISK3 - AT hard disk state machine
	name	ATDISK3
	comment !
********************* Start of Specifications *************************
*								      *
*  Source File Name: ATDSK3.ASM 				      *
*								      *
*  Descriptive Name: AT Hard Disk State Machine 		      *
*								      *
*  Function: Provides the fixed disk state machine and common	      *
*	     subroutines for supporting disk I/O.		      *
*								      *
*  Entry Points:  FixedExecute, FixedInterrupt			      *
*		  Reset_Fixed, Fixed_Init			      *
*								      *
********************** End of Specifications ************************** !

	.286p

	include abdebug.inc		; debug and listing control

	include absegs.inc		; code and data segment definitions

	BREAK <External code entry points>
Code	segment
; Routines located in ABDSK1.ASM
	extrn	DoneRequest:near

; Routines located in ABSUBR.ASM
	extrn	Get_BDS_Addr:near
	extrn	PhysToVirtESDI:near
	extrn	PhysToVirtDSSI:near
	extrn	UN_PhysToVirt:near
	extrn	UN_PhysToVirtES:near
        extrn   SectorSizeToSectorIndex:near
        extrn   SectorIndexToSectorSize:near

; Routines in ATCACHE.ASM
        extrn   Cache_Init:near

; Routines located in ATDSK2.ASM
	extrn	Setup:near
	extrn	MapSector:near

; Routines located in ABDSK4.ASM
	extrn	Cancel_F2Timer:near
	extrn	Set_F2Timer:near

; Routines located in ABDSK5.ASM
	extrn	Setup_Partition_BDSs:near  ; build BDSs for partitions

; Routines located in ATCACHE.ASM
        extrn   FreeCacheBuf:near
        extrn   CacheSector:near

; Routines loacated in ABINIT.ASM
  CodeInit segment
	extrn	AllocBDSChain:near	   ;
  CodeInit ends
Code	ends


	BREAK <External data references>

	.xlist				;
	include struc.inc		; structured macros
if1
	include abmac.inc		; private macros
endif
	include bpb.inc 		; bios parameter block
	include devhlp.inc		; device helper definitions
	include error.inc		; DOS error codes
	include abdskequ.inc		; general equates
	include abddrv.inc		; IOStruc
	include atstate.inc		; state machine definitions
	include abioctl.inc		; IOCTL equates
	include abint13.inc		; ROM and I/O equates
	include ibmrom.inc		; AT low core ROM data area
        include atcache.inc             ; Equates for caching on the AT
	include infoseg.inc		; Dos Info Seg (for TRACEDD macro
	include raststdv.inc		; TRACEDD macro

	ListLevel %ablistlevel,%abdebug

	BREAK	<Data Segment>

BioData segment

; Data located in ABDSK1.ASM
	extrn	DevHlpFunc:dword
	extrn	Drive80:word	       ; BDS of 1st physical hard disk

; Data located in ATINT13.ASM
	extrn	SemInt13:byte
	extrn	Block_Int13:byte      

; Data located in ABINIT.ASM
	extrn	SysInfoSeg:word

; Data located in ABDSK4.ASM

	extrn	F2Timer_Active:byte
	extrn	TimerInt:word
	extrn	F2Count:word

	public	NumFix, CacheActive, ATMCH

NumFix		db	0	; number of fixed drives.
ATMCH		db	1	; this is an AT machine
CacheActive	dw	0	; 1 = cache is active 
InterruptState	db	0	; Indicates whether or not we are expecting an
				; interrupt
				; (1=interrupt expected, 0=not expected)

NestedInts	dw	-1

; When we get a stage on interrupt return code the normal
; procedure would be to start off a "timeout" timer and cancel it when
; the interrupt occurs.
; There is a problem if the interrupt comes in BEFORE the the timer is set!
; (the nonexisting timer would be cancel and then the timer would be set
; with no one to cancel him - eek!)
; The solution to this problem is TimerCallFlag.  by incrementing it every
; time we want to start a timer and decrementing it every time a we want
; to cancel the timer we can tell if any timers are outstanding or NOT.

TimerCallFlag	dw	0

	public	Fixed
Fixed	IOStruc <,,IDLE>		; info about current fixed disk I/O

	db SIZE ATStruc - SIZE IOStruc dup (?)


;** Error codes corresponding to fixed controller errors bits returned in
;   the error register.

Hard_Err_Table	db	error_I24_gen_failure
		db	error_I24_seek
		db	error_I24_gen_failure
		db	error_I24_gen_failure
		db	error_I24_sector_not_found
		db	error_I24_gen_failure
		db	error_I24_read_fault
		db	error_I24_sector_not_found

;=======================================================
;==  Disk Device Drvier Trace information	      ==
;=======================================================

Trace_Cmplt_f	  db   0	  ; Flag  0 = no complete envt needed
				  ;	  1 = complete envt needed
Trace_Minor	  dw   ?	  ; Trace Minor Code

; The following area is sent to the system trace area

Trace_Area	  Label byte
Trace_Phys_id	  dw   ?	  ; id = 80 or 81

Trace_Cmplt_Stat  Label word	  ; Status for complete event
TRACE_SIZE_CMPLT  EQU  $-Trace_Area+2 ; Length of complete data packet
Trace_RBA	  dd   ?	  ; physical RBA, low dword
		  dd   0	  ; physical RBA, high dword, always 0
Trace_Sectors	  dw   ?	  ; Number of sectors to transfer
Trace_Buffer	  dd   ?	  ; Physical buffer address
TRACE_SIZE	  EQU  $-Trace_Area ; length of start data packet


BioData ends


	BREAK	<Code Segment>
Code	segment

	assume	cs:BiosSeg,ds:BiosGroup,es:nothing
CodeInit	segment

      public CacheIO
CacheIO proc
   ret				   ; just for delete linkage error
CacheIO endp

CodeInit  ends
;*
;

;
;************************************************************************

	BREAK <Hard Disk Interrupt Routine>
	comment !
****************** Start of Specifications ****************************
*								      *
* Subroutine Name: FixedInterrupt				      *
*								      *
* Descriptine Name: Fixed disk controller interrupt handler	      *
*								      *
* Function: service fixed disk controller interrupts - INT 76H	      *
*								      *
* Notes: entered in real and protect mode			      *
*	 entered in interrupt mode context			      *
*								      *
*	 This code replaces the ROM INT D handler (fixed disk	      *
*	 emulation). We first check to see if we are emulating	      *
*	 a ROM request.  if we were, we pass control off to the       *
*	 ROM routine, otherwise (if the drive is active), we	      *
*	 reset the WD1000 fixed disk controller, and check to see     *
*	 if it is in an error condition.  If it is in an error,       *
*	 we reissue the request for ErrLim times.  If there is no     *
*	 error, we just call FixedExecute and return.		      *
*								      *
* Entry Point: FixedInterrupt					      *
*								      *
* Linkage:  far call from kernel interrupt handler		      *
*	    DS	  - BiosGroup					      *
*								      *
* Input: none							      *
*								      *
* Exit-Normal: far ret to kernel				      *
*								      *
* Exit-Error:  far ret to kernel				      *
*								      *
* Effects:							      *
*								      *
* Internal Refereences: 					      *
*								      *
* External References:						      *
*								      *
************************ End of Specifications ************************ !

	DebugPublic FixedInterrupt
FixedInterrupt proc far
  .IF <Block_Int13 ne 0> or	     ; is current Int13 request 	  
				     ;	being blocked?			  
  .IF <SemInt13 eq 0> and	     ; is a direct Int13 request being made?
  .IF <Fixed.Active ne 0>	     ; device active?
				     ;
     mov     F2Timer_Active, 0	     ; ignore timer
				     ;
     cmp     InterruptState, 0	     ; check to see if this is a spurious
     je      Int13Interrupt	     ; interrupt (IF yes THEN GOTO Int13Interrupt
				     ;
     dec     TimerCallFlag	     ;
     cmp     TimerInt, 0	     ; Check to see if we are restarting the
     jnz     Int13Interrupt	     ; command via TimeOut timer (F2Timer).
				     ;
     inc     NestedInts 	     ;***
     jnz     DoEOI		     ;***

     ENABLE			     ;***
     mov     al, IRQ_Fixed	     ; (Use EOI DevHelp)
     DEVHLP  DevHlp_EOI 	     ;
PostProcess:			     ;***
     ENABLE			     ;
     call FixedExecute		     ; call state machine
     DISABLE
     dec     NestedInts 	     ;***
     cmp     NestedInts, -1	     ;***
     jne     PostProcess	     ;***

  .ELSE 			     ;	 ...

; The following code has been taken "almost verbatim" from the IBM ROM code.
; The only thing missing is the issuance of Int 15H.
; We need this code since we are unable to use the ROM routine in Protected
; mode.
Int13Interrupt:
     mov     ax,RomData 	     ;
     mov     es,AX		     ;
     assume  es:RomData 	     ;
     mov     HF_INT_FLAG,0FFh	     ;
DoEOI:				     ;***
     mov     al, IRQ_Fixed	     ; (Use EOI DevHelp)
     DEVHLP  DevHlp_EOI

;    ENABLE			     ;***
  .ENDIF			     ;
end_int:
  clc				     ;	 must inform INT mrg that it is ours
  ret				     ;

FixedInterrupt	endp

	BREAK <FixedExecute>
	comment !
************************ Start of Specifications *********************
*								     *
* Subroutine Name: FixedExecute 				     *
*								     *
* Descriptine Name: Fixed Disk device state machine		     *
*								     *
* Function: Perform hard disk I/O operation.			     *
*								     *
* Notes:							     *
*								     *
* Entry Point: FixedExecute					     *
*								     *
* Linkage:  near call						     *
*	    DS	  - BiosGroup					     *
*								     *
* Input: Fixed IOStruc						     *
*								     *
* Exit-Normal:							     *
*								     *
* Exit-Error:							     *
*								     *
* Effects:							     *
*								     *
* Internal Refereences: 					     *
*								     *
* External References:						     *
*								     *
************************* End of Specifications ********************** !

	BREAK <Fixed Disk state machine dispatch table>
BioData segment

public	FxExStart, FxExCalc, FxExError, FxExDone, FxExRead, FxExWrite
public	FxExVerify, FxExSetParam, FxExIdle


FixedDispatch	label word
	dw	CodeOffset FxExStart	   ; 0
	dw	CodeOffset FxExCalc	   ; 1
        dw      CodeOffset FxExParamSet    ; 2
        dw      CodeOffset FxExStartRead   ; 3
        dw      CodeOffset FxExStartWrite  ; 4
        dw      CodeOffset FxExStartVerify ; 5
        dw      CodeOffset FxExVerify      ; 6
	dw	CodeOffset FxExDone	   ; 7
	dw	CodeOffset FxExIdle	   ; 8
	dw	CodeOffset FxExError	   ; 9
        dw      CodeOffset FxExCacheRead   ; 10
	dw	CodeOffset FxExError	   ; 11
	dw	CodeOffset FxExRead	   ; 12
	dw	CodeOffset FxExWrite	   ; 13
	dw	CodeOffset FxExSetParam    ; 14

BioData ends

	BREAK <FixedExecute>
;** Fixed Execute
;
;	FixedExecute processes a disk request after it has been set up.
;	When the disk is inactive (State = Idle), it is called to start
;	the device.  For all subsequent events, it is called on the disk
;	interrupt which signalled the completion of that subfunction.
;	Some states do not involve waiting for an interrupt to occur.
;	This routine runs entirely off the 'Fixed' data structure.
;
;	This routine is called from FixedRead, if the state of the machine
;	is Idle, and from FixedInterrupt if we are not in the middle of a
;	request to the ROM.
	public FixedExecute
FixedExecute	proc

  xor bh,bh
  mov bl,Fixed.State	      ; Get current machine state
  shl bx,1
  jmp FixedDispatch[bx]       ; Dispatch to correct routine

	BREAK <Fixed Disk States>
;**	Fixed state Start
;
;	Do setup calculations to figure out sector, start
;	up motor, advance to Calc state.
;
;  Entered on initially picking up a new request to do and on error retries.
;  If error retries start here, then multiple sector requests will always start
;  at the beginning rather than at the point of the error! Why?

FxExStart:
	mov	si,DataOffset Fixed	; SI = pointer to per-device info.
	les	bx,Fixed.Queue		; ES:BX = pointer to current request
	call	Setup			; Do setup calculations
	call	Set_F2Timer

IF2
        ERRNZ   FxExCalc-$              ; Fall through
ENDIF

;*      Fixed state Calc
;
;       Calculate cylinder, head and sector. Set up command packet
;       for controller. Issue the Read or Write command.
;
;  Entered after SetParam state and also on further sectors of a
;  multiple sector request.

FxExCalc:
        mov     si,DataOffset Fixed     ; SI = pointer to per-device info.
        les     bx,Fixed.Queue          ; ES:BX = pointer to current request
        mov     al,es:[bx].PktUnit
        call    Get_BDS_Addr            ; DI = drive parameters
        call    MapSector               ; Get head, cylinder and sector
        CALL    CacheSector             ; Set up for caching
;
; Now do the operation. If the fCach_Hit bit is set, we can read the data
; from the cache buffer to user buffer.
;
        mov     bx,Fixed.Flags
        mov     al,SetParam             ; Assume next state is SetParam
	test	Fixed.CacheFlg,fCach_Hit
	jz	fec10			; Data is not in the cache
        mov     al,FxCacheRead          ; We are doing a cached READ
fec10:
        mov     Fixed.State,al
        jmp     FixedExecute            ; Go to next state

;*      Fixed state SetParam
;
; Set up the disk controller by telling it what type of device it is
; talking to. We have obtained this info. from the CMOS database.
; For multi-sector I/O, we need to tell the controller the max. numbers
; of heads and sectors/track.
;
FxExSetParam:
        les     bx,Fixed.Queue          ; ES:BX = pointer to current request
        mov     al,es:[bx].PktUnit
        call    Get_BDS_Addr            ; DI = drive parameters
        mov     Fixed.State,ParamSet    ; Next state is ParamSet
        Call    FDParam
	LJC	FixedExecute		; No interrupt expected if error
	call	Set_Timeout_Timer	;
	ret

;*	Fixed state SetParam
;
;	Get the status byte off the status register and examine it.
;	If it is Error, then next state is Error
;	Else next state is Calc

FxExParamSet:
	call	FDGetStat		; Get status of controller
	test	al,FX_ERROR
	jz	FxEx0
	mov	Fixed.State,sError	; Next state is Error
	mov	Fixed.DCB,al		; Save status in DCB
        jmp     FxEx1_5

FxEx0:
        mov     Fixed.State,FxStartRead
        test    Fixed.Flags,fRead
        jnz     FxEx1
        mov     Fixed.State,FxStartWrite
        test    Fixed.Flags,fWrite
        jnz     FxEx1
Do_Verify:
        mov     Fixed.State,FxStartVerify
FxEx1:
; Set up command packet to be sent to disk controller.
; Write pre-compensation cylinder has already been set up by SetParam.
        mov     ax,Fixed.NumSectors
        mov     Fixed.DCB+1,al          ; Number of sectors
        mov     al,Fixed.Sector
        inc     al                      ; Sector number is 1-based
        mov     Fixed.DCB+2,al          ; Sector number
        mov     ax,Fixed.Cyl
        mov     Fixed.DCB+3,al          ; Cylinder (low)
        mov     Fixed.DCB+4,ah          ; Cylinder (high)
        xor     ah,ah
        mov     al,Fixed.PhysDrv
        and     al,1H                   ; Only need low bit
        shl     ax,4
        or      al,0A0H
        or      al,Fixed.Head
        mov     Fixed.DCB+5,al          ; Drive/Head combination
FxEx1_5:
;	Trace I/O if our event is active
;
	public	TraceIt
TraceIt:
	SaveReg <es,cx,bx>
	mov	ax,SysInfoSeg	  ; See if our major event is active
	mov	es,ax
	TRACEDD Trace_Major
	jz	FxEx1_NoTrc	  ; No, continue without trace

;Figure out minor code
	mov	si,Offset Fixed
	test	Fixed.Flags,fREAD ; Read?
	jz	FxEx1_NotRead
	mov	cx,TRACE_READ	  ; Its a read
	jmp	short FxEx1_DoTrc
FxEx1_NotRead:
	test	Fixed.Flags,fWRITE ; Write? (could be write verfiy)
	jz	FxEx1_NoTrc
	test	Fixed.Flags,fVERIFY ; Write verify?
	jz	FxEx1_Wrt
	mov	cx,TRACE_WRITEV     ; Its a write verify
	jmp	short FxEx1_DoTrc
FxEx1_Wrt:
	mov	cx,TRACE_WRITE	    ; Its a plain write

FxEx1_DoTrc:
; Fill in the trace data area
	les	bx,Fixed.Queue

	mov	al,Fixed.PhysDrv       ; Physical drive id
	mov	byte ptr Trace_Phys_id,al

	mov	ax,word ptr es:[bx].IOPhysRBA	 ; RBA
	mov	word ptr Trace_RBA,ax
	mov	ax,word ptr es:[bx].IOPhysRBA+2
	mov	word ptr Trace_RBA+2,ax

	mov	ax,Fixed.Count	       ; Sectors to transfer
	mov	Trace_Sectors,ax

	mov	ax,word ptr es:[bx].IOpData ;Data buffer
	mov	word ptr Trace_Buffer,ax
	mov	ax,word ptr es:[BX].IOpData+2
	mov	word ptr Trace_Buffer+2,ax

; Do the Dev Help
	mov	Trace_Minor,cx	    ; Remember minor code & pass to DevHlp
	mov	ax,TRACE_MAJOR	    ;	Pass Major code
	mov	bl,TRACE_SIZE	    ;	Pass data length
	mov	si,offset Trace_Area ;	Pass data area address
	DEVHLP DevHlp_RAS
	mov	Trace_Cmplt_f,1     ; Say completion event needed

FxEx1_NoTrc:
	RestoreReg <bx,cx,es>
	jmp	FixedExecute


; *	Fixed state Write
;
; Entered on interrupt from the controller after the first 512 bytes of the
; operation have been written out to the disk, and on subsequent Writes.
; We ensure that no error occurred in the previous operation and if more
; data needs to be written out.
; If there is an error, or we are done, we go to the next state (Error, Verify
; or Done) as no more interrupts will come our way.
; If more data needs to be written out, we write out the next 512 bytes to the
; data port, and return i.e. "wait" for an interrupt.

FxExWrite:
	call	FDGetStat		; Get status byte
	test	al,FX_ERROR
	jz	FxEx2
	mov	Fixed.State,sError	; Next state is Error
	mov	Fixed.DCB,al		; Save status in DCB
	jmp	FixedExecute

; It is OK to write out the next 512 bytes to the controller
FxEx2:
; Now adjust the appropriate counters and pointers from the previous WRITE
	mov	ax,512			; number of bytes written out
	add	word ptr Fixed.CurrAddr,ax
	adc	word ptr Fixed.CurrAddr+2,0	; handle any carry
	dec	Fixed.SecToGo		; More sectors to write out?
	jz	FxEx3

	mov	InterruptState, 1	; expect an interrupt
	call	FDWrite 		; Write out next 512 bytes
	call	Set_Timeout_Timer
	ret				; "Wait" for next interrupt

 ; Now check to see if the next state is Done or Verify
FxEx3:
	mov	Fixed.State,Done
	test	Fixed.Flags,FVerify
	ljnz	 Do_Verify
	jmp	FixedExecute

;* Fixed state StartWrite
;
; The controller issues an interrupt when it is done writing out the buffer
; to the disk. We write out 512 bytes to the data port, once we get a DRQ
; status, and then return i.e. "wait" for an interrupt.
;
FxExStartWrite:
        mov     Fixed.State,Write
        mov     al,FX_CWRITE+FX_NORETRY
        mov     Fixed.DCB+6,al          ; Command
        call    SendCmdPkt              ; Send off command to controller
        JC      Write_Err               ; No interrupt expected if error
        call    FDWrite                 ; Write out 512 bytes to controller
        ret                             ; "Wait" for interrupt

Write_Err:
        MOV     Fixed.State,sError
        JMP     FixedExecute


;* Fixed state StartRead
;
; The controller issues an interrupt when the buffer is ready to be read by
; the system. We issue the Read command to the controller and then return.
; i.e. "Wait" for the interrupt.
;
FxExStartRead:
        mov     Fixed.State,Read
        mov     al,FX_CREAD+FX_NORETRY
        mov     Fixed.DCB+6,al          ; Command
        Call    SendCmdPkt              ; Send off command packet to controller
	LJC	FixedExecute		; No interrupt expected if error
	call	Set_Timeout_Timer
	ret				; "Wait" for interrupt

; *	Fixed state read
;
; Entered on interrupt from the controller when it has performed the read
; operation and has 512 bytes to be read by the system.
; We ensure that no error occurred in the previous operation.
; We read the 512 bytes of data from the port in any case, since the
; controller reads the sector even if there was an error.
; If there was an error, or we are done, we go to the next state as no more
; interrupts will come our way.
; If we have more data to read, we return i.e. "wait" for an interrupt.

FxExRead:
	call	FDGetStat		    ; Get status of controller in AL
	SaveReg <es>
	SaveReg <ax>			    ; Preserve status
	mov	ax,WORD PTR Fixed.CurrAddr+2; get pointer to data location
	mov	bx,WORD PTR Fixed.CurrAddr  ; AX:BX is 32-bit address of data
	mov	dx,FX_PDAT

	mov	cx,512			    ; byte count
	call	PhysToVirtESDI

; *** WARNING ***
; We are now going to read in the data from the data port. We do not want to
; be interrupted since we would lose data, so we disable interrupts and
; re-enable them when we are done.

	mov	cx,256			    ; read in 256 words
	cld
	DISABLE
	REPE	INSW
	RestoreReg <ax> 		    ; Restore Status
	test	al,FX_ERROR
	JZ	FxEx5
	mov	Fixed.State,sError
	mov	Fixed.DCB,al		    ; Save status in DCB
	ENABLE				    ; OK to take interrupts now
	call	UN_PhysToVirt
	RestoreReg <es>
	jmp	FixedExecute

FxEx5:
; Adjust the pointers and counts appropriately
;
; We need to be careful about enabling interrupts before ALL the fields in
; our data structures have been updated. If we get a spurious or a nested
; interrupt, we have to ensure that we will get to the correct state.
; We need to be especially careful since calling Un_PhysToVirt or POP ES
; may take "a long time".
;
	mov	cx,512
	add	word ptr Fixed.CurrAddr,cx
	adc	word ptr Fixed.CurrAddr+2,0	; handle any carry
	mov	InterruptState, 1		; assumn not finished
	dec	Fixed.SecToGo
	jnz	FxEx5_5
        mov     al,FxCacheRead              ; Assume we are going through the cache
	test	Fixed.CacheFlg,fCach_Active
	jnz	FxEx5_3 		    ; We are caching data
        mov     al,Done                     ; Next state is done. We are not going through the cache
	mov	InterruptState, 0	    ; wrong we're finished
FxEx5_3:
        mov     Fixed.State,al              ; Go to next state
	jmp	short FxEx5_8
FxEx5_5:
	ENABLE					; OK to take interrupts now
	call	UN_PhysToVirt
	RestoreReg <es>
	call	Set_Timeout_Timer
	ret
FxEx5_8:
	ENABLE					; OK to take interrupts now
	call	UN_PhysToVirt
	RestoreReg <es>
	jmp	FixedExecute

;* Fixed state StartVerify
;
; Verify is like a Read except that no data is returned. An interrupt is issued
; when the operation is complete or if there is an error.
FxExStartVerify:
        mov     Fixed.State,Verify
        mov     Fixed.DCB+6,FX_Verify+FX_NoRetry
        Call    SendCmdPkt
        LJC     FixedExecute            ; No interrupt expected if error
        RET                             ; "Wait" for interrupt


;** Fixed state Verify
;
;       Have executed a write function, must now verify.

FxExVerify:
        mov     Fixed.State,Done            ; Assume no error.
        call    FDGetStat
        test    al,FX_ERROR
        jz      FxEx6
        mov     Fixed.State,sError
        mov     Fixed.DCB,al                ; Save controller status
FxEx6:
        jmp    FixedExecute


; *     Fixed state CacheRead
;
; Entered when we have a buffer in the cache that contains data that the
; driver has been requested to read off the disk. We go through the cache
; even if the data does not span the whole buffer.
;
; WARNING!!!!
;
;	Care must be taken to ensure that DS or ES are not put on the stack
;	after they have been mapped to a buffer since if this buffer is in
;	high memory and we are in real mode, the LOADALL mapping will be lost.
;
FxExCacheRead:
	mov	Fixed.State,Done
	mov	al,Fixed.SectorSize
	call	SectorIndexToSectorSize
	xor	ch,ch
	mov	cl,Fixed.XferSec
	mul	cx
	xchg	ax,cx			; CX = offset into buffer
	mov	bx,word ptr Fixed.XferAddr
	mov	ax,word ptr Fixed.XferAddr+2
	add	bx,cx
	adc	ax,0			; AX:BX = address into buffer for data
	mov	cx,Fixed.NumBytes
	push	ds
	pop	es			; es=BiosGroup
; Since this is the address of the cache, we use a special descriptor
	mov	si,SELECTOR2		; Special selector 2
	mov	dh,RESULTDSSI		; Result to go in ds:si
	mov	dl,DevHlp_PhystoVirt
	Call	[DevHlpFunc]		; DS:SI -> xfer address
ASSUME	ES:BiosGroup, DS:NOTHING
	push	es			; save BiosGroup selector
	mov	bx,word ptr Fixed.Addr
	mov	ax,word ptr Fixed.Addr+2
	mov	cx,Fixed.NumBytes
	SaveReg <cx,si> 		; Save transfer length and buffer ptr
	mov	si,-1			; Get pointer to user buffer
	mov	dh,RESULTESDI		; Result to go in es:di
	mov	dl,DevHlp_PhystoVirt
	Call	[DevHlpFunc]		; ES:DI -> xfer address
	RestoreReg <si,cx>		; Restore buffer ptr and transfer length
	cld
	clc
	rcr	cx,1			; Convert to word count
	rep	movsw
	jnc	fecr2			; Transfer is done
	movsb
fecr2:
	pop	ds			; DS = BiosGroup selector
ASSUME	DS:BiosGroup
	mov	dl,DevHlp_UNPhystoVirt
	Call	[DevHlpFunc]		; 'Switch back' to mode of entry
	jmp	FixedExecute


;*	Fixed state Done
;
;  If whole request is now complete, mark the request as done and then start
;  the next one if there is one.  If the request is not yet done, adjust
;  values to show the amount of the request done and then go back to the
;  Calc state to do next part.

FxExDone:
  mov  ax,Fixed.XferNumSec              ; ax = # of sectors we did
  sub  Fixed.Count,ax			; Adjust count to number left
  .IF nz				; Request is done, tell DOS
     add word ptr Fixed.First,ax	; Advance sector number
     adc word ptr Fixed.First+2,0	;   ...
     mov ax,Fixed.NumBytes		; Number of bytes handled
     add word ptr Fixed.RealAddr,ax	; Advance data address
     adc word ptr Fixed.RealAddr+2,0	;
     mov Fixed.State,Calc		;
;
; Reset the error flag and the InvalidBuf bit since this I/O went through fine
;
     and Fixed.CacheFlg,NOT (fCach_Reset+fCach_InvalidBuf)
     jmp FixedExecute			;
  .ENDIF				;

FxEx7:					; other code comes here
  mov  di,DataOffset Fixed		;
;
;	Trace I/O completion event
;
     public  TraceItD
TraceItD:
  .IF  <Trace_Cmplt_f eq 1> ; completion event needed
     mov     Trace_Cmplt_f,0
     SaveReg <es,cx,bx>

     les     bx,Fixed.Queue	 ;   Fill in completion status
     mov     ax,es:[bx].PktStatus
     and     ax,STECODE
     mov     Trace_Cmplt_Stat,ax

     mov     cx,Trace_Minor	 ;   Pass completion minor to DevHlp
     or      cl,TRACE_CMPLT
     mov     ax,TRACE_MAJOR	 ;   Pass Major code
     mov     bl,TRACE_SIZE_CMPLT ;   Pass data length
     mov     si,offset Trace_Area ;  Pass data area address
     DEVHLP  DevHlp_RAS

     RestoreReg <bx,cx,es>
  .ENDIF

  mov  ds:TimerCallFlag, 0
  call Cancel_F2Timer			; when done with request cancel timer
  call DoneRequest			; unblock and redrive
  call FxDisabInts			; disable fixed disk interrupts
  jmp  FixedExecute			;

;** Fixed state Idle
;
;	Nothing happening, become inactive.

FxExIdle:
  ret

;** Fixed state Error
;
;	Entered when a non-recoverable error is detected.
;	A sense block has been requested and put into the
;	DCB.

FxExError:
        MOV     DI,DataOffset Fixed     ; SI = pointer to per-device info.
        MOV     [DI].State,Calc         ; Retry the operation
	test	Fixed.DCB,40H		; Not Ready Error
	jz	FixedFails
        cmp     [DI].ErrCnt,ErrLim      ; Reach error limit?
        JAE     FxExErr2                ; We have reached the error limit
	TEST	[DI].CacheFlg,fCach_Active
	jz	FxErr2			; Do not try to free any buffer in cache
        CALL    FreeCacheBuf            ; Mark this buffer as free (DI destroyed)
FxErr2:
        inc     Fixed.ErrCnt            ; We are doing another try
;
;	Trace I/O completion event; Failure, before retry
;
     public  TraceItE
TraceItE:
	.IF  <Trace_Cmplt_f eq 1> ; completion event needed
	   mov	   Trace_Cmplt_f,0
	   SaveReg <cx,bx>

	   mov	   ah,Fixed.DCB        ;   Fill in completion status
	   mov	   dx,FX_PERR	       ;   (DCB1 and Error status)
	   in	   al,dx
	   mov	   Trace_Cmplt_Stat,ax

	   mov	   cx,Trace_Minor      ;   Pass completion minor to DevHlp
	   or	   cl,TRACE_CMPLT
	   mov	   ax,TRACE_MAJOR      ;   Pass Major code
	   mov	   bl,TRACE_SIZE_CMPLT ;   Pass data length
	   mov	   si,offset Trace_Area ;  Pass data area address
	   DEVHLP  DevHlp_RAS

	   RestoreReg <bx,cx>
	.ENDIF

	jmp	FixedExecute		; Back to state machine loop

;
; We check to see if we are caching The I/O. If so, then the error may be due
; to a bad sector on the disk. We Retry the operation without caching to see
; if the error is due to a bad sector that has been noted in the FAT. If it is
; not, we will catch it the second time around.
FxExErr2:
	TEST	[DI].CacheFlg,fCach_Active
	jz	FixedFails		; Fail operation. We were not caching anything
	OR	[DI].CacheFlg,fCach_Reset   ; Flag to try I/O without caching
        CALL    FreeCacheBuf            ; Mark this buffer as free (DI destroyed)
        mov     Fixed.ErrCnt,0          ; Zero out the error count for another retry
        JMP     FixedExecute            ; Back to retry the operation

FixedFails:
	mov	[DI].State,Done 	; Request is done.
	mov	dx,FX_PERR		; Get error code from error port
	in	al,DX
	mov	ah,[DI].DCB		; Get status byte that was saved earlier
; Examine 'Not Ready' error first
	mov	bl,Error_I24_Not_Ready
	test	ah,40H
	jz	Error_Found
; Check for Write fault
	mov	bl,Error_I24_Write_Fault
	test	ah,20H
	jnz	Error_Found
; Map the error code returned by the controller onto a DOS error code
	call	MapHardError
Error_Found:
; BL contains error code to be returned to DOS
	push	ES
	les	DI,Fixed.Queue	       ; Get ptr to request
	mov	al,bl
	ERRNZ	<((STERR+STDON) AND 255)>; Bits must be in high byte
	mov	ah,(STERR+STDON)/256
	mov	es:[di].PktStatus,ax	; Set error and code
	pop	es
	jmp	FxEx7		; Advance to Done state

FixedExecute endp

	BREAK	<Routines that interface to hard disk controller>

;*** MapHardError - Map error code to a DOS error code
;
;   MapHerdError maps the error code returned by the fixed disk controller
;   onto a DOS error code for return to the DOS.
;
;   ENTRY   al - Byte from the controller Error Register
;
;   EXIT    bl - DOS error code
;
;   ASSUMPTION - An error HAS occurred. The mapping is done via a look-up
;		 table.
;
;   MODIFIED - ax,bx

	DebugPublic MapHardError
MapHardError proc

	SaveReg <cx>
	mov	cx,7		    ; Examine all bits in AL
MHE1:
	shl	al,1
	jc	MHE2
	loop	MHE1

; CX contains the index into Hard_Err_Table corresponding to the error.
; Use it to get the correct code from the table.
MHE2:
	mov	bx,DataOffset Hard_Err_Table
	add	bx,cx
	mov	bl,BYTE PTR [bx]
	RestoreReg <cx>
	ret
MapHardError	endp

;** FDGetStat - Get status of controller
;
;	FDGetStat - Get the status of the fixed disk controller in AL.
;
;	ENTRY	NONE
;
;	EXIT	al contains the byte from the status port

	DebugPublic FDGetStat
FDGetStat   proc

	push	dx
	mov	dx,FX_PSTAT
	in	al,dx
	test   al, 020H 	     ;
	.IF  nz 		     ; if write fault then set error
	   or	al, 041H	     ; and retry bits
	.ENDIF			     ;
	pop	dx
	ret

FDGetStat   endp

;** FDWrite - Write out 512 bytes to controller
;
;	FDWrite - Write out 512 bytes out to the fixed disk controller.
;
;	ENTRY	Fixed set up for performing the write to the drive.
;		ds = BiosGroup
;
;	EXIT	Data read in at location in Fixed.CurrAddr.
;		ALL registers preserved.
;
;	ALGORITHM
;		Wait for the controller to issue a 'Data Request' (DRQ)
;		before sending off the data.
;		Move the pointer to the data to the next 512 bytes.

	DebugPublic FDWrite
FDWrite proc

	SaveReg <ax,cx,dx,si>
	mov	dx,FX_PSTAT
WAIT_DRQ:
	in	al,dx
	test	al,FX_DRQ
	jz	WAIT_DRQ
	SaveReg  <ds>
	push	ds			; set up es to point to our data segment
	pop	es			; set up es to point to our data segment
	mov	ax,WORD PTR Fixed.CurrAddr+2; Get pointer to data to be transferred
	mov	bx,WORD PTR Fixed.CurrAddr  ; ax:bx is 32-bit pointer to data
	mov	dx,FX_PDAT
	mov	cx,512			; byte count
	call	PhysToVirtDSSI

; We are now going to write out the data to the data port. We do not want to
; be interrupted since we would lose data, so we disable interrupts and
; re-enable them when we are done.

	mov	cx,256			    ; transfer 256 words
	cld
	DISABLE
	REPE	OUTSW
	call	UN_PhysToVirtES 	; this is ok since we know that ES
	RestoreReg  <ds>		; points to our data segment!!
	ENABLE
  RestoreReg <si,dx,cx,ax>
  ret

FDWrite     endp

;** FDParam - Set up fixed disk controller for I/O
;
;	FDParam - Set up the controller for the I/O
;
;	Entry	  DS:DI point to drive parameters
;		  DS = BiosGroup
;
;	Exit	  Fixed disk controller knows what type of drive is in system.
;		  ALL registers preserved.
;	Algorithm
;		  FDParam initially sets up the controller for the I/O. It
;		  checks the status and then sends it a packet telling it what
;		  type of device it is talking to.

	DebugPublic FDParam
FDParam proc

	pusha
	mov	dx,FX_RFDR
	mov	al,8H
	cmp	[DI].BPBnHead,8
	jae	FDP1
	mov	al,0
FDP1:
	out	dx,al		    ; Tell controller number of heads we have

; Set up command packet to be sent to disk controller
	mov	ax,[DI].WrtPrCmp
	shr	ax,2			; divide starting cylinder by 4
	mov	Fixed.DCB,al		; Write precompensatiion
	mov	ax,[DI].BPBtrksiz
	mov	Fixed.DCB+1,al		; Number of sectors
	xor	al,al
	mov	Fixed.DCB+2,al		; Sector Zero
	mov	Fixed.DCB+3,al		; Cylinder (low) - zero
	mov	Fixed.DCB+4,al		; Cylinder (high) - zero
	xor	ah,ah
	mov	al,Fixed.PhysDrv
	and	al,1			; Only need low bit
	shl	ax,4
	or	al,0a0h
	mov	cx,[di].BPBnHead
	dec	cl			; Head must be 0-based
	or	al,cl
	mov	Fixed.DCB+5,al		; Drive and Head
	mov	al,FX_SETP
	mov	Fixed.DCB+6,al
	call	SendCmdPkt		; Send off command to controller
	popa
	ret

FDParam  endp

;** FDCommand - Send off command to fixed disk controller.
;
;	FDCommand will send the previously set up command packet to the fixed
;	disk controller. It first ensures that the controller is ready to
;	accept the data.
;
;	Entry	  DCB fields in Fixed set up for sending off command.
;
;	Exit	  ALL registers preserved.

	DebugPublic FDCommand
FDCommand   proc

	pusha
	mov	dx,FX_PDRHD
	mov	al,Fixed.DCB+5
	out	dx,al			; select the drive

	call	WaitRdy 		; wait for drive to be ready
	jc	FDCmd1			; We got a timeout error

	mov	dx,FX_PWRP
	mov	si,DataOffset Fixed.DCB
; Send out 7 bytes to controller
	mov	cx,6			; 6 bytes to be written out
	cld
Loop_Send:
	lodsb
	out	dx,al
	inc	dx
	loop	Loop_Send
; Wait till controller ready for command
	call	WaitRdy
	jc	FDCmd1			; We got a timeout error
	lodsb				; Send out command to controller
	mov	InterruptState, 1	; indicate we will expect an interrupt
	mov	ds:TimerInt, 0		; let interrupt come in if we are
					; restarting the command from the
					; (F2Timer) timeout routine.
	out	dx,al			; enable controler with last command
					; byte.
FDCmd1:
	popa
	ret

FDCommand endp

;**	WaitRdy - Wait for fixed disk controller to become ready.
;
;	WaitRdy waits for the drive controller to become ready. It examines
;	the high bit that is read from the status register.
;
;	Entry	  NONE
;
;	Exit	  Carry set if timeout.
;		  Carry clear if controller ready.

	DebugPublic WaitRdy
WaitRdy proc

	push	dx
	mov	dx,FX_PSTAT
	sub	cx,cx
	mov	bl,40H			; outer loop count
Loop_Rdy:
	in	al,dx
	test	al,FX_BUSY
	clc
	jz	WR1
	loop	Loop_Rdy
	dec	bl
	jnz	Loop_Rdy
	stc
WR1:
	pop	dx
	ret

WaitRdy endp

	BREAK <SendCmdPkt>
;**	SendCmdPkt - Send off command packet to controller
;
;	SendCmdPkt waits until the drive controller is ready, enables the
;	fixed disk interrupts, and sends off the command packet to the
;	controller.
;
;	Entry	  NONE
;
;	Exit	  NONE

	DebugPublic SendCmdPkt
SendCmdPkt proc

	call	WaitRdy 	    ; Wait for controller to be ready
	jc	SCP1
	call	FxEnabInts	    ; Enable fixed disk interrupts
	call	FDCommand	    ; Send off command packet to controller
	jnc	SCP2

; We got an error from FDCommand - Assume it was a 'Not Ready' error

SCP1:
	mov	Fixed.State,sError
	mov	Fixed.DCB,FX_ERROR	; Set status to error

SCP2:
  ret

SendCmdPkt endp

	BREAK <FxEnabInts and FxDisaInts>
;** FxEnabInts - Enable fixed disk interrupts
;
;	FxEnabInts reprograms the two 8259s on the AT to enable fixed disk
;	interrupts.
;
;	Entry	  NONE
;
;	Exit	  NONE

	DebugPublic FxEnabInts
FxEnabInts proc

  push	  ax
  DISABLE
  in	  al,INT_CONT2+1	  ; Program the second chip
  and	  al,NOT 40H		  ; Reset bit 6
  IOWait
  out	  INT_CONT2+1,al
  IOWait
  in	  al,INT_CONT+1 	  ; Program the first chip
  and	  al,NOT 04H		  ; Reset bit 2
  IOWait
  out	  INT_CONT+1,al
  ENABLE
  pop	  ax
  ret

FxEnabInts endp

;**  FxDisabInts - Disable fixed disk interrupts
;
;	FxDisabInts reprograms the two 8259s on the AT to disable fixed disk
;	interrupts.
;
;	Entry	  must be disabled
;
;	Exit	  NONE

	DebugPublic FxDisabInts
FxDisabInts proc

  push	  ax			  ; must be called disabled
  DISABLE
  in	  al,INT_CONT2+1	  ;
  or	  al,40H		  ; Mask off IRQ 14.
  IOWait			  ;
  out	  INT_CONT2+1,AL	  ;
  ENABLE
  pop	  ax			  ;
  ret				  ;

FxDisabInts endp


;** F2Timeout	 - Time out occurred
;
;   F2Timeout is called by timer services indicating the an interrupt did
;   not occur within the specified time period.  This will restart the
;   command using the retry logic.
;
;   ENTRY CS and DS set by timer services
;
;   EXIT  DriveStruc updated
;
;   USES  flags

	public F2Timeout
F2Timeout  proc

	dec	TimerCallFlag		; occurred before interrupt!!!
	SaveReg <di,cx> 		; active drive had a time out so
	call	FDGetStat		; get status and
	mov	Fixed.DCB,al		; Save status in DCB
	mov  Fixed.State,sError 	;   it's an error
	call FixedExecute		;   call state machine
	RestoreReg <cx,di>		;   ...
	ret
F2Timeout  endp


;** Set_Timeout_Timer	- Set timeout timer in case interrupt does not
;			  occur.
;   ENTRY	none
;
;   EXIT
;
;   USES  flags
Set_Timeout_Timer	proc	near
	DISABLE 		     ; This is to check to see if the Interrupt
	inc	TimerCallFlag	     ; occurred before setting the timer!!!
	cmp	TimerCallFlag, 0     ; If so then we don't need to call timer
				     ; services to turn off the timer!
	jle	no_set_timer	     ; jmp if the destination is less then the source (signed)
	mov	F2Count, 5	     ; (7 x 35H x 1FH) = 11.5 sec decimal
	mov	F2Timer_Active, 1
no_set_timer:
	ENABLE
	ret
Set_Timeout_Timer	endp



       comment !
****************** Start of Specifications ****************************
*								      *
* Subroutine Name: Fixed_Init					      *
*								      *
* Descriptine Name: Fixed disk Initialzation routine		      *
*								      *
* Function:							      *
*	     This routine is invoked in DriveInit routine in the      *
*	     process of INIT command.				      *
*	     In this routine, enable Fixed Disk controller, and       *
*	     set up actual and recommended BPB for physical	      *
*	     fixed disks.					      *
*								      *
*	     If a fixed disk exists, link Drive-B BDS chanin to       *
*	     Drive-C BDS chain. And if the secondo fixed disk	      *
*	     exists, link Drive-C BDS chain to Drive-D. 	      *
*	     Terminate BDS chain by setting -1 to anchor pointer.     *
*								      *
* Notes:							      *
*								      *
* Entry Point: Fixed_Init					      *
*								      *
* Linkage:  near call from DriveInit				      *
*	    DS	  - BiosGroup					      *
*								      *
* Input: pointer to initialization parameters			      *
*								      *
* Output:							      *
*								      *
* Exit-Normal: near ret to DriveInit with 'C' flag off                *
*								      *
* Exit-Error:  near ret to DriveInit with 'C' flag set                *
*								      *
************************ End of Specifications ************************ !
      BREAK <Fixed_Init>

CodeInit    segment	    ;

	public Fixed_Init
Fixed_Init proc
  pusha 				  ;
  SaveReg  <es,bx>

  mov bx,IRQ_Fixed			  ; install the interrupt handler
  mov ax,CodeOffset FixedInterrupt	  ;   ...
  xor dh, dh				  ; (we will share this interrupt
					  ; level (0))
  DEVHLP DevHlp_SetIRQ			  ; sets carry on error
  .IF nc				  ; if 'C' not set,
    call AllocBDSChain			  ;   reallocate BDS entries
    call Setup_Partition_BDSs		  ;   build BDSs for partitions
  .ENDIF				  ;

 ; set up BDS for physical hard disk BDSs (80h,81h)
 ;
  push	ds				; make sure ds point to DDD data area
  pop	es
  mov di, Drive80			; ds:di -> 80h disk BDS
  .WHILE <[di].Link ne -1>		;
    SaveReg di				; save BDS pointer
    lea  si,[di].BPBsecsiz		; ds:si -> BPB for hard file
    lea  di,[di].BPBRsecsiz		; es:di -> RecBPB
    mov  cx,(size a_BPB) - 12		; # bytes to move
    repe movsb				; copy recommend BPB to media
    RestoreReg di			; restore BDS pointer
    mov di,[di].Link			; point to next BDS
  .ENDWHILE				;

  RestoreReg  <bx,es>                   ; Get pointer to INIT packet

; Get pointer to Cache data area:       ;
  SaveReg <es>
  les     bx, es:[bx].InitpBPB
  mov     bx, es:[bx]
  call    Cache_Init
  RestoreReg  es

  popa					;
  ret					;
Fixed_Init endp

CodeInit ends
Code	 ends
	 end
