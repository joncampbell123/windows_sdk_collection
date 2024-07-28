/* BOUNCER.H - Header file for BOUNCER sample screen saver application.
 */

#include <scrnsave.h>

#define ID_BITMAP   100
#define ID_SPEED    101
#define ID_XPOS     102
#define ID_YPOS     103
#define ID_VELOCITY 104
#define ID_GRAVITY  105
#define ID_SOUND    106
#define ID_PAUSE    107
#define ID_PASSWORDPROTECTED        108
#define ID_SETPASSWORD              109
#define ID_HELP                     110

#define ID_TIMER 200

#define DEF_SPEED 50                    // Timer period in milliseconds
#define DEF_INIT_XPOS -110              // Starting x 
#define DEF_INIT_YPOS 0                 // Starting y
#define DEF_INIT_VELOC 5                // Starting velocity
#define DEF_INIT_GRAVITY 3              // Starting gravity
#define DEF_SOUND TRUE                  // Starting sound state
#define DEF_PAUSE TRUE                  // Starting bottom-pause state

/* Function prototypes
 */
void GetIniEntries(void);
void MoveImage(HWND hWnd);
void GetIniSettings(void);
void WriteProfileInt(LPSTR key, LPSTR tag, int i);
extern BOOL FAR PASCAL ScreenSaverConfigureDialog(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
