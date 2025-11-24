/*
 * rechndlr.h - Reconciliation handler SPI description.
 *
 *   Copyright (c) 1993-1995, Microsoft Corp.  All rights reserved.
 */


#include <syncbase.h>


#ifndef __RECHNDLR_H__
#define __RECHNDLR_H__


#ifdef __cplusplus
extern "C" {                     /* Assume C declarations for C++. */
#endif   /* __cplusplus */


/* Constants
 ************/

/* RECHANDLERPROC messages */

#define RH_BEGIN_REC             (1)
#define RH_END_REC               (2)
#define RH_COPY                  (3)
#define RH_MERGE                 (4)


/* Types
 ********/

/* return codes */

typedef LRESULT RECRESULT;
DECLARE_STANDARD_TYPES(RECRESULT);

typedef LRESULT RECSTATUS;
DECLARE_STANDARD_TYPES(RECSTATUS);

/* reconciliation handler entry point */

typedef RECRESULT (CALLBACK *RECHANDLERPROC)(UINT, LPARAM);

/* reconciliation file */

typedef struct _recfile
{
   LPCSTR lpcszPath;
   HVOLUMEID hvid;
   FILESTAMP fs;
   DWORD dwReserved;
}
RECFILE;
DECLARE_STANDARD_TYPES(RECFILE);

/* reconciliation file node */

typedef struct _recfilenode
{
   struct _recfilenode FAR *lprfnNext;
   RECFILE rf;
}
RECFILENODE;
DECLARE_STANDARD_TYPES(RECFILENODE);

/* copy description (RH_COPY) */

typedef struct _copydesc
{
   RECSTATUSPROC rsp;
   LPARAM lpCallbackData;
   RECFILE rfSrc;
   ULONG ulcRecFileNodes;
   LPRECFILENODE lprfnFirst;
}
COPYDESC;
DECLARE_STANDARD_TYPES(COPYDESC);

/* merge description (RH_MERGE) */

typedef struct _mergedesc
{
   RECSTATUSPROC rsp;
   LPARAM lpCallbackData;
   RECFILE rfDest;
   ULONG ulcRecFileNodes;
   LPCRECFILENODE lprfnFirst;
}
MERGEDESC;
DECLARE_STANDARD_TYPES(MERGEDESC);


/* Prototypes
 *************/

/* default reconciliation handler interface */

RECRESULT WINAPI DefRecHandler(UINT, LPARAM);


#ifdef __cplusplus
}                                /* End of extern "C" {. */
#endif   /* __cplusplus */


#endif   /* ! __RECHNDLR_H__ */

