		PAGE  60, 132
		TITLE Token-Ring NDI Driver - DATA.ASM
;**********************************************************************;
;**********************************************************************;
;								       ;
; File Name:       DATA.ASM					       ;
;								       ;
; Program Name:    TRNDD (Token-Ring Network Device Driver)	       ;
;								       ;
; Companion Files: None						       ;
;								       ;
; Function:        Data declarations for the permanent data segment.   ;
;                  The file includes the declarations for the follow-  ;
;		   data structures:				       ;
;								       ;
;		 - Master Control Block                                ;
;		 - Global Variables				       ;
;								       ;
;								       ;
; History:             						       ;
;								       ;
;     02/10/89  Mike Witkowski - Original Version 0.0		       ;
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

;--------------------------;
; External Code References ;
;--------------------------;
		EXTRN	int_exit:	NEAR
 		EXTRN	XmitHandler:	NEAR
		EXTRN	CommandHandler:	NEAR
		EXTRN	StatusHandler:	NEAR
		EXTRN	CheckHandler:	NEAR
		EXTRN	SCBHandler:	NEAR
 
PAGE_BREAK <Device Driver Global Data Definitions>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  	          Device Driver Global Data Definitions                ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

START_DATA

;-------------------------------------;
; Publicize All Global Data Variables ;
;-------------------------------------;
		PUBLIC	gbl_dvrstk
 		PUBLIC  gbl_stack_nest
 		PUBLIC  gbl_dosstk_ss
		PUBLIC	gbl_dosstk_sp
		PUBLIC	gbl_init_blk
		PUBLIC	gbl_open
		PUBLIC	gbl_dynmem
		PUBLIC	gbl_initflag
		PUBLIC	gbl_int_dos


;--------------------------------------------;
; COMPAQ Copyright                           ;
;--------------------------------------------;
                DB      'Copyright (C) COMPAQ Computer Corp.,1990'


;--------------------------------------------;
; Network Driver Decsription                 ;
;--------------------------------------------;
MLABEL <NDIDescription>, %MBYTE, %MPUBLIC	
IFDEF TYPE_3C603
                DB      '3COM 3C603 Token-Ring Adapter', 0
ELSEIF TYPE_P1345
                DB      'Proteon ProNet-4 P1344/5/7 Token-Ring Adapter', 0
ELSEIF TYPE_WDTCPLUS
                DB      'Western Digital TokenCard Master Token-Ring Adapter', 0
ELSEIF TYPE_M8110
                DB      'Racore M8110 Token-Ring Adapter', 0
ENDIF


;--------------------------------------------;
; Define Miscellaneous Global Data Variables ;
;--------------------------------------------;
		DW	100 DUP(0)	; DOS Device Driver Stack
gbl_dvrstk	EQU	$		; Top of DOS DD Stack
gbl_stack_nest  DW	0		; Nested stack use counter
gbl_dosstk_ss   DW	0		; Saved DOS stack segment
gbl_dosstk_sp   DW	0		; Saved DOS stack offset

gbl_dynmem	DW	?		; Dynamic memory segment address
gbl_int_dos     DD	0               ; Old interrupt vector
gbl_initflag    DW      0               ; NIC initialized flag 

		
MLABEL <gbl_RPLaddr>, %MDWORD, %MPUBLIC	
                DB      0c0h, 0, 40h, 0, 0, 0   ; RIPL group address
		
gbl_init_blk	INIT_Block <>		; Global adapter init mask block

gbl_open        OPEN_Block <>           ; Adapter open block

PAGE_BREAK <Interrupt Reason Branch Table>
;----------------------------------------------------------------------;
; Data Structure Name: Interrupt Reason Branch Table                   ;
;								       ;
; Description: The Interrupt Reason Branch Table is used to convert a  ;
;              three bit interrupt reason code into an address of the  ;
;              routine that handles that particular interrupt.         ; 
;----------------------------------------------------------------------;
MLABEL <gbl_int_table>, %MWORD, %MPUBLIC 
		DW	CheckHandler            ; Adapter Check
		DW	CheckHandler		; N/A
		DW	StatusHandler		; Ring Status 
		DW	SCBHandler		; SCB Clear
		DW	CommandHandler		; Command Status
		DW	int_exit		; Receive Status
 		DW	XmitHandler		; Transmit Status
		DW	int_exit		; Transmit Status
		DW	CheckHandler		; N/A


PAGE_BREAK <Transmit List Count Conversion Table>
;----------------------------------------------------------------------;
; Data Structure Name: Transmit List Count Conversion Table            ;
;								       ;
; Description: The Transmit List Count Conversion Table is used to     ;
;              convert the number transmit data pointers to the number ;
;              of lists required to transmit that many pointers.       ; 
;----------------------------------------------------------------------;
MLABEL <gbl_xlist_table>, %MWORD, %MPUBLIC 
		DW	0
		DW	1
		DW	1
		DW	1
		DW	2
		DW	2
		DW	2
		DW	3
		DW	3
		DW	3


PAGE_BREAK <Adapter Specific Table>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Structure Name: Adapter Specific Information Table                  ;
;                                                                      ;
;  Description: The Adapter Specific Table contains all the adapter    ;
;               specific information that is specified in the          ;
;		PROTOCOL.INI file.  If the info is not provided in     ;
;               PROTOCOL.INI file, then the defaults from the Default  ;
;               Adapter Specific Information Tables will be used.  Any ;
;               change to the data structure here must also be made to ;
;               the adapter specific structure in MACSTRCT.INC.        ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

IFDEF TYPE_3C603
MLABEL <adp_stuff>, %MWORD, %MPUBLIC
MLABEL <adapter_int>, %MWORD
		DW	3 		; Adapter #0 interrupt
		DW	0 		; Adapter #0 interrupt
		DW	0 		; Adapter #0 interrupt
		DW	0 		; Adapter #0 interrupt

MLABEL <adapter_dma>, %MWORD
		DW	1		; Adapter #0 DMA 
		DW	0		; Adapter #0 DMA 
		DW	0		; Adapter #0 DMA 
		DW	0		; Adapter #0 DMA 

MLABEL <adapter_base>, %MWORD
		DW	0300h		; Adapter #0 base address
		DW	0		; Adapter #0 base address
		DW	0		; Adapter #0 base address
		DW	0		; Adapter #0 base address

MLABEL <adapter_speed>, %MWORD
		DW	0		; Adapter #0 media speed
		DW	0		; Adapter #0 media speed
		DW	0		; Adapter #0 media speed
		DW	0		; Adapter #0 media speed

MLABEL <adapter_mtype>, %MWORD
		DW	0		; Adapter #0 media type 
		DW	0		; Adapter #0 media type 
		DW	0		; Adapter #0 media type 
		DW	0		; Adapter #0 media type 
ENDIF


IFDEF TYPE_PWD
MLABEL <adp_stuff>, %MWORD, %MPUBLIC
MLABEL <adapter_int>, %MWORD
		DW	3 		; Adapter #0 interrupt
		DW	0 		; Adapter #0 interrupt
		DW	0 		; Adapter #0 interrupt
		DW	0 		; Adapter #0 interrupt

MLABEL <adapter_dma>, %MWORD
		DW	5		; Adapter #0 DMA 
		DW	0		; Adapter #0 DMA 
		DW	0		; Adapter #0 DMA 
		DW	0		; Adapter #0 DMA 

MLABEL <adapter_base>, %MWORD
		DW	0A20h		; Adapter #0 base address
		DW	0		; Adapter #0 base address
		DW	0		; Adapter #0 base address
		DW	0		; Adapter #0 base address

MLABEL <adapter_speed>, %MWORD
		DW	0		; Adapter #0 media speed
		DW	0		; Adapter #0 media speed
		DW	0		; Adapter #0 media speed
		DW	0		; Adapter #0 media speed

MLABEL <adapter_mtype>, %MWORD
		DW	0		; Adapter #0 media type 
		DW	0		; Adapter #0 media type 
		DW	0		; Adapter #0 media type 
		DW	0		; Adapter #0 media type 
ENDIF

END_DATA
		END
