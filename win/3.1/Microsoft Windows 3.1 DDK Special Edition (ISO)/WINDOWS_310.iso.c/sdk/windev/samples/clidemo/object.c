/* 
 * object.c - OLE object support routines 
 *
 * Created by Microsoft Corporation.
 * (c) Copyright Microsoft Corp. 1990 - 1992  All Rights Reserved
 */

//--- INCLUDES ----

#include <windows.h>                   //- WINDOWS
#include <shellapi.h>                  //- SHELL
#include <ole.h>                       //- OLE

#include "global.h"                    //- global variables and structures
#include "stream.h"                    //- application includes:
#include "object.h"
#include "clidemo.h"
#include "demorc.h"
#include "utility.h"
#include "dialog.h"
#include "register.h"

#define  HIMETRIC_PER_INCH   2540      //- number HIMETRIC units per inch


//--- VARIABLES ---

//--- Globals
int     cOleWait     = 0;
                    
int     giXppli;                       //- pixels per logical inch along width
int     giYppli;                       //- pixels per logical inch along height 


/***************************************************************************
 * CallBack()
 *
 * This routine will be called whenever an object has been changed, 
 * saved, renamed, is being painted, or an  asynchronous operation has 
 * completed. This routine is called by the OLE client DLL in the 
 * above situations.  A pointer to this function is kept in the client
 * vtbl.  It is our obligation as a client application to insure that a
 * pointer to this procedure is in the vtbl.
 *
 * IMMPORTANT: notice that we are posting messages here rather that doing
 * the work right away.  Well, this is done to avoid any possibility of
 * getting into another dispatch message loop.  A MessageBox woul do this!
 *
 * Returns int - see below 
 *
 * The return value is generally ignored, except for these notifications:  
 * OLE_QUERY_PAINT and  OLE_QUERY_RETRY. For these two notifications, 
 * returning TRUE means continue the current operation(eg painting or retry)
 * Returning FALSE means stop the current operation. This is useful as an 
 * object which takes a long time to paint can be interrupted in order to 
 * perform other operations.
 ***************************************************************************/

int FAR PASCAL CallBack(               //- ENTRY:
   LPOLECLIENT    lpClient,            //- client application pointer
   OLE_NOTIFICATION flags,             //- notification code being sent
   LPOLEOBJECT    lpObject             //- OLE object pointer
){                                     //- LOCAL:
   APPITEMPTR     pItem;               //- application item pointer

                                 
   pItem = (APPITEMPTR)LOWORD(lpClient);
   switch (flags) 
   {
      case OLE_CLOSED:                 //- server has closed
         if (!pItem->fVisible)
         {
            PostMessage(hwndFrame, WM_DELETE,(WORD)pItem,0L);
            Dirty(DOC_UNDIRTY);
         }
         SetFocus( hwndFrame );
         break;

      case OLE_SAVED:                  //- server has saved object
      case OLE_CHANGED:                //- object has changes
         cOleWait++;
         pItem->fServerChangedBounds = pItem->fVisible = TRUE;
         PostMessage(pItem->hwnd, WM_CHANGE, NULL, 0L);
         break;

      case OLE_RELEASE:                //- notification that an asynchronous
         ToggleBlockTimer(FALSE,NULL); //- toggle timer off
         if (hRetry)
            PostMessage(hRetry,WM_COMMAND,IDCANCEL,0L);

         if (cOleWait)                 //- operation has completed
         {
            pItem->fRetry = TRUE;
            if (!--cOleWait)
               Hourglass(FALSE);
            Release(pItem);
         } 
         break;

      case OLE_QUERY_RETRY:          //- Continue retrying.
         ToggleBlockTimer(FALSE,NULL);//- toggle timer off
         if (!hRetry && pItem->fRetry)
            PostMessage(hwndFrame,WM_RETRY,(WORD)pItem,0L);
         return (pItem->fRetry);
             
      case OLE_QUERY_PAINT:          //- continue repainting
         return TRUE;                //- a false return terminates either

        default:
            break;
    }
    return 0;                          //- return value is ignored in 
                                       //- most cases, see header
}

/***************************************************************************
 * Release()
 *
 * Check for an error on the OLE_RELEASE notification. 
 **************************************************************************/

static void Release(                   //- ENTRY:
   APPITEMPTR     pItem                //- Item pointer
){                                     //- LOCAL:
   WORD           wParam;              //- error code parameter

   if ((wParam = OleQueryReleaseError(pItem->lpObject)) == OLE_OK) 
      return;

   switch (OleQueryReleaseMethod(pItem->lpObject)) 
   {
      case OLE_LNKPASTE:
         pItem->fVisible = FALSE;
         break;

      case OLE_CREATEFROMTEMPLATE:
      case OLE_CREATE:
         pItem->fVisible = FALSE;
         cOleWait++;
         PostMessage(hwndFrame, WM_DELETE,(WORD)pItem,1L);
         Dirty(DOC_UNDIRTY);
   }
                                  //- post a message to the main window
                                  //- which will display a message box
   PostMessage(hwndFrame,WM_ERROR,wParam,NULL);

}

/***************************************************************************
 *  Error()
 *
 *  This function checks for error conditions
 *  generated by OLE API callsFor OLE_WAIT_FOR_RELEASE,
 *  we keep track of the number of objects waiting, when
 *  this count is zero, it is safe to exit the application.
 *
 *  Returns OLESTATUS -  0 if OLE_WAIT_FOR_RELEASE or OLE_OK
 *                       otherwise the OLESTATUS returned after an action
 *                       is taken.
 *************************************************************************/

OLESTATUS FAR Error(                   //- ENTRY
   OLESTATUS      olestat              //- OLE status
){

   switch (olestat) 
   {
      case OLE_WAIT_FOR_RELEASE:
         if (!cOleWait)
            Hourglass(TRUE);
         cOleWait++;                   //- increment wait count

      case OLE_OK:
         return 0;

      case OLE_ERROR_STATIC:           //- static object
         ErrorMessage(W_STATIC_OBJECT);
         break;

      case OLE_ERROR_REQUEST_PICT:
      case OLE_ERROR_ADVISE_RENAME:
      case OLE_ERROR_DOVERB:
      case OLE_ERROR_SHOW:
      case OLE_ERROR_OPEN:
      case OLE_ERROR_NETWORK:
      case OLE_ERROR_ADVISE_PICT:
      case OLE_ERROR_COMM:             //- Invalid links
         InvalidLink();
         break;

      case OLE_BUSY:
         RetryMessage(NULL,RD_CANCEL);

      default:
         break;
    }
    return olestat;
}


/****************************************************************************
 * PreItemCreate()
 * 
 * This routine allocates an application item structure. A pointer to this 
 * structure is passed as the client structure, therefore we need to 
 * have a pointer to the vtbl as the first entry. We are doing this 
 * to allow acess to the application item information during a OLE
 * DLL callback.  This approach simplifies matters.
 *
 * Returns APPITEMPTR - a pointer to a new application item structure
 *                      which can operate as a client structure.
 ***************************************************************************/

APPITEMPTR FAR PreItemCreate(          //- ENTRY:
   LPOLECLIENT    lpClient,            //- OLE client pointer
   BOOL           fShow,               //- show/no-show flag
   LHCLIENTDOC    lhcDoc               //- client document handle
){                                     //- LOCAL:
   HANDLE         hitem;               //- temp handle for new item
   APPITEMPTR     pItem;               //- application item pointer


   if (hitem = LocalAlloc(LMEM_MOVEABLE | LMEM_ZEROINIT, sizeof(APPITEM)))
      if (pItem = (APPITEMPTR)LocalLock(hitem))
      {                                //- set the vtbl pointer
         pItem->oleclient.lpvtbl     = lpClient->lpvtbl;
         pItem->lpObjectUndo         = NULL;
         pItem->fVisible             = fShow;
         pItem->fServerChangedBounds = FALSE;
         pItem->lhcDoc               = lhcDoc;

         return pItem;                 //- SUCCESS return
      }

   ErrorMessage(E_FAILED_TO_ALLOC);
   return NULL;                        //- ERROR return

}


/***************************************************************************
 * ItemWndProc()
 *
 * This function handles item window message processing.
 * There is an item window for each OLE object. This was done to
 * to simplify hit testing and repainting. These windows are child
 * windows.

 * returns long - standard child routine
 **************************************************************************/

long FAR PASCAL ItemWndProc(           //- ENTRY:
   HWND           hwnd,                //- standard windows parameters
   UINT       msg,
   WPARAM	    wParam,
   LPARAM	    lParam
){                                     //- LOCAL:
   static POINT   dragPt;              //- Mouse drag point 
   static RECT    dragRect;            //- Mouse drag rectangle 
   static BOOL    fCaptured;           //- captured flag
   APPITEMPTR     pItem;               //- application item pointer
   PAINTSTRUCT    ps;                  //- paint structure
   POINT          pt;                  //- point
   RECT           rc;                  //- bounding rectangle

   switch (msg) 
   {     
      case WM_SIZE:
         if (pItem = (APPITEMPTR)GetWindowWord(hwnd,0))
         {
            if (!pItem->fServerChangedBounds && pItem->otObject == OT_EMBEDDED)
               ObjSetBounds(pItem);
            else
               pItem->fServerChangedBounds = FALSE;
         }
         break;

      case WM_CHANGE:
         --cOleWait;
         pItem = (APPITEMPTR)GetWindowWord(hwnd,0);
         if (!Error(OleQueryBounds(pItem->lpObject, &rc))) 
         {
            ConvertToClient(&rc);

            SetWindowPos(
               hwnd, 
               NULL, 
               0, 
               0,
               rc.right - rc.left + 2*GetSystemMetrics(SM_CXFRAME),
               rc.bottom - rc.top + 2*GetSystemMetrics(SM_CYFRAME),
               SWP_NOZORDER | SWP_NOMOVE | SWP_DRAWFRAME
            );

            if (!pItem->fNew && !fLoadFile)
               ShowNewWindow(pItem);
            else
               InvalidateRect(hwnd, NULL, TRUE);

            Dirty(DOC_DIRTY);
         }
         break;

      case WM_NCLBUTTONDOWN:
         SetTopItem((APPITEMPTR)GetWindowWord(hwnd,0));
         return (DefWindowProc(hwnd, msg, wParam, lParam));

      case WM_PAINT:
         BeginPaint(hwnd, (LPPAINTSTRUCT)&ps);
         GetClientRect(hwnd, &rc);
         pItem = (APPITEMPTR)GetWindowWord(hwnd, 0);
                                       //- Call OLE draw
         Error(OleDraw(pItem->lpObject, ps.hdc, &rc, NULL, NULL));

         EndPaint(hwnd, (LPPAINTSTRUCT)&ps);
         break;

      case WM_LBUTTONDBLCLK:           //- execute a verb 
         ANY_OBJECT_BUSY;
         ExecuteVerb(OLEVERB_PRIMARY,(APPITEMPTR)GetWindowWord(hwnd,0));
         break;

      case WM_LBUTTONDOWN:    
         GetWindowRect(hwnd, (LPRECT)&dragRect);
         ScreenToClient(hwndFrame, (LPPOINT)&dragRect);
         ScreenToClient(hwndFrame, (LPPOINT)&dragRect.right);

         dragPt.x = LOWORD(lParam);
         dragPt.y = HIWORD(lParam);

         ClientToScreen(hwnd, (LPPOINT)&dragPt);
         ScreenToClient(hwndFrame, (LPPOINT)&dragPt);

         SetCapture(hwnd);
         fCaptured = TRUE;
         SetTopItem((APPITEMPTR)GetWindowWord(hwnd,0));
         break;

      case WM_LBUTTONUP:
         if (!fCaptured)
                break;
         ReleaseCapture();
         fCaptured = FALSE;
         Dirty(DOC_DIRTY);
         break;

      case WM_MOUSEMOVE:
         if (!fCaptured)
            break;
         pt.x = LOWORD(lParam);
         pt.y = HIWORD(lParam);

         ClientToScreen(hwnd, (LPPOINT)&pt);
         ScreenToClient(hwndFrame, (LPPOINT)&pt);

         OffsetRect(
               (LPRECT)&dragRect, 
               pt.x - dragPt.x, 
               pt.y - dragPt.y
         );

         MoveWindow(
            hwnd, 
            dragRect.left, dragRect.top,
            dragRect.right - dragRect.left,
            dragRect.bottom - dragRect.top, TRUE
         );

         dragPt.x = pt.x;
         dragPt.y = pt.y;
         break;
   
      default: 
         return (DefWindowProc(hwnd, msg, wParam, lParam));
   }
   return 0L;

}

/****************************************************************************
 * PostItemCreate()
 *
 * This function creates a child window which will contain the newly 
 * created OLE object. A pointer to our item information is stored in the 
 * extra bytes of this window. This is where we internally keep track
 * of information related to the object as well as the
 * pointer to the object for subsequent OLE API calls.  This routine is
 * called after an OLE object has been created by the client library.
 *
 * Returns BOOL - TRUE if application item has been created.
 ****************************************************************************/

BOOL FAR PostItemCreate(               //- ENTRY:
   LPOLEOBJECT    lpObject,            //- OLE object pointer
   long           otObject,            //- OLE object type
   LPRECT         lprcObject,          //- object bounding rect
   APPITEMPTR     pItem                //- application item pointer
){                                     //- LOCAL:
   int            i;                   //- index
   RECT           rc;                  //- bounding rectangle
   char           pData[OBJECT_LINK_MAX];//- copy of link data

   if (lprcObject)                     //- if the size of the objects
      rc = *lprcObject;                //- bounding rectangle is not
   else if (OleQueryBounds(lpObject, &rc) == OLE_OK) 
      ConvertToClient(&rc);
   else 
      SetRect(&rc, 0, 0, 0, 0);

   if (!(pItem->hwnd = CreateWindow(   //- Create the child window 
         szItemClass, "",
         WS_BORDER | WS_CHILD | WS_CLIPSIBLINGS | WS_THICKFRAME,
         rc.left,rc.top,
         rc.right - rc.left + 2 * GetSystemMetrics(SM_CXFRAME),
         rc.bottom - rc.top + 2 * GetSystemMetrics(SM_CYFRAME),
         hwndFrame, NULL, hInst, NULL
   ))) goto Error;

                                       //- in windows extra bytes
   SetWindowWord(pItem->hwnd, 0, (WORD)pItem);

   pItem->otObject = otObject;
   pItem->lpObject = lpObject;
   pItem->fRetry  = TRUE;

   if( pItem->otObject == OT_EMBEDDED )//- if object is embedded tell library
   {                                   //- the container name and object name.
      int cb = CBOBJNAMEMAX;           //- The name will be the server window title.
      char sz[CBOBJNAMEMAX];           //- when the object is edited. 

      OleQueryName(lpObject, (LPSTR)sz, &cb );

      WaitForObject(pItem);
      Error(OleSetHostNames(lpObject, (LPSTR)szAppName, (LPSTR)sz ));
      WaitForObject(pItem);
   }
   else if (pItem->otObject == OT_LINK)//- if the object is linked  
   {                                   //- retrieve update options

      WaitForObject(pItem);
      if(Error(OleGetLinkUpdateOptions(pItem->lpObject, &pItem->uoObject)))
         goto Error;
      
      if (ObjGetData(pItem,pData))
      {
         for (i=0; pData[i];i++);      //- Skip past the server name 
         pItem->aLinkName = AddAtom(&pData[++i]);
      } 
      else
         pItem->aLinkName = AddAtom("");
   }
   iObjects++;
   Dirty(DOC_DIRTY);
                                       //- a user interface recommendations.
   return TRUE;                        //- SUCCESS return

Error:                                 //- ERROR Tag

   ErrorMessage(E_FAILED_TO_CREATE_CHILD_WINDOW);
   FreeAppItem(pItem);
   
   return FALSE;                       //- ERROR return

}
   
/***************************************************************************
 * ConvertToClient()
 *
 * This function will convert to client from himetric.
 **************************************************************************/

void FAR ConvertToClient(              //- ENTRY:
   LPRECT         lprc                 //- pointer to bounding rectangle
){                                     //- LOCAL

   //- If we have an empty rectangle then set the default size 
   if (!(lprc->left || lprc->top || lprc->right || lprc->bottom))
      SetRect(lprc, 0, 0, CXDEFAULT, CYDEFAULT);
   else 
   {
      //- We got the himetric units, converts them to pixels now.     
      lprc->right   = MulDiv (giXppli, (lprc->right - lprc->left),
                          HIMETRIC_PER_INCH);
                      
      lprc->bottom  = MulDiv (giYppli, (lprc->top - lprc->bottom),
                          HIMETRIC_PER_INCH);

      lprc->left    = 0;
      lprc->top     = 0;      
    }
}

/***************************************************************************
 * ObjInsert()
 * 
 * Query the user for object type to insert and insert the new OLE object
 ***************************************************************************/

void FAR ObjInsert(                    //- ENTRY:
   LHCLIENTDOC    lhcDoc,              //- OLE document handle              
   LPOLECLIENT    lpClient             //- pointer to OLE client structure
){                                     //- LOCAL:   
   FARPROC        lpfnInsertNew;       //- Insert New dialog bos instance thunk
   LPOLEOBJECT    lpObject;            //- pointer to OLE object 
   APPITEMPTR     pItem;               //- item pointer
   char           szServerName[CBPATHMAX];//- Class name for OleCreate() 
   char           szClassName[CBPATHMAX];//- Class name for OleCreate() 
   char           szTmp[CBOBJNAMEMAX]; //- buffer to unique object name 
    
   lpfnInsertNew    = MakeProcInstance(fnInsertNew, hInst);

   if (DialogBoxParam(hInst, MAKEINTRESOURCE(DTCREATE),hwndFrame, 
            lpfnInsertNew, (long)((LPSTR)szClassName)) != IDCANCEL)    
   {
      if (pItem = PreItemCreate(lpClient, FALSE, lhcDoc))
      {
         RegGetClassId(szServerName, szClassName);
         pItem->aServer = AddAtom(szServerName);
         if ( Error( OleCreate(STDFILEEDITING,(LPOLECLIENT)&(pItem->oleclient), 
            (LPSTR)szClassName, lhcDoc,CreateNewUniqueName(szTmp), 
            &lpObject,olerender_draw, 0))) 
         {
            ErrorMessage(E_FAILED_TO_CREATE_OBJECT);
            FreeAppItem(pItem);
         }
         else
            PostItemCreate(lpObject, OT_EMBEDDED, NULL, pItem);
      }
   }  

   FreeProcInstance(lpfnInsertNew);

}

/***************************************************************************
 *  ObjDelete()
 *
 * Delete an OLE object. For this application, all OLE objects 
 * are associated with a child window; therefore the window must be 
 * destroyed.
 *
 * NOTE: There is one case when we call OleRelease and the other when
 * we call OleDelete.  We call OleRelease when we are deregistering
 * a document and OleDelete when removing an object from a document. 
 **************************************************************************/

void FAR ObjDelete(                    //- ENTRY:
   APPITEMPTR     pItem,               //- pointer to application item
   BOOL           fDelete              //- delete or release flag
){                                     //- LOCAL:

   if (pItem->lpObjectUndo)
   {
      Error(OleDelete(pItem->lpObjectUndo));
                                       //- wait for asynchronous operation
      WaitForObject(pItem);
   }

   if (fDelete ? Error(OleDelete(pItem->lpObject)) 
                     : Error(OleRelease(pItem->lpObject)))
   {
      ErrorMessage(E_FAILED_TO_DELETE_OBJECT);
      return;                          //- ERROR return
   }

   if (pItem->fVisible)
   {
      ShowWindow(pItem->hwnd, SW_HIDE); 
      pItem->fVisible = FALSE;
   }
                                       //- the operation has to complete
   WaitForObject(pItem);               //- before the application structure
  
   FreeAppItem(pItem); 
   iObjects--;

}   


/***************************************************************************
 *  ObjPaste()
 *
 *  This function obtains an object from the clipboard.
 *  Handles both embedded and linked objects. An item window is
 *  created for each new object.
 *
 *  Returns BOOL  - TRUE if object was pasted succesfully.
 **************************************************************************/

void FAR ObjPaste(                     //- ENTRY:
   BOOL           fPaste,              //- Paste/PasteLink flag 
   LHCLIENTDOC    lhcDoc,              //- client document handle
   LPOLECLIENT    lpClient             //- pointer to client
){                                     //- LOCAL:          
   LPOLEOBJECT    lpObject;            //- object pointer
   long           otObject;            //- object type
   APPITEMPTR     pItem;               //- application item pointer
   char           szTmp[CBOBJNAMEMAX]; //- temporary object name string

   if (!(pItem = PreItemCreate(lpClient, TRUE, lhcDoc)))
      return;                          //- ERROR return
       
   if (!OpenClipboard(hwndFrame))
      goto Error;                      //- ERROR jump


   if (fPaste)                         //- PASTE the object. 
   {                                   //- Try "StdFileEditing" protocol
      if (Error(OleCreateFromClip(STDFILEEDITING,(LPOLECLIENT)&(pItem->oleclient),lhcDoc,
         CreateNewUniqueName(szTmp),&lpObject, olerender_draw,0))) 
      {
                                       //- next try "Static" protocol
         if (Error(OleCreateFromClip(
                  STATICP, (LPOLECLIENT)&(pItem->oleclient), lhcDoc,
                  CreateNewUniqueName(szTmp), &lpObject, olerender_draw, 0))) 
            goto Error;               //- ERROR jump
      }
   } 
   else 
   {                                   //- LINK therefore must be 
                                       // "STdFileEditing" protocol
        if (Error(OleCreateLinkFromClip(
            STDFILEEDITING,(LPOLECLIENT)&(pItem->oleclient), lhcDoc,
            CreateNewUniqueName(szTmp), &lpObject, olerender_draw, 0)))
            goto Error;                //- ERROR jump
   }

   OleQueryType(lpObject, &otObject);
   CloseClipboard();

   if (!PostItemCreate(lpObject, otObject, NULL, pItem))
      return;                          //- ERROR return

   ShowNewWindow(pItem);
   return;                             //- SUCCESS return


Error:                                 //- TAG Error

   ErrorMessage(E_GET_FROM_CLIPBOARD_FAILED);
   CloseClipboard();
   FreeAppItem(pItem);
   
   return;                             //- ERROR return

}

/***************************************************************************
 * ObjCopy()
 *
 * This function places an OLE object on the clipboard via the \
 * OleCopyToClipboard() function.
 *
 * Returns BOOL - TRUE if object successfully placed on clipboard
 **************************************************************************/

BOOL FAR ObjCopy(                      //- ENTRY:
   APPITEMPTR     pItem                //- pointer to app item
){                                     //- LOCAL:
   BOOL           fReturn = TRUE;      //- return value
   
   if (!OpenClipboard(hwndFrame)) 
      return FALSE;                    //- ERROR return

   EmptyClipboard();

   if (Error(OleCopyToClipboard(pItem->lpObject)))
      fReturn = FALSE;                 //- prepare for ERROR out

   CloseClipboard();
   return fReturn;                     //- ERROR or SUCCESS

}

/***************************************************************************
 *  ObjCreateFromTemplate()
 *
 *  Creates an embedded object from file.
 **************************************************************************/

void FAR ObjCreateFromTemplate(        //- ENTRY:
   LHCLIENTDOC    lhcDoc,              //- client document handle
   LPOLECLIENT    lpClient             //- client vtbl. pointer
){                                     //- LOCAL:
   LPOLEOBJECT    lpObject;            //- OLE object pointer
   APPITEMPTR     pItem;               //- application item pointer
   char           szTmp[CBOBJNAMEMAX]; //- temporary object name string
   char           szFileName[CBPATHMAX];//- file name string

   *szFileName = NULL;

   if (!OfnGetName(hwndFrame, szFileName, IDM_INSERTFILE))
      return;                          //- ERROR operation aborted by user

   if (!(pItem = PreItemCreate(lpClient, FALSE, lhcDoc)))
      return;                          //- ERROR
             
   if (Error(OleCreateFromTemplate(STDFILEEDITING, (LPOLECLIENT)pItem, szFileName,
         lhcDoc, CreateNewUniqueName(szTmp), &lpObject, olerender_draw, 0)))
   {
      ErrorMessage(E_CREATE_FROM_TEMPLATE);
      FreeAppItem(pItem);
      return;                          //- ERROR
   }

   PostItemCreate(lpObject, OT_EMBEDDED, NULL, pItem);

}                                      //- SUCCESS


/****************************************************************************
 * ObjGetData()
 * 
 * Get the object link data.  The data that is retrieved from OLE is copied
 * into lpLinkData if lpLinkData is not NULL.  Otherwise, space is dynamically
 * allocated or reallocated; space is allocated if pItem->lpLinkData is NULL
 * otherwise the pointer is reallocated. The data is returned is freed if
 * there has been an OLE_WARN_DELETE_DATA error.
 ***************************************************************************/

BOOL FAR ObjGetData(                   //- ENTRY:
   APPITEMPTR     pItem,               //- OLE object
   LPSTR          lpLinkData           //- pointer to linkdata
){                                     //- LOCAL:
   HANDLE         hData;               //- handle to OLE link data 
   LPSTR          lpData;              //- pointer to OLE link data
   LPSTR          lpWork;              //- copy of OLE link data
   BOOL           fFree = FALSE;       //- free OLE memory flag
   LONG           lSize;               //- size of OLE link data
   int            i;
   
   switch (Error(OleGetData(pItem->lpObject, 
      (pItem->otObject == OT_LINK ? vcfLink : vcfOwnerLink), &hData)))
   {
      case OLE_WARN_DELETE_DATA:
         fFree = TRUE;
      case OLE_OK:
         if(lpData = GlobalLock(hData))
         {
                                       //- copy the link data to new buffer
            lSize=SizeOfLinkData(lpData);
            
            if (!lpLinkData)
            {
               if (!pItem->lpLinkData)  //- allocate
                  AllocLinkData(pItem,lSize);
               else                     //- otherwise reallocate
                  ReallocLinkData(pItem,lSize);
               lpWork = pItem->lpLinkData;
            }
            else
               lpWork = lpLinkData;

            if (lpWork)
               for (i=0L; i<(int)lSize; i++)     
                  *(lpWork+i)=*(lpData+i);

            GlobalUnlock(hData);       //- free the linked data as needed
            if (fFree)
               GlobalFree(hData);

            return TRUE;               //- SUCCESS      
         }
      default:
         return FALSE;                 //- FAILURE
   }   

}

/***************************************************************************
 * ObjChangeLink()
 *
 * Change the linkdata.  This routine will change the document portion of
 * link data to lpDoc.  The old linkdata is expected to be in 
 * lpaItem->lpLinkData    
 **************************************************************************/

void FAR ObjChangeLinkData(            //- ENTRY:
   APPITEMPTR     pItem,               //- OLE object   
   LPSTR          lpDoc                //- document name
){                                     //- LOCAL:
   LONG           lSize;               //- used to link data size
   LPSTR          lpLinkData;          //- OLE link data pointer
   static char    pWork[OBJECT_LINK_MAX]; //- used to construct new link data
   int            i;                   //- index
  
   pItem->aLinkName = AddAtom(lpDoc);

   for (
      lpLinkData = pItem->lpLinkData, i=0; 
      pWork[i] = *lpLinkData; 
      lpLinkData++, i++
   );
                                       //- into working buffer.
   lstrcpy((LPSTR)&pWork[++i],lpDoc);  //- copy new document name. 
                                      
   for (; pWork[i]; i++);              //- skip to end of document name
   for (++lpLinkData;*lpLinkData;lpLinkData++);
                                       //- copy item name.
   lstrcpy((LPSTR)&pWork[++i],++lpLinkData);
   for (; pWork[i]; i++);              //- skip to end of buffer
                                       //- which is the end of item info.
   pWork[++i] = NULL;                  //- add extra null.

   lSize = SizeOfLinkData(pWork);      //- reallocate space so there is 
   ReallocLinkData(pItem,lSize);       //- a properly sized block of info
                                       //- to send the linked data to the
   if (lpLinkData = pItem->lpLinkData) //- OLE DLL.
      for (i=0; i<(int)lSize; i++)     //- copy new linkdata into this space
         *lpLinkData++ = pWork[i];
   else
      return;                          //- ERROR return
    
   Error(OleSetData(pItem->lpObject, vcfLink, 
         (HANDLE)GlobalHandle(HIWORD(pItem->lpLinkData))));
      
}                                      //- SUCCESS return

/****************************************************************************
 * ObjSaveUndo()
 *
 * Clone the OLE object so that any changes to object can be undone if the
 * user choses to exit without update.                                        
 ***************************************************************************/
  
void FAR ObjSaveUndo(                  //- ENTRY:
   APPITEMPTR     pItem                //- application item
){                                     //- LOCAL:
   char           szTmp[CBOBJNAMEMAX]; //- holder of object name
   LPSTR          lpClone;             //- pointer to clond object name
   int            i;                   //- index

   if (!pItem->lpObjectUndo)
   {
      i=CBOBJNAMEMAX;
      OleQueryName(pItem->lpObject, szTmp, &i);
                                       //- give clone a unique name by 
                                       //- altering object name prefix.
      for (lpClone = OBJCLONE, i=0; *lpClone; szTmp[i++] = *lpClone++);  

      if (Error(OleClone(pItem->lpObject, (LPOLECLIENT)pItem,
         pItem->lhcDoc, szTmp, &(pItem->lpObjectUndo))))
      return;                          //- ERROR return

      pItem->otObjectUndo  = pItem->otObject;
      pItem->uoObjectUndo  = pItem->uoObject;
      pItem->aLinkUndo     = pItem->aLinkName;

      GetClientRect(pItem->hwnd, &pItem->rect);

      if (OleQueryOpen(pItem->lpObject) == OLE_OK)
         pItem->fOpen = TRUE;
      
   }

}                                      //- SUCCESS return

/****************************************************************************
 * ObjUndo()
 *
 * Restore an object to its state before changes.  The lpObject Undo is a 
 * clone to the original object with a different name, therefore, all we
 * have to do is rename that object and ditch the changed object.
 ***************************************************************************/
  
void FAR ObjUndo(                      //- ENTRY:
   APPITEMPTR     pItem                //- application item
){                                     //- LOCAL:
   char           szTmp[CBOBJNAMEMAX]; //- object name holder
   int            i;                   //- index

   OleQueryName(pItem->lpObject, szTmp, &i);
   if (Error(OleDelete(pItem->lpObject)))
      return;                          //- ERROR return
                                       //- reset app item vars
   pItem->lpObject      = pItem->lpObjectUndo;      
   pItem->otObject      = pItem->otObjectUndo;
   pItem->uoObject      = pItem->uoObjectUndo;
   pItem->aLinkName     = pItem->aLinkUndo;
   pItem->lpObjectUndo  = (LPOLEOBJECT)NULL;
   pItem->otObjectUndo  = (LONG)NULL;

   if (Error(OleRename(pItem->lpObject,szTmp)))
      return;                          //- ERROR return

   if (pItem->fOpen)
   {
      Error(OleReconnect(pItem->lpObject));
      pItem->fOpen = FALSE;
   }

   SetWindowPos(
      pItem->hwnd, 
      NULL, 0, 0,
      pItem->rect.right - pItem->rect.left + 2*GetSystemMetrics(SM_CXFRAME),
      pItem->rect.bottom - pItem->rect.top + 2*GetSystemMetrics(SM_CYFRAME),
      SWP_NOZORDER | SWP_NOMOVE | SWP_DRAWFRAME
   );

   InvalidateRect(pItem->hwnd,NULL,TRUE);

}                                      //- SUCCESS return


/****************************************************************************
 * ObjDelUndo()
 *
 * Delete the undo object if the user is happy with the changes he/she made.
 ***************************************************************************/

void FAR ObjDelUndo(                   //- ENTRY:
   APPITEMPTR     pItem                //- application item
){

   if (Error(OleDelete(pItem->lpObjectUndo)))
      return;                          //- ERROR return

   pItem->lpObjectUndo = (LPOLEOBJECT)NULL;
   pItem->otObjectUndo = (LONG)NULL;
   DeleteAtom(pItem->aLinkUndo);
   pItem->lpObjectUndo = NULL;
   
}                                      //- SUCCESS return

/****************************************************************************
 * ObjFreeze()
 *
 * Convert an object to a static object.
 ***************************************************************************/
  
void FAR ObjFreeze(                    //- ENTRY:
   APPITEMPTR     pItem                //- application item
){                                     //- LOCAL:
   char           szTmp[CBOBJNAMEMAX]; //- temporary object name
   LPSTR          lpTemp;              //- temporary prefix string
   LPOLEOBJECT    lpObjectTmp;         //- temporary object pointer
   int            i;                   //- index

   i=CBOBJNAMEMAX;
   OleQueryName(pItem->lpObject, szTmp, &i);
                                       //- create a unique name by changing 
                                       //- the object name prefix
   for (lpTemp = OBJTEMP, i=0; *lpTemp; szTmp[i++] = *lpTemp++);  

                                       //- this API creates a static object
   if (Error(OleObjectConvert(pItem->lpObject, STATICP, (LPOLECLIENT)pItem,
      pItem->lhcDoc, szTmp, &lpObjectTmp)))
      return;
                                       //- delete old object
   if (Error(OleDelete(pItem->lpObject)))
      return;

   WaitForObject(pItem);  

   pItem->lpObject = lpObjectTmp;
   pItem->otObject = OT_STATIC;
   pItem->uoObject = -1L;

   for (lpTemp = OBJPREFIX, i=0; *lpTemp; szTmp[i++] = *lpTemp++);  
   if (Error(OleRename(pItem->lpObject,szTmp)))
      return;
   
   
}

/***************************************************************************
 *  ObjCreateWrap()
 *
 * Create a wrapped object from the drag and drop feature of the 3.1 shell.
 * NOTE: We are assuming that only one file has been dropped.  See the SDK
 * documentation for instructions on how to deal with multiple files.
 ***************************************************************************/

void FAR ObjCreateWrap(                //- ENTRY:
   HANDLE         hdrop,               //- handle to dropped object
   LHCLIENTDOC    lhcDoc,              //- document handle
   LPOLECLIENT    lpClient             //- pointer to client structure
){                                     //- LOCAL:
   char           szDragDrop[CBPATHMAX];//- Drag and drop file name 
   LPOLEOBJECT    lpObject;            //- pointer to OLE object 
   POINT          pt;                  //- position of dropped object
   RECT           rc;                  //- object size and position 
   char           szTmp[CBOBJNAMEMAX]; //- buffer for unique object name 
   APPITEMPTR     pItem;               //- application item pointer
   int            x,y;                 //- icon sizes
      
   x = GetSystemMetrics(SM_CXICON) / 2;
   y = GetSystemMetrics(SM_CYICON) / 2;
                                       //- Get the drag and drop filename
                                       //- position 
   DragQueryPoint(hdrop, &pt);
   DragQueryFile(hdrop, 0, szDragDrop, CBPATHMAX);
   DragFinish(hdrop);

   SetRect(&rc, pt.x - x, pt.y - y, pt.x + x, pt.y + y);

   if (!(pItem = PreItemCreate(lpClient, TRUE, lhcDoc)))
      return;                          //- ERROR return
                                       //- create OLE object
   if (Error(OleCreateFromFile(STDFILEEDITING, (LPOLECLIENT)pItem, 
         "Package", szDragDrop, lhcDoc, CreateNewUniqueName(szTmp), 
         &lpObject, olerender_draw, 0)))
   {
      ErrorMessage(E_FAILED_TO_CREATE_OBJECT);
      FreeAppItem(pItem);
      return;                          //- ERROR return
   }

   if (PostItemCreate(lpObject, OT_EMBEDDED, &rc, pItem))    
      ShowNewWindow(pItem);
   
}                                      //- SUCCESS return

/***************************************************************************
 *  UpdateObjectMenuItem()
 *
 *  Add an object popup menu for the chosen object if multiple verbs exist.
 *  The registration system is used to determine which verbs exist for the
 *   given object. 
 **************************************************************************/

void FAR UpdateObjectMenuItem(         //- ENTRY:
   HMENU       hMenu                   //- main menu
){                                     //- LOCAL
   int         cVerbs;                 //- verb
   APPITEMPTR  pItem;                  //- application item ponter
   DWORD       dwSize = KEYNAMESIZE;
   char        szClass[KEYNAMESIZE], szBuffer[200];
   char        szVerb[KEYNAMESIZE];
   HANDLE      hPopupNew=NULL;
   HKEY        hkeyTemp;
   char        pLinkData[OBJECT_LINK_MAX];
                                       //- delete current item and submenu 
   DeleteMenu(hMenu, POS_OBJECT, MF_BYPOSITION );
   
   if (!(pItem = GetTopItem()) )
      goto Error;                      //- ERROR jump
   else if (!pItem->fVisible)
      goto Error;                      //- ERROR jump
                                       //- if STATIC ?
   if ((pItem->otObject != OT_EMBEDDED) && (pItem->otObject != OT_LINK)) 
      goto Error;                      //- ERROR jump

   if (!ObjGetData(pItem, pLinkData))  //- get linkdata as key reg database
      goto Error;                      //- ERROR jump
                                       //- open reg database   
   if (RegOpenKey(HKEY_CLASSES_ROOT, NULL, &hkeyTemp)) 
      goto Error;                      //- ERROR jump
                                       //- check if class is reg-db
   if (RegQueryValue(HKEY_CLASSES_ROOT, pLinkData, szClass, &dwSize))
   {
      RegCloseKey(hkeyTemp);
      goto Error;                      //- ERROR jump    
   }

   for (cVerbs=0; ;++cVerbs)           //- extract all verbs from reg-db
   {
      dwSize = KEYNAMESIZE;
      wsprintf(szBuffer, "%s\\protocol\\StdFileEditing\\verb\\%d",
                                     (LPSTR)pLinkData,cVerbs);

      if (RegQueryValue(HKEY_CLASSES_ROOT, szBuffer, szVerb, &dwSize))
         break;

      if (!hPopupNew)
         hPopupNew = CreatePopupMenu();

      InsertMenu(hPopupNew,(UINT) -1, (UINT)MF_BYPOSITION,(UINT) IDM_VERBMIN+cVerbs, szVerb);
   }

   //- NOTE: For International versions the following  verb menu
   //- may need to be formatted differently.

   switch (cVerbs)                     //- determine how many verbs found
   {
      case 0:                          //- none
         wsprintf(szBuffer, "Edit %s %s", (LPSTR)szClass, (LPSTR)"&Object");
         InsertMenu(hMenu, POS_OBJECT, MF_BYPOSITION, IDM_VERBMIN, szBuffer);
         break;

      case 1:                          //- one
         wsprintf(szBuffer, "%s %s %s", (LPSTR)szVerb, (LPSTR)szClass, 
            (LPSTR)"&Object");
         DestroyMenu(hPopupNew);
         InsertMenu(hMenu, POS_OBJECT, MF_BYPOSITION, IDM_VERBMIN, szBuffer);
         break;

     default:                          //- > 1
         wsprintf(szBuffer, "%s %s", (LPSTR)szClass, (LPSTR)"&Object");
         InsertMenu(hMenu, POS_OBJECT, MF_BYPOSITION | MF_POPUP, hPopupNew, szBuffer);
         EnableMenuItem(hMenu, POS_OBJECT, MF_ENABLED|MF_BYPOSITION);
         break;           
   }

   RegCloseKey(hkeyTemp);              //- close reg-db
   return;                             //- SUCCESS return

Error:                                 //- ERROR tag
   
   InsertMenu(hMenu, POS_OBJECT, MF_BYPOSITION, NULL, "&Object");
   EnableMenuItem(hMenu, POS_OBJECT, MF_GRAYED | MF_BYPOSITION);

}                                      //- ERROR return

/***************************************************************************
 *  ExecuteVerb()
 *
 *  Execute the verb for the given object.
 ***************************************************************************/

void FAR ExecuteVerb(                  //- ENTRY:
   int iVerb,                          //- verb
   APPITEMPTR pItem                    //- application item pointer
){                                     //- LOCAL
   RECT        rc;                     //- holds client area bounding rect

   if (pItem->otObject == OT_STATIC)   //- if the object is static beep
   {
      ErrorMessage(W_STATIC_OBJECT);
      return;                          //- return
   }
                                       //- get cliet area rectangle
   GetClientRect(hwndFrame, (LPRECT)&rc);
                                       //- execute OLE verb
   if (Error(OleActivate(pItem->lpObject, iVerb, TRUE, TRUE, hwndFrame, &rc))) 
      return;
  
   WaitForObject(pItem);               //- wait for async. operation
 

}                                      //- SUCCESS return

/****************************************************************************
 * ObjSetBounds
 *
 * Set the object bounds.  The object bounds are the child windos bounding
 * rectangle.  OLE servers recieve need the bounding rectangle in HIMETRIC
 * coordinates.  So, we convert from screen coordinates to HIMETRIC.
 *
 * Returns BOOL - TRUE if successful.
 ***************************************************************************/

BOOL FAR ObjSetBounds(                 //- ENTRY:
   APPITEMPTR     pItem                //- application item pointer 
){                                     //- LOCAL:
   RECT           itemRect;            //- bounding rectangle      

   GetWindowRect(pItem->hwnd,&itemRect);//- get item window react

   itemRect.right -= GetSystemMetrics(SM_CXFRAME);
   itemRect.left += GetSystemMetrics(SM_CXFRAME);
   itemRect.top += GetSystemMetrics(SM_CYFRAME);
   itemRect.bottom -= GetSystemMetrics(SM_CYFRAME);

   itemRect.right  = MulDiv ((itemRect.right - itemRect.left),
                        HIMETRIC_PER_INCH, giXppli);
   itemRect.bottom = - MulDiv((itemRect.bottom - itemRect.top),
                        HIMETRIC_PER_INCH, giYppli);    
   itemRect.top    = NULL;
   itemRect.left   = NULL;
                                       //- set the rect for the server
   if (Error(OleSetBounds(pItem->lpObject,(LPRECT)&itemRect)))
      return FALSE;                    //- ERROR return

   WaitForObject(pItem);               //- wait for async. operation
   return TRUE;                        //- SUCCESS return

}
