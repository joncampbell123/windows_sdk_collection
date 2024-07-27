#include "motion.h"

extern char szNRSeg2Msg[];
extern char szAppName[];

int FAR PASCAL MotionNRSeg2( hWnd )
HANDLE hWnd;
{
    MessageBox( hWnd, (LPSTR)szNRSeg2Msg, (LPSTR)szAppName, MB_OK );
}
