/**************************************************************************
*                                                                         *
*      File:  INIT.C                                                      *
*                                                                         *
*   Purpose:  Contains the code to create and show the window for each    *
*             instance of the app.                                        *
*                                                                         *
* Functions:  BOOL InitInstance(HANDLE, int)                              *
*                                                                         *
*  Comments:                                                              *
*                                                                         *
*   History:  Date      Reason                                            *
*             --------  -----------------------------------               *
*                                                                         *
*             10/01/91  Created                                           *
*                                                                         *
**************************************************************************/

#define WIN31

#include "windows.h"  
#include "commdlg.h"
#define ININIT
#include "cddemo.h"
#undef  ININIT

BOOL FAR InitInstance(HANDLE hInstance, int nCmdShow)
{
  ghInst = hInstance;

  ghWnd = CreateWindow(gszCommonWClass,
                       gszAppName,
                       WS_OVERLAPPEDWINDOW, 
                       CW_USEDEFAULT, 
                       nCmdShow, 
                       CW_USEDEFAULT, 
                       CW_USEDEFAULT, 
                       NULL, 
                       NULL, 
                       hInstance, 
                       NULL);

  if (!ghWnd)
     return(FALSE);

  ShowWindow(ghWnd, nCmdShow);
  UpdateWindow(ghWnd);
  
  return(TRUE);
}
