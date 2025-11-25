// Document.cpp: implementation of the CDocument class.
//
//////////////////////////////////////////////////////////////////////


#define _WIN32_WINNT 0x0400
#define _ATL_APARTMENT_THREADED

#include "vcproj.h"
extern TCHAR g_szStart[];
extern TCHAR g_szEnd[];
extern TCHAR g_szAll[];
extern char szAppName[];
LPTSTR   GetStringRes (int id);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CDocument::~CDocument()
{

}


BOOL CDocument::Initialize(HWND hwndDoc)
{
    m_hWnd = hwndDoc;
    return m_ClipList.Initialize(hwndDoc);
}

void inline WriteLong(IStream *pStream, long val)
{
    pStream->Write(&val, sizeof(val), NULL);
}

long inline ReadLong(IStream *pStream)
{
    long val;
    pStream->Read(&val, sizeof(val), NULL);
    return val;
}

void CDocument::ResetContents(void)
{
    m_TargetFileName.Empty();
    m_VideoCodecDisplayName.Empty();
    m_AudioCodecDisplayName.Empty();
    m_ClipList.ResetContents();
}

BOOL CDocument::ReadFromStream(IStream * pStream)
{
    ResetContents();
      
    m_TargetFileName.ReadFromStream(pStream);
    m_VideoCodecDisplayName.ReadFromStream(pStream);
    m_AudioCodecDisplayName.ReadFromStream(pStream);
    m_Height = ReadLong(pStream);
    m_Width = ReadLong(pStream);
    m_PixelDepth = ReadLong(pStream);

    m_ClipList.ReadFromStream(pStream);
    return TRUE;
}

BOOL CDocument::WriteToStream(IStream * pStream)
{
    m_TargetFileName.WriteToStream(pStream);
    m_VideoCodecDisplayName.WriteToStream(pStream);
    m_AudioCodecDisplayName.WriteToStream(pStream);
    WriteLong(pStream, m_Height);
    WriteLong(pStream, m_Width);
    WriteLong(pStream, m_PixelDepth);
    m_ClipList.WriteToStream(pStream);
    return TRUE;
}


BOOL CDocument::NewClip()
{
    CClip *pClip;
    int i = m_ClipList.CurSelClipIndex();
    if (i < 0) {    // Nothing selected = -1;  Add to end.
        i = m_ClipList.NumClips();
    }
    m_ClipList.AddClip(i, &pClip);
    if (pClip->DoSettingsDialog(m_hWnd)) {
        m_ClipList.UpdateClipView(i);
        MarkDirty();
    } else {
        m_ClipList.DeleteClip(i);
    }
    return TRUE;
}

BOOL CDocument::DeleteClip()
{
    int i = m_ClipList.CurSelClipIndex();
    if (i >= 0) {
        m_ClipList.DeleteClip(i);
        MarkDirty();
    }
    return TRUE;
}

BOOL CDocument::EditClip()
{
    int i = m_ClipList.CurSelClipIndex();
    if (i >= 0) {
        CClip *pClip = m_ClipList.GetClip(i);
        if (pClip->DoSettingsDialog(m_hWnd)) {
            m_ClipList.UpdateClipView(i);
            MarkDirty();
        }
    }
    return TRUE;
}

extern HINSTANCE hInst;



void InitOpenFileName(HWND hWnd, OPENFILENAME& OpenFileName, TCHAR * pszFileName)
{
    OpenFileName.lStructSize       = sizeof(OPENFILENAME);
    OpenFileName.hwndOwner         = hWnd;
    OpenFileName.hInstance         = hInst;
    OpenFileName.lpstrFilter       = NULL;
    OpenFileName.lpstrCustomFilter = NULL;
    OpenFileName.nMaxCustFilter    = 0;
    OpenFileName.nFilterIndex      = 0;
    OpenFileName.lpstrFile         = pszFileName;
    OpenFileName.nMaxFile          = MAX_PATH;
    OpenFileName.lpstrFileTitle    = NULL;
    OpenFileName.nMaxFileTitle     = 0;
    OpenFileName.lpstrInitialDir   = NULL;
    OpenFileName.lpstrTitle        = NULL;
    OpenFileName.nFileOffset       = 0;
    OpenFileName.nFileExtension    = 0;
    OpenFileName.lpstrDefExt       = NULL;
    OpenFileName.lCustData         = 0;
    OpenFileName.lpfnHook 	   = NULL;
    OpenFileName.lpTemplateName    = NULL;
    OpenFileName.Flags             = OFN_SHOWHELP;
}


BOOL CDocument::OpenFile()
{
    USES_CONVERSION;
    BOOL bWorked = FALSE;
    TCHAR szFile[MAX_PATH];

    szFile[0] = 0;
    OPENFILENAME OpenFileName;
    InitOpenFileName(m_hWnd, OpenFileName, szFile);

    if (GetOpenFileName(&OpenFileName)) {
        CComBSTR szTryThisName(T2BSTR(szFile));
        CComPtr<IStorage> pStorage;
        HRESULT hr = StgOpenStorage(szTryThisName, NULL,  
                                    STGM_READ | STGM_SHARE_EXCLUSIVE, NULL, 0,
                                    &pStorage);
        if (SUCCEEDED(hr)) {
            CComPtr <IStream> pStream;
            hr = pStorage->OpenStream(L"VidClip", NULL, STGM_READ | STGM_SHARE_EXCLUSIVE, 0, &pStream);
            if (SUCCEEDED(hr)) {
                bWorked = ReadFromStream(pStream);
                if (bWorked) {
                    m_DocumentFileName = szTryThisName;
                }
            }
        }
    }
    return bWorked;
}


BOOL CDocument::SaveAsFile(bool bShowDialog)
{
    USES_CONVERSION;
    BOOL bWorked = FALSE;
    TCHAR szFile[MAX_PATH];

    if (!m_DocumentFileName || bShowDialog) {
        OPENFILENAME OpenFileName;
        InitOpenFileName(m_hWnd, OpenFileName, szFile);
        if (m_DocumentFileName) {
            lstrcpy(szFile, OLE2T(m_DocumentFileName));    
        } else {
            szFile[0] = 0;
        }
        if (!GetSaveFileName(&OpenFileName)) {
            return FALSE;
        }
        m_DocumentFileName = T2OLE(szFile);
    }

    CComPtr<IStorage> pStorage;
    HRESULT hr = StgCreateDocfile(m_DocumentFileName, 
                 STGM_READWRITE | STGM_CREATE | STGM_SHARE_EXCLUSIVE, 0,
                 &pStorage);
    if (SUCCEEDED(hr)) {
        CComPtr<IStream> pStream;
        hr = pStorage->CreateStream(L"VidClip", STGM_READWRITE | STGM_CREATE | STGM_SHARE_EXCLUSIVE, 0, 0, &pStream);
        if (SUCCEEDED(hr)) {
            bWorked = WriteToStream(pStream);
        }
    }
    return bWorked;
}



//////////////////// CLIPS


void inline WriteStreamTime(IStream *pStream, STREAM_TIME val)
{
    pStream->Write(&val, sizeof(val), NULL);
}

STREAM_TIME inline ReadStreamTime(IStream *pStream)
{
    STREAM_TIME val;
    pStream->Read(&val, sizeof(val), NULL);
    return val;
}



BOOL CClip::WriteToStream(IStream *pStream)
{
    m_FileName.WriteToStream(pStream);
    WriteStreamTime(pStream, m_stStart);
    WriteStreamTime(pStream, m_stEnd);
    return TRUE;
}

BOOL CClip::ReadFromStream(IStream *pStream)
{
    m_FileName.Empty();
    m_FileName.ReadFromStream(pStream);
    m_stStart = ReadStreamTime(pStream);
    m_stEnd = ReadStreamTime(pStream);
    return TRUE;
}

//////////////////// CLIP LIST

void CClipList::InsertCol(int iColNum, TCHAR * pszColHeader, int Width)
{
    LV_COLUMN col = {LVCF_FMT | LVCF_WIDTH | LVCF_TEXT, LVCFMT_LEFT, Width, pszColHeader, 0, 0};
    ListView_InsertColumn(m_hLV, iColNum, &col);
}

BOOL CClipList::Initialize(HWND hwndParent)
{
    m_hLV = CreateWindowEx(0, WC_LISTVIEW, "ClipList", WS_VISIBLE | WS_CHILDWINDOW | WS_CLIPSIBLINGS | LVS_REPORT | LVS_SINGLESEL,
                           0, 0, 0, 0, hwndParent, (HMENU)1, hInst, NULL);


    InsertCol(0, "File Name", 400);
    InsertCol(1, "Start", 100);
    InsertCol(2, "Stop", 100);
    return TRUE;
}


int CClipList::NumClips(void)
{
    return ListView_GetItemCount(m_hLV);
}

int CClipList::CurSelClipIndex(void)
{
    return ListView_GetNextItem(m_hLV, -1, LVNI_ALL | LVNI_SELECTED);
}

CClip * CClipList::GetClip(int i)
{
    LV_ITEM lvi = {LVIF_PARAM, i, 0};
    ListView_GetItem(m_hLV, &lvi);
    return (CClip *)(lvi.lParam);
}

BOOL CClipList::AddClip(int i, CClip **ppClip)
{
    CClip *pNewClip = new CClip();
    if (!pNewClip) {
        return FALSE;
    }
    LV_ITEM lvi = {LVIF_PARAM | LVIF_TEXT, i, 0};
    lvi.pszText = "<New Clip>";
    lvi.lParam = (LPARAM)pNewClip;
    ListView_InsertItem(m_hLV, &lvi);
    lvi.pszText = "----";
    lvi.mask = LVIF_TEXT;
    lvi.iSubItem = 1;
    ListView_SetItem(m_hLV, &lvi);
    lvi.iSubItem = 2;
    ListView_SetItem(m_hLV, &lvi);
    *ppClip = pNewClip;

    return TRUE;
}

void CClipList::DeleteClip(int i)
{
    delete GetClip(i);
    ListView_DeleteItem(m_hLV, i);
}


void CClipList::UpdateClipView(int i)
{
    USES_CONVERSION;
    CClip *pClip = GetClip(i);

    LV_ITEM lvi = {LVIF_TEXT, i, 0};
    lvi.pszText = OLE2T(pClip->m_FileName);
    ListView_SetItem(m_hLV, &lvi);


    if (pClip->m_stStart == 0 && pClip->m_stEnd == 0) {
        lvi.iSubItem = 1;
        lvi.pszText = g_szAll;
        ListView_SetItem(m_hLV, &lvi);
        lvi.iSubItem = 2;
        lvi.pszText = _T("");
        ListView_SetItem(m_hLV, &lvi);
    } else {
        TCHAR s[20];
        long Second = (long)(pClip->m_stStart / (STREAM_TIME)10000000);
        wsprintf(s, _T("%li"), Second);
        lvi.iSubItem = 1;
        lvi.pszText = pClip->m_stStart == 0 ? g_szStart : s;
        ListView_SetItem(m_hLV, &lvi);

        Second = (long)(pClip->m_stEnd / (STREAM_TIME)10000000);
        wsprintf(s, _T("%li"), Second);
        lvi.iSubItem = 2;
        lvi.pszText = pClip->m_stEnd == 0 ? g_szEnd : s;
        ListView_SetItem(m_hLV, &lvi);
    }
}



BOOL CClipList::WriteToStream(IStream *pStream)
{
    int lNumClips = NumClips();
    WriteLong(pStream, lNumClips);
    for (int i = 0; i < lNumClips; i++) {
        GetClip(i)->WriteToStream(pStream);
    }
    return TRUE;

}

void CClipList::ResetContents()
{
    int lNumClips = NumClips();
    for (int i = 0; i < lNumClips; i++) {
        delete GetClip(i);
    }
    ListView_DeleteAllItems(m_hLV);
}

BOOL CClipList::ReadFromStream(IStream *pStream)
{
    ResetContents();

    int lNumClips = ReadLong(pStream);
    for (int i = 0; i < lNumClips; i++) {
        CClip *pClip;
        AddClip(i, &pClip);
        pClip->ReadFromStream(pStream);
        UpdateClipView(i);
    }
    return TRUE;
}

void CClipList::SetSize(int Height, int Width)
{
    MoveWindow(m_hLV, 0, 0, Height, Width, TRUE);
}


LRESULT CALLBACK ClipSettingsDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);


BOOL CClip::DoSettingsDialog(HWND hwndParent)
{
    return DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_CLIP), hwndParent, (DLGPROC)ClipSettingsDlg, (LPARAM)this);
}


void inline SetEditSeconds(HWND hDlg, int ID, STREAM_TIME st)
{
    long Second = (long)(st / (STREAM_TIME)10000000);
    TCHAR s[20];
    wsprintf(s, _T("%li"), Second);
    SendDlgItemMessage(hDlg, ID, WM_SETTEXT, 0, (LPARAM)s);
}


void CClip::InitDialog(HWND hDlg)
{
    USES_CONVERSION; 
    CenterWindow (hDlg, GetWindow (hDlg, GW_OWNER));
    SendDlgItemMessage(hDlg, IDC_EDIT_FILENAME, WM_SETTEXT, 0, (LPARAM)W2T(m_FileName));
    SetEditSeconds(hDlg, IDC_EDIT_START, m_stStart);
    SetEditSeconds(hDlg, IDC_EDIT_END, m_stEnd);
}


bool CClip::CleanUpDialog(HWND hDlg, bool bSaveSettings)
{
    bool bWorked = true;
    if (bSaveSettings) {
        TCHAR szEditContents[MAX_PATH];
        TCHAR *pUnused;
        STREAM_TIME stStart, stEnd;

        SendDlgItemMessage(hDlg, IDC_EDIT_START, WM_GETTEXT, sizeof(szEditContents) / sizeof(TCHAR), (LPARAM)szEditContents);
        stStart = (STREAM_TIME)strtol(szEditContents, &pUnused, 10) * (STREAM_TIME)10000000;

        SendDlgItemMessage(hDlg, IDC_EDIT_END, WM_GETTEXT, sizeof(szEditContents) / sizeof(TCHAR), (LPARAM)szEditContents);
        stEnd = (STREAM_TIME)strtol(szEditContents, &pUnused, 10) * (STREAM_TIME)10000000;

        SendDlgItemMessage(hDlg, IDC_EDIT_FILENAME, WM_GETTEXT, sizeof(szEditContents) / sizeof(TCHAR), (LPARAM)szEditContents);

        if (stStart <= stEnd || stEnd == 0) {
            HANDLE hTest = CreateFile(szEditContents, GENERIC_READ, FILE_SHARE_READ, NULL, 
                                      OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (hTest == INVALID_HANDLE_VALUE) {
                bWorked = false;
                MessageBox(hDlg, GetStringRes(IDS_FILE_DOES_NOT_EXIST), szAppName, MB_OK);
            } else {
                CloseHandle(hTest);
                m_FileName = szEditContents;
                m_stStart = stStart;
                m_stEnd = stEnd;
            }
        } else {
            bWorked = false;
            MessageBox(hDlg, GetStringRes(IDS_INVALID_TIMES), szAppName, MB_OK);
        } 
    }
    return bWorked;
}

//
//  FUNCTION: ClipSettings(HWND, unsigned, WORD, LONG)
//
//
//  MESSAGES:
//
// WM_INITDIALOG - initialize dialog box
// WM_COMMAND    - Input received
//
//

LRESULT CALLBACK ClipSettingsDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    CClip *pClip = (CClip *)GetWindowLong(hDlg, DWL_USER);
    switch (message) {
        case WM_INITDIALOG:
            pClip = (CClip *)lParam;
            SetWindowLong(hDlg, DWL_USER, lParam);
            pClip->InitDialog(hDlg);
            return TRUE;

    

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
                if (pClip->CleanUpDialog(hDlg, LOWORD(wParam) == IDOK)) {
                    EndDialog(hDlg, LOWORD(wParam) == IDOK);
                }
                return TRUE;
            }
            if (LOWORD(wParam) == IDC_BUTTON_BROWSE) {
                TCHAR szFile[MAX_PATH];
                szFile[0] = 0;
                OPENFILENAME OpenFileName;
                InitOpenFileName(hDlg, OpenFileName, szFile);
                if (GetOpenFileName(&OpenFileName)) {
                    SendDlgItemMessage(hDlg, IDC_EDIT_FILENAME, WM_SETTEXT, 0, (LPARAM)szFile);
                }
                return TRUE;
            }
            break;
   }

   return FALSE;
}
