#define ISRM_RUPT WM_USER+255

int	FAR CDECL wsprintf(LPSTR lpszOut, LPCSTR lpszFmt, ...);
void FAR PASCAL Install_Handler();
void FAR PASCAL De_Install_Handler();
void FAR PASCAL SetISRWindow(HWND);
int  FAR PASCAL GetISRCount(void);
void FAR PASCAL Do_Int();

