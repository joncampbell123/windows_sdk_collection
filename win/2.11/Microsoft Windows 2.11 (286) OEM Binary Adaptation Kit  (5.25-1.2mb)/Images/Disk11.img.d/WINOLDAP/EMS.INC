ifndef SHORTER

; some useful constants for EEMS

KBYTES			equ	1024
PARAGRAPH		equ	16
LOG2_KBYTES		equ	10
LOG2_PARAGRAPH		equ	4
PDB_SIZE		equ	10H	; Size in paras of DOS PDB
FRAME_SIZE		equ	1000H	; 64Kb LIM frame size in paras
BANK_WRITE		equ	0	; Write function in RWBank
BANK_READ		equ	1	; Read function in RWBank
;
; the EMM commands

EMM_API			equ	67h

wEMM_STATUS		equ	word ptr 4000h
wEMM_PAGE_FRAME		equ	word ptr 4100h
wEMM_PAGE_COUNTS	equ	word ptr 4200h
wEMM_GET_PID16		equ	word ptr 4300h
wEMM_MAP_PAGE		equ	word ptr 4400h
wEMM_FREE_PID		equ	word ptr 4500h
wEMM_VERSION		equ	word ptr 4600h
wEMM_SAVE_MAP		equ	word ptr 4700h
wEMM_RESTORE_MAP	equ	word ptr 4800h
wEMM_GET_PID_PAGES	equ	word ptr 4C00h
;
ifndef EMS4

wEMM_CONTEXT_SAVE	equ	word ptr 4E00h
wEMM_CONTEXT_RESTORE	equ	word ptr 4E01h
wEMM_CONTEXT_SWITCH	equ	word ptr 4E02h
wEMM_CONTEXT_SIZE	equ	word ptr 4E03h

endif

ifdef EMS4

wEMM_CONTEXT_SAVE	equ	word ptr 5C00h
wEMM_CONTEXT_RESTORE	equ	word ptr 5C01h
wEMM_CONTEXT_SWITCH	equ	word ptr 5C02h
wEMM_CONTEXT_SIZE	equ	word ptr 5C03h

endif

wEMM_MAP_PHYSICAL_PAGES	equ	word ptr 5001h
;
wEMM_REALLOCATE_FOR_PID	equ	word ptr 5100h
;
wEMM_MAPPABLE_ARRAY	equ	word ptr 5800h
wEMM_MAPPABLE_ARRAY_LEN	equ	word ptr 5801h
wEMM_HARDWARE_CAPS	equ	word ptr 5900h
wEMM_RAW_PAGE_COUNTS	equ	word ptr 5901h
;
wEMM_GET_RAW_PID	equ	word ptr 5A00h
;
wEMM_GET_REGISTER_SET	equ	word ptr 5B00h
wEMM_SET_REGISTER_SET	equ	word ptr 5B01h
wEMM_SWITCH_REGISTER_SET equ	word ptr 5B02h
wEMM_REGISTER_SET_LEN	equ	word ptr 5B03h
wEMM_ALLOC_REGISTER_SET	equ	word ptr 5B04h
wEMM_FREE_REGISTER_SET	equ	word ptr 5B05h
wEMM_MAP_DMA_CHANNEL	equ	word ptr 5B06h
wEMM_UNMAP_DMA_CHANNEL	equ	word ptr 5B07h

endif ; SHORTER
;
; EEMS info structure
; 
; Structure fields are expected to be zero-initialized 
;
EEMSInfo	Struc
 Pageperframe	dw	?	; # raw pages per LIM pageframe
 PageSize	dw	?	; Raw page size in paras
 StartFrame	dw	? 	; Starting segment of LIM frame
 EndFrame	dw	?	; Next segment after LIM frame
				; EndFrame = StartFrame +FrameSize
 LowestBankSeg	dw	?	; Lowest EEMS physical segment
 ContextSize	dw	?	; EEMS context size in paras
 Map_Pages	dw	?	; # mappable pages (4 bytes per entry)
 ConvSize	dw	?       ; = LowestBankseg - (TopPDB +PDB_SIZE)
 TopPDBbank	dw	?	; EEMS physical segment where TopPDB resides
 PID0Pages	dw	?	; # raw pages owned by PID0 (0..255)
 BankLine	dw	?	; line dividing bankable and non-bankable segs
 WOAContextSize	dw	?	; Winoldap Mapping array context in paras
 WOAMapWords	dw	?	; Its size in words
 fEEMS		db	?	; = 1: EEMS supported
		db	?	; Word aligned
EEMSInfo	ends
;
EEMSInfoSize	equ	(SIZE EEMSInfo)
;
ifndef SHORTER

EEMSSwapInfo	Struc
 SaveWinPID	dw	?	; PID for saving Windows in conventional memory
 SaveAppPID	dw	?	; PID for saving App in conventional memory
 AppPID		dw      ?	; PID for App portion in EEMS
 PartitionPages dw	?	; # pages allocated to instance partition
 AppPages	dw	?	; # raw logical pages assigned to App
 WinContextSeg	dw	?	; Windows EEMS context save segment
 WOAContextSeg	dw	?	; Winoldap mapping save segment
 AppContextSeg	dw	?	; App EEMS context save segment
 CurrentPage	dw	?	; Current free logical page related to active PID
 CurrentPara	dw	?	; Current free para in LIM frame
EEMSSwapInfo	ends
;
EEMSSwapInfoSize equ	(SIZE EEMSSwapInfo)

EEMShwInfo	struc
 hwi_pagesize	dw	?	; Raw page size in paras
 hwi_extra_regs dw	?	; # extra regs sets
 hwi_context	dw	?	; # bytes needed to save a register set context
 hwi_DMA	dw	?	; # settable DMA channels
EEMShwInfo	Ends
;
EEMShwInfoSize	equ 	(SIZE EEMShwInfo)

endif           ;SHORTER
