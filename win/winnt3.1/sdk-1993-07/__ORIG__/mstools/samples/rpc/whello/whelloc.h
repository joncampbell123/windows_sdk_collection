#ifdef WIN16
#define APIENTRY         FAR PASCAL
#define UNREFERENCED_PARAMETER
#endif

#define IDM_ABOUT         100
#define IDM_SERVER        200
#define IDM_EXIT          300
#define IDM_ENDPOINT      400
#define IDM_SEND          500
#define IDM_BIND          600

#define IDD_SERVERNAME    201
#define IDD_ENDPOINTNAME  401
#define IDD_MESSAGE       501

#define CNLEN              15    /* computer name length */
#define UNCLEN        CNLEN+2    /* \\computername       */
#define PATHLEN           260    /* Path                 */
#define MSGLEN            300    /* arbitrary large number for message size */
#define MAXPROTSEQ         20    /* protocol sequence "ncacn_np" */       

#define DEFAULT_ENDPOINT    "\\pipe\\whello"                    
#define DEFAULT_PROT_SEQ    "ncacn_np"                         
#define DEFAULT_MESSAGE     "hello, world"

#define EXCEPT_MSG "The remote procedure call failed. \
Please make sure the server application is running and \
that the server name and endpoint name are correct."


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
BOOL InitApplication(HANDLE);
BOOL InitInstance(HANDLE, int);

LONG APIENTRY MainWndProc(HWND, UINT, UINT, LONG);
BOOL APIENTRY About(HWND, UINT, UINT, LONG);
BOOL APIENTRY Server(HWND, UINT, UINT, LONG);
BOOL APIENTRY Endpoint(HWND, UINT, UINT, LONG);
BOOL APIENTRY Send(HWND, UINT, UINT, LONG);
RPC_STATUS Bind(HWND);
