#define IDR_MENU1                       101
#define IDI_CUTLIST                     102
#define IDD_MEDIATIMES                  103
#define IDOKTIMES                       104
#define IDD_ABOUT                       105
#define IDD_LESSTHAN2                   106
#define IDC_TRIMIN                      1001
#define IDC_TRIMOUT                     1002
#define IDC_TRIMIN2                     1003
#define IDC_TRIMOUT2                    1004
#define ID_FILE_EXIT                    40001
#define IDM_ADDFILE                     40002
#define IDM_RUN                         40003
#define ID_FILE_ABOUT                   40004
#define IDC_STATIC			                -1

#define MAX_CLIPS 150
#define SCALE 10000000   // scale for 1 second of reference time

// Forward prototyping
void TearDownTheGraph (void);

// clip (element) details
struct ClipDetails  
  {   
    TCHAR szFilename[MAX_PATH];   // name of file containing clip
    REFERENCE_TIME start;         // Start (Trim In) position of clip within file
    REFERENCE_TIME stop;          // Stop (Trim Out) position of clip within file
  }; 

// cutlist is a collection of clips (elements)
struct ClipCollection
  { 
    int nNumClips;
    ClipDetails List[MAX_CLIPS];
  }; 

#define WM_GRAPHNOTIFY  WM_USER+13

#define HELPER_RELEASE(x) { if (x) x->Release(); x = NULL; }

#define APPLICATIONNAME "Simple Cutlist"
#define CLASSNAME "SimpleCutList"

HWND            ghApp;
WORD            wDlgRes;
HINSTANCE       ghInst;
ClipCollection  gTheSet;            // Cutlist 
TCHAR           gszScratch[2048];   // General, scratch string (e.g. error msgs)

