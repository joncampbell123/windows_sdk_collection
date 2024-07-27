;************************************************************************
;									*
;   Copyright (C) 1983,1984,1985,1986,1987 by Microsoft Inc.		*
;									*
;************************************************************************

	TITLE SYSTEM - InquireSystem procedure to return info about devices
.286p

	include system.inc

;; AT&T Machines running DOS 2.11, revision 1.0 place this
;; value into SingleDriveLoc
ATTOld21Loc EQU 1670h

;; AT&T Machines running DOS 2.11, revisions 1.1, 2.0, 2.01 place
;; this value into SingleDriveLoc
ATT21Loc EQU	14A0h

;; AT&T Machines running DOS 3.10, revisons 1.0 and 1.01 place
;; this value into SingleDriveLoc
ATT31Loc EQU	14d0h

MultHIMEM		EQU	43h	; HIMEM.SYS int 2fh multiplex
MHM_ReqInstall		EQU	00h	; Installation check
MHM_ReqInstall_Ret	EQU	0FFh	; I'm here Return

externFP	NoHookDOSCall

sBegin	CODE	    ; Beginning of code segment
assumes CS,CODE

;
; InquireSystem(what,which) - returns oem specific information
;	what is the code for the device
;	which specifies which one of those devices
;
;   WHAT = 0	    Timer resolution
;	Return the resolution of the timer specified by the which
;	parameter in DX:AX.  Windows always uses which == 0
;
;   WHAT = 1	    Disk Drive Information (Drive A = 0)
;	which is the disk drive (A = 0)
;	Returns:
;	    ax = 0 means the drive does not exist.  if dx != 0 then the drive
;		maps to the drive in dx instead (A = 1) AND the drive is
;		REMOVEABLE.
;	    ax = 1 means the drive does not exist.  if dx != 0 then the drive
;		maps to the drive in dx instead (A = 1) AND the drive is
;		FIXED.
;	    ax = 2 means the drive is removable media
;	    ax = 3 means the drive is fixed media
;
;   WHAT = 2	    Enable/Disable one drive logic
;	which = 0 means disable, which <> 0 means enable
;	This code enables/disables the RAM BIOS message:
;	"Please insert disk for drive B:"
;
externW		AT_DOS30

;
; The following flag deals with some unpleasantness in the fast boot code.
;   The fast boot code delays our INIT call till to late because some code
;   in KERNEL which uses InquireSystem is called first. We fix this problem
;   with this flag......
;
globalB 	SystemIsInited,0

; Following from RAMDRIVE
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Unfortunately the code in ramdrive is very machine dependent
; necessitating the use of a system flag to store the machine
; configuration. The system flag is initialised during init time
; and used when the caching services are requested. One bit which
; is set and tested during caching is the state of the a20 line
; when the cache code is entered. This is used because there are
; applications which enable the a20 line and leave it enabled 
; throughout the duration of execution.  Since ramdrive is a device
; driver it shouldn't change the state of the environment.
;
; The system flag bit assignments are:
;
;	-------------------------------------------------
;	|  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
;	-------------------------------------------------
;	   |-----|     |     |	   |	 |     |     |
;	      |        |     |	   |	 |     |     -----286 (and AT)
;	      |        |     |	   |	 |     -----------386 (later than B0)
;	     not       |     |	   |	 -----------------PS/2 machine
;	    used       |     |	   -----------------------Olivetti (not used)
;		       |     -----------------------------A20 state (enabled ?)
;		       -----------------------------------DOS 3.x >= 3.3

; The Olivetti guys have defined a flag of their own. This should be removed
; and the bit assigned out here for them should be used. 
;
sys_flg		db	0
;
;	equates used for the system flag
;
M_286		equ	00000001B
M_386		equ	00000010B
M_PS2		equ	00000100B
M_OLI		equ	00001000B
A20_ST		equ	00010000B
DOS_33		equ	00100000B
HAVE_FFFE	equ	01000000B
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; A20 address line state determination addresses
;
low_mem label	dword
	dw	20h*4
	dw	0

high_mem label	dword
	dw	20h*4 + 10h
	dw	0ffffh

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; A20 PS2 equates
;
PS2_PORTA   equ 0092h
GATE_A20    equ 010b

; End RAMDRIVE stuff

globalB		numFloppies,0
globalB		oneDriveFlag,0
globalW		coProcFlag,0
globalD		prevInt13proc,0
globalD		HiMem,0

DOSVER		DW	0

;; SingleDriveLoc defaults to the value of SingleDrive (504h) on other
;; than AT&T machines.	Otherwise the value is changed during
;; the execution of single drive enable/disable.
SingleDriveLoc	dw	SingleDrive

cProc	InitSystem,<PUBLIC,FAR>
cBegin	nogen
	cmp	SystemIsInited,0	; Have we already done this?
	jnz	no_80x87		; Yes
	inc	SystemIsInited		; We will now init
	cCall	InitA20			; See if we have memory at FFFE:20
	int	11h			; get equipment word
	push	ax
	mov	cl,6
	shr	ax,cl
	and	al,00000011b		; isolate drive count
	mov	numFloppies,al
	pop	ax

; Set CoProcFlag

	mov	CoProcFlag,0
	test	al,2			; this is the IBM approved method
	jz	no_80x87		;   to check for an 8087
	mov	CoProcFlag,94		; size of save area
	FNINIT
no_80x87:
	mov	ax,1
	ret

cEnd	nogen

cProc	InquireSystem,<FAR,PUBLIC,NODATA>
    parmW   what
    parmW   which
cBegin
	cmp	SystemIsInited,0	; Are we ready for this call?
	jnz	DoInq			; Yes
	call	InitSystem		; No, Set up
DoInq:
	mov	ax,what
;---------------------------------------
;
;  Timer information
;
	or	ax,ax
	jnz	is1
	mov	dx,res_high
	mov	ax,res_low
	jmp	ISDone
;---------------------------------------
;
;  Drive information
;
is1:	dec	ax		; ax = 1?
	jz	DriveInfo
	jmp	is5

DriveInfo:
	mov	ah,19H		; get the current disk
	cCall	NoHookDOSCall
	mov	bx,Which	; try to set to this disk
	cmp	al,bl		; already there?
	jz	DriveData	; yes, drive is good

	push	ax
	mov	dx,bx
	mov	ah,0eH		; set to new disk
	cCall	NoHookDOSCall
	mov	ah,19H		; get the current disk
	cCall	NoHookDOSCall
	mov	bh,al
	pop	dx
	mov	ah,0eH		; restore current disk
	cCall	NoHookDOSCall
	cmp	bh,bl		; Drive good?
	jz	DriveData	; yes
	jmp	is9		; no, this drive totally bad

DriveData:
    ; First check if this is network. We must do this first because
    ;  the removeable and phantom IOCTL calls return errors if you feed
    ;  them network drives. If it is network then we know it is non-removable
    ;  and not phantom.
	mov	ax,4409H	; IOCTL is Remote
	mov	bx,Which
	inc	bx		; A = 1
	cCall	NoHookDOSCall
	jc	DoRem		; Call didn't work, go ahead
	test	dx,0001000000000000B
	jz	DoRem		; Drive is local
	mov	cx,FIXED	; Drive is not removeable
	jmp	short NoRemap	; Drive is not phantom

DoRem:
    ; Now Check "removeability"
	mov	ax,4408H	; IOCTL is removeable
	mov	bx,Which
	inc	bx		; A = 1
	cCall	NoHookDOSCall
	jc	OLDRemove	; Call didn't work, use old method
	mov	cx,FIXED
	test	ax,1
	jnz	DrivePhantom
	mov	cx,REMOVEABLE

DrivePhantom:
    ; Now check for phantom drives
	mov	ax,440EH	; IOCTL get logical map
	mov	bx,Which
	inc	bx		; A = 1
	cCall	NoHookDOSCall
	jc	OLDPhantom	; Call didn't work, use old method
	or	al,al		; If AL=0, drive is NOT phantom
	jz	NoRemap
	cmp	bl,al		; Drive maps to self?
	jz	NoRemap		; Yes, drive is not phantom
	xor	ah,ah
	mov	dx,ax		; DX is real drive
SetPhantomRet:
	xor	ax,ax		; Set removeable return
	cmp	cx,REMOVEABLE
	jz	IsDoneV
	inc	ax		; Set fixed return
	jmp	short IsDoneV

NoRemap:
	xchg	ax,cx		; AX = type of drive
	xor	dx,dx		; Indicate no remapping
IsDoneV:
	jmp	ISDone


OLDRemove:
    ; Check removeability with equipment word
	xor	ax,ax
	or	al,numFloppies	; just one floppy on system?
	jnz	OLDR1		; no, continue
	inc	ax		; pretend we have two floppies...
OLDR1:
	cmp	ax,which
	mov	cx,FIXED
	jb	DrivePhantom
	mov	cx,REMOVEABLE
	jmp	short DrivePhantom

OLDPhantom:
    ; Check phantomness with equipment word
	cmp	Which,1		; Drive B is only phantom
	jnz	NoRemap		; Not drive B, so not phantom
	cmp	numFloppies,0	; Single floppy system?
	jnz	NoRemap		; No, drive B is real
	mov	dx,1		; Drive B is really drive A
	jmp	short SetPhantomRet

;---------------------------------------------------
;
;  Single Floppy enable/disable
;
is5:
	dec	ax			; floppy enable disable?
	jz	is5a
	jmp	is9

is5a:
	cmp	cs:[DOSVER],0		; Inited?
	jnz	is5b			; yes
	mov	ah,30H			; Get DOS version
	int	21H
	xchg	ah,al			; major <-> minor
	mov	cs:[DOSVER],ax

is5b:	cmp	which,0			; 0=disable
	jnz	is6
;
;  Disable various OEM things
;
	cmp	cs:[DOSVER],0314H		; Below DOS 3.20?
	jae	nosingdrv1			; No, no ROM area diddle

	mov	ax,0fc00h			;; is this an AT&T machine ?
	mov	es,ax				;; look for start of 'OLIVETTI'
	cmp	es:[0050h],'LO'
	jnz	ATTCheckDone			;; No, continue

;;
;;  AT&T versions of MS-DOS prior to 3.20 do not keep information
;;  on the last floppy drive accessed at 504h.	According to the
;;  version, this flag can be found at one of three locations (see
;;  the equates at the top of this file).  The purpose of this section
;;  of code is to locate the bytes and patch them accordingly.
;;

	mov	[SingleDriveLoc],ATT31Loc	;; assume DOS 3.1
	cmp	[DOSVER],030Ah			;; see if that is the case
	jz	ATTCheckDone
	xor	ax,ax				;; one final check
	mov	es,ax				;; --to see if old DOS 2.11
	mov	bx,ATT21Loc
	mov	[SingleDriveLoc],bx		;; assume later ver of 2.11
	cmp	es:[bx+1],0a0dh			;; CR-LF here?
	jz	ATTCheckDone			;; yes, right version
	mov	[SingleDriveLoc],ATTOld21Loc	;; nope, real old 2.11 version
ATTCheckDone:

	xor	ax,ax
	mov	es,ax
	mov	bx,[SingleDriveLoc]		;; set to drive A
	xchg	al,byte ptr es:[bx]
	mov	cs:[oneDriveFlag],al		; remember previous setting
nosingdrv1:
;
;  This is a long story.  There is a bug in the IBM PC AT INT 13H ROM code.
;  IBM DOS 3.10 has a work around for the problem, but DOS 3.00 does not.
;  If an interrupt service routine takes along time while a disk read or write
;  is proceeding it may complete too early.  Windows fixes this problem with
;  the following weirdness
	cmp	cs:[DOSVER],0300H	; is it DOS 3.00?
	jnz	is9			; nope
	mov	ax,0F000H
	mov	es,ax
	cmp	byte ptr es:[0FFFEH],0FCH ; is it an AT?
	jnz	is9			  ; nope..
	call	HookInt13
	jmp	short is9
;
;   Enable various OEM things
;
is6:
	cmp	cs:[DOSVER],0314H	; Below DOS 3.20?
	jae	nosingdrv2		; No, no ROM diddle
	xor	ax,ax
	mov	es,ax
	mov	bx,[SingleDriveLoc]	;; pointer to value
	mov	al,cs:[oneDriveFlag]
	mov	byte ptr es:[bx],al		;; restore to correct drive
nosingdrv2:
;
; If we stuck in an INT 13H handler, take it out
;
	lds	dx,cs:[prevInt13Proc]
	mov	cx,ds
	jcxz	is9
	mov	ax,2513H
	int	21H

is9:	xor	dx,dx
	xor	ax,ax
ISDone:
cEnd	Inquire


HookInt13:
	mov	ax,3513H	; get previous int 13 proc
	int	21h
	mov	word ptr cs:[prevInt13proc  ],bx
	mov	word ptr cs:[prevInt13proc+2],es
	push	cs
	pop	ds
	mov	dx,codeOffset Int13Handler
	mov	ax,2513H
	int	21H
	ret


Int13Handler	proc	far
	cmp	ah,2			; function 2 through...
	jb	i13x
	cmp	ah,12			; function 12?
	ja	i13x
	test	dl,80H			; on a hard disk?
	jz	i13x			; nope...
	mov	byte ptr cs:[AT_DOS30],0
	sti
	pushf
	call	cs:[prevInt13proc]
	mov	byte ptr cs:[AT_DOS30],1
	ret	2

i13x:	jmp	cs:[prevInt13Proc]
Int13Handler	endp


;-----------------------------------------------------------------------;
; Get80x87SaveSize                                                      ;
; 									;
; 									;
; Arguments:								;
; 									;
; Returns:								;
; 									;
; Error Returns:							;
; 									;
; Registers Preserved:							;
; 									;
; Registers Destroyed:							;
; 									;
; Calls:								;
; 									;
; History:								;
; 									;
;  Thu Feb 05, 1987 10:15:13p  -by-  David N. Weise   [davidw]          ;
; Wrote it.								;
;-----------------------------------------------------------------------;

cProc	Get80x87SaveSize,<PUBLIC,FAR>
cBegin nogen
	mov	ax,CoProcFlag
	ret	
cEnd nogen


;-----------------------------------------------------------------------;
; Save80x87State                                                        ;
; 									;
; 									;
; Arguments:								;
; 									;
; Returns:								;
; 									;
; Error Returns:							;
; 									;
; Registers Preserved:							;
; 									;
; Registers Destroyed:							;
; 									;
; Calls:								;
; 									;
; History:								;
; 									;
;  Thu Feb 05, 1987 10:15:17p  -by-  David N. Weise   [davidw]          ;
; Wrote it.								;
;-----------------------------------------------------------------------;

cProc	Save80x87State,<PUBLIC,FAR>
;	parmD	savearea
cBegin nogen
	mov	bx,sp
	les	bx,[bx][4]
	fsave	es:[bx]
	ret
cEnd nogen


;-----------------------------------------------------------------------;
; Restore80x87State                                                     ;
; 									;
; 									;
; Arguments:								;
; 									;
; Returns:								;
; 									;
; Error Returns:							;
; 									;
; Registers Preserved:							;
; 									;
; Registers Destroyed:							;
; 									;
; Calls:								;
; 									;
; History:								;
; 									;
;  Thu Feb 05, 1987 10:15:23p  -by-  David N. Weise   [davidw]          ;
; Wrote it.								;
;-----------------------------------------------------------------------;

cProc	Restore80x87State,<PUBLIC,NEAR>
;	parmD	savearea
cBegin nogen
	mov	bx,sp
	les	bx,[bx][4]
	frstor	es:[bx]
	ret
cEnd nogen

;-----------------------------------------------------------------------;
; InitA20								;
; 									;
; 									;
; Arguments:								;
; 									;
; Returns:								;
; 									;
; Error Returns:							;
; 									;
; Registers Preserved:							;
;		SI, DI, DS, ES						;
; 									;
; Registers Destroyed:							;
;		AX, BX, CX, DX, Flags					;
; 									;
; Calls:								;
; 									;
; History:								;
; 									;
; author:  sunilp, august 1, 1987.  thanks to rickha for most of this	;
; 	   routine.							;
;-----------------------------------------------------------------------;

cProc	InitA20,<PUBLIC,NEAR>,<cx,si,di,ds,es>
cBegin
	mov	ax,(MultHIMEM SHL 8) OR MHM_ReqInstall	; install check
	int	2Fh
	or	al, al				; Under HIMEM.SYS?
	jz	no_chance_mate
	mov	ax, (MultHIMEM SHL 8) OR 10h	; Get Control Function Address
	xor	bx, bx
	clc
	int	2Fh
	jc	no_chance_mate
	or	bx, bx				; No change, we've been fooled!
	jz	no_chance_mate
	mov	word ptr [HiMem], bx		; Fill in our pointer
	mov	word ptr [Himem+2], es

	xor	ah, ah				; Get Version
	call	[HiMem]
	cmp	ax, 101h			; Currently 1.x
	jb	no_chance_mate

	mov	ax, (1 shl 8) or 64		; Request 64k of wrap region
	call	[HiMem]
	or	ax, ax
	jz	no_chance_mate

	or	[sys_flg], HAVE_FFFE		; We own the Wrap Region

	mov	ah, 7				; Is A20On?
	call	[HiMem]
	or	ax, ax
	jz	Have_It
	or	[sys_flg], A20_ST		; Already ON....
Have_It:
	xor	ax, ax
	jmp	short A20I_Exit

no_chance_mate:				; Nothing to do
	mov	ax, 1			; Inform caller

A20I_Exit:

cEnd

;-----------------------------------------------------------------------;
; A20_Proc								;
; 									;
; Arguments:								;
; 									;
; Returns:								;
; 									;
; Error Returns:							;
; 									;
; Registers Preserved:							;
; 									;
; Registers Destroyed:							;
; 									;
; Calls:								;
; 									;
; History:								;
; 									;
;  Thu Mar 31, 1988 -by-  Tony Gosling					;
; 									;
;-----------------------------------------------------------------------;

cProc	A20_Proc,<PUBLIC,FAR,NODATA>
	parmW	enable
cBegin
	cmp	SystemIsInited,0	; Are we ready for this call?
	jnz	DoA20			; Yes
	call	InitSystem		; No, Set up
DoA20:
	test	[sys_flg], HAVE_FFFE	; OK machine?
	jz	A20Not
	cmp	enable, 1		; Enable if 1
	je	Enable_A20

	test	[sys_flg], A20_ST
	jnz	Skip_Disable		; Was on when we started, don't disable
	mov	ah, 4			; DISABLE
	call	[HiMem]
Skip_Disable:
	cmp	enable, 2		; 2 is Bye-Bye, Release Hi Mem as well
	jne	A20Suc

	mov	ah, 2			; Release the wrap region
	call	[HiMem]
	jmp	short A20Suc

Enable_A20:
	mov	ah, 3			; ENABLE
	call	[HiMem]
A20Suc:
	xor	ax, ax
	jmp	short A20Exit
A20Not:
	mov	ax, 2
	jmp	short A20Exit
A20Err:
	mov	ax, 1
A20Exit:

cEnd

sEnd	CODE		; End of code segment

END	InitSystem
