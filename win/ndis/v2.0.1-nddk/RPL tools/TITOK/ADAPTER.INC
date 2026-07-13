;**********************************************************************;
;**********************************************************************;
;								       ;
; File Name:       ADAPTER.INC                    		       ;
;								       ;
; Program Name:    TRNDD (Token-Ring Network Device Driver)	       ;
;								       ;
; Companion Files: None						       ;
;								       ;
; Function:        This module contains all the adapter specific macro ;
;		   definitions for the macros used throughout the      ;
;		   TRNDD source code.  The macros defined include the  ;
;                  following:                                          ;     
;                                                                      ;     
;                - INTDMAENABLE: Enable adapter interrupts and DMA     ;     
;                - INTENABLE: Enable adapter interrupts                ;     
;                - INTDISABLE: Disable adapter interrupts              ;     
;                                                                      ;     
;                  This module also contains all the equates that are  ;
;                  adapter specific.                                   ;
;                                                                      ;     
; History:             						       ;
;								       ;
;     04/20/89  Mike Witkowski - Original Version 0.0		       ;
;     11/09/89  Mike Witkowski - Added support for Proteon P1390/1     ;
;								       ;
;**********************************************************************;
;**********************************************************************;

;----------------------------------------------------------------------;
; Adapter Bus Type Equate definitions -                                ;
;                These equates define the adapter bus types allowed.   ;
;----------------------------------------------------------------------;
ISA             EQU     0               ; ISA Bus Type
EISA            EQU     1               ; EISA Bus Type

;----------------------------------------------------------------------;
; Proteon P1345 Configuration Register definitions -                   ;
;                These equates define the bit settings in the P1345    ;
;                adapter's configuration register.                     ;
;----------------------------------------------------------------------;
PCFG_SHRQ 	EQU	80h		; SHRQ (Read only)
PCFG_FRAQ  	EQU	40h		; FRAQ (Read only)
PCFG_INTS_ON 	EQU	20h		; Interrupt Enable
PCFG_INSERT  	EQU	10h		; Ring Insert (Read only)
PCFG_DMAS_ON 	EQU	08h		; DMA Enable		
PCFG_RESET   	EQU	04h		; Reset 
PCFG_ADPTYPE 	EQU	02h		; Adapter type (1 = AT)
PCFG_WRAP    	EQU	01h		; Wrap status (read only)

PCFG_INTDMA_EN	EQU	PCFG_INTS_ON + PCFG_DMAS_ON

PCFG_INTS_OFF	EQU	0dfh		; Interrupt disable mask


;----------------------------------------------------------------------;
; Proteon P1390 Configuration Register definitions -                   ;
;                These equates define the bit settings in the P1390    ;
;                adapter's configuration register.                     ;
;----------------------------------------------------------------------;
PCFG9_MTYPE 	EQU	80h		; Media Type (1=I, 0=III)
PCFG9_RSVD  	EQU	40h		; Reserved (0)
PCFG9_MSPEED 	EQU	20h		; Media Speed (0=16, 1=4)
PCFG9_LED  	EQU	10h		; LED control (0=off, 1=on)
PCFG9_INTS_ON 	EQU	08h		; Interrupt Enable
PCFG9_RESET   	EQU	04h		; Reset 
PCFG9_816 	EQU	02h		; Eagle 8/16 select
PCFG9_STYPE    	EQU	01h		; Slot type (0=16bit, 1=8bit)


;----------------------------------------------------------------------;
; 3COM 3C603 Configuration Register definitions -                      ;
;                These equates define the bit settings in the 3C603    ;
;                adapter's configuration register.                     ;
;----------------------------------------------------------------------;
TCFG_DMASEL  	EQU	4000h		; DMA Select (0 = 1, 1 = 3)
TCFG_ISELB   	EQU	2000h		; Interrupt select
TCFG_ISELA   	EQU	1000h		; Interrupt select
TCFG_LPBK    	EQU	0800h		; Loop Back Select
TCFG_RFSH    	EQU	0400h		; Refresh select (0 = yes)		
TCFG_NWAIT   	EQU	0200h		; Wait State Select (0 = yes) 
TCFG_SHADOW  	EQU	0100h		; Shadow Registers Select (1 = norm)
TCFG_SIFACK  	EQU	0080h		; SIF Transfer Acknowledge
TCFG_DINTEN  	EQU	0040h		; DMA Done Interrupt Enable
TCFG_RINTEN  	EQU	0020h		; SIF Data Request Interrupt Enable
TCFG_SINTEN  	EQU	0010h		; SIF Interrupt Enable
TCFG_DMAEN   	EQU	0008h		; DMA Enable
TCFG_MASTER  	EQU	0004h		; Master mode Enabled
TCFG_DDIR    	EQU	0002h		; DMA Direction
TCFG_SIFRST  	EQU	0001h		; Reset Adapter


;----------------------------------------------------------------------;
; COPPAQ Jupiter Configuration Register definitions -                  ;
;                These equates define the bit settings in the Jupiter  ;
;                adapter's configuration register.                     ;
;----------------------------------------------------------------------;
JCFG_INTS  	EQU	0e000h		; Interrupt Level 5, 9 10, 11, 15
JCFG_INTRIG    	EQU	1000h		; Interrupt Level Trigger
JCFG_MEDIA   	EQU	0800h		; Type 3 media select (else type 1)
JCFG_16MBS    	EQU	0400h		; 16Mbs select (else 4Mbs)
JCFG_MEM        EQU     0300h           ; Memory select bits
JCFG_ENABLE  	EQU	0001h		; Adapter Enable 

JUP_ID1         EQU     4f42h           ; Jupiter Product register ID
JUP_ID2         EQU     110eh           ; Jupiter Product register ID


;----------------------------------------------------------------------;
; INTENABLE -  enables adapter interrupts using the interrupt enable/  ;
;              disable bit in the adapter's CONBUF register.           ;
;----------------------------------------------------------------------;
INTENABLE	MACRO
		push	di			; Save DI register

                mov     di, [bx].acb_adpnum             ; Get adapter number
                call    WORD PTR es:gbl_int_enable[di]  ; Call enable routine

                pop     di                      ; Restore DI Register
		ENDM


;----------------------------------------------------------------------;
; INTDISABLE - disables adapter interrupts using the interrupt enable/ ;
;              disable bit in the adapter's CONBUF register.           ;
;----------------------------------------------------------------------;
INTDISABLE	MACRO
		push	di			; Save DI register

                mov     di, [bx].acb_adpnum              ; Get adapter number
                call    WORD PTR es:gbl_int_disable[di]  ; Call enable routine

                pop     di                      ; Restore DI Register
		ENDM
