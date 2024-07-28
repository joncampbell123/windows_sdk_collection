//===== DBWINLIB public declarations

BOOL WINAPI _export _loadds OutputInit(void);

// GetOutFlags values
#define DBOF_HASMONO        0x0001
#define DBOF_HASCOM1        0x0002
#define DBOF_HASCOM2        0x0004

UINT WINAPI _export _loadds GetOutputFlags(void);

// Output modes
#define OMD_NONE                0
#define OMD_BUFFER              1
#define OMD_COM1                2
#define OMD_COM2                3
#define OMD_MONO                4

BOOL WINAPI _export _loadds SetOutputMode(UINT mode);
UINT WINAPI _export _loadds GetOutputMode(void);

void WINAPI _export _loadds SetBufferNotify(HWND hwnd);

typedef struct tagOUTBUFINFO
{
    UINT cch;
    int cLines;
    LPCSTR lpch;
} OUTBUFINFO;

BOOL WINAPI _export _loadds GetOutputBufferInfo(OUTBUFINFO FAR* lpobi);
void WINAPI _export _loadds ResetBuffer(void);

#define WM_BUFFERNOTEMPTY       (WM_USER+1000)

BOOL WINAPI _export _loadds SetTaskFilter(LPCSTR lpszTaskName);
void WINAPI _export _loadds GetTaskFilter(LPSTR lpszTaskName, int cchMax);
