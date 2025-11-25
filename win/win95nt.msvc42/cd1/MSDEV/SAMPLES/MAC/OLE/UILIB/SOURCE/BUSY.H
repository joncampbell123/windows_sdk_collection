/*
 * BUSY.H
 *
 * Internal definitions, structures, and function prototypes for the
 * OLE 2.0 UI Busy dialog.
 *
 * Copyright (c)1992-1994 Microsoft Corporation, All Right Reserved
 */

#ifndef _BUSY_H_
#define _BUSY_H_

#define DIDBusy 						  10005

//Convert Dialog identifiers
#define BZ_BTN_SWITCHTO					  1
#define BZ_BTN_RETRY					  2
#define BZ_BTN_CANCEL					  3
#define BZ_UITEM_MSG					  4
#define BZ_UITEM_SWITCHTOOUTLINE		  5

#define BZ_UITEM_FIRST				BZ_UITEM_MSG
#define BZ_UITEM_LAST				BZ_UITEM_SWITCHTOOUTLINE

#define BZ_ITEM_MAX					BZ_UITEM_SWITCHTOOUTLINE

//Internally used structure
typedef struct tagBUSY
    {
    //Keep this item first as the Standard* functions depend on it here.
    LPOLEUIBUSY     lpOBZ;       //Original structure passed.

    /*
     * What we store extra in this structure besides the original caller's
     * pointer are those fields that we need to modify during the life of
     * the dialog or that we don't want to change in the original structure
     * until the user presses OK.
     */

    unsigned long            dwFlags;                // Flags passed in
    char					 szMsg[255];			 // store the message for the dialog
} BUSY, *PBUSY, *LPBUSY;

// Internal function prototypes
Boolean GetTaskInfo(ProcessSerialNumberPtr pPSN, StringPtr processName);
void BuildBusyDialogString(DialogPtr pDialog, unsigned long dwFlags, StringPtr processName, char szBuf[]);
#ifndef _MSC_VER
pascal Boolean BusyDialogProc(DialogPtr pDialog, EventRecord *pEvent, short *nItem);
pascal void BusyUserItemProc(DialogPtr pDialog, short iitem);
#else
Boolean __pascal BusyDialogProc(DialogPtr pDialog, EventRecord *pEvent, short *nItem);
void __pascal BusyUserItemProc(DialogPtr pDialog, short iitem);
#endif
void BusyCleanup(WindowPtr hDlg);
unsigned int FBusyInit(DialogPtr pDialog, POLEUIBUSY pOBZ);
unsigned int BZInitUserItems(DialogPtr pDialog);

#endif //_BUSY_H_
