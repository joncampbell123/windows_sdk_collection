
/*
 *      Windows/Network Printer Interface Functions
 *      Copyright (C) Microsoft 1989-1993
 *
 *      Standard WINNET Driver Header File, spec version 3.10
 */


#ifndef _INC_WFWPRT
#define _INC_WFWPRT	/* #defined if windows.h has been included */

#ifndef RC_INVOKED
#pragma pack(1)         /* Assume byte packing throughout */
#endif  /* RC_INVOKED */

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif	/* __cplusplus */


/* length for character arrays in structures (excluding zero terminator) */
#define CNLEN		15		    /* Computer name length	 */
#define UNLEN		20		    /* Maximum user name length	 */
#define QNLEN		12		    /* Queue name maximum length */
#define PDLEN		 8		    /* Print destination length	 */
#define DTLEN		 9		    /* Spool file data type	 */
		                        /* e.g. PM_Q_STD,PM_Q_RAW   */

typedef unsigned SPLERR;    	/* err */
#define SPLENTRY WINAPI


typedef struct _PRJINFO {   /* prj1 */
    USHORT  uJobId;
    CHAR    szUserName[UNLEN+1];
    CHAR    pad_1;
    CHAR    szNotifyName[CNLEN+1];
    CHAR    szDataType[DTLEN+1];
    LPSTR   pszParms;
    USHORT  uPosition;
    USHORT  fsStatus;
    LPSTR	pszStatus;
    ULONG   ulSubmitted;
    ULONG   ulSize;
    LPSTR	pszComment;
} PRJINFO;
typedef PRJINFO FAR *PPRJINFO;
typedef PRJINFO NEAR *NPPRJINFO;

typedef struct _PRJINFO2 {   /* prj2 */
    USHORT  uJobId;
    USHORT  uPriority;
    LPSTR	pszUserName;
    USHORT  uPosition;
    USHORT  fsStatus;
    ULONG   ulSubmitted;
    ULONG   ulSize;
    LPSTR	pszComment;
    LPSTR	pszDocument;
} PRJINFO2;
typedef PRJINFO2 FAR *PPRJINFO2;
typedef PRJINFO2 NEAR *NPPRJINFO2;

typedef struct _PRJINFO3 {   /* prj */
    USHORT  uJobId;
    USHORT  uPriority;
    LPSTR	pszUserName;
    USHORT  uPosition;
    USHORT  fsStatus;
    ULONG   ulSubmitted;
    ULONG   ulSize;
    LPSTR	pszComment;
    LPSTR	pszDocument;
    LPSTR	pszNotifyName;
    LPSTR	pszDataType;
    LPSTR	pszParms;
    LPSTR	pszStatus;
    LPSTR	pszQueue;
    LPSTR	pszQProcName;
    LPSTR	pszQProcParms;
    LPSTR	pszDriverName;
    PDRIVDATA pDriverData;
    LPSTR	pszPrinterName;
} PRJINFO3;
typedef PRJINFO3 FAR *PPRJINFO3;
typedef PRJINFO3 NEAR *NPPRJINFO3;


typedef struct _PRDINFO {    /* prd1 */
    CHAR    szName[PDLEN+1];
    CHAR    szUserName[UNLEN+1];
    USHORT  uJobId;
    USHORT  fsStatus;
    LPSTR	pszStatus;
    USHORT  time;
} PRDINFO;
typedef PRDINFO FAR *PPRDINFO;
typedef PRDINFO NEAR *NPPRDINFO;


typedef struct _PRDINFO3 {   /* prd */
    LPSTR	    pszPrinterName;
    LPSTR	    pszUserName;
    LPSTR	    pszLogAddr;
    USHORT  uJobId;
    USHORT  fsStatus;
    LPSTR	    pszStatus;
    LPSTR	    pszComment;
    LPSTR	    pszDrivers;
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
    LPSTR	    pszSepFile;
    LPSTR	    pszPrProc;
    LPSTR	    pszDestinations;
    LPSTR	    pszParms;
    LPSTR	    pszComment;
    USHORT  fsStatus;
    USHORT  cJobs;
} PRQINFO;
typedef PRQINFO FAR *PPRQINFO;
typedef PRQINFO NEAR *NPPRQINFO;


typedef struct _PRQINFO3 {  /* prq */
    LPSTR	    pszName;
    USHORT  uPriority;
    USHORT  uStartTime;
    USHORT  uUntilTime;
    USHORT  pad1;
    LPSTR	    pszSepFile;
    LPSTR	    pszPrProc;
    LPSTR	    pszParms;
    LPSTR	    pszComment;
    USHORT  fsStatus;
    USHORT  cJobs;
    LPSTR	    pszPrinters;
    LPSTR	    pszDriverName;
    PDRIVDATA pDriverData;
} PRQINFO3;
typedef PRQINFO3 FAR *PPRQINFO3;
typedef PRQINFO3 NEAR *NPPRQINFO3;


/****************************************************************
 *																*
 *		Function prototypes										*
 *																*
 ****************************************************************/


SPLERR SPLENTRY DosPrintQEnum(
	    LPSTR	    pszServer,
	    USHORT  uLevel,
	    LPBYTE   pbBuf,
	    USHORT  cbBuf,
	    PUSHORT pcReturned,
	    PUSHORT pcTotal
	    );

SPLERR SPLENTRY DosPrintQGetInfo(
	    LPSTR	    pszServer,
	    LPSTR	    pszQueueName,
	    USHORT  uLevel,
	    LPBYTE   pbBuf,
	    USHORT  cbBuf,
	    PUSHORT pcbNeeded
	    );

SPLERR SPLENTRY DosPrintQSetInfo(
	    LPSTR	    pszServer,
	    LPSTR	    pszQueueName,
	    USHORT  uLevel,
	    LPBYTE   pbBuf,
	    USHORT  cbBuf,
	    USHORT  uParmNum
	    );


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


#ifndef RC_INVOKED
#pragma pack()          /* Revert to default packing */
#endif  /* RC_INVOKED */

#ifdef __cplusplus
}                       /* End of extern "C" { */
#endif	/* __cplusplus */

#endif  /* _INC_WFWPRT */


