/*
 * syncbase.h - Definitions shared by file synchronization engine API
 *              description (synceng.h) and reconciliation handler SPI
 *              description (rechndlr.h).
 *
 *   Copyright (c) 1993-1995, Microsoft Corp.  All rights reserved.
 */


#ifndef __SYNCBASE_H__
#define __SYNCBASE_H__


#ifdef __cplusplus
extern "C" {                     /* Assume C declarations for C++. */
#endif   /* __cplusplus */


/* Constants
 ************/

/* RECRESULT return codes */

#define RR_SUCCESS               (0)
#define RR_FAILURE               (-1)
#define RR_SRC_OPEN_FAILED       (-2)
#define RR_SRC_READ_FAILED       (-3)
#define RR_DEST_OPEN_FAILED      (-4)
#define RR_DEST_WRITE_FAILED     (-5)
#define RR_MERGE_CONFLICT        (-6)
#define RR_ABORT                 (-7)
#define RR_UNAVAILABLE_VOLUME    (-8)
#define RR_OUT_OF_MEMORY         (-9)
#define RR_FILE_CHANGED          (-10)

/* FILESTAMP conditions */

#define FS_COND_UNAVAILABLE      (0)
#define FS_COND_DOES_NOT_EXIST   (1)
#define FS_COND_EXISTS           (2)

/* RECSTATUSPROC messages */

#define RS_BEGIN_COPY            (0)
#define RS_DELTA_COPY            (1)
#define RS_END_COPY              (2)
#define RS_BEGIN_MERGE           (3)
#define RS_DELTA_MERGE           (4)
#define RS_END_MERGE             (5)


/* Macros
 *********/

/*
 * For a type "FOO", define the standard derived types LPFOO, CFOO, and LPCFOO.
 */

#define DECLARE_STANDARD_TYPES(type)      typedef type FAR *LP##type; \
                                          typedef const type C##type; \
                                          typedef const type FAR *LPC##type;


/* Types
 ********/

/* not included in windows.h */

DECLARE_STANDARD_TYPES(UINT);
DECLARE_STANDARD_TYPES(ULONG);

/* handles */

DECLARE_HANDLE(HVOLUMEID);
DECLARE_STANDARD_TYPES(HVOLUMEID);

/* file stamp */

typedef struct _filestamp
{
   UINT uCondition;
   DWORD dwLowDateTimeMod;
   DWORD dwHighDateTimeMod;
   DWORD dwcbLowLength;
   DWORD dwcbHighLength;
}
FILESTAMP;
DECLARE_STANDARD_TYPES(FILESTAMP);

/* reconciliation status callback */

typedef BOOL (CALLBACK *RECSTATUSPROC)(UINT, LPARAM, LPARAM);


/* Prototypes
 *************/

/* volume ID interface */

BOOL WINAPI IsPathOnVolume(LPCSTR, HVOLUMEID);

/* file stamp interface */

BOOL WINAPI GetFileStamp(HFILE, LPFILESTAMP);
int WINAPI CompareFileStamps(LPCFILESTAMP, LPCFILESTAMP);


#ifdef __cplusplus
}                                /* End of extern "C" {. */
#endif   /* __cplusplus */


#endif   /* ! __SYNCBASE_H__ */

