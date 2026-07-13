		PAGE  60, 132
		TITLE Token-Ring RIPL Boot ROM Driver - UTIL.ASM
;**********************************************************************;
;**********************************************************************;
;								       ;
; File Name:       UTIL.ASM          				       ;
;								       ;
; Program Name:    Token-Ring RIPL Boot ROM Device Driver              ;
;								       ;
; Companion Files: None						       ;
;								       ;
; Function:        This module contains all the utility routines that  ;
;                  are used throughout the TRNDD.  The following       ;
;	 	   routines are defined in this module:		       ;
;								       ;
;		   QueueSCB - queue GRB to SCB			       ;
;		   VirttoPhys - convert vitual addresses to physical   ;
;		   OpenCard - Open adapter routine                     ;
;		   ISAIntEnable - Enable ISA card interrupts           ;
;                  ISAIntDisable - Disable ISA card interrupts         ;    
;                  IssueEOI - Issue EOI to interrupt controller        ;
;                  DriverPoll - Used by ROM DLC to poll driver         ;
;								       ;
; History:             						       ;
;								       ;
;     03/13/89  Mike Witkowski - Original Version 0.0		       ;
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
                EXTRN   adp_stuff:              WORD
                EXTRN   gbl_dynmem:             WORD


;--------------------------;
; External Code References ;
;--------------------------;
                EXTRN   ResetAdapter:           NEAR


START_CODE

PAGE_BREAK <Queue SCB Routine Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  Queue SCB Routine                                    ;
;                   					               ;
;  Description: The Queue SCB routine accepts a General Request Block  ;
;               address from the stack and either writes the SCB info  ;
;               Directly to the SCB, or queues the request block based ;
;               whether the SCB is currently in use.                   ;
;                                                                      ;
;  Input:       DI - General Request Block (GRB)                       ;
;               BX - pointer to MCB                                    ;
;                                                                      ;
;  Output:      pop GRB off stack                                      ;
;								       ;
;  Calls:       none                                                   ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

		PUBLIC	QueueSCB
QueueSCB	PROC	NEAR	

; 
;  First, save registers that we destroy.
;
		PUSHREG	dx, di, si  
		pushf

		mov	dx, [bx].acb_sif_int	; Load SIFINT reg address

;
;  Check to see if the SCB is free to use.  If it is, then 
;  write the dummy SCB info from the GRB into the real SCB.  If the 
;  SCB is not free, then see if there are any other GRB waiting for
;  it. If there are other's waiting, we don't need to do anything. 
;  If no other GRB is waiting, then set the next req pointer to point
;  to this GRB. 
;
queue_scb:  		
		cmp	[bx].acb_scb.SCB_Cmd, 0	; Is SCB free ? 
		SJNZ	queue_wait		;   No, queue request

		cmp	[bx].acb_xmit_whead, 0	; Any xmits waiting ?
		SJZ	queue_write		;   No, write SCB

queue_wait:
		mov	[bx].acb_req_next, si	; Set this GRB as next
                mov     ax, SIFINT_SCBREQST     ; Set prodding interrupt
		SJMP	queue_out		; Ask for int when done


;
;  The SCB is free.  Copy all the info in the dummy SCB within the 
;  GRB into the real adapter SCB. 
; 
queue_write:
		lea	si, [di].GRB_SCBCmd	; Point to GRB
		lea	di, [bx].acb_scb	; Point to SCB

                push    ds                      ; Make ES = DS
                pop     es                      ;

                movsw                           ; Copy SCB
                movsw
                movsw
                
		mov	ax, SIFINT_CMD		; Set SIF command pattern 

queue_out:
		out	dx, ax			; Write SIF command


;
;  Exit from the routine
;
queue_exit:
		popf                            ; Restore interrupts (maybe)

		POPREG	dx, di, si      	; Restore registers
		ret				; Return to the caller

QueueSCB	ENDP

PAGE_BREAK <Virtual to Physical Routine Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  Virtual to Physical Routine                          ;
;                   					               ;
;  Description: The Virtual to Physical Routine converts a passed      ;
;               virtual address into a 32 physical address.  This      ;
;               routine places the physical address into memory        ;
;               "swapped" so that in linear memory, it looks like a    ;
;               motorola type address.                                 ;
;                                                                      ;
;  Input:       Virtual Address (on the Stack)                         ;
;               ES - Data Selector/Segment                             ;
;                                                                      ;
;  Output:      Physical Address (on the Stack)                        ;
;								       ;
;  Calls:       none                                                   ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

		PUBLIC	VirttoPhys
VirttoPhys	PROC	NEAR	
		STKFRAME

		PUSHREG	bx, si	 	; Save used registers

     		mov	ax, [bp+6]	; Get the selector/segment
		mov	si, [bp+4]	; Get the offset

		clc	      		; Clear the carry flag
		rol	ax, 1	  	; Put 8 bit extension into the
		rol	ax, 1		;   AH register so that it is
		rol	ax, 1		;   already swapped for writing
		rol	ax, 1 		;   to memory.
		mov	bx, ax          ;
                and     ax, 000fh       ;
                xchg    ah, al          ;
                and     bx, 0fff0h      ; Isolate lower 12 segment bits
		add	bx, si		; Add shifted segment to offset
		adc	ah, 0		; If carry, add one to high byte
		xchg	bh, bl	        ; Make sure low bytes are in order

		mov	[bp+4], ax	; Store Hi word
		mov	[bp+6], bx 	; Store Lo word
				
		POPREG	bx, si		; Restore registers

		STKFRMEND			; Remove stack frame
		ret				; Return to the caller

VirttoPhys	ENDP


PAGE_BREAK <Open Card Routine Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  Open Card Routine                                    ;
;                   					               ;
;  Description: The Open Card routine is responsible for openning      ;
;               the adapter hardware onto the Token-Ring.      	       ;	
;                                                                      ;
;  Input:       BX - pointer to MCB                                    ;
;               DI - pointer to GRB                                    ;
;                                                                      ;
;  Output:      none                                                   ;
;								       ;
;  Calls:       QueueSCB                                               ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

		PUBLIC	OpenCard
OpenCard	PROC	NEAR	
		PUSHREG	cx, dx,	si  		; Save registers

		push	di			; Save GRB pointer

;----------------------------------------------------------------------;
;  Now, set up the transmit lists and the immediate transmit 	       ;
;  buffers in the ACB.  We'll start by doing the lists. 	       ;
;----------------------------------------------------------------------;
		mov	di, dyn_xmit    	; Get ptr to xmit list area
		mov	cx, MAXMITS * XMITLISTS ; Calculate # of lists
		mov	[bx].acb_xlist_cnt, cx	; Place list count into ACB
		mov	[bx].acb_xmit_head, di	; Set first list in free Q
	
open_xmit_cfg:
		mov	[di].XMIT_CSTAT, 0	         ; Clear CSTAT field
		and	[di].XMIT_FwdPtr.Mlo, EVEN_PTR   ; Set even fwd ptr  
		add	di, size_XMIT			 ; Get next list
		loop	open_xmit_cfg		         ; Config next list

		mov	[bx].acb_xmit_whead, cx    ; Clear various xmit Q's
		mov	[bx].acb_xmit_wtail, cx    ; 
		mov	[bx].acb_xmit_ahead, cx    ;
		mov	[bx].acb_xmit_atail, cx    ;
		
		PAGE
;----------------------------------------------------------------------;
;  Now configure the immediate transmit buffers.  These will begin     ;
;  immediately after the transmit lists. 			       ;
;----------------------------------------------------------------------;
		mov	[bx].acb_ibuf_free, di	; Set point to first buffer
		mov	cx, MAXMITS     	; Set number of buffers
		mov	ax, di		 	; Copy pointer
		
open_ibuf_cfg:
		add	ax, XMITIBUFSIZE + size_IBUF	
						; Point to next buffer
		
		mov	[di].IBUF_Ptr, ax  	; Set forward pointer

		mov	si, di			; Save last pointer

		add 	di, XMITIBUFSIZE + size_IBUF	
						; Point to next buffer

		loop	open_ibuf_cfg		; Configure next buffer		 
		
		mov	[si].IBUF_Ptr, 0 	; Clear last's pointer



;----------------------------------------------------------------------;
;  Now it is time to configure the receive lists and receive data      ;
;  buffers. 							       ;
;----------------------------------------------------------------------;
		mov	di, dyn_rcv     	; Get pointer to rcv memory
		mov	cx, MAXRCVS     	; Get number of rcv lists
		mov	[bx].acb_rcv_head, di	; Set first rcv list

open_rcv_cfg:
		mov	[di].RCV_CSTAT, RCSTAT_GO   ; Activate list's CSTAT
		mov	di, [di].RCV_Next           ; Set new list address
		loop    open_rcv_cfg		    ; Configure next list/buf	


;
;  Now set up the GRB to reflect the open adapter request.
;
		pop	di				; Restore GRB pointer

		mov	[di].GRB_SCBCmd, TMS_OPEN	; Set TMS380 command
		mov	ax, [bx].acb_open_addr.Mhi	; Set SCB parameter 
		mov	[di].GRB_SCBPHi, ax		;   pointer 
		mov	ax, [bx].acb_open_addr.Mlo	;
		mov	[di].GRB_SCBPLo, ax		;  

		call	QueueSCB		; Queue the general request

		POPREG	cx, dx, si		; Restore registers
		ret				; Return to caller

OpenCard	ENDP


		PUBLIC	ISAIntEnable
ISAIntEnable	PROC	NEAR	
;
;  Enable interrupt at the interrupt controller.  First figure out
;  which controller to unmask.  
;
		mov	ah, [bx].acb_int_mask	  ; Get int enable mask

		mov	di, [bx].acb_adpnum	  ; Find service char table

		mov	al, BYTE PTR cs:adp_stuff[di].adp_int  
                                                  ; Get the int level
		cmp	al, 7			  ; Is it on secondary ctrlr ?
		jbe	en_prim 		  ;   No, its on controller #1

		in	al, INT_CTRL2_MR	; Get int mask
		and	al, ah			; Turn on our interrupt
		out	INT_CTRL2_MR, al	;  
		jmp	en_on			; Continue

en_prim:	
		in	al, INT_CTRL1_MR	; Get int mask
		and	al, ah			; Turn on our interrupt
		out	INT_CTRL1_MR, al	;  

en_on:
		ret				; Return to caller

ISAIntEnable	ENDP


PAGE_BREAK <ISA Interrupt Disable Routine Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  ISA Interrupt Disable Routine                        ;
;                   					               ;
;  Description: The ISA Interrupt Disable Routine disables interrupts  ;
;               from an ISA card by masking the interrupt at the       ;
;               interrupt controller.  This can be done since the      ;
;               interrupt levels for ISA cards are edge triggered and  ;
;               are not sharable.                                      ;
;                                                                      ;
;  Input:       DS:BX - adapter MCB                                    ;
;                                                                      ;
;  Output:      none                                                   ;
;								       ;
;  Calls:       none                                                   ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

		PUBLIC	ISAIntDisable
ISAIntDisable	PROC	NEAR	
;
;  Disable interrupt at the interrupt controller.  First figure out
;  which controller to unmask.  
;
		mov	ah, [bx].acb_int_mask	  ; Get int enable mask
		not	ah			  ; Make it disable mask

		mov	di, [bx].acb_adpnum	  ; Find service char table

		mov	al, BYTE PTR cs:adp_stuff[di].adp_int
                                                  ; Get the int level
		cmp	al, 7			  ; Is it on secondary ctrlr ?
		jbe	dis_prim		  ;   No, its on controller #1

		in	al, INT_CTRL2_MR	; Get int mask
		or	al, ah			; Turn off our interrupt
		out	INT_CTRL2_MR, al	;  
		jmp	dis_off			; Continue

dis_prim:	
		in	al, INT_CTRL1_MR	; Get int mask
		or	al, ah			; Turn off our interrupt
		out	INT_CTRL1_MR, al	;  

dis_off:
		ret				; Return to caller

ISAIntDisable	ENDP


PAGE_BREAK <IssueEOI Routine Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  IssueEOI Routine                                     ;
;                   					               ;
;  Description: The IssueEOI routine issues a EOI to the interrupt     ;
;               controller.  This is done differently (maybe) depend-  ;
;               on whether we're under DOS or OS/2.                    ;
;                                                                      ;
;  Input:       DS:BX - adapter MCB                                    ;
;                                                                      ;
;  Output:      none                                                   ;
;								       ;
;  Calls:       none                                                   ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

		PUBLIC	IssueEOI
IssueEOI	PROC	NEAR	

		mov	di, [bx].acb_adpnum	  ; Find service char table

		mov	al, BYTE PTR cs:adp_stuff[di].adp_int
                                                  ; Get the int level
		cmp	al, 7			  ; Is it on secondary ctrlr ?
		SJBE	int_prim		  ;   No, its on controller #1

		sub	al, 8			; Adjust int level
		or	al, EOI_SPECIFIC	; Make it specific EOI
		out	INT_CTRL2, al		; EOI controller #1 
		mov	al, 2			; Set int level 2

int_prim:	
		or	al, EOI_SPECIFIC	; Make it specific EOI
		out	INT_CTRL1, al		; EOI controller #1 

		ret				; Return to caller

IssueEOI	ENDP


PAGE_BREAK <NDIPoll Routine Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  NDIPoll Routine                                      ;
;                   					               ;
;  Description: The NDIPoll routine is used by the RIPL ROM DLC        ;
;               portion of the RIPL ROM to poll the driver portion so  ;
;               that the driver portion can do timeout work.  Our      ;
;               driver does not need this polling action so return     ;
;               to caller without doing anything.                      ;
;                                                                      ;
;  Input:       none                                                   ;
;                                                                      ;
;  Output:      none                                                   ;
;								       ;
;  Calls:       none                                                   ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

		PUBLIC	NDIPoll
NDIPoll	        PROC	NEAR	

		ret				; Return to caller

NDIPoll	        ENDP


PAGE_BREAK <NDIDisengage Routine Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  NDIDisengage Routine                                 ;
;                   					               ;
;  Description: The NDIDisengage routine is used by the RIPL ROM DLC   ;
;               portion of the RIPL ROM to signal the Network Driver   ;
;               to release its IRQ, DMA, and memory resources.         ;
;                                                                      ;
;  Input:       none                                                   ;
;                                                                      ;
;  Output:      none                                                   ;
;								       ;
;  Calls:       none                                                   ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

		PUBLIC	NDIDisengage
NDIDisengage    PROC	NEAR	

                push    ds                      ; Save data segment

		mov	ds, cs:gbl_dynmem	; Point to dynamic memory
		xor	bx, bx                  ; Set pointer to ACB
                

;
;  First, reset adapter.
;
                xor     ax, ax                  ; Set hard reset
                call    ResetAdapter            ; Reset the adapter

;
;  Disable interrupts and DMAs.
;
                call    ISAIntDisable           ; Disable interrupts

		mov	di, [bx].acb_adpnum     ; Get adapter number

		mov	al, BYTE PTR cs:adp_stuff[di].adp_dma
                                                ; Get DMA level
                or      al, 04h                 ; Disable DMA channel bit 

                cmp     al, 4                   ; Is DMA on 1st controller ?                
                SJAE    dis_dma2                ;   No, its on second 

		out	DMA_CTRL_WSMB1, al	; Disable channel
		SJMP    dis_exit 		; Continue processing

dis_dma2:
		sub	al, 4	   		; Adjust DMA level  
		out	DMA_CTRL_WSMB2, al	; Disable channel
                                                
dis_exit:
                pop     ds                      ; Restore caller's DS
		ret				; Return to caller

NDIDisengage	ENDP


END_CODE
		END
