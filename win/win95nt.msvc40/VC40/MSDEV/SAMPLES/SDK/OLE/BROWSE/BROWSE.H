#ifdef WIN32

#ifdef UNICODE
    #define FROM_OLE_STRING(str) str
    #define TO_OLE_STRING(str) str 
#else
    #define FROM_OLE_STRING(str) ConvertToAnsi(str)
    char* ConvertToAnsi(OLECHAR FAR* szW);  
    #define TO_OLE_STRING(str) ConvertToUnicode(str)
    OLECHAR* ConvertToUnicode(char FAR* szA);   
    // Maximum length of string that can be converted between Ansi & Unicode
    #define STRCONVERT_MAXLEN 500         
#endif

#else  // WIN16
  #define APIENTRY far pascal  
  #define TCHAR char
  #define TEXT(sz) sz 
  #define FROM_OLE_STRING(str) str  
  #define TO_OLE_STRING(str) str 
  #define LPTSTR LPSTR   
  #define LPCTSTR LPCSTR
  
  // Windows NT defines the following in windowsx.h
  #define GET_WM_COMMAND_ID(w,l) (w)
  #define GET_WM_COMMAND_CMD(w,l) HIWORD(l)
  #define GET_WM_COMMAND_HWND(w,l) LOWORD(l)
#endif

// Function prototypes
int APIENTRY WinMain (HINSTANCE, HINSTANCE, LPSTR, int);
#ifdef WIN16
BOOL __export CALLBACK MainDialogFunc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
#else
BOOL CALLBACK MainDialogFunc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
#endif
 
typedef enum {
    TYPE_FUNCTION = 0,
    TYPE_PROPERTY = 1,
    TYPE_CONSTANT = 2,
    TYPE_PARAMETER = 3
} OBJTYPE;

class CBrowseApp
{
public:      
    void Init(HWND hwndMain);
    void Cleanup();    
    void ClearTypeLibStaticFields();   
    void ClearTypeInfoStaticFields();     
    void ClearElementStaticFields();
    void ClearParamStaticFields();
    HRESULT BrowseTypeLibrary();
    HRESULT ChangeTypeInfosSelection();
    HRESULT ChangeElementsSelection();
    HRESULT ChangeParametersSelection(); 
    void OpenElementHelpFile();   
    void EmptyList(HWND hwndList); 
    HRESULT LoadList(LPDISPATCH pdispItems, int nListID);  
    HRESULT TypeToString(LPDISPATCH pdispTypeDesc, LPTSTR pszTypeName);  
    void IDLFlagsToString(int n, LPTSTR psz);   
    void CallConvToString(int n, LPTSTR psz);
    void FuncKindToString(int n, LPTSTR psz);    
    void InvokeKindToString(int n, LPTSTR psz);  
    void VariantToString(VARIANT v, LPTSTR psz);

private:
   HINSTANCE m_hinst;           // App's HINSTANCE.   
   HWND m_hwndMain;             // App's main window which is a dialog..
   HFONT m_hfont;               // Non-bold font used for dialog fields.
   TCHAR m_szHelpFile[128];     // Help file used by type library being browsed.
   long m_lElemHelpCtx;         // Help context of TypeInfo element being browsed.
};    
