
#ifndef _WINUTB_H_ 
#define _WINUTB_H_

#include <windows.h>
#include <commctrl.h>
#include <stdlib.h>
#include <string.h>
#include <devioctl.h>

#include "..\usbloop.h"


#define MIN_PIPE 0
#define MAX_BUFFER_SIZE 80
#define MAX_LOOP 16383
#define DEFAULT_TRANSACT_BUFFER 256
#define EOL '\0'
#define USAGE_ERROR 80

char chBuff[MAX_BUFFER_SIZE];
HWND    hWnd1, hLogWindow;
HMENU 	hMenu1;
static char szAppName [] = "USB Loopback Test App";
static char szClassName[] = "LoopBackUSB";
char szDeviceName[16];
int iNumDevices;
int iDevice; 
HANDLE hUSBLOOP, hDevice;
int iInputBuffLen;
int iOutputBuffLen;
int iINpipe;
int iOUTpipe;
int iInstance; 
int iIterations;
ULONG ulNumberDevices;
HANDLE hCombo;
HANDLE hComboIn, hComboOut, hComboBox;
BOOLEAN bTestRunning;
ULONG ulCount;
BOOL bAutomate;
BOOL bAutoClose;
HINSTANCE hGlobalInstance;
ULONG displayFreq;
BOOL bAsyncTest;

int  DoMain(HANDLE hInstance);
void CleanUp(HWND hWnd);

	// in MISC.C
void DisplayOutput(LPSTR pString,...);
void OutputDeviceDesc(PUSB_DEVICE_DESCRIPTOR pUsbHandles);
void OutputConfigDesc(PUSB_CONFIGURATION_DESCRIPTOR pConfigDesc);
void OutputInterfaceDesc(PUSB_INTERFACE_DESCRIPTOR pIntfDesc);
void OutputEndPtDesc(PUSB_ENDPOINT_DESCRIPTOR pEndPtDesc, ULONG ulNum);
BOOL ProcessCmdLine(LPSTR pCmd);
void usage();
long ctlParseCmdLine( LPCSTR szCmdLine );

HANDLE Init(HANDLE hInstance, HANDLE hPrevInstance, LPSTR  lpszCmdLine, 
		int CmdShow);
LONG FAR PASCAL UTBTestApp (HWND hWnd,UINT msgID,WORD wParam,LONG lParam);
BOOL FAR PASCAL AboutDlgProc(HWND hDlg, UINT message,UINT wParam,LONG lParam);
BOOL FAR PASCAL SelectPipesDlg(HWND hDlg, UINT message,
							UINT wParam, LONG lParam);
BOOL FAR PASCAL NumberItsDlg(HWND hDlg, UINT message,
							UINT wParam, LONG lParam);					
BOOL FAR PASCAL NumInstDlg(HWND hDlg, UINT message,
							UINT wParam, LONG lParam);
BOOL FAR PASCAL DevHandlDlg(HWND hDlg, UINT message,
							UINT wParam, LONG lParam);
BOOL FAR PASCAL DisplayUsage(HWND hDlg, UINT message,
							UINT wParam, LONG lParam);
							
void ResetLoopMenu();

#define LoopBackUSB			500

#define IDM_OPEN_USBLOOP	10
#define IDM_DO_LOOPBACK		11
#define IDM_CLOSE_USBLOOP	12
#define IDM_STOP_LOOPBACK	13
#define IDM_GET_VERSION		14
#define IDM_EXITAPP			15

#define IDM_SHOW_DEV_DESC	21
#define IDM_SHOW_CONF_DESC	22
#define IDM_SHOW_INTF_DESC	23
#define IDM_SHOW_ENDP_DESC	24

#define IDM_ABOUT			30


#define IDD_INPUT_GROUP             101
#define IDD_INPUT_NUM               102
#define IDD_PIPE_TEXT               103
#define IDD_INPUT_BUFF_TEXT         104
#define IDD_INPUT_BUFF              105
#define IDD_INPUT_TYPE              106
#define IDD_INPUT_INTERRUPT         107
#define IDD_INPUT_BULK              108
#define IDD_INPUT_ISOCH             109
#define IDD_INPUT_NUM_TEXT          110
#define IDD_OUTPUT_GROUP            111
#define IDD_OUTPUT_NUM              112
#define IDD_OUTPUT_NUM_TEXT         113
#define IDD_OUTPUT_BUFF_TEXT        114
#define IDD_OUTPUT_BUFF             115
#define IDD_OUTPUT_TYPE             116
#define IDD_OUTPUT_INTERRUPT        117
#define IDD_OUTPUT_BULK             118
#define IDD_OUTPUT_ISOCH            119
#define IDD_ITERATIONS_TEXT         120
#define IDD_ITERATIONS              121
#define IDD_USB_DEVICE              122
#define IDD_USB_TEXT                124

#define IDD_NUMINST_TEXT	140
#define IDD_NUMINST			141

#define IDC_LOOP_ICON		200

#endif
