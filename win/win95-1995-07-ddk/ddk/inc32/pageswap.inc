;****************************************************************************
;                                                                           *
; THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
; KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
; IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
; PURPOSE.                                                                  *
;                                                                           *
; Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
;                                                                           *
;****************************************************************************

;******************************************************************************
;
;   Title:	PageSwap.Inc - Demand Paging Swap Device Services
;
;   Version:	4.00
;
;==============================================================================


Begin_Service_Table PageSwap


PageSwap_Service PageSwap_Get_Version, LOCAL
PageSwap_Service PageSwap_Invalid_Service1, LOCAL
PageSwap_Service PageSwap_Invalid_Service2, LOCAL
PageSwap_Service PageSwap_Invalid_Service3, LOCAL
PageSwap_Service PageSwap_Invalid_Service4, LOCAL
PageSwap_Service PageSwap_Invalid_Service5, LOCAL
PageSwap_Service PageSwap_Test_IO_Valid, LOCAL
PageSwap_Service PageSwap_Read_Or_Write, LOCAL
PageSwap_Service PageSwap_Grow_File, LOCAL
PageSwap_Service PageSwap_Init_File, LOCAL

End_Service_Table PageSwap


; Buffer descriptor for PageFile_Read_Or_Write

PageSwapBufferDesc	STRUC
PS_BD_Next		dd	?	; ignored
PS_BD_Cmd		db	?	; PF_Read_Data or PF_Write_Data
PS_BD_Priority		db	?	; ignored
PS_BD_Status		db	?	; return value, PFS_Failure or PFS_Success
PS_BD_nPages		db	?	; number of pages to read or write
PS_BD_Buffer_Ptr	dd	?	; linear address to transfer to or from
PS_BD_File_Page 	dd	?	; page offset within paging file
;PS_BD_Call_Back	 dd	 ?	; field existed in 3.1, but not any more
;PS_BD_Semaphore	 dd	 ?	; field existed in 3.1, but not any more
PageSwapBufferDesc	ENDS
