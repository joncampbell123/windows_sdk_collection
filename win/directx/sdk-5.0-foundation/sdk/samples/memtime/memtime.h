/* Menu Items */
#define MENU_ABOUT       2
#define MENU_EXIT        4
#define MENU_OPEN        5
#define MENU_COPY        6
#define MENU_PASTE       7
#define MENU_CUT         8
#define MENU_TIMEALL 9
#define MENU_REALTIME 10

#define MENU_TIME       50


/****************************************************************************
****************************************************************************/
struct _timing_result;

typedef DWORD __cdecl MemFunc(PVOID pSource, PVOID pDest, DWORD Heigh, DWORD Width, DWORD Pitch, DWORD Count);

typedef struct _timing_result
{
        DWORD Time;
        MemFunc *pTimer;
        char const *pDescription;
        DWORD Iterations;
} TIMING_RESULT, *PTIMING_RESULT;



#ifdef DEBUG
    extern void FAR CDECL dprintf(LPSTR szFormat, ...);
    #define DPF dprintf
#else
    #define DPF ; / ## /
#endif
