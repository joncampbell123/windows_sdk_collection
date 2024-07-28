;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1989-1990
;
;   Title:	VMD.INC - Virtual Mouse Device Services
;
;   Version:	3.00
;
;   Date:	01-May-1989
;
;   Author:	RAL
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;   01-May-1989 RAL Virtual Mouse Device Services
;
;==============================================================================


Begin_Service_Table VMD

VMD_Service	VMD_Get_Version, LOCAL
VMD_Service	VMD_Set_Mouse_Type, LOCAL
VMD_Service	VMD_Get_Mouse_Owner, LOCAL

End_Service_Table VMD

;
;   Mouse type definitions
;
VMD_Type_Undefined	EQU	0
VMD_Type_Bus		EQU	1
VMD_Type_Serial 	EQU	2
VMD_Type_InPort 	EQU	3
VMD_Type_PS2		EQU	4
VMD_Type_HP		EQU	5
VMD_Type_Serial_w_port	EQU	VMD_Type_Serial OR 80h

;
;   Call-Out API definitions (passed in Client_CX)
;
VMD_CO_API_Test_Inst	EQU	0
VMD_CO_API_Get_Call_Back EQU	1
