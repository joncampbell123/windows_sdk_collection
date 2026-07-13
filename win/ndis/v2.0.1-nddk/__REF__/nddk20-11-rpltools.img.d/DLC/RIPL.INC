;**********************************************************************;
;**********************************************************************;
;								       ;
; File Name:       RIPL.INC                     		       ;
;								       ;
; Program Name:    RIPL DLC Boot ROM Module             	       ;
;								       ;
; Companion Files: None						       ;
;								       ;
; Function:        This module contains all the data strcuture defini- ;
;		   tions for the various RIPL Frame formats used in    ;
;                  the RIPL process.                                   ;
;                                                                      ;     
; History:             						       ;
;								       ;
;     02/26/90  Mike Witkowski - Original                              ;
;								       ;
;**********************************************************************;
;**********************************************************************;


;---------------------------------------------------------------------;
; Structure Name: RIPL States (RS)                                    ;
; 								      ;
; Description: These equates define the primary states that the RIPL  ;
;              protocol process may take on.                          ;
;---------------------------------------------------------------------;
RS_FIND 	EQU	0		; FIND State
RS_TRANSFER 	EQU	1		; TRANSFER State
RS_ALERT 	EQU	2		; ALERT State
RS_LAST 	EQU	3		; LAST State
RS_QUIESCENT 	EQU	4		; QUIESCENT State
RS_ERROR 	EQU	8		; ERROR State


;---------------------------------------------------------------------;
; Structure Name: Packet Related Equetes                              ;
; 								      ;
; Description: These equates define various packet releated values.   ;
;---------------------------------------------------------------------;
PGMCMD_FIND     EQU     0100h           ; FIND Packet Type
PGMCMD_FOUND    EQU     0200h           ; FOUND Packet Type
PGMCMD_SDREQ    EQU     1000h           ; SEND.DATA.REQUEST Packet Type
PGMCMD_SDRESP   EQU     2000h           ; SEND.DATA.RESPONSE Packet Type
PGMCMD_LERROR   EQU     4000h           ; LOAD.ERROR Packet Type

;---------------------------------------------------------------------;
; Structure Name: FIND Frame Format Structure                         ;
; 								      ;
; Description: Structure of the FIND Frame.                           ;
;---------------------------------------------------------------------;
Find_Frame      STRUC
FiF_DSAP        DB	0fch                    ; Destination SAP
FiF_SSAP        DB	0fch                    ; Source SAP
FiF_LCtrl       DB	03h                     ; LLC Control (UI frame)
FiF_PgmLen      DB	00, 53h                 ; Program Length
FiF_PgmCmd	DB	00, 01 	                ; Program Command
FiF_CorrHdr     DB      00, 08, 40h, 03         ; Correlator Vector
FiF_Corr        DB      00, 00, 00, 00          ; Correlator
FiF_InfoHdr     DB      00, 10h, 00, 08         ; Info Header
FiF_FrameHdr    DB      00, 06, 40h, 09         ; Frame size sub-vector
FiF_MaxFrame    DB      ?, ?                    ; Maximum Frame Size
FiF_ClassHdr    DB      00, 06, 40h, 0ah        ; Connect Class sub-vector
FiF_ConnClass   DB      00, 01h                 ; Type of connection service
FiF_SrcHdr      DB      00, 0ah, 40h, 06        ; Local address vector
FiF_SrcAddr     DB      6 DUP (?)               ; Local Node Address
FiF_LSAPHdr     DB      00, 05, 40h, 07         ; Logical SAP Vector
FiF_RSAP        DB      0fch                    ; Remote SAP value
FiF_SearchHdr   DB      00, 28h, 00, 04         ; Search Vector
FiF_LoaderHdr   DB      00, 24h, 0c0h, 05       ; Loader info sub-vector
FiF_MachConf    DB      8 DUP (?)               ; Machine Configuration
FiF_EquipFlags  DB      ?, ?                    ; Equipment Flags
FiF_MemSize     DB      ?, ?                    ; Memory Size
FiF_EC          DB      8 DUP (0)               ; EC Level of RIPL module
FiF_AdapterID   DB      0, 0                    ; Adapter Identifier
FiF_AdapterEC   DB      10 DUP (0)              ; Adapter EC Level
Find_Frame      ENDS

size_FiF        EQU     SIZE Find_Frame         ; Size of FIND frame


;---------------------------------------------------------------------;
; Structure Name: FOUND Frame Format Structure                        ;
; 								      ;
; Description: Structure of the FOUND Frame.                          ;
;---------------------------------------------------------------------;
Found_Frame     STRUC
FoF_DSAP        DB	0fch                    ; Destination SAP
FoF_SSAP        DB	0fch                    ; Source SAP
FoF_LCtrl       DB	03h                     ; LLC Control (UI frame)
FoF_PgmLen	DB	00, 3ah                 ; Program Length
FoF_PgmCmd	DB	00, 02 	                ; Program Command
FoF_CorrHdr     DB      00, 08, 40h, 03         ; Correlator Vector
FoF_Corr        DB      00, 00, 00, 00          ; Correlator
FoF_RespHdr     DB      00, 05, 40h, 0bh        ; Response Header
FoF_RespCode    DB      ?                       ; Response Code
FoF_DestHdr     DB      00, 0ah, 40h, 0ch       ; Set address vector
FoF_DestAddr    DB      6 DUP (?)               ; Requesting device address
FoF_SrcHdr      DB      00, 0ah, 40h, 06        ; Loader address vector
FoF_SrcAddr     DB      6 DUP (?)               ; Loading device ring address
FoF_InfoHdr     DB      00, 10h, 00, 08         ; Info Header
FoF_FrameHdr    DB      00, 06, 40h, 09         ; Frame size sub-vector
FoF_MaxFrame    DB      ?, ?                    ; Maximum Frame Size
FoF_ClassHdr    DB      00, 06, 40h, 0ah        ; Connect Class sub-vector
FoF_ConnClass   DB      ?, ?                    ; Type of connection service
FoF_LSAPHdr     DB      00, 05, 40h, 07         ; Logical SAP Vector
FoF_RSAP        DB      ?                       ; Remote SAP value
Found_Frame     ENDS

size_FoF        EQU     SIZE Found_Frame        ; Size of FOUND frame


;---------------------------------------------------------------------;
; Structure Name: SEND.FILE.REQUEST Frame Format Structure            ;
; 								      ;
; Description: Structure of the SEND.FILE.REQUEST Frame.              ;
;---------------------------------------------------------------------;
Send_Frame      STRUC
SFR_DSAP        DB	?                       ; Destination SAP
SFR_SSAP        DB	0fch                    ; Source SAP
SFR_LCtrl       DB	03h                     ; LLC Control (UI frame)
SFR_PgmLen      DB	00, 53h                 ; Program Length
SFR_PgmCmd	DB	00, 10h	                ; Program Command
SFR_SeqHdr      DB      00, 08, 40h, 11h        ; Sequence Vector
SFR_Seq         DB      00, 00, 00, 00          ; Sequence Number
SFR_InfoHdr     DB      00, 10h, 00, 08         ; Info Header
SFR_FrameHdr    DB      00, 06, 40h, 09         ; Frame size sub-vector
SFR_MaxFrame    DB      ?, ?                    ; Maximum Frame Size
SFR_ClassHdr    DB      00, 06, 40h, 0ah        ; Connect Class sub-vector
SFR_ConnClass   DB      ?, ?                    ; Type of connection service
SFR_SrcHdr      DB      00, 0ah, 40h, 06        ; Local address vector
SFR_SrcAddr     DB      6 DUP (?)               ; Local Node Address
SFR_LSAPHdr     DB      00, 05, 40h, 07         ; Logical SAP Vector
SFR_RSAP        DB      ?                       ; Remote SAP value
SFR_SearchHdr   DB      00, 28h, 00, 04         ; Search Vector
SFR_LoaderHdr   DB      00, 24h, 0c0h, 05       ; Loader info sub-vector
SFR_MachConf    DB      8 DUP (?)               ; Machine Configuration
SFR_EquipFlags  DB      ?, ?                    ; Equipment Flags
SFR_MemSize     DB      ?, ?                    ; Memory Size
SFR_EC          DB      8 DUP (0)               ; EC Level of RIPL module
SFR_AdapterID   DB      0, 0                    ; Adapter Identifier
SFR_AdapterEC   DB      10 DUP (0)              ; Adapter EC Level
Send_Frame      ENDS

size_SFR        EQU     SIZE Send_Frame         ; Size of SDR frame


;---------------------------------------------------------------------;
; Structure Name: FILE.DATA.RESPONSE Frame Format Structure           ;
; 								      ;
; Description: Structure of the FILE.DATA.RESPONSE Frame.             ;
;---------------------------------------------------------------------;
File_Frame      STRUC
FDR_DSAP        DB	0f8h                    ; Destination SAP
FDR_SSAP        DB	0f8h                    ; Source SAP
FDR_LCtrl       DB	03h                     ; LLC Control (UI frame)
FDR_PgmLen      DB	?, ?                    ; Program Length
FDR_PgmCmd	DB	00, 20h	                ; Program Command
FDR_SeqHdr      DB      00, 08, 40h, 11h        ; Sequence Vector
FDR_Seq         DB      ?, ?, ?, ?              ; Sequence Number
FDR_LoaderHdr   DB      00, 0dh, 0c0h, 14h      ; Loader info sub-vector
FDR_LocateAddr  DB      ?, ?, ?, ?              ; Location Address
FDR_XferAddr    DB      ?, ?, ?, ?              ; Transfer Address
FDR_Flags       DB      ?                       ; Options Flags
FDR_DataHdr     DB      ?, ?, 40h, 18h          ; Data Header
FDR_FileData    DB      ?                       ; Start of file dtata
File_Frame      ENDS

size_FDR        EQU     SIZE File_Frame         ; Size of SDR frame

FF_EOF          EQU     80h                     ; End-Of-File flag
FF_XFER         EQU     40h                     ; Transfer Enable Flag
FF_LOCATE       EQU     20h                     ; Locate Enable Flag
FF_ACK          EQU     10h                     ; Acknowledge flag


ALERT_BADERROR  EQU     0000h                   ; Bad LOAD.ERROR error code
ALERT_NOMEM     EQU     0004h                   ; Out of memory error
ALERT_OVERRUN   EQU     0006h                   ; Memory Overrun error


;---------------------------------------------------------------------;
; Structure Name: PROGRAM.ALERT Frame Format Structure                ;
; 								      ;
; Description: Structure of the PROGRAM.ALERT Frame.                  ;
;---------------------------------------------------------------------;
Alert_Frame     STRUC
ALT_DSAP        DB	0f8h                    ; Destination SAP
ALT_SSAP        DB	0f8h                    ; Source SAP
ALT_LCtrl       DB	03h                     ; LLC Control (UI frame)
ALT_PgmLen      DB	00, 59h                 ; Program Length
ALT_PgmCmd	DB	00, 30h	                ; Program Command
ALT_SeqHdr      DB      00, 08, 40h, 11h        ; Sequence Vector
ALT_Seq         DB      ?, ?, ?, ?              ; Sequence Number
ALT_InfoHdr     DB      00, 10h, 00, 08         ; Info Header
ALT_FrameHdr    DB      00, 06, 40h, 09         ; Frame size sub-vector
ALT_MaxFrame    DB      ?, ?                    ; Maximum Frame Size
ALT_ClassHdr    DB      00, 06, 40h, 0ah        ; Connect Class sub-vector
ALT_ConnClass   DB      00, 01h                 ; Type of connection service
ALT_SrcHdr      DB      00, 0ah, 40h, 06        ; Local address vector
ALT_SrcAddr     DB      6 DUP (?)               ; Local Node Address
ALT_LSAPHdr     DB      00, 05, 40h, 07         ; Logical SAP Vector
ALT_RSAP        DB      0fch                    ; Remote SAP value
ALT_SearchHdr   DB      00, 28h, 00, 04         ; Search Vector
ALT_LoaderHdr   DB      00, 24h, 0c0h, 05       ; Loader info sub-vector
ALT_MachConf    DB      8 DUP (?)               ; Machine Configuration
ALT_EquipFlags  DB      ?, ?                    ; Equipment Flags
ALT_MemSize     DB      ?, ?                    ; Memory Size
ALT_EC          DB      8 DUP (0)               ; EC Level of RIPL module
ALT_AdapterID   DB      0, 0                    ; Adapter Identifier
ALT_AdapterEC   DB      10 DUP (0)              ; Adapter EC Level
ALT_FileHdr     DB      00, 04, 40h, 13h        ; File Header 
ALT_AlertHdr    DB      00, 06, 40h, 19h        ; Alert Header
ALT_AlertCode   DB      ?, ?                    ; Alert Code                
Alert_Frame     ENDS

size_ALT        EQU     SIZE Alert_Frame         ; Size of SDR frame


;---------------------------------------------------------------------;
; Structure Name: LOAD.ERROR Frame Format Structure                   ;
; 								      ;
; Description: Structure of the LOAD.ERROR Frame.                     ;
;---------------------------------------------------------------------;
Load_Frame      STRUC
LDE_DSAP        DB	0f8h                    ; Destination SAP
LDE_SSAP        DB	0f8h                    ; Source SAP
LDE_LCtrl       DB	03h                     ; LLC Control (UI frame)
LDE_PgmLen      DB	?, ?                    ; Program Length
LDE_PgmCmd	DB	00, 40h	                ; Program Command
LDE_ErrHdr      DB      00, 06, 40h, 1ah        ; Error Header
LDE_ErrorCode   DB      ?, ?                    ; Error Code
LDE_DataHdr     DB      ?, ?, 40h, 13h          ; Data Header
LDE_FileData    DB      ?                       ; Start of file dtata
Load_Frame      ENDS

LOADERR_RESTART EQU     0000h                   ; Restart error code
LOADERR_BADFILE EQU     0200h                   ; File Not Found error code


;---------------------------------------------------------------------;
; Structure Name: Event Decriptor (ED) Structure                      ;
; 								      ;
; Description: Structure of Event Descriptor which are use to         ;
;              represent the various events that can occur in the     ;
;              RIPL ROM.                                              ;
;---------------------------------------------------------------------;
ED_Struct      STRUC
ED_Ptr         DD      0               ; Queue Pointer
ED_Opcode      DW      0               ; Event Opcode
ED_ComCode     DB      0               ; Completion Code
ED_Rsvd1       DB      0               ; Not Used
ED_TO          DW      0               ; Time Out Interval
ED_DestAddr    DB      6 DUP (?)       ; Destination Address
ED_Work        DB      10 DUP (?)      ; Driver Workspace        
ED_DCount      DW      0               ; Number of descriptors
ED_Descript    DB      6 DUP (?)       ; Frame descriptors
ED_Struct      ENDS

size_ED        EQU     SIZE ED_Struct ; ED size

Descript_Struct STRUC
DPointer        DD      ?               ; Descriptor Pointer
DLen            DW      ?               ; Length of packet descriptor
Descript_Struct ENDS


;---------------------------------------------------------------------;
; Structure Name: ED Opcode Definitions                               ;
; 								      ;
; Description: Defines the valid Opcodes that may appear in the ED    ;
;              Opcode field.                                          ;
;---------------------------------------------------------------------;
EOC_FTimer      EQU     0               ; Find Timer Op Code
EOC_SFRTimer    EQU     2               ; FDR Timer Op Code
EOC_ATimer      EQU     4               ; Alert Timer Op Code
EOC_Xmit        EQU     6               ; Transmit Op Code
EOC_Rcv         EQU     8               ; Receive Op Code


;---------------------------------------------------------------------;
; Structure Name: Timer Count Structure Definition                    ;
; 								      ;
; Description: Defines the fields of the timer counter and timer      ;
;              cycle counter structure.                               ;
;---------------------------------------------------------------------;
TO_Struct       STRUC
TO_count        DW      ?               ; Timer counter
TO_cycle        DW      ?               ; Second tick counter
TO_Struct       ENDS

;---------------------------------------------------------------------;
; Structure Name: Miscellaneous Constants                             ;
; 								      ;
; Description: Defines miscellaneous constants used throughout the    ;
;              RIPL ROM module.                                       ;
;---------------------------------------------------------------------;
SEND_RETRY      EQU     50h             ; Send Frame retry count

TIME_FIND       EQU     19              ; FIND timer (~1 sec)
TIME_SFR        EQU     19              ; SFR timer (~1 sec)
TIME_ALERT      EQU     19              ; ALERT timer (~1 sec)

ED_COUNT        EQU     10              ; Count of EDs
BUFFER_COUNT    EQU     8               ; Count of buffers
BUFFER_SIZE     EQU     2048            ; Buffer size

TIMERINT        EQU     0070h           ; BIOS System Timer INT Vector ADDR

BIOSRAMSEG      EQU     0040h           ; BIOS RAM Data Area Segment Address
BASEMEMSIZE     EQU     0013h           ; BIOS RAM Base Memory Size variable
ROMSIZE         EQU     0002h           ; Pointer to ROM size in ROM image

INT13_VECTOR    EQU     004ch           ; Offset to INT 13 Vector
INT18_VECTOR    EQU     0060h           ; Offset to INT 18 Vector
INTFD_VECTOR    EQU     03F4h           ; Offset to INT FD Vector
LAN_STATUS      EQU     04d0h           ; Lan adapter status
