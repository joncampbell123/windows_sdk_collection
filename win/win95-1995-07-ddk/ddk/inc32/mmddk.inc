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

;==========================================================================;
;  mmddk.h -- Include file for Multimedia Device Development
;  Version 4.00
;--------------------------------------------------------------------------;
;  Note: You must include the WINDOWS.H and MMSYSTEM.H header files
;        before including this file.
;  Define:         Prevent inclusion of:
;  --------------  --------------------------------------------------------
;  MMNOMIDIDEV     MIDI support
;  MMNOWAVEDEV     Waveform support
;  MMNOAUXDEV      Auxiliary output support
;  MMNOMIXERDEV    Mixer support
;  MMNOTIMERDEV    Timer support
;  MMNOJOYDEV      Joystick support
;  MMNOMCIDEV      MCI support
;  MMNOTASKDEV     Task support
;==========================================================================;

OFFSEL	STRUC
Off	DW	?
Sel	DW	?
OFFSEL	ENDS

LOHI	STRUC
Lo	DW	?
Hi	DW	?
LOHI	ENDS
DCB_TYPEMASK	EQU	0007H           ; callback type mask
DCB_NULL	EQU	0000H           ; unknown callback type
; flags for wFlags parameter of DriverCallback()
DCB_WINDOW	EQU	0001H           ; dwCallback is a HWND
DCB_TASK	EQU	0002H           ; dwCallback is a HTASK
DCB_FUNCTION	EQU	0003H           ; dwCallback is a FARPROC
DCB_EVENT	EQU	0005H           ; dwCallback is an EVENT
DRVM_INIT	EQU	64H
DRVM_EXIT	EQU	65H
DRVM_MAPPER	EQU	2000H
DRVM_USER	EQU	4000H
ifndef MMNOWAVEDEV
;***************************************************************************
;                     Waveform device driver support
;*************************************************************************
; maximum number of wave device drivers loaded
MAXWAVEDRIVERS	EQU	10

WAVEOPENDESC	STRUC
wod_hWave	DW	?
wod_lpFormat	DD	?
wod_dwCallback	DD	?
wod_dwInstance	DD	?
wod_uMappedDeviceID	DW	?
wod_dnDevNode	DD	?
WAVEOPENDESC	ENDS
WODM_USER	EQU	DRVM_USER
WIDM_USER	EQU	DRVM_USER
WODM_MAPPER	EQU	DRVM_MAPPER
WIDM_MAPPER	EQU	DRVM_MAPPER
WODM_INIT	EQU	DRVM_INIT
WIDM_INIT	EQU	DRVM_INIT
; messages sent to wodMessage() entry-point function
WODM_GETNUMDEVS	EQU	3
WODM_GETDEVCAPS	EQU	4
WODM_OPEN	EQU	5
WODM_CLOSE	EQU	6
WODM_PREPARE	EQU	7
WODM_UNPREPARE	EQU	8
WODM_WRITE	EQU	9
WODM_PAUSE	EQU	10
WODM_RESTART	EQU	11
WODM_RESET	EQU	12
WODM_GETPOS	EQU	13
WODM_GETPITCH	EQU	14
WODM_SETPITCH	EQU	15
WODM_GETVOLUME	EQU	16
WODM_SETVOLUME	EQU	17
WODM_GETPLAYBACKRATE	EQU	18
WODM_SETPLAYBACKRATE	EQU	19
WODM_BREAKLOOP	EQU	20
; messages sent to widMessage() entry-point function
WIDM_GETNUMDEVS	EQU	50
WIDM_GETDEVCAPS	EQU	51
WIDM_OPEN	EQU	52
WIDM_CLOSE	EQU	53
WIDM_PREPARE	EQU	54
WIDM_UNPREPARE	EQU	55
WIDM_ADDBUFFER	EQU	56
WIDM_START	EQU	57
WIDM_STOP	EQU	58
WIDM_RESET	EQU	59
WIDM_GETPOS	EQU	60
endif  ;ifndef MMNOWAVEDEV
ifndef MMNOMIDIDEV
;***************************************************************************
;                        MIDI device driver support
;*************************************************************************
; maximum number of MIDI device drivers loaded
MAXMIDIDRIVERS	EQU	10
MODM_USER	EQU	DRVM_USER
MIDM_USER	EQU	DRVM_USER
MODM_MAPPER	EQU	DRVM_MAPPER
MIDM_MAPPER	EQU	DRVM_MAPPER
MODM_INIT	EQU	DRVM_INIT
MIDM_INIT	EQU	DRVM_INIT

MIDIOPENDESC	STRUC
mod_hMidi	DW	?
mod_dwCallback	DD	?
mod_dwInstance	DD	?
MIDIOPENDESC	ENDS
; messages sent to modMessage() entry-point function
MODM_GETNUMDEVS	EQU	1
MODM_GETDEVCAPS	EQU	2
MODM_OPEN	EQU	3
MODM_CLOSE	EQU	4
MODM_PREPARE	EQU	5
MODM_UNPREPARE	EQU	6
MODM_DATA	EQU	7
MODM_LONGDATA	EQU	8
MODM_RESET	EQU	9
MODM_GETVOLUME	EQU	10
MODM_SETVOLUME	EQU	11
MODM_CACHEPATCHES	EQU	12
MODM_CACHEDRUMPATCHES	EQU	13
; messages sent to midMessage() entry-point function
MIDM_GETNUMDEVS	EQU	53
MIDM_GETDEVCAPS	EQU	54
MIDM_OPEN	EQU	55
MIDM_CLOSE	EQU	56
MIDM_PREPARE	EQU	57
MIDM_UNPREPARE	EQU	58
MIDM_ADDBUFFER	EQU	59
MIDM_START	EQU	60
MIDM_STOP	EQU	61
MIDM_RESET	EQU	62
endif  ;ifndef MMNOMIDIDEV
ifndef MMNOAUXDEV
;***************************************************************************
;                  Auxiliary audio device driver support
;*************************************************************************
; maximum number of auxiliary device drivers loaded
MAXAUXDRIVERS	EQU	10
AUXM_INIT	EQU	DRVM_INIT
AUXM_USER	EQU	DRVM_USER
AUXDM_MAPPER	EQU	DRVM_MAPPER
; messages sent to auxMessage() entry-point function
AUXDM_GETNUMDEVS	EQU	3
AUXDM_GETDEVCAPS	EQU	4
AUXDM_GETVOLUME	EQU	5
AUXDM_SETVOLUME	EQU	6
endif  ;ifndef MMNOAUXDEV
ifndef MMNOTIMERDEV
ifndef _WIN32_VXD

TIMEREVENT	STRUC
te_wDelay	DW	?
te_wResolution	DW	?
te_lpFunction	DD	?
te_dwUser	DD	?
te_wFlags	DW	?
TIMEREVENT	ENDS
; messages sent to tddMessage() function
TDD_KILLTIMEREVENT	EQU	<(DRV_RESERVED+0)>  ; indices into a table of
TDD_SETTIMEREVENT	EQU	<(DRV_RESERVED+4)>  ; functions; thus offset by
TDD_GETSYSTEMTIME	EQU	<(DRV_RESERVED+8)>  ; four each time...
TDD_GETDEVCAPS	EQU	<(DRV_RESERVED+12)> ; room for future expansion
TDD_BEGINMINPERIOD	EQU	<(DRV_RESERVED+16)> ; room for future expansion
TDD_ENDMINPERIOD	EQU	<(DRV_RESERVED+20)> ; room for future expansion
endif  ; ifndef _WIN32_VXD
endif  ;ifndef MMNOTIMERDEV
ifndef MMNOJOYDEV
; RegisterWindowMessage with this to get msg id of config changes
JOY_CONFIGCHANGED_MSGSTRING	EQU	<"MSJSTICK_VJOYD_MSGSTR">
; pre-defined joystick types
JOY_HW_NONE	EQU	0
JOY_HW_CUSTOM	EQU	1
JOY_HW_2A_2B_GENERIC	EQU	2
JOY_HW_2A_4B_GENERIC	EQU	3
JOY_HW_2B_GAMEPAD	EQU	4
JOY_HW_2B_FLIGHTYOKE	EQU	5
JOY_HW_2B_FLIGHTYOKETHROTTLE	EQU	6
JOY_HW_3A_2B_GENERIC	EQU	7
JOY_HW_3A_4B_GENERIC	EQU	8
JOY_HW_4B_GAMEPAD	EQU	9
JOY_HW_4B_FLIGHTYOKE	EQU	10
JOY_HW_4B_FLIGHTYOKETHROTTLE	EQU	11
JOY_HW_LASTENTRY	EQU	12
; calibration flags
JOY_ISCAL_XY	EQU	00000001H	; XY are calibrated
JOY_ISCAL_Z	EQU	00000002H	; Z is calibrated
JOY_ISCAL_R	EQU	00000004H	; R is calibrated
JOY_ISCAL_U	EQU	00000008H	; U is calibrated
JOY_ISCAL_V	EQU	00000010H	; V is calibrated
JOY_ISCAL_POV	EQU	00000020H	; POV is calibrated
; point of view constants
JOY_POV_NUMDIRS	EQU	4
JOY_POVVAL_FORWARD	EQU	0
JOY_POVVAL_BACKWARD	EQU	1
JOY_POVVAL_LEFT	EQU	2
JOY_POVVAL_RIGHT	EQU	3
; Specific settings for joystick hardware
JOY_HWS_HASZ	EQU	00000001H	; has Z info?
JOY_HWS_HASPOV	EQU	00000002H	; point of view hat present
JOY_HWS_POVISBUTTONCOMBOS	EQU	00000004H	; pov done through combo of buttons
JOY_HWS_POVISPOLL	EQU	00000008H	; pov done through polling
JOY_HWS_ISYOKE	EQU	00000010H	; joystick is a flight yoke
JOY_HWS_ISGAMEPAD	EQU	00000020H	; joystick is a game pad
JOY_HWS_ISCARCTRL	EQU	00000040H	; joystick is a car controller
; X defaults to J1 X axis
JOY_HWS_XISJ1Y	EQU	00000080H	; X is on J1 Y axis
JOY_HWS_XISJ2X	EQU	00000100H	; X is on J2 X axis
JOY_HWS_XISJ2Y	EQU	00000200H	; X is on J2 Y axis
; Y defaults to J1 Y axis
JOY_HWS_YISJ1X	EQU	00000400H	; Y is on J1 X axis
JOY_HWS_YISJ2X	EQU	00000800H	; Y is on J2 X axis
JOY_HWS_YISJ2Y	EQU	00001000H	; Y is on J2 Y axis
; Z defaults to J2 Y axis
JOY_HWS_ZISJ1X	EQU	00002000H	; Z is on J1 X axis
JOY_HWS_ZISJ1Y	EQU	00004000H	; Z is on J1 Y axis
JOY_HWS_ZISJ2X	EQU	00008000H	; Z is on J2 X axis
; POV defaults to J2 Y axis, if it is not button based
JOY_HWS_POVISJ1X	EQU	00010000H	; pov done through J1 X axis
JOY_HWS_POVISJ1Y	EQU	00020000H	; pov done through J1 Y axis
JOY_HWS_POVISJ2X	EQU	00040000H	; pov done through J2 X axis
; R defaults to J2 X axis
JOY_HWS_HASR	EQU	00080000H	; has R (4th axis) info
JOY_HWS_RISJ1X	EQU	00100000H	; R done through J1 X axis
JOY_HWS_RISJ1Y	EQU	00200000H	; R done through J1 Y axis
JOY_HWS_RISJ2Y	EQU	00400000H	; R done through J2 X axis
; U & V for future hardware
JOY_HWS_HASU	EQU	00800000H	; has U (5th axis) info
JOY_HWS_HASV	EQU	01000000H	; has V (6th axis) info
; Usage settings
JOY_US_HASRUDDER	EQU	00000001H	; joystick configured with rudder
JOY_US_PRESENT	EQU	00000002H	; is joystick actually present?
JOY_US_ISOEM	EQU	00000004H	; joystick is an OEM defined type

JOYCALIBRATE	STRUC
jcal_wXbase	DW	?
jcal_wXdelta	DW	?
jcal_wYbase	DW	?
jcal_wYdelta	DW	?
jcal_wZbase	DW	?
jcal_wZdelta	DW	?
JOYCALIBRATE	ENDS

joypos_tag	STRUC
jpos_dwX	DD	?
jpos_dwY	DD	?
jpos_dwZ	DD	?
jpos_dwR	DD	?
jpos_dwU	DD	?
jpos_dwV	DD	?
joypos_tag	ENDS

joyrange_tag	STRUC
jrange_jpMin	DB	SIZE joypos_tag DUP (?)
jrange_jpMax	DB	SIZE joypos_tag DUP (?)
jrange_jpCenter	DB	SIZE joypos_tag DUP (?)
joyrange_tag	ENDS

joyreguservalues_tag	STRUC
jreguv_dwTimeOut	DD	?
jreguv_jrvRanges	DB	SIZE joyrange_tag DUP (?)
jreguv_jpDeadZone	DB	SIZE joypos_tag DUP (?)
joyreguservalues_tag	ENDS

joyreghwsettings_tag	STRUC
jreghws_dwFlags	DD	?
jreghws_dwNumButtons	DD	?
joyreghwsettings_tag	ENDS

joyreghwvalues_tag	STRUC
jreghwv_jrvHardware	DB	SIZE joyrange_tag DUP (?)
jreghwv_dwPOVValues	DD	JOY_POV_NUMDIRS DUP (?)
reghwv_dwCalFlags	DD	?
joyreghwvalues_tag	ENDS

joyreghwconfig_tag	STRUC
jreghwc_hws	DB	SIZE joyreghwsettings_tag DUP (?)
jreghwc_dwUsageSettings	DD	?
jreghwc_hwv	DB	SIZE joyreghwvalues_tag DUP (?)
jreghwc_dwType	DD	?
joyreghwconfig_tag	ENDS
; messages sent to joystick driver's DriverProc() function
JDD_GETNUMDEVS	EQU	<(DRV_RESERVED+0001H)>
JDD_GETDEVCAPS	EQU	<(DRV_RESERVED+0002H)>
JDD_GETPOS	EQU	<(DRV_RESERVED+0101H)>
JDD_SETCALIBRATION	EQU	<(DRV_RESERVED+0102H)>
JDD_CONFIGCHANGED	EQU	<(DRV_RESERVED+0103H)>
JDD_GETPOSEX	EQU	<(DRV_RESERVED+0104H)>
endif  ;ifndef MMNOJOYDEV
ifndef MMNOMCIDEV
ifndef _WIN32_VXD
;***************************************************************************
;                      MCI device driver support
;*************************************************************************
; internal MCI messages
MCI_OPEN_DRIVER	EQU	<(DRV_RESERVED+0001H)>
MCI_CLOSE_DRIVER	EQU	<(DRV_RESERVED+0002H)>
; string return values only used with MAKEMCIRESOURCE
MCI_FALSE	EQU	<(MCI_STRING_OFFSET+19)>
MCI_TRUE	EQU	<(MCI_STRING_OFFSET+20)>
; resource string return values
MCI_FORMAT_RETURN_BASE	EQU	<MCI_FORMAT_MILLISECONDS_S>
MCI_FORMAT_MILLISECONDS_S	EQU	<(MCI_STRING_OFFSET+21)>
MCI_FORMAT_HMS_S	EQU	<(MCI_STRING_OFFSET+22)>
MCI_FORMAT_MSF_S	EQU	<(MCI_STRING_OFFSET+23)>
MCI_FORMAT_FRAMES_S	EQU	<(MCI_STRING_OFFSET+24)>
MCI_FORMAT_SMPTE_24_S	EQU	<(MCI_STRING_OFFSET+25)>
MCI_FORMAT_SMPTE_25_S	EQU	<(MCI_STRING_OFFSET+26)>
MCI_FORMAT_SMPTE_30_S	EQU	<(MCI_STRING_OFFSET+27)>
MCI_FORMAT_SMPTE_30DROP_S	EQU	<(MCI_STRING_OFFSET+28)>
MCI_FORMAT_BYTES_S	EQU	<(MCI_STRING_OFFSET+29)>
MCI_FORMAT_SAMPLES_S	EQU	<(MCI_STRING_OFFSET+30)>
MCI_FORMAT_TMSF_S	EQU	<(MCI_STRING_OFFSET+31)>
MCI_VD_FORMAT_TRACK_S	EQU	<(MCI_VD_OFFSET+5)>
WAVE_FORMAT_PCM_S	EQU	<(MCI_WAVE_OFFSET+0)>
WAVE_MAPPER_S	EQU	<(MCI_WAVE_OFFSET+1)>
MCI_SEQ_MAPPER_S	EQU	<(MCI_SEQ_OFFSET+5)>
MCI_SEQ_FILE_S	EQU	<(MCI_SEQ_OFFSET+6)>
MCI_SEQ_MIDI_S	EQU	<(MCI_SEQ_OFFSET+7)>
MCI_SEQ_SMPTE_S	EQU	<(MCI_SEQ_OFFSET+8)>
MCI_SEQ_FORMAT_SONGPTR_S	EQU	<(MCI_SEQ_OFFSET+9)>
MCI_SEQ_NONE_S	EQU	<(MCI_SEQ_OFFSET+10)>
MIDIMAPPER_S	EQU	<(MCI_SEQ_OFFSET+11)>

MCI_OPEN_DRIVER_PARMS	STRUC
mciodrv_wDeviceID	DW	?
mciodrv_lpstrParams	DD	?
mciodrv_wCustomCommandTable	DW	?
mciodrv_wType	DW	?
MCI_OPEN_DRIVER_PARMS	ENDS
; maximum length of an MCI device type
MCI_MAX_DEVICE_TYPE_LENGTH	EQU	80
; flags for mciSendCommandInternal() which direct mciSendString() how to
; interpret the return value
MCI_RESOURCE_RETURNED	EQU	00010000H  ; resource ID
MCI_COLONIZED3_RETURN	EQU	00020000H  ; colonized ID, 3 bytes data
MCI_COLONIZED4_RETURN	EQU	00040000H  ; colonized ID, 4 bytes data
MCI_INTEGER_RETURNED	EQU	00080000H  ; integer conversion needed
MCI_RESOURCE_DRIVER	EQU	00100000H  ; driver owns returned resource
; invalid command table ID
MCI_NO_COMMAND_TABLE	EQU	-1
; command table information type tags
MCI_COMMAND_HEAD	EQU	0
MCI_STRING	EQU	1
MCI_INTEGER	EQU	2
MCI_END_COMMAND	EQU	3
MCI_RETURN	EQU	4
MCI_FLAG	EQU	5
MCI_END_COMMAND_LIST	EQU	6
MCI_RECT	EQU	7
MCI_CONSTANT	EQU	8
MCI_END_CONSTANT	EQU	9
endif  ;ifndef _WIN32_VXD
endif  ;ifndef MMNOMCIDEV
ifndef MMNOTASKDEV
TASKERR_NOTASKSUPPORT	EQU	1
TASKERR_OUTOFMEMORY	EQU	2
endif  ;ifndef MMNOTASKDEV
