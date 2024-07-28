/* Control I.D.'s */

#define IDC_ITEM1   1
#define IDC_ITEM2   2
#define IDC_ITEM3   3

/* User-defined messages */

#define WM_USER_SET_DOC_WND_SIZE (WM_USER+1)

/* Maximum values */

#define APP_MAX_SIZE          8
#define TOPIC_MAX_SIZE        8
#define ITEM_NAME_MAX_SIZE    8
#define ITEM_VALUE_MAX_SIZE   8
#define EXECUTE_STRING_MAX_SIZE 100
#define CONV_MAX_COUNT	      10
#define ADVISE_MAX_COUNT      30

/* Global data */

HWND   hwndMain;
HANDLE hInst;
int    nDoc;
char   szDocName[10];
HANDLE hClipboardData;
int    cfLink;

/* External procedures in SERVDDE.C */

long FAR PASCAL DDEWndProc(HWND, UINT, WPARAM, LPARAM);
void InitAckTimeOut(void);
void SendData(HWND, HWND, char *, char *, BOOL, BOOL, BOOL);
void SendTerminate(HWND, HWND);
void ServerAcknowledge(HWND, HWND, LONG);
void ServerAdvise(HWND, HWND, LONG);
void ServerExecute(HWND, HWND, HANDLE);
void ServerInitiate(HWND, LONG);
void ServerPoke(HWND, HWND, LONG);
void ServerRequest(HWND, HWND, LONG);
void ServerTerminate(HWND, HWND);
void ServerUnadvise(HWND, HWND, LONG);
void TerminateConversations(void);


/* External procedures in SERVDATA.C */

BOOL AddAdvise(HWND, HANDLE, ATOM, int);
BOOL AddConv(HWND, HWND);
BOOL AtLeastOneConvActive(void);
void CheckOutSentData(HWND, int, ATOM, HANDLE);
void DoEditCopy(int);
BOOL GetAdviseData(HWND, int, char*, char*, BOOL*, BOOL*);
HWND GetHwndClientDDE(HWND);
HWND GetNextAdvise(HWND, int);
HWND GetNextConv(HWND);
void GlobalFreeSentData(HWND, int);
BOOL IsConvInTerminateState(HWND);
void RemoveConv(HWND);
BOOL RemoveAdvise(HWND, int);
void SetConvInTerminateState(HWND);

