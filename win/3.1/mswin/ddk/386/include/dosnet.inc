;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1988-1990
;
;   Title:	DOSNET.Inc - Public services for DOSNET
;
;   Version:	1.00
;
;   Date:	09-Jun-89
;
;   Author:	ARR
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;   09-Jun-1989 ARR Original
;
;==============================================================================

Begin_Service_Table DOSNET

DOSNET_Service	DOSNET_Get_Version, LOCAL
DOSNET_Service	DOSNET_Send_FILESYSCHANGE, LOCAL
DOSNET_Service	DOSNET_Do_PSP_Adjust, LOCAL

End_Service_Table DOSNET
