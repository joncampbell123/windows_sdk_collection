#include "vcproj.h"



// Local Header Files
#include "vidclip.h"
#include "resource.h"
#include "atlbase.h"
extern CComModule _Module;
#include "atlcom.h"
#include "strmif.h"
#include "uuids.h"
#include "document.h"


class CSettings
{
public:
    CSettings(CDocument *pDoc) : m_pDocument(pDoc) {};
    void Init(HWND hDlg);
    void FillCompressorList(HWND hWndCombo, REFGUID CatGuid, ICreateDevEnum *pCreateDevEnum, CComBSTR& szDisplayName);
    void CleanCompressorList(HWND hWndCombo, LPOLESTR * ppDisplayName);
    void CleanUp(HWND hDlg, bool bSave);

    CDocument * m_pDocument;
};

void GetMonikerName(IMoniker *pM, LPOLESTR *ppDisplayName)
{
    CComPtr <IBindCtx> pBindCtx;
    CreateBindCtx(0, &pBindCtx);
    pM->GetDisplayName(pBindCtx, NULL, ppDisplayName);
}


void CSettings::FillCompressorList(
    HWND hWndCombo,
    REFGUID CatGuid,
    ICreateDevEnum *pCreateDevEnum,
    CComBSTR &szDisplayName)
{
    CComPtr <IMalloc> pMalloc;
    CoGetMalloc(1, &pMalloc);
    bool bFoundPrevSelection = false;
    IEnumMoniker *pEm;
    HRESULT hr = pCreateDevEnum->CreateClassEnumerator(
        CatGuid,
        &pEm,
        0);
    if(hr == S_OK)
    {
        // enumerator returns S_FALSE if the category is empty
        ULONG cFetched;
        IMoniker *pM;
        while(hr = pEm->Next(1, &pM, &cFetched),
              hr == S_OK)
        {
            IPropertyBag *pPropBag;
            hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag);
            if(SUCCEEDED(hr))
            {
                VARIANT var;
                var.vt = VT_BSTR;
                hr = pPropBag->Read(L"FriendlyName", &var, 0);
                if(SUCCEEDED(hr))
                {
                    TCHAR szString[MAX_PATH];
                    WideCharToMultiByte(
                        CP_ACP, 0, var.bstrVal, -1,
                        szString, sizeof(szString), 0, 0);

                    int ID = SendMessage(hWndCombo, CB_ADDSTRING, 0, (LPARAM)&szString);
                    SendMessage(hWndCombo, CB_SETITEMDATA, ID, (LPARAM)pM);
                    SysFreeString(var.bstrVal);
                    if (!bFoundPrevSelection && szDisplayName) {
                        LPOLESTR pThisDevName;
                        GetMonikerName(pM, &pThisDevName);
                        if (pThisDevName) {
                            if (wcscmp(szDisplayName, pThisDevName) == 0) {
                                bFoundPrevSelection = true;
                                SendMessage(hWndCombo, CB_SETCURSEL, ID, 0);
                            }
                            pMalloc->Free(pThisDevName);
                        }
                    }
                }
                pPropBag->Release();
            }

            /// pM->Release(); -- Will do this later!
        }
        pEm->Release();
    }
    static TCHAR szNone[] = "<None>";
    int ID = SendMessage(hWndCombo, CB_ADDSTRING, 0, (LPARAM)&szNone);
    if (!bFoundPrevSelection) {
        SendMessage(hWndCombo, CB_SETCURSEL, ID, 0);
    }
}


void inline SetEditLong(HWND hDlg, int ID, long val)
{
    TCHAR s[20];
    wsprintf(s, _T("%li"), val);
    SendDlgItemMessage(hDlg, ID, WM_SETTEXT, 0, (LPARAM)s);
}


void CSettings::Init(HWND hDlg)
{
    USES_CONVERSION; 
    CComPtr <ICreateDevEnum> pCreateDevEnum;    
    HRESULT hr = CoCreateInstance(
        CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
        IID_ICreateDevEnum, (void**)&pCreateDevEnum);
    if(SUCCEEDED(hr)) {
        FillCompressorList(GetDlgItem(hDlg, IDC_COMBO_VIDCOMP),
                           CLSID_VideoCompressorCategory,
                           pCreateDevEnum,
                           m_pDocument->m_VideoCodecDisplayName);
        FillCompressorList(GetDlgItem(hDlg, IDC_COMBO_AUDIOCOMP),
                           CLSID_AudioCompressorCategory,
                           pCreateDevEnum,
                           m_pDocument->m_AudioCodecDisplayName);
    }
    CheckRadioButton(hDlg, IDC_RADIO16BIT, IDC_RADIO24BIT, IDC_RADIO16BIT + m_pDocument->m_PixelDepth);
    SendDlgItemMessage(hDlg, IDC_EDIT_FILENAME, WM_SETTEXT, 0, (LPARAM)W2T(m_pDocument->m_TargetFileName));
    SetEditLong(hDlg, IDC_HEIGHT, m_pDocument->m_Height);
    SetEditLong(hDlg, IDC_WIDTH, m_pDocument->m_Width);
}


void CSettings::CleanCompressorList(HWND hWndCombo, LPOLESTR * ppDisplayName)
{
    *ppDisplayName = NULL;
    int IDSelected = SendMessage(hWndCombo, CB_GETCURSEL, 0, 0);
    if (IDSelected != CB_ERR) {
        IMoniker *pMoniker = (IMoniker *)SendMessage(hWndCombo, CB_GETITEMDATA, IDSelected, 0);
        if (pMoniker) {
            GetMonikerName(pMoniker, ppDisplayName);
        }
    }
    int NumItems = SendMessage(hWndCombo, CB_GETCOUNT, 0, 0);
    for (int i = 0; i < NumItems; i++) {
        IMoniker *pMoniker = (IMoniker *)SendMessage(hWndCombo, CB_GETITEMDATA, i, 0);
        if (pMoniker) {
            pMoniker->Release();
        }
    }
}

void CSettings::CleanUp(HWND hDlg, bool bSave)
{
    USES_CONVERSION;
    WCHAR *pVideoName = NULL;
    WCHAR *pAudioName = NULL;
    CleanCompressorList(GetDlgItem(hDlg, IDC_COMBO_VIDCOMP), &pVideoName);
    CleanCompressorList(GetDlgItem(hDlg, IDC_COMBO_AUDIOCOMP), &pAudioName);
    if (bSave) {
        m_pDocument->m_VideoCodecDisplayName = pVideoName;
        m_pDocument->m_AudioCodecDisplayName = pAudioName;
    }
    CComPtr <IMalloc> pMalloc;
    CoGetMalloc(1, &pMalloc);
    pMalloc->Free(pVideoName);
    pMalloc->Free(pAudioName);
    if (bSave) {
        TCHAR szEditContents[MAX_PATH];
        TCHAR *pUnused;
        m_pDocument->MarkDirty();
        SendDlgItemMessage(hDlg, IDC_EDIT_FILENAME, WM_GETTEXT, sizeof(szEditContents) / sizeof(TCHAR), (LPARAM)szEditContents);
        m_pDocument->m_TargetFileName = szEditContents;

        SendDlgItemMessage(hDlg, IDC_HEIGHT, WM_GETTEXT, sizeof(szEditContents) / sizeof(TCHAR), (LPARAM)szEditContents);
        m_pDocument->m_Height = strtol(szEditContents, &pUnused, 10);
        SendDlgItemMessage(hDlg, IDC_WIDTH, WM_GETTEXT, sizeof(szEditContents) / sizeof(TCHAR), (LPARAM)szEditContents);
        m_pDocument->m_Width = strtol(szEditContents, &pUnused, 10);

        m_pDocument->m_PixelDepth = IsDlgButtonChecked(hDlg, IDC_RADIO16BIT) ? 0 : 1;
        
    }
}


void DoSettingsDialog(HINSTANCE hInst, HWND hWnd, CDocument *pDoc)
{
    CSettings Settings(pDoc);
    DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_SETTINGS), hWnd, (DLGPROC)SettingsDlg, (LPARAM)&Settings);
}

//
//  FUNCTION: Settings(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for "About" dialog box
//       This version allows greater flexibility over the contents of the 'About' box,
//       by pulling out values from the 'Version' resource.
//
//  MESSAGES:
//
// WM_INITDIALOG - initialize dialog box
// WM_COMMAND    - Input received
//
//

LRESULT CALLBACK SettingsDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    CSettings *pSettings = (CSettings *)GetWindowLong(hDlg, DWL_USER);
    switch (message) {
        case WM_INITDIALOG:
            pSettings = (CSettings *)lParam;
            SetWindowLong(hDlg, DWL_USER, lParam);
            pSettings->Init(hDlg);
            return TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
                pSettings->CleanUp(hDlg, LOWORD(wParam) == IDOK);
                EndDialog(hDlg, TRUE);
                return TRUE;
            }
            break;
   }

   return FALSE;
}
