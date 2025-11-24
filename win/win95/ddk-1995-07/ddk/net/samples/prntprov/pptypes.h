/*****************************************************************/ 
/**               Microsoft Windows 95                          **/
/**           Copyright (C) Microsoft Corp., 1991-1995          **/
/*****************************************************************/ 

/* PPTYPES.H
 * 
 *  Core data structures for MS network print provider
 *
 */      
 
#ifndef __pptypes_h__
#define __pptypes_h__

//
// Build a recognizable 4 byte signature
//
#define MAKESIG( ch0, ch1, ch2, ch3 )\
        ( (DWORD)(BYTE)(ch0) | ( (DWORD)(BYTE)(ch1) << 8 ) |    \
        ( (DWORD)(BYTE)(ch2) << 16 ) | ( (DWORD)(BYTE)(ch3) << 24 ) )

//
// Network printer record.  This is what the handle returned by
// PPOpenPrinter points to. Signature is NPQ_SIGNATURE ("NPQ")
//
typedef struct NETPRINTERQUEUE_tag {   

  DWORD signature;

  ACCESS_MASK AccessGranted;
  HANDLE hFile;
  BOOL bConnected;
  BOOL bSupportsWin95;

  char szServerName[UNCLEN+1];
  char szQueueName[PATHLEN];

} NETPRINTERQUEUE;

typedef NETPRINTERQUEUE FAR *PNETPRINTERQUEUE;
typedef NETPRINTERQUEUE FAR *LPNETPRINTERQUEUE;

#define NPQ_SIGNATURE MAKESIG('N','P','Q',0)

#define PtrFromHandle(h) ((PNETPRINTERQUEUE) (char NEAR *) h)

#endif   // __pptypes_h__

