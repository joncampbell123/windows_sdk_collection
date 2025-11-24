#include <windows.h>
#include <stdlib.h>
#include <direct.h>
#include "setupapi.h"
#include "msdetect.h"
#include "msregdb.h"
#include "msshared.h"

#include "sample.h"


INT	installType;
CHAR	szInstallPath[cchMax];
CHAR	szSrcDir[cchMax];


HANDLE	hInst;


/* ********************************************************************* */
VOID BuildLists()
{
	int iCursorSav = ShowWaitCursor();

	BuildCopyList();
	BuildBillboardList();

	RestoreCursor(iCursorSav);
}


/* ********************************************************************* */
VOID BuildCopyList()
{
	ClearCopyList();
	AddSectionFilesToCopyList("Minimum Files", szSrcDir, szInstallPath);

	if (installType == itComplete)
		AddSectionFilesToCopyList("Extra Files", szSrcDir, szInstallPath);
}


/* ********************************************************************* */
VOID BuildBillboardList()
{
	AddToBillboardList("mscuistf.dll", DLG_BOARD1, "FModelessDlgProc", 5);
	AddToBillboardList("mscuistf.dll", DLG_BOARD2, "FModelessDlgProc", 5);
	AddToBillboardList("mscuistf.dll", DLG_BOARD3, "FModelessDlgProc", 5);
	AddToBillboardList("mscuistf.dll", DLG_BOARD4, "FModelessDlgProc", 5);

	SetCopyGaugePosition(155, 130);
}


/* ********************************************************************* */
VOID Install()
{
	CHAR rgchT[16];
	INT  iCursorSav;

	CopyFilesInCopyList();

	UIStartDlg("mscuistf.dll", DLG_REGINI, "FModelessDlgProc", 0, NULL,
			(LPSTR)rgchT, 16);
	iCursorSav = ShowWaitCursor();
	
	CreateIniKeyValue("WIN.INI", "Extensions", "smp", SzCat2Str(szInstallPath,
			"SAMPLE.EXE ^.", "smp"), cmoOverwrite);

	CreateIniKeyValue("WIN.INI", "Sample", "Path", szInstallPath, cmoOverwrite);

	CreateProgmanGroup("Sample 4.0", "", cmoVital);
	ShowProgmanGroup("Sample 4.0",  1, cmoVital);

	CreateProgmanItem("Sample 4.0", "Sample Exe", SzCatStr(szInstallPath,
				"sample.exe"), "", cmoOverwrite);

	RestoreCursor(iCursorSav);
	UIPop(1);
}





/* **************************************************************** */
VOID AskQuit()
{
	CHAR rgchT[16];

	while (TRUE)
		{
		UIStartDlg("mscuistf.dll", DLG_QUIT, "FQuitDlgProc", HELP_ASKQUIT,
			"FHelpDlgProc", (LPSTR)rgchT, 15);
	
		switch ((WORD)(rgchT[0]+rgchT[1]))
			{
		default:
			continue;

		case IDC_QUITEXIT:
			UIPopAll();
			EndSetupToolkit();
			exit(0);

		case IDC_CANCEL:
		case IDC_QUITCONTINUE:
			break;
			}

		UIPop(1);
		return;
		}
}

/* **************************************************************** */
VOID WelcomeDialog()
{
        CHAR rgchT[16];

        while (TRUE)
            {
		UIStartDlg("mscuistf.dll", DLG_WELCOME, "FInfoDlgProc",
				HELP_APPLICATION, "FHelpDlgProc", (LPSTR)rgchT, 15);
	
		switch ((WORD)(rgchT[0]+rgchT[1]))
			{
		case IDC_CANCEL:
		case IDC_TYPEEXIT:
			AskQuit();
			continue;

		default:
			continue;

		case IDC_CONTINUE:
			break;
			}

		UIPop(1);
		return;
		}
}

/* **************************************************************** */
VOID InstallTypeDialog()
{
	CHAR rgchT[16];

	while (TRUE)
		{
		SetSymbolValue("RadioDefault", "1");
		UIStartDlg("mscuistf.dll", DLG_INSTALLTYPE, "FRadioDlgProc",
				HELP_INSTALLTYPE, "FHelpDlgProc", (LPSTR)rgchT, 15);
	
		switch ((WORD)(rgchT[0]+rgchT[1]))
			{
		case IDC_CANCEL:
		case IDC_TYPEEXIT:
			AskQuit();
			continue;

		default:
			continue;

		case IDC_CONTINUE:
			GetSymbolValue("ButtonChecked", rgchT, 15);
			installType = (*rgchT == '1') ? itComplete : itMinimum;
			break;
			}

		UIPop(1);
		return;
		}
}


/* **************************************************************** */
VOID BlankPathDialog()
{
	CHAR rgchT[16];

	while (TRUE)
		{
		UIStartDlg("mscuistf.dll", DLG_BLANK, "FInfo0DlgProc", 0, NULL,
				(LPSTR)rgchT, 15);
	
		switch ((WORD)(rgchT[0]+rgchT[1]))
			{
		default:
			continue;

		case IDC_BLANKEXIT:
			AskQuit();
			continue;

		case IDC_CANCEL:
		case IDC_BLANKCONTINUE:
			break;
			}

		UIPop(1);
		return;
		}
}


/* **************************************************************** */
VOID DestIsSrcDialog()
{
	CHAR rgchT[16];

	while (TRUE)
		{
		UIStartDlg("mscuistf.dll", DLG_DESTISSRC, "FInfo0DlgProc", 0, NULL,
				(LPSTR)rgchT, 15);
	
		switch ((WORD)(rgchT[0]+rgchT[1]))
			{
		default:
			continue;

		case IDC_DESTISSRCEXIT:
			AskQuit();
			continue;

		case IDC_CANCEL:
		case IDC_DESTISSRCCONTINUE:
			break;
			}

		UIPop(1);
		return;
		}
}


/* **************************************************************** */
VOID InvalidPathDialog()
{
	CHAR rgchT[16];

	while (TRUE)
		{
		UIStartDlg("mscuistf.dll", DLG_INVALIDPATH, "FInfo0DlgProc", 0, NULL,
				(LPSTR)rgchT, 15);
	
		switch ((WORD)(rgchT[0]+rgchT[1]))
			{
		default:
			continue;

		case IDC_INVALIDEXIT:
			AskQuit();
			continue;

		case IDC_CANCEL:
		case IDC_INVALIDCONTINUE:
			break;
			}

		UIPop(1);
		return;
		}
}


/* **************************************************************** */
VOID AppendSlash(LPSTR sz1)
{
	LPSTR sz2 = sz1;

	while (*sz2)
		{
		sz1 = sz2;
		sz2 = AnsiNext(sz1);
		}

	if (sz2 != sz1 + 1 || *sz1 != '\\')
		{
		*sz2++ = '\\';
		*sz2 = '\0';
		}
}


/* **************************************************************** */
VOID StripSlash(LPSTR sz1)
{
	LPSTR sz2 = sz1;

	while (*sz2)
		{
		sz1 = sz2;
		sz2 = AnsiNext(sz1);
		}

	if (sz2 == sz1 + 1 && *sz1 == '\\')
		*sz1 = '\0';
}


/* **************************************************************** */
BOOL FInstallPathDialog()
{
	CHAR rgchT[16];
	BOOL fRet;
	CHAR sz[cchMax];

	lstrcpy(sz, szInstallPath);

	StripSlash(sz);
	SetSymbolValue("EditTextIn", sz);
	SetSymbolValue("EditFocus", "END");

	while (TRUE)
		{
		UIStartDlg("mscuistf.dll", DLG_INSTALLPATH, "FEditDlgProc",
			HELP_INSTALLPATH, "FHelpDlgProc", (LPSTR)rgchT, 15);

		if ((WORD)(rgchT[0]+rgchT[1]) == IDC_PATHEXIT)
			{
			AskQuit();
			continue;
			}
		else if ((WORD)(rgchT[0]+rgchT[1]) == IDC_PATHCANCEL
				|| (WORD)(rgchT[0]+rgchT[1]) == IDC_CANCEL
				|| (WORD)(rgchT[0]+rgchT[1]) == IDC_BACK)
			{
			fRet = FALSE;
			break;
			}

		GetSymbolValue("EditTextOut", sz, cchMax);

		if (lstrlen(sz) == 0)
			{
			BlankPathDialog();
			continue;
			}
	
		if (!FValidDir(sz))
			{
			InvalidPathDialog();
			continue;
			}

		lstrcpy(szInstallPath, sz);
		AppendSlash(szInstallPath);

		if (lstrcmp(szInstallPath, szSrcDir) == 0)
			{
			DestIsSrcDialog();
			continue;
			}
	
		fRet = TRUE;
		break;
		}

	UIPop(1);
	return fRet;
}






/* ********************************************************************* */
VOID SetupError(int error)
{
	CHAR rgchT[16];
	BOOL fLoopForDialog = TRUE;
	static BOOL fInsideSetupError = FALSE;

	if (!fInsideSetupError)
		{
		fInsideSetupError = TRUE;

		UIPopAll();

		while (fLoopForDialog)
			{
			UIStartDlg("mscuistf.dll", DLG_ERROR, "FInfo0DlgProc", 0, NULL,
				(LPSTR)rgchT, 15);
	
			switch ((WORD)(rgchT[0]+rgchT[1]))
				{
			default:
				continue;

			case IDC_CANCEL:
			case IDC_ERROREXIT:
				fLoopForDialog = FALSE;
				break;
				}
			}

		UIPop(1);
		}

	EndSetupToolkit();
	exit(0);
}


/* **************************************************************** */
INT PASCAL WinMain(HANDLE hInstance, HANDLE hPrevInstance, LPSTR szCmdLineArgs,
		INT nCmdShow)
{
	hInst = hInstance;

	if (InitSetupToolkit(szCmdLineArgs) > 0)        // always should come first 
		{
		if (FInitRegDb())
			{
			SetBitmap("mscuistf.dll", 1);
			SetTitle("Sample 4.0 Setup");
			ReadInfFile(SzCatStr(szCurDir, "sample.inf"));

			lstrcpy(szInstallPath, "C:\\SAMPLE\\");
			GetSymbolValue("STF_SRCDIR", szSrcDir, cchMax);

			WelcomeDialog();

			do	{
				InstallTypeDialog();
				} while (!FInstallPathDialog());

			BuildLists();
			Install();
			TerminateRegDb();
			}

		EndSetupToolkit();
	        }
	exit(0);
	return(0);
}
