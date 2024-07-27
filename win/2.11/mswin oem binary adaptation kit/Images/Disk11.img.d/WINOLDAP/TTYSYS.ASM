page ,132
	TITLE	HP System - save and restore the Vectra firmware
;=========================================================================
;
; TTYSYS 
;
; Description	Routines which save and restore the save of the HP EX-BIOS
;		on Vectra.
;
; History	12/5/85  Created by Richard Hawes.
;				    Tuan Nguyen
;
;		08/12/86 ~~Expand the HPSaveTable to save and
;			   restore V_WINDOWS
;
;		6/11/87  Move HPmouse code to TTYSWAP (RMH).
;		
;		9/24/87	 Added code to save speed.
;		
;		9/24/87  Created a SaveTranslators function which
;			 works on Vectra ES and RS.
;
;		$Author:   rmh  $
;		$Revision:   1.5  $
;		$Date:   28 Sep 1987 16:01:50  $
;
;
;==========================================================================

	?WIN	= 0
	.xlist
	include	equate.inc
	include	cmacros.inc
	.list
	include	ttysys.inc
page

sBegin	DATA 	


; Well defined save table. Well defined areas are those whose size
; is known before run time.
;
SAVE_TABLE_REC	struc	
SVector	dw	?
SFirst	dw	?			     
SLast	dw	?
SLoc	dw	?
SAVE_TABLE_REC	ends  

DefineTable 	label 	word 
SAVE_TABLE_REC 	<V_LTOUCH, LD_SOURCE, LD_ACCUM_Y+2, LTouch>
SAVE_TABLE_REC 	<V_LPOINTER, LD_SOURCE, LD_ACCUM_Y+2, LPointer>
SAVE_TABLE_REC 	<V_LTABLET, LD_SOURCE, LD_ACCUM_Y+2, LTablet>
SAVE_TABLE_REC	<V_SPCCP, DH_V_PARENT, DH_MAJOR, HPCCP>
SAVE_TABLE_REC	<V_PSOFTKEY, DH_V_PARENT, DH_MAJOR, HPSoftkeys>
	
NUM_DEFINE_TBL_ENTRIES	equ	($-DefineTable)/8
					
sEnd	DATA
 

page
sBegin	CODE
	assumes cs, CODE
	assumes	ds, DATA

ExternB		<fVectra>		; (TTYTASK)
ExternW		<HPsize>
;
	public	IsVectra, FindVectra, EnableVectra, DisableVectra,
	public	SaveLinkMapping, RestoreLinkMapping, 
	public  SaveKeyTrans, RestoreKeyTrans

GlobalW	HPVectorTable, 0		; Address of the HP Vector table


CurHPEntry	db	0		; Current HPEntry vector
CurHILState	db	0		; Bit 6= current HIL OFF state

page 

;------------------------------------------------------------------
;
; SysCall	Driver, AXReg
;
; Purpose	General purpose HP system calling routine
;
; Parameters	Driver  which will be stored in BP
;		AX value
;
; Results	returns AH which is 0 for success
;				    2 for unsupported
;
;-------------------------------------------------------------------	
SysCall macro	device, AXReg
	mov	ax, device
	push	ax
	mov	ax, AXReg
	call	HPSysCall
	endm

cProc	HPSysCall,<NEAR>, <ds,bp>
	parmW	Device

cBegin	HPSysCall
	mov	bp, Device
	int	HPENTRY			; Status in AH

cEnd	HPSysCall
	 
page
;--------------------------------------------------------------------------
;
; SaveHPSystem( lpSystemState ):Size
;
; Purpose	To save the state of the HP EX-BIOS on the Vectra
;
; Parameters	lpSystemState - if NULL just return size. 
;		DS= SS=  Winoldap data segment
;
; Results	The size of SystemState in Paras
;
; Notes		The following items are saved:
;			HP Vector table
;			LTABLET	 data structure
;			LPOINTER data structure
;			LTOUCH	 data structure
;			all physical drivers mapping	       
;
;		Since the size of the system state can vary with different
;		version of Vectra, SaveHPSystem should be called first with
;		lpSystemState = NULL to get the size of the system state.
;
;---------------------------------------------------------------------------
cProc	SaveHPSystem, <NEAR,PUBLIC>, <DS,ES,SI,DI>
	parmD	HPState
	localW	StateSize	
cBegin	SaveHPSystem	

; set DS - Winoldap data segment, ES - Saved HP data segment, cld
	SysCall V_SYSTEM, <F_INS_BASEHPVT shl 8>
	mov	CS:[HPVectorTable], es
	cld
	
; Find the size of SAVE_STATE if HPState is NULL
; if load is true load them up
;
	les	di, HPState		; Inquire HPstate size if NULL
	mov	ax, es
	or	ax, di
	jz	SaveRet

	
; Save the vector table
	EnterCrit  
	xor	si, si
	mov	DS, CS:[HPVEctorTable]
	les	di, HPState
	lea 	di, ES:[di].VectorTable
 	mov	cx, (V_WINDOWS+6)/2
	cld
	rep	movsw

; Save the well defined data areas
	
	push	SS
	pop	DS			; DS = Winoldap Data segment
	lea	si, DefineTable
	mov	cx, NUM_DEFINE_TBL_ENTRIES		    
define_save_loop:
	mov	ax, DS:[si].SLoc
	add	ax, OFF_HPstate
	Save	cx
	cCall	SaveData, <DS:[si].SVector, DS:[si].SFirst, DS:[si].SLast, ax>
	add	si, size SAVE_TABLE_REC
	loop	define_save_loop

; Save the link mapping 
	les	di, HPState
	lea	di, es:[di].Devices
	call	SaveLinkMapping

; Save the keyboard translators
	les	di, HPState
	lea	di, es:[di].Translators
	call	SaveKeyTrans

; Save the speed and key click of the system. Note this will not
; work without EX-BIOS. Tuff Luck.
;
	les	di, HPState
	mov	bl, 1			; STATE_1
   	SysCall	V_SCANDOOR, <F_STATE_IOCTL shl 8 + SF_GET_STATE>
   	mov	es:[di].Click, bh
	mov	bl, 3			; STATE_3
   	SysCall	V_SCANDOOR, <F_STATE_IOCTL shl 8 + SF_GET_STATE>
   	mov	es:[di].Speed, bh
	
	LeaveCrit
SaveRet:
	mov	AX, size SAVE_STATE     ; AX= Size of HPstate in bytes using

cEnd	SaveHPSystem	


page
;--------------------------------------------------------------------------
;
; RestoreHPSystem( lpSystemState )
;
; Purpose	To restore the state of the HP EX-BIOS on the Vectra
;
; Parameters	Long pointer lpSystemState
;		DS = SS = Winoldap data segment
;
; Notes		The following items are resotred:
;			HP Vector table
;			LTABLET	 data structure
;			LPOINTER data structure
;			LTOUCH	 data structure
;			all physical drivers mapping	       
;
;---------------------------------------------------------------------------
cProc	RestoreHPSystem, <NEAR,PUBLIC>, <DS,ES,SI,DI>
	parmD	HPRState
	localV	CurrentDevices, 14

cBegin	RestoreHPSystem	
		
	EnterCrit

; Restore the speed and key click of the system
	les	di, HPState
	mov	bl, 1			; STATE_1
   	mov	bh, es:[di].Click
   	SysCall	V_SCANDOOR, <F_STATE_IOCTL shl 8 + SF_SET_STATE>
	mov	bl, 3			; STATE_3
   	mov	bh, es:[di].Speed
   	SysCall	V_SCANDOOR, <F_STATE_IOCTL shl 8 + SF_SET_STATE>
	
; Restore the keyboard translators
	les	di, HPRState
	lea	di, es:[di].Translators
	call	RestoreKeyTrans

; Restore the link
	les	di, HPRState
	lea	di, es:[di].Devices
	call	RestoreLinkMapping
					
; Restore the defined data areas
	push	ss
	pop	es
	mov	ds, SEG_HPRState

; ES= Winoldap data - DS= Saved HP area
	lea	si, DefineTable
	mov	cx, NUM_DEFINE_TBL_ENTRIES		    
restore_loop:
	mov	ax, ES:[si].SLoc
	add	ax, OFF_HPRState
	Save	cx
	cCall	RestoreData, <ES:[si].SVector, ES:[si].SFirst, ES:[si].SLast, ax>
	add	si, size SAVE_TABLE_REC
	loop	restore_loop		

; Restore the vector table
	mov	es, CS:[HPVectorTable]	; DS= Saved HP area - ES= HP Vector table
	lds	si, HPRState
	lea 	si, [si].VectorTable
	xor	di, di
 	mov	cx, (V_WINDOWS+6)/2
	cld
	rep	movsw
		 
	LeaveCrit

cEnd	RestoreHPSystem	

;---------------------------------------------------------------------------
;
; SaveKeyTrans - Save the state of the keyboard translators.
;
;	The state of the keyboard translators are saved in two places:
;	First, in the headers of V_SCCP and V_PSOFTKEY and in the
;	8042 chip.
;	
;
; Entry
;	ES:DI	- Pointer to the save area. Must be 6 bytes.
;	DS, SS  - Winoldap data segment
;	HP ENTRY must be 6Fh
;
; Exit
;
; Uses
;	AX, BX
;
;---------------------------------------------------------------------------
SaveKeyTrans:
	test	[fVectra], 1
	jz	skExit

   ; Save the cursor pad header
   ;	
	mov	bx, V_SPCCP		; Cursor cursor pad	
	call	GetParent
	mov	es:[di], ax

   ; Save the SoftKeys header
   ;	
	mov	bx, V_PSOFTKEY		
	call	GetParent
	mov	es:[di+2], ax
   
   ; Save the state of Scandor. If V_SCANDOOR is not present
   ; these calls will have no effect.
   ;
   	mov	bl, 0			; State 0
   	SysCall	V_SCANDOOR, <F_STATE_IOCTL shl 8 + SF_GET_STATE>
   	mov	es:[di+4], bh
   	mov	bl, 2			; State 2
   	SysCall	V_SCANDOOR, <F_STATE_IOCTL shl 8 + SF_GET_STATE>
   	mov	es:[di+5], bh
skExit:
	ret
	
;	
; Save the parent
;
;    BX    =  Driver vector number
;    ES:DI -> Storage word	
;
GetParent:
	push	es
	
; get HP vector table
	mov	es, cs:[HPVectorTable]	
	
; get the Device header address
	mov	es, es:[bx+4]		; Get the DS of the driver
	mov	ax, es:[DH_V_PARENT]	; load the parent

; Store the parent
	pop	es
	ret

;---------------------------------------------------------------------------
;
; RestoreKeyTrans - Restore the state of the keyboard translators.
;
;	The state of the keyboard translators are saved in two places:
;	First, in the headers of V_SCCP and V_PSOFTKEY and in the
;	8042 chip.
;	
;
; Entry
;	ES:DI	- Pointer to the save area. Must be 6 bytes.
;	DS, SS  - Winoldap data segment
;	HP ENTRY must be 6Fh
;
; Exit
;
; Uses
;	AX, BX
;
;---------------------------------------------------------------------------
RestoreKeyTrans:
	test	[fVectra], 1
	jz	rkExit

   ; Restore the cursor pad header
   ;	
	mov	bx, V_SPCCP		; Cursor cursor pad	
	mov	ax, es:[di]
	call	SetParent

   ; Restore the SoftKeys header
   ;	
	mov	bx, V_PSOFTKEY		
	mov	ax, es:[di+2]
	call	SetParent
   
   ; Restore the state of Scandor. If V_SCANDOOR is not present
   ; these calls will have no effect.
   ;
   	mov	bl, 0			; State 0
   	mov	bh, es:[di+4] 
   	SysCall	V_SCANDOOR, <F_STATE_IOCTL shl 8 + SF_SET_STATE>
   	mov	bl, 2			; State 2
   	mov	bh, es:[di+5]
   	SysCall	V_SCANDOOR, <F_STATE_IOCTL shl 8 + SF_SET_STATE>
rkExit:
	ret
	
;	
; Set the parent
;
;    BX    =  Driver vector number
;    ES:DI -> Storage word	
;
SetParent:
	push	es
	
; get HP vector table
	mov	es, cs:[HPVectorTable]	
	
; get the Device header address
	mov	es, es:[bx+4]		; Get the DS of the driver
	mov	es:[DH_V_PARENT], ax	; set the parent

; Store the parent
	pop	es
	ret


;---------------------------------------------------------------------------
;
; SaveLinkMapping
;
;	Save the device mapping of the HP-HIL link.
;
; Entry
;	ES:DI	- Pointer to the save area. Must be 28 bytes.
;	DS, SS  - Winoldap data segment
;	HP ENTRY must be 6Fh
;
; Exit
;
; Uses
;	C Convention
;
;---------------------------------------------------------------------------
cProc 	SaveLinkMapping, <NEAR>, <ES,DS,SI,DI>
cBegin
	call	IsVectra
	jz	sdExit
	
	mov	si, di
	SysCall V_SINPUT, <F_INQUIRE_ALL shl 8>
	mov	si, di
	add	si, 14	
	SysCall V_SINPUT, <F_IO_CONTROL shl 8 + SF_GET_LINKS>
sdExit:
cEnd

;---------------------------------------------------------------------------
;
; RestoreLinkMapping
;
;	Restore the device mapping of the HP-HIL link.
;
; Entry
;	ES:DI	- Pointer to the save area. Must be 28 bytes.
;	DS, SS  - Winoldap data segment
;	HP ENTRY must be 6Fh
;
; Exit
;
; Uses
;	C convention
;
;---------------------------------------------------------------------------
cProc	RestoreLinkMapping, <NEAR>, <DS,ES,SI,DI>
	localV	CurrentDevices, 14
cBegin		
	call	IsVectra
	jz	rdExit
	
; Has the link changed ?
	push	es
	push	ss
	pop	es
	lea	si, CurrentDevices
	SysCall V_SINPUT, <F_INQUIRE_ALL shl 8>
	pop	es

; DS= Saved HP area - ES= Winoldap data
	push	di
	mov	cx, 7
	cld
	repe	cmpsw
	pop	di
	je	restore_old
	
; Yes: restore the default mapping

	SysCall V_SINPUT, <F_IO_CONTROL shl 8 + SF_DEF_LINKS>
	jmp	rdExit
	
; No: Set the current mapping to OldLinkMapping 
restore_old:

	mov	si, di
	add	si, 14	
        SysCall V_SINPUT, <F_IO_CONTROL shl 8 + SF_SET_LINKS>
rdExit:
cEnd





page
;---------------------------------------------------------------------------
;
; SaveData( Vector, Loc, Start, End ) 
;
; Purpose 	Save the data contained in a driver data space
;
; Parameters	Vector - HP System vector number
;		First - starting address
;		Last - ending address
;		Loc - save pointer
;
; On Entry	es - HP saved Data segment
;		ds - Winoldap data
;
; On Exit	es, ds, si, di preserved
;
;--------------------------------------------------------------------------
cProc	SaveData, <NEAR,PUBLIC>, <es, ds, si, di>
	parmW	Vector
	parmW	First
	parmW	Last
	parmW	Loc
cBegin	SaveData	    

; move the data
	mov	bx, Vector    		;get describe record address
	mov	DS, CS:[HPVectorTable]
	mov	ds, ds:[bx+4]		
	mov	si, First
	mov	di, Loc			;load the destenation addres
	mov	cx, Last
	sub	cx, si	
	cld
	rep 	movsb

cEnd	SaveData


page
;---------------------------------------------------------------------------
;
; RestoreData( Vector, Loc, Start, End ) 
;
; Purpose 	Restore the data contained in a driver data space
;
; Parameters	SS= Winoldap Data segment
;		DS= Saved HP data area
;		Vector - HP System vector number
;		First - starting address
;		Last - ending address
;		Loc - save pointer
;
;--------------------------------------------------------------------------
cProc	RestoreData, <NEAR,PUBLIC>, <ES, SI, DI>
	parmW	Vector
	parmW	First
	parmW	Last
	parmW	Loc
cBegin	RestoreData	    

; find the HP vector table

	mov	es, CS:[HPVectorTable]

; move the data
	mov	bx, Vector    		;get describe record address
	mov	es, es:[bx+4]		; ES= device data segment
	mov	di, First
	mov	si, Loc
	mov	cx, Last
	sub	cx, di	
	cld
	rep 	movsb
	
cEnd	RestoreData



page 
;====== IsVectra ============================================================ 
;
; Determine if this is a Vectra A, A+, A++ with EX-BIOS resident.
;
; Entry:
;  DS:		Winoldap data
;
; Exit:
;   If it is a Vectra ax != 0 and zero flag cleared
;   
; Uses:
;
;
;==============================================================================
IsVectra	Proc	NEAR
;
;  Check if the PC is a Vectra. If Yes, then call HPSystem to get the
;  current size of the HP state

	push	ES			; Save it
	mov	AX, BIOS_SEG
	mov	ES, AX			; ES: Segment of ID code
	cmp	Word Ptr ES:[ID_OFFSET], 'PH'
	pop	ES			; Restore entry ES
	je	ivCheckBios
	jmp	ivNo			; Not a Vectra, so no extra HP processing
;
;  Check if EX-BIOS is present
;
ivCheckBios:	
	mov	AX, F16_INQUIRE
	mov	BX, AX			; set BX with out HP
	int	INT_KBD
	cmp	BX, 'HP'		; EX-BIOS present?
	je	ivYes			; No, finish
ivNo:	
	xor	ax, ax
	jmp	ivRet
ivYes:	
	xor	AX, AX
	inc	ax	
ivRet:	
	ret
IsVectra 	endp


	
page 
;====== FindVectra ============================================================ 
;
; Set fVectra flag if we have a Vectra 	A, A+, or A++ with EX-BIOS resident.
; Initialize HPSize accordingly.
;
; This code is only accessed once when the first context-switchable BAD app
; is executed. It is called from Loadgrab in TTYTASK
;
; Entry:
;  DS:		Winoldap data
;
; Exit:
;  fvectra, HPSize
;
; Uses:
;
;
;==============================================================================

FindVectra	Proc	NEAR
	call	IsVectra
	jz	FindVRet		; No, finish
	mov	CS:[fVectra], 1 	; Yes, flag we have a Vectra
	cmp	CS:[HPSize], 0		; First BAD appl ?
	jnz	FindVRet		; NO, so size is already determined
;
; Under Windows, it is ASSUMED that HPentry is always 6Fh so we can issue
; EX-BIOS calls safely.
;
	xor	AX, AX
	cCall	SaveHPSystem, <AX, AX>

;  AX:	Size of the HP state in Bytes
;  Assume DS= Winoldap data segment

	mov	CS:[HPSize], AX

FindVRet:
	ret
;
FindVectra	endp


page
;======= EnableVectra ========================================================
;
; If we have a Vectra A, A+, or A++ with EX-BIOS, save the current HPEntry
; vector, HPHIL state and set HPentry=6Fh, Turn ON HPHIL.
;
; Entry:
;   DS:		Winoldap DATA
;
;
; Exit:
;   CurHPentry, CurHILState
;
; Regs:
;   AX, BX
;
;=============================================================================

cproc	EnableVectra, <NEAR>

cBegin
	cmp	CS:[fVectra], 1
	jne	EnVret			; No special processing if not a vectra

	push	BP
	push	DS

;
; Save current HP entry and set it to be 6Fh
;
	mov	AX, F16_GET_INT_NUMBER
	int	INT_KBD
	mov	CS:[CurHPEntry], HPENTRY ; Assume we have HPentry= 6Fh
	cmp	AH, RS_UNSUPPORTED
	je	EnVectraA		; We have a Vectra A, A+ if unsupported
	mov	CS:[CurHPEntry], AH	; Save it if valid
;
	mov	BL, HPENTRY		;~~tqn 061287
	mov	AX, F16_SET_INT_NUMBER
	int	INT_KBD
;
; Save current HPHIL state and set it ON
;		  
EnVectraA:
	mov	AH, F_SYSTEM
	mov	AL, SF_REPORT_STATE
	mov	BP, V_HPHIL
	int	HPENTRY
;
; Bit 14 of BX (Status Word) = 1: HPHIL OFF
;			       0: 	ON
;
	mov	CS:[CurHILState], BH
;
; Turn HIL ON
;
	mov	AH, F_IO_CONTROL
	mov	AL, SF_HIL_ON
	mov	BP, V_HPHIL
	int	HPENTRY
	pop	DS
	pop	BP
;
EnVret:	
;
cEnd	EnableVectra


page
;======= DisableVectra =======================================================
;
; Restore the Vectra environment according to CurHPEntry and CurHILState
; Assume that HPENTRY is always 6Fh
;
; Entry:
;   DS:		Winoldap DATA
;
; Exit:
;
;
; Regs:
;   AX, BX
;
;=============================================================================

cProc	DisableVectra, <NEAR>

cBegin
	cmp	CS:[fVectra], 1
	jne	DisVRet
;
	push	BP
	push	DS
;
; Restore the HIL state according to CurHILState
;
	mov	AH, F_IO_CONTROL
	mov	BP, V_HPHIL
	mov	AL, SF_HIL_ON		;~~tqn 061287 Assume HIL is ON
	test	CS:[CurHILState], B_HIL_STATE
	je	DisVHIL			; 0= correct assumption
	mov	AL, SF_HIL_OFF		;~~tqn 061287

DisVHIL:
	int	HPENTRY
;
; Restore the Saved HPEntry
;
	mov	AX, F16_SET_INT_NUMBER
	mov	BL, CS:[CurHPEntry]
	int	INT_KBD
;
	pop	DS
	pop	BP

DisVRet:
;
cEnd	DisableVectra


sEnd	CODE

	END
