/*
  OLE SERVER DEMO           
  Server.c             
                                                                     
  This file contains server methods and various server-related support 
  functions.
                                                                     
  (c) Copyright Microsoft Corp. 1990 - 1992  All Rights Reserved   
*/                                                                     
 


#define SERVERONLY
#include "Windows.h"
#include "Ole.h"
#include "SrvrDemo.h"

/* 
   Important Note:

   No method should ever dispatch a DDE message or allow a DDE message to
   be dispatched.
   Therefore, no method should ever enter a message dispatch loop.
   Also, a method should not show a dialog or message box, because the 
   processing of the dialog box messages will allow DDE messages to be
   dispatched.
*/


/* Abbrev
 * ------
 *
 * Return a pointer to the filename part of a fully-qualified pathname.
 *
 * LPSTR lpsz - Fully qualified pathname
 * 
 * CUSTOMIZATION: May be useful, but not necessary.
 *
 */
LPSTR Abbrev (LPSTR lpsz)
{
   LPSTR lpszTemp;
   
   lpszTemp = lpsz + lstrlen(lpsz) - 1;
   while (lpszTemp > lpsz && lpszTemp[-1] != '\\')
      lpszTemp--;
   return lpszTemp;
}



/* FreeVTbls
 * ---------
 *
 * Free the instances of all the OLE methods created by InitVTbls.
 *
 * 
 * CUSTOMIZATION: Your application might not use global variables for srvrvtbl,
 *                docvtbl, and objvtbl.
 */
void FreeVTbls (void)
{
   FreeProcInstance (srvrvtbl.Create);
   FreeProcInstance (srvrvtbl.CreateFromTemplate);
   FreeProcInstance (srvrvtbl.Edit);
   FreeProcInstance (srvrvtbl.Exit);
   FreeProcInstance (srvrvtbl.Open);
   FreeProcInstance (srvrvtbl.Release);

   FreeProcInstance (docvtbl.Close);
   FreeProcInstance (docvtbl.GetObject);
   FreeProcInstance (docvtbl.Release);
   FreeProcInstance (docvtbl.Save);
   FreeProcInstance (docvtbl.SetColorScheme);
   FreeProcInstance (docvtbl.SetDocDimensions);
   FreeProcInstance (docvtbl.SetHostNames);

   FreeProcInstance (objvtbl.DoVerb);
   FreeProcInstance (objvtbl.EnumFormats);
   FreeProcInstance (objvtbl.GetData);
   FreeProcInstance ((FARPROC)objvtbl.QueryProtocol);
   FreeProcInstance (objvtbl.Release);
   FreeProcInstance (objvtbl.SetBounds);
   FreeProcInstance (objvtbl.SetColorScheme);
   FreeProcInstance (objvtbl.SetData);
   FreeProcInstance (objvtbl.SetTargetDevice);
   FreeProcInstance (objvtbl.Show);
}



/* InitServer
 * ----------
 *
 * Initialize the server by allocating memory for it, and calling
 * the OleRegisterServer method.  Requires that the server method table
 * has been properly initialized.
 * 
 * HWND hwnd      - Handle to the main window
 * LPSTR lpszLine - The Windows command line
 * 
 * RETURNS: TRUE if the memory could be allocated, and the server
 *          was properly registered.
 *          FALSE otherwise
 *
 * CUSTOMIZATION: Your application might not use a global variable 
 *                for srvrMain.
 *
 */
BOOL InitServer (HWND hwnd, HANDLE hInst)
{
    srvrMain.olesrvr.lpvtbl = &srvrvtbl;

    if (OLE_OK != OleRegisterServer
         (szClassName, (LPOLESERVER) &srvrMain, &srvrMain.lhsrvr, hInst, 
          OLE_SERVER_MULTI))
      return FALSE;
    else
      return TRUE;
}



/* InitVTbls
 * ---------
 *
 * Create procedure instances for all the OLE methods.
 * 
 * 
 * CUSTOMIZATION: Your application might not use global variables for srvrvtbl,
 *                docvtbl, and objvtbl.
 */
void InitVTbls (void)
{
   typedef LPVOID (FAR PASCAL *LPVOIDPROC) (LPOLEOBJECT, LPSTR);

   // Server method table
   (FARPROC)srvrvtbl.Create	     = MakeProcInstance (SrvrCreate,	      hInst);
   (FARPROC)srvrvtbl.CreateFromTemplate
                            = MakeProcInstance (SrvrCreateFromTemplate,hInst);
   (FARPROC)srvrvtbl.Edit	     = MakeProcInstance (SrvrEdit,	      hInst);
   (FARPROC)srvrvtbl.Execute	     = MakeProcInstance (SrvrExecute,	      hInst);
   (FARPROC)srvrvtbl.Exit	     = MakeProcInstance (SrvrExit,	      hInst);
   (FARPROC)srvrvtbl.Open	     = MakeProcInstance (SrvrOpen,	      hInst);
   (FARPROC)srvrvtbl.Release	     = MakeProcInstance (SrvrRelease,	      hInst);

   // Document method table
   (FARPROC)docvtbl.Close	     = MakeProcInstance (DocClose,	      hInst);
   (FARPROC)docvtbl.GetObject	     = MakeProcInstance (DocGetObject,	      hInst);
   (FARPROC)docvtbl.Execute	     = MakeProcInstance (DocExecute,	      hInst);
   (FARPROC)docvtbl.Release	     = MakeProcInstance (DocRelease,	      hInst);
   (FARPROC)docvtbl.Save	     = MakeProcInstance (DocSave,	      hInst);
   (FARPROC)docvtbl.SetColorScheme   = MakeProcInstance (DocSetColorScheme,   hInst);
   (FARPROC)docvtbl.SetDocDimensions = MakeProcInstance (DocSetDocDimensions, hInst);
   (FARPROC)docvtbl.SetHostNames     = MakeProcInstance (DocSetHostNames,     hInst);

   // Object method table
   (FARPROC)objvtbl.DoVerb	     = MakeProcInstance (ObjDoVerb,	      hInst);
   (FARPROC)objvtbl.EnumFormats      = MakeProcInstance (ObjEnumFormats,      hInst);
   (FARPROC)objvtbl.GetData	     = MakeProcInstance (ObjGetData,	      hInst);
   (FARPROC)objvtbl.QueryProtocol    = MakeProcInstance
				 ((FARPROC)ObjQueryProtocol,hInst);
   (FARPROC)objvtbl.Release	     = MakeProcInstance (ObjRelease,	      hInst);
   (FARPROC)objvtbl.SetBounds	     = MakeProcInstance (ObjSetBounds,	      hInst);
   (FARPROC)objvtbl.SetColorScheme   = MakeProcInstance (ObjSetColorScheme,   hInst);
   (FARPROC)objvtbl.SetData	     = MakeProcInstance (ObjSetData,	      hInst);
   (FARPROC)objvtbl.SetTargetDevice  = MakeProcInstance (ObjSetTargetDevice,  hInst);
   (FARPROC)objvtbl.Show	     = MakeProcInstance (ObjShow,	      hInst);
}



/* SetTitle 
 * --------
 *
 * Sets the main window's title bar. The format of the title bar is as follows
 *   
 * If embedded
 *        <Server App name> - <object type> in <client doc name>
 *   
 *      Example:  "Server Demo - SrvrDemo Shape in OLECLI.DOC"
 *                where OLECLI.DOC is a Winword document
 *   
 * otherwise
 *        <Server App name> - <server document name>     
 *   
 *      Example:  "Server Demo - OLESVR.SD" 
 *                where OLESVR.SD is a Server demo document
 *
 * LPSTR lpszDoc    - document name
 * BOOL  fEmbedded  - If TRUE embedded document, else normal document      
 * 
 * RETURNS: OLE_OK
 *
 * 
 * CUSTOMIZATION: Your application may store the document's name somewhere
 *                other than docMain.aName.  Other than that, you may
 *                find this a useful utility function as is.
 *
 */
void SetTitle (LPSTR lpszDoc, BOOL fEmbedded)
{
   char szBuf[cchFilenameMax];

   if (lpszDoc && lpszDoc[0])
   {
      // Change document name.
      if (docMain.aName)
         GlobalDeleteAtom (docMain.aName);
      docMain.aName = GlobalAddAtom (lpszDoc);
   }

   if (fEmbedded)
   {
     // 
      if (lpszDoc && lpszDoc[0]) 
      {
         wsprintf (szBuf, "%s - SrvrDemo Shape in %s", (LPSTR) szAppName, 
             Abbrev (lpszDoc));
      }
      else
      {
         // Use name from docMain
         char szDoc [cchFilenameMax];
     
         GlobalGetAtomName (docMain.aName, szDoc, cchFilenameMax);
         wsprintf (szBuf, "%s - SrvrDemo Shape in %s", (LPSTR) szAppName, 
             Abbrev (szDoc));
      }
      SetWindowText (hwndMain, (LPSTR)szBuf);
   } 
   else if (lpszDoc && lpszDoc[0])
   {
      wsprintf (szBuf, "%s - %s", (LPSTR) szAppName, Abbrev(lpszDoc));
      SetWindowText (hwndMain, szBuf);
   }
}




/* SrvrCreate                SERVER "Create" METHOD
 * ----------
 *
 * Create a document, allocate and initialize the OLESERVERDOC structure,
 * and associate the library's handle with it.
 * In this demo server, we also create an object for the user to edit.
 * 
 * LPOLESERVER lpolesrvr          - The server structure registered by
 *                                  the application
 * LHSERVERDOC lhdoc              - The library's handle
 * LPSTR lpszClassName            - The class of document to create
 * LPSTR lpszDoc                  - The name of the document
 * LPOLESERVERDOC FAR *lplpoledoc - Indicates the server doc structure to be
 *                                  created
 * 
 * RETURNS:        OLE_OK if the named document was created.
 *                 OLE_ERROR_NEW if the document could not be created.
 * 
 * CUSTOMIZATION: Your application might not call CreateNewObj.
 *
 */
OLESTATUS FAR PASCAL SrvrCreate
   (LPOLESERVER lpolesrvr, LHSERVERDOC lhdoc, LPSTR lpszClassName, 
    LPSTR lpszDoc, LPOLESERVERDOC FAR *lplpoledoc)
{
    if (!CreateNewDoc (lhdoc, lpszDoc, doctypeEmbedded)) 
        return OLE_ERROR_NEW;

    // Although the document has not actually been changed, the client has not
    // received any data from the server yet, so the client will need to be
    // updated.  Therefore, CreateNewObj sets fDocChanged to TRUE.
    CreateNewObj (TRUE);
    *lplpoledoc = (LPOLESERVERDOC) &docMain;
    EmbeddingModeOn();
    return OLE_OK;
}



/* SrvrCreateFromTemplate        SERVER "CreateFromTemplate" METHOD
 * ----------------------
 *
 * Create a document, allocate and initialize the OLESERVERDOC structure, 
 * initializing the document with the contents named in the template name, 
 * and associate the library's handle with the document structure.
 * 
 * LPOLESERVER lpolesrvr        - The server structure registered by
 *                                the application
 * LHSERVERDOC lhdoc            - The library's handle
 * LPSTR lpszClassName          - The class of document to create
 * LPSTR lpszDoc                - The name of the document
 * LPSTR lpszTemplate           - The name of the template
 * LPOLESERVERDOC FAR *lplpoledoc - Indicates the server doc structure 
 *                                  to be created
 * 
 * RETURNS:        OLE_OK if the named document was created.
 *                 OLE_ERROR_TEMPLATE if the document could not be created.
 *
 * CUSTOMIZATION: None
 *
 */
OLESTATUS FAR PASCAL SrvrCreateFromTemplate
   (LPOLESERVER lpolesrvr, LHSERVERDOC lhdoc, LPSTR lpszClassName, 
    LPSTR lpszDoc, LPSTR lpszTemplate, LPOLESERVERDOC FAR *lplpoledoc)
{
    if (!CreateDocFromFile(lpszTemplate, lhdoc, doctypeEmbedded)) 
        return OLE_ERROR_TEMPLATE;

    *lplpoledoc = (LPOLESERVERDOC) &docMain;

    // Although the document has not actually been changed, the client has not
    // received any data from the server yet, so the client will need to be
    // updated.
    fDocChanged = TRUE;
    EmbeddingModeOn();
    return OLE_OK;
}



/* SrvrEdit                SERVER "Edit" METHOD
 * --------
 *
 * A request by the libraries to create a document, allocate and
 * initialize the OLESERVERDOC structure, and associate the
 * library's handle with the document structure.
 * We create an object which will be modified by the SetData method
 * before the user has a chance to touch it.
 * 
 * LPOLESERVER lpolesrvr          - The server structure registered by
 *                                  the application
 * LHSERVERDOC lhdoc              - The library's handle
 * LPSTR lpszClassName            - The class of document to create
 * LPSTR lpszDoc                  - The name of the document
 * LPOLESERVERDOC FAR *lplpoledoc - Indicates the server doc structure to be
 *                                  created
 * 
 * RETURNS:        OLE_OK if the named document was created.
 *                 OLE_ERROR_EDIT if the document could not be created.
 * 
 * CUSTOMIZATION: None
 *
 */
OLESTATUS FAR PASCAL SrvrEdit 
   (LPOLESERVER lpolesrvr, LHSERVERDOC lhdoc, LPSTR lpszClassName, 
    LPSTR lpszDoc, LPOLESERVERDOC FAR *lplpoledoc)
{
    if (!CreateNewDoc (lhdoc, lpszDoc, doctypeEmbedded))
        return OLE_ERROR_EDIT;

    // The client is creating an embedded object for the server to edit,
    // so initially the client and server are in sync.
    fDocChanged = FALSE;
    *lplpoledoc = (LPOLESERVERDOC) &docMain;
    EmbeddingModeOn();
    return OLE_OK;

}


/* SrvrExecute                SERVER "Execute" METHOD
 * --------
 *
 * This application does not support the execution of DDE execution commands.
 * 
 * LPOLESERVER lpolesrvr - The server structure registered by
 *                         the application
 * HANDLE hCommands      - DDE execute commands
 * 
 * RETURNS: OLE_ERROR_COMMAND
 *
 * CUSTOMIZATION: Re-implement if your application supports the execution of
 *                DDE commands.
 *
 */
OLESTATUS FAR PASCAL SrvrExecute (LPOLESERVER lpolesrvr, HANDLE hCommands)
{
   return OLE_ERROR_COMMAND;
}



/* SrvrExit                SERVER "Exit" METHOD
 * --------
 *
 * This method is called the library to instruct the server to exit.
 * 
 * LPOLESERVER lpolesrvr - The server structure registered by
 *                         the application
 * 
 * RETURNS: OLE_OK
 * 
 * CUSTOMIZATION: None
 *
 */
OLESTATUS FAR PASCAL SrvrExit (LPOLESERVER lpolesrvr)
{
   if (srvrMain.lhsrvr)
   // If we haven't already tried to revoke the server.
   {
      StartRevokingServer();
   }
   return OLE_OK;
}



/* SrvrOpen                SERVER "Open" METHOD
 * --------
 *
 * Open the named document, allocate and initialize the OLESERVERDOC 
 * structure, and associate the library's handle with it.
 * 
 * LPOLESERVER lpolesrvr          - The server structure registered by
 *                                  the application
 * LHSERVERDOC lhdoc              - The library's handle
 * LPSTR lpszDoc                  - The name of the document
 * LPOLESERVERDOC FAR *lplpoledoc - Indicates server doc structure to be
 *                                  created
 * 
 * RETURNS:        OLE_OK if the named document was opened.
 *                 OLE_ERROR_OPEN if document could not be opened correctly.
 * 
 * CUSTOMIZATION: None
 *
 */
OLESTATUS FAR PASCAL SrvrOpen (LPOLESERVER lpolesrvr, LHSERVERDOC lhdoc,
                               LPSTR lpszDoc, LPOLESERVERDOC FAR *lplpoledoc)
{
    if (!CreateDocFromFile (lpszDoc, lhdoc, doctypeFromFile))
        return OLE_ERROR_OPEN;

    *lplpoledoc = (LPOLESERVERDOC) &docMain;
    return OLE_OK;
}



/* SrvrRelease                SERVER "Release" METHOD
 * -----------
 *
 * This library calls the SrvrRelease method when it is safe to quit the
 * application.  Note that the server application is not required to quit.
 * 
 * srvrMain.lhsrvr != NULL indicates that SrvrRelease has been called
 * because the client is no longer connected, not because the server called
 * OleRevokeServer.
 * Therefore, only start the revoking process if the document is of type
 * doctypeEmbedded or if the server was opened for an invisible update.
 * 
 * srvrmain.lhsrvr == NULL indicates that OleRevokeServer has already 
 * been called (by the server application), and srvrMain is a lame duck.
 * It is safe to quit now because SrvrRelease has just been called.
 *
 * Note that this method may be called twice: when OleRevokeServer is 
 * called in StartRevokingServer, SrvrRelease is called again.  
 * Therefore we need to be reentrant.
 * 
 * LPOLESERVER lpolesrvr - The server structure to release
 * 
 * RETURNS: OLE_OK
 * 
 * CUSTOMIZATION: None
 *
 */
OLESTATUS FAR PASCAL SrvrRelease (LPOLESERVER lpolesrvr)
{
   if (srvrMain.lhsrvr)
   {
      if (fRevokeSrvrOnSrvrRelease 
          && (docMain.doctype == doctypeEmbedded 
              || !IsWindowVisible (hwndMain)))
         StartRevokingServer();
   }
   else      
   {
      fWaitingForSrvrRelease = FALSE;
      // Here you should free any memory that had been allocated for the server.
      PostQuitMessage (0);
   }
   return OLE_OK;
}



/* StartRevokingServer
 * -------------------
 *
 * Hide the window, and start to revoke the server.  
 * Revoking the server will let the library close any registered documents.
 * OleRevokeServer may return OLE_WAIT_FOR_RELEASE.
 * Calling StartRevokingServer starts a chain of events that will eventually
 * lead to the application being terminated.
 *
 * RETURNS: The return value from OleRevokeServer
 *
 * CUSTOMIZATION: None
 *
 */
OLESTATUS StartRevokingServer (void)
{
   OLESTATUS olestatus;

   if (srvrMain.lhsrvr)
   {
      LHSERVER lhserver;
      // Hide the window so user can do nothing while we are waiting. 
      ShowWindow (hwndMain, SW_HIDE);
      lhserver = srvrMain.lhsrvr;
      // Set lhsrvr to NULL to indicate that srvrMain is a lame duck and that
      // if SrvrRelease is called, then it is ok to quit the application.
      srvrMain.lhsrvr = NULL;
      olestatus = OleRevokeServer (lhserver);
   }
   else
      // The programmer should ensure that this never happens.
      ErrorBox ("Fatal Error: StartRevokingServer called on NULL server.");
   return olestatus;
}
