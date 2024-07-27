#include "windows.h"

/* The following three variables are initialized once when the application
** is first loaded and then copied into each succeeding instance, using the
** GetInstanceData procedure */


long FAR PASCAL MotionWndProc(HWND, unsigned, WORD, LONG);
int  FAR PASCAL MotionInit( HANDLE );
int  FAR PASCAL MotionNRSeg1( HANDLE );
int  FAR PASCAL MotionNRSeg2( HANDLE );

#define IDMNRCALL1   0x1001
#define IDMNRCALL2   0x1002
#define IDMLEXPAND   0x1003
#define IDMGCOMPACT  0x1004
#define IDMABOUT     0x1005
#define IDSLEXPAND   0x1006
#define IDSAPPNAME   0x1007
#define IDSABOUT     0x1008
#define IDSSEG1MSG   0x1009
#define IDSSEG2MSG   0x100A
#define ABOUTBOX     1
