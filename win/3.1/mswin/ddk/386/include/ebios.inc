;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1989-1990
;
;   Title:	ebios.inc -
;
;   Version:	1.00
;
;   Date:	17-Jan-1989
;
;   Author:	RAP
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;   17-Jan-1989 RAP
;
;==============================================================================

Begin_Service_Table EBIOS

EBIOS_Service	  EBIOS_Get_Version, LOCAL
;
; Following service exists in EBIOS devices with version >= 2.00
;
EBIOS_Service	  EBIOS_Get_Unused_Mem, LOCAL

End_Service_Table EBIOS
