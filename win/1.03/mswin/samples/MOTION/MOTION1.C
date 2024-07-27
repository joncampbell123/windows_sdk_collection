#include "motion.h"

extern char szNRSeg1Msg[];
extern char szAppName[];

int FAR PASCAL MotionNRSeg1( hWnd )
HANDLE hWnd;
{
    MessageBox( hWnd, (LPSTR)szNRSeg1Msg, (LPSTR)szAppName, MB_OK );
}
