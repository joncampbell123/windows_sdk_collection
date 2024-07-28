#include "dbwindlp.h"

static char szTaskName[10];
static char szProcName[80];

int GetProcName(LPSTR lpsz, FARPROC lpfn)
{
    GLOBALENTRY ge;
    MODULEENTRY me;
    int cch;

    cch = 0;
    ge.dwSize = sizeof(ge);
    me.dwSize = sizeof(me);
    if (GlobalEntryHandle(&ge, (HGLOBAL)HIWORD(lpfn)) &&
            (ge.wType == GT_CODE) &&
            ModuleFindHandle(&me, ge.hOwner) )
    {
        cch = dbprintf(lpsz, "%s(%d)%x:%x", (LPSTR)me.szModule, ge.wData);
    }
    return cch + dbprintf(lpsz, "%x:%x", HIWORD(lpfn), LOWORD(lpfn));
}

int GetTaskName(LPSTR lpsz, HTASK htask)
{
    TASKENTRY te;

    te.dwSize = sizeof(te);
    if (TaskFindHandle(&te, htask))
        return dbprintf(lpsz, "%s", (LPSTR)te.szModule);
    else
        return dbprintf(lpsz, "%x", htask);
}

// FatalExit notification formatting
//
void FormatRip(LPSTR lpsz, NFYRIP FAR* lprip)
{
    GetTaskName(szTaskName, GetCurrentTask());

    dbprintf(lpsz, "%s: FatalExit(%x) at CS:IP=%x:%x SS:BP=%x:%x\r\n",
            (LPSTR)szTaskName,
            lprip->wExitCode,
            lprip->wCS, lprip->wIP,
            lprip->wSS, lprip->wBP);
}

// LogParamError notification formatting
//
#define FMT_WORD    0
#define FMT_DWORD   1
#define FMT_LP      2

struct TYPEMAP
{
    UINT err;
    char* szType;
    char* szFmt;
};

static char szParam[] = "parameter";
static char szD16[] = "%s %s: Invalid %s: %d\r\n";
static char szX16[] = "%s %s: Invalid %s: %x\r\n";
static char szX32[] = "%s %s: Invalid %s: %x%x\r\n";
static char szLP[]  = "%s %s: Invalid %s: %x:%x\r\n";

#define DEFMAP(err, type, fmt) \
    { err, type, fmt }

struct TYPEMAP typemap[] =
{
    DEFMAP(ERR_BAD_VALUE,        "value",        szD16),
    DEFMAP(ERR_BAD_INDEX,        "index",        szD16),
    DEFMAP(ERR_BAD_FLAGS,        "flags",        szX16),
    DEFMAP(ERR_BAD_SELECTOR,     "selector",     szX16),
    DEFMAP(ERR_BAD_DFLAGS,       "flags",        szX32),
    DEFMAP(ERR_BAD_DVALUE,       "value",        szX32),
    DEFMAP(ERR_BAD_DINDEX,       "index",        szX32),
    DEFMAP(ERR_BAD_PTR,          "pointer",      szLP),
    DEFMAP(ERR_BAD_FUNC_PTR,     "function pointer", szLP),
    DEFMAP(ERR_BAD_STRING_PTR,   "string pointer", szLP),
    DEFMAP(ERR_BAD_HINSTANCE,    "HINSTANCE",    szX16),
    DEFMAP(ERR_BAD_HMODULE,      "HMODULE",      szX16),
    DEFMAP(ERR_BAD_GLOBAL_HANDLE,"global handle", szX16),
    DEFMAP(ERR_BAD_LOCAL_HANDLE, "local handle", szX16),
    DEFMAP(ERR_BAD_ATOM,         "atom",         szX16),
    DEFMAP(ERR_BAD_HWND,         "HWND",         szX16),
    DEFMAP(ERR_BAD_HMENU,        "HMENU",        szX16),
    DEFMAP(ERR_BAD_HCURSOR,      "HCURSOR",      szX16),
    DEFMAP(ERR_BAD_HICON,        "HICON",        szX16),
    DEFMAP(ERR_BAD_GDI_OBJECT,   "HGDIOBJ",      szX16),
    DEFMAP(ERR_BAD_HDC,          "HDC",          szX16),
    DEFMAP(ERR_BAD_HPEN,         "HPEN",         szX16),
    DEFMAP(ERR_BAD_HFONT,        "HFONT",        szX16),
    DEFMAP(ERR_BAD_HBRUSH,       "HBRUSH",       szX16),
    DEFMAP(ERR_BAD_HBITMAP,      "HBITMAP",      szX16),
    DEFMAP(ERR_BAD_HRGN,         "HRGN",         szX16),
    DEFMAP(ERR_BAD_HPALETTE,     "HPALETTE",     szX16),
    DEFMAP(ERR_BAD_HANDLE,       "HANDLE",       szX16),
    DEFMAP(ERR_BAD_HFILE,        "HFILE",        szX16),
    DEFMAP(ERR_BAD_HMETAFILE,    "HMETAFILE",    szX16),
    DEFMAP(ERR_BAD_CID,          "CID",          szX16),
    DEFMAP(ERR_BAD_HDRVR,        "HDRVR",        szX16),
    DEFMAP(ERR_BAD_HDWP,         "HDWP",         szX16)
};

void FormatLogParamError(LPSTR lpsz, NFYLOGPARAMERROR FAR* lplpe)
{
    UINT err;
    char* pszFmt;
    char* pszType;
    int i;
    void FAR* FAR* param = lplpe->lpBadParam;

    pszFmt  = szX32;
    pszType = szParam;
    err = (lplpe->wErrCode & ~ERR_WARNING);
    for (i = 0; i < (sizeof(typemap) / sizeof(struct TYPEMAP)); i++)
    {
        if (typemap[i].err == err)
        {
            pszFmt = typemap[i].szFmt;
            pszType = typemap[i].szType;
            break;
        }
    }

    if (lplpe->wErrCode & ERR_WARNING)
    {
        lstrcpy(lpsz, "wn ");
        lpsz += 3;
    }
    else
    {
        lpsz = lstrcpy(lpsz, "err ");
        lpsz += 4;
    }

    GetTaskName(szTaskName, GetCurrentTask());
    GetProcName(szProcName, lplpe->lpfnErrorAddr);

    if (pszFmt == szLP)
        dbprintf(lpsz, pszFmt, (LPSTR)szTaskName, (LPSTR)szProcName, (LPSTR)pszType, SELECTOROF(param), OFFSETOF(param));
    else if (pszFmt == szX32)
        dbprintf(lpsz, pszFmt, (LPSTR)szTaskName, (LPSTR)szProcName, (LPSTR)pszType, (DWORD)param);
    else
        dbprintf(lpsz, pszFmt, (LPSTR)szTaskName, (LPSTR)szProcName, (LPSTR)pszType, (WORD)(DWORD)param);
}

// LogError notification formatting
//
struct ERRMAP
{
    UINT err;
    char* szErr;
};

struct ERRMAP errmap[] =
  {
    { ERR_GALLOC,     "GlobalAlloc failed" },
    { ERR_GREALLOC,   "GlobalReAlloc failed" },
    { ERR_GLOCK,      "GlobalLock failed" },
    { ERR_LALLOC,     "LocalAlloc failed" },
    { ERR_LREALLOC,   "LocalReAlloc failed" },
    { ERR_LLOCK,      "LocalLock failed" },
    { ERR_ALLOCRES,   "AllocResource failed" },
    { ERR_LOCKRES,    "LockResource failed" },
    { ERR_LOADMODULE, "LoadModule failed" },

    { ERR_CREATEDLG,  "CreateDialog() failed: Couldn't load menu or create window" },
    { ERR_CREATEDLG2, "CreateDialog() failed: Couldn't create window" },
    { ERR_REGISTERCLASS, "RegisterClass failed: Class already exists" },
    { ERR_DCBUSY,     "DC Cache full: Too many GetDC() calls" },
    { ERR_CREATEWND,  "CreateWindow failed: Window class not found" },
    { ERR_STRUCEXTRA, "Unallocated extra window/class word index used" },
    { ERR_LOADSTR,    "LoadString() failed" },
    { ERR_LOADMENU,   "LoadMenu() failed" },
    { ERR_NESTEDBEGINPAINT, "Nested BeginPaint() calls" },
    { ERR_BADINDEX,   "Invalid window word index value" },
    { ERR_CREATEMENU, "CreateMenu failed" },

    { ERR_CREATEDC,   "CreateDC failed" },
    { ERR_CREATEMETA, "CreateMetafile: Can't create metafile" },
    { ERR_SELBITMAP,  "Bitmap already selected" },
    { ERR_DELOBJSELECTED, "Selected object deleted" }
  };

#define ERR_SIZEMASK   (ERR_BYTE | ERR_WORD | ERR_DWORD)

void FormatLogError(LPSTR lpsz, NFYLOGERROR FAR* lple)
{
    int i;
    char* pszErr;
    UINT err;
    int cch;

    err = (lple->wErrCode & ~(ERR_WARNING | ERR_SIZEMASK));

    pszErr = "";
    for (i = 0; i < sizeof(errmap) / sizeof(struct ERRMAP); i++)
    {
        if (err == errmap[i].err)
        {
            pszErr = errmap[i].szErr;
            break;
        }
    }

    GetTaskName(szTaskName, GetCurrentTask());

    if (lple->wErrCode & ERR_WARNING)
    {
        cch = dbprintf(lpsz, "%s: Warning %x: %s",
                (LPSTR)szTaskName, lple->wErrCode, (LPSTR)pszErr);
    }
    else
    {
        cch = dbprintf(lpsz, "%s: Error %x: %s",
                (LPSTR)szTaskName, lple->wErrCode, (LPSTR)pszErr);
    }

    lpsz += cch;

    switch (lple->wErrCode & ERR_SIZEMASK)
    {
    case ERR_BYTE:
        dbprintf(lpsz, "%x\r\n", LOBYTE(LOWORD(lple->lpInfo)));
        break;
    case ERR_WORD:
        dbprintf(lpsz, "%x\r\n", LOWORD(lple->lpInfo));
        break;
    case ERR_DWORD:
        dbprintf(lpsz, "%x%x\r\n", HIWORD(lple->lpInfo), LOWORD(lple->lpInfo));
        break;
    default:
        dbprintf(lpsz, "\r\n");
    }
}

int _cdecl dbprintf(LPSTR lpszOut, LPCSTR lpszFmt, ...)
{
    LPSTR lpsz = lpszOut;
    BYTE FAR* lpArgs = (BYTE FAR*)(&lpszFmt + 1);
    char ch;

    while ((ch = *lpszFmt++) != 0)
    {
        if (ch != '%')
        {
            *lpsz++ = ch;
            continue;
        }
        switch (*lpszFmt++)
        {
        case 0:
            lpszFmt--;
            break;

        case '%':
            *lpsz++ = '%';
            break;

        case 'x':
            lpsz = HexOut(lpsz, *(WORD FAR*)lpArgs);
            lpArgs += sizeof(WORD);
            break;

        case 'd':
            lpsz = DecOut(lpsz, *(short FAR*)lpArgs);
            lpArgs += sizeof(WORD);
            break;

        case 's':
            {
                LPCSTR lpszSrc = *(LPCSTR FAR*)lpArgs;
                lpArgs += sizeof(LPCSTR);

                while (*lpszSrc)
                    *lpsz++ = *lpszSrc++;
            }
            break;
        }
    }
    *lpsz = 0;

    return lpsz - lpszOut;
}

LPSTR HexOut(LPSTR lpch, WORD value)
{
    int i;
    static WORD wP16[] = { 0x1000, 0x0100, 0x0010, 0x0001 };

    for (i = 0; i != 4; i++)
    {
        char ch = (char)((value / wP16[i]) & 0x0f) + (char)'0';
        if (ch > '9')
            ch += 'A' - '0' - 10;
        *lpch++ = ch;
    }
    return lpch;
}

LPSTR DecOut(LPSTR lpch, short value)
{
    int i;
    static short sP10[] = { 10000, 1000, 100, 10, 1 };
    BOOL fDigWritten = FALSE;

    if (value < 0)
    {
        *lpch++ = '-';
        value = -value;
    }

    for (i = 0; i != 5; i++)
    {
        char ch = (char)((value / sP10[i]) % 10) + (char)'0';
        if (ch != '0' || i == 4 || fDigWritten)
        {
            fDigWritten = TRUE;
            *lpch++ = ch;
        }
    }
    return lpch;
}
