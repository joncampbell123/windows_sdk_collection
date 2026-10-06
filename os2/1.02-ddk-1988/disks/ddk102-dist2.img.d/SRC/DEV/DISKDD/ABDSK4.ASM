;
; Copyright (c) Microsoft Corporation,  1988
;
	page	,132
	title	ABDISK4 - MS OS/2 timer services
	name	ABDISK4
	comment !
********************* Start of Specifications *************************
*								      *
*  Source File Name: ABDSK4.ASM 				      *
*								      *
*  Descriptive Name: Timer Services				      *
*								      *
*  Function: Provides timer services routines for the floppy and      *
*	     hard disk state machines.				      *
*								      *
*  Entry Points:  DriveTimer, FTimer, FloppyTimer, SetTimer2	      *
*		  Set_FloppyTimer, F2Timer, Set_FTimerU, Set_FTimerS, *
*		  Set_F2Timer, Cancel_f2timer, Cancel_FloppyTimer,    *
*		  SetTimer1, Set_FloppyTimerU, Set_FloppyTimerS,      *
*		  Cancel_FTimer, and TimerInit. 		      *
*								      *
*  External References:  See EXTRN statements below		      *
*								      *
********************** End of Specifications ************************** !

	.286p

	include abdebug.inc		; debug and listing control

	include absegs.inc		; code and data segment definitions

	BREAK <External code entry points>
Code	segment

; Routines located in ABDSK2.ASM /ATDSK2.ASM
	extrn	DriveExecute:near
	extrn	TimeoutHandler:near
; Routines located in ABDSK3.ASM /ATDSK2.ASM
	extrn	FTimeout:near

; Routines located in ATDSK3.ASM
	extrn	F2Timeout:near

; Routines located in ABSUBR.ASM
	extrn	Get_BDS_Addr:near

Code	ends

	BREAK <External data references>
BioData segment

; Data located in ABDSK1.ASM
	extrn	Start_BDS:word
	extrn	DevHlpFunc:dword

; Data located in ABDSK2.ASM / ATDSK2.asm
	extrn	Drive:byte
	extrn	MotorOff:near

; Data located in ABINT13.ASM
	extrn	SemInt13:byte

; Data located in ABDSK3.ASM or ATDSK3.ASM
	extrn	ATMCH:byte

BioData ends

	.xlist				;
	include struc.inc		; structured macros
if1
	include abmac.inc		; private macros
endif
	include devhlp.inc		; Device helper definitions.
	include abdskequ.inc		; general equates
	include abddrv.inc		; IOStruc
	include infoseg.inc		; for the clock interval

	ListLevel %ablistlevel,%abdebug
	.xall

	BREAK	<Data Segment>
BioData segment

	public	TimerActive,TimerConv, MOffDelay, UTick, MTick
TimerConv	dw	?	; conversion factor for ms => ticks
UTick		dw	?
MTick		dw	?
TimerActive	db	0	; bit flags for active timers
MOffDelay	db	7	; delay count for motor
				; (Timer2Count)x(MOffDelay)x(31ms)
				;    = motor off time
FloppyTimer_Active db	0	; variable to indicate whether
				; or not the timer is active
FTimer_Active db    0		; variable to indicate whether
				; or not the timer is active

	public	F2Timer_Active, F2Count, TimerInt
F2Timer_Active	db	0	; for fixed disk timeouts
F2Count 	dw	5
TimerInt	dw	0	; indicates whether or not the disk interrupt
				; interrupted the timer interrupt

BioData ends

	BREAK	<Code Segment>
Code	segment

	assume	cs:BiosSeg,ds:BiosGroup,es:nothing

	BREAK <DriveTimer>
	comment !
************************ Start of Specifications *********************
*								     *
* Subroutine Name: DriveTimer					     *
*								     *
* Descriptine Name: motor timer interrupt handler		     *
*								     *
* Function:							     *
*	DriveTimer is called periodically to perform time	     *
*	related services for the floppy driver.  There is	     *
*	one service performed; a motor turn off service 	     *
*	when a drive is not active.				     *
*								     *
* Notes:							     *
*								     *
* Entry Point: DriveTimer					     *
*								     *
* Linkage:  near call						     *
*	    DS	  - BiosGroup					     *
*								     *
*  It's assumed that all registers have been saved by the            *
*  caller. (Via the DevHlp_NestedInt call)			     *
*								     *
* Input: DS = data segment					     *
*								     *
* Exit-Normal:	FAR RET 					     *
* Exit-Error:	FAR RET 					     *
* Effects:	none						     *
* Internal References:						     *
*								     *
* External References:						     *
*								     *
************************* End of Specifications ********************** !

	public DriveTimer
DriveTimer proc far

  .IF <TimerActive ne 0> and		; any timers active?
  .IF <SemInt13    e  0>		; An INT 13 in progress?
     call    MotorOff
  .ENDIF
  ret

DriveTimer endp

	BREAK <FloppyTimer>
	comment !
************************ Start of Specifications *********************
*								     *
* Subroutine Name: FloppyTimer					     *
*								     *
* Descriptine Name: one shot timer for floppy disk		     *
*								     *
* Function:							     *
*	FloppyTimer provides a one shot timer for the floppy	     *
*	disk to use.						     *
*								     *
* Notes: This routine is called only when Set_FloppyTimer or	     *
*	 SetTimer1 have been called.				     *
*								     *
* Entry Point: FloppyTimer					     *
*								     *
* Linkage:  near call						     *
*	    DS	  - BiosGroup					     *
*								     *
* Input: DS = data segment					     *
*								     *
* Exit-Normal:	FAR RET 					     *
* Exit-Error:	FAR RET 					     *
* Effects:	none						     *
* Internal References:						     *
*								     *
* External References:						     *
*								     *
************************* End of Specifications ********************** !

	     Public FloppyTimer
FloppyTimer proc far

     DISABLE
     .IF  <FloppyTimer_Active eq 1>
	mov	FloppyTimer_Active, 0
	call	TimeoutHandler
	ENABLE
	pusha				   ; the timer has expired
	SaveReg es			   ;   ...
	mov	bx, 0ffffH
	mov	ax, CodeOffset FloppyTimer ; discontinue timer service
	DEVHLP	DevHlp_TickCount	   ; routine
	call	DriveExecute		   ; call diskette state machine
	RestoreReg es			   ;   ...
	popa				   ;   ...
     .ENDIF
	ENABLE
	ret				   ;   ...
FloppyTimer endp

	BREAK <FTimer>
	comment !
************************ Start of Specifications *********************
*								     *
* Subroutine Name: FTimer					     *
*								     *
* Descriptine Name: one shot timer for fixed disk (NON-AT)	     *
*		    and Floppy disk (AT)			     *
*								     *
* Function:							     *
*	Fixed timer provides a one shot timer for the fixed	     *
*	and floppy  disk to use.				     *
*								     *
* Notes: This routine is called only when Set_FTimer is 	     *
*	 executed.						     *
*								     *
* Entry Point: FTimer						     *
*								     *
* Linkage:  near call						     *
*	    DS	  - BiosGroup					     *
*								     *
* Input: DS = data segment					     *
*								     *
* Exit-Normal:	FAR RET 					     *
* Exit-Error:	FAR RET 					     *
* Effects:	none						     *
* Internal References:						     *
*								     *
* External References:						     *
*								     *
************************* End of Specifications ********************** !

	     Public FTimer
FTimer proc far

     .IF  <FTimer_Active eq 1>
	mov	FTimer_Active, 0
	pusha					;
	SaveReg es				;
	mov	bx, 0ffffH
	mov	ax, CodeOffset FTimer		; discontinue timer service
	DEVHLP	DevHlp_TickCount		; routine
	call	FTimeout			;
	RestoreReg es				;
	popa					;
     .ENDIF
	ret					   ;

FTimer endp

	BREAK <F2Timer>
	comment !
************************ Start of Specifications *********************
*								     *
* Subroutine Name: F2Timer					     *
*								     *
* Descriptine Name: one shot timeout timer for fixed disk (AT)	     *
*								     *
* Function:							     *
*	There is a bug in the fixed disk controler portion of the    *
*	AT Dual card that presents a problem.  On certain cards      *
*	under stessfull condition the Western Digital controler      *
*	pull a command off the bus.  Two things could happen. For    *
*	one a spurrious interrupt could occur. Secondly the current  *
*	current multisector command could be suspended thus we	     *
*	could wait forever for an interrupt.  The latter is what     *
*	this timer is for.  If we are in a I/O operation expecting   *
*	an interrupt and it doesn't come this timer will restart     *
*	the I/O command again. We will call the state machine which  *
*	will get status creating an interrupt that we will ignor     *
*	and we we restart the command.				     *
*								     *
* Notes: This routine is called only when Set_F2Timer is	     *
*	 executed.						     *
*								     *
* Entry Point: F2Timer						     *
*								     *
* Linkage:  near call						     *
*	    DS	  - BiosGroup					     *
*								     *
* Input: DS = data segment					     *
*								     *
* Exit-Normal:	FAR RET 					     *
* Exit-Error:	FAR RET 					     *
* Effects:	none						     *
* Internal References:						     *
*								     *
* External References:						     *
*								     *
************************* End of Specifications ********************** !

	     Public F2Timer
F2Timer proc far

     DISABLE					;
     .IF  <F2Timer_Active eq 1> 		; is timer active
	dec	F2Count 			;
	jnz	retf2				;
						;
	mov	TimerInt, 1			; ignor interrupts
	mov	F2Timer_Active, 0		; inactivate timer
	pusha					;
	SaveReg es				;
	ENABLE					;
	call	F2Timeout			; call state machine
	mov	TimerInt, 0			; acknowledge interrputs
	RestoreReg es				;
	popa					;
     .ENDIF					;
retf2:
	ENABLE
	ret					;

F2Timer endp

	BREAK	<Set_Timer1, Set_FloppyTimer>
;***	Timing routines for floppy and fixed drives
;
;	  The following routines provide a means to "arm" the various
;	timers.

;***	Set_FloppyTimer - Arm one shot timer for floppy drive
;
;	Set_FloppyTimer will arm the FloppyTimer.  Input parameter
;	values in microseconds will be converted to timer
;	ticks.
;
;	ENTRY	DX:AX = delay value in microseconds
;
;	EXIT	AX = timer ticks
;		CF set if timer armed
;		CF clear if zero count passed
;
;	USES	AX, DX, and Flags

	public SetTimer1, Set_FloppyTimerU, Set_FloppyTimerS, Set_FloppyTimer
SetTimer1 proc near		  ; AT uses milleseconds
  xor dx,dx			  ; set up for divide
  .IF <ax eq 0> 		  ; zero count?
     clc			  ;
     ret			  ;
  .ENDIF			  ;
  push	MTick
  pop	TimerConv		  ; set up divide
  jmp	  short   setfloppy

Set_FloppyTimerU label near	  ; uses microseconds
Set_FloppyTimer  label near	  ; uses microseconds
  .IF <ax eq 0> and		  ; zero count?
  .IF <dx eq 0> 		  ; zero count?
     clc			  ;
     ret			  ;
  .ENDIF			  ;
  push	UTick
  pop	TimerConv		  ; set up divide
  jmp	  short   setfloppy

Set_FloppyTimerS label near	  ; uses seconds
  .IF <ax eq 0> 		  ; zero count?
     clc			  ;
     ret			  ;
  .ENDIF			  ;
  mov	TimerConv, THOUSAND	  ; set up multiply
  mul	TimerConv	; convert to seconds(AX) to milliseconds(DX:AX)
		    jnc     no_overflow1
		    overflow1: jmp short overflow1
		    no_overflow1:
  push	MTick
  pop	TimerConv		  ; set up divide

setfloppy:
  div	TimerConv	  ; convert to milliseconds(DX:AX) to ticks
			; the quotient in AX, remainder in DX
  .IF <dx ne 0> 		  ; remainder?
     inc ax			  ; yes, round up
  .ENDIF			  ;
  SaveReg  bx
  mov	bx, ax
  mov	ax, CodeOffset FloppyTimer  ; install timer service
DISABLE
  mov	  FloppyTimer_Active, 1
  DEVHLP  DevHlp_TickCount	    ; routine
ENABLE
  RestoreReg   bx

  stc				   ;
  ret				   ;

SetTimer1 endp

	BREAK <Cancel_FloppyTimer>
;***	Cancel_FloppyTimer - cancel one shot timer for floppy drive
;
;	Cancel_FloppyTimer will cancel the FloppyTimer.
;
;	ENTRY	none
;
;	EXIT
;
;	USES	flags

	public Cancel_FloppyTimer
Cancel_FloppyTimer proc near

	mov	FloppyTimer_Active, 0
	SaveReg <ax,bx,dx>
	mov	bx, 0ffffH			; maximum length
	mov	ax, CodeOffset FloppyTimer	; set up timer service routine
	DEVHLP	DevHlp_TickCount		; also sets carry on error
	RestoreReg <dx,bx,ax>			;
	ret					;
Cancel_FloppyTimer endp

	BREAK <Set_FTimer>
;***	Set_FTimer - Arm one shot timer for fixed (NON-AT) and
;		     floppy (AT) drives
;
;	Set_FTimer will arm the FTimer.  Input parameter
;	values in microseconds will be converted to timer
;	ticks.
;
;	ENTRY	DX:AX = delay value in microseconds
;
;	EXIT	CF set if timer armed
;		CF clear if zero count passed
;
;	USES	AX, DX, and Flags

	public Set_FTimerU
	public Set_FTimerS
Set_FTimerU proc near
  .IF <ax eq 0> and		  ; zero count?
  .IF <dx eq 0> 		  ; zero count?
     clc			  ;
     ret			  ;
  .ENDIF			  ;
  push	UTick
  pop	TimerConv		  ; set up divide
  jmp	  short   setit

Set_FTimerS label near
  .IF <ax eq 0> 		  ; zero count?
     clc			  ;
     ret			  ;
  .ENDIF			  ;
  mov	TimerConv, THOUSAND	  ; set up multiply
  mul	TimerConv ; convert to seconds(AX) to milliseconds(DX:AX)
		    jnc     no_overflow2
		    overflow2: jmp short overflow2
		    no_overflow2:
  push	MTick
  pop	TimerConv		  ; set up divide

setit:
  div TimerConv 	; convert to milliseconds(DX:AX) to ticks
			; the quotient in AX, remainder in DX
  .IF <dx ne 0> 		  ; remainder?
     inc ax			  ; yes, round up
  .ENDIF			  ;
   SaveReg  bx
   mov	bx, ax
   mov	ax, CodeOffset FTimer  ; install timer service
DISABLE
   mov	   FTimer_Active, 1
   DEVHLP DevHlp_TickCount	  ; routine
ENABLE
   RestoreReg  bx
   stc				  ;
   ret				  ;

Set_FTimerU endp

	BREAK <Cancel_FTimer>
;***	Cancel_FTimer - Cancel one shot timer for fixed and floppy drives
;
;	Cancel_FTimer will cancel the FTimer.
;
;	ENTRY	none
;
;	EXIT
;
;	USES	flags

	public	Cancel_FTimer
Cancel_FTimer proc near

	mov	FTimer_Active, 0
	SaveReg <ax,bx,dx>
	mov	bx, 0ffffH			; maximum length
	mov	ax, CodeOffset FTimer		; set up timer service routine
	DEVHLP	DevHlp_TickCount		; also sets carry on error
	RestoreReg <dx,bx,ax>			;
	ret					;

Cancel_FTimer	   endp

	BREAK <Set_F2Timer>
;***	Set_F2Timer - Arm one shot timeout timer for fixed (AT)
;
;	Set_F2Timer will arm the F2Timer.  Input parameter
;	values in microseconds will be converted to timer
;	ticks.
;
;	ENTRY	none
;
;	EXIT	none
;
;	USES	Flags

	public Set_F2Timer
Set_F2Timer proc near
   SaveReg  <bx, ax>
   mov	bx,  0010H
   mov	ax, CodeOffset F2Timer		; install timer service
   DEVHLP DevHlp_TickCount		; routine
   RestoreReg  <ax, bx>
   ret
Set_F2Timer endp

	BREAK <Cancel_F2Timer>
;***	Cancel_F2Timer - Cancel one shot timer for fixed drive
;
;	Cancel_F2Timer will cancel the F2Timer.
;
;	ENTRY	none
;
;	EXIT
;
;	USES	flags
	public	Cancel_F2Timer
Cancel_F2Timer proc near

	mov	F2Timer_Active, 0
	SaveReg <ax,bx,dx>
	mov	bx, 0ffffH			; maximum length
	mov	ax, CodeOffset F2Timer		; set up timer service routine
	DEVHLP	DevHlp_TickCount		; also sets carry on error
	RestoreReg <dx,bx,ax>			;
	ret					;

Cancel_F2Timer	    endp

	BREAK <SetTimer2>
;***	SetTimer2 - Arm timer 2
;
;	SetTimer2 will set a motor off timeout for the
;	drive whose parameter block is pointed to by
;	DS:BX
;
;	ENTRY	DS:BX = pointer to per drive info.
;
;	EXIT	NONE
;
;	USES	Flags

	public SetTimer2
SetTimer2 proc

	push	ax
	mov	al, MoffDelay
	mov	[bx].Timer,al
	or	TimerActive,1
	pop	ax
	ret
SetTimer2 endp

CodeInit	segment

;***	TimerInit
;
;	.....
;	......
;
;	EXIT	NONE
;
;	USES	Flags
	public	TimerInit
TimerInit	proc	near
	pusha
	push	es

	mov	al, GLOBINFOSEG 		; get var globalinfoseg
	DEVHLP	DevHlp_GetDOSVar		; also sets carry on error
	mov	es, ax				; ax:bx -> sysinfoseg
	mov	es, WORD PTR es:[bx]		; es:bx -> addr/sysinfo
	xor	bx,bx
	mov	ax, es:[bx].SIS_ClkIntrvl	; get timer interval

	xor	dx, dx
	mov	cx, 10
	div	cx

	mov	MTick, ax			;
	xor	dx, dx
	mov	cx, THOUSAND
	mul	cx
	mov	UTick, ax
	mov	TimerConv, ax

	mov	bx, TIMER2COUNT
	mov	ax, CodeOffset DriveTimer	; set up timer service routine
	DEVHLP	DevHlp_TickCount		; also sets carry on error

	mov	bx, 0ffffH
	mov ax, CodeOffset FloppyTimer		; set up timer service routine
	DEVHLP	DevHlp_TickCount		; also sets carry on error

	mov ax, CodeOffset FTimer		; set up timer service routine
	DEVHLP	DevHlp_TickCount		; also sets carry on error

	cmp	ATMCH, 1			; don't start a timer for
	jne	nonAT				; the NON-AT machines

	mov ax, CodeOffset F2Timer		; set up timer service routine
	DEVHLP	DevHlp_TickCount		; also sets carry on error
nonAT:
	pop	es
	popa
	ret
TimerInit      endp


CodeInit     ends

Code	ends

	end
