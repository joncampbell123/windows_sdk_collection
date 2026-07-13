;**********************************************************************;
;**********************************************************************;
;								       ;
; File Name:       DRVSEG.INC                   		       ;
;								       ;
; Program Name:    RIPL DLC Boot ROM Module             	       ;
;								       ;
; Companion Files: None						       ;
;								       ;
; Function:        This module contains all the segment definitions    ;
;		   to be used throughout the source code.              ;
;                                                                      ;     
; History:             						       ;
;								       ;
;     02/20/90  Mike Witkowski - Original Version 0.0		       ;
;								       ;
;**********************************************************************;
;**********************************************************************;


PAGE_BREAK <Group Declarations>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;                            GROUP DECLARATIONS                        ;
;								       ;
; Description: All code and data is placed into the same group.        ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

CGROUP		GROUP	HEADER,DATA,CODE
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
;            - HEADER:   contains the Boot ROM header. It must be      ;
;                        the first segment defined because it must be  ;
;                        the first element in the load module.         ;
;            - DATA:     contains all the permanent data.              ;
;            - CODE:     contains all the permanent code.              ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

HEADER		SEGMENT BYTE PUBLIC 'DATA'           ; Common segment 
HEADER		ENDS				     ;   definition for the
					             ;   device header and
DATA		SEGMENT WORD PUBLIC 'DATA' 	     ;   permanent data 
DATA		ENDS				     ;   segments. 
                                                     ;
CODE		SEGMENT BYTE PUBLIC 'CODE'           ;
CODE		ENDS			             ;
						     ;


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

START_HEADER	MACRO
HEADER		SEGMENT
		ENDM

END_HEADER	MACRO
HEADER		ENDS
		ENDM

START_DATA	MACRO
DATA		SEGMENT
		ENDM

END_DATA	MACRO
DATA		ENDS
		ENDM
