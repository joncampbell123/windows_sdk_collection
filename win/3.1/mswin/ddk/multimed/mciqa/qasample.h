/****************************************************************************
 *
 *   qasample.h
 *
 *   Copyright (c) 1991-1992 Microsoft Corporation.  All Rights Reserved.
 *
 *
 *   Sample MCI Device Driver
 *
 *      Testbed interface
 *
 ***************************************************************************/

/* filled in by a call to qsInfo() */
typedef struct {
    DWORD       dwLength;       /* length of media */
    DWORD       dwPosition;     /* current position */
    int         nMode;          /* current mode */
} QS_INFO;
typedef QS_INFO FAR *LPQS_INFO;
    
/* returned from qsCurrentMode and from qsInfo */
#define QS_PLAYING  1
#define QS_STOPPED  2
#define QS_PAUSED   3

/* APIs used to control the testbed device */
extern BOOL  qsOpen(int nDevice);
extern BOOL  qsClose(int nDevice);
extern BOOL  qsStop(int nDevice);
extern BOOL  qsPlay(int nDevice, DWORD dwFrom, DWORD dwTo);
extern BOOL  qsSeek(int nDevice, DWORD dwTo);
extern BOOL  qsInfo(int nDevice, LPQS_INFO lpInfo);
extern BOOL  qsPause(int nDevice);
extern int   qsCurrentMode(int nDevice);
extern DWORD qsCurrentPosition(int nDevice);
extern DWORD qsMediaLength(int nDevice);
