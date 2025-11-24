/*** 
*tibrowse.cpp
*
*  Copyright (C) 1992, Microsoft Corporation.  All Rights Reserved.
*  Information Contained Herein Is Proprietary and Confidential.
*
*Purpose:
*  Type Information Browser
*
*
*Implementation Notes:
*
*****************************************************************************/

#include <windows.h>
#include <ole2.h>
#include <dispatch.h>

#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <commdlg.h>

#include "resource.h"

void OpenTypeLib(char FAR*);
void SetSelectedType(DWORD);
void FillMemberList(ITypeInfo FAR *, TYPEATTR FAR *, HWND);
void SetSelectedMember(DWORD); 
void SetSelectedParam(DWORD dwIndex);
void UpdateMemberInfo(MEMBERID memid);                   
void AssertFail(char FAR*, WORD);
void MethodError(HRESULT hresult);                   
void Cleanup(void);                   
void MemFree(void FAR*);

#define	CHECKRESULT(X) \
  {HRESULT hresult = (X); \
    if(hresult != NOERROR && FAILED(GetScode(hresult))) MethodError(hresult); }

#ifdef _DEBUG
# define Assert(expr) if (!(expr)) AssertFail(__FILE__, __LINE__);
# define SideAssert(expr) Assert(expr)
#else
# define Assert(expr)
# define SideAssert(expr) (expr)
#endif

#ifdef WIN32
#define EXPORT
#else
#define EXPORT _export
#endif

static HWND g_hwndMain;
static char g_szAppName[] = "TiBrowse" ;

static ITypeLib  FAR *g_ptlib;
static ITypeInfo FAR *g_ptinfoCur;
static TYPEATTR  FAR *g_ptypeattrCur;

static char * g_rgszTKind[] = {
    "Enum",		/* TKIND_ENUM */
    "Struct",		/* TKIND_RECORD */
    "Module",		/* TKIND_MODULE */
    "Interface",	/* TKIND_INTERFACE */
    "Dispinterface",	/* TKIND_DISPATCH */
    "Coclass",		/* TKIND_COCLASS */
    "Typedef",		/* TKIND_ALIAS */

    /* NOTE: the following aren't supported in typeinfo yet */
    "Union",		/* TKIND_UNION */
    "Encap Union"	/* TKIND_ENCUNION */
};

long FAR PASCAL EXPORT WndProc (HWND, UINT, WPARAM, LPARAM) ;

int PASCAL
WinMain(
    HANDLE hInstance,
    HANDLE hPrevInstance,
    LPSTR lpszCmdLine,
    int nCmdShow)
{
    MSG msg;
    WNDCLASS wndclass;
    OPENFILENAME ofn;                         
 
    if(!hPrevInstance){
      wndclass.style          = CS_HREDRAW | CS_VREDRAW;
      wndclass.lpfnWndProc    = WndProc ;
      wndclass.cbClsExtra     = 0 ;
      wndclass.cbWndExtra     = DLGWINDOWEXTRA ;
      wndclass.hInstance      = hInstance ;
      wndclass.hIcon          = LoadIcon (hInstance, g_szAppName) ;
      wndclass.hCursor        = LoadCursor (NULL, IDC_ARROW) ;
      wndclass.hbrBackground  = (HBRUSH) (COLOR_WINDOW + 1) ;
      wndclass.lpszMenuName   = NULL ;
      wndclass.lpszClassName  = g_szAppName ;

      RegisterClass (&wndclass) ;
    }
       
    if(OleInitialize(NULL) != NOERROR){
      MessageBox(NULL, "unable to initialize Ole", g_szAppName, MB_OK);
      return 0;
    }
     
    g_hwndMain = CreateDialog(hInstance, g_szAppName, 0, NULL);

    /* Open up the type library, and add the names of all the types to
     *	the list box.  If command line parameter given, assume it is a
     *	valid file name.  Otherwise, use COMMDLG.DLL to put up a file open
     *	dialog and query the user for a filename.
     */
    if(!lstrcmp(lpszCmdLine, "")){
      char FileBuf[128];
            
      memset(&ofn, 0, sizeof(OPENFILENAME));
      ofn.lStructSize = sizeof(OPENFILENAME);
      ofn.hwndOwner = g_hwndMain;
      ofn.lpstrFile = (LPSTR)&FileBuf;
      ofn.nMaxFile = sizeof(FileBuf);
      *FileBuf = '\0';
      ofn.lpstrFilter = "Type Libraries\0*.tlb\0\0";
      ofn.nFilterIndex = 1; 
      ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
      if(GetOpenFileName(&ofn) == 0){ 
	DWORD dwerr;

	dwerr = CommDlgExtendedError();
        /* CONSIDER: do something with this error code */
        Cleanup();
        exit(1);
      }
      lpszCmdLine = ofn.lpstrFile;	/* get file user selected */
    }

    OpenTypeLib(lpszCmdLine);
     
    ShowWindow(g_hwndMain, nCmdShow);

    while(GetMessage (&msg, NULL, 0, 0)){
      TranslateMessage (&msg);
      DispatchMessage (&msg);
    }
	 
    Cleanup();

    return msg.wParam;
}


void Cleanup()
{
    if(g_ptinfoCur != NULL)
      g_ptinfoCur->Release();
    if(g_ptlib != NULL)
      g_ptlib->Release();
    OleUninitialize();	 
}


long FAR PASCAL EXPORT
WndProc(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    DWORD dwIndex;

    switch(message){
    case WM_COMMAND: 
      /* We deal with the following events:
       * The selection changes on the type list and we have
       *  to update the member list & type info.
       * The selection changes on the member list and we have
       *  to update the param list & member info.
       * Selection changes on a parameter and we have to
       *  update the param info.
       */
#ifdef WIN32
  #define wParamX LOWORD(wParam)
  #define notifyMsg HIWORD(wParam)
#else
  #define wParamX wParam
  #define notifyMsg HIWORD(lParam)
#endif
      switch(wParamX){
      case IDC_TYPELIST:
        if(notifyMsg == LBN_SELCHANGE){
          dwIndex = SendDlgItemMessage(hwnd, IDC_TYPELIST, LB_GETCURSEL, 0, 0L);
	  SetSelectedType(dwIndex);
	}
	break;
      case IDC_MEMBERLIST:
        if(notifyMsg == LBN_SELCHANGE){
          dwIndex = SendDlgItemMessage(hwnd, IDC_MEMBERLIST, LB_GETCURSEL,0,0L);
	  SetSelectedMember(dwIndex);
	}
        break;
      case IDC_PARAMLIST:
        if(notifyMsg == LBN_SELCHANGE){
          dwIndex = SendDlgItemMessage(hwnd, IDC_PARAMLIST, LB_GETCURSEL, 0,0L);
	  SetSelectedParam(dwIndex);
	}
	break;
      }
      return 0;

    case WM_DESTROY:
      PostQuitMessage(0);
      return 0 ;
    }
    return DefWindowProc (hwnd, message, wParam, lParam) ;
}


void OpenTypeLib (char FAR *sztlib)
{                                     
    UINT utypeinfoCount, i;
    BSTR bstrName;
    TLIBATTR FAR* ptlibattr;
	
    /* load the type library */
    CHECKRESULT(LoadTypeLib(sztlib, &g_ptlib));

    /* get library attributes for the fun of it */
    CHECKRESULT(g_ptlib->GetLibAttr(&ptlibattr));

    /* release library attributes */
    g_ptlib->ReleaseTLibAttr(ptlibattr);

    /* Now add each of the names to the type list */
    utypeinfoCount = g_ptlib->GetTypeInfoCount();
    for(i = 0; i < utypeinfoCount; i++){
      CHECKRESULT(g_ptlib->GetDocumentation(i, &bstrName, NULL, NULL, NULL));  
      Assert(bstrName);
      SendDlgItemMessage(g_hwndMain, IDC_TYPELIST, LB_ADDSTRING, 0, (LPARAM)bstrName);
      SysFreeString(bstrName);
    }
}

/*
 * SetSelectedType
 * 
 * When the user changes the selection of a type, this function updates the
 * dialog by changing the member list and the help for the type. It also sets
 * g_ptinfoCur to refer to the typeinfo.
 */
void SetSelectedType (DWORD dwIndex)
{
    BSTR bstrDoc;
    LPSTR szData;
    char szBuf[40];
    HRESULT hresult;
    DWORD dwHelpContext;

    if(g_ptinfoCur != NULL){
      g_ptinfoCur->ReleaseTypeAttr(g_ptypeattrCur);
      g_ptinfoCur->Release();
    }

    /* Clear out the member list */
    SendDlgItemMessage(g_hwndMain, IDC_MEMBERLIST, LB_RESETCONTENT,0,0L);
    if(dwIndex != LB_ERR)
    {
      /* Note that the index in the list box is conveniently the
       * same as the one to pass to GetTypeInfo.
       */
	    
      CHECKRESULT(g_ptlib->GetTypeInfo((UINT) dwIndex, &g_ptinfoCur));
      CHECKRESULT(g_ptinfoCur->GetTypeAttr(&g_ptypeattrCur));
				  
      // TypeKind
      szData = g_rgszTKind[g_ptypeattrCur->typekind];
      SendDlgItemMessage(g_hwndMain, IDC_TYPEKIND, WM_SETTEXT, 0, (LPARAM)szData);

      // GUID
      hresult = StringFromCLSID(g_ptypeattrCur->guid, &szData);
      if(hresult != NOERROR)
	szData = "error!";
      SendDlgItemMessage(g_hwndMain, IDC_GUID, WM_SETTEXT, 0, (LPARAM)szData);
      MemFree(szData);

      // Version
      wsprintf(szBuf, "%u.%02u",
	g_ptypeattrCur->wMajorVerNum, g_ptypeattrCur->wMinorVerNum);
      SendDlgItemMessage(
	g_hwndMain, IDC_VERSION, WM_SETTEXT, 0, (LPARAM)(LPSTR)szBuf);
    
    
      CHECKRESULT(
	g_ptlib->GetDocumentation(
	  (UINT)dwIndex, NULL, &bstrDoc, &dwHelpContext, NULL));

      // Help Context
      _ltoa((long)dwHelpContext, szBuf, 10);
      SendDlgItemMessage(
	g_hwndMain, IDC_HELPCONTEXT, WM_SETTEXT, 0, (LPARAM)(LPSTR)szBuf);

      // Documentation string
      szData = (bstrDoc != NULL) ? (LPSTR)bstrDoc : "<none>";
      SendDlgItemMessage(g_hwndMain, IDC_HELP, WM_SETTEXT, 0, (LPARAM)szData);
      SysFreeString(bstrDoc);
    
      FillMemberList(g_ptinfoCur, g_ptypeattrCur, GetDlgItem(g_hwndMain, IDC_MEMBERLIST)); 
    }
    else /* no item is selected -- NULL out the pointers */
    {
      g_ptinfoCur = NULL;
      g_ptypeattrCur = NULL;
    }

    SendDlgItemMessage(g_hwndMain, IDC_PARAMLIST, LB_RESETCONTENT, 0, 0L);
}

/*
 * FillMemberList.
 *
 * Sets the current typeinfo to the typeinfo indexed by dwIndex, and then
 * fills in the list box with the members of the type.
 */
 
void
FillMemberList(
    ITypeInfo FAR *ptypeinfo,
    TYPEATTR FAR *ptypeattr,
    HWND hwndMemberList)
{
    MEMBERID memid; 
    BSTR bstrName;
    UINT i;
    FUNCDESC FAR *pfuncdesc;
    VARDESC  FAR *pvardesc;
	
    /* Now add all of the functions and all of the vars.
     * This is somewhat roundabout.
     * For each one, we need to get the funcdesc, or the vardesc.
     * From that we get the MEMBERID, and finally can get to the name.
     */
    for(i = 0; i < ptypeattr->cFuncs; i++){
      CHECKRESULT(ptypeinfo->GetFuncDesc(i, &pfuncdesc));
      memid = pfuncdesc->memid;
      CHECKRESULT(ptypeinfo->GetDocumentation(memid, &bstrName, NULL, NULL, NULL));
      ptypeinfo->ReleaseFuncDesc(pfuncdesc);
      pfuncdesc = NULL;
	    
      Assert(bstrName);				
      SendMessage(hwndMemberList, LB_ADDSTRING, 0, (LPARAM) bstrName);
      SysFreeString(bstrName);
    }
				    
    for(i = 0; i < ptypeattr->cVars; i++)
    {
      CHECKRESULT(ptypeinfo->GetVarDesc(i, &pvardesc));
      memid = pvardesc->memid;
      CHECKRESULT(ptypeinfo->GetDocumentation(memid, &bstrName, NULL, NULL, NULL));
      ptypeinfo->ReleaseVarDesc(pvardesc);
      pvardesc = NULL;
					
      Assert(bstrName);
      SendMessage(hwndMemberList, LB_ADDSTRING, 0, (LPARAM) bstrName);
      SysFreeString(bstrName);
    }
}

/*
 * SetSelectedMember
 *
 * When a member of a type is selected, update the help to be the help of the member, and
 * if the member is a function update the parameter list to reflect that it is a function.
 *
 */
 
void SetSelectedMember(DWORD dwIndex)
{
    MEMBERID memid;
    HWND hwndParamList;

    /* In any case, we'll need to clear out the parameter list. */
    hwndParamList = GetDlgItem(g_hwndMain, IDC_PARAMLIST);
    SendMessage(hwndParamList, LB_RESETCONTENT, 0, 0L); 
	
    if (dwIndex == LB_ERR)
    	return;
		
    /* if this is a function, fill the param list, otherwise just fill
     * in the item info.
     */
    if(dwIndex < g_ptypeattrCur->cFuncs){
      FUNCDESC FAR *pfuncdesc;
      USHORT i;
      UINT cNames;
      const UINT MAX_NAMES = 40;
      BSTR rgNames[MAX_NAMES];

      CHECKRESULT(g_ptinfoCur->GetFuncDesc((UINT) dwIndex, &pfuncdesc));
      memid = pfuncdesc->memid;
      UpdateMemberInfo(memid);
    
      CHECKRESULT(g_ptinfoCur->GetNames(memid, rgNames, MAX_NAMES,&cNames));
      for(i = 1; i < cNames; i++){
	Assert(rgNames[i])
	SendMessage(hwndParamList, LB_ADDSTRING, 0, (LPARAM) rgNames[i]);
	SysFreeString(rgNames[i]);
      }
      Assert(rgNames[0]);	          
      SysFreeString(rgNames[0]);
      g_ptinfoCur->ReleaseFuncDesc(pfuncdesc);
    }
    else
    {
      VARDESC FAR *pvardesc;

      CHECKRESULT(
        g_ptinfoCur->GetVarDesc(
	  (UINT)(dwIndex - g_ptypeattrCur->cFuncs), &pvardesc));
      memid = pvardesc->memid;
      UpdateMemberInfo(memid);
      g_ptinfoCur->ReleaseVarDesc(pvardesc);
    }

}


/* sets fields on the dialog (such as help string and help context) from
   the type information.
*/
void
UpdateMemberInfo(MEMBERID memid)                   
{
    BSTR bstrDoc;
    LPSTR szData;
    DWORD dwHelpContext;
    char szBuf[40];                    

    /* get the member information */
    CHECKRESULT(g_ptinfoCur->GetDocumentation(memid, NULL, &bstrDoc, &dwHelpContext, NULL));

    /* update the help string displayed in the dialog */
    if (bstrDoc)
      szData = (LPSTR)bstrDoc;
    else /* no help string for this item */
      szData = "<none>";

    SendDlgItemMessage(g_hwndMain, IDC_HELP, WM_SETTEXT, 0, (LPARAM)szData);
    SysFreeString(bstrDoc);
        
    /* update the help context displayed in the dialog */
    _ltoa((long)dwHelpContext, szBuf, 10);
    SendDlgItemMessage(g_hwndMain, IDC_HELPCONTEXT, WM_SETTEXT, 0, (LPARAM)((LPSTR)szBuf));
 }

/*
 * SetSelectedParam
 * 
 * CONSIDER: Enhance to show parameter type information here.
 */
void SetSelectedParam(DWORD dwIndex)
{
}

void
AssertFail(char FAR * szFile, WORD  lineNo)
{
    char szAssert[255];
    int id;

    wsprintf(szAssert, "Assertion failed.  File %s, line %d.", szFile, lineNo);

    id = MessageBox(NULL, szAssert, "TiBrowse Assertion.  OK to continue, CANCEL to quit.", MB_OKCANCEL | MB_TASKMODAL);
    if (id == IDCANCEL)
        exit(1);
}

void
MethodError(HRESULT hresult)
{

    /* CONSIDER: add code to figure out what specific error this is */

    MessageBox(NULL, "Error returned from TYPELIB.DLL", g_szAppName, MB_OK);

    Cleanup();

    exit(1);
}

/* free using the task allocator */
void
MemFree(void FAR* pv)
{
    HRESULT hresult;
    IMalloc FAR* pmalloc;

    hresult = CoGetMalloc(MEMCTX_TASK, &pmalloc);

    if(hresult != NOERROR){
      MessageBox(NULL, "error accessing task allocator", g_szAppName, MB_OK);
      return;
    }

    pmalloc->Free(pv);
    pmalloc->Release();
}
