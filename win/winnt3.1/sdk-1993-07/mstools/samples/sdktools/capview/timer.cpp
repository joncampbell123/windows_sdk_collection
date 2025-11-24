#include	<afxwin.h>
#include	<afxdlgs.h>
#include  	<afxcoll.h>
#include	<string.h>

#include "types.h"
#include "ranges.h"
#include "dispobj.h"
#include "timer.h"

extern "C" char * UnDName(char *);


TIMER::TIMER(char * pchDll, char * pchFunction, int cCalls, TIMETYPE timeTotal,
          TIMETYPE timeThisFunc) :
      strDllName(pchDll), strFunctName(pchFunction), rtDisplay(0, 0, 10, 10)
    {
        NumCalls = cCalls;
        TotalTime = timeTotal;
        TimeThisFunction = timeThisFunc;

        /*
         * Correct these values on an open
         */

        strDisplay = "";
        
    }

    void TIMER::DoForeground(CDC * cdc, BOOL fZoom, RANGESTRUCT * prs)
    {
        CRect   rtTmp = rtDisplay;
	int	i;
	int	j;

	if (fXYZ && IsOpen()) {
	    i = iBrush[1 + fTotal*2];
	    j = iBrush[0 + fTotal*2];
	} else {
	    i = iBrush[0 + fTotal*2];
	    j = iBrush[0 + fTotal*2];
	}

        CBrush cBrushI( prs->BackColor[i] );
        CBrush cBrushJ( prs->BackColor[j] );


        DoBackground(cdc, &cBrushI, &cBrushJ);

	if (!fZoom) {
	    rtTmp.OffsetRect(rtArea.TopLeft());

	    cdc->SetTextColor(prs->TextColor[i] * RGB(255, 255, 255));

	    cdc->DrawText(strDisplay, -1, &rtTmp, DT_NOCLIP|DT_EXPANDTABS);
            
	}
        return;
    }

void TIMER::DoSetPercents(TIMETYPE progTime)
{
    TIMER * p = Parent();
    TIMETYPE    parentTotal;

    if (p == NULL) {
        parentTotal = TotalTime;
    } else {
        parentTotal = p->TotalTime;
    }

    perCent[0] = (int) TotalTime.PerCent(parentTotal);
    perCent[1] = (int) TimeThisFunction.PerCent(TotalTime);
    perCent[2] = (int) TotalTime.PerCent(progTime);
    perCent[3] = (int) TimeThisFunction.PerCent(progTime);
    return;
}                               /* TIMER::DoSetPercents() */

void TIMER::DoSetColors(RANGESTRUCT * prs)
{
    int         i;
    int         j;

    for (i=0; i<4; i++) {
        for (j=0; j<prs->cRanges-1; j++) {
            if ((prs->Above[j] <= perCent[i]) &&
                (perCent[i] < prs->Above[j+1])) {
                break;
            }
        }
        iBrush[i] = j;
    }

    if ((iBrush[2] == 0) && prs->fPrune) {
        fPrune = TRUE;
    } else {
        fPrune = FALSE;
    }

    return;
}                               /* TIMER::DoSetColors() */


void TIMER::DoSetString()
{
    char        rgch[256];

    strDisplay = strDllName + "\n" + strFunctName;
    sprintf(rgch, "\n%d\n%3d%%\t%3d%%\t", NumCalls, perCent[1], perCent[3]);
    strDisplay += rgch;
    strDisplay += TimeThisFunction.format(rgch);
    sprintf(rgch, "\n%3d%%\t%3d%%\t", perCent[0], perCent[2]);
    strDisplay += rgch;
    strDisplay += TotalTime.format(rgch);

    return;
}                               /* TIMER::DoSetString() */
      
    

void TIMER::DoAssignSize(CDC * pcdc)
{
    rtDisplay.SetRect(4, 4, 10, 10);
    pcdc->DrawText(strDisplay, -1, &rtDisplay, DT_CALCRECT|DT_EXPANDTABS);
    return;
}                               /* TIMER::DoAssignSize() */


void TIMERROOT::AddTiming(int iDepth, char * szModule, char * szName, int cCalls, TIMETYPE timeTotal, TIMETYPE timeFunction)
{
    TIMER * pTimer = new TIMER( szModule, szName, cCalls, timeTotal, timeFunction);

    if (pRoot == NULL) {
        pTimerCur = pRoot = new TIMER( "APP", "***", 0, 0, 0);
        level = -1;
    }

    pRoot->AddTime(timeFunction);

    if (iDepth == level) {
        pTimerCur->AddSibling(pTimer);
    } else if (iDepth == level + 1) {
        pTimerCur->AddChild( pTimer );
        level = iDepth;
    } else  {
        while (level != iDepth) {
            pTimerCur = pTimerCur->Parent();
            level -= 1;
        }
        pTimerCur->AddSibling(pTimer);
    }
    pTimerCur = pTimer;
    return;
}                               /* TIMERROOT::AddTiming() */

void TIMERROOT::SetProperties(int flags, BOOL fZoom, RANGESTRUCT * prs, CDC * pcdc)
{
    BOOL        fLoc = flags & SET_LOCATION;
    int         i;

    if (fLoc) {
        flags |= SET_LOCATIONPREP;
        flags &= ~SET_LOCATION;
        rgLoc.RemoveAll();
    }
    pRoot->SetProperties(flags, prs, pcdc, fZoom, pRoot->timeTotal());

    if (fLoc) {
        rgLoc[0].left = spaceHorizontal;
        rgLoc[0].y = 0;
        for (i=1; i<rgLoc.GetSize(); i++) {
            rgLoc[i].left = rgLoc[i-1].left + rgLoc[i-1].width + spaceHorizontal;
            rgLoc[i].y = 0;
        }
    
        pRoot->SetProperties(SET_LOCATION, prs, pcdc, fZoom);
    }

    return;
}

