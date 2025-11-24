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

/***	SF.INC - System File Table */


/***	System File Table SuperStructure
 *
 *	The system file table entries are allocated in contiguous groups.
 *	There may be more than one such groups; the SF "superstructure"
 *	tracks the groups.
 */

struct SF {
    struct SF *SFLink;			/* pointer to next table */
    USHORT SFCount;			/* number of entries */
    USHORT SFTable;			/* beginning of array of sf_entry's */
};
typedef struct SF SF;


/***	System file table entry
 *
 *	These are the structures which are at SFTABLE in the SF structure.
 */

struct sf_entry {
    USHORT sf_ref_count;		/* number of processes sharing entry */
					/*   if FCB then ref count */
    USHORT sf_mode;			/* access mode or high bit on if FCB */
    UCHAR sf_attr;			/* attribute of file */
    USHORT sf_flags;			/*Bits 8-15
					 * Bit 15 = 1 if remote file
					 *	 = 0 if local file or device
					 * Bit 14 = 1 if date/time is not to be
					 *   set from clock at CLOSE.  Set by
					 *   FILETIMES and FCB_CLOSE.  Reset by
					 *   other reseters of the dirty bit
					 *   (WRITE)
					 * Bit 13 = Pipe bit (reserved)
					 *
					 * Bits 0-7 (old FCB_devid bits)
					 * If remote file or local file, bit
					 * 6=0 if dirty Device ID number, bits
					 * 0-5 if local file.
					 * bit 7=0 for local file, bit 7
					 *      =1 for local I/O device
					 * If local I/O device, bit 6=0 if EOF (input)
					 *		Bit 5=1 if Raw mode
					 *		Bit 0=1 if console input device
					 *		Bit 1=1 if console output device
					 *		Bit 2=1 if null device
					 *		Bit 3=1 if clock device
					 */
    void *sf_devptr;			/* Points to DPB if local file, points
					 * to device header if local device,
					 * points to net device header if
					 * remote
					 */
    USHORT sf_firclus;			/* First cluster of file (bit 15 = 0) */
    USHORT sf_time;			/* Time associated with file */
    USHORT sf_date;			/* Date associated with file */
    ULONG sf_size;			/* Size associated with file */
    ULONG sf_position;			/* Read/Write pointer or LRU count for FCBs */
/*
 * Starting here, the next 7 bytes may be used by the file system
 * to store an ID.
 */
    USHORT sf_cluspos;			/* Position of last cluster accessed */
    ULONG sf_dirsec;			/* Sector number of directory sector
					 * for this file
					 */
    UCHAR sf_dirpos;			/* Offset of this entry in the above */
/*
 * End of 7 bytes of file-system specific info.
 */
    UCHAR sf_name[11];			/* 11 character name that is in the
					 * directory entry.  This is used by
					 * close to detect file deleted and
					 * disk changed errors.
					 */

/*
 * SHARING INFO
 */
    ULONG sf_chain;			/* link to next SF */
    USHORT sf_UID;
    USHORT sf_PID;
    USHORT sf_MFT;
    USHORT sf_lstclus;			/*AN009; Last cluster accessed */
    void *sf_IFS_HDR;
};
typedef struct sf_entry sf_entry;

/* ASM
sf_fsda 	EQU	BYTE PTR sf_cluspos	     ;DOS 4.00
sf_serial_ID	EQU	WORD PTR sf_firclus	     ;DOS 4.00
sf_netid	EQU	BYTE PTR sf_cluspos
sf_OpenAge	EQU	WORD PTR sf_position+2
sf_LRU		EQU	WORD PTR sf_position
*/

#define	SF_DEFAULT_NUMBER	5

/*
 * Note that we need to mark an SFT as being busy for OPEN/CREATE.  This is
 * because an INT 24 may prevent us from 'freeing' it.  We mark this as such
 * by placing a -1 in the ref_count field.
 */

#define	SF_BUSY			(-1)


/*
 * mode mask for FCB detection
 */
#define	SF_ISFCB		0x8000

/*
 * mode bit for disableing last access date updating
 */
#define  SF_NOLASTACCESS 0x04
/*
 * Flag word masks
 */
#define	SF_ISNET		0x8000
#define	SF_CLOSE_NODATE 	0x4000
#define	SF_PIPE 		0x2000
#define	SF_NO_INHERIT		0x1000
#define	SF_NET_SPOOL		0x0800

//**RMFHFE** Handle_Fail_I24 	EQU	0000000100000000B  /*BIT 8 - DISK FULL I24 ERROR */

/*
 * Local file/device flag masks
 */
#define	DEVID_FILE_CLEAN	0x40	/* true if file and not written */
#define	DEVID_FILE_MASK_DRIVE	0x3F	/* mask for drive number */

#define	DEVID_DEVICE		0x80	/* true if a device */
#define	DEVID_DEVICE_EOF	0x40	/* true if end of file reached */
#define	DEVID_DEVICE_RAW	0x20	/* true if in raw mode */
#define	DEVID_DEVICE_SPECIAL	0x10	/* true if special device */
#define	DEVID_DEVICE_CLOCK	0x08	/* true if clock device */
#define	DEVID_DEVICE_NULL	0x04	/* true if null device */
#define	DEVID_DEVICE_CON_OUT	0x02	/* true if console output */
#define	DEVID_DEVICE_CON_IN	0x01	/* true if consle input */

/*
 * structure of devid field as returned by IOCTL is:
 *
 *	BIT	7   6	5   4	3   2	1   0
 *	      |---|---|---|---|---|---|---|---|
 *	      | I | E | R | S | I | I | I | I |
 *	      | S | O | A | P | S | S | S | S |
 *	      | D | F | W | E | C | N | C | C |
 *	      | E |   |   | C | L | U | O | I |
 *	      | V |   |   | L | K | L | T | N |
 *	      |---|---|---|---|---|---|---|---|
 *	ISDEV = 1 if this channel is a device
 *	      = 0 if this channel is a disk file
 *
 *	If ISDEV = 1
 *
 *	      EOF = 0 if End Of File on input
 *	      RAW = 1 if this device is in Raw mode
 *		  = 0 if this device is cooked
 *	      ISCLK = 1 if this device is the clock device
 *	      ISNUL = 1 if this device is the null device
 *	      ISCOT = 1 if this device is the console output
 *	      ISCIN = 1 if this device is the console input
 *
 *	If ISDEV = 0
 *	      EOF = 0 if channel has been written
 *	      Bits 0-5	are  the  block  device  number  for
 *		  the channel (0 = A, 1 = B, ...)
 */
#define	DEVID_ISDEV	0x80
#define	DEVID_EOF	0x40
#define	DEVID_RAW	0x20
#define	DEVID_SPECIAL	0x10
#define	DEVID_ISCLK	0x08
#define	DEVID_ISNUL	0x04
#define	DEVID_ISCOT	0x02
#define	DEVID_ISCIN	0x01

#define	DEVID_BLOCK_DEV 0x1F		/* mask for block device number */


/* XLATOFF */
/*
 *  For the benefit of the IFSMGR...
 */

#define	devid_device		DEVID_DEVICE
#define	devid_file_clean 	DEVID_FILE_CLEAN
#define	devid_file_mask_drive	DEVID_FILE_MASK_DRIVE
#define	sf_busy			SF_BUSY
#define	sf_isfcb		SF_ISFCB
#define	sf_isnet		SF_ISNET
#define	sf_net_spool		SF_NET_SPOOL
#define	sf_no_inherit		SF_NO_INHERIT
/* XLATON */
