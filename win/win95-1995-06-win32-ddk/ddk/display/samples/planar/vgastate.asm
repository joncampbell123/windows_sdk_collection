;****************************************************************************
;                                                                           *
; THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
; KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
; IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
; PURPOSE.                                                                  *
;                                                                           *
; Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
;                                                                           *
;****************************************************************************

        page    ,132
;-----------------------------Module-Header-----------------------------;
; Module Name:	VGASTATE.ASM
;
; This file contains the pointer shape support routines required to be
; able to save and restore the EGA registers when a pointer shape needs
; to be drawn at interrupt time.
;
; Exported Functions:	none
;
; Public Functions:	save_hw_regs
;			res_hw_regs
;			init_hw_regs
;
; Public Data:		none
;
; General Description:
;
;	SAVE_HW_REGS is called by the pointer shape routine to save
;	the state of those EGA's registers which must be used to draw
;	the pointer shape.
;
;	RES_HW_REGS is called by the pointer shape routine to restore
;	the state of those EGA registers saved by SAVE_HW_REGS, and
;	to prepare for the next call to SAVE_HW_REGS.
;
;	INIT_HW_REGS is called immediately after the EGA is placed
;	into graphics mode to initialize locations in EGA memory that
;	are required by SAVE_HW_REGS.
;
; Restrictions:
;
;	These routines are intended to be executed while protected
;	with some form of a semephore.	The contents of both the
;	Graphics Controller Address Register and the Sequencer
;	Address Register are assumed to belong to these pieces of
;	code while executing, and assumed not to change unless done
;	so by these routines.
;
;	Win386 will not preempt us while we are in the driver.	They
;	will restore the EGA to the default state as defined in EGA.INC,
;	and save and restore the last 16 bytes of whichever 16K segment
;	the display memory ends on (3FF0 or 7FF0).
;
;	INIT_HW_REGS must be called before any call to either
;	SAVE_HW_REGS or RES_HW_REGS.
;
;	RES_HW_REGS must be called prior to the next call to
;	SAVE_HW_REGS or the detection code will fail.
;-----------------------------------------------------------------------;
       .xlist
        include cmacros.inc
	include macros.inc
	include	mflags.inc
        include ega.inc
	include egamem.inc
        .list

	??_out	egastate		;;Identify if not in quiet mode


	public	save_hw_regs
	public	res_hw_regs
	public	init_hw_regs

ifdef _BANK
	externNP SaveBankState
	externNP RestoreBankState
endif

;	Data register I/O addresses must follow the Address register
;	I/O addresses.

	errnz	GRAF_DATA-GRAF_ADDR-1
	errnz	SEQ_DATA-SEQ_ADDR-1


;	Don't want to explicitly define a read mode on every output to
;	the Graphics Controller's Mode Register.  The default should be
;	Data Read if not specified.

	errnz	M_DATA_READ


;	The ordering of the planes and their index values are assumed
;	to be the following.  It is unlikely that these will ever
;	change.

	errnz	MM_C3-00001000b 	;Map mask values
	errnz	MM_C2-00000100b
	errnz	MM_C1-00000010b
	errnz	MM_C0-00000001b

	errnz	RM_C0			;Read map values (sequential)
	errnz	RM_C1-RM_C0-1
	errnz	RM_C2-RM_C1-1
	errnz	RM_C3-RM_C2-1


	page
sBegin	Data

externB	do_int_30			;int 30 call necessary or not


;       The following locations are used to save and restore
;	the indicated EGA registers.


saved_registers label	word

		db	GRAF_ENAB_SR	;Enable Set/Reset register address
saved_enab_sr	db	?		;Computed Enable Set/Reset

		db	GRAF_DATA_ROT	;Data Rotate register address
saved_data_rot  db      ?               ;Computed Data Rotate function

                db      GRAF_READ_MAP   ;Read Map register address
saved_read_map  db      ?               ;Computed Read Map Select

                db      GRAF_BIT_MASK   ;Bit Mask register address
saved_bit_mask  db      ?               ;Computed Bit Mask

saved_map_mask  db      ?               ;Computed Map Mask

                db      GRAF_MODE       ;Mode Register address.
saved_mode      db      ?               ;Saved value of the Mode reg.

sEnd	Data

	page
sBegin	Code
	assumes cs,Code
;-----------------------------Public-Routine----------------------------;
; save_hw_regs
;
; Save Hardware Video Registers
;
; This routine is called by the pointer shape drawing code whenever
; the state of the EGA registers must be save.  The contents of the
; following registers are saved:
;
;       Map Mask
;       Enable Set/Reset
;       Data Rotate
;       Read Map Select
;       Bit Mask
;       Processor Latches
;
;
; The pointer shape drawing routine must call RES_HW_REGS to restore
; the registers and prepare the internal work areas for the next call
; to this routine.
;
; Entry:
;       DS              = Data segment selector
;       ES              = EGA  memory  selector
; Returns:
;       GRAF_MODE       = M_DATA_READ+M_PROC_WRITE      (EGA register)
;       GRAF_ENAB_SR    = all planes disabled           (EGA register)
;	GRAF_BIT_MASK	= all bits enabled		(EGA register)
;	GRAF_DATA_ROT	= DR_SET			(EGA register)
;       SEQ_MAP_MASK    = all planes enabled            (EGA register)
; Error Returns:
;       none
; Registers Destroyed:
;       AX,BX,CX,DX,FLAGS
;	GRAF_ADDR					(EGA register)
; Registers Preserved:
;       SI,DI,BP,DS,ES
;	SEQ_ADDR					(EGA register)
; Calls:
;       none
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; //
; //	New EGA Shadow Specification
; //
; //	The registers used by the current color drivers are:
; //
; //	    Sequencer:		    Map Mask
; //
; //	    Graphics Controller:    Set/Reset
; //				    Enable Set/Reset
; //				    Color Compare
; //				    Data Rotate
; //				    Read Map Select
; //				    Mode
; //				    Color Don't Care
; //				    Bit Mask
; //
; //
; //	Of these registers, the following are currently shadowed for
; //	the interrupt cursor drawing code:
; //
; //	    Sequencer:		    Map Mask
; //
; //	    Graphics Controller:    Enable Set/Reset
; //				    Data Rotate
; //				    Mode
; //				    Bit Mask
; //
; //
; //
;	The following registers are saved and resored at interrupt time
; //
; //	    Sequencer:		    Map Mask
; //
; //	    Graphics Controller:    Enable Set/Reset
; //				    Data Rotate
; //				    Read Map Select
; //				    Bit Mask
; //				    Set/Reset
; //				    Color Compare
; //	    Processor Latches
; //
; //
; //	The limitations proposed are:
; //
; //	    1)	The driver must indicate if it is in Data Read mode or
; //		Color Compare Read mode.  The driver must indicate the
; //		write mode in use.  These values are part of the same
; //		register, so only one register need be shadowed.
; //
; //
; //	The process:
; //
; //	    1)	Place the EGA into Write Mode 1.  In this mode, the
; //		contents of the EGA's latches are written to memory.
; //		Since the Read Mode is part of the same byte, it
; //		will be set to Data Read mode, which will be required
; //		by the state detection code.
; //
; //	    2)  The other registers are directly read off from the ports
; //       
; //        3)  Program VGA for READ MODE 0 and WRITE MODE 0
; //
; //	The state of the registers has now been determined.
;
; NULL save_hw_regs()
; {
;   perform steps 1 thru 3;
;   GRAF_ENAB_SR = all planes disabled;
;   return();
; }
;-----------------------------------------------------------------------;
	page
	assumes ds,Data
	assumes es,EGAMem


save_hw_regs    proc    near

ifdef	FIREWALLS
	int	3
	db	'missing firewall'
endif

;	Perform step 1.

        mov     dx,EGA_BASE+GRAF_ADDR

; read the mode register.

	mov	al,GRAF_MODE
	out	dx,al
	inc	dx
	in	al,dx
	mov	saved_mode,al
	dec	dx
	mov	ax,M_LATCH_WRITE shl 8 + GRAF_MODE
	out16	dx,ax

;	Perform step 1 to save the current contents of the processor
;	latches.  Note that the Sequencer Address Register always has
;	the address of the Map Mask register in it, so the output
;	operation can be a single 8-bit I/O operation.

; get the map mask register value first and then set it to MM_ALL

	mov	dl,SEQ_ADDR		; DX --> Sequencer Index register
	mov	al,02h			; (fix for Chicago bug with Borland's
	out	dx,al			; Turbo Debugger v3.0)
	inc	dl			; DX --> Sequencer Data register
	in	al,dx			; get the current value
	mov	saved_map_mask,al	; save it
        mov     al,MM_ALL
        out     dx,al
        mov     saved_latches,al

; save the sequencer map mask value

	dec	dx		
	mov	dl,GRAF_ADDR

; read the read map register.

	mov	al,GRAF_READ_MAP
	out	dx,al
	inc	dx
	in	al,dx
        mov     saved_read_map,al
	dec	dx

; read the enable_sr register and set to disable all planes

	mov	al,GRAF_ENAB_SR		
	out	dx,al			; set up the index
	inc	dx			; point to the data port
	in	al,dx
	mov	saved_enab_sr,al	; save the value
	xor	al,al			; disable all planes for set/reset
	out	dx,al
	dec	dx			; restore GRX address register value

; read the data_rot register and set it to DR_SET

	mov	al,GRAF_DATA_ROT	
	out	dx,al			; set up the index
	inc	dx
	in	al,dx			; get the value
	mov	saved_data_rot,al	; save it
	mov	al,DR_SET
	out	dx,al			; set it to the return value
	dec	dx			; restore DX

; read bit mask register and then enable all bits

	mov	al,GRAF_BIT_MASK
	out	dx,al			; set up the index
	inc	dx
	in	al,dx			; get the value
	mov	saved_bit_mask,al	; save the value
	mov	al,0ffh			; enable all bits
	out	dx,al
	dec	dx			; restore DX

; program the mode register to be write mode 0, read mode 0

	mov	al,GRAF_MODE
	out	dx,al
	inc	dx
	mov	al,(M_DATA_READ + M_PROC_WRITE)
	out	dx,al

; The state of the EGA has now been saved.

ifdef _BANK	
	push	dx
	call	SaveBankState
	pop	dx
endif
        ret

save_hw_regs   endp
	page
;-----------------------------Public-Routine----------------------------;
; res_hw_regs
;
; Restore Hardware Video Registers
;
; This routine is called by the pointer shape drawing code whenever
; the state of the EGA registers is to be restored.  The contents of
; the following registers are restored:
;
;       Map Mask
;       Enable Set/Reset
;       Data Rotate
;       Read Map Select
;       Bit Mask
;       Mode
;       Processor Latches
;
; The pointer shape drawing routine must have previously called
; SAVE_HW_REGS to save the registers in the work areas from which
; this routine restores them.
;
; The internal work areas used by SAVE_HW_REGS will be reset for the
; next call to SAVE_HW_REGS.
;
; Entry:
;       DS              = Data segment selector
;       ES              = EGA  memory  selector
;       GRAF_MODE       = M_DATA_READ+M_PROC_WRITE      (EGA register)
;       GRAF_ENAB_SR    = all planes disabled           (EGA register)
;       SEQ_ADDR        = SEQ_MAP_MASK                  (EGA register)
;       SEQ_MAP_MASK    = all planes enabled            (EGA register)
; Returns:
;	GRAF_ADDR	= GRAF_BIT_MASK 		(EGA register)
; Error Returns:
;       none
; Registers Destroyed:
;       AX,BX,DX,FLAGS
; Registers Preserved:
;       SI,DI,BP,DS,ES
;	SEQ_ADDR					(EGA register)
; Calls:
;       set_proc_locs
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; NULL res_hw_regs()
; {
;   GRAF_DATA_ROT	= DR_SET;	    // must use DR_SET mode
;   GRAF_BIT_MASK	= 0xFF; 	    // must alter all bits
;   SEQ_MAP_MASK	= saved_map_mask;   // restore saved registers
;   GRAF_READ_MAP	= saved_read_map;
;   GRAF_DATA_ROT	= saved_data_rot;
;   GRAF_ENAB_SR	= saved_enab_sr;
;   GRAF_MODE		= saved_mode;      
;   GRAF_BIT_MASK	= saved_bit_mask;   // must be last written
;   return();				    //	 to GRAF_ADDR
; }
;-----------------------------------------------------------------------;


	assumes ds,Data
	assumes es,EGAMem


res_hw_regs     proc    near

ifdef	FIREWALLS
	int	3
	db	'missing firewall'
endif

        mov     dx,EGA_BASE + GRAF_ADDR
	mov	ax,DR_SET shl 8 + GRAF_DATA_ROT
	out16	dx,ax
        mov     ax,0FF00h+GRAF_BIT_MASK
	out16	dx,ax


	mov	dl,SEQ_ADDR		; DX --> Sequencer Index register
	mov	al,02h			; (fix for Chicago bug with Borland's
	out	dx,al			; Turbo Debugger v3.0)
	inc	dl			; DX --> Sequencer Data register
	mov	al,saved_map_mask
	out	dx,al

        mov     dl,GRAF_ADDR
	mov	ax,wptr saved_read_map[-1]
	out16	dx,ax
	mov	ax,wptr saved_data_rot[-1]
	out16	dx,ax
	mov	ax,wptr saved_enab_sr[-1]
	out16	dx,ax
	mov	ax,wptr saved_mode[-1]
	out16	dx,ax
	mov	ax,wptr saved_bit_mask[-1]
	out16	dx,ax
        mov     al,saved_latches

ifdef _BANK	
	push	dx
	call	RestoreBankState
	pop	dx
endif
        ret

res_hw_regs     endp
	page
;-----------------------------Public-Routine----------------------------;
; init_hw_regs
;
; Initialize Hardware Video Registers
;
; This routine is called at display initialization time to initialize
; the state required to save and restore the EGA's registers and
; processor latches.
;
; The default EGA state assumed by the rest of the display driver
; code is also initialized.
;
; This code is intended to be called immediately after the EGA has
; been programmed for graphics mode and the palette registers set.
;
; Entry:
;       DS              = Data segment selector
;       ES              = EGA  memory  selector
;
;       The following are set when graphics mode is entered:
;
;       GRAF_ENAB_SR    = all planes disabled           (EGA register)
;       GRAF_DATA_ROT   = DR_SET                        (EGA register)
;       GRAF_MODE       = M_DATA_READ+M_PROC_WRITE      (EGA register)
;       SEQ_MAP_MASK    = all planes enabled            (EGA register)
; Returns:
;	shadowed_mode[C0:C3]= M_DATA_READ+M_PROC_WRITE	(EGA shadow)
;	GRAF_ENAB_SR	    = all planes disabled	(EGA register)
;	GRAF_DATA_ROT	    = DR_SET			(EGA register)
;	GRAF_MODE	    = M_DATA_READ+M_PROC_WRITE	(EGA register)
;	GRAF_BIT_MASK	    = All bits enabled		(EGA register)
;	GRAF_ADDR	    = GRAF_BIT_MASK		(EGA register)
;	SEQ_MAP_MASK	    = all planes enabled	(EGA register)
;	SEQ_ADDR	    = SEQ_MAP_MASK		(EGA register)
; Error Returns:
;       none
; Registers Destroyed:
;       AX,BX,DX,FLAGS
; Registers Preserved:
;       SI,DI,BP,DS,ES
; Calls:
;       none
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; NULL far init_hw_regs()
; {
;   SEQ_ADDR		= SEQ_MAP_MASK; 	    // all code assumes this
;   plane_index[C3:C0]	= C3:C0;		    // set read map index
;   SEQ_MAP_MASK	= MM_ALL;		    // enable all planes
;   GRAF_ADDR		= GRAF_BIT_MASK;	    // --> to BIT MASK
;   shadowed_mode[C0:C3]= M_READ_MODE+M_PROC_WRITE; // init shadowed mode
;   return();					    //	 in all planes
; }
;-----------------------------------------------------------------------;

if 0
;----------------------------------------------------------------------------;
; the following macro is used to do a INT 30 before an out to let kernel in  ;
; protected mode what value I am setting.				     ;
;----------------------------------------------------------------------------;

pout	macro	rdx,reg
	local	pout_normal

	cmp	do_int_30,1		;;is int 30 call needed ?
	jnz	pout_normal		;;no
	dw	__virtualize_io		
pout_normal:
	out	rdx,reg
	endm
endif

;----------------------------------------------------------------------------;


	assumes ds,Data
	assumes es,EGAMem


init_hw_regs	proc	far

ifdef	FIREWALLS
	int	3
	db	'missing firewall'
endif

        mov     dx,EGA_BASE+SEQ_ADDR    ;Set Sequencer Addr reg to point
	mov	al,SEQ_MAP_MASK 	;  to Map Mask register.
	out	dx,al			;
	inc	dx			; dx -> SEQ_DATA

	mov	ax,RM_C3 shl 8 + MM_C3	;Set Read Map index into each plane.
        errnz   MM_C3-00001000b         ;Must execute loop four times

init_hw_regs_10:
	out	dx,al			;Select plane
        mov     plane_index,ah          ;Set plane's index into the plane
        dec     ah                      ;Show next plane
        shr     al,1                    ;Set next Map Enable mask
	jnz	init_hw_regs_10 	;More planes to deal with

        mov     al,MM_ALL               ;Set Map Mask to all planes
	out	dx,al

        mov     dl,GRAF_ADDR            ;Leave the Graphics Controller
        mov     al,GRAF_BIT_MASK        ;  Address register set to the
	out	dx,al			;  Bit Mask

;	Set the shadowed Mode Register to be DATA_READ and PROCESSOR
;	WRITE.	This value must be shadowed in all planes.

	mov	shadowed_mode,M_DATA_READ+M_PROC_WRITE
	mov	saved_mode,M_DATA_READ+M_PROC_WRITE

        ret

init_hw_regs	endp
if 	MASMFLAGS and PUBDEFS
	public	saved_enab_sr
	public	saved_data_rot
	public	saved_read_map
	public	saved_bit_mask
	public	saved_map_mask
	public	init_hw_regs_10
endif
sEnd	Code
	end
