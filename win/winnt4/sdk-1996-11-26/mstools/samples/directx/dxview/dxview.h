//================================================================
// Defines
//================================================================

// Window Width and Height
#define DXView_WIDTH       640
#define DXView_HEIGHT      300

// Child controls
#define IDC_LV          0x2000
#define IDC_TV          0x2003

// Imagelist first and last icons
#define IDI_FIRSTIMAGE  IDI_DIRECTX
#define IDI_LASTIMAGE   IDI_CAPSOPEN

#define IDC_SPLIT                       100

#define IDM_EXIT                        40001
#define IDM_ABOUT                       40002
#define IDM_VIEWAVAIL                   40003
#define IDM_VIEWALL                     40004

#define IDI_DIRECTX                     100
#define IDI_CAPS                        101
#define IDI_CAPSOPEN                    102

//================================================================
// Typedefs
//================================================================

typedef void (*SELCALLBACK)(LPARAM lParam1, LPARAM lParam2);

typedef struct
{
    SELCALLBACK Callback;
    LPARAM lParam1;
    LPARAM lParam2;
} NODEINFO;

typedef struct
{
    char *  szName;         // name of cap
    DWORD   dwOffset;       // offset to cap
    DWORD   dwFlag;         // bit flag for cal
} CAPDEF;

typedef struct
{
    char *szName;         // name of cap
    SELCALLBACK Callback;
    LPARAM lParam2;
} CAPDEFS;

typedef struct
{
        D3DDEVICEDESC d3dDeviceDesc;
        GUID  guid;
} CAP3DDEVICEDESC;

//================================================================
// Function prototypes
//================================================================

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

BOOL InitInstance(HINSTANCE hInstance, LPSTR lpCmdLine, int nCmdShow, int iWidth, int iHeight);

BOOL DXView_OnCreate(HWND hwnd);
void DXView_OnCommand(HWND hwnd, WPARAM wParam);
void DXView_OnSize(HWND hwnd);
void DXView_OnTreeSelect(HWND hwndTV, NM_TREEVIEW *ptv);
void DXView_OnListViewDblClick(HWND hwndLV, NM_LISTVIEW *plv);
void DXView_Cleanup(void);
void DXView_FillTree(HWND hwndTV);
BOOL DXView_InitImageList(void);

void LVAddColumn(HWND hwndLV, int i, char *name, int width);
int  LVAddText(HWND hwndLV, int col, char *sz, ...);
HTREEITEM TVAddNode(HTREEITEM hParent, char *szText, BOOL fKids, int iImage, SELCALLBACK Callback, LPARAM lParam1, LPARAM lParam2);

void DDAddCaps(LPARAM lParam1, LPARAM lParam2);
void DSAddCaps(LPARAM lParam1, LPARAM lParam2);
void DPAddCaps(LPARAM lParam1, LPARAM lParam2);
void D3AddCaps(LPARAM lParam1, LPARAM lParam2);
void DDAddVideoModes(LPARAM lParam1, LPARAM lParam2);
void DPAddSessions(LPARAM lParam1, LPARAM lParam2);
void DDFourCCFormat(LPARAM lParam1, LPARAM lParam2);

HRESULT CALLBACK D3EnumCallback(
            LPGUID pid,
            LPSTR lpDriverDesc,
            LPSTR lpDriverName, 
            LPD3DDEVICEDESC lpD3DDeviceDesc1, 
            LPD3DDEVICEDESC lpD3DDeviceDesc2, 
            LPVOID lpContext);
