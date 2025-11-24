//*************************************************************
//  File name: devmode.h
//
//  Description: 
//
//*************************************************************

// Typedef's for the shell & commctrl functions that we call explicitly
typedef HPROPSHEETPAGE (WINAPI* CREATEPROC)(LPPROPSHEETPAGE);

typedef int (WINAPI* PROPSHEETPROC)(LPCPROPSHEETHEADER);

// Identifiers for GetProcAddress stuff--use names for now, since the
// ordinals will probably change later on.
#define SHELL_PROPSHEET   "PropertySheet"
#define SHELL_CREATEPAGE  "CreatePropertySheetPage"
#define COMMCTRL_INIT     MAKEINTRESOURCE(17)


int   NEAR PASCAL EnterDevMode(HWND,HINSTANCE,LPENVIRONMENT,LPSTR,
                               LPSTR,LPENVIRONMENT,LPSTR,WORD,BOOL,
                               LPFNADDPROPSHEETPAGE,LPARAM);

UINT  FAR  PASCAL PropPageCallback(HWND,UINT,LPPROPSHEETPAGE);
int   NEAR PASCAL IEndDevMode(LPDI,LPSTR);
int   FAR  PASCAL ExtDeviceMode(HWND,HINSTANCE,LPENVIRONMENT,LPSTR,LPSTR,LPENVIRONMENT,LPSTR,WORD);
int   NEAR PASCAL ModifyDeviceMode(LPENVIRONMENT,LPENVIRONMENT);
int   NEAR PASCAL GetDeviceMode(LPENVIRONMENT,LPSTR,LPSTR,LPSTR);

/*** EOF: devmode.h ***/
