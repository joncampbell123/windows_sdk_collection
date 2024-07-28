/* Maximum sizes */

#define APP_MAX_SIZE		 32
#define TOPIC_MAX_SIZE		 32
#define ITEM_MAX_SIZE		 8
#define VALUE_MAX_SIZE           32
#define EXECUTE_STRING_MAX_SIZE  100
#define CONV_MAX_COUNT		 8
#define ITEMS_PER_CONV_MAX_COUNT 5
#define CONVINFO_MAX_SIZE (APP_MAX_SIZE+TOPIC_MAX_SIZE+ITEM_MAX_SIZE+22)

/* Global data */

HANDLE	hInst;
HWND	hwndMain;
int     xDelta, yDelta;
int     nHorzRes, nVertRes;
WORD	cfLink;

/* Typdef's shared across modules */

typedef enum PENDINGACK
{
    NONE,
    ADVISE,
    UNADVISE,
    POKE,
    REQUEST,
    EXECUTE,
};

/* External procedures in CLIENT.C */

BOOL IsInRequestDlg(void);
void RequestSatisfied(LPSTR);

/* External procedures in CLIDATA.C */

BOOL AddItemToConv(HWND, char*);
BOOL AddConv(HWND, HWND, char*, char*);
BOOL AtLeastOneConvActive(void);
BOOL DoesAdviseAlreadyExist(HWND, char*);
HWND FindConvGivenAppTopic(char *, char*);
void GetAppAndTopic(HWND, char*, char*);
HWND GetHwndServerDDE(HWND);
HWND GetNextConv(HWND);
enum PENDINGACK GetConvPendingAck(HWND);
int  HexToInt(char *);
void InitDataTextMetrics(void);
BOOL IsConvInTerminateState(HWND, HWND);
BOOL IsHwndClientDDEUsed(HWND);
BOOL LetUserPickConversation(HWND);
void PaintConvData(HWND);
BOOL RemoveItemFromConv(HWND, char*);
BOOL RemoveConv(HWND, HWND);
void SetConvInTerminateState(HWND, HWND);
BOOL SetConvItemValue(HWND, char*, LPSTR);
void SetConvPendingAck(HWND, enum PENDINGACK);


/* External Procedures in CLIDDE.C */

void ClientAcknowledge(HWND, HWND, LONG, BOOL);
void ClientReceiveData(HWND, HWND, LONG);
void ClientTerminate(HWND, HWND);
long FAR PASCAL DDEWndProc(HWND, UINT, WPARAM, LPARAM);
void DoPasteLink(void);
void InitAckTimeOut(void);
void SendAdvise(HWND, HWND, char*);
void SendExecute(HWND, HWND, char*);
HWND SendInitiate(char *, char*);
void SendPoke(HWND, HWND, char*, char*);
void SendRequest(HWND, HWND, char*);
void SendTerminate(HWND, HWND);
void SendUnadvise(HWND, HWND, char*);
void TerminateConersations(void);

