;===============================================================================
;		File	WNETCAPS.INC
;===============================================================================

WNNC_SPEC_VERSION			equ	0001h

WNNC_NET_TYPE				equ	0002h

	WNNC_NET_None			equ	0000h
	WNNC_NET_MSNet			equ	0100h
	WNNC_NET_LanMan 		equ	0200h
	WNNC_NET_NetWare		equ	0300h
	WNNC_NET_Vines			equ	0400h

WNNC_DRIVER_VERSION			equ	0003h

WNNC_USER				equ	0004h

	WNNC_USR_GetUser		equ	0001h

WNNC_CONNECTION 			equ	0006h

	WNNC_CON_AddConnection		equ	0001h
	WNNC_CON_CancelConnection	equ	0002h
	WNNC_CON_GetConnections 	equ	0004h

WNNC_PRINTING				equ	0007h

	WNNC_PRT_OpenJob		equ	0002h
	WNNC_PRT_CloseJob		equ	0004h
	WNNC_PRT_GetQueue		equ	0008h
	WNNC_PRT_HoldJob		equ	0010h
	WNNC_PRT_ReleaseJob		equ	0020h
	WNNC_PRT_CancelJob		equ	0040h
	WNNC_PRT_SetJobCopies		equ	0080h
	WNNC_PRT_WatchQueue		equ	0100h
	WNNC_PRT_UnwatchQueue		equ	0200h
	WNNC_PRT_LockQueueData		equ	0400h
	WNNC_PRT_UnlockQueueData	equ	0800h
	WNNC_PRT_ChangeMsg		equ	1000h
	WNNC_PRT_AbortJob		equ	2000h
	WNNC_PRT_NoArbitraryLock	equ	4000h
	WNNC_PRT_WriteJob		equ	8000h

WNNC_DEVMODE				equ	0008h

	WNNC_DEV_Devmode		equ	0001h

WNNC_ERROR				equ	000ah

	WNNC_ERR_GetError		equ	0001h
	WNNC_ERR_GetErrorInfo		equ	0002h

WNNC_BROWSE				equ	000bh

	WNNC_BRW_BrowseDialog		equ	0001h
