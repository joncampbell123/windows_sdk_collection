;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1988-1990
;
;   Title:	PageFile.Inc - Demand Paging Swap Device Services
;
;   Version:	1.1
;
;   Date:	4-Oct-90
;
;   Author:	JEM
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;    4-Oct-90	01  JEM -- adapted from pageswap.inc
;
;==============================================================================


Begin_Service_Table PageFile

PageFile_Service PageFile_Get_Version,	 LOCAL
PageFile_Service PageFile_Init_File,	 LOCAL
PageFile_Service PageFile_Clean_Up,	 LOCAL
PageFile_Service PageFile_Grow_File,	 LOCAL
PageFile_Service PageFile_Read_Or_Write, LOCAL
PageFile_Service PageFile_Cancel,	 LOCAL
PageFile_Service PageFile_Test_IO_Valid, LOCAL

End_Service_Table PageFile


; Commands to PageFile_Read_Or_Write

PF_Read_Data	EQU	0		; Read a page
PF_Write_Data	EQU	1		; Write a page

; Flags to PageFile_Read_Or_Write

PF_Low_Priority      EQU	0
PF_High_Priority     EQU	1
PF_High_Priority_Bit EQU	0


; Paging Types

PF_Paging_None	EQU	1		; Paging not active
PF_Paging_DOS	EQU	2		; Paging I/O via DOS or Int 13h
PF_Paging_HW	EQU	3		; Paging direct to hardware


; Read/Write I/O Call Back Status

PFS_Success	EQU	0
PFS_Failure	EQU	1
PFS_Canceled	EQU	11h
