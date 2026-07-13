;**********************************************************************;
;**********************************************************************;
;								       ;
; File Name:       DRVSEG.INC                   		       ;
;								       ;
; Program Name:    Token-Ring RIPL Boot ROM Device Driver              ;
;								       ;
; Companion Files: None						       ;
;								       ;
; Function:        This module contains all the segment definitions    ;
;		   to be used throughout the source code.              ;
;                                                                      ;     
; History:             						       ;
;								       ;
;     03/10/89  Mike Witkowski - Original Version 0.0		       ;
;     02/27/90  Mike Witkowski - Reworked for RIPL                     ;
;								       ;
;**********************************************************************;
;**********************************************************************;


PAGE_BREAK <Group Declarations>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;                            GROUP DECLARATIONS                        ;
;								       ;
; Description: For the DOS case, all segments are grouped into a       ;
;              single segment so we can place all discardable segments ;
;              at the very end of the load module.  This is done be-   ;
;              cause normally the data is placed before the code, but  ;
;              since DOS will not allow the discarded data in such a   ;
;              case to be used again, it would be wasted.  So, to take ;
;              up as little room as possible, we'll place all discard- ;
;              ables to the rear.                                      ;
;							               ;
;              For the OS/2 case, we are allowed to break up the code  ;
;              and data segments because OS/2 will make use of the     ;
;              data space we leave behind.  All in all, its a better   ;
;              looking load module.                                    ; 							
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

CGROUP		GROUP	DATA,CODE
DGROUP 		EQU	<CGROUP>

                ASSUME  CS:CGROUP,DS:DGROUP,ES:DGROUP,SS:DGROUP


PAGE_BREAK <Segment Order Declarations>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;                        SEGMENT ORDER DECLARATIONS                    ;
;								       ;
; Description: This section declares all the segments in their proper  ;
;              order.  After these declarations, all code and data     ;
;              segments may use the generic segment macros defined in  ;
;              the next section to delimit themselves.                 ;
;								       ;
;              The segment order will be different based on whether    ;
;              this is a DOS driver or not.  If it is, then the dis-   ;
;              cardable segments go last.  If it isn't, then we will   ;
;              split up the code and data parts.                       ;
;        							       ;
;              The segments that are defined for this driver are:      ;
;            - DATA:     contains all the permanent data.              ;
;            - CODE:     contains all the permanent code.              ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

DATA		SEGMENT WORD PUBLIC 'DATA' 	     
DATA		ENDS				     

CODE		SEGMENT BYTE PUBLIC 'CODE'          
CODE		ENDS			             

PAGE_BREAK <Segment Start/End Macro Declarations>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;                    SEGMENT START/END MACRO DECLARATIONS              ;
;								       ;
; Description: This section declares all the segment delimiting macros ;
;              so we can simplify our lives throughout the remainder   ;
;              of the code.                                            ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

START_CODE	MACRO
CODE		SEGMENT
		ASSUME CS:CGROUP,DS:DGROUP,ES:DGROUP,SS:DGROUP
		ENDM

END_CODE	MACRO
CODE		ENDS
		ENDM

START_DATA	MACRO
DATA		SEGMENT
		ENDM

END_DATA	MACRO
DATA		ENDS
		ENDM
