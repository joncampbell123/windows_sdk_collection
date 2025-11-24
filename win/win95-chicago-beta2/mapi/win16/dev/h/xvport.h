/*
 -  X V P O R T . H
 -
 *  Header File for XVPORT. Tools
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

//Initialize viewports dll 
BOOL    XVPInit(HWND);                                       

//Initialize multithreading 
BOOL    XVPInitMultiThread();                                       

//Deinitialize viewports dll
VOID    XVPDeInit(); 

//Create a viewport
UINT    XVPCreateViewport(LPSTR szTitle);

//Destroy the viewport
VOID    XVPDestroyViewport(UINT uID);   

//Log a string with identation
VOID    XVPLog(UINT uID, int nLevel, LPSTR lpDesc );

//Log a Pass or Fail          
VOID    XVPLogStatus(UINT uID,int nLevel, LPSTR lpDesc, BOOL IsFail);  

  

//Position viewport on Screen
VOID    XVPPosition(UINT uID, int );                                   

//Set Default log Level
VOID    XVPSetLogLevel(UINT uID,int nLevel);                           

//Clear Viewport
VOID    XVPReset(UINT uID);                                           

//Sets the title of the viewport
VOID    XVPSetTitle(UINT uID,LPSTR szTitle);                           

//Inverts the order of display
VOID    XVPInvert(UINT uID);                                          

//Modifies the show state.
VOID    XVPShow(UINT uID,int nShow);                                                                     

#ifdef __cplusplus
}
#endif



//Define Logging port levels
#define CASE_LEVEL       0
#define FUNCTION_LEVEL   1
#define DETAIL_LEVEL     2

// define Dialog Positions 
#define    TOP_LEFT      1
#define    TOP_RIGHT     2
#define    CENTER        3
#define    BOTTOM_LEFT   4
#define    BOTTOM_RIGHT  5

// define log string types
#define FAIL 1
#define PASS 2
#define HEAD 4
#define TAIL 8

//define show flags
#define XVP_HIDE 0
#define XVP_SHOW 1
#define XVP_MIN  2
#define XVP_MAX  3


