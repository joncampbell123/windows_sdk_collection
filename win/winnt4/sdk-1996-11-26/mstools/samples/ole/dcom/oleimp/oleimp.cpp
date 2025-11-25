// ===========================================================================
// File: O L E I M P . C P P
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
// 
//  The OLEIMP sample demonstrates a method for emulating auto impersonation
// of clients. Normally, objects created on the server will run in the
// context of the server.  An object method must explicitly impersonate the
// client if the method is to run in the context of the client.  This
// sample demonstrates a way to arrange for methods to "automatically" run
// in the context of the client.
// 
// This program is based on the oleapt sample.  It uses a set of apartment
// threads that each potentially impersonate a client. All objects created
// for a user run in the same apartment.  The security for the server
// is explicitly set up to allow the world call access.  Launch permission
// will still need to be set using the DCOMCNFG.EXE utility if you want
// other than the default launch permission.
// 
// The server registers a class-factory on the main application thread and
// also spawns several worker threads. When requests arrive from clients
// to create instances of the class, the server class-factory picks a
// worker thread for the object in the following manner:
// 
//   - If there is a thread currently impersonating that user, use that
//     thread.
//   - Otherwise, if there is a free thread, set that thread up to
//     impersonate the client and use that thread.
//   - Otherwise fail the CreateInstance call.
// 
// The server then goes through the process of having the object created
// within the thread and properly marshalled from the worker thread
// back to the class factory where it can be returned to the caller. Note
// that this marshalling to the main thread is fairly transient, it lasts
// for creation only: subsequent calls from the client to the object go
// straight from the client's process into the worker apartment.
// 
// To register this server run OLEIMP.EXE with the -AutoRegister command
// line switch.  OLEIMP.EXE registers itself to run as the interactive
// user.  For more information about the activation options for DCOM
// applications see the DCOM document on the Win32 SDK CD in the
// \doc\spec\ole directory.
//
// This sample may be compiled as either UNICODE or ANSI.
// 
// Copyright 1996 Microsoft Corporation.  All Rights Reserved.
// ===========================================================================

// %%Includes: ---------------------------------------------------------------
#define INC_OLE2
#define STRICT
#include <windows.h>
#include <initguid.h>
#include <stdio.h>
#include <string.h>
#include <tchar.h>
#include "util.h"

// %%Constants: --------------------------------------------------------------
#define cServerThreads  5
#define szWindowClass   TEXT("OLEIMP_CLASS")
#define szTitleServer   TEXT("SERVER: OLE Apartment Impersonation Sample")
#define szCloseQuery    TEXT("Client references still exist. Are you sure you want to close?")
#define COINIT_APARTMENTTHREADED    2
#define MAX_USER_NAME	50
const LARGE_INTEGER     bZero = {0,0};

// %%Guids: ------------------------------------------------------------------
DEFINE_GUID(CLSID_CObject, 0x35b79d1, 0xd6d3, 0x11cf, 0xb9, 0xd4, 0x0, 0xaa, 0x0, 0xa2, 0x16, 0xe0);

// %%Classes: ----------------------------------------------------------------
// the class-factory object exists in the main application apartment/thread
// and is used to create instances of the worker objects on worker threads.
class CClassFactory : public IClassFactory
    {
  // IClassFactory
    STDMETHODIMP            QueryInterface(REFIID iid, void **ppv);
    STDMETHODIMP_(ULONG)    AddRef(void)    { return mcRef++; }
    STDMETHODIMP_(ULONG)    Release(void)   { if (--mcRef == 0) { delete this; return 0; } return mcRef; }
    STDMETHODIMP            CreateInstance(LPUNKNOWN punkOuter, REFIID iid, LPVOID FAR *ppv);
    STDMETHODIMP            LockServer(BOOL fLock);

  // data members
    ULONG   mcRef;

  // constructors/destructors
  public:
    CClassFactory() { mcRef = 1; }
    };

// this worker object is simple: it simply supports IUnknown. more interesting
// interfaces can be readily added here and implemented for the worker.
class CObject : public IUnknown
    {
  // IUnknown
    STDMETHODIMP            QueryInterface(REFIID iid, void **ppv);
    STDMETHODIMP_(ULONG)    AddRef(void)    { return mcRef++; }
    STDMETHODIMP_(ULONG)    Release(void);

  // data members
    ULONG   mcRef;

  // constructors/destructors
  public:
    CObject() { mcRef = 1; }
    };

// %%Globals: ----------------------------------------------------------------
BOOL        vfServer;                       // is this instance a server or client?
HANDLE      vrghThread[cServerThreads];     // worker thread handles
DWORD       vrgtid[cServerThreads];         // worker thread id's
HANDLE      vrghEvent[cServerThreads];      // creation event for each worker
INT         vrgcObjects[cServerThreads];    // # of objects created on each thread
PSID		vrgpSid[cServerThreads];		// SID of user in each apt.
LPTSTR		vrgpszAptUser[cServerThreads];	// name of each apt. user
HANDLE      vhEventServer;                  // creation-complete event for class-factory
CClassFactory   vclassfactory;
HWND        vhwndMain;
LPSTREAM    vpstmMarshalling;               // scratch stream used for cross-apt marshalling
HRESULT     vhrThreadStatus;                // signals status to class-factory

// %%Prototypes: -------------------------------------------------------------
LRESULT CALLBACK    MainWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT             ServerThreadProc(LPARAM lParam);
BOOL                FAutoRegister(HINSTANCE hinst);
BOOL				FindSlot(PSID psid, int *piThread);

// ---------------------------------------------------------------------------
// %%Function: MainWndProc
// ---------------------------------------------------------------------------
 LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT lresult = TRUE;
	int i;

    switch (message)
        {
        case WM_PAINT:
            {
            HDC hdc;
            PAINTSTRUCT ps;
            int i;

            if ((hdc = BeginPaint(hwnd, &ps)) != NULL)
                {
                for (i=0; i<cServerThreads; i++)
                    {
                    TCHAR    rgch[200];

					// if a thread which is impersonating a user no
					// longer has any objects, free up the resources
					// and stop impersonating
					if (0 == vrgcObjects[i] && NULL != vrgpSid[i])
						{
							UtilFreeMem(vrgpszAptUser[i]);
							vrgpszAptUser[i] = NULL;
							UtilFreeMem(vrgpSid[i]);
							vrgpSid[i] = NULL;
							SetThreadToken (&vrghThread[i], NULL);
						}

                    wsprintf(rgch, TEXT("Thread #%d (%s): %d objects"),
                    			i,
                    			(NULL == vrgpszAptUser[i])?
                    				TEXT("No User") :
                    				vrgpszAptUser[i],
                    			vrgcObjects[i]);
                    TextOut(hdc, 20, 10+(i*25), rgch, lstrlen(rgch));
                    }
                }
            EndPaint(hwnd, &ps);
            }
            break;


        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_CLOSE:
            // check for clients and prompt in the UI.
            for (i=0; i<cServerThreads; i++)
                {
                if (vrgcObjects[i] > 0)
                    {
                    if (MessageBox(hwnd, szCloseQuery,
                            szTitleServer, MB_YESNO) == IDNO)
                        return FALSE;
                    else
                        break;
                    }
                }

        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
        }

    return lresult;
}  // MainWndProc

// ---------------------------------------------------------------------------
// %%Function: WinMain
//  Initialization and main message-pump
// ---------------------------------------------------------------------------
 int PASCAL
WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR szCmdLine, int nCmdShow)
{
    WNDCLASS wndclass;
    HRESULT hr;
    int     i;
    DWORD   dwRegister = 0;
    MSG     msg;
	PSECURITY_DESCRIPTOR psd = NULL;

    // parse the command-line
    if (szCmdLine)
        {
        LPSTR   sz = strtok(szCmdLine, " \t");

		if (sz)
			{
			if (!strcmpi(sz, "-AutoRegister"))
				{
			    if (!FAutoRegister(hinst))
			        {
			        hr = GetLastError();
			        goto LCleanup;
			        }
				else
					{
					MessageBox (GetDesktopWindow(),
						TEXT("Registered Successfully!"),
						szTitleServer, MB_OK);
					return S_OK;
					}
				}
			else
				vfServer = TRUE;
			}
        }
 		
    // initialize COM
    hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(hr))
        {
        ErrorMessage(NULL, TEXT("CoInitializeEx"), hr);
        return hr;
        }

	// psd must remain valid until after we uninit COM
	if (!CreateAppSecurityDescriptor(&psd))
		{
		return E_FAIL;
		}

	// initialize the security layer
	hr = CoInitializeSecurity (
					psd,
					-1,
					NULL,
					NULL,
					RPC_C_AUTHN_LEVEL_CONNECT,
					RPC_C_IMP_LEVEL_IMPERSONATE,
					NULL,
					0,
					NULL);
    if (FAILED(hr))
        {
        ErrorMessage(NULL, TEXT("CoInitializeSecurity"), hr);
        return hr;
        }

    // create the threads and synchronization events
    // which the server will need
    for (i=0; i<cServerThreads; i++)
        {
        TCHAR    rgch[32];

        // create the thread suspended so its event can be
        // created using its thread-id and it will be able to
        // use it as soon as it runs
        vrghThread[i] = CreateThread(NULL,
            0,
            (LPTHREAD_START_ROUTINE)&ServerThreadProc,
            0,
            CREATE_SUSPENDED,
            &vrgtid[i]);
        if (vrghThread[i] == NULL)
            {
            hr = GetLastError();
            goto LCleanup;
            }

        // a thread can use its thread-id to OpenEvent this existing
        // event at any time. it can also look for its thread-id in
        // vrgtid[] to determine its index for use with vrghEvent.
        // this event signals to a worker thread to create a new CObject
        wsprintf(rgch, TEXT("Thread_%d"), vrgtid[i]);
        vrghEvent[i] = CreateEvent(NULL, FALSE, FALSE, rgch);
        if (vrghEvent[i] == NULL)
            {
            hr = GetLastError();
            goto LCleanup;
            }

		// initially no user is associated with a thread
		vrgpSid[i] = NULL;
		vrgpszAptUser[i] = NULL;

        // now that the event is available, let the thread run
        ResumeThread(vrghThread[i]);
        }

    // this signals the status of a worker threads creation after
    // receiving its create signal via vrghEvent[i]
    vhEventServer = CreateEvent(NULL, FALSE, FALSE, TEXT("Server"));
    if (vhEventServer == NULL)
        {
        hr = GetLastError();
        goto LCleanup;
        }

    // register the class-factory with COM
    hr = CoRegisterClassObject(CLSID_CObject,
        (IUnknown *)&vclassfactory,
        CLSCTX_LOCAL_SERVER,
        REGCLS_MULTIPLEUSE,
        &dwRegister);
    if (FAILED(hr))
        goto LCleanup;

    // create an IStream to be used for marshalling new objects between
    // the worker and the CClassFactory
    hr = CreateStreamOnHGlobal(NULL, TRUE, &vpstmMarshalling);
    if (FAILED(hr))
        goto LCleanup;

    // create the server UI
    memset(&wndclass, 0, sizeof(wndclass));
    wndclass.lpszClassName = szWindowClass;
    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = MainWndProc;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)(COLOR_APPWORKSPACE + 1);
    wndclass.hInstance = hinst;

    if (!RegisterClass(&wndclass) ||
        (vhwndMain = CreateWindow(szWindowClass,
            szTitleServer,
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            HWND_DESKTOP, NULL, hinst, NULL)) == NULL)
        {
        hr = GetLastError();
        goto LCleanup;
        }

    ShowWindow(vhwndMain, nCmdShow);

    while (GetMessage(&msg, NULL, 0, 0))
        {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        }
    hr = msg.wParam;

LCleanup:
    if (hr != S_OK)
        ErrorMessage(NULL, TEXT("CoInitializeEx"), hr);

    // we explicitly don't clean up threads and events
    if (dwRegister != 0)
        CoRevokeClassObject(dwRegister);
    if (vpstmMarshalling != NULL)
        vpstmMarshalling->Release();

    CoUninitialize();

	FreeAppSecurityDescriptor (psd);

    return hr;
}  // WinMain


// ---------------------------------------------------------------------------
// %%Function: FAutoRegister
//  Registers the CObject class in the registry.
// ---------------------------------------------------------------------------
 BOOL
FAutoRegister(HINSTANCE hinst)
{
    static const TCHAR szClassDesc[]			= TEXT("Ole Apartment Impersonation Sample");
    static const TCHAR szCLSIDEntry[]			= TEXT("CLSID\\{035B79D1-D6D3-11cf-B9D4-00AA00A216E0}");
	static const TCHAR szAppIDEntry[]			= TEXT("APPID\\{035B79D1-D6D3-11cf-B9D4-00AA00A216E0}");
	static const TCHAR szAppIDString[]			= TEXT("{035B79D1-D6D3-11cf-B9D4-00AA00A216E0}");
	static const TCHAR szAppID[]				= TEXT("AppID");
	static const TCHAR szLocalServer32[]		= TEXT("LocalServer32");
	static const TCHAR szRunAs[]				= TEXT("RunAs");
	static const TCHAR szInteractiveUser[]		= TEXT("Interactive User");
    TCHAR       szPath[512];
	TCHAR		szModuleEntry[512];
	LPTSTR		index;
    HKEY        hkeyT = NULL;

    // install the CLSID key and get the FQP for this executable
    if ((RegSetValue(HKEY_CLASSES_ROOT, szCLSIDEntry, REG_SZ, szClassDesc,
            lstrlen(szClassDesc)) != ERROR_SUCCESS) ||
        (RegCreateKey(HKEY_CLASSES_ROOT, szCLSIDEntry, &hkeyT)
            != ERROR_SUCCESS) ||
        !GetModuleFileName(hinst, szPath, sizeof(szPath)))
        return FALSE;

	// install the LocalServer32 value
    if (RegSetValue(hkeyT, szLocalServer32, REG_SZ, szPath, lstrlen(szPath))
            != ERROR_SUCCESS)
        goto LErrExit;

	// install a pointer to our App ID
    if (RegSetValueEx(hkeyT, szAppID, 0, REG_SZ, (LPBYTE)szAppIDString,
    		sizeof(szAppIDString)) != ERROR_SUCCESS)
        goto LErrExit;

    RegCloseKey(hkeyT);

	// install the App ID
	if ((RegSetValue(HKEY_CLASSES_ROOT, szAppIDEntry, REG_SZ, szClassDesc,
			lstrlen(szClassDesc)) != ERROR_SUCCESS) ||
		(RegCreateKey(HKEY_CLASSES_ROOT, szAppIDEntry, &hkeyT)
			!= ERROR_SUCCESS))
		goto LErrExit;

	// indicate that we should run as the interactive user
	if (RegSetValueEx(hkeyT, szRunAs, 0, REG_SZ, (LPBYTE)szInteractiveUser,
			sizeof(szInteractiveUser)) != ERROR_SUCCESS)
		goto LErrExit;

    RegCloseKey(hkeyT);

	// get the App ID module entry
	index = _tcsrchr (szPath, TEXT('\\'));
	if (NULL == index)
		index = szPath;
	else
		index += 1;

	lstrcpy (szModuleEntry, TEXT("APPID\\"));
	lstrcat (szModuleEntry, index);

	// Create the module entry in the APPID tree
	if (RegCreateKey(HKEY_CLASSES_ROOT, szModuleEntry, &hkeyT)
			!= ERROR_SUCCESS)
		goto LErrExit;

	// associate the App ID with the module
	if (RegSetValueEx(hkeyT, szAppID, 0, REG_SZ, (LPBYTE)szAppIDString,
			sizeof(szAppIDString)) != ERROR_SUCCESS)
		goto LErrExit;

    RegCloseKey(hkeyT);

    return TRUE;

LErrExit:
    RegCloseKey(hkeyT);
    return FALSE;
}  // FAutoRegister

// ===========================================================================
//                          C C L A S S F A C T O R Y
// ===========================================================================

// ---------------------------------------------------------------------------
// %%Function: CClassFactory::QueryInterface
//  Returns a new reference of the specified iid-type to a CClassFactory.
// ---------------------------------------------------------------------------
 STDMETHODIMP
CClassFactory::QueryInterface(REFIID iid, void **ppv)
{
    *ppv = NULL;

    if (iid == IID_IClassFactory || iid == IID_IUnknown)
        {
        *ppv = (IClassFactory *)this;
        }
    if (*ppv != NULL)
        {
        AddRef();
        return S_OK;
        }
    return E_NOINTERFACE;
}  // CClassFactory::QueryInterface

// ---------------------------------------------------------------------------
// %%Function: CClassFactory::CreateInstance
//  Creates a new instance of a CObject on a worker thread.
// ---------------------------------------------------------------------------
 STDMETHODIMP
CClassFactory::CreateInstance(LPUNKNOWN punkOuter, REFIID iid, void **ppv)
{
    LPUNKNOWN   punk;
    HRESULT     hr;
	HANDLE		hToken = NULL, hPrimaryToken = NULL;
	PSID		psidUser = NULL;
	int			iThread;

	// Get the client's token
	hr = CoImpersonateClient ();
    if (FAILED(hr))
	    {
        ErrorMessage(NULL, TEXT("CoImpersonateClient"), hr);
	    return hr;
		}

	if (!OpenThreadToken(GetCurrentThread(), TOKEN_ALL_ACCESS, TRUE, &hToken))
		{
		hr = GetLastError();
		ErrorMessage(NULL, TEXT("OpenThreadToken"), hr);
		return hr;
		}

	CoRevertToSelf ();

	__try
		{
		if (!GetUserSidFromToken (hToken, &psidUser))
			{
			ErrorMessage(NULL, TEXT("GetUserSidFromToken"), 0);
			return(E_FAIL);
			}

		// find an index for this user
		if (!FindSlot(psidUser, &iThread))
			return(E_OUTOFMEMORY);	// not enough "resources"
		
		// if this is a previously free slot, set it up
		if (NULL == vrgpSid[iThread])
			{
			// The impersonation token we first get disappears after
			// the call completes.  We need to get a more permanent
			// token for the worker thread.
			if (!DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS, NULL,
					SecurityImpersonation, TokenPrimary, &hPrimaryToken))
			    {
				hr = GetLastError ();
		        ErrorMessage(NULL, TEXT("DuplicateTokenEx"), hr);
				return(hr);
				}

			CloseHandle (hToken);
			hToken = NULL;

			if (!DuplicateTokenEx(hPrimaryToken, TOKEN_ALL_ACCESS, NULL,
					SecurityImpersonation, TokenImpersonation, &hToken))
			    {
				hr = GetLastError ();
		        ErrorMessage(NULL, TEXT("DuplicateTokenEx"), hr);
				return(hr);
				}

			if (!SetThreadToken (&vrghThread[iThread], hToken))
				{
				hr = GetLastError ();
				ErrorMessage(NULL, TEXT("SetThreadToken"), hr);
				return(hr);
				}

			// store and null out stuff we don't want freed or closed
			vrgpSid[iThread] = psidUser;
			psidUser = NULL;
			}
		}
	__finally
		{
		UtilFreeMem(psidUser);
		if (NULL != hToken)
			CloseHandle (hToken);
		if (NULL != hPrimaryToken)
			CloseHandle (hPrimaryToken);
		}

    *ppv = NULL;
    if (punkOuter != NULL)
        return CLASS_E_NOAGGREGATION;

    // trigger the worker thread that we want to create an object
    SetEvent(vrghEvent[iThread]);

    // now wait for the object to signal its completion
    WaitForSingleObject(vhEventServer, INFINITE);

    // once the worker thread signals completion, vhrThreadStatus
    // lets us know if the creation process was successful, and if
    // vpstmMarshalling creates a marshalled interface pointer
    if (FAILED(vhrThreadStatus))
        return vhrThreadStatus;

    // unmarshal an IUnknown from the scratch stream. if unmarshaling
    // fails, it takes care of releasing the object inside the marshal-data
    hr = vpstmMarshalling->Seek(bZero, STREAM_SEEK_SET, NULL);
    if (FAILED(hr))
        return hr;
    hr = CoUnmarshalInterface(vpstmMarshalling, IID_IUnknown, (void **)&punk);
    if (FAILED(hr))
        return hr;

    // get a reference to the interface asked for
    hr = punk->QueryInterface(iid, ppv);
    punk->Release();

    return hr;
}  // CClassFactory::CreateInstance

 STDMETHODIMP
CClassFactory::LockServer(BOOL fLock)
{
    // there's no need to support this for this sample
    return E_FAIL;
}  // CClassFactory::LockServer


// ===========================================================================
//                               C O B J E C T
// ===========================================================================

// ---------------------------------------------------------------------------
// %%Function: ServerThreadProc
//  The worker thread function. Handles messages for objects of its thread/apt
// and creates new objects.
// ---------------------------------------------------------------------------
 LRESULT
ServerThreadProc(LPARAM lParam)
{
    HRESULT hr;
    MSG     msg;
    int     iThread;

    // figure out which thread this is: it needs its synchronization event
    for (iThread=0; iThread<cServerThreads; iThread++)
        {
        if (vrgtid[iThread] == GetCurrentThreadId())
            break;
        }
    if (iThread==cServerThreads)
        return E_UNEXPECTED;

    // initialize COM
    hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(hr))
        {
        MessageBeep(0);
        return hr;
        }

    // apartment message/event loop
    // (see SDK documentation for MsgWaitForMultipleObjects)
    // here worker message loops last forever. in situations without a
    // static number of worker threads, the loop could easily be terminated by
    // WM_QUITs sent from the main thread which might manage the worker thread
    // pool more carefully.
    while (TRUE)
        {
        DWORD dwWaitResult;

        // wait for any message sent or posted to this queue
        // or for one of the passed handles to become signaled
        dwWaitResult = MsgWaitForMultipleObjects(1, &vrghEvent[iThread],
            FALSE, INFINITE, QS_ALLINPUT);

        // result tells us the type of event we have:
        // a message or a signaled handle

        // if there are one or more messages in the queue ...
        if (dwWaitResult == (WAIT_OBJECT_0 + 1))
            {
            // dispatch all of the messages in this next loop
            // (here is where we'd check for WM_QUITs to end this
            // worker thread if we wanted to)
            while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
                DispatchMessage(&msg);
            }
        else
            {
            // this thread was signaled to create a new object
            try
                {
                LPUNKNOWN   punk;

				// if the user name isn't set, look it up
				// we could do this in the class factory, but this gives
				// us a sanity check on the impersonation
				if (NULL == vrgpszAptUser[iThread])
					{
					LPTSTR pszUser;
					DWORD cchUser = MAX_USER_NAME;

					if (!UtilAllocMem ((PVOID*)&pszUser, cchUser*sizeof(TCHAR)))
						throw E_OUTOFMEMORY;

					if (!GetUserName (pszUser, &cchUser))
						lstrcpy (pszUser, TEXT("Name not found"));

					vrgpszAptUser[iThread] = pszUser;
					}

                // create a new CObject
                punk = (IUnknown *)new CObject;
                if (punk == NULL)
                    throw E_OUTOFMEMORY;

                hr = vpstmMarshalling->Seek(bZero, STREAM_SEEK_SET, NULL);
                if (FAILED(hr))
                    throw hr;
                hr = CoMarshalInterface(vpstmMarshalling,
                    IID_IUnknown,
                    punk,
                    MSHCTX_INPROC,
                    NULL,
                    MSHLFLAGS_NORMAL);
                if (FAILED(hr))
                    throw hr;

                // punk is now referenced by its marshal-data in vpstmMarshalling.
                // we release our local reference here so the unmarshaller will
                // have the sole reference. a common mistake is to forget this
                // release and end up with orphaned objects in the server.
                punk->Release();

                // modify the global state -- the # of objects/thread -- which is
                // displayed in the server UI
                vrgcObjects[iThread]++;
                InvalidateRect(vhwndMain, NULL, TRUE);

                vhrThreadStatus = S_OK;
                }
            catch (HRESULT hr)
                {
                vhrThreadStatus = hr;
                }
            SetEvent(vhEventServer);
            }

        }

    CoUninitialize();
    return msg.wParam;
}  // ServerThreadProc

// ---------------------------------------------------------------------------
// %%Function: CObject::QueryInterface
//  Returns a new reference of the specified iid-type to a CObject.
// ---------------------------------------------------------------------------
 STDMETHODIMP
CObject::QueryInterface(REFIID iid, void **ppv)
{
    *ppv = NULL;

    if (iid == IID_IUnknown)
        {
        *ppv = (IUnknown *)this;
        }
    if (*ppv != NULL)
        {
        AddRef();
        return S_OK;
        }
    return E_NOINTERFACE;
}  // CObject::QueryInterface

// ---------------------------------------------------------------------------
// %%Function: CObject::Release
//  Handles releases of references to a CObject. Purpose here is to have code
// which alters the global state which is displayed in the servers UI.
// ---------------------------------------------------------------------------
 STDMETHODIMP_(ULONG)
CObject::Release(void)
{
    if (--mcRef == 0)
        {
        // change the global server state -- the # of objects/thread --
        // reflected in the server UI
        for (int i=0; i<cServerThreads; i++)
            {
            if (vrgtid[i] == GetCurrentThreadId())
                {
				// A sanity check to see if we're still impersonating...
				TCHAR szUser[MAX_USER_NAME];
				DWORD cchUser = MAX_USER_NAME;

				if (!GetUserName (szUser, &cchUser))
					ErrorMessage(NULL, TEXT("GetUserName"), GetLastError ());
				else
					{
					if (lstrcmp (szUser, vrgpszAptUser[i]))
						ErrorMessage (NULL, TEXT("Impersonation error."), S_OK);
					}

                vrgcObjects[i]--;
                InvalidateRect(vhwndMain, NULL, TRUE);
                break;
                }
            }

        delete this;
        return 0;
        }
    return mcRef;
}  // CObject::Release


// ---------------------------------------------------------------------------
// %%Function: FindSlot
//  Determines what slot in the array of worker threads may be used by the
// user represented by psid.
// ---------------------------------------------------------------------------
BOOL FindSlot(PSID psid, int *piThread)
{
	int iThread;

	// see if there is already an apartment impersonating this user
	for (iThread = 0; iThread < cServerThreads; iThread++)
		{
		if ((NULL != vrgpSid[iThread]) &&
				(EqualSid (psid, vrgpSid[iThread])))
			break;
		}

	// if this user didn't have a thread already, find a free one
	if (iThread == cServerThreads)
		{
		for (iThread = 0; iThread < cServerThreads; iThread++)
			{
			if (NULL == vrgpSid[iThread])
				break;
			}

		if (iThread == cServerThreads) // no resources available...
			return(FALSE);
		}

	// return the index to use
	*piThread = iThread;
	return(TRUE);
}

// EOF =======================================================================
