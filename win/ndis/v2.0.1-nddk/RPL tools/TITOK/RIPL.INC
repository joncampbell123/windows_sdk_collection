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
;     03/08/90  Mike Witkowski - Original                              ;
;								       ;
;**********************************************************************;
;**********************************************************************;


;---------------------------------------------------------------------;
; Structure Name: Event Descriptor (ED) Structure                     ;
; 								      ;
; Description: Structure of Event Descriptors which are use to        ;
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
ED_XmitType    DB      ?               ; Transmit Type (Workspace)        
ED_Work        DB      9 DUP (?)       ; Driver Workspace        
ED_DCount      DW      0               ; Number of descriptors
ED_Descript    DB      6 DUP (?)       ; Frame descriptors
ED_Struct      ENDS

size_ED        EQU     SIZE ED_Struct ; ED size

Descript_Struct STRUC
DPointer        DD      ?               ; Descriptor Pointer
DLen            DW      ?               ; Length of packet descriptor
Descript_Struct ENDS

size_descript   EQU     SIZE Descript_Struct

XT_BROADCAST    EQU     0               ; Transmit RPL broadcast
XT_DIRECTED     EQU     1               ; Transmit directed frame
