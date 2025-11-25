#define IDR_MENU1                       101
#define IDI_CUTLIST                     102
#define ID_FILE_EXIT                    40001
#define ID_CUTLIST_PLAY                 40002
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

#define HELPER_RELEASE(x) { if (x) x->Release(); x = NULL; }

#define APPLICATIONNAME "CLText"
#define CLASSNAME "CLText"

HWND            ghApp;
HMENU           ghMenu;
WORD            wDlgRes;
HINSTANCE       ghInst;
ClipCollection  gTheSet;            // Cutlist 
TCHAR           gszScratch[2048];   // General, scratch string (e.g. error msgs)
LONGLONG        glTotalLength;
UINT            gTimerNum;

#define MBOX(a) MessageBox(ghApp, a, APPLICATIONNAME, MB_OK)

