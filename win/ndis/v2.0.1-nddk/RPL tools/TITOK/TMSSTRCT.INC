;**********************************************************************;
;**********************************************************************;
;								       ;
; File Name:       TMSSTRCT.INC                  		       ;
;								       ;
; Program Name:    TRNDD (Token-Ring Network Device Driver)	       ;
;								       ;
; Companion Files: None						       ;
;								       ;
; Function:        This module contains all the data strcuture defini- ;
;		   tions that are specific to the TMS380 software      ;
;		   interface specification.  The data structures       ;
;                  defined are as follows:                             ;     
;                                                                      ;     
;                - TMS380 Adapter Command Definitions                  ;
;                - TMS380 SIF Register Offset Definitions              ;
;                - Adapter Interrupt Register Bit Definition           ;     
;                - Adapter Configuration Register Bit Definition       ;     
;                - TMS380 Adapter Command Definitions                  ;     
;                - System Command Block (SCB)                          ;     
;                - System Status Block (SSB)                           ;     
;                - Adapter Initialization Parameter Block              ;     
;                - Adapter Open Parameter Block                        ;     
;                - Adapter Transmit Parameter List                     ;     
;                - Transmit CSTAT Bit Definitions                      ;     
;                - Transmit Status Bit Definitions                     ;     
;                - Adapter Receive Parameter List                      ;     
;                - Receive CSTAT Bit Definitions                       ;     
;                - Receive Status Bit Definitions                      ;     
;                - Read Error Log Buffer Definition                    ;     
;                                                                      ;     
; History:             						       ;
;								       ;
;     03/09/89  Mike Witkowski - Original Version 0.0		       ;
;     09/27/89  Mike Witkowski - Added "no interrupt" support.         ;
;     10/09/89  Mike Witkowski - Modified adapter buffer to be 1K.     ;
;     10/12/89  Mike Witkowski - Make product ID "non-IBM" ID.         ;
;     12/07/89  Mike Witkowski - Added frame format struc for filter.  ;
;     12/15/89  Mike Witkowski - Added support for GOODFS definition.  ;
;     01/29/90  Mike Witkowski - Added pass source address to xmit.    ;
;								       ;
;**********************************************************************;
;**********************************************************************;


;---------------------------------------------------------------------;
; Structure Name: TMS380 Adapter Command Definitions                  ;
; 								      ;
; Description: The TMS380 Adapter Commands Definitions define the     ;
;              reversed byte ordering of the TMS380 adapter command   ;
;              word values.
;---------------------------------------------------------------------;
TMS_RESET	EQU	0000h		; Reset Adapter (doesn't exist)
TMS_CMDREJECT	EQU	0200h		; Command Reject
TMS_OPEN	EQU	0300h		; Open Adapter
TMS_TRANSMIT	EQU	0400h		; Transmit Frame
TMS_XMITHALT	EQU	0500h		; Transmit Halt
TMS_RECEIVE	EQU	0600h		; Receive
TMS_CLOSE	EQU	0700h		; Close Adapter
TMS_SETGROUP	EQU	0800h		; Set Group Address
TMS_SETFUNCT	EQU	0900h		; Set Functional Address
TMS_READLOG	EQU	0a00h		; Read Error Log
TMS_READADP	EQU	0b00h	        ; Read Adapter Buffer


;---------------------------------------------------------------------;
; Structure Name: TMS380 SIF Register Offset Definitions              ;
; 								      ;
; Description: The TMS380 Register Offset Definitions describe the    ;
;              offsets to and number of TMS380 SIF registers.         ;
;---------------------------------------------------------------------;
NUMREGS		EQU	4

SIF_DATA_OFF	EQU	0		; SIF data register
SIF_DINC_OFF	EQU	2		; SIF data autoincrment register
SIF_ADDR_OFF	EQU	4		; SIF address register
SIF_INT_OFF	EQU	6		; SIF interrupt register
SIF_ACTL_OFF    EQU     8               ; SIF ACTL register


;---------------------------------------------------------------------;
; Structure Name: Adapter Control Register Bit Definitions            ;
; 								      ;
; Description: The Adapter Control Register Bit Definitions define    ;
;              functions of the individual bits in the Adapter Con-   ;
;              trol register of the EAGLE chip.  Bit combinations are ;
;	       also defined here.                                     ;
;---------------------------------------------------------------------;
ACTL_SWHLDA	EQU	0800h		; Software Hold Acknowledge 
ACTL_SWDDIR	EQU	0400h		; Current SDDIR signal level
ACTL_SWHRQ	EQU	0200h		; Software Hold Request 
ACTL_PSDMALEN	EQU	0100h		; Psuedo System DMA Length
ACTL_ARESET	EQU	0080h		; Adapter Reset
ACTL_CPHALT	EQU	0040h	        ; Comm Processor Halt
ACTL_BOOT	EQU	0020h		; Bootstrapped CP Code
ACTL_ROM	EQU	0010h	        ; Reserved
ACTL_SINTEN	EQU	0008h		; System Interrupt Enable
ACTL_PEN	EQU	0004h		; Adapter Parity Enable
ACTL_INPUT0	EQU	0002h		; Reserved


;---------------------------------------------------------------------;
; Structure Name: Adapter Interrupt Register Bit Definitions          ;
; 								      ;
; Description: The Adapter Interrupt Register Bit Definitions define  ;
;              functions of the individual bits in the System Inter-  ;
;              rupt register of the TMS380 chipset.  Bit combinations ;
;	       are also defined here.                                 ;
;---------------------------------------------------------------------;
SIFINT_ADPINT	EQU	8000h		; Adapter Interrupt 
SIFINT_RESET	EQU	4000h		; Reset request
SIFINT_SSBCLR	EQU	2000h		; SSB Clear 
SIFINT_EXECUTE	EQU	1000h		; Execute Command
SIFINT_SCBREQ	EQU	0800h		; SCB Clear request
SIFINT_RCVCON	EQU	0400h	        ; Receive Continue
SIFINT_RCVVLD	EQU	0200h		; Receive Valid
SIFINT_XMTVLD	EQU	0100h	        ; Transmit Valid
SIFINT_SYSINT	EQU	0080h		; System Interrupt

SIFINT_CMD	EQU	SIFINT_ADPINT + SIFINT_EXECUTE + SIFINT_SYSINT
SIFINT_SSBCLEAR	EQU	SIFINT_ADPINT + SIFINT_SSBCLR + SIFINT_SYSINT
SIFINT_SCBREQST	EQU	SIFINT_ADPINT + SIFINT_SCBREQ + SIFINT_SYSINT
SIFINT_RCVVALID	EQU	SIFINT_ADPINT + SIFINT_RCVVLD + SIFINT_SYSINT
SIFINT_XMTVALID	EQU	SIFINT_ADPINT + SIFINT_XMTVLD + SIFINT_SYSINT

INT_ADPCHECK	EQU	0000h		; Adapter Check Interrupt 
INT_RINGSTAT	EQU	0004h		; Ring Status Interrupt 
INT_SCBCLEAR	EQU	0006h		; SCB Clear Interrupt 
INT_COMMAND	EQU	0008h		; Command Interrupt 
INT_TRANSMIT	EQU	000ch		; Transmit Interrupt 
INT_RECEIVE	EQU	000ah		; Receive Interrupt 


;---------------------------------------------------------------------;
; Structure Name: Adapter Control Register Bit Definitions            ;
; 								      ;
; Description: The Adapter Control Register Bit Definitions define    ;
;              the functions of the individual bits in the control    ;
;              register of the 3COM Token-Ring board.  Bit  combin-   ;
;              ations are also defined here.                          ;
;---------------------------------------------------------------------;
DMASEL		EQU	4000h		; DMA Select (0 = 1, 1 = 3)
ISELB		EQU	2000h		; Interrupt select
ISELA		EQU	1000h		; Interrupt select
LPBK		EQU	0800h		; Loop Back Select
RFSH		EQU	0400h		; Refresh select (0 = yes)		
NWAIT		EQU	0200h		; Wait State Select (0 = yes) 
SHADOW		EQU	0100h		; Shadow Registers Select (1 = norm)
SIFACK		EQU	0080h		; SIF Transfer Acknowledge
DINTEN		EQU	0040h		; DMA Done Interrupt Enable
RINTEN		EQU	0020h		; SIF Data Request Interrupt Enable
SINTEN		EQU	0010h		; SIF Interrupt Enable
DMAEN		EQU	0008h		; DMA Enable
MASTER		EQU	0004h		; Master mode Enabled
DDIR		EQU	0002h		; DMA Direction
SIFRST		EQU	0001h		; Reset Adapter


;---------------------------------------------------------------------;
; Structure Name:  System Command Block (SCB) Structure Definition    ;
; 								      ;
; Description: The System Command Block Structure Definition defines  ;
;              the structure of the TMS380 based SCB block.           ;
;---------------------------------------------------------------------;
SCB_Block	STRUC
SCB_Cmd		DW	?		; SCB Command field
SCB_Ptr		DD	?	        ; SCB Pointer field
SCB_Block	ENDS

size_SCB	EQU	SIZE SCB_Block
	

;---------------------------------------------------------------------;
; Structure Name:  System Status Block (SSB) Structure Definition     ;
; 								      ;
; Description: The System Status Block Structure Definition defines   ;
;              the structure of the TMS380 based SSB block.  Also in- ;
;              cluded are the SSB status field bit definitions.       ;
;---------------------------------------------------------------------;
SSB_Block	STRUC
SSB_Cmd		DW	?		; SSB Command field
SSB_Status	DW	?		; SSB Status field
SSB_Ptr		DD	?	        ; SSB Pointer field
SSB_Block	ENDS

size_SSB	EQU	SIZE SSB_Block

SSB_GOOD	EQU	0080h		; SSB Command successful status


;---------------------------------------------------------------------;
; Structure Name:  Adapter Initialization Parameter Block Definition  ;
; 								      ;
; Description: The Adapter Initialization Parameter Block Definition  ;
;              defines the structure of the TMS380 based initiali-    ;
;              zation block.  					      ;
;---------------------------------------------------------------------;
INIT_Block	STRUC
INIT_Options	DW	9f00h		; Initialization Options	       
INIT_Vectors	DB	6 DUP(0)       	; Interrupt vector codes
INIT_Rburst	DW	0		; Receive DMA burst size	       
INIT_Xburst	DW	0		; Transmit DMA burst size	       
INIT_DMARetry	DW	0505h		; DMA retry counts	       
INIT_Block	ENDS

size_INIT	EQU	SIZE INIT_Block


;---------------------------------------------------------------------;
; Structure Name:  Open Adapter Parameter Block Structure Definition  ;
; 								      ;
; Description: The Open Adapter Parameter Block Structure Definition  ;
;              defines the structure of the TMS380 based parameter    ;
;              block passed to the adapter on an Open Adapter request.;
;	       All parameter defaults listed here are already byte    ;
;              swapped for DMA into the adapter.  Open options are    ;
;              also defined here.                                     ;	
;---------------------------------------------------------------------;
OPEN_Block	STRUC		    	
OPEN_Options	DW	?		; Open options
OPEN_NodeAddr	DB	6 DUP (0)	; Adapter node address
OPEN_GroupAddr	DB	4 DUP (0)	; Adapter group address
OPEN_FunctAddr	DB	4 DUP (0)	; Adapter functional address
OPEN_RLSize	DW	0e00h		; Receive list size
OPEN_XLSize	DW	1a00h		; Transmit list size
OPEN_BufSize	DW	0004h		; Adapter buffer size (1K)
OPEN_RAMStart	DW	00640h		; Adapter RAM start address
OPEN_RAMEnd	DW	0fe7fh		; Adapter RAM end address
OPEN_Xbufmin	DB	06h		; Adapter xmit min buf count
OPEN_Xbufmax	DB	08h		; Adapter xmit max buf count
OPEN_ProdIdPtr	DD	?		; Product ID pointer
OPEN_ProdID     DB      09h, 10h        ; Product ID
                DB      0f0h, 0f0h, 0f0h, 0f0h, 0f0h, 0f0h, 0f0h, 0f0h
                DB      0f0h, 0f0h, 0f0h, 0f0h, 0f0h, 0f0h, 0f0h, 0f0h
                DB      0f0h, 0f0h, 0f0h, 0f0h, 0f0h, 0f0h, 0f0h, 0f0h
                DB      0f0h, 0f0h, 0f0h, 0f0h, 0f0h, 0f0h, 0f0h, 0f0h
                DB      0f0h, 0f0h
OPEN_Block	ENDS

size_OPEN	EQU	SIZE OPEN_Block


OOPTS_WRAP      EQU     8000h           ; Open Wrap Mode
OOPTS_DHARD     EQU     4000h           ; Disable Hard Errors
OOPTS_DSOFT     EQU     2000h           ; Disable Soft Errors
OOPTS_PADPM     EQU     1000h           ; Pass Adapter MAC Frames
OOPTS_PATTM     EQU     0800h           ; Pass Attention MAC Frames
OOPTS_PADR      EQU     0400h           ; Pad Routing Field
OOPTS_FHOLD     EQU     0200h           ; Frame Hold 
OOPTS_CONT      EQU     0100h           ; Contender
OOPTS_PBCNM     EQU     0080h           ; Pass Beacon MAC Frames
OOPTS_ETR       EQU     0040h           ; Early Token Release
OOPTS_CMAC      EQU     0004h           ; Copy All MAC Frames
OOPTS_CNMAC     EQU     0002h           ; Copy All Non-MAC Frames
OOPTS_FONLY     EQU     0001h           ; Pass First Buffer Only


;---------------------------------------------------------------------;
; Structure Name:  Transmit List Structure Definition                 ;
; 								      ;
; Description: The Transmit List Structure Definition defines the     ;
;              structure of the TMS380 based Transmit Parameter List. ;
;              Also included are the definitions of the TRNDD parti-  ;
;              cular fields that have been added to the end of the    ;
;              list structure to allow easy management of the trans-  ;
;              mit process.                                           ;
;---------------------------------------------------------------------;
XMIT_List	STRUC
XMIT_Next	DW	? 		; Intel pointer to next list
XMIT_FwdPtr	DD	?		; Motorola pointer to next list
XMIT_CSTAT	DW	?		; Command/Status field
XMIT_Fsize	DW	?	        ; Frame size
XMIT_Dptrs	DB	18 DUP (?)	; Transmit list data pointers
XMIT_MyMoto	DD	?		; My motorola address
XMIT_ED	        DD	?	        ; ED Pointer
XMIT_ImmPtr	DW	?		; Immediate buffer pointer address
XMIT_EOF	DW	?		; End of Frame list pointer
XMIT_List	ENDS

size_XMIT	EQU 	SIZE XMIT_List	; Size of transmit list

ODD_PTR		EQU	00100h	  	; Odd forward pointer mask	
EVEN_PTR	EQU	0feffh		; Even forward pointer mask


;---------------------------------------------------------------------;
; Structure Name:  Transmit List Data Pointer Structure Definition    ;
; 								      ;
; Description: The Transmit List Data Pointer Structure Definition    ;
;              defines the structure of the data pointers contained   ;
;              in the TMS380 based Transmit Parameter List.           ;
;---------------------------------------------------------------------;
XDP_Struc	STRUC
XDP_Length	DW	?	      	; Data pointer's data length
XDP_Ptr		DD	?	        ; Data pointer's data address
XDP_Struc	ENDS

size_XDP	EQU	SIZE XDP_Struc	; Size of xmit list data pointer

DATA_NOT_LAST	EQU	00080h		; "Not last" mask for length field
DATA_LAST	EQU	0ff7fh		; "Last" mask for length field


;---------------------------------------------------------------------;
; Structure Name:  Transmit List CSTAT Bit Definitions                ;
; 								      ;
; Description: The Transmit List CSTAT Bit Definitions defines the    ;
;              meaning of the bits in the CSTAT field of the transmit ;
;              list.                                                  ;
;---------------------------------------------------------------------;
XCSTAT_VALID	EQU	0080h		; Transmit Valid 
XCSTAT_COMPLETE	EQU	0040h		; Transmit Frame Complete
XCSTAT_SOF	EQU	0020h	        ; Transmit Start of Frame 
XCSTAT_EOF	EQU	0010h		; Transmit End of Frame
XCSTAT_FINT	EQU	0008h		; Transmit Frame Interrupt
XCSTAT_ERROR	EQU	0004h	        ; Transmit Error
XCSTAT_PASSSRC	EQU	0001h	        ; Transmit Pass Source Address
XCSTAT_GOODFS   EQU     8800h           ; Good Transmit FS btye

XCSTAT_LSOF	EQU	00a0h		; Transmit Start of Frame


;---------------------------------------------------------------------;
; Structure Name:  Transmit SSB Status Codes Definitions              ;
; 								      ;
; Description: The Transmit SSB Status Codes Definitions defines the  ;
;              meaning of the bits in the Status field of the trans-  ;
;              mit completion SSB.                                    ;
;---------------------------------------------------------------------;
XSTAT_CMDCMPLT	EQU	0080h		; Command Complete
XSTAT_FRMCMPLT	EQU	0040h		; Frame Complete
XSTAT_LERROR	EQU	0020h		; List Error


;---------------------------------------------------------------------;
; Structure Name:  Receive List Structure Definition                  ;
; 								      ;
; Description: The Receive List Structure Definition defines the      ;
;              structure of the TMS380 based Receive Parameter List.  ;
;              Also included are the definitions of the TRNDD parti-  ;
;              cular fields that have been added to the end of the    ;
;              list structure to allow easy management of the receive ;
;              process.                                               ;
;---------------------------------------------------------------------;
RCV_List	STRUC
RCV_Next	DW	? 		; Intel pointer to next list
RCV_FwdPtr	DD	?		; Motorola pointer to next list
RCV_CSTAT	DW	?		; Command/Status field
RCV_Fsize	DW	?	        ; Frame size
RCV_Dsize	DW	?  		; Receive list data size 
RCV_Dptr	DD	?		; Receive list data pointer 
RCV_Buf		DW	?		; Receive buffer 
RCV_List	ENDS

size_RCV	EQU 	SIZE RCV_List - 2	; Size of transmit list


;---------------------------------------------------------------------;
; Structure Name:  Receive List CSTAT Bit Definitions                 ;
; 								      ;
; Description: The Receive List CSTAT Bit Definitions defines the     ;
;              meaning of the bits in the CSTAT field of the receive  ;
;              list.                                                  ;
;---------------------------------------------------------------------;
RCSTAT_VALID	EQU	0080h		; Receive Valid 
RCSTAT_COMPLETE	EQU	0040h		; Receive Frame Complete
RCSTAT_SOF	EQU	0020h	        ; Receive Start of Frame 
RCSTAT_EOF	EQU	0010h		; Receive End of Frame
RCSTAT_FINT	EQU	0008h		; Receive Frame Interrupt

IFDEF NOINTS
RCSTAT_GO	EQU	0080h		; Receive Valid/Frame Interrupt
ELSE
RCSTAT_GO	EQU	0088h		; Receive Valid/Frame Interrupt
ENDIF


;---------------------------------------------------------------------;
; Structure Name:  Token-Ring Frame Format                            ;
; 								      ;
; Description: The Token-Ring Frame Format structure defines the      ;
;              fields of a valid Token-Ring frame.                    ;
;---------------------------------------------------------------------;
Frame_Format    STRUC   
FF_ACFC 	DW	?               ; AC/FC Field
FF_DEST0        DW      ?               ; Destination Address
FF_DEST1        DW      ?               ; Destination Address
FF_DEST2        DW      ?               ; Destination Address
FF_SRC0         DW      ?               ; Source Address
FF_SRC1         DW      ?               ; Source Address
FF_SRC2         DW      ?               ; Source Address
FF_RI           DW      ?               ; Routing Field
Frame_Format    ENDS   


;---------------------------------------------------------------------;
; Structure Name:  Read Error Log Buffer Structure Definition         ;
; 								      ;
; Description: The Read Error Log Buffer Structure Definition defines ;
;              the structure of the TMS380 based Read Error Log.      ;
;---------------------------------------------------------------------;
REL_Block	STRUC
REL_LineError	DB	?
REL_Rsvd1	DB	?
REL_BurstError	DB	?
REL_ARIFCIError	DB	?
REL_Rsvd2	DB	?
REL_Rsvd3	DB	?
REL_LostError	DB	?
REL_Congestion	DB	?
REL_CopiedError	DB	?
REL_Rsvd4	DB	?
REL_TokenError	DB	?
REL_Rsvd5	DB	?
REL_DMABUSError	DB	?
REL_DMAPARError	DB	?
REL_Block	ENDS

size_REL	EQU	SIZE REL_Block
