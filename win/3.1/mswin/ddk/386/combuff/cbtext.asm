PAGE 58,132
;******************************************************************************
TITLE CBTEXT.ASM - COM Buffer Device Text File
;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1990
;
;   Title:	CBTEXT.ASM - COM Buffer Device Text file
;
;   Version:	1.00
;
;   Date:	30-Jan-1990
;
;   Author:	RAL
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;   30-Jan-1990 RAL
;
;==============================================================================

	.386p


;******************************************************************************
;			       P U B L I C S
;******************************************************************************

	PUBLIC COMBuff_Size_Ini_String
	PUBLIC COMBuff_Size_Ini_Number
	PUBLIC COMBuff_Prot_Ini_String
	PUBLIC COMBuff_Prot_Ini_Number
	PUBLIC COMBuff_XOFF_String


;******************************************************************************
;			      I N C L U D E S
;******************************************************************************

	.XLIST
	INCLUDE VMM.Inc
	.LIST


;******************************************************************************
;		   I N I T I A L I Z A T I O N	 D A T A
;******************************************************************************

VxD_IDATA_SEG

COMBuff_Size_Ini_String db "COM"
COMBuff_Size_Ini_Number db ?
			db "BUFFER", 0

COMBuff_Prot_Ini_String db "COM"
COMBuff_Prot_Ini_Number db ?
			db "PROTOCOL", 0


COMBuff_XOFF_String db "XOFF", 0

VxD_IDATA_ENDS


	END
