#include	<afxwin.h>
#include	<afxdlgs.h>
#include  	<afxcoll.h>
#include	<string.h>

#include "types.h"
#include "ranges.h"
#include "dispobj.h"


void DISPOBJ::PaintTree(CDC * cdc, BOOL fZoom, RANGESTRUCT * prs, RECT rtPaint)
{
    DISPOBJ * p = pChild;
    CRect       rt;
    CRect	rt1;
    BOOL	fLines;
    BOOL	fChildren;
    
    
    if (fOpen && (p != NULL)) {
        fChildren = rt.IntersectRect( &rtChildren, &rtPaint);
	rt1.UnionRect( &rtChildren, &rtArea);
	fLines = rt.IntersectRect(&rt1, &rtPaint);
	while ( p != NULL) {
	    if (!p->fPrune) {
		if (fLines) {
		    cdc->MoveTo( ptRight );
		    cdc->LineTo( p->ptLeft );
		}
		if (fChildren) {
		    p->PaintTree(cdc, fZoom, prs, rtPaint);
		}
	    }
	    p = p->pSib;
        }
    }

    if (rt.IntersectRect(&rtArea, &rtPaint)) {
        DoForeground(cdc, fZoom, prs);
    }

    return;
}


void DISPOBJ::DoSetLocation(BOOL fZoom, BOOL fPrep)
{
    CRect       rt;

    if (fPrep) {
        rtChildren.SetRectEmpty();
        rtArea.SetRectEmpty();
        rtTotal.SetRectEmpty();
        if (fPrune) {
            return;
        }

        if (iLevel >= rgLoc.GetUpperBound()) {
            AAA aaa = {0, 0, 0};
            rgLoc.SetAtGrow(iLevel, aaa);
        }
            
        if (rgLoc[iLevel].width < Width(fZoom)) {
            rgLoc[iLevel].width = Width(fZoom);
        }

        return;
    } else {
        if (fPrune || ((pParent != NULL) && !pParent->fOpen)) {
            return;
        }
        
        rtArea.top = rgLoc[iLevel].y + spaceVertical;
        rtArea.left = rgLoc[iLevel].left;
        rtArea.right = rgLoc[iLevel].left + rgLoc[iLevel].width;
        rtArea.bottom = rtArea.top + Height(fZoom);

        rgLoc[iLevel].y = rtArea.bottom;

        ptRight.x = rtArea.right;
        ptLeft.x = rtArea.left;
        ptLeft.y = ptRight.y = (rtArea.bottom - rtArea.top) / 2 + rtArea.top;

        rtTotal = rtArea;

        if (fOpen && pChild) {
            DISPOBJ *   p = pChild;
            CRect       rt1;

            while (p != NULL) {
                rt1 = p->rtTotal;
                if (!rt1.IsRectEmpty()) {
                    if (rtChildren.IsRectEmpty()) {
                        rtChildren = rt1;
                    } else {
                        rtChildren |= rt1;
                    }
                }
                p = p->pSib;
            }

            if (!rtChildren.IsRectEmpty()) {
                rtTotal |= rtChildren;
            }
        }
    }

    return;
}                               /* DISPOBJ::DoSetLocation() */

void DISPOBJ::SetPropOnOne(int flags, RANGESTRUCT * prs, CDC * pcdc,
                BOOL fZoom, TIMETYPE progTime)
{
    if (flags & SET_PERCENT) {
        DoSetPercents(progTime);
    }

    if (flags & SET_COLORS) {
        DoSetColors(prs);
    }

    if (flags & SET_STRING) {
        DoSetString();
    }

    if (flags & SET_SIZE) {
        DoAssignSize(pcdc);
    }

    if (flags & SET_LOCATIONPREP) {
        DoSetLocation(fZoom, TRUE);
    }

    if (flags & SET_LOCATION) {
        DoSetLocation(fZoom, FALSE);
    }

    return;
}


void DISPOBJ::SetProperties(int flags, RANGESTRUCT * prs, CDC * pcdc,
                BOOL fZoom, TIMETYPE progTime)
{
    if (pChild != NULL) {
        pChild->SetProperties(flags, prs, pcdc, fZoom, progTime);
    }

    SetPropOnOne(flags, prs, pcdc, fZoom, progTime);

    if (pSib != NULL) {
        pSib->SetProperties(flags, prs, pcdc, fZoom, progTime);
    }

    return;
}                               /* DISPOBJ::SetProperties() */
