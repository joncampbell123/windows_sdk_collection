#ifndef _INC_DLG
#define _INC_DLG

// Public declarations

int SampleDlg_Do(HWND hwndOwner);

// Private declarations

// Dialog procedure

BOOL CALLBACK _export SampleDlg_OldDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT SampleDlg_DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Default message handler

LRESULT SampleDlg_DefProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Message handlers

BOOL SampleDlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
void SampleDlg_OnCommand(HWND hwnd, UINT id, HWND hwndCtl, UINT codeNotify);
BOOL SampleDlg_OnMsgFilter(HWND hwnd, MSG FAR* lpmsg, int context);

#endif  // !_INC_DLG
