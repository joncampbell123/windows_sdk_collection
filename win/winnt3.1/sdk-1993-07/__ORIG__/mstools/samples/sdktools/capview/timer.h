/*
 *
 */

class TIMER : public DISPOBJ
{
    int		NumCalls;
    TIMETYPE	TotalTime;
    TIMETYPE	TimeThisFunction;
    CString     strDllName;
    CString     strFunctName;
    CString	strDisplay;
    CRect	rtDisplay;

    int	        perCent[4];
    int         iBrush[4];

public:
   TIMER(char * pchDll, char * pchFunction, int cCalls, TIMETYPE timeTotal,
          TIMETYPE timeThisFunc);
    TIMER * Parent()
    {
        return (TIMER *) DISPOBJ::Parent();
    }

    TIMER * Child()
    {
        return (TIMER *) DISPOBJ::Child();
    }
    
    TIMER * Sib()
    {
        return (TIMER *) DISPOBJ::Sib();
    }

    TIMETYPE& timeTotal() { return TotalTime; };

    void AddTime(TIMETYPE timeAdd) { TimeThisFunction += timeAdd; TotalTime += timeAdd; return; };
    
    void DoForeground(CDC * cdc, BOOL fZoom, RANGESTRUCT * prs);
    int  Height(BOOL fZoom) { return fZoom ? 13 : (rtDisplay.Height() + 8);}
    int  Width(BOOL fZoom) { return fZoom ? 28 : (rtDisplay.Width() + 8);}

    void DoSetPercents(TIMETYPE t);
    void DoSetColors(RANGESTRUCT * prs);
    void DoSetString(void);
    void DoAssignSize(CDC * pcdc);
};

class TIMERROOT
{
    TIMER *     pRoot;
    int         level;
    TIMER *     pTimerCur;

public:
    TIMERROOT() { pTimerCur = pRoot = NULL; level = 0; };
    
   void AddTiming(int iDepth, char * szModule, char * szName, int cCalls, TIMETYPE totalTime, TIMETYPE functionTime);

    void PaintTree(CDC * cdc, BOOL fZoom, RANGESTRUCT * prs, RECT rt)
    {
        pRoot->PaintTree(cdc, fZoom, prs, rt);
        return;
    };

    void SetProperties(int flags, BOOL fZoom = FALSE, RANGESTRUCT * prs = 0, CDC * pcdc = 0);

    BOOL DblClick( CPoint pt) { return pRoot->DblClick(pt); };

    void ExpandAll(void) { pRoot->ExpandAll(); return; };

    CRect& RectTotal(void) { return pRoot->rtTotal; };
};
