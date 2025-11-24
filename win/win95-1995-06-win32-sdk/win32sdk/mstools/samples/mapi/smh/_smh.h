/*
 *  _ S M H . H
 *
 *  Sample mail handling hook
 *  Copyright 1992-95 Microsoft Corporation.  All Rights Reserved.
 */

/*
 *  Object Vtable definition and declairation
 */
#undef  INTERFACE
#define INTERFACE struct _SMH
#undef  MAPIMETHOD_
#define MAPIMETHOD_(type, method)   MAPIMETHOD_DECLARE(type, method, SMH_)
        MAPI_IUNKNOWN_METHODS (IMPL)
        MAPI_ISPOOLERHOOK_METHODS (IMPL)
#undef  MAPIMETHOD_
#define MAPIMETHOD_(type, method)   MAPIMETHOD_TYPEDEF(type, method, SMH_)
        MAPI_IUNKNOWN_METHODS (IMPL)
        MAPI_ISPOOLERHOOK_METHODS (IMPL)
#undef  MAPIMETHOD_
#define MAPIMETHOD_(type, method)   STDMETHOD_(type, method)
DECLARE_MAPI_INTERFACE (SMH_)
{
    MAPI_IUNKNOWN_METHODS (IMPL)
    MAPI_ISPOOLERHOOK_METHODS (IMPL)
};

/*
 *  Misc. Defines
 *
 *  GROW_SIZE is the incremental amount to grow buffers that hold arrays
 *  of structures.  This is used to reduce the number of allocations in
 *  certain situations (ie. configuration)
 */
#define GROW_SIZE   4

/*
 *  cStoMax is the maximum number of stores per profile supported by SMH.
 *  This limit is purely an imposed limit and can be increased or
 *  decreased with no other changes required.
 */
#define cStoMax     64

/*
 *  cchRuleMax is used to limit the names and values of filters.
 */
#define cchRuleMax  79


/*
 *  Store table structure definitions
 *
 *  STOENTRY
 *      This structure is used as a part of the STOTABLE structure.
 *      The union component to the structure allows SMH to cast an
 *      LPSPropValue pointer to a LPSTOENTRY where the first property
 *      is dropped in favor of storing an LPMDB.
 *
 *  STO
 *      This stucture is defined such that SMH can cast an SRow pointer
 *      to an LPSTO.  Again, this construct is used extensively by the
 *      STOTABLE implementation.
 *
 *  STOTABLE
 *      This stucture can be mapped to an SRowSet.  The store table
 *      implementation uses this to let MAPI allocate the memory needed
 *      for keeping pointers to open stores.
 */
typedef struct _STOENTRY
{
    union
    {
        LPMDB           lpmdb;
        SPropValue      valPad;
    };
    SPropValue          lpProps[];

} STOENTRY, FAR * LPSTOENTRY;

typedef struct _STO
{
    ULONG               ulReserved;
    ULONG               cValues;
    union
    {
        LPSTOENTRY      lpstoe;
        LPSPropValue    lpProps;
    };

} STO, FAR * LPSTO;

typedef struct _STOTABLE
{
    ULONG               cSto;
    STO                 aSto[cStoMax];

} STOTABLE, FAR * LPSTOTABLE;


/*
 *  Archive bucket structures definitions
 *
 *  DFT
 *      The DFT structure contains two FILETIMEs that define the delta of
 *      time supported by the archive folder.
 *
 *  BKIT
 *      The BKIT structure contains all the information required to
 *      process the archival of a message into that bucket.
 *
 *      The EntryID of the parent folder, the year folder, and the
 *      monthly folder are all cashed as is the MAPIFOLDER object.  The
 *      DFT for the year and month are cached as well.
 */
typedef struct _DFT
{
    FILETIME            ftStart;
    FILETIME            ftEnd;

} DFT, FAR * LPDFT;

typedef struct _BKIT
{
    ULONG               cbeid;
    LPENTRYID           lpeid;
    LPMAPIFOLDER        lpfldr;
    DFT                 dft;

    ULONG               cbeidYr;
    LPENTRYID           lpeidYr;
    LPMAPIFOLDER        lpfldrYr;
    DFT                 dftYr;

    ULONG               cbeidParent;
    LPENTRYID           lpeidParent;
    LPMAPIFOLDER        lpfldrParent;

} BKIT, FAR * LPBKIT;


/*
 *  SMH stucture definitions
 *  
 *  WB
 *      The WB (wastebasket) structure contains the information
 *      required to archive deleted mail on any given message store
 *  
 *  RULE
 *      The RULE structure contains the information required to filter an
 *      inbound message into the desired target folder
 *  
 *  OOF
 *      The OOF structure contains the information required to generate
 *      out-of-office responses to incoming messages
 *  
 *  SMH
 *      The SMH object is supports the ISpoolerHook interface and is the
 *      object handed to mapi to facilitate all message filtering.
 *  
 */
typedef struct _WB
{
    struct _WB FAR *    wbNext;         /* next pointer */
    struct _SMH FAR *   lpsmh;          /* owning SMH object */

    LPMDB               lpmdb;          /* MDB on which this WB applies */
    LPMAPITABLE         lptbl;          /* contents table of the wastebasket */
    LPMAPIFOLDER        lpfldr;         /* folder object of the wastebasket */
    LPSPropValue        lpvalEid;       /* entryid of the wastebasket */
    ULONG               ulAdvz;         /* connection ID returned from Advise() */
    BKIT                bkit;           /* bucket cache for this WB */

} WB, FAR * LPWB;

typedef struct _RULE
{
    struct _RULE FAR *  rlNext;         /* next pointer */

    MAPIUID             muid;           /* profile section UID */
    UINT                rlTyp;          /* rule type */
    ULONG               ulFlags;        /* rule attributes */
    SRestriction FAR *  lpres;          /* recipient rule restriction */
    TCHAR               lpszData[cchRuleMax];   /* value to filter against */
    TCHAR               lpszSound[cchRuleMax];  /* sound to play */
    LPMDB               lpmdb;          /* MDB owning the target folder */
    LPMAPIFOLDER        lpfldr;         /* target folder for filter */
    LPSPropValue        lpvalEid;       /* entryid of target folder */
    BKIT                bkit;           /* bucket cache for rule */

} RULE, FAR * LPRULE;

typedef struct _OOF
{
    HINSTANCE           hlib;           /* library handle */
    LPTABLEDATA         lptad;          /* Table data object */
    LPMAPITABLE         lptbl;          /* Table data view */
    ULONG               cRecips;        /* Count of recips */

} OOF, FAR * LPOOF;

typedef struct _SMH
{
    SMH_Vtbl FAR *      lpVtbl;
    ULONG               lcInit;         /* object refcount */
    HANDLE              hevtConfig;     /* configuration change event */
    HINSTANCE           hinst;          /* DLL instance */

    LPMAPISESSION       lpsess;         /* client session */
    LPALLOCATEBUFFER    lpfnAlloc;      /* MAPIAllocateBuffer */
    LPALLOCATEMORE      lpfnAllocMore;  /* MAPIAllocateMore */
    LPFREEBUFFER        lpfnFree;       /* MAPIFreeBuffer */
    MAPIUID             muid;           /* hook provider section UID */

    BOOL                fCatSm:1;       /* options flags */
    BOOL                fCatSmByYr:1;
    BOOL                fCatWb:1;
    LPSTOTABLE          lpstotbl;       /* store table */
    SPropValue          valEx;          /* msg class exclustions */
    LPWB                lstWb;          /* list pointer of WBs */
    LPRULE              lstRl;          /* list pointer of rules */
    BKIT                bkitSm;         /* bucket cache for sent mail */
    OOF                 oof;            /* OOF information */

} SMH, FAR * LPSMH;


/*
 *  Configuration dialog structure definition
 *
 *  SMHDLG
 *      This structure is used to pass important information in and out
 *      of the configuration dialogs and property sheets.
 */
typedef struct _SMHDLG
{
    HINSTANCE           hinst;          /* DLL instance */
    HWND                hwnd;           /* parent window */

    LPALLOCATEBUFFER    lpfnAlloc;      /* MAPIAllocateBuffer */
    LPALLOCATEMORE      lpfnAllocMore;  /* MAPIAllocateMore */
    LPFREEBUFFER        lpfnFree;       /* MAPIFreeBuffer */
    LPMALLOC            lpmalloc;       /* pointer to LPMALLOC obj */
    LPMAPISUP           lpsup;          /* pointer to support obj */
    LPPROVIDERADMIN     lpadmin;        /* pointer to profile admin obj */

    LPSPropTagArray     lpspt;          /* proptag array of config props */
    LPSPropValue        lpvalSMH;       /* config prop values */
    LPPROFSECT          lpsec;          /* profile section obj */

    ULONG               ulFlags;        /* configuration flags */
    SCODE               sc;             /* dialog failure error code */

    LPSPropValue        lpval;          /* short-term propval buffer */
    TCHAR               rgchT[cchRuleMax + 1];  /* stort-term name buffer */

    BOOL                fDirty;         /* configuration changed flag */

    UINT                cmuid;          /* rule UID list count */
    UINT                cmuidMax;       /* rule UID list max */
    LPMAPIUID           lpmuid;         /* rule UID list */

} SMHDLG, FAR * LPSMHDLG;


/*
 *  SMH proptags
 */
#define PR_SMH_FLAGS                    PROP_TAG(PT_LONG,       0x6600)
#define PR_SMH_RULES                    PROP_TAG(PT_BINARY,     0x6601)
#define PR_SMH_EXCLUSIONS               PROP_TAG(PT_MV_TSTRING, 0x6602)

/*
 *  Values for PR_SMH_FLAGS
 */
#define SMH_FILTER_SENTMAIL             ((ULONG)0x00000001)
#define SMH_FILTER_SENTMAIL_YR          ((ULONG)0x00000002)
#define SMH_FILTER_DELETED              ((ULONG)0x00000010)
#define SMH_FILTER_DELETED_YR           ((ULONG)0x00000020)
#define SMH_FILTER_INBOUND              ((ULONG)0x00000100)
#define SMH_UNREAD_VIEWER               ((ULONG)0x00001000)

/*
 *  RULE proptags
 */
#define PR_RULE_TYPE                    PROP_TAG(PT_LONG,       0x6610)
#define PR_RULE_DATA                    PROP_TAG(PT_BINARY,     0x6611)
#define PR_RULE_FLAGS                   PROP_TAG(PT_LONG,       0x6612)
#define PR_RULE_TARGET_ENTRYID          PROP_TAG(PT_BINARY,     0x6613)
#define PR_RULE_TARGET_PATH             PROP_TAG(PT_TSTRING,    0x6614)
#define PR_RULE_TARGET_PATH_A           PROP_TAG(PT_STRING8,    0x6614)
#define PR_RULE_TARGET_PATH_W           PROP_TAG(PT_UNICODE,    0x6614)
#define PR_RULE_STORE_ENTRYID           PROP_TAG(PT_BINARY,     0x6615)
#define PR_RULE_STORE_DISPLAY_NAME      PROP_TAG(PT_TSTRING,    0x6616)
#define PR_RULE_STORE_DISPLAY_NAME_A    PROP_TAG(PT_STRING8,    0x6616)
#define PR_RULE_STORE_DISPLAY_NAME_W    PROP_TAG(PT_UNICODE,    0x6616)
#define PR_RULE_SOUND_FILENAME          PROP_TAG(PT_TSTRING,    0x6617)
#define PR_RULE_SOUND_FILENAME_A        PROP_TAG(PT_STRING8,    0x6617)
#define PR_RULE_SOUND_FILENAME_W        PROP_TAG(PT_UNICODE,    0x6617)


/*
 *  Values for PR_RULE_TYPE
 */
#define RL_SUBJECT                      ((UINT)0x0001)
#define RL_FROM                         ((UINT)0x0002)
#define RL_ATTACH                       ((UINT)0x0004)
#define RL_BODY                         ((UINT)0x0008)
#define RL_TO                           ((UINT)0x0010)
#define RL_CC                           ((UINT)0x0020)
#define RL_BCC                          ((UINT)0x0040)
#define RL_RECIP                        ((UINT)0x0080)
#define RL_CLASS                        ((UINT)0x0100)

/*
 *  Values for PR_RULE_FLAGS
 */
#define RULE_ARCHIVED                   ((ULONG)0x00000001)
#define RULE_ARCHIVED_BY_YEAR           ((ULONG)0x00000002)
#define RULE_NOT                        ((ULONG)0x00000004)
#define RULE_DELETE                     ((ULONG)0x00000008)
#define RULE_TERMINAL                   ((ULONG)0x80000000)

/*
 *  Rule configuration flags
 */
#define NEW_RULE                        ((UINT)1)
#define EDIT_RULE                       ((UINT)2)

/* Logon properties */

enum {ipSMHFlags, ipRules, ipExc, cpMax};

/* Rule properties */

enum {ipDisp, ipType, ipData, ipRLFlags, ipEID, ipPath, ipSEID, ipStore, ipSound, cpRLMax};

/* Message properties */

enum {ipMsgFlgs, ipSubj, ipSentRep, ipSentRepEA, cpMsgPrps};

/* Recipient restriction enums */

enum {ivRecip, ivEmail, ivDispNm, cvMax };
enum {iresAnd, iresRecip, iresOr, iresEmail, iresDispNm, cresMax };


/*
 *  Logon properties required flags
 */
#define REQ_PR_SMH_FLAGS                ((ULONG)(1<<ipSMHFlags))
#define REQ_PR_SMH_RULES                ((ULONG)(1<<ipRules))
#define REQ_PROPS                       REQ_PR_SMH_FLAGS
ULONG UlChkReqProps (ULONG cval, LPSPropValue lpval);


/*
 *  Configuration dialogs
 */
#if !defined (_WIN95)
#define RegDlg3D(_inst)         Ctl3dRegister (_inst);
#define MakeDlg3D(_dlg)         Ctl3dSubclassDlgEx(_dlg, CTL3D_ALL)
#define UnregDlg3D(_inst)       Ctl3dUnregister (_inst);
#else
#define RegDlg3D(_inst)
#define MakeDlg3D(_dlg)
#define UnregDlg3D(_inst)
#endif

#define FHandleWm(_fn,_dlg,_wm,_wp,_lp) HANDLE_WM_##_wm(_dlg,_wp,_lp,_fn##_##_wm)
HRESULT HrDisplayPropSheets (HINSTANCE hinst, HWND hwnd, LPSMHDLG lpsmhdlg);


/*
 *  Configuration Events
 */
#ifdef  WIN32
HRESULT HrGetConfigEvent (HANDLE FAR *);
VOID SignalConfigChanged (VOID);
BOOL FConfigChanged (HANDLE);
#define ResetConfigEvent(_hevt)     ResetEvent(_hevt)
#define CloseConfigEvent(_hevt)     CloseHandle(_hevt)
#else
#define HrGetConfigEvent(_lphevt)   (hrSuccess)
#define SignalConfigChanged()
#define FConfigChanged(_hevt)       (FALSE)
#define ResetConfigEvent(_hevt)
#define CloseConfigEvent(_hevt)
#endif


/*
 *  Stores table
 */
HRESULT HrInitStoresTable (LPSMH, LPMAPISESSION);
HRESULT HrOpenMdbFromEid (LPSMH, ULONG, LPENTRYID, LPMDB FAR *);
HRESULT HrOpenMdbFromName (LPSMH, LPTSTR, LPMDB FAR *);
HRESULT HrOpenStoEntry (LPMAPISESSION, LPSTO, LPMDB FAR *);
VOID ReleaseStoresTable (LPSMH);


/*
 *  WM_NOTIFY #defines (taken from WINUSER.H)
 */
#ifdef  WIN16
typedef struct tagNMHDR
{
    HWND  hwndFrom;
    UINT  idFrom;
    UINT  code;
}   NMHDR;
typedef NMHDR FAR * LPNMHDR;
#define WM_NOTIFY   0x004E
#define HANDLE_WM_NOTIFY(hwnd,wParam,lParam,fn) (fn)((hwnd),(int)(wParam),(NMHDR FAR*)(lParam))

/*
 *  Mapped bitmapi #defines (taken from COMMCTRL.H)
 */
typedef struct
{
    COLORREF from;
    COLORREF to;
} COLORMAP, FAR * LPCOLORMAP;

HBITMAP WINAPI CreateMappedBitmap(HINSTANCE hInstance, INT idBitmap, BOOL bDiscardable,
    LPCOLORMAP lpColorMap, INT iNumMaps);
#endif  /* WIN16 */
