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
; Module Name:	EGASTATE.ASM
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
	page
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
; //	The hypothesis is that there is a method wherein the state
; //	of the EGA registers used by the Color EGA Drivers can be
; //	determined at interrupt time, with minimum limitations on
; //	the display driver.
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
; //	By following the prescribed algorithm, the following registers
; //	could be saved and restored at interrupt time:
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
; //
; //	    2)	Determine the state of the Map Mask Register.
; //		This is accomplished by:
; //
; //		A)  Write to two locations in EGA memory where the
; //		    bytes are known to be different in the same
; //		    plane.  These locations will be maintained by
; //		    the cursor code and EGA initialization code in
; //		    this state.
; //
; //		B)  If the bytes are the same for a given plane, then
; //		    that plane was enabled for writing.  If the bytes
; //		    are different for a given plane, then that plane
; //		    was not enabled for writing.
; //
; //		The Map Mask Register may now be altered.
; //
; //
; //	    3)	Save the EGA Processor Latches.  This is accomplished
; //		by:
; //
; //		    A)	Enabling all planes for writing.
; //
; //		    B)	Writing to a predefined location in video RAM
; //			in Write Mode 1.
; //
; //		This location (in all four planes) now contains the
; //		contents of the Processor Latches at the time of the
; //		interrupt.
; //
; //		The Processor Latches may now be altered.
; //
; //
; //	    4)	Determined the Read Map Select Register.  Since the
; //		EGA has already been placed into Data Read mode,
; //		simply read from a predefined location.  Each plane
; //		of this location has the plane index value in it.
; //		The value returned will be the Read Map Select index.
; //
; //		The Read Map Select Register may now be altered.
; //
; //
; //	    5)	Determine the state of the Bit Mask Register and the
; //		Data Rotate Register.  Since the EGA driver doesn't
; //		use the Rotate Count, only the boolean function of
; //		the Data Rotate register need be determined.  This is
; //		accomplished by the following:
; //
; //		    A)	Enable Write Mode 2.  In Write Mode 2, the
; //			Enable Set/Reset Register and the Rotate
; //			Count are ignored.  Any host data is taken to
; //			be a color, with 8 bits of D0 being written
; //			to C0, and so forth for the other planes.
; //
; //		    B)	Perform two writes (XCHGs) to EGA memory in
; //			a location where the planes are defined as
; //			follows:
; //
; //				C3 C2 C1
; //
; //				FF FF 00
; //
; //			The first write consists of writing 00 to
; //			plane C1.  The second write consists of
; //			writing FF to planes C2 and C3.
; //
; //		    C)	By interpreting the results, the value of the
; //			Bit Mask Register and the Data Rotate Register
; //			can be determined:
; //
; //				C3 C2 C1
; //
; //				FF FF 00     Bit Mask is 0
; //				FF 00 FF     Mode is SET
; //				FF FF FF     Mode is OR
; //				FF 00 00     Mode is AND
; //				00 FF FF     Mode is XOR
; //
; //			Anywhere that the result is the original value,
; //			the Bit Mask Register was 0.  If all bits show
; //			the original value, then the Bit Mask Register
; //			was 0 in all bits, and the test must be rerun
; //			with the Bit Mask enabled for at least one bit
; //			(this will be the case when copying from screen
; //			to screen).
; //
; //			Note that C0 never participates in the decision.
; //
; //			Any '1' bit in C1 indicates a bit which changed.
; //			Any '0' bit in C2 or C3 indicates a bit which
; //			changed.
; //
; //
; //	    6)	Determine the state of the Enable Set/Reset Register.
; //		This is done by the following:
; //
; //		    A)	Enable all bits of the Bit Mask Register.
; //
; //		    B)	Enable writing to all planes.
; //
; //		    C)	Set Write Mode 0.  Enable Set/Reset is
; //			recognized in Write Mode 0.
; //
; //		    D)	Set Data Rotate Function Select to SET.
; //
; //		    E)	Write 55h to a location.
; //
; //		    F)	Any plane of the location where the result
; //			isn't 55h was enabled for Set/Reset.
; //
; //	The state of the registers has now been determined.
;
; NULL save_hw_regs()
; {
;   perform steps 1 thru 6;
;   GRAF_ENAB_SR = all planes disabled;
;   return();
; }
;-----------------------------------------------------------------------;


;	The func_select table is used to translate the results of step
;	5C into the actual value for the Data Rotate register.	The
;	algorithm which scans the planes to determine the boolean
;	operation of the Data Rotate register returns 0 wherever a
;	changed occured in the table listed for step 5C.
;
;	Since C0 is never involved in the result, it is masked off
;	via a SHR instruction.


func_select     label   byte
        db      DR_SET                  ;000 - illegal
        db      DR_SET                  ;001 - illegal
        db      DR_XOR                  ;010 - XOR
        db      DR_SET                  ;011 - illegal
        db      DR_SET                  ;100 - SET
        db      DR_AND                  ;101 - AND
        db      DR_OR                   ;110 - OR
        db      DR_SET                  ;111 - illegal


	page
	assumes ds,Data
	assumes es,EGAMem


save_hw_regs    proc    near

ifdef	FIREWALLS
	int	3
	db	'missing firewall'
endif



;	Perform steps 1 and 2A for determining the value of the Map
;	Mask register.	The actual value of the Map Mask cannot be
;	computed until after the processor latches have been saved.
;	This step also sets Data Read mode which is required by the
;	Read Map detection code.


        mov     dx,EGA_BASE+GRAF_ADDR
	mov	ax,M_LATCH_WRITE shl 8 + GRAF_MODE
	out16	dx,ax
        mov     known_word,ax


;	Perform step 3 to save the current contents of the processor
;	latches.  Note that the Sequencer Address Register always has
;	the address of the Map Mask register in it, so the output
;	operation can be a single 8-bit I/O operation.


        mov     dl,SEQ_DATA
        mov     al,MM_ALL
        out     dx,al
        mov     saved_latches,al


;	Perform step 4 to determine and save the contents of the
;	Read Map register.


        mov     al,plane_index
        mov     saved_read_map,al


;	The result of the Bit Mask test will be ANDed into SAVED_BIT_MASK.
;	This allows the test loop to be repeated when the Bit Mask is all
;	zeros and still return the correct result (FF AND 00 AND ?? = 00).


        mov     saved_bit_mask,0FFh


	page
;	Perform steps 5A and 5B to set up for determining the value of
;	the Data Rotate register.


        mov     dl,GRAF_DATA            ;Graphics Controller still has
        mov     al,M_COLOR_WRITE        ;  address of Mode register
        out     dx,al

save_hw_regs_retry:
        mov     dl,SEQ_DATA
        mov     ax,MM_C0+MM_C2
        out     dx,al
        xchg    ah,enable_test
        errnz   <HIGH (MM_C0+MM_C2)>    ;AH must be zero
        mov     ax,0FF00h+MM_C1+MM_C3
        out     dx,al
        xchg    ah,enable_test


;	Perform step 5C to determine the boolean function set in the
;	Data Rotate register, and the value of the Bit Mask register.
;	If no bits are enabled in the Bit Mask register, then the
;	operation must be repeated with at least one bit of the Bit
;	Mask enabled.
;
;       Step 2B will also be performed within the same read loop.
;
;
;       The loop will use the following registers:
;
;               AX      work
;               BL      XOR test mask, Function Select mask
;               BH      Read Map Select
;               CH      accumulates Map Mask
;               CL      accumulates Bit Mask
;
;	The XOR test mask is XORed with the contents of the ENABLE_TEST
;	byte for the given plane.  The result of the XOR will give a 1
;	for any bit which changed (and thus was enabled in the bitmask)
;
;	The function select mask is used to accumulate planes where a
;	change took place so that Function Select can be determined via
;	a table lookup.  A 0 will be returned for each plane where a
;	change occured.


        mov     dl,GRAF_ADDR            ;Set Graphics Controller Addr Reg
        mov     al,GRAF_READ_MAP        ;  to Read Map Select Register
        out     dx,al
        inc     dx                      ;--> Graphics Controller Data Reg
	mov	bx,RM_C3 shl 8 + 0C0h	;BH = Read Map index and loop counter
                                        ;BL = the XOR mask
        xor     cx,cx                   ;Accumulates Map Mask and Bit Mask
	errnz	(RM_C3+1)-4		;Execute loop for all four planes

save_hw_next_plane:
        mov     al,bh                   ;Set Read Map Select
        out     dx,al
        mov     ax,known_word           ;Determine Map Mask for this plane
	xor	al,ah			;  Set 'C' if identical, indicating
	cmp	al,1			;    this plane was enabled
	rcl	ch,1			;  Save Map Enable for this plane
        mov     al,bl                   ;Get XOR test mask into AH
        cbw
        xor     ah,enable_test          ;AH = 1 wherever bit changed
        or      cl,ah                   ;Accumulate Bit Mask
        cmp     ah,1                    ;Want 'C' if no bits changed
        adc     bl,bl                   ;Accum function, set next XOR mask
        dec     bh                      ;Set next Read Map index
	jns	save_hw_next_plane	;More planes to process
        and     saved_bit_mask,cl       ;Save Bit Mask register value


;       Enable all bits of the Bit Mask in preparation of step 6.
;       This will also have to be done for retrying the boolean
;       function if the Bit Mask was 0, so this handles both cases.


	dec	dx			;--> Graphics Controller Addr reg
        mov     ax,0FF00h+GRAF_BIT_MASK ;Enable all bits for alteration
	out16	dx,ax

        or      cl,cl                   ;Retry if no bits enabled in Bit Mask
        jz      save_hw_regs_retry      ;  No bits, retry

;
;       Finish steps 2B and 5C.  To finish step 2B, save the computed
;	Map Mask value.  To finish step 5C, take the index computed for
;	the boolean function and perform a table lookup to determine
;	the actual value of the Data Rotate register.


        mov     saved_map_mask,ch       ;Finish 2B - Save Map Mask
	xor	bh,bh			;Finish 5C
	shr	bx,1			;  D0 is always 0, so the table
	mov	al,func_select[bx]	;    can be 8 bytes instead of 16
	mov	saved_data_rot,al	;  Save Data Rotate


;       Perform steps 6B through 6E to prepare for determining
;       Enable Set/Reset. Step 6A has already been performed.


        mov     dl,SEQ_DATA
        mov     al,MM_ALL
        out     dx,al

        mov     dl,GRAF_ADDR
	mov	ax,M_PROC_WRITE shl 8 + GRAF_MODE
	out16	dx,ax
        mov     bx,5501h                ;BH = pattern, BL = 1 for comparing

        mov     al,GRAF_DATA_ROT
;       mov     ah,DR_SET
        errnz   DR_SET-M_PROC_WRITE     ;Must be the same
	out16	dx,ax

        mov     enable_test,bh


;       Perform step 6F.  Any plane which didn't have the 55h written
;       in the location ENABLE_TEST is enabled for Set/Reset


        inc     ax                      ;Set the Graphics Controller Address
        out     dx,al                   ;  Register to point to the Read Map
        errnz   GRAF_READ_MAP-GRAF_DATA_ROT-1

        inc     dx                      ;--> Graphics Controller Data register
;       mov     ax,RM_C3                ;AH accumulates Enable mask
        dec     ax                      ;AL is Read Map and loop counter
        errnz   GRAF_READ_MAP-RM_C3-1

save_hw_regs_sr_loop:
        out     dx,al                   ;Set read plane
        mov     cl,enable_test          ;Set 'C' if byte isn't 55h
        xor     cl,bh
        cmp     cl,bl
        cmc
        adc     ah,ah                   ;Propagate 'C' as Enable bit
        dec     al                      ;Set next plane
        jns     save_hw_regs_sr_loop    ;More planes to test
	mov	saved_enab_sr,ah	;Save Enable Set/Reset

	dec	dx			;--> Graphics Controller Addr register
	mov	ax,GRAF_ENAB_SR 	;Disable any set/reset
	errnz	<HIGH GRAF_ENAB_SR>	;Must be zero
	out16	dx,ax


;       The state of the EGA has now been saved.

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
;   set_test_locs;			    // init locations for next time
;   SEQ_MAP_MASK	= saved_map_mask;   // restore saved registers
;   GRAF_READ_MAP	= saved_read_map;
;   GRAF_DATA_ROT	= saved_data_rot;
;   GRAF_ENAB_SR	= saved_enab_sr;
;   GRAF_MODE		= shadowed_mode[?]; // just use current plane
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

        call    set_test_locs

	mov	al,saved_map_mask
	out	dx,al
        mov     dl,GRAF_ADDR
	mov	ax,wptr saved_read_map[-1]
	out16	dx,ax
	mov	ax,wptr saved_data_rot[-1]
	out16	dx,ax
	mov	ax,wptr saved_enab_sr[-1]
	out16	dx,ax
        mov     al,GRAF_MODE
	mov	ah,shadowed_mode	;Get user's read and write mode
	out16	dx,ax
	mov	ax,wptr saved_bit_mask[-1]
	out16	dx,ax
        mov     al,saved_latches
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
;   set_test_locs;				    // init ega ram locations
;   plane_index[C3:C0]	= C3:C0;		    // set read map index
;   SEQ_MAP_MASK	= MM_ALL;		    // enable all planes
;   GRAF_ADDR		= GRAF_BIT_MASK;	    // --> to BIT MASK
;   shadowed_mode[C0:C3]= M_READ_MODE+M_PROC_WRITE; // init shadowed mode
;   return();					    //	 in all planes
; }
;-----------------------------------------------------------------------;


	assumes ds,Data
	assumes es,EGAMem

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

init_hw_regs	proc	far

ifdef	FIREWALLS
	int	3
	db	'missing firewall'
endif

        mov     dx,EGA_BASE+SEQ_ADDR    ;Set Sequencer Addr reg to point
        mov     al,SEQ_MAP_MASK         ;  to Map Mask register.  Only this
	out	dx,al			;  routine will do this

        call    set_test_locs           ;Initialize SAVE_HW_REGS locations

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

        ret

init_hw_regs	endp
	page
;----------------------------Private-Routine----------------------------;
; set_test_locs
;
; Set Test Locations for SAVE_HW_STATE
;
; This routine is called to initialize those locations of EGA memory
; use by SAVE_HW_REGS to determine the state of the EGA registers.
;
; Entry:
;       DS              = Data segment selector
;       ES              = EGA  memory  selector
;       GRAF_ENAB_SR    = all planes disabled           (EGA register)
;       GRAF_DATA_ROT   = DR_SET                        (EGA register)
;       GRAF_MODE       = M_DATA_READ+M_PROC_WRITE      (EGA register)
;       SEQ_MAP_MASK    = all planes enabled            (EGA register)
;	DH		= EGA_BASE
; Returns:
;       GRAF_ENAB_SR    = all planes disabled           (EGA register)
;       GRAF_DATA_ROT   = DR_SET                        (EGA register)
;       GRAF_MODE       = M_DATA_READ+M_PROC_WRITE      (EGA register)
;       SEQ_ADDR        = SEQ_MAP_MASK                  (EGA register)
;	DL		= SEQ_DATA			(EGA register)
; Error Returns:
;       none
; Registers Destroyed:
;	AX,BX
; Registers Preserved:
;	DH,SI,DI,BP,DS,ES
; Calls:
;       none
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; NULL near set_test_locs()
; {
;   known_word[C0:C3]  = 0x00FF;
;   enable_test[C0:C1] = 0x00;
;   enable_test[C2:C3] = 0xFF;
;   return();
; }
;-----------------------------------------------------------------------;


	assumes ds,Data
	assumes es,EGAMem


set_test_locs   proc    near

ifdef	FIREWALLS
	int	3
	db	'missing firewall'
endif

        mov     bx,00FFh                ;Set 00FF into all planes
	mov	known_word,bx		;  of a known word

	mov	dl,SEQ_DATA		;Set 00 into C0 and C1
	mov	al,MM_C0+MM_C1
        out     dx,al
        mov     enable_test,bh
        mov     al,MM_C2+MM_C3          ;Set FF into C2 and C3
        out     dx,al
        mov     enable_test,bl

	ret

set_test_locs   endp

sEnd	Code
if	MASMFLAGS and PUBDEFS
	public	saved_enab_sr
	public	saved_data_rot
	public	saved_read_map
	public	saved_bit_mask
	public	saved_map_mask
	public	init_hw_regs_10
endif
	end
