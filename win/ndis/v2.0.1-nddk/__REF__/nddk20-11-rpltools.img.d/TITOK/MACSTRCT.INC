;**********************************************************************;
;**********************************************************************;
;								       ;
; File Name:       MACSTRCT.INC                  		       ;
;								       ;
; Program Name:    TRNDD (Token-Ring Network Device Driver)	       ;
;								       ;
; Companion Files: None						       ;
;								       ;
; Function:        This module contains all the data strcuture defini- ;
;		   tions of structures that are internally kept by the ;
;                  NDI driver to go about its operations. The data     ;
;                  structures defined are as follows:                  ;     
;                                                                      ;     
;                - Global Device Driver State Equates	               ;
;                - MAC Internal Adapter Control Block                  ;     
;                - Adapter Sub-section Table Structure                 ;     
;                - Initialization Code Pointer Table                   ;     
;                - Adapter Specific Information Table                  ;     
;                - Recognized Parameter Table                          ;     
;                                                                      ;     
;                  In addition, this module also defines those PC      ;
;                  related structures that are necesary for the device ;
;                  driver to do its job.  These structures include:    ;
;                						       ;
;                - PC 8259A Interrupt controller locations             ;                                       
;                - PC 8237A-5 DMA controller locations                 ;
;                                                                      ;     
; History:             						       ;
;								       ;
;     03/09/89  Mike Witkowski - Original Version 0.0		       ;
;     02/13/90  Mike Witkowski - Reworked for RIPL                     ;
;								       ;
;**********************************************************************;
;**********************************************************************;

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;                 TRNDD INTERNAL STRUCTURES AND EQUATES                ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

;---------------------------------------------------------------------;
; Structure Name: Global Device Driver State Equates                  ;
; 								      ;
; Description: The Global Device Driver State Equates describe the    ;
;              major functional states that the TRNDD may enter.      ;
;              When the TRNDD is first loaded into memory, its state  ;
;              is QUIESCENT.  Once the initialization routine is      ;
;              called, the driver may enter the ERROR state, or the   ;
;              OPERATIONAL state depending on the outcome of the      ;
;              ininitialization process.                              ;
;---------------------------------------------------------------------;
QUIESCENT	EQU	0
ERROR		EQU	1
OPERATIONAL	EQU	2


;---------------------------------------------------------------------;
; Structure Name: MAC Internal Adapter Control Block (ACB)            ;
; 								      ;
; Description: The MAC internal adapter control block contains all    ;
;              internal variables for ONE SINGLE adapter.  For each   ;
;              adapter supported by this driver, there is a Master    ;
;              Control Block defined which contains a MAC internal    ;
;              adapter control block as a component.  The variables   ;
;              in the ACB are used by the NDI driver to maintain      ;
;              internal statistics, driver states, and resources.     ;
;---------------------------------------------------------------------;
adapter_control STRUC
acb_state	DW	0    		; Adapter Primary State
acb_adpnum	DW	0		  ; Adapter Number (0 - MAXADPS)
acb_scb		DB      size_SCB DUP (0)  ; Adapter System Control Block
acb_ssb		DB	size_SSB DUP (0)  ; Adapter System Status Block
acb_scb_addr	DD	0		  ; Physical address of SCB
acb_ssb_addr	DD	0		; Physical address of SSB
acb_adp_cfg	DW	0		; Adapter configuration register
acb_sif_data	DW	0	        ; Adapter SIF data register
acb_sif_dinc	DW	0		; Adapter SIF data autoincrement reg
acb_sif_addr	DW	0		; Adapter SIF address register
acb_sif_int	DW	0		; Adapter SIF interrupt register
acb_sif_actl    DW      0               ; Adapter SIF adapter control reg
acb_sif_addrx   DW      0               ; Adapter SIF extended address reg
acb_opn_blk	DB	size_OPEN DUP(0) ; Adapter open block
acb_open_addr	DD	0	        ; Address of open parm block
acb_log_buf	DB	size_REL DUP(0) ; Adapter error log
acb_intnest	DW	0		; Interrupt depth counter
acb_flags	DB	0		; General Flag Byte
acb_int_mask  	DB	0		; Adapter interrupt enable mask
acb_ibuf_free	DW	0		; Transmit immediate buffer free Q
acb_xlist_cnt	DW	0		; Count of available transmit lists
acb_req_next	DW	0		; Next request to go to SSB
acb_xmit_head	DW	0		; Next available transmit list
acb_xmit_whead	DW	0		; Transmit list queue waiting for SCB
acb_xmit_wtail	DW	0		; Transmit list queue waiting for SCB
acb_xmit_ahead	DW	0	        ; Transmit list queue being serviced
acb_xmit_atail	DW	0		; Transmit list queue being serviced
acb_rcv_head	DW	0		; Receive list queue pointer
adapter_control ENDS

acb_size	EQU	SIZE adapter_control	; ACB size


;---------------------------------------------------------------------;
; Structure Name: ACB Adapter States (AS)                             ;
; 								      ;
; Description: These equates define the primary states that an        ;
;              adapter may take on.                                   ;
;---------------------------------------------------------------------;
AS_NOTINSTALLED	EQU	0		; Adapter not installed
AS_CARDERROR	EQU	1		; Adapter reset error
AS_INITERROR	EQU	2		; Adapter initialization error
AS_INSTALLED	EQU	3		; Adapter installed/not reset
AS_IRQERROR	EQU	4		; Adapter IRQ error
AS_DMAERROR	EQU	5		; Adapter DMA error
AS_DOWNERROR	EQU	6		; Adapter download error
AS_MEDIAERROR	EQU	7		; Adapter media error
AS_SPEEDERROR	EQU	8		; Adapter ring speed error
AS_RESET	EQU	9	        ; Adapter reset
AS_INITIALIZED	EQU	10		; Adapter initialized
AS_OPENERROR	EQU	11		; Adapter open error
AS_OPENED	EQU	12		; Adapter Opened
AS_HARDERROR	EQU	13		; Adapter suffered hardware error
AS_INTTEST	EQU	14		; Driver performing int test

STATE_BAD       EQU     0               ; Value to indicate inoperable state
                         

;---------------------------------------------------------------------;
; Structure Name: ACB Flags                                           ;
; 								      ;
; Description: These equates define the various states of the gen-    ;
;              eral flag byte variable.  The following flags are de-  ;
;              fined:                                                 ;
;                                                                     ;
;              INTERRUPT INDICATION PENDING - This flag is used to    ;
;              signal when an indication complete should be issued    ;
;              to the upper protocol driver.                          ;
;                                                                     ;
;              INTERRUPT VALID - This flag is used inside the adapter ;
;              interrupt handler to indicate when a valid interrupt   ;
;              has been received by the driver.                       ;
;---------------------------------------------------------------------;
INT_PENDING	EQU	01h
INT_SCBPENDING  EQU     02h
INT_REENTER     EQU     04h
INT_VALID       EQU     80h


;---------------------------------------------------------------------;
; Structure Name: ACB Sub-section Table Element                       ;
; 								      ;
; Description: The ACB Sub-section Table Element structure is used    ;
;              during initialization time to allocate the subsections ;
;              of the ACB that are of variable size.                  ;
;---------------------------------------------------------------------;
sub_table_elmt	STRUC
sub_size	DW	? 		; Subsection size (# of elements)
sub_size_ptr	DW	?		; Offset to size field in MCB
sub_ptr		DW	?		; Offset to ptr field within MCB
sub_mult	DW	?		; Size Multiplier
sub_table_elmt	ENDS

size_sub_table	EQU	SIZE sub_table_elmt


;---------------------------------------------------------------------;
; Structure Name: Adapter Specific Information Table                  ;
; 								      ;
; Description: The Adapter Specific Information Table contains all    ;
;              the adapter specific information that was acquired at  ;
;              the time the ConfigMemoryImage was parsed.             ;
;---------------------------------------------------------------------;
adp_specific	STRUC
adp_int		DW MAX_ADPS DUP (?) 	; Adapter interrupt level 
adp_dma		DW MAX_ADPS DUP (?)	; Adapter dma level 
adp_base	DW MAX_ADPS DUP (?)	; Adapter base address 
adp_speed	DW MAX_ADPS DUP (?)	; Adapter media speed
adp_mtype	DW MAX_ADPS DUP (?)	; Adapter media type
adp_specific	ENDS


;---------------------------------------------------------------------;
; Structure Name: General Request Block Structure Definition          ;
; 								      ;
; Description: The General Request Block Structure Definition defines ;
;              the structure of the internal general requests blocks  ; 
;              that are used to keep track of queued general requests ;
;              made by the upper protocol driver.                     ;
;---------------------------------------------------------------------;
GRB_Block	STRUC
GRB_Ptr		DW	?		  	; Forward Pointer
GRB_ProtID	DW	?			; Protocol Module ID
GRB_Handle	DW	?			; Request Handle
GRB_Opcode	DW	?			; Request Op code
GRB_Parm1	DW	?			; Request Parm #1
GRB_Parm2	DD	?			; Request Parm #2
GRB_SCBCmd	DW	?			; Dummy SCB command
GRB_SCBPHi	DW	?			; Dummy SCB Hi Ptr
GRB_SCBPLo	DW	?			; Dummy SCB Lo Ptr
GRB_Block	ENDS

GENREQSIZE	EQU	SIZE GRB_Block   	; General request size


;---------------------------------------------------------------------;
; Structure Name: Immediate Buffer Block Structure Definition         ;
; 								      ;
; Description: The Immediate Buffer Block Structure Definition de-    ;
;              fines the structure of the internal immediate buffer   ;
;              blocks that are used to temporarily store transmit     ;
;              immediate data.                                        ;
;---------------------------------------------------------------------;
IBUF_Struc	STRUC
IBUF_Ptr	DW	?		; Pointer to next buffer
IBUF_Reserved	DW	?	        ; Reserved
IBUF_MyMoto	DD	?		; This buffer's motorola address
IBUF_Struc	ENDS

size_IBUF	EQU	SIZE IBUF_Struc


;---------------------------------------------------------------------;
; Structure Name: Important Driver Default Values                     ;
; 								      ;
; Description: The Important Driver Default Values used to configure  ;
;              driver memory size.                                    ;
;---------------------------------------------------------------------;
XMITIBUFSIZE	EQU	14		; Xmit immediate buffer size
RCVBUFSIZE	EQU	512		; Same as above	
MAXRCVS         EQU     3               ; Maximum number of receives
RCVSIZE         EQU     MAXRCVS*(size_RCV+RCVBUFSIZE)

MAXTXBUFS       EQU     2               ; Maximum number of descriptors
XMITLISTS	EQU	1  
XMITMULT	EQU	(XMITLISTS * size_XMIT) + XMITIBUFSIZE  + size_IBUF  
					; Xmit element size
MAXMITS         EQU     8               ; Maximum number of xmit requests
XMITSIZE        EQU     MAXMITS*XMITMULT

MEMPAGES        EQU     ((acb_size+XMITSIZE+RCVSIZE) - 1)/400h + 1


;---------------------------------------------------------------------;
; Structure Name: Dynamic Memory Structure Definition                 ;
; 								      ;
; Description: The Dynamic Memory Structure Definition defines the    ;
;              structure of the memory allocated to hold the ACB and  ;
;              other control data structures.                         ;
;---------------------------------------------------------------------;
dynamic_struct  STRUC
dyn_acb         DB      acb_size DUP (?)        ; ACB
dyn_GRB         DB      GENREQSIZE DUP (?)      ; General Request Block
dyn_xmit        DB      XMITSIZE DUP(?)         ; Transmit structures
dyn_rcv         DB      RCVSIZE DUP (?)         ; Receive structures              
dynamic_struct  ENDS


;----------------------------------------------------------------------;
; Structure name: NDI Return Codes                                     ;
;								       ;
; Description: The valid return codes that may be return from the NDI  ;
;              driver.                                                 ;
;----------------------------------------------------------------------;
SUCCESSFUL		EQU	0000h				     
REQUEST_QUEUED		EQU	0002h	
OUT_OF_RESOURCE		EQU	0006h	
INVALID_PARAMETER	EQU	0007h	
INVALID_FUNCTION	EQU	0008h	
NOT_SUPPORTED		EQU	0009h	
HARDWARE_ERROR  	EQU	000ah	
TRANSMIT_ERROR  	EQU	000bh	
NO_SUCH_DESTINATION	EQU	000ch	
HARDWARE_NOT_FOUND	EQU 	0023h	 
HARDWARE_FAILURE	EQU 	0024h
CONFIGURATION_FAILURE	EQU 	0025h
INTERRUPT_CONFLICT	EQU 	0026h
INITIALIZATION_FAILED	EQU 	0028h
GENERAL_FAILURE		EQU	00ffh


;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;            PERSONAL COMPUTER RELATED STRUCTURES AND EQUATES          ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

;---------------------------------------------------------------------;
; Structure Name: PC 8259A Interrupt Controller Locations             ;
; 								      ;
; Description: Obvious...                                             ;
;---------------------------------------------------------------------;
INT_CTRL1	EQU	020h		; 8259A Int Controller #1
INT_CTRL1_MR	EQU	021h		; 8259A Int Controller #1
INT_CTRL1_ELR	EQU	4d0h		; 8259A Int Controller #1
INT_CTRL2	EQU	0a0h		; 8259A Int Controller #2
INT_CTRL2_MR	EQU	0a1h		; 8259A Int Controller #2
INT_CTRL2_ELR	EQU	4d1h		; 8259A Int Controller #2

EOI_SPECIFIC	EQU	060h		; Specific interrupt EOI
	

;---------------------------------------------------------------------;
; Structure Name: PC 8237A-5 DMA Controller Locations                 ;
; 								      ;
; Description: Obvious...                                             ;
;---------------------------------------------------------------------;
DMA_CTRL_MODE1	EQU	0bh		; 8237A-5 #1 Mode Register 
DMA_CTRL_WSMB1	EQU	0ah		; 8237A-5 #1 Write Single Mask Bit
DMA_CTRL_MODE2	EQU	0d6h		; 8237A-5 #2 Mode Register 
DMA_CTRL_WSMB2	EQU	0d4h		; 8237A-5 #2 Write Single Mask Bit
