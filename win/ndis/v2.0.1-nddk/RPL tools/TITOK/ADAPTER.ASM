		PAGE  60, 132
		TITLE Token-Ring RIPL Boot ROM Driver - ADAPTER.ASM
;**********************************************************************;
;**********************************************************************;
;								       ;
; File Name:       ADAPTER.ASM     				       ;
;								       ;
; Program Name:    Token-Ring RIPL Boot ROM Device Driver              ;
;								       ;
; Companion Files: None						       ;
;								       ;
; Function:        This module contains the adapter dependent initial- ;
;                  ization code.  Therefore, all code in this module   ;
;		   is located in the INITCODE segment.  The following  ;
;                  routines are defined in this module:                ;
;								       ;
;		 - Find_Card: Find and reset adapter                   ;   
;		 - ResetAdapter: Reset the adapter		       ;
;		 - InitAdapter: Initialize the adapter		       ;
;								       ;
; History:             						       ;    		
;								       ;
;     03/17/89  Mike Witkowski - Original Version 0.0		       ;
;     02/27/90  Mike Witkowski - Reworked for RIPL                     ;
;								       ;
;**********************************************************************;
;**********************************************************************;

;-------------------------------------;
; Include all general companion files ;
;-------------------------------------;
 					   
		.XLIST
	       	INCLUDE includes.inc  	
		.LIST

PAGE_BREAK <External References>
;-----------------------------------;
; External Data/Variable References ;
;-----------------------------------;

                EXTRN   adp_stuff:        WORD
		EXTRN	gbl_init_blk:	  WORD
		EXTRN	DLCNetAddress:	  BYTE
                EXTRN   gbl_int_dos:      DWORD        
		EXTRN	gbl_initflag:     WORD

;--------------------------;
; External Code References ;
;--------------------------;
      		EXTRN	VirttoPhys:	  NEAR
		EXTRN	interrupt:        NEAR


PAGE_BREAK <Find_Card Routine Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  Find_Card Routine                                    ;
;                   					               ;
;  Description: The Find_Card routine is responsible for determining   ;
;               if a card is installed and, if so, resetting it.  We   ;
;               also set the IRQ and interrupt vector, and we setup    ;
;               the DMA controller.                                    ;
;                                                                      ;
;  Input:       DS:BX point to the parent MCB of the adapter           ;
;               CS    points to TRNDD data segment                     ;
;                                                                      ;
;  Output:      Update MCB ACB_state to reflect adapter state, IRQ and ;
;               DMA controllers are also adjusted if card is OK.       ;
;								       ;
;  Calls:       ResetAdapter, VirttoPhys, Board_Test, InitAdapter      ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

START_CODE

		PUBLIC	Find_Card
Find_Card  	PROC 	NEAR

		PUSHREG	bp, bx, cx, dx, di, si	; Save important regs
                
                mov     di, [bx].acb_adpnum            ; Get adapter number
		mov	dx, cs:adp_stuff[di].adp_base  ; Get card base address

                call    Board_Test              ; Test if adapter is present
                LJNZ    find_exit               ;   No, exit

		PAGE	
;----------------------------------------------------------------------;
;  Call the configuration set up routine.  Then, check to see if       ;
;  the adapter is an Eagle based adapter.  If its an Eagle adapter, we :
;  must adjust the open parameters if 16Mbs operation is desired.      ;
;----------------------------------------------------------------------; 
		mov	bp, [bx].acb_adpnum	; Save adapter number

                IFCALL  <sc_>                   ; Call config routine
                LJC     find_exit               ; If error, exit


IFDEF CHIP_EAGLE
                cmp     cs:adp_stuff[bp].adp_speed, 16
                                                ; Is media speed = 16Mbs ?
                SJNE    reset_con               ;   No, continue

                mov     WORD PTR [bx].acb_opn_blk.OPEN_Xbufmin, 1919h
                                                ; Set new adap buffer counts

                mov     WORD PTR [bx].acb_opn_blk.OPEN_RAMStart, 0000h
                mov     WORD PTR [bx].acb_opn_blk.OPEN_RAMEnd, 0ffffh
                                                ; Set memory size to maximum
ENDIF


;
;  Now let's do a hardware reset to the adapter to make sure its in 
;  a state that we like.  If there is an error, set CARDERROR state, 
;  else, set the RESET state. 
; 
                xor     ax, ax                  ; Request a hard reset
		call	ResetAdapter		; Reset the adapter		


reset_skip:
		mov	dx, [bx].acb_sif_int	; Set SIF interrupt reg
 	        mov	cx, 0ffffh 		; Set max count

bud_wait1:	in	al, dx			; Read SIF interrupt reg
		and	al, 0f0h		; Clear low nibble
		cmp	al, 020h		; Has adapter diags begun ?
		SJAE	bud_wait2		;   Yes, wait to complete 
		loop	bud_wait1		;   No, try again

card_failed:   	mov	[bx].acb_state, AS_CARDERROR  ; Set card error
		LJMP	find_exit		      ; Exit routine

bud_wait2:	in	al, dx			; Read SIF interrupt reg
		and	al, 0f0h		; Clear low nibble
		cmp	al, 030h		; Has adapter cmpltd diags ?
		SJB	bud_wait2		;   No, wait to complete 
			
		cmp	al, 040h		; Is board OK ?
		SJNE	card_failed		;   No, set error

		mov     [bx].acb_state, AS_RESET	; Set card reset

		PAGE

;
;  If everything went OK on reset, let's setup the IRQ and interrupt
;  service routine.
;
		mov 	ax, bx			; Save MCB address
		mov	di, [bx].acb_adpnum	        ; Get adapter number
		mov	bx, cs:adp_stuff[di].adp_int	; Get interrupt level

                cmp     bx, 2                   ; Is interrupt level = 2 ?
                SJNE    int_adj_skip            ;   No, skip this

                mov     bx, 9                           ; Set interrupt 
                mov     cs:adp_stuff[di].adp_int, bx    ;   level 9

int_adj_skip:
		mov 	di, ax			; Set MCB address

                IFCALL  <irq_>                  ; Call IRQ processing
                LJC     setirq_err              ; If error, return

                cmp     gbl_initflag, 0         ; Has driver been init'ed ?
                LJNE    set_dma                 ;   Yes, skip this

                push    ax                      ; Save level/edge select
		
		mov	dx, INT_CTRL1_MR	; Set int controller #1
		mov	cx, bx			; Save int level
		cmp	cl, 7			; We on right controller ?
		SJBE	ctrler_ok		;   Yep, continue 

		mov	dx, INT_CTRL2_MR	; Set int controller #2
		sub 	cl, 8			; Adjust int level offset
		
ctrler_ok:	mov	ah, 11111110b		; Make IRQ mask
		rol	ah, cl			; Put enable bit in right

		mov	[di].acb_int_mask, ah	; Save interrupt enable mask

                pop     cx                      ; Restore edge level select
                or      cx, cx                  ; Is trigger selected ?
                SJZ     set_int_val             ;   No, continue

                push    ax                      ; Save enable mask
                push    dx                      ; Save int controller

                mov     ch, cl                  ; Save edge/level flag
                mov     cl, ah                  ; Load int mask value
		mov	dx, INT_CTRL1_ELR	; Set int controller #1
                cmp     bx, 8                   ; Is int on 1st controller ?
                SJB     ctrlr_ok2               ;   Yes, continue
		mov	dx, INT_CTRL2_ELR	; Set int controller #2
                
ctrlr_ok2:
                in      al, dx                  ; Get edge/level settings
                and     al, cl                  ; Set to edge trigger 
                cmp     ch, 1                   ; Was it edge trigger select ?
                SJE     set_trig_val            ;   Yes, it done
                not     cl                      ; Invert mask
                or      al, cl                  ; Set level trigger

set_trig_val:
                out     dx, al                  ; Set edge/level settings

                cmp     ch, 1                   ; Was it edge trigger select ?
                SJE     set_trig_done           ;   Yes, don't modify mask
                mov     dx, [di].acb_adp_cfg    ; Get config register
                in      ax, dx                  ; Read it
                and     ax, JCFG_INTS           ; Isolate int bits
                mov     [di].acb_int_mask, ah   ; Save them

set_trig_done:
                pop     dx                      ; Restore int controller
                pop     ax                      ; Restore int enable mask
                
set_int_val:

                push    ax                      ; Save enable mask
                push    dx                      ; Save int controller

		mov	al, bl			; Get IRQ number
                cmp     al, 9                   ; Is it level 9 ?
                SJNE    int_lvl_ok              ;   No, skip this
                
                mov     al, 2                   ; Make it level 2

int_lvl_ok:
                add     al, 08h                 ; Point to correct INT
                cmp     al, 10h                 ; Is it below INT 10h ?
                SJB     int_set                 ;   Yes, go set it up

                add     al, 60h                 ; Point to correct INT

int_set:
                push    ax                      ; Save INT number

                xor     bx, bx                  ; Clear target register
                mov     bl, al                  ; Copy INT vector 
                shl     bx, 1                   ; Point to vector in memory
                shl     bx, 1                   ; Point to vector in memory

                xor     ax, ax                  ; Point to int vector memory
                mov     es, ax                  ;

                mov     ax, es:[bx].offp                ; Save old vector
                mov     WORD PTR cs:gbl_int_dos, ax     ; 
                mov     ax, es:[bx].segp                ; 
                mov     WORD PTR cs:gbl_int_dos+2, ax   ;

                pop     ax                      ; Restore INT number

                mov     es:[bx].offp, OFFSET CGROUP:interrupt
                mov     es:[bx].segp, cs        ; Set new interrupt vector

                pop     dx                      ; Restore int controller
                pop     ax                      ; Restore int enable mask
		in	al, dx	   		; Get current IRQs
		and	al, ah			; Add in our's
		out	dx, al			; Put it back into 8259
                SJMP    set_dma                 ; Set up DMA

setirq_err:
		mov	[di].acb_state, AS_IRQERROR  ; Set IRQ error
		LJMP	find_exit   	             ; Exit routine
                

;
;  Now we're ready to set up the DMA controller.
;
set_dma:
                xor     al, al                  ; DEBUG DEBUG DEBUG
		out	DMA_CTRL_WSMB2, al	;

                mov     bx, di                  ; Restore MCB pointer   
		mov	al, 0c0h         	; Set DMA cascade mode			
		mov	di, [bx].acb_adpnum     ; Get adapter number

		mov	ah, BYTE PTR cs:adp_stuff[di].adp_dma
                                                ; Get DMA level

                IFCALL  <dma_>                  ; Set up DMA/INTS
                SJNC    dma_ok                  ; If error, return
                
		mov	[bx].acb_state, AS_DMAERROR  ; Set DMA error
		LJMP	find_exit		     ; Exit routine


dma_ok:
                cmp     gbl_initflag, 0         ; Has driver been init'ed ?
                SJNE    init_adp                ;   Yes, skip this

                cmp     ah, 4                   ; Is DMA on 1st controller ?                
                SJAE    dma_ok2                 ;   No, its on second 


dma_ok1:
		add	al, ah			; Set DMA channel number
		out	DMA_CTRL_MODE1, al	; Write DMA mode
		xchg	ah, al			
		out	DMA_CTRL_WSMB1, al	; Set channel enable
		SJMP    init_adp 		; Continue processing

dma_ok2:
		sub	ah, 4	   		; Adjust DMA level  
		add	al, ah			; Set DMA channel number
		out	DMA_CTRL_MODE2, al	; Write DMA mode
		xchg	ah, al			
		out	DMA_CTRL_WSMB2, al	; Set channel enable

		PAGE
;
;  Now it is time to initialize the adapter. 
;
init_adp:
		call    InitAdapter		; Start adapter initialize

		mov	dx, [bx].acb_sif_int	; Set SIF int register

init_wait:			 		
		in	ax, dx			; Read SIFINT register
		and	ax, 00ffh	        ; Look at only LSB
		cmp	ax, 0040h		; Is initialization done
		SJE	init_wait		;   No, try again

		or	ax, ax			; Is there a good completion?
		SJE     init_good		;   Yes, continue normally

		mov	[bx].acb_state, AS_INITERROR	; Set card error
		SJMP	find_exit			; Return to caller	

init_good:
		mov	[bx].acb_state, AS_INTTEST	; Set interrupt test


;
;  Now, set up the open table options for this adapter for early 
;  token release if its an Eagle-based adapter and operating at 
;  16Mbs.
;
                mov     cx, NOT OOPTS_ETR               ; Set ~ETR mask

IFDEF CHIP_EAGLE
                mov     di, [bx].acb_adpnum             ; Get adapter number
                cmp     cs:adp_stuff[di].adp_speed, 16  ; Is speed = 16Mbs ?
                SJNE    no_etr                          ;   No, clear ETR
                or      cx, OOPTS_ETR                   ; Set ETR enable
no_etr:
ENDIF

                and     [bx].acb_opn_blk.OPEN_Options, cx
                                                ; Set ETR option
        
;
;  Now it is time to find out what the burned-in address of the board
;  is.  This done differently for each type of adapter. 
;
                IFCALL  <bia_>                  ; Call read BIA routine


;
;  Exit from the routine by restoring the registers that we destroyed.
;
find_exit:
		POPREG	bp, bx, cx, dx, di, si	; Restore saved regs
		ret

Find_Card	ENDP


PAGE_BREAK <Board Test Routine Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  Board Test Routine                                   ;
;                   					               ;
;  Description: The Board Test Routine performs a standard test to     ;
;               verify that one of the supported adapters is present   ;
;               in the system at the port address specified.           ;
;                                                                      ;
;  Input:       BX - pointer to MCB                                    ;
;               DX - base address of SIF (TMS380 chip set)             ; 
;               SI - Adapter Type                                      ;
;                                                                      ;
;  Output:      AX - 0 = board found, non-0 = board not found          ;
;		     (zero flag)                  		       ;
;								       ;
;  Calls:       Register Setup routines                                ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

Board_Test	PROC	NEAR	

		PUSHREG	cx, dx, di		; Save registers

;
;  First, let's make the call to the appropriate register set up
;  routine for this adapter. 
;
                lea     di, [bx].acb_adp_cfg    ; Point to reg locations
                IFCALL  <sr_>                   ; Call reg setup routine                    
                SJNC    bt_ok                   ; If OK, then proceed
                mov     ax, 1                   ; Make a non-zero flag
                or      ax, ax                  ;
                SJMP    bt_exit                 ; Exit with error


;
;  Now lets perform the actual adapter detection test. 
;  
bt_ok:
		mov	dx, [bx].acb_sif_addr	; Point to SIF address reg
		mov	ax, 0200h		; Set arbritrary address
		out	dx, ax			; Load it into SIF_ADDR
		add	ax, 2			; Inc address by 2
		push	ax			; Save the test address
                push	dx			; Save SIF_ADDR				
		mov     dx, [bx].acb_sif_dinc	; Point to data inc reg
		in	ax, dx			; Read the data inc reg
						;   This should cause the
						;   SIF_ADDR reg to inc X 2
		pop	dx			; Restore SIF_ADDR
		in	ax, dx			; Get new address in SIF_ADDR
		and	ax, 07ffh		; Clear bits we can't set
		pop	cx			; Restore test address
		xor	ax, cx			; They should the same


bt_exit:
		POPREG	cx, dx, di		; Restore registers
		ret


Board_Test	ENDP


IFDEF TYPE_WDTCPLUS
PAGE_BREAK <WD TokenCard Plus Register Set Up Routine Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  WD TokenCard Plus Register Set Up Routine            ;
;                   					               ;
;  Description: The WD TokenCard Plus Register Set Up Routine sets up  ;
;               the register locations in the MCB and makes sure the   ;
;               WD TokenCard Plus board is in the proper state for the ;
;               board test.  This routine uses most of the P1345 code. ;
;               Also, this routine modifies the open parameter block   ;
;               to make on card memory from 1006h to bffeh             ;
;                                                                      ;
;  Input:       BX - pointer to MCB                                    ;
;               DX - base address of SIF (TMS380 chip set)             ; 
;               SI - Adapter Type                                      ;
;                                                                      ;
;  Output:      Modifies MCB register fields                           ;
;								       ;
;  Calls:       none                                                   ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

sr_wdtcplus	PROC	NEAR	
;
;  Modify the open parameter block to make memory from 1006h to bffeh
;  (as opposed to the default 4006h to 7ffeh), because this card has
;  44K of RAM on board that we might as well use.  Everything else 
;  works just like a proteon P1345 so we'll just fall into that code.
;
                mov     [bx].acb_opn_blk.OPEN_RamStart, 0610h
                mov     [bx].acb_opn_blk.OPEN_RamEnd, 0febfh
                                                ; Set new memory addresses
                
sr_wdtcplus     ENDP
ENDIF


IFDEF TYPE_PWD
PAGE_BREAK <P1345 Register Set Up Routine Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  P1345 Register Set Up Routine                        ;
;                   					               ;
;  Description: The P1345 Register Set Up Routine sets up the register ;
;               locations in the MCB and makes sure the P1345 board is ;
;               in the proper state for the board test.                ;
;                                                                      ;
;  Input:       BX - pointer to MCB                                    ;
;               DX - base address of SIF (TMS380 chip set)             ; 
;               SI - Adapter Type                                      ;
;                                                                      ;
;  Output:      Modifies MCB register fields                           ;
;								       ;
;  Calls:       none                                                   ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

sr_p1345	PROC	NEAR	
;
;  First, set up the configuration register.
;
                push    es                      ; Save driver data segment
                push    ds                      ; Make ES = DS
                pop     es                      ;

                mov     ax, dx                  ; Copy the base address
                add     ax, 9                   ; Point to config register
                stosw                           ; Store config's address
                mov     ax, dx                  ; Copy the base address


;
;  Now it is time to set up the register locations in the MCB.  We do
;  this by branching to the 3COM set up code.
;
                mov     cx, NUMREGS             ; Set number of registers
                SJMP    sr3_setloop             ; Set up register locations

sr_p1345        ENDP
ENDIF


PAGE_BREAK <3C603 Register Set Up Routine Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  3C603 Register Set Up Routine                        ;
;                   					               ;
;  Description: The 3C603 Register Set Up Routine sets up the register ;
;               locations in the MCB and makes sure the 3C603 board is ;
;               in the proper state for the board test.                ;
;                                                                      ;
;  Input:       BX - pointer to MCB                                    ;
;               DX - base address of SIF (TMS380 chip set)             ; 
;               SI - Adapter Type                                      ;
;                                                                      ;
;  Output:      Modifies MCB register fields                           ;
;								       ;
;  Calls:       none                                                   ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

sr_3c603	PROC	NEAR	
;
;  First, set up the configuration register and make sure the adapter
;  is not in a reset state. 
;
IFDEF TYPE_3C603
                push    es                      ; Save driver data segment
                push    ds                      ; Make ES = DS
                pop     es                      ;

                mov     ax, dx                  ; Copy base address
                add     ax, 6                   ; Point to config register
                mov     cx, NUMREGS + 1         ; Set number of registers


;
;  Before setting register addresses, be sure shadow bit is set so we 
;  can see the TMS380 SIF registers. 
;
                push    dx                      ; Save base address
                push    ax                      ; Save control address
                mov     dx, ax                  ; Point to control register
                mov     ax, TCFG_SHADOW         ; Set SHADOW bit    
                out     dx, ax                  ; 
                pop     ax                      ; Restore control address
                pop     dx                      ; Restore base address
ENDIF


;
;  Now, set up the register locations in the MCB. 
;
sr3_setloop:
                stosw                           ; Copy register address
                add     ax, 2                   ; Point to next register
                loop    sr3_setloop             ; Copy next address

                pop     es                      ; Restore driver data seg            
                clc                             ; Set good return code
                ret                             ; Return to caller

sr_3c603        ENDP


IFDEF TYPE_M8110
PAGE_BREAK <M8110 Register Set Up Routine Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  M8110 Register Set Up Routine                        ;
;                   					               ;
;  Description: The M8110 Register Set Up Routine sets up the register ;
;               locations in the MCB and makes sure the M8110 board is ;
;               in the proper state for the board test.                ;
;                                                                      ;
;  Input:       BX - pointer to MCB                                    ;
;               DX - base address of SIF (TMS380 chip set)             ; 
;               SI - Adapter Type                                      ;
;                                                                      ;
;  Output:      Modifies MCB register fields                           ;
;								       ;
;  Calls:       none                                                   ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

sr_m8110	PROC	NEAR	
;
;  First, set up the reset register.  We must also make sure that 
;  the Racore card has its reset released since its power up state
;  in reset active. 
;
                push    es                      ; Save driver data segment
                push    ds                      ; Make ES = DS
                pop     es                      ;

                mov     ax, dx                  ; Copy the base address
                add     ax, 0eh                 ; Point to reset register
                stosw                           ; Store reset's address
                mov     cx, dx                  ; Copy base address
                mov     dx, ax                  ; Point to "clear reset" reg
                in      al, dx                  ; Make sure reset is released
                mov     ax, cx                  ; Copy the base address
                

;
;  Now it is time to set up the register locations in the MCB.  We do
;  this by branching to the 3COM set up code.
;
                mov     cx, NUMREGS             ; Set number of registers
                SJMP    sr3_setloop             ; Set up register locations

sr_m8110        ENDP
ENDIF

PAGE_BREAK <P1345 Configuration Set Up Routine Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  P1345 Configuration Set Up Routine                   ;
;                   					               ;
;  Description: The P1345 Configuration Set Up Routine does nothing    ;
;               because there is nothing besides IRQ and DMA on the    ;
;               adapter that is software configurable.                 ;
;                                                                      ;
;  Input:       BX - pointer to MCB                                    ;
;               SI - Adapter Type                                      ;
;                                                                      ;
;  Output:      none                                                   ;
;								       ;
;  Calls:       none                                                   ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

sc_p1345	PROC	NEAR	
sc_wdtcplus     EQU     sc_P1345
;
;  Fall through to 3COM routine.
;

sc_p1345        ENDP


PAGE_BREAK <Racore M8110 Configuration Set Up Routine Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  M8110 Configuration Set Up Routine                   ;
;                   					               ;
;  Description: The M8110 Configuration Set Up Routine does nothing    ;
;               because there is nothing on the adapter that is        ;
;               software configurable.                                 ;
;                                                                      ;
;  Input:       BX - pointer to MCB                                    ;
;               SI - Adapter Type                                      ;
;                                                                      ;
;  Output:      none                                                   ;
;								       ;
;  Calls:       none                                                   ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

sc_m8110	PROC	NEAR	
;
;  Fall through to 3COM routine.
;

sc_m8110        ENDP


PAGE_BREAK <3C603 Configuration Set Up Routine Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  3C603 Configuration Set Up Routine                   ;
;                   					               ;
;  Description: The 3C603 Configuration Set Up Routine does nothing    ;
;               because there is nothing besides IRQ and DMA on the    ;
;               adapter that is software configurable.                 ;
;                                                                      ;
;  Input:       BX - pointer to MCB                                    ;
;               SI - Adapter Type                                      ;
;                                                                      ;
;  Output:      Modifies MCB register fields                           ;
;								       ;
;  Calls:       none                                                   ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

sc_3c603	PROC	NEAR	
;
;  Just return to caller.
;
                clc                             ; Set good return code
                ret                             ; Return to caller

sc_3c603        ENDP


IFDEF TYPE_3C603
PAGE_BREAK <3C603 IRQ Set Up Routine Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  3C603 IRQ Set Up Routine                             ;
;                   					               ;
;  Description: The 3C603 IRQ Set Up Routine sets up the 3C603's on-   ;
;               board control registers with the proper interrupt      ;
;               selection bits set.                                    ;
;                                                                      ;
;  Input:       DS:DI - pointer to MCB                                 ;
;               SI - Adapter Type                                      ;
;               BX - Interrupt Level                                   ;
;               BP - Adapter Number                                    ;
;                                                                      ;
;  Output:      Carry set if error                                     ;
;		AX - 0 = edge trigger ints, ~0 = level trigger ints    ;
;								       ;
;  Calls:       none                                                   ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

irq_3c603	PROC	NEAR	

		mov	cx, bx			; Copy int level
                cmp     cx, 9                   ; Is interrupt level = 9 ?
                SJNE    init_not_9              ;   No, no ajustment needed
                xor     cx, cx                  ;   Yes, set mask = 0
                SJMP    init_int_ok             ;     and skip checks
init_not_9:
		sub	cx, 2			; Calculate bit pattern
		cmp	cx, 4			;   for control register
		SJB	init_int_ok		; If too high, just set 0
		mov	cx, 0003h		; Set default of level 5
		
init_int_ok:
		and	cx, 0003h		; Clear all but low bits
		shl	cx, 1			; Put bits in right position
		shl	cx, 1			;
		shl	cx, 1			;
		shl	cx, 1			;
		xchg	ch, cl			;
		mov	dx, [di].acb_adp_cfg	; Get CONTROL register
		in	ax, dx			; Get contents
		and	ax, NOT (ISELA + ISELB) ; Clear int select bits
		or	ax, cx			; Set int level
		out	dx, ax			; Write new contents

                xor     ax, ax                  ; Set nonselectable trigger
                clc                             ; Set good return code
		ret                             ; Return to caller

irq_3c603	ENDP


ELSE
PAGE_BREAK <P1345 IRQ Set Up Routine Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  P1345 IRQ Set Up Routine                             ;
;                   					               ;
;  Description: The P1345 IRQ Set Up Routine does nothing since there  ;
;               is no software control over interrupt levels on the    ;
;               P1345 adapter.                                         ;
;                                                                      ;
;  Input:       DS:DI - pointer to MCB                                 ;
;               SI - Adapter Type                                      ;
;               BX - Interrupt Level                                   ;
;               BP - Adapter Number                                    ;
;                                                                      ;
;  Output:      Carry set if error                                     ;
;		AX - 0 = edge trigger ints, ~0 = level trigger ints    ;
;								       ;
;  Calls:       none                                                   ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

irq_p1345	PROC	NEAR	
irq_wdtcplus    EQU     irq_p1345

                xor     ax, ax                  ; Set nonselectable trigger
                clc                             ; Set good return code
		ret                             ; Return to caller

irq_p1345	ENDP
ENDIF


IFDEF TYPE_3C603
PAGE_BREAK <3C603 Read BIA Routine Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  3C603 Read BIA Routine                               ;
;                   					               ;
;  Description: The 3C603 Read BIA Routine reads the burned-in address ;
;               from the shadowed registers on the 3C603 adapter.      ;
;                                                                      ;
;  Input:       DS:BX - points to MCB                                  ;
;                                                                      ;
;  Output:      Loads BIA into MAC specific characteristics table      ;
;								       ;
;  Calls:       bia_complete                                           ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

bia_3c603	PROC	NEAR	
                push    cs                      ; Make ES = CS
                pop     es                      ;

		pushf				; Save flags
		cli				; Disable ints
		mov	dx, [bx].acb_adp_cfg 	; Get CONTROL register
		in	ax, dx			; Get contents
		and	ax, NOT SHADOW		; Clear SHADOW bit
		out	dx, ax			; Set new contents
		
		lea	di, DLCNetAddress	; Point to BIA location
		add	dx, 2			; Move BIA into the
		in	ax, dx			;   burned-in address field. 
		or	ax, 40h			; ((Set valid node address))
		stosw   		        ; 
		add	dx, 2			; 
		in	ax, dx			;
		stosw			        ;
		add	dx, 2			;
		in	ax, dx			;
		stosw			        ;

		sub	dx, 6			; Point to CONTROL register
		in	ax, dx			; Get contents
		or	ax, SHADOW		; Turn on SHADOW bit
		out	dx, ax			; Set new contents
		popf				; Restore flags

		lea	di, [bx].acb_opn_blk.OPEN_NodeAddr
						; Point to open node address

                push    ds                      ; Make ES and DS point to
                pop     es                      ;   dynamic data segment

                call    bia_complete            ; Complete BIA processing

                ret                             ; Return to caller

bia_3c603	ENDP
ENDIF


IFDEF TYPE_M8110
PAGE_BREAK <M8110 Read BIA Routine Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  M8110 Read BIA Routine                               ;
;                   					               ;
;  Description: The M8110 Read BIA Routine reads the burned-in address ;
;               from the Boot Prom on the adapter card.                ;
;                                                                      ;
;  Input:       DS:BX - points to MCB                                  ;
;               ES - Driver's Data Segment/Selector                    ;
;                                                                      ;
;  Output:      Loads BIA into MAC specific characteristics table      ;
;								       ;
;  Calls:       bia_complete                                           ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

bia_m8110	PROC	NEAR	

		mov	dx, [bx].acb_sif_data 	; Get SIF data register
                add     dx, 0ah                 ; Point to status reg
                in      al, dx                  ; Get status reg contents

                mov     cl, al                  ; Copy status contents
                push    bx                      ; Save MCB pointer
                mov     ax, 000ch               ; Point to BIA EPROM
                mov     bx, 0dff8h              ;
                test    cl, 0ch                 ; Is low or high address ?
                SJZ     m_addr_ok               ;   It's low, continue
                mov     ax, 000dh               ; Set high address 

m_addr_ok:
                mov     cl, 4                   ; Set rotate count
                ror     ax, cl                  ; Set segment address
                mov     es, ax                  ; Set up ES with BIA segment
                mov     si, bx                  ; Copy BIA address pointer
                pop     bx                      ; Restore MCB pointer
		lea	di, DLCNetAddress	; Point to BIA location
                push    es                      ; Swap DS and ES
                push    ds                      ;
                pop     es                      ;
                pop     ds                      ;
                mov     cx, 6                   ; Set address size
                rep     movsb                   ; Copy the burned-in address

                push    es                      ; Make ES and DS point to
                pop     ds                      ;   MCB selector

		lea	di, [bx].acb_opn_blk.OPEN_NodeAddr
						; Point to open node address

                call    bia_complete            ; Complete BIA processing

                ret                             ; Return to caller

bia_m8110	ENDP
ENDIF


IFDEF TYPE_PWD
PAGE_BREAK <P1345 Read BIA Routine Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  P1345 Read BIA Routine                               ;
;                   					               ;
;  Description: The P1345 Read BIA Routine reads the burned-in address ;
;               by issuing a Read Adapter command to the adapter.      ;
;                                                                      ;
;  Input:       DS:BX - points to MCB                                  ;
;               ES - Driver's Data Segment/Selector                    ;
;                                                                      ;
;  Output:      Loads BIA into MAC specific characteristics table      ;
;								       ;
;  Calls:       bia_complete                                           ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

bia_p1345	PROC	NEAR	
bia_wdtcplus    EQU     bia_p1345

		mov	dx, [bx].acb_sif_addr 	; Get SIF address register
		mov	ax, 0a00h		; Set BIA pointer address
		out	dx, ax			; 
		mov	dx, [bx].acb_sif_data	; Set SIF data register
	        in	ax, dx			; Get BIA address pointer	
		xchg	ah, al	 		; Make it motorola form

		lea	si, [bx].acb_log_buf	; Get read adapter buffer
		mov	WORD PTR [si], 0600h	; Load address size
		mov	[si+2], ax		; Load address address

		mov	[bx].acb_scb.SCB_Cmd, TMS_READADP   
						  ; Set read adapter command
		push	ds			  ; Pass buffer address to
		push	si			  ;   conversion routine to
		call	VirttoPhys		  ;   convert to moto form.
		pop	[bx].acb_scb.SCB_Ptr.Mhi  ; Put motorola address into
		pop	[bx].acb_scb.SCB_Ptr.Mlo  ;   SCB. 

		mov	dx, [bx].acb_sif_int  	; Get SIFINT register
		mov	ax, SIFINT_CMD		; Set command
		out	dx, ax			; Inform adapter

pinit_wait:
		cmp	[bx].acb_ssb.SSB_Cmd, TMS_READADP
						; Wait for command to complete
		jne	pinit_wait		; If not done, try again
	
                push    cs                      ; Make ES = CS
                pop     es                      ;

                mov     cx, 3                   ; Set word count
		lea	di, DLCNetAddress	; Point to BIA location
		rep     movsw			; Put the burned-in address

                ret                             ; Return to caller

bia_p1345	ENDP
ENDIF


PAGE_BREAK <BIA Processing Completion Routine Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  BIA Processing Completion Routine                    ;
;                   					               ;
;  Description: The BIA Processing Completion Routine copies the BIA   ;
;               to an optional destination (typically, the open para-  ;
;               meter block).                                          ;
;                                                                      ;
;  Input:       DS:BX - points to MCB                                  ;
;               ES:DI - points to an optional destination              ;
;                                                                      ;
;  Output:      modifies specified address fields                      ;
;								       ;
;  Calls:       none                                                   ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

bia_complete	PROC	NEAR	

                or      di, di                  ; Is it valid ?
                SJZ     bia_return              ;   No, exit 
                mov     cx, 3                   ; Set word count
                push    ds                      ; Save dynamic data segment
                push    cs                      ; Make DS = CS
                pop     ds                      ;
		lea	si, DLCNetAddress	; Point to BIA location
                rep     movsw                   ; Copy BIA to destination
                pop     ds                      ; Restore data segment

bia_return:
                ret                             ; Return to caller

bia_complete	ENDP


IFDEF TYPE_3C603
PAGE_BREAK <3C603 DMA Set Up Routine Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  3C603 DMA Set Up Routine                             ;
;                   					               ;
;  Description: The 3C603 DMA Set Up Routine sets up the 3C603's on-   ;
;               board control registers with the proper DMA seclection ;
;               bits set. This routine also enables interrupts from    ;
;               the adapter.                                           ;
;                                                                      ;
;  Input:       DS:BX - pointer to MCB                                 ;
;               AH - DMA Level                                         ;
;                                                                      ;
;  Output:      none                                                   ;
;								       ;
;  Calls:       none                                                   ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

dma_3c603	PROC	NEAR	

		cmp	ah, 2			; Is level = 2 ?
		SJE	cdmaerror		;   Yes, error
		cmp	ah, 4			; Is level = 4 ?
		SJE	cdmaerror 		;   Yes, error
		cmp	ah, 8			; Is level < 8 ?
		SJB	cdma_ok	        	;   Yes, continue

cdmaerror:
                stc                             ; Set bad return code
                ret                             ; Return to caller


cdma_ok:
                push    cx                      ; Save used register
                push    ax                      ; Save DMA level
                mov     cx, TCFG_SINTEN         ; Set int enable mask
                cmp     ah, 1                   ; Is DMA level = 1 ?
                SJE     cdma_1                  ;   Yes, mask is OK
                or      cx, TCFG_DMASEL         ; Set DMA select bit

cdma_1:
		mov	dx, [bx].acb_adp_cfg	; Get CONTROL port address
		in	ax, dx			; Get contents of CONTROL
		or	ax, cx  		; Enable ints and DMA  
		out	dx, ax			; Write it back out
                pop     ax                      ; Restore DMA level
                pop     cx                      ; Restore used register

                clc                             ; Set good return code
		ret                             ; Return to caller

dma_3c603	ENDP
ENDIF


IFDEF TYPE_PWD
PAGE_BREAK <P1345 DMA Set Up Routine Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  P1345 DMA Set Up Routine                             ;
;                   					               ;
;  Description: The P1345 DMA Set Up Routine sets up the P1345's on-   ;
;               board control register to enable DMA's.  This routine  ;
;               also enables INT's from the adapter.                   ;
;                                                                      ;
;  Input:       DS:BX - pointer to MCB                                 ;
;               AH - DMA Level                                         ;
;                                                                      ;
;  Output:      none                                                   ;
;								       ;
;  Calls:       none                                                   ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

dma_p1345	PROC	NEAR	
dma_wdtcplus    EQU     dma_p1345
                
                push    cx                      ; Save used register
                mov     cl, PCFG_INTDMA_EN      ; Set enable bits

pdma_go:
		cmp	ah, 5			; Is level < 5 ?
		SJB	pdmaerror		;   Yes, error
		cmp	ah, 8			; Is level < 8 ?
		SJB	pdma_ok	        	;   Yes, continue

pdmaerror:
                stc                             ; Set bad return code
                SJMP    pdma_exit               ; Return to caller


pdma_ok:
                push    ax                      ; Save DMA level
		mov	dx, [bx].acb_adp_cfg	; Get CONBUF port address
		in	al, dx			; Get contents of CONBUF
		or	al, cl           	; Enable ints and DMA 
		out	dx, al			; Write it back out
                pop     ax                      ; Restore DMA level

                clc                             ; Set good return code

pdma_exit:
                pop     cx                      ; Restore used register
		ret                             ; Return to caller

dma_p1345	ENDP
ENDIF

IFDEF TYPE_M8110
PAGE_BREAK <M8110 DMA Set Up Routine Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  M8110 DMA Set Up Routine                             ;
;                   					               ;
;  Description: The M8110 DMA Set Up Routine does nothing because      ;
;               DMA are always enabled on the Racore M8110 adapter.    ;
;               This routine just falls through the the Jupiter        ;
;               routine.                                               ;
;                                                                      ;
;  Input:       DS:BX - pointer to MCB                                 ;
;               AH - DMA Level                                         ;
;                                                                      ;
;  Output:      none                                                   ;
;								       ;
;  Calls:       none                                                   ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

dma_m8110	PROC	NEAR	
;
;  Do nothing, just return to caller.
;
                clc             ; Set good return flag
                ret             ; Return to caller

dma_m8110	ENDP
ENDIF


PAGE_BREAK <Reset Adapter Routine Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  Reset Adapter Routine                                ;
;                   					               ;
;  Description: The Reset Adapter Routine routes the reset adapter     ;
;               request to the proper adapter reset handler.           ;
;                                                                      ;
;  Input:       BX - pointer to MCB                                    ;
;               SI - Adapter Type                                      ;
;               AX - Hard/Soft reset value (0 = hard, ~0 = soft)       ;
;                                                                      ;
;  Output:      none                                                   ;
;								       ;
;  Calls:       specific reset handlers                                ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

		PUBLIC	ResetAdapter
ResetAdapter	PROC	NEAR	

                or      ax, ax          ; Was hard reset requested ?
                SJZ     reset_hard      ;   Yes, call hard reset routine

                mov     dx, [bx].acb_sif_int    ; Get interrupt register
                mov     ax, 0ff00h              ; Set soft reset command
                out     dx, ax                  ; Reset adapter
                ret                             ; Return to caller

reset_hard:                
                IFCALL  <reset_>        ; Jump to proper reset routine
                ret                     ; Return to caller

ResetAdapter	ENDP


IFDEF TYPE_3C603
PAGE_BREAK <3C603 Reset Adapter Routine Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  3C603 Reset Adapter Routine                          ;
;                   					               ;
;  Description: The 3C603 Reset Adapter Routine asserts and deasserts  ;
;               the hardware reset line to the TMS380 based adapter    ;
;               causing it to begin its Bring Up Diagnostics (BUD)     ;
;               code.  The routine is not responsible for determining  ;
;               the success or failure of the diagnostics.             ;
;                                                                      ;
;  Input:       BX - pointer to MCB                                    ;
;                                                                      ;
;  Output:      none                                                   ;
;								       ;
;  Calls:       none                                                   ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

reset_3c603	PROC	NEAR	

		PUSHREG	cx, dx			; Save registers
                pushf                           ; Save flags
                cli                             ; Disable interrupts

		mov	dx, [bx].acb_adp_cfg	; Get adapter control reg
                in      ax, dx                  ;

		or	ax, TCFG_SHADOW OR TCFG_MASTER OR TCFG_SIFRST
                                        	; Set RESET and SHADOW mask
		out	dx, ax			; Assert RESET line to card		

		sub	dx, 3			; Point to config reg
		mov	al, 0eh 		; Set CMDDLY and WAIT# signal
		out	dx, al			; Write to config register

		mov	cx, 1000 		; Set wait loop count
reset_wait1:	in	al, dx			; Wait long enough for 
		loop	reset_wait1	        ;   reset to take effect

		add	dx, 3			; Point to control reg

                in      ax, dx                  ; Get control register
		and	ax, NOT TCFG_SIFRST     ; Clear RESET mask
		out	dx, ax			; Deassert reset line
                
                popf                            ; Restore interrupts
		POPREG	cx, dx			; Restore registers
		ret

reset_3c603	ENDP
ENDIF


IFDEF TYPE_PWD
PAGE_BREAK <P1345 Reset Adapter Routine Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  P1345 Reset Adapter Routine                          ;
;                   					               ;
;  Description: The P1345 Reset Adapter Routine asserts and deasserts  ;
;               the hardware reset line to the TMS380 based adapter    ;
;               causing it to begin its Bring Up Diagnostics (BUD)     ;
;               code.  The routine is not responsible for determining  ;
;               the success or failure of the diagnostics.             ;
;                                                                      ;
;  Input:       BX - pointer to MCB                                    ;
;                                                                      ;
;  Output:      none                                                   ;
;								       ;
;  Calls:       none                                                   ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

reset_p1345	PROC	NEAR	
reset_wdtcplus  EQU     reset_p1345

		PUSHREG	cx, dx			; Save registers

		mov	dx, [bx].acb_adp_cfg	; Get adapter config reg
		mov	al, PCFG_RESET		; Set RESET mask
		out	dx, al			; Assert RESET line to card		

		mov	cx, 1000 		; Set wait loop count
reset_wait2:	in	al, dx			; Wait long enough for 
		loop	reset_wait2	        ;   reset to take effect

		xor	al, al		        ; Clear RESET mask
		out	dx, al			;   Deassert reset line

		POPREG	cx, dx			; Restore registers
		ret

reset_p1345	ENDP
ENDIF


IFDEF TYPE_M8110
PAGE_BREAK <M8110 Reset Adapter Routine Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  M8110 Reset Adapter Routine                          ;
;                   					               ;
;  Description: The M8110 Reset Adapter Routine asserts and deasserts  ;
;               the hardware reset line to the TMS380 based adapter    ;
;               causing it to begin its Bring Up Diagnostics (BUD)     ;
;               code.  The routine is not responsible for determining  ;
;               the success or failure of the diagnostics.             ;
;                                                                      ;
;  Input:       BX - pointer to MCB                                    ;
;                                                                      ;
;  Output:      none                                                   ;
;								       ;
;  Calls:       none                                                   ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

reset_m8110	PROC	NEAR	

		PUSHREG	cx, dx			; Save registers

		mov	dx, [bx].acb_adp_cfg	; Get adapter reset reg
                inc     dx                      ; Point to "set reset" reg
		in	al, dx			; Assert RESET line to card		
                                                ;   (Dummy value in AL)
        
		mov	cx, 1000 		; Set wait loop count
reset_wait3:	in	al, dx			; Wait long enough for 
		loop	reset_wait3	        ;   reset to take effect

		dec     dx		        ; Point to "clear reset" reg
		in	al, dx			;   Deassert reset line

		POPREG	cx, dx			; Restore registers
		ret

reset_m8110	ENDP
ENDIF


PAGE_BREAK <Initialize Adapter Routine Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  Initialize Adapter Routine                           ;
;                   					               ;
;  Description: The Initialize Adapter Routine performs the initialize ;
;               process on the a TMS380 based adapter.  This routine   ;
;               does not wait for the initialization to complete.      ;
;                                                                      ;
;  Input:       BX - pointer to MCB                                    ;
;                                                                      ;
;  Output:      none                                                   ;
;								       ;
;  Calls:       none                                                   ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

		PUBLIC	InitAdapter
InitAdapter	PROC	NEAR	

		PUSHREG	cx, dx, si 		; Save registers

		mov	dx, [bx].acb_sif_addr	; Set SIF address reg
		mov	ax, 0a00h		; Set init parm base address
		out	dx, ax			;   to 0A00H in adapter mem

		mov	dx, [bx].acb_sif_dinc	; Set SIF data inc reg

		mov	cx, size_INIT / 2	; Move the global init
		lea	si, gbl_init_blk	;   parameter mask block 
                push    ds                      ; Save data segment
                push    cs                      ; Set DS = global segment
                pop     ds                      ;
                
init_loop:                                      ;
		lodsw	        		;   
                out     dx, ax                  ;
                loop    init_loop               ;

                pop     ds                      ; Restore data segment

		mov	ax, [bx].acb_scb_addr.Mhi  ; Load physical SCB address
		xchg	ah, al			   ;  into the adapter. This
		out	dx, ax			   ;  must be done by swapping
		mov	ax, [bx].acb_scb_addr.Mlo  ;  bytes because TMS380 DIO
		xchg	ah, al			   ;  swaps bytes (but DMA
		out	dx, ax			   ;  doesn't).

		mov	ax, [bx].acb_ssb_addr.Mhi  ; Load physical SSB address
		xchg	ah, al			   ;  into the adapter. This
		out	dx, ax			   ;  must be done by swapping
		mov	ax, [bx].acb_ssb_addr.Mlo  ;  bytes because TMS380 DIO
		xchg	ah, al			   ;  swaps bytes (but DMA
		out	dx, ax			   ;  doesn't).

		mov	dx, [bx].acb_sif_int	; Set SIFINT reg address
		mov	ax, SIFINT_CMD		; Write command execute
		out	dx, ax			;

		POPREG	cx, dx, si 		; Restore registers
		ret

InitAdapter	ENDP

END_CODE

		END
