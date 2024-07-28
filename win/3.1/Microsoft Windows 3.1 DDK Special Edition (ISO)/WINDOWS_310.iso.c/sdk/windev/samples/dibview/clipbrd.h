
/* Macros to display/remove hourglass cursor for lengthy operations */
#define StartWait() hcurSave = SetCursor(LoadCursor(NULL,IDC_WAIT))
#define EndWait()   SetCursor(hcurSave)

HANDLE     CopyHandle           (HANDLE h);
HBITMAP    CopyBitmap           (HBITMAP hbm);
HBITMAP    CropBitmap           (HBITMAP hbm, HPALETTE hPal, LPRECT lpRect, LPPOINT lpptSize);
HANDLE     RenderFormat         (HWND hWndClip, int cf, POINT ptDIBSize);
HANDLE     RealizeDibFormat     (DWORD biStyle, WORD biBits);
void       HandleCopyClipboard  (void);
void       HandlePasteClipboard (void);

