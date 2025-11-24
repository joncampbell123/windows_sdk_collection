/*
 *  BLDDT.C
 *
 *  Support for building a display table from resources and data
 *  in memory.
 *
 *  Copyright 1993-1995 Microsoft Corporation. All Rights Reserved.
 */

#pragma warning(disable:4001)   /* single line comments */
#pragma warning(disable:4054)   /* cast function pointer to data pointer */
#pragma warning(disable:4100)   /* unreferenced formal parameter */
#pragma warning(disable:4127)   /* conditional expression is constant */
#pragma warning(disable:4201)   /* nameless struct/union */
#pragma warning(disable:4204)   /* non-constant aggregate initializer */
#pragma warning(disable:4209)   /* benign typedef redefinition */
#pragma warning(disable:4214)   /* bit field types other than int */
#pragma warning(disable:4505)   /* unreferenced local function removed */
#pragma warning(disable:4514)   /* unreferenced inline function removed */
#pragma warning(disable:4702)   /* unreachable code */
#pragma warning(disable:4704)   /* inline assembler turns off global optimizer */
#pragma warning(disable:4705)   /* statement has no effect */
#pragma warning(disable:4706)   /* assignment within conditional expression */
#pragma warning(disable:4710)   /* function not expanded */
#pragma warning(disable:4115)   /* named type def in parens */

#define _INC_OLE
#include <windows.h>
#pragma warning(disable:4001)   /* single line comments */
#include <windowsx.h>
#include <mapiwin.h>
#include <mapidbg.h>
#include <ole2.h>

#include <mapidefs.h>
#include <mapicode.h>
#include <mapitags.h>
#include <mapix.h>
#include <mapispi.h>
#include <mapiguid.h>
#include <stddef.h>
#include <mapiperf.h>
#include <mapiutil.h>

#ifdef DEBUG
#include <unkobj.h>     /* For LH_SetHeapName on ITABLE's heap. */
#endif

#ifdef MAC
#define LoadResource        AfxLoadResource
#endif

#include <_memcpy.h>

#pragma SEGMENT(DispTbl)

#ifndef WIN32
/*  Windows structures documented for Win16 but not defined in SDK */
/*  Snitched from \capone\inc\ourtypes.h 12/29/93 */

#pragma pack(1)

typedef struct {
    DWORD   style;
    BYTE    cdit;
    WORD    x;
    WORD    y;
    WORD    cx;
    WORD    cy;
} DLGTEMPLATE, FAR *LPDLGTEMPLATE;

typedef struct {
    WORD    x;
    WORD    y;
    WORD    cx;
    WORD    cy;
    WORD    id;
    DWORD   style;
} DLGITEMTEMPLATE, FAR *LPDLGITEMTEMPLATE;

#pragma pack()
#endif  /* !WIN32 */

/*  Additional stuff for handling resources */
#define cchMaxDlgString         256

#if defined(WIN16)
#define CbDlgStrlen(_s)     Cbtszsize(_s)
#elif defined (WIN32)
#if defined (CHICAGO)
__inline int
CbDlgStrlen (LPVOID _s)
{
    unsigned short *wp = _s;
    while (*wp++) ;
    return ((unsigned char *)wp - (unsigned char *)_s);
}
#else
#define CbDlgStrlen(_s)     ((lstrlenW((LPWSTR)(_s))+1) * sizeof(WCHAR))
#endif
#endif

/*  Display table column set */

#define cColsDT         9

SizedSPropTagArray(cColsDT, ptaColsDT) =
{
    cColsDT,
    {
        PR_ROWID,
        PR_XPOS,
        PR_DELTAX,
        PR_YPOS,
        PR_DELTAY,
        PR_CONTROL_TYPE,
        PR_CONTROL_STRUCTURE,
        PR_CONTROL_ID,
        PR_CONTROL_FLAGS,
    }
};

/*  Local functions */

CALLERRELEASE       FreeDT;
ULONG               CtltypeOfItem(LPDLGITEMTEMPLATE pitem);

/*
 -  BuildDisplayTable
 -
 *  Purpose:
 *      Creates a display table from a data structure consisting of
 *      1) Windows resources with localizable strings and screen
 *      positions; 2) mostly static data giving property interface
 *      mappings for the fields to be displayed.
 *
 *  Arguments:
 *      lpfAllocateBuffer   in      MAPI allocation family
 *      lpfAllocateMore     in      ditto
 *      lpfFreeBuffer       in      ditto
 *      hInstance           in      instance of module to retrieve
 *                                  the resources from
 *      cPages              in      number of property sheets
 *      lpPage              in      array of property sheet descriptions
 *      ulFlags             in      MAPI_UNICODE
 *      lppTable            out     the display table
 *
 *  Returns:
 *      HRESULT
 *
 *  Errors:
 *      out of memory, or invalid inputs
 */
HRESULT STDAPICALLTYPE BuildDisplayTable(
    LPALLOCATEBUFFER    lpfAllocateBuffer,
    LPALLOCATEMORE      lpfAllocateMore,
    LPFREEBUFFER        lpfFreeBuffer,
    LPMALLOC            lpMalloc,
    HINSTANCE           hInstance,
    UINT                cPages,
    LPDTPAGE            lpPage,
    ULONG               ulFlags,
    LPMAPITABLE *       lppTable,
    LPTABLEDATA *       lppTblData)
{
    HRESULT         hr;
    SCODE           sc;
    LPTABLEDATA     ptad = NULL;
    SSortOrderSet   so = { 0 };
    LPSPropValue    rgprop = NULL;
    LPSPropValue    pprop;
    UINT            iPage;
    UINT            ictl;
    LPDTCTL         pctl;
    ULONG           irow = 0L;
    SRow            row;
    UINT            cbText;
    UINT            cb;
    LPVOID          pv;
    LPDLGTEMPLATE   pdlg;
    TCHAR           szText[cchMaxDlgString+1];

#ifdef  PARAMETER_VALIDATION
    Assert(cPages);
    AssertSz( !lpfAllocateBuffer || !IsBadCodePtr( FARPROC( lpfAllocateBuffer ),
            "lpfAllocateBuffer fails address check" );
            
    AssertSz( !lpfAllocateMore || !IsBadCodePtr( FARPROC( lpfAllocateMore ),
            "lpfAllocateMore fails address check" );
    
    AssertSz( !lpfFreeBuffer || !IsBadCodePtr( FARPROC( lpfFreeBuffer ),
            "lpfFreeBuffer fails address check" );
    
#ifdef  DEBUG
    if (lpMalloc)
        DebugTrace("BuildDisplayTable: lpMalloc is unused, pass NULL\n");
#endif  
            
    if (IsBadReadPtr(lpPage, cPages*sizeof(DTPAGE)))
    {
        DebugTraceArg(BuildDisplayTable, "lpPage fails address check");
        return ResultFromScode(E_INVALIDARG);
    }
    /*  Detailed validation of individual structures happens further down */
    if (IsBadWritePtr(lppTable, sizeof(LPMAPITABLE)))
    {
        DebugTraceArg(BuildDisplayTable, "lppTable fails address check");
        return ResultFromScode(E_INVALIDARG);
    }

    /*  Detailed validation of individual structures happens further down */
    if (lpTblData && IsBadWritePtr(lppTblData, sizeof(LPTABLEDATA)))
    {
        DebugTraceArg(BuildDisplayTable, "lppTblData fails address check");
        return ResultFromScode(E_INVALIDARG);
    }

#endif  /* PARAMETER_VALIDATION */

    *lppTable = NULL;

    /*  Create the table data object */
    if (FAILED(sc = CreateTable((LPIID)&IID_IMAPITableData, lpfAllocateBuffer,
        lpfAllocateMore, lpfFreeBuffer, NULL, TBLTYPE_SNAPSHOT, PR_ROWID,
            (LPSPropTagArray)&ptaColsDT, &ptad)))
    {
        DebugTrace("BuildDisplayTable:  Can't create IMAPITableData");
        goto retSc;
    }
    #ifdef DEBUG
    LH_SetHeapName(((LPUNKOBJ)ptad)->lhHeap, "Display Table TAD Heap");
    #endif
    MAPISetBufferName(ptad, "Display Table TAD Object");

    /*  Loop over pages */
    for (iPage = 1; iPage <= cPages; ++iPage, ++lpPage)
    {
        HRSRC       hrsrc;
        HGLOBAL     hglobal = 0;
        LPBYTE      pb = NULL;
        LPDLGITEMTEMPLATE pitem;

#ifdef  PARAMETER_VALIDATION
        if (lpPage->cctl == 0 ||
            IsBadReadPtr(lpPage, sizeof(DTPAGE)) ||
            IsBadReadPtr(lpPage->lpctl, lpPage->cctl * sizeof(DTCTL)))
        {
            DebugTrace("BuildDisplayTable: page %d fails address check\n", iPage);
            goto badArg;
        }
#endif  /* PARAMETER_VALIDATION */

        /*  Load the resource for this page */
        if (!(hrsrc = FindResource(hInstance, lpPage->lpszResourceName, RT_DIALOG))
            || !(hglobal = LoadResource(hInstance, hrsrc))
            || !(pb = LockResource(hglobal)))
        {
            DebugTrace("BuildDisplayTable: failed to load resource on page %d\n", iPage);
            goto badArg;
        }

        /*  Validate the dialog template */
        pdlg = (LPDLGTEMPLATE)pb;
        Assert(lpPage->cctl == (UINT)pdlg->cdit || lpPage->cctl == (UINT)pdlg->cdit + 1);
        /* //$  more validation? */

        /*  Skip pb to the first dialog item template */
        pb += sizeof(DLGTEMPLATE);  /*  skip header */
        pb += CbDlgStrlen(pb);      /*  skip menu name */
        pb += CbDlgStrlen(pb);      /*  skip class name */
                                    /*  copy dialog caption to right charset */
#if !defined(UNICODE) && defined(WIN32) /* && defined(_WINNT_) */
        cbText = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)pb, -1,
            szText, sizeof(szText), NULL, NULL);
        if (!cbText)
        {
            DebugTrace("BuildDisplayTable: failed to convert Unicode (%ld)\n", GetLastError());
            sc = E_FAIL;
            goto retSc;
        }
#else
        cbText = CbDlgStrlen(pb);
        MemCopy(szText, pb, min(cbText, sizeof(szText)));
        szText[cchMaxDlgString] = 0;
#endif
        pb += CbDlgStrlen(pb);      /*  ...and skip dialog caption */
        if (pdlg->style & DS_SETFONT)
        {
            pb += sizeof(short int);    /*  skip point size */
            pb += CbDlgStrlen(pb);      /*  skip typeface name */
        }
#if defined(WIN32) /* && defined(_WINNT_) */
        if ((ULONG)pb % 4)          /*  Skip to 4-byte boundary */
            pb += 2;
        Assert(((ULONG)pb % 4) == 0);
#endif

        /*  Properties for the New Page control, which must come first */
        /*  if it is there at all, are obtained from the dialog. */
        if (lpPage->cctl && lpPage->lpctl[0].ulCtlType == DTCT_PAGE)
        {
            SPropValue  rgpropT[9];
            BYTE        rgb[100];
            TCHAR       szComponent[cchMaxDlgString+1];
            LPTSTR      lpszComponent = szComponent;
            ULONG       cbComponent;

            /*
             *  If not already passed in, get the string associated with
             *  the szComponent from resources
             */

            if (!(lpPage->ulItemID & 0xFFFF0000))
            {
#if defined(WIN32) && !defined(MAC)
                if (lpPage->lpctl[0].ulCtlFlags & MAPI_UNICODE)
                {
                    LoadStringW(hInstance,
                        (UINT) lpPage->ulItemID,
                        (LPWSTR) lpszComponent,
                        cchMaxDlgString);
                } else
#endif /* defined(WIN32) */
                {
                    LoadStringA(hInstance, 
                        (UINT) lpPage->ulItemID,
                        (LPSTR) lpszComponent,
                        cchMaxDlgString);
                }
            } else
            {
                lpszComponent = lpPage->lpszComponent;
            }

            /*  Row ID */
            rgpropT[0].ulPropTag = PR_ROWID;
            rgpropT[0].Value.ul = irow++;

            /*  No coordinates for page */

            /*  Control type */
            rgpropT[1].ulPropTag = PR_CONTROL_TYPE;
            rgpropT[1].Value.l = DTCT_PAGE;

            /*  Control flags */
            rgpropT[2].ulPropTag = PR_CONTROL_FLAGS;
            rgpropT[2].Value.l = lpPage->lpctl[0].ulCtlFlags;

            /*  Control structure */
            memset(rgb, 0, sizeof(rgb));
            rgpropT[3].ulPropTag = PR_CONTROL_STRUCTURE;
            if (!szText[0])
                lstrcpy(szText, TEXT("General"));       /* //$BUG not localizable */

            cbText = min(Cbtszsize(szText), sizeof(rgb)-sizeof(DTBLPAGE)-1);
            MemCopy(rgb+sizeof(DTBLPAGE), szText, cbText);

            ((LPDTBLPAGE)rgb)->ulbLpszLabel = sizeof(DTBLPAGE);
            ((LPDTBLPAGE)rgb)->ulFlags = lpPage->lpctl[0].ctl.lppage->ulFlags;

            ((LPDTBLPAGE)rgb)->ulbLpszComponent = sizeof(DTBLPAGE)+cbText;
            cbComponent = min(Cbtszsize(lpszComponent), sizeof(rgb)-sizeof(DTBLPAGE)-cbText-1);
            MemCopy(rgb+sizeof(DTBLPAGE)+cbText, lpszComponent, cbComponent);
            ((LPDTBLPAGE)rgb)->ulContext = lpPage->lpctl[0].ctl.lppage->ulContext;

            rgpropT[3].Value.bin.cb = sizeof(DTBLPAGE) + cbText + cbComponent;
            rgpropT[3].Value.bin.lpb = rgb;

            /* Control notification. */
            rgpropT[4].ulPropTag     = PR_CONTROL_ID;
            rgpropT[4].Value.bin.lpb = lpPage->lpctl[0].lpbNotif;
            rgpropT[4].Value.bin.cb  = lpPage->lpctl[0].cbNotif;

            /* X-Y position, X-Y delta */
            rgpropT[5].ulPropTag     = PR_XPOS;
            rgpropT[5].Value.l = 0;
            rgpropT[6].ulPropTag     = PR_DELTAX;
            rgpropT[6].Value.l = 0;

            rgpropT[7].ulPropTag     = PR_YPOS;
            rgpropT[7].Value.l = 0;
            rgpropT[8].ulPropTag     = PR_DELTAY;
            rgpropT[8].Value.l = 0;

            /*  Pop the row in */
            row.cValues = 9;
            row.lpProps = rgpropT;
            if (HR_FAILED(hr = ptad->lpVtbl->HrModifyRow(ptad, &row)))
                goto ret;

            /*  We've already processed the first control */
            ictl = 2;
        }
        else
            /*  Begin with the first control */
            ictl = 1;

        /*  Loop over remaining controls on this page */
        for (pctl = lpPage->lpctl+ictl-1; ictl<= lpPage->cctl; ++ictl, ++pctl)
        {
            /*  Build the display table columns for this row */
            if (FAILED(sc = (*lpfAllocateBuffer)(cColsDT*sizeof(SPropValue),
                    (LPVOID FAR *)&rgprop)))
                goto retSc;
            pprop = rgprop;

            /*  Remember the control text for this item (if any), and */
            /*  advance the pointer to the next dialog item. */
            pitem = (LPDLGITEMTEMPLATE)pb;
            pb += sizeof(DLGITEMTEMPLATE);      /*  point to control type */
#if defined(WIN16)
            if (*pb & 0x80)
                ++pb;
            else
                pb += CbDlgStrlen(pb);          /*  skip to item text */
#elif defined(WIN32) /* && defined(_WINNT_) */
            if (*((WORD *)pb) == 0xffff)
                pb += 4;
            else
                pb += CbDlgStrlen(pb);
#endif

#if !defined(UNICODE) && defined(WIN32) /* && defined(_WINNT_) */
            /*  Copy item text to szText */
            cbText = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)pb, -1,
                szText, sizeof(szText), NULL, NULL);
            if (!cbText)
            {
                DebugTrace("BuildDisplayTable: failed to convert Unicode (%ld)\n", GetLastError());
                sc = E_FAIL;
                goto retSc;
            }
#else
            cbText = CbDlgStrlen(pb);
            MemCopy(szText, pb, min(cbText, sizeof(szText)));
            szText[cchMaxDlgString] = 0;
#endif

#if defined(WIN16)
            pb += CbDlgStrlen(pb);              /*  skip the item text */
#elif defined(WIN32) /* && defined(_WINNT_) */
            if (*((WORD *)pb) == 0xffff)
                pb += 4;
            else
                pb += CbDlgStrlen(pb);
#endif
#ifdef  WIN16
            pb += *pb + 1;                      /*  Skip extra info. */
#elif defined(WIN32) /* && defined(_WINNT_) */
            if ((ULONG)pb % 4)                  /*  Skip creation data. */
                pb += 2;
            else
                pb += 4;
            Assert(((ULONG)pb % 4) == 0);
#endif

            /*  Validations */
            if (pctl->ulItemID != pitem->id)
            {
                DebugTrace("BuildDisplayTable: item ID mismatch, dtct ID %ld, resource ID %ld, page %d, line %d\n"
                          , pctl->ulItemID, pitem->id, iPage, ictl);
                goto badArg;
            }
            if (pctl->ulCtlType != CtltypeOfItem(pitem))
            {
                DebugTrace("BuildDisplayTable: control type mismatch, dtct ID %ld, page %d, line %d\n"
                          , pctl->ulItemID, iPage, ictl);
                goto badArg;
            }
            if (pctl->ulCtlFlags & ~(DT_MULTILINE | DT_EDITABLE |
                DT_REQUIRED | DT_SET_IMMEDIATE | DT_PASSWORD_EDIT |
                DT_ACCEPT_DBCS | DT_SET_SELECTION))
            {
                DebugTrace("BuildDisplayTable: unknown control flags, dtct ID %ld, page %d, line %d\n"
                          , pctl->ulItemID, iPage, ictl);
                goto badArg;
            }

            /*  row ID */
            pprop->ulPropTag = PR_ROWID;
            pprop->Value.ul = irow++;
            ++pprop;

            /*  Coordinates */
            pprop->ulPropTag = PR_XPOS;
            pprop->Value.l = pitem->x;
            ++pprop;
            pprop->ulPropTag = PR_DELTAX;
            pprop->Value.l = pitem->cx;
            ++pprop;
            pprop->ulPropTag = PR_YPOS;
            pprop->Value.l = pitem->y;
            ++pprop;
            pprop->ulPropTag = PR_DELTAY;
            pprop->Value.l = pitem->cy;
            ++pprop;

            /*  Control type */
            pprop->ulPropTag = PR_CONTROL_TYPE;
            pprop->Value.l = (long)pctl->ulCtlType;
            ++pprop;

            /* Control notification. */
            pprop->ulPropTag     = PR_CONTROL_ID;
            pprop->Value.bin.lpb = pctl->lpbNotif;
            pprop->Value.bin.cb  = pctl->cbNotif;
            ++pprop;

            /*  Control flags.  */
            pprop->ulPropTag = PR_CONTROL_FLAGS;
            pprop->Value.l = pctl->ulCtlFlags;
            ++pprop;



            /*  Control structure, which depends on the type. */
            /*  In the switch we set up 'cb' the structure size and */
            /*  'pv' the memory for the structure; common code puts */
            /*  them into the prop value array. */

            cb = 0;
            pv = NULL;

            switch ((short)pctl->ulCtlType)
            {
            case DTCT_LABEL:
            {
#define plabel  ((LPDTBLLABEL)pv)

                /*  Text from the dialog item template becomes the */
                /*  label text (required). */
                cb = sizeof(DTBLLABEL) + cbText;
                if (FAILED(sc = (*lpfAllocateMore)(cb, rgprop, &pv)))
                    goto retSc;
                *plabel = *(pctl->ctl.lplabel);
                Assert(plabel->ulbLpszLabelName == sizeof(DTBLLABEL));
                MemCopy(plabel+1, szText, cbText);
                break;
#undef plabel
            }

            case DTCT_EDIT:
            {
#define pedit   ((LPDTBLEDIT)pv)

                /*  The required filter string comes with the structure, */
                /*  NOT from the dialog template. Text from the dialog */
                /*  template is IGNORED; any initial value for the edit */
                /*  comes from the property interface. */
                cb = sizeof(DTBLEDIT) + Cbtszsize(pctl->lpszFilter);
                if (FAILED(sc = (*lpfAllocateMore)(cb, rgprop, &pv)))
                    goto retSc;
                MemCopy(pedit, pctl->ctl.lpedit, sizeof(DTBLEDIT));
                lstrcpy((LPTSTR)(pedit+1), pctl->lpszFilter);
                Assert(pedit->ulbLpszCharsAllowed = sizeof(DTBLEDIT));
                break;
#undef pedit
            }

            case DTCT_LBX:
            {
#define plbx    ((LPDTBLLBX)pv)

                /*  No text */
                cb = sizeof(DTBLLBX);
                if (FAILED(sc = (*lpfAllocateMore)(cb, rgprop, &pv)))
                    goto retSc;
                *plbx = *(pctl->ctl.lplbx);
                break;
#undef plbx
            }

            case DTCT_COMBOBOX:
            {
#define pcombobox   ((LPDTBLCOMBOBOX)pv)
                /*  Filter string as for an edit */
                /*  Dialog template text is IGNORED */
                cb = sizeof(DTBLCOMBOBOX) + Cbtszsize(pctl->lpszFilter);
                if (FAILED(sc = (*lpfAllocateMore)(cb, rgprop, &pv)))
                    goto retSc;
                MemCopy(pcombobox, pctl->ctl.lpcombobox, sizeof(DTBLCOMBOBOX));
                lstrcpy((LPTSTR)(pcombobox+1), pctl->lpszFilter);
                Assert(pcombobox->ulbLpszCharsAllowed == sizeof(DTBLCOMBOBOX));
                break;
#undef pcombobox
            }

            case DTCT_DDLBX:
            {
#define pddlbx  ((LPDTBLDDLBX)pv)
                /*  No text or other extra data */
                cb = sizeof(DTBLDDLBX);
                if (FAILED(sc = (*lpfAllocateMore)(cb, rgprop, &pv)))
                    goto retSc;
                *pddlbx = *(pctl->ctl.lpddlbx);
                break;
#undef pddlbx
            }

            case DTCT_CHECKBOX:
            {
#define pcheckbox   ((LPDTBLCHECKBOX)pv)
                /*  Text from the dialog item template is the label */
                cb = sizeof(DTBLCHECKBOX) + cbText;
                if (FAILED(sc = (*lpfAllocateMore)(cb, rgprop, &pv)))
                    goto retSc;
                *pcheckbox = *(pctl->ctl.lpcheckbox);
                MemCopy(pcheckbox+1, szText, cbText);
                pcheckbox->ulbLpszLabel = sizeof(DTBLCHECKBOX);
                break;
#undef pcheckbox
            }

            case DTCT_GROUPBOX:
            {
#define pgroupbox   ((LPDTBLGROUPBOX)pv)
                /*  Text from the dialog item template becomes the label */
                cb = sizeof(DTBLGROUPBOX) + cbText;
                if (FAILED(sc = (*lpfAllocateMore)(cb, rgprop, &pv)))
                    goto retSc;
                *pgroupbox = *(pctl->ctl.lpgroupbox);
                MemCopy(pgroupbox+1, szText, cbText);
                pgroupbox->ulbLpszLabel = sizeof(DTBLGROUPBOX);
                break;
#undef pgroupbox
            }

            case DTCT_BUTTON:
            {
#define pbutton ((LPDTBLBUTTON)pv)
                /*  Text from the dialog item template is the label */
                cb = sizeof(DTBLBUTTON) + cbText;
                if (FAILED(sc = (*lpfAllocateMore)(cb, rgprop, &pv)))
                    goto retSc;
                *pbutton = *(pctl->ctl.lpbutton);
                MemCopy(pbutton+1, szText, cbText);
                pbutton->ulbLpszLabel = sizeof(DTBLBUTTON);
                break;
#undef pbutton
            }

            case DTCT_RADIOBUTTON:
            {
#define pradio  ((LPDTBLRADIOBUTTON)pv)
                /*  Text from the dialog item template is the label */
                cb = sizeof(DTBLRADIOBUTTON) + cbText;
                if (FAILED(sc = (*lpfAllocateMore)(cb, rgprop, &pv)))
                    goto retSc;
                *pradio = *(pctl->ctl.lpradiobutton);
                MemCopy(pradio+1, szText, cbText);
                pradio->ulbLpszLabel = sizeof(DTBLRADIOBUTTON);
                break;
#undef pradio
            }

            case DTCT_INKEDIT:
            {
#define pinkedit    ((LPDTBLINKEDIT)pv)

                /*  The required filter string comes with the structure, */
                /*  NOT from the dialog template. Text from the dialog */
                /*  template is IGNORED; any initial value for the edit */
                /*/ comes from the property interface. */
                cb = sizeof(DTBLINKEDIT) + Cbtszsize(pctl->lpszFilter);
                if (FAILED(sc = (*lpfAllocateMore)(cb, rgprop, &pv)))
                    goto retSc;
                MemCopy(pinkedit, pctl->ctl.lpinkedit, sizeof(DTBLINKEDIT));
                lstrcpy((LPTSTR)(pinkedit+1), pctl->lpszFilter);
                Assert(pinkedit->ulbLpszCharsAllowed = sizeof(DTBLINKEDIT));
                break;
#undef pinkedit
            }

            case DTCT_MVLISTBOX:
            {
#define pmvlbx  ((LPDTBLMVLISTBOX)pv)
                /*  No text or other extra data */
                cb = sizeof(DTBLMVLISTBOX);
                if (FAILED(sc = (*lpfAllocateMore)(cb, rgprop, &pv)))
                    goto retSc;
                *pmvlbx = *(pctl->ctl.lpmvlbx);
                break;
#undef pmvlbx
            }

            case DTCT_MVDDLBX:
            {
#define pmvddlbx    ((LPDTBLMVDDLBX)pv)
                /*  No text or other extra data */
                cb = sizeof(DTBLMVDDLBX);
                if (FAILED(sc = (*lpfAllocateMore)(cb, rgprop, &pv)))
                    goto retSc;
                *pmvddlbx = *(pctl->ctl.lpmvddlbx);
                break;
#undef pmvddlbx
            }

            case DTCT_PAGE:
                /*  Shouldn't see this */
                Assert(FALSE);
                break;
            }

            pprop->ulPropTag = PR_CONTROL_STRUCTURE;
            pprop->Value.bin.cb = cb;
            pprop->Value.bin.lpb = pv;
            ++pprop;

            /*  Add the new line to the display table */
            row.cValues = pprop - rgprop;
            row.lpProps = rgprop;
            if (HR_FAILED(hr = ptad->lpVtbl->HrModifyRow(ptad, &row)))
                goto ret;

/*cleanupLine: */
            (*lpfFreeBuffer)(rgprop);
            rgprop = NULL;
        }

/*cleanupPage: */
        if (hglobal)
        {
            if (pb)
            {
                UnlockResource(hglobal);
                pb = NULL;
            }
            FreeResource(hglobal);
            hglobal = 0;
        }
    }

    /*  Get a view on the table data and return it */
    if (lppTblData==NULL)
        hr = ptad->lpVtbl->HrGetView(ptad, &so, FreeDT, (ULONG)ptad, lppTable);
    else
        hr = ptad->lpVtbl->HrGetView(ptad, &so, NULL, (ULONG)ptad, lppTable);

    #ifdef DEBUG
    if (hr == hrSuccess)
    {
        LH_SetHeapName(((LPUNKOBJ)*lppTable)->lhHeap, "Display Table View Heap");
        MAPISetBufferName(*lppTable, "Display Table View Object");
    }
    #endif

    /*  ...and fall through */

ret:
    (*lpfFreeBuffer)(rgprop);
    if (HR_FAILED(hr))
    {
        if (ptad)
            ptad->lpVtbl->Release(ptad);
    }
    else if (lppTblData)
    {
        *lppTblData=ptad;
    }

    DebugTraceResult(BuildDisplayTable, hr);
    return hr;

retSc:
    hr = ResultFromScode(sc);
    goto ret;

badArg:
    hr = ResultFromScode(E_INVALIDARG);
    goto ret;
}

void STDAPICALLTYPE
FreeDT(ULONG ulCallerData, LPTABLEDATA ptad, LPMAPITABLE ptable)
{
    Assert(ulCallerData == (ULONG)ptad);
    ptad->lpVtbl->Release(ptad);
}

ULONG
CtltypeOfItem(LPDLGITEMTEMPLATE pitem)
{
    LPTSTR  szClass = (LPTSTR)(pitem+1);
    WORD    wClass = 0xffff;
    BYTE    bStyle = (BYTE)pitem->style;

    /*  Not making the 0xffff stuff platform-specific is a little cheesy, */
    /*  but we're going to get away with it because we only support */
    /*  the standard control types, which should never appear as */
    /*  Unicode strings. */

    /*  First figure out the basic control type. */
#ifdef WIN16
    if (*((LPBYTE)szClass) & 0x80)
        wClass = *((LPBYTE)szClass);
#endif
#if defined(WIN32)  /*  && defined(_WINNT_) */
    if (*((WORD *)szClass) == 0xffff)
        wClass = ((WORD *)szClass)[1];
#endif
    else
    {
        if (!lstrcmpi(szClass, "BUTTON"))
            wClass = 0x0080;
        else if (!lstrcmpi(szClass, "EDIT"))
            wClass = 0x0081;
        else if (!lstrcmpi(szClass, "STATIC"))
            wClass = 0x0082;
        else if (!lstrcmpi(szClass, "LISTBOX"))
            wClass = 0x0083;
        else if (!lstrcmpi(szClass, "COMBOBOX"))
            wClass = 0x0085;
    }

    /*  Next, figure out the subtype based on style bits. */
    if (wClass == 0x0080)
    {
        if (bStyle == BS_PUSHBUTTON || bStyle == BS_DEFPUSHBUTTON)
            return DTCT_BUTTON;
        else if (bStyle == BS_RADIOBUTTON || bStyle == BS_AUTORADIOBUTTON)
            return DTCT_RADIOBUTTON;
        else if (bStyle == BS_CHECKBOX || bStyle == BS_AUTOCHECKBOX)
            return DTCT_CHECKBOX;
        else if (bStyle == BS_GROUPBOX)
            return DTCT_GROUPBOX;
        else
            return 0xffffffff;
    }
    else if (wClass == 0x0081)
        return DTCT_EDIT;
    else if (wClass == 0x0082)
        return DTCT_LABEL;
    else if (wClass == 0x0083)
        return DTCT_LBX;
    else if (wClass == 0x0085)
    {
        if (bStyle == CBS_DROPDOWN || bStyle == CBS_DROPDOWNLIST)
            return DTCT_DDLBX;
        return DTCT_COMBOBOX;
    }

    /*  Not one of ours. */
    return 0xffffffff;
}
