/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
****************************************************************************/

/*
 *  EISA.H - 	EISA include file for structure definitions of the private
 *		info the EISA enumerator uses.
 *
 */

#define	EISA_FUNCTION_INFO_SIZE		320
#define	EISA_STACK_SIZE			1536
#define	EISA_NUM_SLOTS			16
#define EISA_BIOS_OFFSET 		0xf859
#ifndef	NEC_98
#define EISA_SIGNATURE_OFFSET		0xffd9
#define EISA_SIGNATURE			0x41534945	//"EISA"
#else	//NEC_98
#define EISA_SIGNATURE_OFFSET		0x0458
#define EISA_SIGNATURE			0x80	//"NESA"
#endif	//NEC_98
#define	EISA_DESCRIPTION_LENGTH		80

#define	EISA_FUNCTION_FLAG_DISABLED	0x80
#define	EISA_FUNCTION_FLAG_FREE_FORM	0x40
#define	EISA_FUNCTION_FLAG_PORT_INIT	0x20
#define	EISA_FUNCTION_FLAG_PORT		0x10
#define	EISA_FUNCTION_FLAG_DMA		0x08
#define	EISA_FUNCTION_FLAG_IRQ		0x04
#define	EISA_FUNCTION_FLAG_MEMORY	0x02
#define	EISA_FUNCTION_FLAG_DESCRIPTION	0x01

#define	EISA_RESOURCES			4
#define	EISA_RES_NEXT			0x80
#define	EISA_ANY_RES			0x1E

#define	EISA_SLOT_FLAGS_INCOMPLETE	0x8000
#define	EISA_SLOT_FLAGS_LOCKED		0x0400
#define	EISA_SLOT_FLAGS_IOCHKERR	0x0200
#define	EISA_SLOT_FLAGS_DISABLEABLE	0x0100
#define	EISA_SLOT_FLAGS_DUPLICATE	0x0080
#define	EISA_SLOT_FLAGS_NO_ID		0x0040

#ifdef	NEC_98
#define	NESA_FO_INT					0x1F
#define	NESA_FO_GET_DEVICE_ID		0xC2
#define	NESA_FO_GET_REGISTER_INFO	0xC3
#define	NESA_FO_GET_NESA_SET		0xC4
#define	NESA_FO_GET_NESA_SET_IRQ	0x10
#define	NESA_FO_GET_NESA_SET_DMA	0x20
#define	NESA_FO_GET_NESA_SET_IO		0x30
#define	NESA_FO_GET_NESA_SET_MEM	0x40
#define	resMEM	0x73
#define	resIRQ	0xB2
#define	resDMA	0xC0
#define	resIO	0xC8
#endif	//NEC_98

struct	EISA_function_info_s {
	DWORD				dwID;
	WORD				wSlotFlags;
	WORD				wCFGLevel;
	BYTE				Selection[26];
	BYTE				bFunctionFlags;
	BYTE				bData[(EISA_FUNCTION_INFO_SIZE-35)];
};

typedef	struct EISA_function_info_s	EISAFUNCTIONINFO;
typedef	EISAFUNCTIONINFO		*PEISAFUNCTIONINFO;

struct	EISA_adapter_s {
	struct EISA_adapter_s		*pNextAdapter;
	DWORD				dwSlotID;
	BYTE				bSlotFlags;
	BYTE				bSlot;
	BYTE				bNumFunctions;
	struct EISA_function_info_s	FunctionInfo[1];
};

typedef	struct EISA_adapter_s		EISAADAPTER;
typedef	EISAADAPTER			*PEISAADAPTER;

struct	EISA_MEM_s {
	BYTE				bMemFlags;
	BYTE				bMemDataSize;
	BYTE				bBase[3];
	WORD				wLen;
};

typedef	struct EISA_MEM_s		EISAMEM;
typedef	EISAMEM				*PEISAMEM;

#define	EISA_IRQ_SHARED			0x40

struct	EISA_IRQ_s {
	BYTE				bIRQFlags;
	BYTE				bIRQReserved;
};

typedef	struct EISA_IRQ_s		EISAIRQ;
typedef	EISAIRQ				*PEISAIRQ;

struct	EISA_IO_s {
	BYTE				bIOFlags;
	WORD				wIOPort;
};

typedef	struct EISA_IO_s		EISAIO;
typedef	EISAIO				*PEISAIO;

struct	EISA_DMA_s {
	BYTE				bDMAFlags;
	BYTE				bDMATiming;
};

typedef	struct EISA_DMA_s		EISADMA;
typedef	EISADMA				*PEISADMA;

