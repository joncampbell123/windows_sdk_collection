/*-----------------------------------------------------------------

DDE include file

------------------------------------------------------------------*/
#include "error.inc"

/*
 * Constants 
 *
 */

/*--- DDE Messages ---*/
#define WM_DDE_INITIATE     	0x03e0
#define WM_DDE_TERMINATE    	0x03e1
#define WM_DDE_ADVISE	    	0x03e2
#define WM_DDE_UNADVISE     	0x03e3
#define WM_DDE_ACK	    	0x03e4
#define WM_DDE_DATA	    	0x03e5
#define WM_DDE_REQUEST	    	0x03e6
#define WM_DDE_POKE	    	0x03e7
#define WM_DDE_EXECUTE	    	0x03e8
#define DDE_INITIATE     	0
#define DDE_TERMINATE    	1
#define DDE_ADVISE	    	2
#define DDE_UNADVISE     	3
#define DDE_ACK	 	   	4
#define DDE_DATA	    	5
#define DDE_REQUEST	    	6
#define DDE_POKE	    	7
#define DDE_EXECUTE	    	8
#define NUM_DDE_MESSAGES	9

/*--- Mode flags ---*/
#define MF_ACKREQUIRED	    	0x8000
#define MF_RELEASE	    	0x2000
#define MF_ACKREL	    	MF_ACKREQUIRED | MF_RELEASE 
#define MF_NODATA	    	0x4000
#define MF_EVENT		0x0001

/*--- Macros ---*/
#define FIRST_MACROID	    	1024	 
#define EXECUTE_MACROID		1033   
#define ADD_MACRO	    	0
#define DELETE_MACRO	    	1
#define MODIFY_MACRO	    	2

/*--- Acknowledge ---*/
#define POSITIVE_ACK		0x8000
#define NEGATIVE_ACK		0x0000
#define ACK_BIT 		0x8000

/*--- Data Objects  ---*/
#define NUM_NAMED_OBJS		8
#define STATE_OBJ		0
#define INFO_OBJ		1
#define MACRO_LIST_OBJ		2
#define MENU_LIST_OBJ		3
#define WINDOW_INFO_OBJ		4
#define SYSITEMS_OBJ		5
#define TOPICS_OBJ		6
#define FORMATS_OBJ		7
#define NUM_UPDATE_OBJS		4
#define GMEM_OBJECT		GMEM_DDESHARE | GMEM_MOVEABLE | GMEM_ZEROINIT

/*--- ShellInfo attributes flags  ---*/
#define AF_SPECIFIC	    	0x0001
#define AF_TEMPORARY	   	0x0010
#define AF_NOTEMPS		0x0008

/*--- State levels ---*/
#define SF_NOT_STARTED		0	/* The App hasn't started 	*/ 
#define SF_RUNNING		1	/* The App is running 		*/
#define SF_TERMINATED		2	/* The App has terminated 	*/
#define SF_CLOSED		3	/* " recieved a valid CLOSE msg */
#define UF_STARTED		0	/* The user started the app 	*/
#define UF_SWITCHED_IN		1	/* The user switched in to the app */
#define UF_SWITCHED_OUT		2	/* The user switched out of the app */
#define UF_TERMINATED		3	/* The user terminated the app 	*/

/*--- WindowInfo actions ---*/
#define WI_NOACTION		0	
#define WI_SHOW			1	/* Show the window */
#define WI_HIDE			2	/* Hide the window */
#define WI_DEFAULTICON		4	/* Use the default icon */
#define WI_PASSEDICON		8	/* Use the passed icon */


/*--- Default Memory sizes ---*/
#define DM_MACRO		0
#define DM_MENU			0

/* 
 * Data Structures
 * 
 */
 
/*--- DDE conversation structures ---*/ 
typedef struct
    {
    WORD    wMode,
	    wFormat;
    }
    HEADER;
typedef HEADER far	*LPHEADER;

typedef struct
    {
    WORD    wOAState;
    WORD    wLastUserAction;
    BOOL    bUserLock;
    WORD    wExitCode;
    WORD    res1,
            res2;	
    }
    STATE;
typedef STATE far	*LPSTATE;

typedef struct
    {
    WORD    wOptions,
	    wFormat;
    }
    OPTIONS;
typedef OPTIONS far	*LPOPTIONS;

typedef struct
    {
    WORD    wNumActive,
	    wNumGranted,
	    wNumOther;
    WORD    fPifBehavior;
    HANDLE  hInstance;
    WORD    res1,
    	    res2;
    char    szCommandLine[1]; 	    			
    }
    WOAINFO;
typedef WOAINFO far	*LPWOAINFO;
    
typedef struct
    {
    WORD    wMode,
	    wFormat;
    WORD    wMacroSize,
	    wMenuSize;
    HANDLE  hInstance;
    WORD    wAttributes;     
    WORD    res1, 
    	    res2;	       
    }
    SHELLINFO;
typedef SHELLINFO far	*LPSHELLINFO;

typedef struct
    {
    WORD    wMacroID;
    WORD    wAction;
    char    szMacro[1];
    }
    MACRO;
typedef MACRO far	*LPMACRO;    
        
typedef struct
    {
    WORD    fAction;
    WORD    res1,
            res2;
    }
    WINDOWINFO;
typedef WINDOWINFO far	*LPWINDOWINFO;    

typedef struct
    {
    WORD    wHotX,
    	    wHotY,
    	    wWidth,
    	    wHeight,
    	    wWidthBytes,
    	    wColor;
    }
    ICON;
typedef ICON far	*LPICON;    

typedef enum 
    { None, Identify, Control }
    CONVSTATE;

typedef enum  
    {NoAction, PokeInfo, TerminateAll }
    ACTION;
    
/*--- General Tuple Object ---*/
struct Tuple				    /* Basic structure used by	    */ 
    {					    /*	 both Advisories and Shell  */
    struct Tuple    *next, *prev;	    /* Double linked list	    */
    WORD	    index;		    /* Index field		    */
    WORD	    field;		    /* extra field		    */
    WORD	    field2;	            /* extra field                  */
    };	
typedef struct Tuple TUPLE;
typedef TUPLE	    *PTUPLE;

/*--- Update Links ---*/
struct Link				    /* Basic structure used by	    */ 
    {					    /*	 both Links and Shells	    */
    struct Link    *next, *prev;	    /* Double linked list	    */
    ATOM	    aLink;		    /* Link atom		    */
    WORD	    wOptions;		    /* Mode of the advice  	    */
    };	
typedef struct Link LINK;
typedef LINK	    *PLINK;

/*--- Requesting shell attributes ---*/
enum SUBSTATES				    /* Identify converation substates. */
    { 
      Init,				    /* Just created		    */
      WaitForData,			    /* A ShellInfo request	    */
      WaitForAck,			    /* A WinOldAp Info was poked.   */
      ReadyForTerm 			    /* Conversation ready to end.   */
    };

struct Shell				    /* Same format as TUPLE.*/
    {					       
    struct Shell    *next, *prev;	    /* Double linked list	    */
    HWND	    hShellWnd;		    /* Shells window		    */
    enum SUBSTATES  subState;		    /* Current Identify sub state   */
    WORD	    priority;               /* Priority of the shell        */
    }; 
typedef struct Shell SHELL;
typedef SHELL	    *PSHELL;	
    		
/*--- Objects ---*/
struct Obj				    /* List of objects which can be */
    {					    /*	 requested.                 */
    char    *szName;
    long    (*getObj)(WORD, LPSTR);
    }; 
typedef struct Obj  OBJ;
typedef OBJ	    *POBJ;

struct UObj				    /* List of objects which can be */
    {					    /*	 updated                    */
    char    *szName;
    WORD    (*updateObj)(LPSTR);
    }; 
typedef struct UObj  UOBJ;
typedef UOBJ	     *PUOBJ;

/*--- Types -----*/
#define LOCAL	
#define EXPORT
#define IMPORT			extern

/*
 * Functions  
 *
 */
 
/*--- DDE message functions ---*/
LOCAL void	Data (HWND, long ),
		Request (HWND, long),
		IdentifyTerminate (HWND, long ),
		ControlTerminate (HWND, long),
		Poke (HWND, long),
		Initiate (HWND, long),
		Execute (HWND, long),
		Advise (HWND, long),
		Unadvise (HWND, long),
		ControlAck (HWND, long ),
		InitiateAck (HWND, long ),
		WoaInfoAck (HWND, long );
		
/*--- Identify Conversation functions ---*/
LOCAL ACTION    GetNextAction ( SHELL *);
LOCAL void      StartIdentify ();		    
LOCAL void      PokeWoaInfo ();
LOCAL void      RequestShellInfo ();
LOCAL void      TerminateAllConv ();

/*--- Control Conversation functions ---*/
LOCAL void      PostEvent (ATOM);
LOCAL void	PostPendingEvents();
LOCAL void      PostAck (long);	    

/*--- Common coverstaion functions ---*/
LOCAL void	ProcessMessageTwo();

/*--- System Conversation functions ---*/
LOCAL LONG far pascal SystemWndProc ( HWND, unsigned, WORD, long );

/*--- Functions to get and update objects ---*/
LOCAL long      GetDDEObject( ATOM, WORD, WORD);
LOCAL long      GetOldApState( WORD, LPSTR);	    
LOCAL long      GetWoaInfo( WORD, LPSTR );
LOCAL long	GetWindowInfo( WORD, LPSTR );
LOCAL long	GetSysItems( WORD, LPSTR );
LOCAL long	GetTopics( WORD, LPSTR );
LOCAL long	GetFormats( WORD, LPSTR );
LOCAL WORD      UpdateOldApState( LPSTR );
LOCAL WORD      UpdateWindowInfo( LPSTR );

/*--- Tuple Functions ---*/
LOCAL PTUPLE    AddTuple( PTUPLE *, WORD );
LOCAL void      DeleteTuple( PTUPLE *, PTUPLE);
LOCAL PTUPLE    FindTuple( PTUPLE, WORD );	


