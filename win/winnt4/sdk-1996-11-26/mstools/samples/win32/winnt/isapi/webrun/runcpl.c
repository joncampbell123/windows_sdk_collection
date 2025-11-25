/****************************************************************************
*
*
*    PROGRAM: WebRunCpl.c
*
*    PURPOSE: Configuration Control Panel Applet for Web Runner Sample
*
*  Exports:
*
*  LONG CALLBACK CPlApplet(HWND, UINT, LONG, LONG)
*
*   This is the entry point that Control Panel requires so that it can
*   interact with our applet.
*
****************************************************************************/

// Registry Keys and Values for Web Runner parameters
#define RUNNER_REG_KEY            "Software\\WebRunner\\Commands"
#define RUNNER_REG_COMMAND_VALUE  "HTMLListBoxLines"
#define RUNNER_REG_PARAM_KEY      "Software\\WebRunner\\Parameters"
#define RUNNER_REG_MODE_VALUE     "Mode"   // 1 = Admin, 2 = Normal
#define RUNNER_REG_FILEPATH_VALUE "FilePath"
#define DEFAULT_FILEPATH          "C:\\HTTP\\RUNNER.HTM"

// HTML
#define PRESTRING                 "<option>"
#define POSTSTRING                "</option>"

// Standard Includes
#include <windows.h>
#include <cpl.h>
#include <commdlg.h>
#include "runcpl.h"
#include "resource.h"

// Function Defines
void FillListBox(HWND, HKEY);

// Control Panel Applet Information
typedef struct tagApplets
{
    int icon;           // icon resource identifier
    int namestring;     // name-string resource identifier
    int descstring;     // description-string resource identifier
    int dlgtemplate;    // dialog box template resource identifier
    DLGPROC dlgfn;      // dialog box procedure
} APPLETS;

APPLETS RunApplet =
{
    RUN_ICON, RUN_NAME, RUN_DESC, RUN_DLG, RunDlgProc,
};

HANDLE  hModule = NULL;
char    lpFilePath[MAX_PATH];
DWORD   cbFilePathLength = sizeof(lpFilePath);
char    szCtlPanel[30];
HKEY    hKey;

/****************************************************************************
*
*    FUNCTION: DllMain(PVOID, ULONG, PCONTEXT)
*
*    PURPOSE: Win 32 Initialization DLL
*
*    COMMENTS:
*
*
****************************************************************************/

BOOL WINAPI DllMain(
IN PVOID hmod,
IN ULONG ulReason,
IN PCONTEXT pctx OPTIONAL)
{
    if (ulReason != DLL_PROCESS_ATTACH)
    {
        return TRUE;
    }
    else
    {
        hModule = hmod;
    }

    return TRUE;

    UNREFERENCED_PARAMETER(pctx);
}


/****************************************************************************
*
*    FUNCTION: InitRuncplApplet(HWND)
*
*    PURPOSE: loads the caption string for the Control Panel
*
*    COMMENTS:
*
*
****************************************************************************/

BOOL InitRuncplApplet (HWND hwndParent)
{
    LoadString (hModule, CPCAPTION, szCtlPanel, sizeof(szCtlPanel));

    return TRUE;

    UNREFERENCED_PARAMETER(hwndParent);
}


/****************************************************************************
*
*    FUNCTION: TermRunApplet()
*
*    PURPOSE: termination procedure for the stereo applets
*
*    COMMENTS:
*
*
****************************************************************************/

void TermRunApplet()
{
    return;
}


/****************************************************************************
*
*    FUNCTION: CPlApplet(HWND, UINT, LONG, LONG)
*
*    PURPOSE: Processes messages for control panel applets
*
*    COMMENTS:
*
*
****************************************************************************/
LONG CALLBACK CPlApplet (hwndCPL, uMsg, lParam1, lParam2)
HWND hwndCPL;       // handle of Control Panel window
UINT uMsg;          // message
LONG lParam1;       // first message parameter
LONG lParam2;       // second message parameter
{
    LPNEWCPLINFO lpNewCPlInfo;
    static iInitCount = 0;
            
    switch (uMsg) {
        case CPL_INIT:              // first message
            if (!iInitCount)
            {
                if (!InitRuncplApplet(hwndCPL))
                    return FALSE;
            }
            iInitCount++;
            return TRUE;

        case CPL_GETCOUNT:          // second message
            return (LONG)1;
            break;

        case CPL_NEWINQUIRE:        // third message
            lpNewCPlInfo = (LPNEWCPLINFO) lParam2;

            lpNewCPlInfo->dwSize = (DWORD) sizeof(NEWCPLINFO);
            lpNewCPlInfo->dwFlags = 0;
            lpNewCPlInfo->dwHelpContext = 0;
            lpNewCPlInfo->lData = 0;
            lpNewCPlInfo->hIcon = LoadIcon (hModule,
                (LPCTSTR) MAKEINTRESOURCE(RunApplet.icon));
            lpNewCPlInfo->szHelpFile[0] = '\0';

            LoadString (hModule, RunApplet.namestring,
                        lpNewCPlInfo->szName, 32);

            LoadString (hModule, RunApplet.descstring,
                        lpNewCPlInfo->szInfo, 64);
            break;

        case CPL_SELECT:            // application icon selected
            break;


        case CPL_DBLCLK:            // application icon double-clicked
            
			// Start our dialog box
			DialogBox (hModule,
                       MAKEINTRESOURCE(RunApplet.dlgtemplate),
                       hwndCPL,
                       RunApplet.dlgfn);
            break;

         case CPL_STOP:              // sent once per app. before CPL_EXIT
            break;

         case CPL_EXIT:              // sent once before FreeLibrary called
            iInitCount--;
            if (!iInitCount)
                TermRunApplet();
            break;

         default:
            break;
    }
    return 0;
}

/****************************************************************************
*
*    FUNCTION: RunDlgProc
*
*    PURPOSE: Processes messages sent to our dialog box.
*
*    COMMENTS:
*
*
****************************************************************************/
BOOL APIENTRY RunDlgProc (HWND hDlg, UINT message, UINT wParam, LONG lParam)
{
	DWORD OpenType;
	DWORD cbCommandSize = 0;
	char lpNewCommand[256];
	DWORD cbCommandString = 0;
	LPSTR lpCommandString = NULL;
	HKEY hParamKey;
	BOOL AdminEnabled;
	DWORD cbBuffSize = sizeof(AdminEnabled);

    switch (message)
    {

	    case WM_INITDIALOG:
			// Disable Remove Button until something is selected
			// in our listbox.
			EnableWindow(GetDlgItem(hDlg, ID_REMOVE), FALSE);
			
			// Open our registry key (or create it if it
			// doesn't already exist)
			if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, 
				           RUNNER_REG_KEY, 
						   0, 
						   "WEBRUNNER",
						   REG_OPTION_NON_VOLATILE,
						   KEY_READ | KEY_WRITE, 
						   NULL,
						   &hKey,
						   &OpenType) != ERROR_SUCCESS)
			{
				MessageBox(hDlg, "Could not open or create Web Runner \
					              Registry entries", "Error", MB_OK);
				EndDialog(hDlg, FALSE);
				return TRUE;
			}

			// Fill listbox with command already entered
			FillListBox(hDlg, hKey);

			// Are we in Admin mode?  Check Registry
			if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, 
						   RUNNER_REG_PARAM_KEY, 
						   0, 
						   "WEBRUNNER",
						   REG_OPTION_NON_VOLATILE,
						   KEY_READ | KEY_WRITE, 
						   NULL,
						   &hParamKey,
						   &OpenType) != ERROR_SUCCESS)
			{
				MessageBox(hDlg, 
					"Could not open or create Web Runner Registry Key", 
					"Web Runner", 
					MB_OK);
				return TRUE;
			}

			if (RegQueryValueEx(hParamKey,
				                RUNNER_REG_MODE_VALUE,
								NULL,
								NULL,
								(LPBYTE)&AdminEnabled,
								&cbBuffSize) != ERROR_SUCCESS)
			{
				// We are not in Admin mode
				AdminEnabled = FALSE;

				if (RegSetValueEx(hParamKey,
					              RUNNER_REG_MODE_VALUE,
								  0,
								  REG_BINARY,
								  (LPBYTE)&AdminEnabled,
								  sizeof(AdminEnabled)) != ERROR_SUCCESS)
				{
					MessageBox(hDlg, 
					           "Could not open or create \
						        Web Runner Registry Balue", 
							   "Web Runner", 
							   MB_OK);
					return TRUE;
				}
			}
			if (AdminEnabled)
			{
				// We are in Admin mode, grey everything
				EnableWindow(GetDlgItem(hDlg, ID_COMMANDLIST), FALSE);
				EnableWindow(GetDlgItem(hDlg, ID_NEWCOMMAND), FALSE);
				EnableWindow(GetDlgItem(hDlg, ID_ADD), FALSE);
				EnableWindow(GetDlgItem(hDlg, ID_BROWSE), FALSE);
				CheckDlgButton(hDlg, ID_ADMIN, 1);
			}
			else
			{
				// We are not in Admin mode setup everything
				EnableWindow(GetDlgItem(hDlg, ID_COMMANDLIST), TRUE);
				EnableWindow(GetDlgItem(hDlg, ID_NEWCOMMAND), TRUE);
				EnableWindow(GetDlgItem(hDlg, ID_ADD), TRUE);
				EnableWindow(GetDlgItem(hDlg, ID_BROWSE), TRUE);
				CheckDlgButton(hDlg, ID_ADMIN, 0);
			}
			
			// Get Filepath of HTML file
			if (RegQueryValueEx(hParamKey,
				                RUNNER_REG_FILEPATH_VALUE,
								NULL,
								NULL,
								(LPBYTE)lpFilePath,
								&cbFilePathLength) != ERROR_SUCCESS)
			{
				// Registry value doesn't exist, create it
				lstrcpy(lpFilePath, DEFAULT_FILEPATH);
				cbFilePathLength = lstrlen(lpFilePath) + 1;

				if (RegSetValueEx(hParamKey,
					              RUNNER_REG_FILEPATH_VALUE,
								  0,
								  REG_BINARY,
								  (LPBYTE)lpFilePath,
								  cbFilePathLength) != ERROR_SUCCESS)
				{
					MessageBox(hDlg, 
						"Could not open or create Web Runner \
						 Registry Value", "Web Runner", MB_OK);
					return TRUE;
				}

				// Prompt them for initial path
				PostMessage(hDlg, WM_COMMAND, ID_CHANGE, 0);
			}

			// Display the HTML file path
			SetDlgItemText(hDlg, ID_FILEPATH, lpFilePath);
			return (TRUE);

		case WM_CLOSE:
			// Bye bye
			EndDialog(hDlg, TRUE);
			return TRUE;

	    case WM_COMMAND:      
            switch(LOWORD(wParam))
			{
			case ID_DONE:
				// Bye bye
				EndDialog(hDlg, TRUE);
				return TRUE;

			case ID_CHANGE:
				// They want to change the html file path
				DialogBox (hModule,
						   MAKEINTRESOURCE(FILE_DIALOG),
						   hDlg,
						   FileDlgProc);
				SetDlgItemText(hDlg, ID_FILEPATH, lpFilePath);
				return TRUE;
				
			case ID_BROWSE:
				if (HIWORD(wParam) == BN_CLICKED)
				{
					// Browse for new command to add to list
					char lpFileName[256];

					GetDlgItemText(hDlg, 
						           ID_NEWCOMMAND, 
								   lpNewCommand, 
								   sizeof(lpNewCommand));
					if (!Browse(hDlg,           // Parent Window
						        lpNewCommand,   // Start at specified location
								lpFileName,     // Actual file selected
								TRUE))          // Indicates Command Browse
						return FALSE;
					SetDlgItemText(hDlg, ID_NEWCOMMAND, lpNewCommand);
					return FALSE;
				}
				break;

			case ID_ADMIN:
				if (HIWORD(wParam) == BN_CLICKED)
				{
					// Request to switch admin mode, open Registry key
					if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, 
								   RUNNER_REG_PARAM_KEY, 
								   0, 
								   "WEBRUNNER",
								   REG_OPTION_NON_VOLATILE,
								   KEY_READ | KEY_WRITE, 
								   NULL,
								   &hParamKey,
								   &OpenType) != ERROR_SUCCESS)
					{
						MessageBox(hDlg, 
							"Could not open or create Web \
							 Runner Registry entries", "Error", MB_OK);
						return TRUE;
					}

					if (SendDlgItemMessage(hDlg, 
						                   ID_ADMIN, 
										   BM_GETCHECK, 
										   0, 0) == 1)
					{
						// Entered Admin mode...grey out everything
						EnableWindow(GetDlgItem(hDlg, ID_COMMANDLIST), FALSE);
						EnableWindow(GetDlgItem(hDlg, ID_NEWCOMMAND), FALSE);
						EnableWindow(GetDlgItem(hDlg, ID_ADD), FALSE);
						EnableWindow(GetDlgItem(hDlg, ID_BROWSE), FALSE);

						AdminEnabled = TRUE;


					}
					else
					{
						// Changed to normal mode...turn controls back on
						EnableWindow(GetDlgItem(hDlg, ID_COMMANDLIST), TRUE);
						EnableWindow(GetDlgItem(hDlg, ID_NEWCOMMAND), TRUE);
						EnableWindow(GetDlgItem(hDlg, ID_ADD), TRUE);
						EnableWindow(GetDlgItem(hDlg, ID_BROWSE), TRUE);

						AdminEnabled = FALSE;
					}
					// Set registry value appropriately
					if (RegSetValueEx(hParamKey,
									  RUNNER_REG_MODE_VALUE,
									  0,
									  REG_BINARY,
									  (BYTE *)&AdminEnabled,
									  sizeof(AdminEnabled)) != ERROR_SUCCESS)
					{
						MessageBox(hDlg, 
							"Error writing to Registry", 
							"Web Runner", 
							MB_OK);
					}
					RegCloseKey(hParamKey);

					// Change the html page to reflect current mode
					
					// Function requires formatted command list...read it
					// from the registry
					RegQueryValueEx(hKey,
									RUNNER_REG_COMMAND_VALUE,
									NULL,
									NULL,
									NULL,
									&cbCommandString); // Get the size first.

					lpCommandString = LocalAlloc(LPTR, cbCommandString);
					
					RegQueryValueEx(hKey,
									RUNNER_REG_COMMAND_VALUE,
									NULL,
									NULL,
									lpCommandString,
									&cbCommandString);

					// Write the html file
					if (!PageCreate(AdminEnabled,		// Admin Mode?
						            lpFilePath,			// path of file
									lpCommandString))   // List of commands
					{
						MessageBox(hDlg, 
							"HTML File could not be written", 
							"Web Runner", 
							MB_OK);
						PostMessage(hDlg, WM_COMMAND, ID_CHANGE, 0);
					}
					LocalFree(lpCommandString);

					return FALSE;
				}
			case ID_COMMANDLIST:
				switch(HIWORD(wParam))
				{
				case LBN_SELCHANGE:
					{
						// When an item is highlighted, enable the Remove
						// button and put the command in the New Command
						// edit control
						DWORD iIndex;

						// Enable Remove button
						EnableWindow(GetDlgItem(hDlg, ID_REMOVE), TRUE);
						
						// Set New Command text to listbox selection
						iIndex = SendDlgItemMessage(hDlg, 
							                        ID_COMMANDLIST, 
													LB_GETCURSEL, 
													0,0);

						SendDlgItemMessage(hDlg, 
										   ID_COMMANDLIST, 
										   LB_GETTEXT, 
										   (WPARAM)iIndex, 
										   (LPARAM)lpNewCommand);
						SetDlgItemText(hDlg, ID_NEWCOMMAND, lpNewCommand);
						return FALSE;
					}
				case LBN_SELCANCEL:
					// If nothing is selected grey out Remove button
					EnableWindow(GetDlgItem(hDlg, ID_REMOVE), FALSE);
					return FALSE;
				}
				return FALSE;

			case ID_ADD:
				// Add New Command to list

				// Get the New Command
				cbCommandSize = GetDlgItemText(hDlg, 
					                           ID_NEWCOMMAND, 
											   lpNewCommand, 
											   sizeof(lpNewCommand));
				
				// If nothing is entered, don't do anything
				if (cbCommandSize == 0) break;

				// Get the current list from the registry
				if (RegQueryValueEx(hKey,
						        RUNNER_REG_COMMAND_VALUE,
								NULL,
								NULL,
								NULL,
								&cbCommandString) != ERROR_SUCCESS)
				{
					MessageBox(hDlg, 
						"Error Reading Memory", 
						"Web Runner", 
						MB_OK);
					return (FALSE);
				}

				// We need enough memory for current command 
				// string + command prefix + new command + command
				// suffix
				cbCommandString = cbCommandString 
					              + cbCommandSize 
								  + lstrlen(PRESTRING) 
								  + lstrlen(POSTSTRING);

				lpCommandString = LocalAlloc(LPTR, cbCommandString);
				
				if (lpCommandString == NULL)
				{
					MessageBox(hDlg, 
						"Error allocating memory", 
						"Web Runner", 
						MB_OK);
					return (FALSE);
				}
				
				// Get the current command list
				if (RegQueryValueEx(hKey,
					RUNNER_REG_COMMAND_VALUE,
					NULL,
					NULL,
					lpCommandString,
					&cbCommandString) != ERROR_SUCCESS)
				{
					MessageBox(hDlg, 
						"Error reading registry", 
						"Web Runner", 
						MB_OK);
					LocalFree(lpCommandString);
					return (FALSE);
				}
				
				// Add prefix, new command, and suffix to current
				// command string
				lstrcat(lpCommandString, PRESTRING);
				lstrcat(lpCommandString, lpNewCommand);
				lstrcat(lpCommandString, POSTSTRING);
				cbCommandString = lstrlen(lpCommandString) 
					              + 1; // Don't forget ending NULL!

				// Set Registry Value
				if (RegSetValueEx(hKey,
						          RUNNER_REG_COMMAND_VALUE,
								  0,
								  REG_SZ,
								  lpCommandString,
								  cbCommandString) != ERROR_SUCCESS)
				{
					MessageBox(hDlg, 
						"Error writing to registry", 
						"Web Runner", 
						MB_OK);
					LocalFree(lpCommandString);
					return (FALSE);
				}
				
				// Add the new command to the listbox
				SendDlgItemMessage(hDlg, 
						           ID_COMMANDLIST,
								   LB_ADDSTRING,
								   0,
								   (LPARAM)lpNewCommand);

				// Clear the New Command Edit Control
				SetDlgItemText(hDlg, ID_NEWCOMMAND, "");

				// Update the html file
				if (!PageCreate(FALSE,	  	        // User Mode
						        lpFilePath,			// path of file
								lpCommandString))   // List of commands
				{
					MessageBox(hDlg, 
						"HTML File could not be written", 
						"Web Runner", 
						MB_OK);
					PostMessage(hDlg, WM_COMMAND, ID_CHANGE, 0);
				}

				return (TRUE);

			case ID_REMOVE:
				{
					// Remove the currently selected item from 
					// the command list

					DWORD iIndex;
					char lpRemoveCommand[257];
					char *lpBeginPtr, *lpEndPtr;

					// Get the index of the current selection
					iIndex = SendDlgItemMessage(hDlg, 
						                        ID_COMMANDLIST, 
												LB_GETCURSEL, 
												0,0);
					if (iIndex == LB_ERR)
					{
						// Nothing selected...just act like nothing happened
						return (FALSE);
					}
					
					// Get the text of the item we are removing
					SendDlgItemMessage(hDlg, 
						               ID_COMMANDLIST, 
									   LB_GETTEXT, 
									   (WPARAM)iIndex, 
									   (LPARAM)lpRemoveCommand);
					
					// Go ahead and delete the item from the listbox now
					SendDlgItemMessage(hDlg,
						               ID_COMMANDLIST,
									   LB_DELETESTRING,
									   (WPARAM)iIndex,
									   0);

					// Also grey out the Remove button until something
					// else is selected
					EnableWindow(GetDlgItem(hDlg, ID_REMOVE), FALSE);

					// Get size of current command string
					if (RegQueryValueEx(hKey,
										RUNNER_REG_COMMAND_VALUE,
										NULL,
										NULL,
										NULL,
										&cbCommandString) != ERROR_SUCCESS)
					{
						MessageBox(hDlg, 
							"Error Reading Registry", 
							"Web Runner", 
							MB_OK);
						return (FALSE);
					}
					
					// Allocate memory to read current command string
					lpCommandString = LocalAlloc(LPTR, cbCommandString);
					if (lpCommandString == NULL)
					{
						MessageBox(hDlg, 
							"Error Allocating Memory", 
							"Web Runner", 
							MB_OK);
						return (FALSE);
					}

					// Read current command string from registry
					if (RegQueryValueEx(hKey,
										RUNNER_REG_COMMAND_VALUE,
										NULL,
										NULL,
										lpCommandString,
										&cbCommandString) != ERROR_SUCCESS)
					{
						MessageBox(hDlg, 
							       "Error Reading Registry", 
								   "Web Runner", 
								   MB_OK);
						LocalFree(lpCommandString);
						return (FALSE);
					}

					// Set lpBeginPtr to beginning of command in command list
					lpBeginPtr = strstr(lpCommandString, lpRemoveCommand);
					// Reset lpBeginPtr to start of command's prefix
					lpBeginPtr -= lstrlen(PRESTRING);
					// Set lpEndPtr to prefix of next command
					lpEndPtr = lpBeginPtr 
						       + lstrlen(PRESTRING) 
							   + lstrlen(lpRemoveCommand) 
							   + lstrlen(POSTSTRING);
					// Copy remainder of command list over command
					lstrcpy(lpBeginPtr, lpEndPtr);
					// Get length of what's left (plus terminating NULL)
					cbCommandString = lstrlen(lpCommandString) + 1;
					// Write new list to the registry
					if (RegSetValueEx(hKey,
									  RUNNER_REG_COMMAND_VALUE,
									  0,
									  REG_SZ,
									  lpCommandString,
									  cbCommandString) != ERROR_SUCCESS)
					{
						MessageBox(hDlg, 
							"Error writing to Registry", 
							"Web Runner", 
							MB_OK);
						LocalFree(lpCommandString);
						return (FALSE);
					}
					// Update the html file
					if (!PageCreate(FALSE,	  	        // User Mode
									lpFilePath,			// path of file
									lpCommandString))   // List of commands
					{
						MessageBox(hDlg, 
							"HTML File could not be written", 
							"Web Runner", 
							MB_OK);
						PostMessage(hDlg, WM_COMMAND, ID_CHANGE, 0);
					}
					LocalFree(lpCommandString);
					break;
				}
			}
	        break;
    }
    return (FALSE);
}

/****************************************************************************
*
*    FUNCTION: FillListBox
*
*    PURPOSE: Reads command list from the registry, parses it, and then
*             fills the listbox control with the entries
*
*    COMMENTS: The way HTML fills a listbox is by passing a string of the
*              form <output> listboxstring </output> for each item to be 
*			   displayed in the listbox.  In order to minimize registry
*			   queries we store the entire command in a single registry
*			   value.  This takes some parsing when modifying it, but
*			   modification is a relatively infrequent event compared to
*			   reading the string which happens every time somehow queries
*			   our page.
*
*
****************************************************************************/
void FillListBox(HWND hDlg, // Handle to our dialog
				 HKEY hKey) // Open handle to the commandlist registry key
{
	DWORD cbCommandString = 0;
	char * lpCommandString = NULL;
	char *lpCmdBegin, *lpCmdEnd;

	// Get size of command list
	if (RegQueryValueEx(hKey,
		            RUNNER_REG_COMMAND_VALUE,
					NULL,
					NULL,
					NULL,
					&cbCommandString) != ERROR_SUCCESS)
	{
		// Could not read Registry key...set it
		if(RegSetValueEx(hKey,
					  RUNNER_REG_COMMAND_VALUE,
					  0,
					  REG_SZ,
					  "",
					  1) != ERROR_SUCCESS)
		{
			MessageBox(NULL, 
				       "Error setting registry key", 
					   "Web Runner", 
					   MB_OK);
			return;
		}
	}

	if (cbCommandString == 0)
	{
		MessageBox(hDlg, 
			       "There are currently no commands configured", 
				   "Web Runner", 
				   MB_OK);
		return;
	}

	// Allocate memory to read command list
	lpCommandString = LocalAlloc(LPTR, cbCommandString);
	if (lpCommandString == NULL)
	{
		MessageBox(hDlg, "Error Allocating Memory", "Web Runner", MB_OK);
		return;
	}
	// Read command list
	if (RegQueryValueEx(hKey,
			            RUNNER_REG_COMMAND_VALUE,
						NULL,
						NULL,
						lpCommandString,
						&cbCommandString) != ERROR_SUCCESS)
	{
		// Could not read Registry key...set it
		if(RegSetValueEx(hKey,
					  RUNNER_REG_COMMAND_VALUE,
					  0,
					  REG_SZ,
					  "",
					  1) != ERROR_SUCCESS)
		{
			MessageBox(NULL, 
				       "Error setting registry key", 
					   "Web Runner", 
					   MB_OK);
		}
		LocalFree(lpCommandString);
		return;
	}
	
	// Set lpCmdBegin to the first prefix in the command list
	lpCmdBegin = strstr(lpCommandString, PRESTRING);
	
	// Loop while we can still find a prefix (which means
	// we have more commands).
	while (lpCmdBegin)
	{
		// Move lpCmdBegin to actual beginning of current command
		lpCmdBegin += lstrlen(PRESTRING);
		// Move lpCmdEnd to end of the current command
		lpCmdEnd = strstr(lpCmdBegin, POSTSTRING);
		if (lpCmdEnd == NULL)
		{
			// Registry is misconfigured...clear it out
			if(RegSetValueEx(hKey,
						  RUNNER_REG_COMMAND_VALUE,
						  0,
						  REG_SZ,
						  "",
						  1) != ERROR_SUCCESS)
			{
				MessageBox(hDlg, 
					       "Error setting registry key", 
						   "Web Runner", 
						   MB_OK);
				LocalFree(lpCommandString);
				return;
			}
		}
		// Put NULL at begginning of command suffix
		// so that our string terminates at the end
		// of the actual command.
		*lpCmdEnd = '\0';
		// Add it to the listbox
		SendDlgItemMessage(hDlg, 
					       ID_COMMANDLIST, 
						   LB_ADDSTRING, 
						   0, 
						   (LPARAM)lpCmdBegin);
		// Find next command prefix
		lpCmdBegin = strstr(++lpCmdEnd, PRESTRING);
	}  // end of while more commands to list

	LocalFree(lpCommandString);
}
				
/****************************************************************************
*
*    FUNCTION: FileDlgProc
*
*    PURPOSE: Processes messages sent File Dialog
*
*    COMMENTS:
*
*    Prompts the user for the full path of the HTML file that we will
*    be creating.
*
****************************************************************************/
BOOL APIENTRY FileDlgProc (HWND hDlg, UINT message, UINT wParam, LONG lParam)
{
	HKEY hParamKey;
	BOOL AdminEnabled;
	DWORD cbBuffSize = sizeof(AdminEnabled);
	DWORD cbCommandString;
	LPSTR lpCommandString;

	
	switch (message)
    {

    case WM_INITDIALOG:
		// Put current path in the edit control
		SetDlgItemText(hDlg, ID_FILEPATH, lpFilePath);
		return (TRUE);

    case WM_COMMAND:      
        switch(LOWORD(wParam))
		{
		case IDOK:
			// Get the string
			GetDlgItemText(hDlg, ID_FILEPATH, lpFilePath, sizeof(lpFilePath));
			// Open (or create if necessary) the parameters registry key
			if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, 
						   RUNNER_REG_PARAM_KEY, 
						   0, 
						   "WEBRUNNER",
						   REG_OPTION_NON_VOLATILE,
						   KEY_READ | KEY_WRITE, 
						   NULL,
						   &hParamKey,
						   NULL) != ERROR_SUCCESS)
			{
				MessageBox(hDlg, 
					"Could not open or create Web Runner Registry Key", 
					"Error", 
					MB_OK);
				return TRUE;
			}
			
			// See if we are in Admin mode
			if (RegQueryValueEx(hParamKey,
				                RUNNER_REG_MODE_VALUE,
								NULL,
								NULL,
								(LPBYTE)&AdminEnabled,
								&cbBuffSize) != ERROR_SUCCESS)
			{
				AdminEnabled = FALSE;

				if (RegSetValueEx(hParamKey,
					              RUNNER_REG_MODE_VALUE,
								  0,
								  REG_BINARY,
								  (LPBYTE)&AdminEnabled,
								  sizeof(AdminEnabled)) != ERROR_SUCCESS)
				{
					MessageBox(hDlg, 
						"Could not open or create Web Runner Registry Balue", 
						"Web Runner", 
						MB_OK);
					return TRUE;
				}
			}
			
			// Get size of command list
			RegQueryValueEx(hKey,
							RUNNER_REG_COMMAND_VALUE,
							NULL,
							NULL,
							NULL,
							&cbCommandString);

			lpCommandString = LocalAlloc(LPTR, cbCommandString);
			// Read command list
			RegQueryValueEx(hKey,
							RUNNER_REG_COMMAND_VALUE,
							NULL,
							NULL,
							lpCommandString,
							&cbCommandString);

			// Create the html page file
			if (!PageCreate(AdminEnabled,     // Admin mode?
				            lpFilePath,       // Path of html file
							lpCommandString)) // Command List
			{
				MessageBox(hDlg, 
					       "File could not be written", 
						   "Web Runner", 
						   MB_OK);
				return FALSE;
			}

			LocalFree(lpCommandString);

			// Set the html file path in registry
			if (RegSetValueEx(hParamKey,
				                RUNNER_REG_FILEPATH_VALUE,
								0,
								REG_SZ,
								(LPBYTE)lpFilePath,
								lstrlen(lpFilePath) + 1) != ERROR_SUCCESS)
			{
				MessageBox(hDlg, 
					       "Could not set Web Runner Registry Value", 
						   "Web Runner", 
						   MB_OK);
				return TRUE;
			}

			EndDialog(hDlg, TRUE);
			return TRUE;

		case IDCANCEL:
			// Normally we don't care if you cancel here, but
			// if we don't have a location for the html file
			// then we won't let you leave until you give us one.

			// Open the parameters key
			if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, 
						   RUNNER_REG_PARAM_KEY, 
						   0, 
						   "WEBRUNNER",
						   REG_OPTION_NON_VOLATILE,
						   KEY_READ | KEY_WRITE, 
						   NULL,
						   &hParamKey,
						   NULL) != ERROR_SUCCESS)
			{
				MessageBox(hDlg, 
					       "Could not open or create Web Runner Registry Key", 
						   "Error", 
						   MB_OK);
				EndDialog(hDlg, FALSE);
				return TRUE;
			}

			// See if there is a filepath yet
			if (RegQueryValueEx(hParamKey,
				                RUNNER_REG_FILEPATH_VALUE,
								NULL,
								NULL,
								(LPBYTE)lpFilePath,
								&cbFilePathLength) != ERROR_SUCCESS)
			{
				// No filepath...you can't leave! HA HA HA!
				MessageBox(hDlg, 
					       "You must specify an HTML filepath for Web Runner", 
						   "Web Runner", 
						   MB_OK);
				return (FALSE);
			}
			EndDialog(hDlg, TRUE);
			return TRUE;

		case ID_BROWSE:
			{
				// Let them browse for where they want to put the file
				char lpFileName[MAX_PATH];

				// Initializes commond browse dialog with current value
				GetDlgItemText(hDlg, 
					           ID_FILEPATH, 
							   lpFilePath, 
							   sizeof(lpFilePath));
				if (Browse(hDlg,		// Parent Window
					       lpFilePath,  // Initial path
						   lpFileName,  // returns actual File
						   FALSE))      // Indicates browsing for HTML file
					// Set the path edit control with result
					SetDlgItemText(hDlg, ID_FILEPATH, lpFilePath);

			}

			break;
		}
	}
	return FALSE;
}

/****************************************************************************
*
*    FUNCTION: PageCreate
*
*    PURPOSE: Creates the HTML page file
*
*	 PARAMETERS:
*         BOOL bMode       - TRUE indicates we are in Admin mode and the
*                              page provides client with an edit box to type
*						       in the command they want to execute
*						     FALSE indicates we are in Normal mode which means
*						       the client gets a listbox of commands that they
*						       can select to execute.  They cannot execute any
*						       other commands
*         CHAR * szFileName - This is the full path of the HTML file we are
*                             going to create.
*         CHAR * ListBox    - This is the command list in preformatted HTML
*                             listbox format
*
*    COMMENTS:
*
*
****************************************************************************/
BOOL PageCreate (BOOL bMode, 	CHAR * szFileName, CHAR * ListBox)
{
	// Mode is TRUE for admin, FALSE for user.
	HANDLE hFile;
	CHAR Buffer[4096] = "";
	DWORD dSize, dWritten;
	SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, FALSE};

	if  ( !(hFile = CreateFile (szFileName, 
		                        GENERIC_WRITE, 
								FILE_SHARE_READ, 
								&sa, 
								CREATE_ALWAYS, 
								FILE_ATTRIBUTE_NORMAL, NULL) ))
			return FALSE;


	lstrcpy (Buffer, "<head><title>WebRunner</title></head>\n<body>\
				<h1>Welcome to WebRunner.</h1><hr><h4>Mode:	");

	if (  bMode  ) 
	{
		// We are in Admin mode...right the admin mode htmp file
		lstrcat (Buffer, "administrator</h4><br><h2>To run your command \
						  please enter it here:</h2><p> \
					      <form action=\"runner.dll\" method=get> \
						  <INPUT NAME=\"COMMAND\" VALUE=\"\" > \
					      <BR><input type=\"submit\" value=\"Submit Entry\"> \
						  <input type=\"reset\" value=\"Reset Form\"> \
					      </form><p><h2>Output:</h2><p><hr><pre>");
	    dSize=lstrlen(Buffer);
		if (!WriteFile(hFile, 
			          (LPCVOID) Buffer, 
					  dSize,  
					  &dWritten, 
					  NULL)) return FALSE;
	}
	else
	{ 
		 // List Box User mode.
		lstrcat (Buffer, "user</h4><br><h2>Please choose command to run:</h2>\
						  <p><form action=\"runner.dll\" method=get> \
						  <SELECT NAME=\"COMMAND\" SIZE=3>");
 	    dSize=lstrlen(Buffer);
		if (!WriteFile(hFile, 
			           (LPCVOID)Buffer, 
					   dSize,  
					   &dWritten, 
					   NULL)) return FALSE;

		// We don't know size of string for registry.
		// just print it without coping to the buffer.
 	    dSize=lstrlen(ListBox);
		if (!WriteFile(hFile, 
			           (LPCVOID)ListBox, 
					   dSize,  
					   &dWritten, 
					   NULL)) return FALSE;
		
		lstrcpy(Buffer, "<p></SELECT><input type=\"submit\" \
						 value=\"Submit Entry\"> \
					     <input type=\"reset\" value=\"Reset Form\"> \
						 </form><p><h2>Output:</h2><p><hr>");

		dSize=lstrlen(Buffer);
		if ( ! WriteFile(hFile, 
			             (LPCVOID)Buffer, 
						 dSize,  
						 &dWritten, 
						 NULL)) return FALSE;
	}
	 
	if (!FlushFileBuffers(hFile))   return FALSE;
	if (!CloseHandle(hFile))  return FALSE;

	return TRUE;
}

/****************************************************************************
*
*    FUNCTION: Browse
*
*    PURPOSE: Uses Common File Open Dialog to specify htmp file or file
*             for our command list
*
*	 PARAMETERS:
*         HWND Gee          - Parent Window
*         CHAR * szFuleName - This is the full path of the HTML file we are
*                             going to create.
*         CHAR * szFileName - This is the command list in preformatted HTML
*                             listbox format
*         BOOL bMode        - TRUE: Look for commands to fill command list
*                             FALSE: Look for HTMP file location
*
*    COMMENTS:
*
*
****************************************************************************/
BOOL Browse (HWND Gee, CHAR * szFullName, CHAR * szFileName, BOOL bMode)
{

   // Mode will specify if we are browesing when we do one
   // of following:
   // 1. Look for commands:      bMode = TRUE
   // 2. Create *.htm file bMode = FALSE;

	OPENFILENAME   ofn= {0};
    CHAR szFile[256]="";       // filename string
    CHAR szFileTitle[256];  // file-title string
    CHAR szFilter [256];		// filter string
    CHAR chReplace;         // strparator for szFilter
    UINT uResourceID; 
    int i, cbString;        // integer count variables

	

    if ( bMode ) 
         // We are looking for commands.
         uResourceID =  IDS_FILTERSTRING;
    else
        // We are looking for   html file
        uResourceID =  IDS_FILTERSTRING_HTML;


    // Get file filter string
    cbString = LoadString(hModule, uResourceID, szFilter, sizeof(szFilter));
    chReplace = szFilter[cbString - 1];
    for (i = 0; szFilter[i] != '\0'; i++)
    {
        if (szFilter[i] == chReplace)
            szFilter[i] = '\0';
    }

	// fill common dialog structure
	ofn.lpstrTitle = "Browse";
	ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = Gee;		   
    ofn.lpstrFilter = szFilter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFileTitle = szFileTitle;
    ofn.nMaxFileTitle = sizeof(szFileTitle);
    ofn.lpstrInitialDir = szFullName;
    ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_NONETWORKBUTTON;


	if (!GetOpenFileName (&ofn)) 
		return FALSE;
	else
	{
		lstrcpy (szFullName, ofn.lpstrFile);
		lstrcpy (szFileName, ofn.lpstrFileTitle);
		return TRUE;
	}
}
