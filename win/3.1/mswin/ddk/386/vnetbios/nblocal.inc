PAGE 58,132
;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1989
;
;   Title:	NBLOCAL.INC - Equates and structures for VNETBIOS VxD
;
;   Version:	1.00
;
;   Date:	13-Mar-1989
;
;   Author:	RAL
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;   13-Mar-1989 RAL Original
;
;==============================================================================


;
; NetBIOS return codes
;
RC_Good 	EQU	000h
RC_Invalid	EQU	003h
RC_Max_Cmd	EQU	022h
RC_Invalid_LANA EQU	023h
RC_In_Progress	EQU	024h
RC_Pending	EQU	0FFh

;
; Some NetBIOS command codes
;
SessCall	EQU	10h		; Call command
SessListen	EQU	11h		; Listen command
HangUp		EQU	12h		; Hang up command
Send		EQU	14h		; Send command
Recv		EQU	15h		; Recv command
RecvAny 	EQU	16h		; Recv any command
ChainSend	EQU	17h		; Chain send command
SendGram	EQU	20h		; Send datagram
RecvGram	EQU	21h		; Recv datagram
SendBroad	EQU	22h		; Send broadcast datagram
RecvBroad	EQU	23h		; Recv broadcast datagram
AddName 	EQU	30h		; Add name command
DelName 	EQU	31h		; Delete name command
Reset		EQU	32h		; Reset command
AdptStat	EQU	33h		; Adapter status command
SessStat	EQU	34h		; Session status command
Cancel		EQU	35h		; Cancel command
AddGName	EQU	36h		; Add group name command
Unlink		EQU	70h		; Unlink command
Install 	EQU	7Fh		; Install check

NoWait		EQU	80h		; APPLIES TO ALL COMMANDS (High bit)


;
; some UB netbios command codes
;
UBNB_Register   EQU     72h
UBNB_SendNmc    EQU     73h
UBNB_Callniu    EQU     74h
UBNB_Calladdr   EQU     75h
UBNB_Listenaddr EQU     76h
UBNB_SendPkt    EQU     77h
UBNB_RcvPkt     EQU     78h
UBNB_SendAttn   EQU     79h
UBNB_RcvAttn    EQU     7Ah
UBNB_Listenniu  EQU     7Bh
UBNB_RcvRaw     EQU     7Ch
UBNB_SendNmc2   EQU     7Dh


;******************************************************************************
;			     S T R U C T U R E S
;******************************************************************************
;
;   Network Control Block structure
;
NCB_Struc	STRUC
NCB_Command	db	?		; Command
NCB_RetCode	db	?		; Return code
NCB_LSN 	db	?		; Local session #
NCB_Num 	db	?
NCB_Buffer_Off	dw	?		; Buffer offset
NCB_Buffer_Seg	dw	?		; Buffer segment
NCB_Length	dw	?		; Buffer length
NCB_CallName	db 16 dup (?)		; Name on local or remote adapter
NCB_Name	db 16 dup (?)		; Name on local adapter
NCB_RTO 	db	?		; Receive timeout
NCB_STO 	db	?		; Send timeout
NCB_Post_Off	dw	?		; Post routine offset
NCB_Post_Seg	dw	?		; Post routine segment
NCB_LanA_Num	db	?		; Adapter #
NCB_Cmd_Cplt	db	?		; Command status field
NCB_Reserved	db 14 dup (?)		; Reserved
NCB_Struc	ENDS


HCB_Next	EQU	DWORD PTR  [-04h]
HCB_Flags	EQU	DWORD PTR  [-08h]
HCB_VM_Handle	EQU	DWORD PTR [-0Ch]
HCB_Real_NCB	EQU	DWORD PTR [-10h]
HCB_NCB_Lin_Addr EQU	DWORD PTR [-14h]
HCB_Buf1_Handle EQU	DWORD PTR [-18h]
HCB_Buf2_Handle EQU	DWORD PTR [-1Ch]

HCB_Header_Size EQU	1Ch
HCB_Size	EQU	SIZE NCB_Struc + HCB_Header_Size

;
;   Other equates for accessing the NCB structure
;
NCB_Buffer_Ptr	EQU	DWORD PTR [NCB_Buffer_Off]
NCB_Post_Ptr	EQU	DWORD PTR [NCB_Post_Off]

;
;   Equates for ChainSend
;
NCB_Length2	EQU	WORD PTR [NCB_CallName]
NCB_Buff2_Off	EQU	WORD PTR [NCB_CallName+2]
NCB_Buff2_Seg	EQU	WORD PTR [NCB_CallName+4]
NCB_Buffer2_Ptr EQU	DWORD PTR [NCB_CallName+2]


VNetBIOS_CB_Struc   STRUC
VN_CB_Hook_List     dd	    0
VN_CB_Flags	    dd	    0
VNetBIOS_CB_Struc   ENDS

VNF_VM_Dead	    EQU     00000001b
VNF_VM_Dead_Bit     EQU     0
VNF_In_Crit	    EQU     00000010b
VNF_In_Crit_Bit     EQU     1
VNF_Timed_Out	    EQU     00000100b
VNF_Timed_Out_Bit   EQU     2


HF_Wait_For_IRET	EQU	000000001b
HF_Wait_For_IRET_Bit	EQU	0
HF_Wait_For_POST	EQU	000000010b
HF_Wait_For_POST_BIT	EQU	1
HF_Wait_For_Sim_POST	EQU	000000100b
HF_Wait_For_Sim_POST_Bit EQU	2
HF_POST_Crit		EQU	000001000b
HF_POST_Crit_Bit	EQU	3
HF_From_PM		EQU	000010000b
HF_From_PM_Bit		EQU	4
HF_NCB_Active		EQU	000100000b
HF_NCB_Active_Bit	EQU	5
HF_Canceled		EQU	001000000b
HF_Canceled_Bit 	EQU	6
HF_From_Int2A		EQU	010000000b
HF_From_Int2A_Bit	EQU	7
HF_Canceling		EQU	100000000b
HF_Canceling_Bit	EQU	8

HF_Busy_Mask EQU HF_Wait_For_IRET OR HF_Wait_For_POST OR HF_Wait_For_Sim_POST OR HF_POST_Crit OR HF_Canceling
