;
; Copyright (C) Microsoft, 1988
;
IOC_DC			equ	8	    ; catagory 8 IOCTLs
IOC_PC			equ	9	    ; catagory 9 IOCTLs
GEN_IOCTL_FN_TST	equ	20H	    ; Used to differentiate between
NON_STD_GEN_IOCTL_FN_TST	equ	80H ; Used to differentiate between

; Function codes to pass in IOCTL request packet

IOCTL_Format	equ	1
IOCTL_Verify	equ	2
IOCTL_Read	equ	3
IOCTL_Write	equ	4
IOCTL_FmtVerify equ	5

IOCTL_ExtDrive	equ	22H			; InstallBDS function
IOCTL_DMAOVERRRUN equ	5FH			; Dma Overrun function
IOCTL_READBACK	  equ	5EH			; @@READBACK function

; internal packet generated for IOCTL requests

IOC_Fun 	equ	byte ptr PktData	; internal function code
IOC_IOpData	EQU	DWORD PTR PktData+1	; Data transfer address ~~~
IOC_Count	EQU	WORD PTR PktData+5	; Sector Count		~~~
IOC_Cyln	equ	word ptr PktData+7	; cylinder
IOC_StartSec	equ	word ptr PktData+9	; starting sector on track
IOC_Head	equ	word ptr PktData+11	; head
IOC_IOPhysRBA	equ	dword ptr PktData+13	; overlaps with IOPhysRBA field
IOC_SecSiz	equ	byte ptr PktData+17	; sector size index
LENSpecPkt	equ	18
ERRNZ		<IOPhysRBA - IOC_IOPhysRBA>	; Ensure overlap
ERRNZ		<PktMax LT LenSpecPkt>

DDI_DeviceParameters_param  struc
	DDI_DP_Command	    db	    ?
DDI_DeviceParameters_param  ends

ReadBack_data		    struc		; @@readback
	RBAction	    db	    ?
	RBDmaOverrunCount   dw	    ?
	RBCount 	    dw	    ?
	RBMiscompares	    dw	    ?
ReadBack_data		    ends

DDI_DeviceParameters_data   struc
	DDI_DP_BPB	    db	    size a_BPB dup (?)
	DDI_DP_Cyln	    dw	    ?
	DDI_DP_DeviceType   db	    ?
	DDI_DP_DeviceAttr   dw	    ?
DDI_DeviceParameters_data   ends


; Category 9, function 63H, get physical device parameters

DDI_PhysDeviceParameters_data	struc
	DDI_PDP_Reserved1   dw	    ?
	DDI_PDP_Cylinders   dw	    ?
	DDI_PDP_Heads	    dw	    ?
	DDI_PDP_Sectors     dw	    ?
	DDI_PDP_Reserved2   dw	    4 dup (?)
DDI_PhysDeviceParameters_data	ends


; Format and Verify track

DDI_FormatPacket_param	struc
	DDI_FP_Command	    db	    ?
	DDI_FP_Head	    dw	    ?
	DDI_FP_Cyln	    dw	    ?
	DDI_FP_Reserved     dw	    ?
	DDI_FP_NumSec	    dw	    ?
	DDI_FP_TTable	    dd	    ?
DDI_FormatPacket_param	ends

; Read/Write/Verify Track

DDI_RWVPacket_param struc
	DDI_RWV_Command     db	    ?
	DDI_RWV_Head	    dw	    ?
	DDI_RWV_Cyln	    dw	    ?
	DDI_RWV_Sec	    dw	    0
	DDI_RWV_NumSec	    dw	    ?
	DDI_RWV_TTable	    dd	    ?
DDI_RWVPacket_param ends

DDI_MapPacket	struc
	DDI_MP_Drive	    db	    ?
DDI_MapPacket	ends
