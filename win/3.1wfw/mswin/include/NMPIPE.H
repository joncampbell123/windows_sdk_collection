/***************************************************************************\
*
* Module Name: NMPIPE.H
*
* Named Pipe Include File
*
* Copyright (c) 1990, Microsoft Corporation.  All rights reserved.
* 
\***************************************************************************/


/*** DosNamedPipes API Support */

#if !defined(INCL_DOSNMPIPES_INCLUDED)
#define INCL_DOSNMPIPES_INCLUDED

typedef void (far pascal *API_CALLBACK)(unsigned long);
typedef unsigned short HPIPE;      /* hp */
typedef HPIPE far *PHPIPE;

#define PIPE_FUNCTION unsigned short pascal far

/*** Data structures and equates used with named pipes ***/

typedef struct _PIPEINFO { /* nmpinf */
        unsigned short  cbOut;
        unsigned short  cbIn;
        unsigned char   cbMaxInst;
        unsigned char   cbCurInst;
        unsigned char   cbName;
        char            szName[1];
} PIPEINFO;
typedef PIPEINFO far *PPIPEINFO;

/* The following structure for backward compatability to nmpipe.h */

struct  npi_data1 {     /* PipeInfo data block (returned, level 1) */
        unsigned short  npi_obuflen;    /* length of outgoing I/O buffer */
        unsigned short  npi_ibuflen;    /* length of incoming I/O buffer */
        unsigned char   npi_maxicnt;    /* maximum number of instances   */
        unsigned char   npi_curicnt;    /* current number of instances   */
        unsigned char   npi_namlen;     /* length of pipe name           */
        char            npi_name[1];    /* start of name                 */
};      /* npi_data1 */

/* defined bits in pipe mode */
#define NP_NBLK                 0x8000 /* non-blocking read/write */
#define NP_SERVER               0x4000 /* set if server end       */
#define NP_WMESG                0x0400 /* write messages          */
#define NP_RMESG                0x0100 /* read as messages        */
#define NP_ICOUNT               0x00FF /* instance count field    */


/*      Named pipes may be in one of several states depending on the actions
 *      that have been taken on it by the server end and client end.  The
 *      following state/action table summarizes the valid state transitions:
 *
 *       Current state      Action                      Next state
 *
 *       <none>             server DosMakeNmPipe        DISCONNECTED
 *       DISCONNECTED       server connect              LISTENING
 *       LISTENING          client open                 CONNECTED
 *       CONNECTED          server disconn              DISCONNECTED
 *       CONNECTED          client close                CLOSING
 *       CLOSING            server disconn              DISCONNECTED
 *       CONNECTED          server close                CLOSING
 *       <any other>        server close                <pipe deallocated>
 *
 *      If a server disconnects his end of the pipe, the client end will enter a
 *      special state in which any future operations (except close) on the file
 *      descriptor associated with the pipe will return an error.
 */

/*
 *      Values for named pipe state
 */

#define NP_DISCONNECTED         1 /* after pipe creation or Disconnect */
#define NP_LISTENING            2 /* after DosNmPipeConnect            */
#define NP_CONNECTED            3 /* after Client open                 */
#define NP_CLOSING              4 /* after Client or Server close      */

/* DosMakeNmPipe open modes */

#define NP_ACCESS_INBOUND       0x0000
#define NP_ACCESS_OUTBOUND      0x0001
#define NP_ACCESS_DUPLEX        0x0002
#define NP_INHERIT              0x0000
#define NP_NOINHERIT            0x0080
#define NP_WRITEBEHIND          0x0000
#define NP_NOWRITEBEHIND        0x4000

/* DosMakeNmPipe and DosQNmPHandState state */

#define NP_READMODE_BYTE        0x0000
#define NP_READMODE_MESSAGE     0x0100
#define NP_TYPE_BYTE            0x0000
#define NP_TYPE_MESSAGE         0x0400
#define NP_END_CLIENT           0x0000
#define NP_END_SERVER           0x4000
#define NP_WAIT                 0x0000
#define NP_NOWAIT               0x8000
#define NP_UNLIMITED_INSTANCES  0x00FF

/* DosWaitNmPipe, DosCallNmPipe, DosMakeNmPipe timeout values */

#define NP_INDEFINITE_WAIT	(-1L)
#define NP_DEFAULT_WAIT		0L

typedef struct _AVAILDATA  {            /* PeekNMPipe Bytes Available record */
        unsigned short  cbpipe;         /* bytes left in the pipe            */
        unsigned short  cbmessage;      /* bytes left in current message     */
} AVAILDATA;
typedef AVAILDATA far *PAVAILDATA;

PIPE_FUNCTION  DosCallNmPipe(unsigned char far *   pszName,
                             unsigned char far *   pInBuf,
                             unsigned short        cbInBuf,
                             unsigned char far *   pbOutBuf,
                             unsigned short        cbOutBuf,
                             unsigned short far *  pcbRead,
                             unsigned long         ulTimeOut);

PIPE_FUNCTION  DosConnectNmPipe(HPIPE hp);

PIPE_FUNCTION  DosDisConnectNmPipe(HPIPE hp);

PIPE_FUNCTION  DosMakeNmPipe(unsigned char far *  pszName,
                             PHPIPE               php,
                             unsigned short       fsOpenMode,
                             unsigned short       fsPipeMode,
                             unsigned short       cbOutBuf,
                             unsigned short       cbInBuf,
                             unsigned long        ulTimeOut);

PIPE_FUNCTION  DosPeekNmPipe(HPIPE                 hp,
                             unsigned char far *   pBuf,
                             unsigned short        cbBuf,
                             unsigned short far *  pcbRead,
                             PAVAILDATA            pAvail,
                             unsigned short far *  pfsState);

PIPE_FUNCTION  DosQNmPHandState(HPIPE                 hp,
                                unsigned short far *  pfsState);

PIPE_FUNCTION  DosQNmPipeInfo(HPIPE                hp,
                              unsigned short       usInfoLevel,
                              unsigned char far *  pBuf,
                              unsigned short       cb);

PIPE_FUNCTION  DosSetNmPHandState(HPIPE           hp,
                                  unsigned short  fsState);

PIPE_FUNCTION  DosTransactNmPipe(HPIPE                 hp,
                                 unsigned char far *   bOutBuf,
                                 unsigned short        cbOut,
                                 unsigned char far *   pInBuf,
                                 unsigned short        cbIn,
                                 unsigned short far *  pcbRead);

PIPE_FUNCTION  DosWaitNmPipe(unsigned char far *  pszName,
                             unsigned long        ulTimeOut);


PIPE_FUNCTION
  DosReadAsyncNmPipe(unsigned short, API_CALLBACK, unsigned short far *,
                                unsigned char far *, unsigned short,
                                unsigned short far *);

PIPE_FUNCTION
  DosWriteAsyncNmPipe(unsigned short, API_CALLBACK, unsigned short far *,
                                unsigned char far *, unsigned short,
                                unsigned short far *);

PIPE_FUNCTION
  DosReadAsyncNmPipe2(unsigned long, unsigned short, API_CALLBACK,
                                unsigned short far *, unsigned char far *,
                                unsigned short, unsigned short far *);

PIPE_FUNCTION
  DosWriteAsyncNmPipe2(unsigned long, unsigned short, API_CALLBACK,
                                unsigned short far *, unsigned char far *,
                                unsigned short, unsigned short far *);

#endif /* INCL_DOSNMPIPES */
