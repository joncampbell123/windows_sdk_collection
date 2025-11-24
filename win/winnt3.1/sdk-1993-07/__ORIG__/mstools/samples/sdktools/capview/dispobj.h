/*
*
*/

#include "array_x.h"

#define spaceVertical 10
#define spaceHorizontal 40

extern  CBrush  CbrushBackground;
extern  CBrush  CbrushFrame;
extern  LOGFONT DefaultFont;

extern CAaaArray  rgLoc;
extern BOOL fXYZ;
extern BOOL fTotal;

enum {
    SET_COLORS=1,
    SET_PERCENT=2,
    SET_STRING=4,
    SET_SIZE=8,
    SET_LOCATION=16,
    SET_LOCATIONPREP=32
};

class DISPOBJ
{
    DISPOBJ *	pParent;
    BOOL 	fOpen;
    DISPOBJ *	pChild;
    DISPOBJ *	pSib;
    CRect       rtChildren;

  public:
    BOOL	fPrune;
    POINT	ptRight;
    POINT	ptLeft;
    CRect	rtArea;
    DISPOBJ *   pRoot;
    int		iLevel;
    CRect       rtTotal;
    
    DISPOBJ * Parent() { return pParent; }
    DISPOBJ * Child() { return pChild; }
    DISPOBJ * Sib() { return pSib; }
    BOOL        IsOpen() { return fOpen; }
    
    DISPOBJ()
    {
	pParent = pChild = pSib = NULL;
	fPrune = fOpen = FALSE;
	iLevel = 0;
	pRoot = this;
    }
    
    void AddSibling(DISPOBJ * pNew) {
	pNew->pParent = pParent;
	pNew->iLevel = iLevel;
	pNew->pRoot = pRoot;
	
	if (pSib == NULL) {
	    pSib = pNew;
	    return;
	}
	
	DISPOBJ * p = pSib;
	while (p->pSib != NULL) {
	    p = p->pSib;
	}
	
	p->pSib = pNew;
	
	return;
    }
    
    void AddChild(DISPOBJ * pNew)
    {
	pNew->pParent = this;
	pNew->iLevel = iLevel + 1;
	pNew->pRoot = pRoot;
	
	if (pChild == NULL) {
	    pChild = pNew;
	    return;
	}
	
	pChild->AddSibling(pNew);
	return;
    }
    
    virtual void DoForeground(CDC * cdc, BOOL fZoom, RANGESTRUCT * prs) = 0;
    
    void PaintTree(CDC * cdc, BOOL fZoom, RANGESTRUCT * prs, RECT rt);
    void DoBackground( CDC * cdc, CBrush * pBrushBack = &CbrushBackground,
                      CBrush * pBrushFrame = &CbrushFrame) {
	cdc->FillRect( &rtArea, pBrushBack );
	cdc->FrameRect( &rtArea, pBrushFrame );

        return;
    }
    
    
    void Close()
    {
	DISPOBJ * p;
	fOpen = FALSE;
	
	if (pChild && (pChild->fOpen)) {
	    p = pChild;
	    while (p != NULL) {
		p->Close();
		p = p->pSib;
	    }
	}
	return;
    }
    
    void ExpandAll()
    {
	if (pSib) {
	    pSib->ExpandAll();
	}

	if (pChild) {
	    pChild->ExpandAll();
	}
	
	Open();
    }
    
    void Open() {
	
	if (fOpen || (pChild == NULL)) {
	    return;
	}
	
	fOpen = TRUE;
    }
    
    virtual int Height(BOOL fZoom) = 0;
    virtual int Width(BOOL fZoom) = 0;
    
  private:
    void  SetPropOnOne(int flags, RANGESTRUCT * prs, CDC * pcdc, BOOL fZoom, TIMETYPE t);
    
public:

    void SetProperties(int flags, RANGESTRUCT * prs = 0, CDC * pcdc = 0, BOOL fZoom = FALSE, TIMETYPE progTime = 0);
    virtual void DoSetPercents(TIMETYPE t) = 0;
    virtual void DoSetColors(RANGESTRUCT * prs) = 0;
    virtual void DoSetString(void) = 0;
    virtual void DoAssignSize(CDC * pcdc) = 0;
    void DoSetLocation(BOOL fZoom, BOOL fPrep);
           
     
     BOOL DblClick( CPoint pt )
     {
	 if (rtArea.PtInRect( pt )) {
	     if (fOpen) {
		 Close();
	     } else {
		 Open();
	     }
	     return TRUE;
	 }
	 
	 if (fOpen && pChild && pChild->DblClick( pt ) ) {
	     return TRUE;
	 }
	 
	 if (pSib && pSib->DblClick( pt ) ) {
	     return TRUE;
	 }
	 
	 return FALSE;
     }
};
  
