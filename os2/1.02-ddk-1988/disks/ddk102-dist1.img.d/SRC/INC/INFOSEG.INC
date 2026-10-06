; copyright (C) Microsoft Corporation, 1988


;*	InfoSeg GDT Data Segment Structure
;
; The structure below defines the content and organization of the system
; information segment (InfoSeg).  The actual table is statically defined in
; SDATA.ASM and the read-only ring 3 selector is defined in the GDT table
; in the same file.  Ring 0, read/write access is obtained by the clock
; device driver using the DevHlp GetDOSVar function.

InfoSegGDT	STRUC

;	Time

SIS_BigTime	DD	(?)		; Time from 1-1-1970 in seconds
SIS_MsCount	DD	(?)		; Freerunning milliseconds counter
SIS_HrsTime	DB	(?)		; Hours
SIS_MinTime	DB	(?)		; Minutes
SIS_SecTime	DB	(?)		; Seconds
SIS_HunTime	DB	(?)		; Hundredths of seconds
SIS_TimeZone	DW	(?)		; Timezone in min from GMT (Set to EST)
SIS_ClkIntrvl	DW	(?)		; Timer interval (units=0.0001 secs)

;	Date

SIS_DayDate	DB	(?)		; Day-of-month
SIS_MonDate	DB	(?)		; Month
SIS_YrsDate	DW	(?)		; Year
SIS_DOWDate	DB	(?)		; Day-of-week (1-1-80 = Tues = 3)

;	Version

SIS_VerMajor	DB	(?)		; Major version number
SIS_VerMinor	DB	(?)		; Minor version number
SIS_RevLettr	DB	(?)		; Revision letter

;	System Status

SIS_CurScrnGrp	DB	(?)		; Current screen group
SIS_MaxScrnGrp	DB	(?)		; Maximum number of screen groups
SIS_HugeShfCnt	DB	(?)		; Shift count for huge segments
SIS_ProtMdOnly	DB	(?)		; Protect-mode-only indicator
SIS_FgndPID	DW	(?)		; Foreground process ID		;#####

;	Scheduler Parms

;;SIS_DynVarFlg	DB	(?)		; Dynamic variation flag (1=enabled)
SIS_Dynamic	DB	(?)		; Dynamic variation flag (1=enabled) ;#####
SIS_MaxWait	DB	(?)		; Maxwait (seconds)
SIS_MinSlice	DW	(?)		; Minimum timeslice (milliseconds)
SIS_MaxSlice	DW	(?)		; Maximum timeslice (milliseconds)

;	Boot Drive

SIS_BootDrv	DW	?		; Drive from which system was booted

;	RAS Major Event Code table

SIS_mec_table	DB	32 DUP(?)	; RAS Major Event Code table

InfoSegGDT	ENDS

SIS_LEN		EQU	SIZE InfoSegGDT


;*	InfoSeg LDT Data Segment Structure
;
; The structure below defines the content and organization of the system
; information in a special per-process segment to be accessable by the
; process through the LDT (read-only).
;

InfoSegLDT	STRUC

LIS_CurProcID	DW	(?)		; Current process ID
LIS_ParProcID	DW	(?)		; Process ID of parent
LIS_CurThrdPri	DW	(?)		; Current thread priority
LIS_CurThrdID	DW	(?)		; Current thread ID
LIS_CurScrnGrp	DW	(?)		; Screengroup
LIS_SubScrnGrp	DW	(?)		; Subscreen group
LIS_Fgnd	DW	(?)		; Current process is in foreground
LIS_RealMode	DB	(?)		; Current process is in real mode
LIS_filler1	DB	(?)		; filler byte

InfoSegLDT	ENDS

LIS_LEN		EQU	SIZE InfoSegLDT
