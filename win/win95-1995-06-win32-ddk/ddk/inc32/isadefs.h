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
 *  ISADEFS.H - ISA P&P definitions for configuration structures
 *
 *  Notes:
 *
 *	This is included by all ISAPNP parsers.
 */

#ifndef NOISADEFS
#define NOISADEFS

#define	ISA_MAX_TAGS				256
#define	ISA_FIRST_DATA_BYTE			9
#define	ISA_IS_LARGE_ITEM(p)			(((*(p))&0x80)==0x80)
#define	ISAS_ITEM_NO_LENGTH(b)			((BYTE)((b)&0xF8))
#define	ISAS_ITEM_LENGTH(p)			((ULONG)(((*(p))&0x7)+1))
#define	ISAS_VERSION				0x08
#define	ISAS_LOG_DEV				0x10
#define	ISAS_COMP_DEV				0x18
#define	ISAS_IRQ_DESC				0x20
#define	ISAS_DMA_DESC				0x28
#define	ISAS_IO_DESC				0x40
#define	ISAS_IO_FIXED_DESC			0x48
#define	ISAS_DF_START				0x30
#define	ISAS_DF_END				0x38
#define	ISAS_END				0x78
#define	ISAL_MEM_DESC				0x81
#define	ISAL_ANSI_DESC				0x82
#define	ISAL_MEM_32_DESC			0x85
#define	ISAL_MEM_32_FIXED_DESC			0x86
#define	ISAL_ITEM_LENGTH(p)			((ULONG)((WORD)(*(PWORD)(p+1))+3))

struct	isa_version_desc_s {
	BYTE	bISAVerTag;			// Resource tag
	BYTE	bISAVerMinPnP;			// Minimum PnP version
	BYTE	bISAVendorVersion;		// Vendor version
};

typedef	struct isa_version_desc_s	ISA_VERSION_DESC;
typedef	ISA_VERSION_DESC		*PISA_VERSION_DESC;

#define	SIZEOF_ISA_VERSION_DESC sizeof(ISA_VERSION_DESC)

struct	isa_log_dev_desc_s {
	BYTE	bISALogDevTag;
	DWORD	dwISALogDevID;
	BYTE	bISALogDevBoot;
	BYTE	bISALogDevRegisters;
};

typedef	struct isa_log_dev_desc_s	ISA_LOG_DEV_DESC;
typedef	ISA_LOG_DEV_DESC		*PISA_LOG_DEV_DESC;

#define	SIZEOF_ISA_LOG_DEV_DESC sizeof(ISA_LOG_DEV_DESC)

struct	isa_comp_dev_desc_s {
	BYTE	bISACompDevTag;
	DWORD	dwISACompDevID;
};

typedef	struct isa_comp_dev_desc_s	ISA_COMP_DEV_DESC;
typedef	ISA_COMP_DEV_DESC		*PISA_COMP_DEV_DESC;

#define	SIZEOF_ISA_COMP_DEV_DESC sizeof(ISA_COMP_DEV_DESC)

struct	isa_ansi_desc_s {
	BYTE	bISAAnsiTag;
	WORD	wISAAnsiLen;
	BYTE	bISAAnsiText;
};

typedef	struct isa_ansi_desc_s	ISA_ANSI_DESC;
typedef	ISA_ANSI_DESC		*PISA_ANSI_DESC;

#define	SIZEOF_ISA_ANSI_DESC sizeof(ISA_ANSI_DESC)

// IRQ attribute flags

#define	ISA_IRQ_TRUE_EDGE_SENSITIVE		0x1
#define	ISA_IRQ_LOW_EDGE_SENSITIVE		0x2
#define	ISA_IRQ_HIGH_TRUE_LEVEL_SENSITIVE	0x3
#define	ISA_IRQ_LOW_TRUE_LEVEL_SENSITIVE	0x4

struct	isa_irq_desc_s {
	BYTE	bISAIrqTag;			// IRQ resource DESC tag
	WORD	wISAIrq0to15;			// IRQ 0-15 mask
	BYTE	bISAIrqAttr;			// IRQ attributes
};

typedef	struct isa_irq_desc_s	ISA_IRQ_DESC;
typedef	ISA_IRQ_DESC		*PISA_IRQ_DESC;

#define	SIZEOF_ISA_IRQ_DESC sizeof(ISA_IRQ_DESC)

// DMA attribute flags

#define	ISA_DMA_TRANSFER_SIZE8			0x00
#define	ISA_DMA_TRANSFER_SIZE816		0x01
#define	ISA_DMA_TRANSFER_SIZE16			0x02
#define	ISA_DMA_IS_ISA_MASTER			0x04
#define	ISA_DMA_COUNT_BYTES			0x08
#define	ISA_DMA_COUNT_WORDS			0x10

struct	isa_dma_desc_s {
	BYTE	bISADmaTag;			// DMA resource DESC tag
	BYTE	bISADma0to7;			// DMA channel mask
	BYTE	bISADmaAttr;			// DMA attributes
};

typedef	struct isa_dma_desc_s	ISA_DMA_DESC;
typedef	ISA_DMA_DESC		*PISA_DMA_DESC;

#define	SIZEOF_ISA_DMA_DESC sizeof(ISA_DMA_DESC)

struct	isa_io_desc_s {
	BYTE	bISAIoTag;			// Resource tag
	BYTE	bISAIoAttr;			// I/O port attributes
	WORD	wISAIoMinBase;			// Minimum base address
	WORD	wISAIoMaxBase;			// Maximum base address
	BYTE	bISAIoIncr;			// alignment or increment
	BYTE	bISAIoDescLen;			// Range length
};

typedef	struct isa_io_desc_s	ISA_IO_DESC;
typedef	ISA_IO_DESC		*PISA_IO_DESC;

#define	SIZEOF_ISA_IO_DESC sizeof(ISA_IO_DESC)

struct	isa_io_fixed_desc_s {
	BYTE	bISAIoFixedTag;			// Resource tag
	WORD	wISAIoFixedBase;		// Base address
	BYTE	bISAIoFixedLen;			// Range length
};

typedef	struct isa_io_fixed_desc_s	ISA_IO_FIXED_DESC;
typedef	ISA_IO_FIXED_DESC		*PISA_IO_FIXED_DESC;

#define	SIZEOF_ISA_IO_DESC sizeof(ISA_IO_DESC)

#define	ISA_MEM_WRITEABLE		0x00000001

struct	isa_mem_desc_s {
	BYTE	bISAMemTag;			// Memory resource DESC tag
	WORD	wISAMemDesLen;			// Length of the resource desciptor
	BYTE	bISAMemAttr;			// Attributes
	WORD	wISAMemMinBase;			// Range minimum base address
	WORD	wISAMemMaxBase;			// Range maximum base address
	WORD	wISAMemIncr;			// Base alignment or increment
	WORD	wISAMemLen;			// Range length in 256 byte blocks
};

typedef	struct isa_mem_desc_s	ISA_MEM_DESC;
typedef	ISA_MEM_DESC		*PISA_MEM_DESC;

#define	SIZEOF_ISA_MEM_DESC sizeof(ISA_MEM_DESC)

struct	isa_ext_mem_desc_s {
	BYTE	bISAExtMemTag;			// Memory resource DESC tag
	WORD	wISAExtMemDesLen;		// Length of the resource desciptor
	BYTE	bISAExtMemAttr;			// Attributes
	DWORD	dwISAExtMemMinBase;		// Range minimum base address
	DWORD	dwISAExtMemMaxBase;		// Range maximum base address
	DWORD	dwISAExtMemIncr;		// Base alignment or increment
	DWORD	dwISAExtMemLen;			// Range length
};

typedef	struct isa_ext_mem_desc_s	ISA_EXT_MEM_DESC;
typedef	ISA_EXT_MEM_DESC		*PISA_EXT_MEM_DESC;

#define	SIZEOF_ISA_EXT_MEM_DESC sizeof(ISA_EXT_MEM_DESC)

struct	isa_ext_mem_fixed_desc_s {
	BYTE	bISAExtMemFixedTag;		// Memory resource DESC tag
	WORD	wISAExtMemFixedDesLen;		// Length of the resource desciptor
	BYTE	bISAExtMemFixedAttr;		// Attributes
	DWORD	dwISAExtMemFixedBase;		// Range minimum base address
	DWORD	dwISAExtMemFixedLen;		// Range length
};

typedef	struct isa_ext_mem_fixed_desc_s	ISA_EXT_MEM_FIXED_DESC;
typedef	ISA_EXT_MEM_FIXED_DESC		*PISA_EXT_MEM_FIXED_DESC;

#define	SIZEOF_ISA_EXT_MEM_FIXED_DESC sizeof(ISA_EXT_MEM_FIXED_DESC)

#define	ISA_MEM_BLOCK_SIZE		256
#define	ISA_MEM_SHIFT_SIZE		8

#endif	// NOISADEFS
