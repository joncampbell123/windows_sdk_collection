		PAGE  60, 132
		TITLE Token-Ring RIPL Boot ROM Driver - INIT.ASM
;**********************************************************************;
;**********************************************************************;
;								       ;
; File Name:       INIT.ASM					       ;
;								       ;
; Program Name:    Token-Ring RIPL Boot ROM Device Driver              ;
;								       ;
; Companion Files: None						       ;
;								       ;
; Function:        This module contains the main line initialization   ;
;                  code for the TRNDD.  The code gets invoked via the  ;
;		   stategy routine INIT call and will verify the       ;
;                  proper adapter(s) is present and functional.        ;
;								       ;
;								       ;
; History:             						       ;
;								       ;
;     03/13/89  Mike Witkowski - Original Version 0.0		       ;
;     02/13/90  Mike Witkowski - Reworked for RIPL                     ;
;     08/20/90  Mike Witkowski - Fix receive buffer init code          ;
;								       ;
;**********************************************************************;
;**********************************************************************;

;-------------------------------------;
; Include all general companion files ;
;-------------------------------------;

		.XLIST
	       	INCLUDE includes.inc  	
		.LIST

PAGE_BREAK <Local Equates>
;---------------;
; Local Equates ;
;---------------;
BIOSRAMSEG      EQU     0040h           ; BIOS RAM Data Area Segment Address
BASEMEMSIZE     EQU     0013h           ; BIOS RAM Base Memory Size variable


PAGE_BREAK <External References>
;-----------------------------------;
; External Data/Variable References ;
;-----------------------------------;
		EXTRN	gbl_message:      WORD
		EXTRN	gbl_dynmem:       WORD
		EXTRN	DLCNetAddress:    BYTE
		EXTRN	gbl_open:         WORD
		EXTRN	gbl_initflag:     WORD
		EXTRN	DLCMaxFrameSize:  WORD


;--------------------------;
; External Code References ;
;--------------------------;
		EXTRN	Find_Card:	  NEAR
		EXTRN	OpenCard:	  NEAR
		EXTRN	VirttoPhys:	  NEAR


PAGE_BREAK <NDIInitialize Routine Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  NDIInitialize Routine                                ;
;                   					               ;
;  Description: The NDIInitialize routine is responsible for initial-  ;
;               izing the adapter.  This routine is called from the    ;
;               DLC RIPL code when it is ready to use the adapter to   ;
;               send out RIPL requests out to the network.             ;
;                                                                      ;
;  Input:       DI - location where to place node address              ;
;               AX - Base ROM memory segment                           ;
;                                                                      ;
;  Output:      AX - return code (0 = good completion)                 ;
;								       ;
;  Calls:       Find_Card, Config_ACB                                  ;
;								       ;
;  Called By:   DLC RIPL module                                        ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

START_CODE

                PUBLIC  NDIInitialize
NDIInitialize  	PROC 	NEAR

                push    ds                      ; Save data segment


;
;  Allocate space from high DOS memory and configure it for the 
;  ACB and supporting memory structures. 
;
                cmp     gbl_initflag, 0         ; Has driver already init'ed?
                SJNE    @F                      ;   Yes, skip allocation
                mov     gbl_initflag, 1         ;   No, set init flag
                call    Allocate_Memory         ; Allocate Memory
                                                ; (DS = allocated segment)
                mov     cs:gbl_dynmem, ds       ; Save global segment        

@@:
                mov     ds, cs:gbl_dynmem       ; Set global segment        
                call    Config_ACB              ; Configure ACB


;
;  Initialize the adapter.  First, we must verify the adapter exists at 
;  the locations specified and reset the hardware.  We must also setup 
;  the interrupt and DMA controllers correctly.  The Find_Card routine 
;  will set the appropriate status in the ACB as to whether the card 
;  is functional or not.    
;
		call	Find_Card		; Find and reset card

 
;
;  Now it is time to perform the interrupt check to verify that the
;  adapter can generate interrupts to the host processor.  This is 
;  done by setting the ACB state to INTTEST and issuing an SCB Clear 
;  request to the adapter.  
;
                cmp     [bx].acb_state, AS_INTTEST  ; Is state = test ?                   
                SJNE    init_exit                   ;   No, return error

                mov     dx, [bx].acb_sif_int    ; Set SIFINT address
                mov     ax, SIFINT_SCBREQST     ; Set SCB Request
                out     dx, ax                  ; Generate interrupt

                mov     si, 10                  ; Set count

int_test:
                xor     cx, cx                  ; Set loop count                
                
int_loop:
                in      ax, dx                      ; Read SIFINT reg
                cmp     [bx].acb_state, AS_INTTEST  ; Is state = test ?                   
                SJNE    int_next                    ;   No, get out
                loop    int_loop                    ; Keep waiting

                dec     si                      ; Is test over ?
                SJNE    int_test                ;   No, keep going

                SJMP    init_exit               ; IRQ failure, return error


;
; Set the maxmimum packet size that this network driver and hardware 
; can handle. 
;
int_next:
                mov     cs:DLCMaxFrameSize, RCVBUFSIZE  
                                                ; Set max packet size


;
; Now open the adapter onto the network. 
;
                mov     di, dyn_GRB             ; Point to GRB
                call    OpenCard                ; Open the adapter                

open_loop:
                cmp     [di].GRB_SCBPHi, SUCCESSFUL     ; Is it OK ?
                SJE     open_ok                         ;   Yes, process

                cmp     [di].GRB_SCBPHi, GENERAL_FAILURE; Did it fail ?
                SJNE    open_loop                       ;   No, keep waiting
                
;
;  The open adapter request failed.  Set error state and return. 
;  
                mov     [bx].acb_state, AS_OPENERROR    ; Set open error
                SJMP    init_exit                       ; Continue


;
;  The open request completed successfully.  Set good return state.
;		
open_ok:
                mov     [bx].acb_state, AS_OPENED       ; Set adapter open

                
;
;  At this point, the ACB state field contains the state of the 
;  adapter.  Use the state field value to index into the message 
;  table for the message to be displayed by the DLC module.  
;
init_exit:
                mov     di, [bx].acb_state      ; Get adapter state
                shl     di, 1                   ; Make it a word offset
                mov     ax, cs:gbl_message[di]  ; Get offset to message

                pop     ds                      ; Restore data segment
   		ret				; Return to caller

NDIInitialize  	ENDP


PAGE_BREAK <Configure ACB Routine Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  Configure ACB Memory Routine                         ;
;                   					               ;
;  Description: The Configure ACB Memory routine is resposible for     ;
;               setting up the transmit lists, receive lists, transmit ;
;               immediate buffers, and receive buffers in the ACB.     ;
;               This must be done at INIT time because we can not be   ;
;               sure what context the Open adapter routine will be     ;
;               called from, therefore, we can not be sure if we can   ;
;               use the VirttoPhys dev help call at that time.         ;
;                                                                      ;
;               This routine also sets up the maximum transmit and re- ;
;               ceive size parameters in the MAC service character-    ;
;               istics table.                                          ;
;                                                                      ;
;  Input:       DS - allocated memory segment                          ;
;                                                                      ;
;  Output:      modifies the ACB fields			               ;
;								       ;
;  Calls:       none                                                   ;
;								       ;
;  Called By:   init_driver                                            ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

Config_ACB  	PROC 	NEAR

                xor     bx, bx                  ; Point to ACB                


;----------------------------------------------------------------------;
;  First, copy the open parameter block mask into the ACB. 	       ;
;----------------------------------------------------------------------;
                push    ds                      ; Make ES = DS
                pop     es                      ;
                push    cs                      ; Make DS = CS
                pop     ds                      ;
                lea     di, [bx].acb_opn_blk    ; Point to open block
                lea     si, gbl_open            ; 
                mov     cx, size_OPEN           ; Set length
                rep     movsb                   ; Copy the block
                push    es                      ; Restore DS
                pop     ds                      ;


;----------------------------------------------------------------------;
;  Now, set up the transmit lists and the immediate transmit 	       ;
;  buffers in the ACB.  We'll start by doing the lists. 	       ;
;----------------------------------------------------------------------;
		mov	di, dyn_xmit	        ; Get ptr to xmit list area
		mov	cx, MAXMITS * XMITLISTS ; Calculate # of lists
		mov	ax, di			; Make a copy of list ptr
		mov	[bx].acb_xmit_head, di	; Set first list in free Q
		xor	si, si			; Clear previous list ptr	     
		push	bx			; Save ACB pointer
	
cnfg_xmit_cfg:
		add	ax, size_XMIT		; Point to next list
		mov	[di].XMIT_Next, ax	; Set next list pointer		
		mov	[di].XMIT_CSTAT, 0	; Clear cmd/status field
		push	ax			; Save next list pointer
		push	ds	 		; Pass the list's vitual
		lea	dx, [di].XMIT_FwdPtr	;   address to converter
		push	dx			;   routine.
		call	VirttoPhys		; Convert to physical addr
		pop	dx			; Get physical address
		pop	bx			;   passed from converter
		pop	ax			; Restore next list pointer

	        mov	[di].XMIT_MyMoto.Mhi, dx  ; Set hi physical addr word
		mov	[di].XMIT_MyMoto.Mlo, bx  ; Set lo physical addr word
		or	si, si			  ; Is there a previous list ?
		SJZ	cnfg_sp_skip		  ;   No, skip this

		mov	[si].XMIT_FwdPtr.Mhi, dx  ; Chain physical address in  
		mov	[si].XMIT_FwdPtr.Mlo, bx  ;   previous buffer.  
		SJMP	cnfg_cfg_cont		  ; Continue

cnfg_sp_skip:
		push	bx   			  ; Save the physical
		push	dx			  ;   address
					
cnfg_cfg_cont:		
		mov	si, di			; Save last tranmsit list
		add	di, size_XMIT		; Point to next list
		loop	cnfg_xmit_cfg		; Configure next list

		pop	[si].XMIT_FwdPtr.Mhi    ; Set last's fwd ptr to    
		pop	[si].XMIT_FwdPtr.Mlo    ;   first's physical addr   

		pop	bx			; Restore ACB pointer
		
		mov	ax, [bx].acb_xmit_head  ; Set last's pointer to
		mov	[si].XMIT_Next, ax	;    first in queue

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

cnfg_ibuf_cfg:
		lea	dx, [di+size_IBUF]	; Set address of imm data
		push	ds			; Put transmit immediate
		push	dx			;   buffer's physical 
		call	VirttoPhys	        ;   address into the 
		pop	[di].IBUF_MyMoto.Mhi	;   physical address field 
		pop	[di].IBUF_MyMoto.Mlo	;   of the buffer.

		mov	si, di			; Save last pointer

		add 	di, XMITIBUFSIZE + size_IBUF	
						; Point to next buffer

		loop	cnfg_ibuf_cfg		; Configure next buffer		 
		

;----------------------------------------------------------------------;
;  Now it is time to configure the receive lists and receive data      ;
;  buffers. 							       ;
;----------------------------------------------------------------------;
		mov	di, dyn_rcv     	; Get pointer to rcv memory
		mov	cx, MAXRCVS     	; Get number of rcv lists
		mov	[bx].acb_rcv_head, di	; Set first receive buffer		
		mov	ax, di
		xor	si, si			; Clear previous pointer			

cnfg_rcv_cfg:
		add	ax, RCVBUFSIZE + size_RCV   ; Point to next list
		mov	[di].RCV_Next, ax	    ; Set list pointer
		mov	[di].RCV_CSTAT, 0	    ; Clear cmd/status field

		push	ax                      ; Save next list pointer

		mov	ax, RCVBUFSIZE          ; Get receive buffer size,
		xchg	ah, al			;   make it motorola form, 
		mov	[di].RCV_Dsize, ax      ;   and put it into list.	

		push	ds	 		; Pass the list's vitual
		lea	ax, [di].RCV_FwdPtr	;   address to converter
		push	ax			;   routine.
		call	VirttoPhys		; Convert to physical addr
		pop	dx			; Get physical address
		pop	ax			;   passed from converter
		
		or	si, si			   ; Is previous buffer set ?
		SJZ	cnfg_save_rcv		   ;   No, save the address
		mov	[si].RCV_FwdPtr.Mhi, dx	   ;   Yes, set it's forward
		mov	[si].RCV_FwdPtr.Mlo, ax	   ;        pointer.
		pop	ax      		   ; Restore next list pointer
		SJMP	cnfg_rcv_cont		   ; Continue

cnfg_save_rcv:
		pop	si			; Restore next list pointer
		push	ax			; Save physical address
		push	dx			;
		mov	ax, si			; Set list pointer in AX

cnfg_rcv_cont:
		push	ax			; Save next list pointer
		push	ds	 		; Pass the buffer's vitual
		lea	ax, [di].RCV_Buf	;   address to converter
		push	ax			;   routine.
		call	VirttoPhys		; Convert to physical addr
		pop	[di].RCV_Dptr.Mhi	; Get physical address
		pop	[di].RCV_Dptr.Mlo	;   passed from converter

		mov	si, di			; Set previous list address
		pop	di			; Restore next list pointer
		mov	ax, di			; Restore next list pointer
		loop    cnfg_rcv_cfg		; Configure next list/buf	

		pop	[si].RCV_FwdPtr.Mhi	; Set last's forward ptr to
		pop	[si].RCV_FwdPtr.Mlo     ;   first's list address	

		mov	ax, [bx].acb_rcv_head	; Set last's pointer to
		mov	[si].RCV_Next, ax	;   first in rcv queue				


;----------------------------------------------------------------------;
;  The next memory initialization that must occur is to determine the  ;
;  physical address of the product ID for this adapter.                ;
;----------------------------------------------------------------------;
		push	ds 			; Make ES = DS
		lea	ax, [bx].acb_opn_blk.OPEN_ProdID
                                        	;   address as parameter
		push	ax			;   address as parm
		call	VirttoPhys		; Convert address to physical

 		pop	[bx].acb_opn_blk.OPEN_ProdIDPtr.Mhi
 		pop	[bx].acb_opn_blk.OPEN_ProdIDPtr.Mlo 
                                        	; Get physical address 


;----------------------------------------------------------------------;
;  The next memory initialization that must occur is to determine the  ;
;  physical address of the open parameter block for this adapter.      ;
;----------------------------------------------------------------------;
		push	ds	  		; Pass open parameter block 
		lea	ax, [bx].acb_opn_blk	;   address as parameter
		push	ax			;   to function
		call	VirttoPhys		; Convert address to physical
 		pop	[bx].acb_open_addr.Mhi	; Get physical address 
 		pop	[bx].acb_open_addr.Mlo	; 


;----------------------------------------------------------------------;
;  The next memory initialization that must occur is to determine the  ;
;  physical address of the SCB for this adapter.                       ;
;----------------------------------------------------------------------;
		push	ds	  		; Pass SCB address
		lea	ax, [bx].acb_scb	;   as parameter
		push	ax			;   to function
		call	VirttoPhys		; Convert address to physical
 		pop	[bx].acb_scb_addr.Mhi	; Get physical address 
 		pop	[bx].acb_scb_addr.Mlo	; 


;----------------------------------------------------------------------;
;  The next memory initialization that must occur is to determine the  ;
;  physical address of the SSB for this adapter.                       ;
;----------------------------------------------------------------------;
		push	ds	  		; Pass SCB address
		lea	ax, [bx].acb_ssb	;   as parameter
		push	ax			;   to function
		call	VirttoPhys		; Convert address to physical
 		pop	[bx].acb_ssb_addr.Mhi	; Get physical address 
 		pop	[bx].acb_ssb_addr.Mlo	; 
				
		ret				; Return to caller

Config_ACB  	ENDP


PAGE_BREAK <Alocate Memory Routine Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  Allocate Memory Routine                              ;
;                   					               ;
;  Description: The Allocate Memory routine allocates memory for the   ;
;               ACB, transmit buffers, and receive buffers.            ;
;                                                                      ;
;  Input:       none                                                   ;
;                                                                      ;
;  Output:      DS - Allocated memory segment                          ;
;								       ;
;  Calls:       none                                                   ;
;								       ;
;  Called By:   init_driver                                            ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

Allocate_Memory	PROC 	NEAR

;
;  Adjust BIOS RAM size to reserve the memory we need. 
;
                mov     ax, BIOSRAMSEG          ; Point to BIOS memory
                mov     ds, ax                  ;
                mov     ax, ds:BASEMEMSIZE      ; Get current RAM size
                sub     ax, MEMPAGES            ; Subtract what we need
                mov     ds:BASEMEMSIZE, ax      ; Set new RAM size


;
;  Set DS to point to new segment
;
                mov     cx, 40h                 ; Calculate the segment 
                mul     cx                      ;   address of our new memory
                mov     ds, ax                  ; Set up DS

		ret				; Return to caller

Allocate_Memory	ENDP


END_CODE	

		END
