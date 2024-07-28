;===============================================================================
;		File	SMB.INC
;===============================================================================

SMBMakeDirectory			equ	0000h
SMBRemoveDirectory			equ	0001h
SMBOpenHandle				equ	0002h
SMBCreateHandle 			equ	0003h
SMBCloseHandle				equ	0004h
SMBFlushHandle				equ	0005h
SMBDeleteFile				equ	0006h
SMBRenameFile				equ	0007h
SMBGetFileAttributes			equ	0008h
SMBSetFileAttributes			equ	0009h
SMBReadHandle				equ	000ah
SMBWriteHandle				equ	000bh
SMBLockHandle				equ	000ch
SMBUnlockHandle 			equ	000dh
SMBCreateTemporaryFile			equ	000eh
SMBMakeNewFile				equ	000fh
SMBCheckPath				equ	0010h
SMBExit 				equ	0011h
SMBLSeek				equ	0012h
SMBTreeConnect				equ	0070h
SMBTreeDisconnect			equ	0071h
SMBNegotiateProtocol			equ	0072h
SMBGetDiskAttributes			equ	0080h
SMBSearchDirectory			equ	0081h
SMBOpenSpoolFile			equ	00c0h
SMBWriteSpoolFile			equ	00c1h
SMBCloseSpoolFile			equ	00c2h
SMBReturnPrintQueue			equ	00c3h
SMBSendSingleBlockMessage		equ	00d0h
SMBSendBroadcastMessage 		equ	00d1h
SMBForwardUserName			equ	00d2h
SMBCancelForwardName			equ	00d3h
SMBGetMachineName			equ	00d4h
SMBStartMultiblockMessage		equ	00d5h
SMBEndMultiblockMessage 		equ	00d6h
SMBSendMultiblockMessage		equ	00d7h

;		SMB Fixed Data Structure

SMB		struc
		smbHeader		db	0ffh, 'SMB'
		smbCommandCode		db	?
		smbErrorCodeClass	db	?
		smbReserved1		db	?
		smbErrorCode		dw	?
		smbReserved2		db	?
		smbReserved3		dw	7 dup ( ? )
		smbTreeID		dw	?
		smbCallersProcessID	dw	?
		smbUserID		dw	?
		smbMultiplexID		dw	?
		smbParameterCount	db	?
SMB		ends

;		SMB Request For Print Queue Entry

SMBGetPrintQ	struc
		smbGPQHeader		db	size SMB dup ( ? )
		smbGPQMaxCount		dw	?
		smbGPQStartIndex	dw	?
		smbGPQByteCount 	dw	?
		smbGPQDataFormat	db	?
		smbGPQDataLength	dw	?
SMBGetPrintQ	ends

;		SMB Print List Entry Data Structure

SMBPrintQEntry	struc
		smbPQEDate		dw	?
		smbPQETime		dw	?
		smbPQEStatus		db	?
		smbPQESpoolFileNumber	dw	?
		smbPQESpoolFileSize	dd	?
		smbPQEReserved		db	?
		smbPQEOriginatorName	db	16 dup ( ? )
SMBPrintQEntry	ends

;		Date format in SMB print queue entry

smbdate 	   record  YearBits:7, MonthBits:4, DayBits:5
smbtime 	   record  HourBits:5, MinuteBits:6, SecondBits:5
