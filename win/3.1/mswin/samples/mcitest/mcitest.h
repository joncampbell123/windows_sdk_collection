/*
 * mcitest.h
 */


/* Resource IDs of the About box, main test box, and error box.
 */
#define IDD_ABOUTBOX            1
#define IDD_MCITEST             2
#define IDD_ERRORDLG            3
#define IDD_DEVICES             4

#define IDI_MCITEST             5
#define IDM_MCITEST             6
#define IDA_MCITEST             7

/* Controls for main dialog
 */
#define ID_INPUT             101
#define ID_OUTPUT            102
#define ID_ERRORCODE         103
#define ID_NOT_SUCCESS       104
#define ID_NOT_SUPER         105
#define ID_NOT_ABORT         106
#define ID_NOT_FAIL          107
#define ID_DEVICE_LIST       108
#define ID_END_DEVICE_LIST   109
#define ID_GO                200
#define ID_STEP              201
#define ID_EXIT              202
#define ID_MCISENDSTR        203
#define ID_MCIEXEC           204
#define ID_RUNCOUNT          205

/* Menu IDs of the various menu options.
 */
#define MENU_ABOUT              20
#define MENU_EXIT               21
#define MENU_START_TEST         22

#define MENU_OPEN               10
#define MENU_SAVE               11
#define MENU_SAVEAS             12

#define MENU_DEVICES            30


/* Function prototypes.
 */
int PASCAL WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
BOOL FAR PASCAL AboutDlgProc(HWND, unsigned, WORD, LONG);
PSTR PASCAL NEAR FileName(PSTR);
void PASCAL NEAR OpenMciFile(HWND, LPSTR);
int PASCAL NEAR get_number_of_devices(void);
void PASCAL NEAR update_device_list(void);
DWORD PASCAL NEAR sendstring(HWND, PSTR);
int PASCAL NEAR NDialog(int, HWND, FARPROC);
BOOL FAR PASCAL ErrDlgFunc(HWND, WORD, WORD, LONG);
void PASCAL NEAR execute(HWND, BOOL);
BOOL FAR PASCAL devices(HWND, WORD, WORD, LONG);
void PASCAL NEAR create_device_list(void);
BOOL FAR PASCAL mcitester(HWND, WORD, WORD, LONG);
BOOL PASCAL NEAR AppInit(HANDLE, HANDLE, LPSTR, WORD);
