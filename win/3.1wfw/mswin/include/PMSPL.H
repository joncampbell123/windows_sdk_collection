/*****************************************************************/
/**		     Microsoft LAN Manager			**/ 
/**	       Copyright(c) Microsoft Corp., 1988-1991		**/
/*****************************************************************/ 
/***************************************************************************\
*
* Module Name: PMSPL.H
*
* OS/2 Presentation Manager Spooler constants, types and function declarations
*
* Copyright (c) International Business Machines Corporation 1981, 1988, 1989
*
* =============================================================================
*
* The following symbols are used in this file for conditional sections.
*
* INCL_SPLFSE			- File System Emulation Functions
* INCL_SPLERRORS		- defined if INCL_ERRORS defined
* INCL_SPLDOSPRINT		- DosPrint APIs
*
\***************************************************************************/

/********************************************************************/
/*								    */
/* Change history:						    */
/*								    */
/* Flag Reason	 Rlse Date   Origin  Comments			    */
/* ---- -------- ---- ------ ------- ------------------------------ */
/* @C1= D24366.7 R200 890612 HDKCS  : 32-bit definitions for 2.00   */
/*	D25084	 R200 891031 MARTINP: LAN Manager spooler support   */
/*	7748	 R200 891201 MARTINP: remove 32-bit defs	    */
/********************************************************************/

#ifdef INCL_ERRORS
    #define INCL_SPLERRORS
#endif /* INCL_ERRORS */

/* common types and constants
** ==========================
*/
/* Names of various OS2.INI spooler variables */

/* XLATOFF */
#define SPLENTRY PASCAL FAR _loadds

#ifndef FARENTRY
    #define FARENTRY PASCAL FAR
#endif
/* XLATON */


#define SPL_INI_SPOOLER		"PM_SPOOLER"
#define SPL_INI_QUEUE		"PM_SPOOLER_QUEUE"
#define SPL_INI_PRINTER		"PM_SPOOLER_PRINTER"
#define SPL_INI_PRINTERDESCR	"PM_SPOOLER_PRINTER_DESCR"
#define SPL_INI_QUEUEDESCR	"PM_SPOOLER_QUEUE_DESCR"
#define SPL_INI_QUEUEDD		"PM_SPOOLER_QUEUE_DD"
#define SPL_INI_QUEUEDDDATA	"PM_SPOOLER_QUEUE_DDDATA"

/* General SPL return values */
#define SPL_ERROR     0L
#define SPL_OK	      1L

/* handle to a spool file */
typedef LHANDLE HSPL;		    /* hspl */
/* Used in recording of PM_Q_STD data via SplStdxxx calls */
typedef LHANDLE HSTD;		    /* hstd */
typedef HSTD FAR *PHSTD;
/* spooler manager open data */
typedef PSZ FAR *PQMOPENDATA;	    /* pqmdop */

/* Public interface function prototypes
** ====================================
*/

/*** Spooler Queue manager Interface */
HSPL   SPLENTRY _loadds SplQmOpen( PSZ, LONG, PQMOPENDATA );
BOOL   SPLENTRY _loadds SplQmStartDoc( HSPL, PSZ );
BOOL   SPLENTRY _loadds SplQmWrite( HSPL, LONG, PBYTE );
BOOL   SPLENTRY _loadds SplQmEndDoc( HSPL );
BOOL   SPLENTRY _loadds SplQmClose( HSPL );
BOOL   SPLENTRY _loadds SplQmAbort( HSPL );
BOOL   SPLENTRY _loadds SplQmAbortDoc( HSPL );

/*** Spooler Queue Processor interface */

BOOL  SPLENTRY _loadds SplQpQueryDt( PLONG, PSZ FAR * );
BOOL  SPLENTRY _loadds SplQpInstall( HWND );

#ifdef INCL_SPLFSE
/*** Direct Device File System Interface */

USHORT SPLENTRY _loadds PrtOpen( PSZ, PUSHORT, PUSHORT, ULONG,
				 USHORT, USHORT, USHORT, ULONG );
USHORT SPLENTRY _loadds PrtClose( HFILE );
USHORT SPLENTRY _loadds PrtWrite( HFILE, PCH, USHORT, PUSHORT );
USHORT SPLENTRY _loadds PrtDevIOCtl( PVOID, PVOID, USHORT, USHORT, HFILE );
#endif /* include File System Emulation functions */

/* Style for SplMessageBox */
/* Same as for WinMsgBox see PMWIN.H for details */

/*** Spooler message interface */
USHORT	SPLENTRY _loadds SplMessageBox( PSZ, USHORT, USHORT,
					PSZ, PSZ, USHORT, USHORT );

/*** PM_Q_STD datatype functions */
BOOL SPLENTRY _loadds SplStdOpen( HDC );
BOOL SPLENTRY _loadds SplStdClose( HDC );
BOOL SPLENTRY _loadds SplStdStart( HDC );
HSTD SPLENTRY _loadds SplStdStop( HDC );
BOOL SPLENTRY _loadds SplStdDelete( HSTD );
BOOL SPLENTRY _loadds SplStdGetBits( HSTD, LONG, LONG, PCH );
LONG SPLENTRY _loadds SplStdQueryLength( HSTD );

/* Error information and return codes
** ==================================
*/
/* Error information for SplMessageBox */
#define SPLINFO_QPERROR	      0x0001
#define SPLINFO_DDERROR	      0x0002
#define SPLINFO_SPLERROR      0x0004
#define SPLINFO_OTHERERROR    0x0080
#define SPLINFO_INFORMATION   0x0100
#define SPLINFO_WARNING	      0x0200
#define SPLINFO_ERROR	      0x0400
#define SPLINFO_SEVERE	      0x0800
#define SPLINFO_USERINTREQD   0x1000

/* Error Data for SplMessageBox */
#define SPLDATA_PRINTERJAM    0x0001
#define SPLDATA_FORMCHGREQD   0x0002
#define SPLDATA_CARTCHGREQD   0x0004
#define SPLDATA_PENCHGREQD    0x0008
#define SPLDATA_DATAERROR     0x0010
#define SPLDATA_UNEXPECTERROR 0x0020
#define SPLDATA_OTHER	      0x8000

/* return code for SplStdQueryLength */
#define SSQL_ERROR (-1L)

#ifdef INCL_SPLERRORS

#include <pmerr.h>

#endif /* INCL_SPLERRORS */



typedef unsigned SPLERR;    /* err */

#ifdef INCL_SPLDOSPRINT


/* length for character arrays in structures (excluding zero terminator) */
#define CNLEN		15		    /* Computer name length	 */
#define UNLEN		20		    /* Maximum user name length	 */
#define QNLEN		12		    /* Queue name maximum length */
#define PDLEN		 8		    /* Print destination length	 */
#define DTLEN		 9		    /* Spool file data type	 */
					    /* e.g. PM_Q_STD,PM_Q_RAW	 */
#define QP_DATATYPE_SIZE 15		    /* returned by SplQpQueryDt	 */
#define DRIV_DEVICENAME_SIZE 31		    /* see DRIVDATA struc	 */
#define DRIV_NAME_SIZE 8		    /* name of device driver	 */
#define PRINTERNAME_SIZE 32		    /* max printer name length	 */
#define FORMNAME_SIZE 31		    /* max form name length	 */
#define MAXCOMMENTSZ	48		    /* queue comment length	 */

#define DEFAULT_LM_PROC "LMPRINT"

/* IOctl for DosPrintJobGetId */
#define SPOOL_LMCAT			83
#define SPOOL_LMGetPrintId		0x60



typedef struct _PRJINFO {   /* prj1 */
    USHORT  uJobId;
    CHAR    szUserName[UNLEN+1];
    CHAR    pad_1;
    CHAR    szNotifyName[CNLEN+1];
    CHAR    szDataType[DTLEN+1];
    PSZ	    pszParms;
    USHORT  uPosition;
    USHORT  fsStatus;
    PSZ	    pszStatus;
    ULONG   ulSubmitted;
    ULONG   ulSize;
    PSZ	    pszComment;
} PRJINFO;
typedef PRJINFO FAR *PPRJINFO;
typedef PRJINFO NEAR *NPPRJINFO;

typedef struct _PRJINFO2 {   /* prj2 */
    USHORT  uJobId;
    USHORT  uPriority;
    PSZ	    pszUserName;
    USHORT  uPosition;
    USHORT  fsStatus;
    ULONG   ulSubmitted;
    ULONG   ulSize;
    PSZ	    pszComment;
    PSZ	    pszDocument;
} PRJINFO2;
typedef PRJINFO2 FAR *PPRJINFO2;
typedef PRJINFO2 NEAR *NPPRJINFO2;

typedef struct _PRJINFO3 {   /* prj */
    USHORT  uJobId;
    USHORT  uPriority;
    PSZ	    pszUserName;
    USHORT  uPosition;
    USHORT  fsStatus;
    ULONG   ulSubmitted;
    ULONG   ulSize;
    PSZ	    pszComment;
    PSZ	    pszDocument;
    PSZ	    pszNotifyName;
    PSZ	    pszDataType;
    PSZ	    pszParms;
    PSZ	    pszStatus;
    PSZ	    pszQueue;
    PSZ	    pszQProcName;
    PSZ	    pszQProcParms;
    PSZ	    pszDriverName;
    PDRIVDATA pDriverData;
    PSZ	    pszPrinterName;
} PRJINFO3;
typedef PRJINFO3 FAR *PPRJINFO3;
typedef PRJINFO3 NEAR *NPPRJINFO3;


typedef struct _PRDINFO {    /* prd1 */
    CHAR    szName[PDLEN+1];
    CHAR    szUserName[UNLEN+1];
    USHORT  uJobId;
    USHORT  fsStatus;
    PSZ	    pszStatus;
    USHORT  time;
} PRDINFO;
typedef PRDINFO FAR *PPRDINFO;
typedef PRDINFO NEAR *NPPRDINFO;


typedef struct _PRDINFO3 {   /* prd */
    PSZ	    pszPrinterName;
    PSZ	    pszUserName;
    PSZ	    pszLogAddr;
    USHORT  uJobId;
    USHORT  fsStatus;
    PSZ	    pszStatus;
    PSZ	    pszComment;
    PSZ	    pszDrivers;
    USHORT  time;
    USHORT  usTimeOut;
} PRDINFO3;
typedef PRDINFO3 FAR *PPRDINFO3;
typedef PRDINFO3 NEAR *NPPRDINFO3;


typedef struct _PRQINFO {   /* prq1 */
    CHAR    szName[QNLEN+1];
    CHAR    pad_1;
    USHORT  uPriority;
    USHORT  uStartTime;
    USHORT  uUntilTime;
    PSZ	    pszSepFile;
    PSZ	    pszPrProc;
    PSZ	    pszDestinations;
    PSZ	    pszParms;
    PSZ	    pszComment;
    USHORT  fsStatus;
    USHORT  cJobs;
} PRQINFO;
typedef PRQINFO FAR *PPRQINFO;
typedef PRQINFO NEAR *NPPRQINFO;


typedef struct _PRQINFO3 {  /* prq */
    PSZ	    pszName;
    USHORT  uPriority;
    USHORT  uStartTime;
    USHORT  uUntilTime;
    USHORT  pad1;
    PSZ	    pszSepFile;
    PSZ	    pszPrProc;
    PSZ	    pszParms;
    PSZ	    pszComment;
    USHORT  fsStatus;
    USHORT  cJobs;
    PSZ	    pszPrinters;
    PSZ	    pszDriverName;
    PDRIVDATA pDriverData;
} PRQINFO3;
typedef PRQINFO3 FAR *PPRQINFO3;
typedef PRQINFO3 NEAR *NPPRQINFO3;


/*
 * structure for DosPrintJobGetId
 */
typedef struct _PRIDINFO {  /* prjid */
    USHORT  uJobId;
    CHAR    szServer[CNLEN + 1];
    CHAR    szQName[QNLEN+1];
    CHAR    pad_1;
} PRIDINFO;
typedef PRIDINFO FAR *PPRIDINFO;
typedef PRIDINFO NEAR *NPPRIDINFO;

/* Boca DCR 1400 support */

/*
 * structure for DosPrintDriverEnum
 */
typedef struct _PRDRIVINFO {
    CHAR    szDrivName[DRIV_NAME_SIZE+1+DRIV_DEVICENAME_SIZE+1];
} PRDRIVINFO;
typedef PRDRIVINFO FAR *PPRDRIVINFO;
typedef PRDRIVINFO NEAR *NPPRDRIVINFO;

/*
 * structure for DosPrintQProcessorEnum
 */
typedef struct _PRQPROCINFO {
    CHAR    szQProcName[QNLEN+1];
} PRQPROCINFO;
typedef PRQPROCINFO FAR *PPRQPROCINFO;
typedef PRQPROCINFO NEAR *NPPRQPROCINFO;

/*
 * structure for DosPrintPortEnum
 */
typedef struct _PRPORTINFO {
    CHAR    szPortName[PDLEN+1];
} PRPORTINFO;
typedef PRPORTINFO FAR *PPRPORTINFO;
typedef PRPORTINFO NEAR *NPPRPORTINFO;


/****************************************************************
 *								*
 *		Function prototypes				*
 *								*
 ****************************************************************/

SPLERR SPLENTRY DosPrintDestEnum(
	    PSZ	    pszServer,
	    USHORT  uLevel,
	    PBYTE   pbBuf,
	    USHORT  cbBuf,
	    PUSHORT pcReturned,
	    PUSHORT pcTotal
	    );

SPLERR SPLENTRY DosPrintDestControl(
	    PSZ	    pszServer,
	    PSZ	    pszDevName,
	    USHORT  uControl
	    );

SPLERR SPLENTRY DosPrintDestGetInfo(
	    PSZ	    pszServer,
	    PSZ	    pszName,
	    USHORT  uLevel,
	    PBYTE   pbBuf,
	    USHORT  cbBuf,
	    PUSHORT pcbNeeded
	    );

SPLERR SPLENTRY DosPrintDestAdd(
	    PSZ	    pszServer,
	    USHORT  uLevel,
	    PBYTE   pbBuf,
	    USHORT  cbBuf
	    );

SPLERR SPLENTRY DosPrintDestSetInfo(
	    PSZ	    pszServer,
	    PSZ	    pszName,
	    USHORT  uLevel,
	    PBYTE   pbBuf,
	    USHORT  cbBuf,
	    USHORT  uParmNum
	    );

SPLERR SPLENTRY DosPrintDestDel(
	    PSZ	    pszServer,
	    PSZ	    pszPrinterName
	    );

SPLERR SPLENTRY DosPrintQEnum(
	    PSZ	    pszServer,
	    USHORT  uLevel,
	    PBYTE   pbBuf,
	    USHORT  cbBuf,
	    PUSHORT pcReturned,
	    PUSHORT pcTotal
	    );

SPLERR SPLENTRY DosPrintQGetInfo(
	    PSZ	    pszServer,
	    PSZ	    pszQueueName,
	    USHORT  uLevel,
	    PBYTE   pbBuf,
	    USHORT  cbBuf,
	    PUSHORT pcbNeeded
	    );

SPLERR SPLENTRY DosPrintQSetInfo(
	    PSZ	    pszServer,
	    PSZ	    pszQueueName,
	    USHORT  uLevel,
	    PBYTE   pbBuf,
	    USHORT  cbBuf,
	    USHORT  uParmNum
	    );

SPLERR SPLENTRY DosPrintQPause(
	    PSZ	    pszServer,
	    PSZ	    pszQueueName
	    );

SPLERR SPLENTRY DosPrintQContinue(
	    PSZ	    pszServer,
	    PSZ	    pszQueueName
	    );

SPLERR SPLENTRY DosPrintQPurge(
	    PSZ	    pszServer,
	    PSZ	    pszQueueName
	    );

SPLERR SPLENTRY DosPrintQAdd(
	    PSZ	    pszServer,
	    USHORT  uLevel,
	    PBYTE   pbBuf,
	    USHORT  cbBuf
	    );

SPLERR SPLENTRY DosPrintQDel(
	    PSZ	    pszServer,
	    PSZ	    pszQueueName
	    );

SPLERR SPLENTRY DosPrintJobGetInfo(
	    PSZ	    pszServer,
	    USHORT  uJobId,
	    USHORT  uLevel,
	    PBYTE   pbBuf,
	    USHORT  cbBuf,
	    PUSHORT pcbNeeded
	    );

SPLERR SPLENTRY DosPrintJobSetInfo(
	    PSZ	    pszServer,
	    USHORT  uJobId,
	    USHORT  uLevel,
	    PBYTE   pbBuf,
	    USHORT  cbBuf,
	    USHORT  uParmNum
	    );

SPLERR SPLENTRY DosPrintJobPause(
	    PSZ	    pszServer,
	    USHORT  uJobId
	    );

SPLERR SPLENTRY DosPrintJobContinue(
	    PSZ	    pszServer,
	    USHORT  uJobId
	    );

SPLERR SPLENTRY DosPrintJobDel(
	    PSZ	    pszServer,
	    USHORT  uJobId
	    );

SPLERR SPLENTRY DosPrintJobEnum(
	    PSZ	    pszServer,
	    PSZ	    pszQueueName,
	    USHORT  uLevel,
	    PBYTE   pbBuf,
	    USHORT  cbBuf,
	    PUSHORT pcReturned,
	    PUSHORT pcTotal
	    );

SPLERR SPLENTRY DosPrintJobGetId(
	    HFILE	hFile,
	    PPRIDINFO	pInfo,
	    USHORT	cbInfo
	    );

/* Boca DCR 1400 support */

/**INTERNAL_ONLY**/

SPLERR EXPENTRY DosPrintDriverEnum(
       PSZ      pszHostName,
       USHORT   sLevel,
       PBYTE    pbBuf,
       USHORT   usBuflen,
       PUSHORT  pusRead,
       PUSHORT  pusTotal
       );

SPLERR EXPENTRY DosPrintQProcessorEnum(
       PSZ      pszHostName,
       USHORT   sLevel,
       PBYTE    pbBuf,
       USHORT   usBuflen,
       PUSHORT  pusRead,
       PUSHORT  pusTotal
       );

SPLERR EXPENTRY DosPrintPortEnum(
       PSZ      pszHostName,
       USHORT   sLevel,
       PBYTE    pbBuf,
       USHORT   usBuflen,
       PUSHORT  pusRead,
       PUSHORT  pusTotal
       );

/**END_INTERNAL**/

/*
 *	Values for parmnum in DosPrintQSetInfo.
 */

#define PRQ_PRIORITY_PARMNUM		2
#define PRQ_STARTTIME_PARMNUM		3
#define PRQ_UNTILTIME_PARMNUM		4
#define PRQ_SEPARATOR_PARMNUM		5
#define PRQ_PROCESSOR_PARMNUM		6
#define PRQ_DESTINATIONS_PARMNUM	7
#define PRQ_PARMS_PARMNUM		8
#define PRQ_COMMENT_PARMNUM		9
#define PRQ_PRINTERS_PARMNUM	       12
#define PRQ_DRIVERNAME_PARMNUM	       13
#define PRQ_DRIVERDATA_PARMNUM	       14
#define PRQ_MAXPARMNUM		       14

/*
 *	Print Queue Priority
 */

#define PRQ_MAX_PRIORITY		1	    /* highest priority */
#define PRQ_DEF_PRIORITY		5
#define PRQ_MIN_PRIORITY		9	    /* lowest priority */
#define PRQ_NO_PRIORITY			0

/*
 *	Print queue status bitmask and values.
 */

#define PRQ_STATUS_MASK			3
#define PRQ_ACTIVE			0
#define PRQ_PAUSED			1
#define PRQ_ERROR			2
#define PRQ_PENDING			3

/*
 *	Print queue status bits for level 3
 */

#define PRQ3_PAUSED		      0x1
#define PRQ3_PENDING		      0x2

/*
 *	Values for parmnum in DosPrintJobSetInfo.
 */

#define PRJ_NOTIFYNAME_PARMNUM	      3
#define PRJ_DATATYPE_PARMNUM	      4
#define PRJ_PARMS_PARMNUM	      5
#define PRJ_POSITION_PARMNUM	      6
#define PRJ_COMMENT_PARMNUM	     11
#define PRJ_DOCUMENT_PARMNUM	     12
#define PRJ_PRIORITY_PARMNUM	     14
#define PRJ_PROCPARMS_PARMNUM	     16
#define PRJ_DRIVERDATA_PARMNUM	     18
#define PRJ_MAXPARMNUM		     18

/*
 *	Bitmap masks for status field of PRJINFO.
 */

/* 2-7 bits also used in device status */

#define PRJ_QSTATUS	 0x0003	     /* Bits 0,1 */
#define PRJ_DEVSTATUS	 0x0ffc	     /* 2-11 bits */
#define PRJ_COMPLETE	 0x0004	     /*	 Bit 2	 */
#define PRJ_INTERV	 0x0008	     /*	 Bit 3	 */
#define PRJ_ERROR	 0x0010	     /*	 Bit 4	 */
#define PRJ_DESTOFFLINE	 0x0020	     /*	 Bit 5	 */
#define PRJ_DESTPAUSED	 0x0040	     /*	 Bit 6	 */
#define PRJ_NOTIFY	 0x0080	     /*	 Bit 7	 */
#define PRJ_DESTNOPAPER	 0x0100	     /*	 Bit 8	 */
#define PRJ_DESTFORMCHG	 0x0200	     /* BIT 9 */
#define PRJ_DESTCRTCHG	 0x0400	     /* BIT 10 */
#define PRJ_DESTPENCHG	 0x0800	     /* BIT 11 */
#define PRJ_DELETED	 0x8000	     /* Bit 15	 */

/*
 *	Values of PRJ_QSTATUS bits in fsStatus field of PRJINFO.
 */

#define PRJ_QS_QUEUED		      0
#define PRJ_QS_PAUSED		      1
#define PRJ_QS_SPOOLING		      2
#define PRJ_QS_PRINTING		      3

/*
 *	Print Job Priority
 */

#define PRJ_MAX_PRIORITY		99	    /* lowest priority */
#define PRJ_MIN_PRIORITY		 1	    /* highest priority */
#define PRJ_NO_PRIORITY			 0


/*
 *	Bitmap masks for status field of PRDINFO.
 *	see PRJ_... for bits 2-11
 */

#define PRD_STATUS_MASK	      0x0003	  /* Bits 0,1 */
#define PRD_DEVSTATUS	      0x0ffc	  /* 2-11 bits */

/*
 *	Values of PRD_STATUS_MASK bits in fsStatus field of PRDINFO.
 */

#define PRD_ACTIVE		   0
#define PRD_PAUSED		   1

/*
 *	Control codes used in DosPrintDestControl.
 */

#define PRD_DELETE		      0
#define PRD_PAUSE		      1
#define PRD_CONT		      2
#define PRD_RESTART		      3

/*
 *	Values for parmnum in DosPrintDestSetInfo.
 */

#define PRD_LOGADDR_PARMNUM	 3
#define PRD_COMMENT_PARMNUM	 7
#define PRD_DRIVERS_PARMNUM	 8
#define PRD_TIMEOUT_PARMNUM	 10

#endif /* INCL_SPLDOSPRINT */
