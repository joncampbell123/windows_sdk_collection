;**********************************************************************;
;**********************************************************************;
;								       ;
; File Name:       MESSAGE.INC 					       ;
;								       ;
; Program Name:    RIPL DLC Boot ROM Module             	       ;
;								       ;
; Companion Files: None						       ;
;								       ;
; Function:        Message Equates for messages printed during the     ;
;                  RIPL process.  	                	       ;	
;								       ;
; History:             						       ;
;								       ;
;     02/26/90  Mike Witkowski - Original Version 0.0		       ;
;								       ;
;**********************************************************************;
;**********************************************************************;

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
; Text Macros                                                          ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
text_banner	EQU	<"Microsoft DLC Remote Program Load (RPL) ROM v.1.0">
size_banner	SIZESTR text_banner
text_drvinit	EQU	<"Initializing Adapter:">
size_drvinit	SIZESTR text_drvinit
text_descript	EQU	<"NIC Description:">
size_descript	SIZESTR text_descript
text_adpok	EQU	<"Adapter Initialized Successfully">
size_adpok	SIZESTR text_adpok
text_blank	EQU	<"                                                             ">
size_blank	SIZESTR text_blank


time_struc      STRUC
TIM_header      DB      14 DUP (?)
TIM_hourhi      DB      ?
TIM_hourlo      DB      ?
TIM_marker1     DB      ':'
TIM_minhi       DB      ?
TIM_minlo       DB      ?
TIM_marker2     DB      ':'
TIM_sechi       DB      ?
TIM_seclo       DB      ?
time_struc      ENDS

text_time	EQU	<"Elapsed Time: 00:00:00">
size_time	SIZESTR text_time


fretry_struc    STRUC
FCNT_header     DB      "Searching for DLC RPL Server, Retries = "
FCNT_hunthou    DB      '0'
FCNT_tenthou    DB      '0'
FCNT_onethou    DB      '0'
FCNT_hundreds   DB      '0'
FCNT_tens       DB      '0'
FCNT_ones       DB      '0'
fretry_struc    ENDS

size_fretry     EQU     SIZE fretry_struc


sretry_struc    STRUC
SCNT_header     DB      "Sending File Requests to DLC RPL Server, Retries = "
SCNT_thousand   DB      '0'
SCNT_hundreds   DB      '0'
SCNT_tens       DB      '0'
SCNT_ones       DB      '0'
sretry_struc    ENDS

size_sretry     EQU     SIZE sretry_struc


seqnce_struc    STRUC   
SEQC_header     DB      "Waiting for Data Frame with Sequence Number: "
SEQC_hunthou    DB      '0'
SEQC_tenthou    DB      '0'
SEQC_onethou    DB      '0'
SEQC_hundreds   DB      '0'
SEQC_tens       DB      '0'
SEQC_ones       DB      '0'
seqnce_struc    ENDS

size_seqnce    EQU     SIZE seqnce_struc


PR_BANNER       EQU     0000h           ; Print row for banner
PR_TIME         EQU     0300h           ; Print row for time
PR_DESCRIPT     EQU     0400h           ; Print row for NIC description
PR_DRVINIT      EQU     0600h           ; Print row for driver init
PR_STATUS       EQU     0700h           ; Print row for DLC status
PR_FIND         EQU     0800h           ; Print row for find retries
PR_SEND         EQU     0a00h           ; Print row for send reties
PR_SEQUENCE     EQU     0b00h           ; Print row for sequence number
