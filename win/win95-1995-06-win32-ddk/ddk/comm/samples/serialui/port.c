/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
****************************************************************************/

//---------------------------------------------------------------------------
//
// File: port.c
//
// This files contains the dialog code for the Port Settings property page.
//
//---------------------------------------------------------------------------

#include "suiprv.h"         // common headers
#include "serialui.h"

#include "res.h"

// This is the structure that is used to fill the 
// max speed listbox
typedef struct _Bauds
    {
    DWORD   dwDTERate;
    int     ids;
    } Bauds;

static Bauds g_rgbauds[] = {
        { 110L,         IDS_BAUD_110     },
        { 300L,         IDS_BAUD_300     },
        { 1200L,        IDS_BAUD_1200    },
        { 2400L,        IDS_BAUD_2400    },
        { 4800L,        IDS_BAUD_4800    },
        { 9600L,        IDS_BAUD_9600    },
        { 19200,        IDS_BAUD_19200   },
        { 38400,        IDS_BAUD_38400   },
        { 57600,        IDS_BAUD_57600   },
        { 115200,       IDS_BAUD_115200  },
        { 230400,       IDS_BAUD_230400  },
        { 560800,       IDS_BAUD_560800  },
        { 1121600,      IDS_BAUD_1121600 },
        };

// Command IDs for the parity listbox
#define CMD_PARITY_EVEN         1
#define CMD_PARITY_ODD          2
#define CMD_PARITY_NONE         3
#define CMD_PARITY_MARK         4
#define CMD_PARITY_SPACE        5

// Command IDs for the flow control listbox
#define CMD_FLOWCTL_XONXOFF      1
#define CMD_FLOWCTL_HARDWARE     2
#define CMD_FLOWCTL_NONE         3

// This table is the generic port settings table
// that is used to fill the various listboxes
typedef struct _PortValues
    {
    union {
        BYTE bytesize;
        BYTE cmd;
        BYTE stopbits;
        };
    int ids;
    } PortValues, FAR * LPPORTVALUES;


#pragma data_seg(DATASEG_READONLY)

// This is the structure that is used to fill the data bits listbox
static PortValues s_rgbytesize[] = {
        { 4,  IDS_BYTESIZE_4  },
        { 5,  IDS_BYTESIZE_5  },
        { 6,  IDS_BYTESIZE_6  },
        { 7,  IDS_BYTESIZE_7  },
        { 8,  IDS_BYTESIZE_8  },
        };

// This is the structure that is used to fill the parity listbox
static PortValues s_rgparity[] = {
        { CMD_PARITY_EVEN,  IDS_PARITY_EVEN  },
        { CMD_PARITY_ODD,   IDS_PARITY_ODD   },
        { CMD_PARITY_NONE,  IDS_PARITY_NONE  },
        { CMD_PARITY_MARK,  IDS_PARITY_MARK  },
        { CMD_PARITY_SPACE, IDS_PARITY_SPACE },
        };

// This is the structure that is used to fill the stopbits listbox
static PortValues s_rgstopbits[] = {
        { ONESTOPBIT,   IDS_STOPBITS_1   },
        { ONE5STOPBITS, IDS_STOPBITS_1_5 },
        { TWOSTOPBITS,  IDS_STOPBITS_2   },
        };

// This is the structure that is used to fill the flow control listbox
static PortValues s_rgflowctl[] = {
        { CMD_FLOWCTL_XONXOFF,  IDS_FLOWCTL_XONXOFF  },
        { CMD_FLOWCTL_HARDWARE, IDS_FLOWCTL_HARDWARE },
        { CMD_FLOWCTL_NONE,     IDS_FLOWCTL_NONE     },
        };

#pragma data_seg()


typedef struct tagPORT
    {
    HWND hdlg;              // dialog handle
    HWND hwndBaudRate;
    HWND hwndDataBits;
    HWND hwndParity;
    HWND hwndStopBits;
    HWND hwndFlowCtl;

    LPPORTINFO pportinfo;   // pointer to shared working buffer
    
    } PORT, FAR * PPORT;


// This structure contains the default settings for the dialog
static struct _DefPortSettings
    {
    int  iSelBaud;
    int  iSelDataBits;
    int  iSelParity;
    int  iSelStopBits;
    int  iSelFlowCtl;
    } s_defportsettings;

// These are default settings
#define DEFAULT_BAUDRATE            9600L
#define DEFAULT_BYTESIZE            8
#define DEFAULT_PARITY              CMD_PARITY_NONE
#define DEFAULT_STOPBITS            ONESTOPBIT
#define DEFAULT_FLOWCTL             CMD_FLOWCTL_NONE


#define Port_GetPtr(hwnd)           (PPORT)GetWindowLong(hwnd, DWL_USER)
#define Port_SetPtr(hwnd, lp)       (PPORT)SetWindowLong(hwnd, DWL_USER, (LONG)(lp))



/*----------------------------------------------------------
Purpose: Fills the baud rate combobox with the possible baud
         rates that Windows supports.
Returns: --
Cond:    --
*/
void PRIVATE Port_FillBaud(
    PPORT this)
    {
    HWND hwndCB = this->hwndBaudRate;
    WIN32DCB FAR * pdcb = &this->pportinfo->dcb;
    int i;
    int n;
    int iMatch = -1;
    int iDef = -1;
    int iSel;
    char sz[MAXMEDLEN];

    // Fill the listbox
    for (i = 0; i < ARRAYSIZE(g_rgbauds); i++)
        {
        n = ComboBox_AddString(hwndCB, SzFromIDS(g_rgbauds[i].ids, sz, sizeof(sz)));
        ComboBox_SetItemData(hwndCB, n, g_rgbauds[i].dwDTERate);

        // Keep our eyes peeled for important values
        if (DEFAULT_BAUDRATE == g_rgbauds[i].dwDTERate)
            {
            iDef = n;
            }
        if (pdcb->BaudRate == g_rgbauds[i].dwDTERate)
            {
            iMatch = n;
            }
        }

    ASSERT(-1 != iDef);
    s_defportsettings.iSelBaud = iDef;

    // Does the DCB baudrate exist in our list of baud rates?
    if (-1 == iMatch)
        {
        // No; choose the default
        iSel = iDef;
        }
    else 
        {
        // Yes; choose the matched value
        ASSERT(-1 != iMatch);
        iSel = iMatch;
        }
    ComboBox_SetCurSel(hwndCB, iSel);
    }


/*----------------------------------------------------------
Purpose: Fills the bytesize combobox with the possible byte sizes.
Returns: --
Cond:    --
*/
void PRIVATE Port_FillDataBits(
    PPORT this)
    {
    HWND hwndCB = this->hwndDataBits;
    WIN32DCB FAR * pdcb = &this->pportinfo->dcb;
    int i;
    int iSel;
    int n;
    int iMatch = -1;
    int iDef = -1;
    char sz[MAXMEDLEN];

    // Fill the listbox
    for (i = 0; i < ARRAYSIZE(s_rgbytesize); i++)
        {
        n = ComboBox_AddString(hwndCB, SzFromIDS(s_rgbytesize[i].ids, sz, sizeof(sz)));
        ComboBox_SetItemData(hwndCB, n, s_rgbytesize[i].bytesize);

        // Keep our eyes peeled for important values
        if (DEFAULT_BYTESIZE == s_rgbytesize[i].bytesize)
            {
            iDef = n;
            }
        if (pdcb->ByteSize == s_rgbytesize[i].bytesize)
            {
            iMatch = n;
            }
        }

    ASSERT(-1 != iDef);
    s_defportsettings.iSelDataBits = iDef;

    // Does the DCB value exist in our list?
    if (-1 == iMatch)
        {
        // No; choose the default
        iSel = iDef;
        }
    else 
        {
        // Yes; choose the matched value
        ASSERT(-1 != iMatch);
        iSel = iMatch;
        }
    ComboBox_SetCurSel(hwndCB, iSel);
    }


/*----------------------------------------------------------
Purpose: Fills the parity combobox with the possible settings.
Returns: --
Cond:    --
*/
void PRIVATE Port_FillParity(
    PPORT this)
    {
    HWND hwndCB = this->hwndParity;
    WIN32DCB FAR * pdcb = &this->pportinfo->dcb;
    int i;
    int iSel;
    int n;
    int iMatch = -1;
    int iDef = -1;
    char sz[MAXMEDLEN];

    // Fill the listbox
    for (i = 0; i < ARRAYSIZE(s_rgparity); i++)
        {
        n = ComboBox_AddString(hwndCB, SzFromIDS(s_rgparity[i].ids, sz, sizeof(sz)));
        ComboBox_SetItemData(hwndCB, n, s_rgparity[i].cmd);

        // Keep our eyes peeled for important values
        if (DEFAULT_PARITY == s_rgparity[i].cmd)
            {
            iDef = n;
            }
        switch (s_rgparity[i].cmd)
            {
        case CMD_PARITY_EVEN:
            if (TRUE == pdcb->fParity && EVENPARITY == pdcb->Parity)
                iMatch = n;
            break;

        case CMD_PARITY_ODD:
            if (TRUE == pdcb->fParity && ODDPARITY == pdcb->Parity)
                iMatch = n;
            break;

        case CMD_PARITY_NONE:
            if (FALSE == pdcb->fParity && NOPARITY == pdcb->Parity)
                iMatch = n;
            break;

        case CMD_PARITY_MARK:
            if (TRUE == pdcb->fParity && MARKPARITY == pdcb->Parity)
                iMatch = n;
            break;

        case CMD_PARITY_SPACE:
            if (TRUE == pdcb->fParity && SPACEPARITY == pdcb->Parity)
                iMatch = n;
            break;

        default:
            ASSERT(0);
            break;
            }
        }

    ASSERT(-1 != iDef);
    s_defportsettings.iSelParity = iDef;

    // Does the DCB value exist in our list?
    if (-1 == iMatch)
        {
        // No; choose the default
        iSel = iDef;
        }
    else 
        {
        // Yes; choose the matched value
        ASSERT(-1 != iMatch);
        iSel = iMatch;
        }
    ComboBox_SetCurSel(hwndCB, iSel);
    }


/*----------------------------------------------------------
Purpose: Fills the stopbits combobox with the possible settings.
Returns: --
Cond:    --
*/
void PRIVATE Port_FillStopBits(
    PPORT this)
    {
    HWND hwndCB = this->hwndStopBits;
    WIN32DCB FAR * pdcb = &this->pportinfo->dcb;
    int i;
    int iSel;
    int n;
    int iMatch = -1;
    int iDef = -1;
    char sz[MAXMEDLEN];

    // Fill the listbox
    for (i = 0; i < ARRAYSIZE(s_rgstopbits); i++)
        {
        n = ComboBox_AddString(hwndCB, SzFromIDS(s_rgstopbits[i].ids, sz, sizeof(sz)));
        ComboBox_SetItemData(hwndCB, n, s_rgstopbits[i].stopbits);

        // Keep our eyes peeled for important values
        if (DEFAULT_STOPBITS == s_rgstopbits[i].stopbits)
            {
            iDef = n;
            }
        if (pdcb->StopBits == s_rgstopbits[i].stopbits)
            {
            iMatch = n;
            }
        }

    ASSERT(-1 != iDef);
    s_defportsettings.iSelStopBits = iDef;

    // Does the DCB value exist in our list?
    if (-1 == iMatch)
        {
        // No; choose the default
        iSel = iDef;
        }
    else 
        {
        // Yes; choose the matched value
        ASSERT(-1 != iMatch);
        iSel = iMatch;
        }
    ComboBox_SetCurSel(hwndCB, iSel);
    }


/*----------------------------------------------------------
Purpose: Fills the flow control combobox with the possible settings.
Returns: --
Cond:    --
*/
void PRIVATE Port_FillFlowCtl(
    PPORT this)
    {
    HWND hwndCB = this->hwndFlowCtl;
    WIN32DCB FAR * pdcb = &this->pportinfo->dcb;
    int i;
    int iSel;
    int n;
    int iMatch = -1;
    int iDef = -1;
    char sz[MAXMEDLEN];

    // Fill the listbox
    for (i = 0; i < ARRAYSIZE(s_rgflowctl); i++)
        {
        n = ComboBox_AddString(hwndCB, SzFromIDS(s_rgflowctl[i].ids, sz, sizeof(sz)));
        ComboBox_SetItemData(hwndCB, n, s_rgflowctl[i].cmd);

        // Keep our eyes peeled for important values
        if (DEFAULT_FLOWCTL == s_rgflowctl[i].cmd)
            {
            iDef = n;
            }
        switch (s_rgflowctl[i].cmd)
            {
        case CMD_FLOWCTL_XONXOFF:
            if (TRUE == pdcb->fOutX && FALSE == pdcb->fOutxCtsFlow)
                iMatch = n;
            break;

        case CMD_FLOWCTL_HARDWARE:
            if (FALSE == pdcb->fOutX && TRUE == pdcb->fOutxCtsFlow)
                iMatch = n;
            break;

        case CMD_FLOWCTL_NONE:
            if (FALSE == pdcb->fOutX && FALSE == pdcb->fOutxCtsFlow)
                iMatch = n;
            break;

        default:
            ASSERT(0);
            break;
            }
        }

    ASSERT(-1 != iDef);
    s_defportsettings.iSelFlowCtl = iDef;

    // Does the DCB value exist in our list?
    if (-1 == iMatch)
        {
        // No; choose the default
        iSel = iDef;
        }
    else 
        {
        // Yes; choose the matched value
        ASSERT(-1 != iMatch);
        iSel = iMatch;
        }
    ComboBox_SetCurSel(hwndCB, iSel);
    }


/*----------------------------------------------------------
Purpose: WM_INITDIALOG Handler
Returns: FALSE when we assign the control focus
Cond:    --
*/
BOOL PRIVATE Port_OnInitDialog(
    PPORT this,
    HWND hwndFocus,
    LPARAM lParam)              // expected to be PROPSHEETINFO 
    {
    LPPROPSHEETPAGE lppsp = (LPPROPSHEETPAGE)lParam;
    HWND hwnd = this->hdlg;

    ASSERT((LPSTR)lppsp->lParam);

    this->pportinfo = (LPPORTINFO)lppsp->lParam;

    // Save away the window handles
    this->hwndBaudRate = GetDlgItem(hwnd, IDC_PS_BAUDRATE);
    this->hwndDataBits = GetDlgItem(hwnd, IDC_PS_DATABITS);
    this->hwndParity = GetDlgItem(hwnd, IDC_PS_PARITY);
    this->hwndStopBits = GetDlgItem(hwnd, IDC_PS_STOPBITS);
    this->hwndFlowCtl = GetDlgItem(hwnd, IDC_PS_FLOWCTL);

    Port_FillBaud(this);
    Port_FillDataBits(this);
    Port_FillParity(this);
    Port_FillStopBits(this);
    Port_FillFlowCtl(this);

    return TRUE;   // allow USER to set the initial focus
    }


/*----------------------------------------------------------
Purpose: WM_COMMAND Handler
Returns: --
Cond:    --
*/
void PRIVATE Port_OnCommand(
    PPORT this,
    int id,
    HWND hwndCtl,
    UINT uNotifyCode)
    {
    HWND hwnd = this->hdlg;
    
    switch (id)
        {
    case IDC_PS_PB_RESTORE:
        // Set the values to the default settings
        ComboBox_SetCurSel(this->hwndBaudRate, s_defportsettings.iSelBaud);
        ComboBox_SetCurSel(this->hwndDataBits, s_defportsettings.iSelDataBits);
        ComboBox_SetCurSel(this->hwndParity, s_defportsettings.iSelParity);
        ComboBox_SetCurSel(this->hwndStopBits, s_defportsettings.iSelStopBits);
        ComboBox_SetCurSel(this->hwndFlowCtl, s_defportsettings.iSelFlowCtl);
        break;

    default:
        break;
        }
    }


/*----------------------------------------------------------
Purpose: PSN_APPLY handler
Returns: --
Cond:    --
*/
void PRIVATE Port_OnApply(
    PPORT this)
    {
    int iSel;
    BYTE cmd;
    WIN32DCB FAR * pdcb = &this->pportinfo->dcb;

    // Determine new speed settings
    iSel = ComboBox_GetCurSel(this->hwndBaudRate);
    pdcb->BaudRate = ComboBox_GetItemData(this->hwndBaudRate, iSel);


    // Determine new byte size
    iSel = ComboBox_GetCurSel(this->hwndDataBits);
    pdcb->ByteSize = (BYTE)ComboBox_GetItemData(this->hwndDataBits, iSel);


    // Determine new parity settings
    iSel = ComboBox_GetCurSel(this->hwndParity);
    cmd = (BYTE)ComboBox_GetItemData(this->hwndParity, iSel);
    switch (cmd)
        {
    case CMD_PARITY_EVEN:
        pdcb->fParity = TRUE;
        pdcb->Parity = EVENPARITY;
        break;

    case CMD_PARITY_ODD:
        pdcb->fParity = TRUE;
        pdcb->Parity = ODDPARITY;
        break;

    case CMD_PARITY_NONE:
        pdcb->fParity = FALSE;
        pdcb->Parity = NOPARITY;
        break;

    case CMD_PARITY_MARK:
        pdcb->fParity = TRUE;
        pdcb->Parity = MARKPARITY;
        break;

    case CMD_PARITY_SPACE:
        pdcb->fParity = TRUE;
        pdcb->Parity = SPACEPARITY;
        break;

    default:
        ASSERT(0);
        break;
        }

    // Determine new stopbits setting
    iSel = ComboBox_GetCurSel(this->hwndStopBits);
    pdcb->StopBits = (BYTE)ComboBox_GetItemData(this->hwndStopBits, iSel);


    // Determine new flow control settings
    iSel = ComboBox_GetCurSel(this->hwndFlowCtl);
    cmd = (BYTE)ComboBox_GetItemData(this->hwndFlowCtl, iSel);
    switch (cmd)
        {
    case CMD_FLOWCTL_XONXOFF:
        pdcb->fOutX = TRUE;
        pdcb->fInX = TRUE;
        pdcb->fOutxCtsFlow = FALSE;
        pdcb->fRtsControl = RTS_CONTROL_DISABLE;
        break;

    case CMD_FLOWCTL_HARDWARE:
        pdcb->fOutX = FALSE;
        pdcb->fInX = FALSE;
        pdcb->fOutxCtsFlow = TRUE;
        pdcb->fRtsControl = RTS_CONTROL_HANDSHAKE;
        break;

    case CMD_FLOWCTL_NONE:
        pdcb->fOutX = FALSE;
        pdcb->fInX = FALSE;
        pdcb->fOutxCtsFlow = FALSE;
        pdcb->fRtsControl = RTS_CONTROL_DISABLE;
        break;

    default:
        ASSERT(0);      // should never be here
        break;
        }

    this->pportinfo->idRet = IDOK;
    }


/*----------------------------------------------------------
Purpose: WM_NOTIFY handler
Returns: varies
Cond:    --
*/
LRESULT PRIVATE Port_OnNotify(
    PPORT this,
    int idFrom,
    NMHDR FAR * lpnmhdr)
    {
    LRESULT lRet = 0;
    
    switch (lpnmhdr->code)
        {
    case PSN_SETACTIVE:
        break;

    case PSN_KILLACTIVE:
        // N.b. This message is not sent if user clicks Cancel!
        // N.b. This message is sent prior to PSN_APPLY
        //
        break;

    case PSN_APPLY:
        Port_OnApply(this);
        break;

    default:
        break;
        }

    return lRet;
    }


/////////////////////////////////////////////////////  EXPORTED FUNCTIONS

static BOOL s_bPortRecurse = FALSE;

LRESULT INLINE Port_DefProc(
    HWND hDlg, 
    UINT msg,
    WPARAM wParam,
    LPARAM lParam) 
    {
    s_bPortRecurse = TRUE;

    return DefDlgProc(hDlg, msg, wParam, lParam); 
    }


/*----------------------------------------------------------
Purpose: Real dialog proc
Returns: varies
Cond:    --
*/
LRESULT Port_DlgProc(
    PPORT this,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
    {
    switch (message)
        {
        HANDLE_MSG(this, WM_INITDIALOG, Port_OnInitDialog);
        HANDLE_MSG(this, WM_COMMAND, Port_OnCommand);
        HANDLE_MSG(this, WM_NOTIFY, Port_OnNotify);
        default:
            return Port_DefProc(this->hdlg, message, wParam, lParam);
        }
    }


/*----------------------------------------------------------
Purpose: Dialog Wrapper
Returns: varies
Cond:    --
*/
BOOL CALLBACK Port_WrapperProc(
    HWND hDlg,          // std params
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
    {
    PPORT this;

    // Cool windowsx.h dialog technique.  For full explanation, see
    //  WINDOWSX.TXT.  This supports multiple-instancing of dialogs.
    //
    if (s_bPortRecurse)
        {
        s_bPortRecurse = FALSE;
        return FALSE;
        }

    this = Port_GetPtr(hDlg);
    if (this == NULL)
        {
        if (message == WM_INITDIALOG)
            {
            this = (PPORT)LocalAlloc(LPTR, sizeof(PORT));
            if (!this)
                {
                MsgBox_Err(hDlg, IDS_OOM_PORT, IDS_CAP_PORT);
                EndDialog(hDlg, IDCANCEL);
                return (BOOL)Port_DefProc(hDlg, message, wParam, lParam);
                }
            this->hdlg = hDlg;
            Port_SetPtr(hDlg, this);
            }
        else
            {
            return (BOOL)Port_DefProc(hDlg, message, wParam, lParam);
            }
        }

    if (message == WM_DESTROY)
        {
        Port_DlgProc(this, message, wParam, lParam);
        LocalFree((HLOCAL)OFFSETOF(this));
        Port_SetPtr(hDlg, NULL);
        return 0;
        }

    return SetDlgMsgResult(hDlg, message, Port_DlgProc(this, message, wParam, lParam));
    }

