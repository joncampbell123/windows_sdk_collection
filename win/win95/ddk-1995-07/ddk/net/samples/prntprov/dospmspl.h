/***************************************************************************\
*
* Module Name: DOSPMSPL.H
*
* DOS Print Spooler constants, types and function declarations
*
* Copyright (c) 1990-1995 Microsoft Corporation
*
* =============================================================================
*
* The following symbols are used in this file for conditional sections:
*
* INCL_SPLERRORS                - defined if INCL_ERRORS defined
*
\***************************************************************************/

#if !defined( DOSPMSPL_INCLUDED )

#ifndef RC_INVOKED
#pragma pack(1)         /* Assume byte packing throughout */
#endif

#define DOSPMSPL_INCLUDED

/* if error definitions are required then allow Spooler errors */
#ifdef INCL_ERRORS
    #define INCL_SPLERRORS
#endif /* INCL_ERRORS */
 
/*
 * Common types and constants
 * ==========================
 */

/* Names of various OS2.INI spooler variables */

#define SPLENTRY pascal far

#ifdef INCL_SPLERRORS
#include <pmerr.h>
#endif /* INCL_SPLERRORS */


/* LAN Manager APIs */

/* length for character arrays in structures (excluding zero terminator) */
#define DRIV_DEVICENAME_SIZE  31            /* see DRIVDATA structure    */
#define DRIV_NAME_SIZE         8            /* name of device driver     */
#define PRINTERNAME_SIZE      32            /* max printer name length   */
#define FORMNAME_SIZE         31            /* max form name length      */

#define DEFAULT_LM_PROC       "LMPRINT"


typedef unsigned SPLERR;    /* err */

typedef struct _PRJINFO {   /* prj1 */
    unsigned short       uJobId;
    char                 szUserName[UNLEN+1];
    char                 pad_1;
    char                 szNotifyName[CNLEN+1];
    char                 szDataType[DTLEN+1];
    unsigned char far *  pszParms;
    unsigned short       uPosition;
    unsigned short       fsStatus;
    unsigned char far *  pszStatus;
    unsigned long        ulSubmitted;
    unsigned long        ulSize;
    unsigned char far *  pszComment;
} PRJINFO;
typedef PRJINFO far *PPRJINFO;
typedef PRJINFO near *NPPRJINFO;

typedef struct _PRJINFO2 {   /* prj2 */
    unsigned short       uJobId;
    unsigned short       uPriority;
    unsigned char far *  pszUserName;
    unsigned short       uPosition;
    unsigned short       fsStatus;
    unsigned long        ulSubmitted;
    unsigned long        ulSize;
    unsigned char far *  pszComment;
    unsigned char far *  pszDocument;
} PRJINFO2;
typedef PRJINFO2 far *PPRJINFO2;
typedef PRJINFO2 near *NPPRJINFO2;

typedef struct _PRJINFO3 {   /* prj */
    unsigned short       uJobId;
    unsigned short       uPriority;
    unsigned char far *  pszUserName;
    unsigned short       uPosition;
    unsigned short       fsStatus;
    unsigned long        ulSubmitted;
    unsigned long        ulSize;
    unsigned char far *  pszComment;
    unsigned char far *  pszDocument;
    unsigned char far *  pszNotifyName;
    unsigned char far *  pszDataType;
    unsigned char far *  pszParms;
    unsigned char far *  pszStatus;
    unsigned char far *  pszQueue;
    unsigned char far *  pszQProcName;
    unsigned char far *  pszQProcParms;
    unsigned char far *  pszDriverName;
    PDRIVDATA            pDriverData;
    unsigned char far *  pszPrinterName;
} PRJINFO3;
typedef PRJINFO3 far *PPRJINFO3;
typedef PRJINFO3 near *NPPRJINFO3;


typedef struct _PRDINFO {    /* prd1 */
    char                 szName[PDLEN+1];
    char                 szUserName[UNLEN+1];
    unsigned short       uJobId;
    unsigned short       fsStatus;
    unsigned char far *  pszStatus;
    unsigned short       time;
} PRDINFO;
typedef PRDINFO far *PPRDINFO;
typedef PRDINFO near *NPPRDINFO;


typedef struct _PRDINFO3 {   /* prd */
    unsigned char far *  pszPrinterName;
    unsigned char far *  pszUserName;
    unsigned char far *  pszLogAddr;
    unsigned short       uJobId;
    unsigned short       fsStatus;
    unsigned char far *  pszStatus;
    unsigned char far *  pszComment;
    unsigned char far *  pszDrivers;
    unsigned short       time;
    unsigned short       pad1;
} PRDINFO3;
typedef PRDINFO3 far *PPRDINFO3;
typedef PRDINFO3 near *NPPRDINFO3;


typedef struct _PRQINFO {   /* prq1 */
    char                 szName[QNLEN+1];
    char                 pad_1;
    unsigned short       uPriority;
    unsigned short       uStartTime;
    unsigned short       uUntilTime;
    unsigned char far *  pszSepFile;
    unsigned char far *  pszPrProc;
    unsigned char far *  pszDestinations;
    unsigned char far *  pszParms;
    unsigned char far *  pszComment;
    unsigned short       fsStatus;
    unsigned short       cJobs;
} PRQINFO;
typedef PRQINFO far *PPRQINFO;
typedef PRQINFO near *NPPRQINFO;


typedef struct _PRQINFO3 {  /* prq */
    unsigned char far *  pszName;
    unsigned short       uPriority;
    unsigned short       uStartTime;
    unsigned short       uUntilTime;
    unsigned short       pad1;
    unsigned char far *  pszSepFile;
    unsigned char far *  pszPrProc;
    unsigned char far *  pszParms;
    unsigned char far *  pszComment;
    unsigned short       fsStatus;
    unsigned short       cJobs;
    unsigned char far *  pszPrinters;
    unsigned char far *  pszDriverName;
    PDRIVDATA            pDriverData;
} PRQINFO3;
typedef PRQINFO3 far *PPRQINFO3;
typedef PRQINFO3 near *NPPRQINFO3;


/*
 * structure for DosPrintJobGetId
 */
typedef struct _PRIDINFO {  /* prjid */
    unsigned short  uJobId;
    char            szServer[CNLEN + 1];
    char            szQName[QNLEN+1];
    char            pad_1;
} PRIDINFO;
typedef PRIDINFO far *PPRIDINFO;
typedef PRIDINFO near *NPPRIDINFO;


/****************************************************************
 *                                                              *
 *              Function prototypes                             *
 *                                                              *
 ****************************************************************/

SPLERR SPLENTRY DosPrintDestEnum(
            unsigned char far *   pszServer,
            unsigned short        uLevel,
            unsigned char far *   pbBuf,
            unsigned short        cbBuf,
            unsigned short far *  pcReturned,
            unsigned short far *  pcTotal
            );

SPLERR SPLENTRY DosPrintDestControl(
            unsigned char far *  pszServer,
            unsigned char far *  pszDevName,
            unsigned short       uControl
            );

SPLERR SPLENTRY DosPrintDestGetInfo(
            unsigned char far *   pszServer,
            unsigned char far *   pszName,
            unsigned short        uLevel,
            unsigned char far *   pbBuf,
            unsigned short        cbBuf,
            unsigned short far *  pcbNeeded
            );

SPLERR SPLENTRY DosPrintDestAdd(
            unsigned char far *  pszServer,
            unsigned short       uLevel,
            unsigned char far *  pbBuf,
            unsigned short       cbBuf
            );

SPLERR SPLENTRY DosPrintDestSetInfo(
            unsigned char far *  pszServer,
            unsigned char far *  pszName,
            unsigned short       uLevel,
            unsigned char far *  pbBuf,
            unsigned short       cbBuf,
            unsigned short       uParmNum
            );

SPLERR SPLENTRY DosPrintDestDel(
            unsigned char far *  pszServer,
            unsigned char far *  pszPrinterName
            );

SPLERR SPLENTRY DosPrintQEnum(
            unsigned char far *   pszServer,
            unsigned short        uLevel,
            unsigned char far *   pbBuf,
            unsigned short        cbBuf,
            unsigned short far *  pcReturned,
            unsigned short far *  pcTotal
            );

SPLERR SPLENTRY DosPrintQGetInfo(
            unsigned char far *   pszServer,
            unsigned char far *   pszQueueName,
            unsigned short        uLevel,
            unsigned char far *   pbBuf,
            unsigned short        cbBuf,
            unsigned short far *  pcbNeeded
            );

SPLERR SPLENTRY DosPrintQSetInfo(
            unsigned char far *  pszServer,
            unsigned char far *  pszQueueName,
            unsigned short       uLevel,
            unsigned char far *  pbBuf,
            unsigned short       cbBuf,
            unsigned short       uParmNum
            );

SPLERR SPLENTRY DosPrintQPause(
            unsigned char far *  pszServer,
            unsigned char far *  pszQueueName
            );

SPLERR SPLENTRY DosPrintQContinue(
            unsigned char far *  pszServer,
            unsigned char far *  pszQueueName
            );

SPLERR SPLENTRY DosPrintQPurge(
            unsigned char far *  pszServer,
            unsigned char far *  pszQueueName
            );

SPLERR SPLENTRY DosPrintQAdd(
            unsigned char far *  pszServer,
            unsigned short       uLevel,
            unsigned char far *  pbBuf,
            unsigned short       cbBuf
            );

SPLERR SPLENTRY DosPrintQDel(
            unsigned char far *  pszServer,
            unsigned char far *  pszQueueName
            );

SPLERR SPLENTRY DosPrintJobGetInfo(
            unsigned char far *   pszServer,
            unsigned short        uJobId,
            unsigned short        uLevel,
            unsigned char far *   pbBuf,
            unsigned short        cbBuf,
            unsigned short far *  pcbNeeded
            );

SPLERR SPLENTRY DosPrintJobSetInfo(
            unsigned char far *  pszServer,
            unsigned short       uJobId,
            unsigned short       uLevel,
            unsigned char far *  pbBuf,
            unsigned short       cbBuf,
            unsigned short       uParmNum
            );

SPLERR SPLENTRY DosPrintJobPause(
            unsigned char far *  pszServer,
            unsigned short       uJobId
            );

SPLERR SPLENTRY DosPrintJobContinue(
            unsigned char far *  pszServer,
            unsigned short       uJobId
            );

SPLERR SPLENTRY DosPrintJobDel(
            unsigned char far *  pszServer,
            unsigned short       uJobId
            );

SPLERR SPLENTRY DosPrintJobEnum(
            unsigned char far *   pszServer,
            unsigned char far *   pszQueueName,
            unsigned short        uLevel,
            unsigned char far *   pbBuf,
            unsigned short        cbBuf,
            unsigned short far *  pcReturned,
            unsigned short far *  pcTotal
            );

SPLERR SPLENTRY DosPrintJobGetId(
            unsigned short  hFile,
            PPRIDINFO       pInfo,
            unsigned short  cbInfo
            );


/*
 *      Values for parmnum in DosPrintQSetInfo.
 */

#define PRQ_PRIORITY_PARMNUM            2
#define PRQ_STARTTIME_PARMNUM           3
#define PRQ_UNTILTIME_PARMNUM           4
#define PRQ_SEPARATOR_PARMNUM           5
#define PRQ_PROCESSOR_PARMNUM           6
#define PRQ_DESTINATIONS_PARMNUM        7
#define PRQ_PARMS_PARMNUM               8
#define PRQ_COMMENT_PARMNUM             9
#define PRQ_PRINTERS_PARMNUM           12
#define PRQ_DRIVERNAME_PARMNUM         13
#define PRQ_DRIVERDATA_PARMNUM         14
#define PRQ_MAXPARMNUM                 14

/*
 *      Print Queue Priority
 */

#define PRQ_MAX_PRIORITY                1           /* highest priority */
#define PRQ_DEF_PRIORITY                5
#define PRQ_MIN_PRIORITY                9           /* lowest priority */
#define PRQ_NO_PRIORITY                 0

/*
 *      Print queue status bitmask and values.
 */

#define PRQ_STATUS_MASK                 3
#define PRQ_ACTIVE                      0
#define PRQ_PAUSED                      1
#define PRQ_ERROR                       2
#define PRQ_PENDING                     3

/*
 *      Print queue status bits for level 3
 */

#define PRQ3_PAUSED                   0x1
#define PRQ3_PENDING                  0x2
/*
 *      Values for parmnum in DosPrintJobSetInfo.
 */

#define PRJ_NOTIFYNAME_PARMNUM        3
#define PRJ_DATATYPE_PARMNUM          4
#define PRJ_PARMS_PARMNUM             5
#define PRJ_POSITION_PARMNUM          6
#define PRJ_COMMENT_PARMNUM          11
#define PRJ_DOCUMENT_PARMNUM         12
#define PRJ_PRIORITY_PARMNUM         14
#define PRJ_PROCPARMS_PARMNUM        16
#define PRJ_DRIVERDATA_PARMNUM       18
#define PRJ_MAXPARMNUM               18

/*
 *      Bitmap masks for status field of PRJINFO.
 */

/* 2-7 bits also used in device status */

#define PRJ_QSTATUS      0x0003      /* Bits 0,1  */
#define PRJ_DEVSTATUS    0x0ffc      /* 2-11 bits */
#define PRJ_COMPLETE     0x0004      /* Bit 2     */
#define PRJ_INTERV       0x0008      /* Bit 3     */
#define PRJ_ERROR        0x0010      /* Bit 4     */
#define PRJ_DESTOFFLINE  0x0020      /* Bit 5     */
#define PRJ_DESTPAUSED   0x0040      /* Bit 6     */
#define PRJ_NOTIFY       0x0080      /* Bit 7     */
#define PRJ_DESTNOPAPER  0x0100      /* Bit 8     */
#define PRJ_DESTFORMCHG  0x0200      /* BIT 9     */
#define PRJ_DESTCRTCHG   0x0400      /* BIT 10    */
#define PRJ_DESTPENCHG   0x0800      /* BIT 11    */
#define PRJ_DELETED      0x8000      /* Bit 15    */

/*
 *      Values of PRJ_QSTATUS bits in fsStatus field of PRJINFO.
 */

#define PRJ_QS_QUEUED         0
#define PRJ_QS_PAUSED         1
#define PRJ_QS_SPOOLING       2
#define PRJ_QS_PRINTING       3

/*
 *      Print Job Priority
 */

#define PRJ_MAX_PRIORITY      99          /* lowest priority */
#define PRJ_MIN_PRIORITY      1           /* highest priority */
#define PRJ_NO_PRIORITY       0


/*
 *      Bitmap masks for status field of PRDINFO.
 *      see PRJ_... for bits 2-11
 */

#define PRD_STATUS_MASK       0x0003      /* Bits 0,1 */
#define PRD_DEVSTATUS         0x0ffc      /* 2-11 bits */

/*
 *      Values of PRD_STATUS_MASK bits in fsStatus field of PRDINFO.
 */

#define PRD_ACTIVE            0
#define PRD_PAUSED            1

/*
 *      Control codes used in DosPrintDestControl.
 */

#define PRD_DELETE            0
#define PRD_PAUSE             1
#define PRD_CONT              2
#define PRD_RESTART           3

/*
 *      Values for parmnum in DosPrintDestSetInfo.
 */

#define PRD_LOGADDR_PARMNUM   3
#define PRD_COMMENT_PARMNUM   7
#define PRD_DRIVERS_PARMNUM   8

#ifndef RC_INVOKED
#pragma pack()  // restore default alignment
#endif

#endif /* DOSPMSPL_INCLUDED */

