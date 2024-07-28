/**************************************************************************
 *
 * InstVer.c - SDK version-checking installer for the 3.0 compatible windows
 *	       DLLs
 *
 **************************************************************************/

#include <dos.h>	/* for file i/o */
#include <fcntl.h>	/* for file i/o */
#include <share.h>	/* for file i/o */
#include <io.h> 	/* for file i/o */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef DLL
    #undef NULL
    #include <windows.h>
#endif

#include "Instpriv.h"
#include "Instver.h"

#ifndef DLL
#define LIB
#endif
#include  <ver.h>


/* forward declaration of functions used in this module */
BOOL PASCAL InstallFiles (LPSTR lpszSrcDir,
			  LPSTR lpszWinDir);

BOOL PASCAL DoVerInstall (LPSTR pszSrcPath,
			  LPSTR pszSrcFile,
			  LPSTR pszDstPath);

BOOL PASCAL GetFilesFromDat (LPSTR lpszDat);

void PASCAL CleanUp (void);

/* header of list of files to be copied */
char *pDatLibFiles = NULL;
BOOL bDatFile;


#ifdef DLL

/* ========================= START: Windows DLL code ====================== */

/**************************************************************************
 *
 * int FAR PASCAL LibMain (hInstance, wDataSeg, wcbHeapSize, lpstrCmdLine)
 *
 * Routine invoked by the dll entry code (LibEntry)
 *
 * ENTRY   : HANDLE hInst    - DLL instance handle
 *	     WORD   wDS      - DLL data segment
 *	     WORD   wcbHS    - number of bytes in DLL's heap
 *	     LPSTR  lpstrCmd - Command line string
 *
 * EXIT    : TRUE - success
 * WARNING : None
 *
 **************************************************************************/
int FAR PASCAL LibMain(HANDLE hInst, WORD wDS, WORD wcbHS ,LPSTR lpstrCmd)
{
     /* Make the DLL data seg. moveable */
     if (wcbHS > 0)
	 UnlockData (0);

     /* success */
     return 1;
}

/**************************************************************************
 *
 * int FAR PASCAL WEP (int nParam)
 *
 * Handles clean-up when DLL is removed from the system. Since there is
 * nothing to clean up, this function just returns success.
 *
 * ENTRY   : nExitType - Exit type code (system shutdown, DLL being
 *			 freed...etc)
 * EXIT    : TRUE - success
 * WARNING : None
 *
 **************************************************************************/
int FAR PASCAL WEP (int nExitType)
{
    return TRUE;
}
/**************************************************************************
 *
 * BOOL InstallVersionFiles (LPSTR szSrcPath, LPSTR szWindowsPath)
 *
 * Top level (public) version-checking installer routine. Copies the DLLs
 * from the specified source pathname to the Windows system directory. If a
 * DAT file is specified, the files listed in it are copied over,
 * otherwise, the default list of files are copied.
 *
 * ENTRY   : LPSTR lpszSrcPath	   - path where the source files are located
 *	     LPSTR lpszWindowsPath - Windows directory or NULL
 *	     LPSTR lpszWindowsPath - .DAT filename or NULL
 *
 * EXIT    : TRUE   - success
 *	     ERR_??code - otherwise ( The complete list is in instver.h )
 *
 * WARNING : If lpszWindowsPath is NULL, the function determines the path
 *	     of the Windows installation.
 *
 **************************************************************************/

BOOL FAR PASCAL InstallVersionFiles (LPSTR lpszSrcPath,
				     LPSTR lpszWindowsPath,
				     LPSTR lpszDatFile)
{
    int wRet;
    static char szWindowsPath [_MAX_PATH];

    /* make sure input pathname and .DAT filenames are real */
    if ( !DosValidDir(lpszSrcPath))
	return ERR_BADSRCDIR;

    /* If a .DAT file is specified, set a flag, otherwise, make sure it
     * exists at the specified path
     */
    if (lpszDatFile){
	if (FileExists (lpszDatFile) < 0)
	    return ERR_BADDATFILE;
	bDatFile = TRUE;
    }
    else
	bDatFile = FALSE;

    /* If user hasn't supplied a Windows pathname, get the pathname of the
     * current Windows installation, otherwise verify the supplied
     * pathname.
     */
    if (!lpszWindowsPath){
	if (!GetWindowsDirectory ((LPSTR)szWindowsPath, _MAX_PATH))
	    return ERR_BADWINDIR;
	lpszWindowsPath = szWindowsPath;
    }
    else{
	if ( !DosValidDir (lpszWindowsPath))
	    return ERR_BADWINDIR;
    }

    /* Parse names in the DAT file into temporary buffer  */
    if (bDatFile){
	if ((wRet = GetFilesFromDat (lpszDatFile)) < 0)
	    return wRet;
    }

    /* Copy each of the files over, after version checking */
    wRet = InstallFiles (lpszSrcPath, lpszWindowsPath);

    return (wRet < 0 ? wRet : TRUE);
}
/* ========================= END: Windows DLL code ======================== */

#else




/* ========================= START: DOS .EXE code ========================= */

int PASCAL GrabArguments (char * argv[], LPSTR szSrcPath,
					 LPSTR szWinPath, LPSTR szDatPath);

/**************************************************************************
 *
 * Entry point for the DOS .EXE version of the installer. The input arguments
 * are the source files pathname, the Windows pathname, and an optional .DAT
 * file containing names of the files to be installed.
 *
 * WARNING : Target Windows directory has to exist.
 *
 **************************************************************************/
main (int argc, char *argv[])
{
    int wRet;
    static char szSrcPath     [_MAX_PATH];
    static char szDatPath     [_MAX_PATH];
    static char szWindowsPath [_MAX_PATH];

    /* Verify that there are at least 2 arguments and parse the argument
     * list to obtain source dir. and windows, and the .DAT (optional) pathnames
     */
    if ((argc < 3) || (!GrabArguments (argv, szSrcPath,
					     szWindowsPath, szDatPath)))
	return ERR_BADARGS;

    /* Validate the input pathnames */

    if ( !DosValidDir ((LPSTR)szSrcPath))
	return ERR_BADSRCDIR;
    if ( !DosValidDir ((LPSTR)szWindowsPath))
	return ERR_BADWINDIR;
    if (bDatFile)
       if (FileExists (szDatPath) < 0)
	   return ERR_BADDATFILE;

    /* Parse names in the DAT file into temporary buffer  */
    if (bDatFile){
	if ((wRet = GetFilesFromDat (szDatPath)) < 0)
	    return wRet;
    }

    /* Copy each of the files over, after version checking */
    wRet = InstallFiles (szSrcPath, szWindowsPath);

    return (wRet < 0 ? wRet : TRUE);
}

/**************************************************************************
 *
 * GrabArguments (char * argv[], LPSTR szSrcPath, LPSTR szWinPath, LPSTR lpszDat)
 *
 * Gets the pathnames from the program's argument list and copies them into
 * buffers.
 *
 * ENTRY   : char * argv[]  - argument list
 *
 * EXIT    : LPSTR lpszSrcPath	   - path where the source files are located
 *	     LPSTR lpszWindowsPath - Windows directory pathname
 *	     LPSTR lpszDatPath	   - Optional .DAT file name
 *
 *	   Returns TRUE if successful, FALSE otherwise
 *	   A flag is set if a .DAT file is specified.
 *
 * WARNING : First two args. are required. The .DAT file is optional
 *
 **************************************************************************/
int PASCAL GrabArguments (char * argv[], LPSTR szSrcPath,
					 LPSTR szWinPath, LPSTR szDatPath)
{
    register char * psz;

    if (psz = argv [1])
	_fstrncpy (szSrcPath, (LPSTR)psz, _MAX_PATH - 1);
    else
	return FALSE;

    if (psz = argv [2])
	_fstrncpy (szWinPath, (LPSTR)psz, _MAX_PATH - 1);
    else
	return FALSE;

    if (psz = argv [3]){
	_fstrncpy (szDatPath, (LPSTR)psz, _MAX_PATH - 1);
	bDatFile = TRUE;
    }
    else{
	*szDatPath = '\0';
	bDatFile = FALSE;
    }
    return TRUE;
}
#endif
/* ========================= END: DOS .EXE code =========================== */





/* ========================= START: common code =========================== */

/**************************************************************************
 *
 * BOOL PASCAL InstallFiles (LPSTR szSrcDir, LPSTR lpszWinDir)
 *
 * Core installation routine. Copies files from either the default internal
 * list, or the .DAT file buffer to the destination Windows system directory.
 * Both the above are in the format:
 *
 *  file1\0file2\0file3\0....fileN\0\0
 *
 * ENTRY   : LPSTR lpszSrcDir - Source files directory
 *	     LPSTR lpszWinDir - Target Windows directory.
 *
 * EXIT    : TRUE   - success
 *	     ERR_??code - otherwise ( The complete list is in instver.h )
 * WARNING : None
 *
 **************************************************************************/

BOOL PASCAL InstallFiles (LPSTR lpszSrcDir, LPSTR lpszWinDir)
{
    int wRet = 0;
    static char *szFilePtr;

    /* point to appropriate filenames buffer */
    if (bDatFile)
       szFilePtr = pDatLibFiles;
    else
       szFilePtr = szDefLibFiles;   /* default */

    for (; *szFilePtr && (wRet >= 0); szFilePtr += _fstrlen(szFilePtr) + 1){

	if ((wRet = DoVerInstall (lpszSrcDir, szFilePtr, lpszWinDir)) < 0){
	    CleanUp ();
	    return wRet;
	}
    }

    /* free DAT file buffer, if it exists */
    CleanUp ();

    return (wRet < 0 ? wRet : TRUE);
}

/**************************************************************************
 *
 * BOOL PASCAL GetFilesFromDat (LPSTR lpszDat)
 *
 * Reads the .DAT file which contains a list of files to be copied and
 * parses it into a buffer in the form:
 *
 *  file1\0file2\0file3\0....fileN\0\0
 *
 * ENTRY   :   LPSTR lpszDat  - Dat file pathname.
 *
 * EXIT    : TRUE   - success
 *	     ERR_??code - otherwise ( The complete list is in instver.h )
 *
 *	     pDatLibFiles points to parsed filename buffer.
 *
 * WARNING : .DAT file has to consist of only filenames (no full pathnames)
 *	     with one filename per line. No comments are allowed. Leading
 *	     and trailing whitespace may be present.
 *
 *	     The DAT file has to contain the filenames as they will appear in
 *	     the destination directory. i.e even if a file exists in source
 *	     directory as FOO.EX_,(a Windows compressed renamed file),the DAT
 *	     file should list it as FOO.EXT. The copying function will take
 *	     care of renaming FOO.EX_ appropriately after it is expanded.
 *
 **************************************************************************/
BOOL PASCAL GetFilesFromDat (LPSTR lpszDat)

{
    static char szDat [_MAX_PATH];
    char * pSrcBuf;
    char * pTmpSrc, *pTmpDst;
    char * pBufEnd;
    int wLen;
    int fh;
    int c;

    /* copy pathname to a NEAR buffer for FOPEN () */
    _fstrcpy (szDat, lpszDat);

    fh = FOPEN (szDat);
    if (fh == -1)
	return ERR_CANTOPENDATFILE;

    /* Get the length of the file */
    wLen = (WORD)FSEEK(fh,0L,SEEK_END);
    FSEEK (fh, 0L, SEEK_SET);

    /* allocate for (temporary)source and (destination).DAT file buffers */
    pSrcBuf = pTmpSrc = ALLOC (wLen);
    if (!pSrcBuf)
	return ERR_NOMEM;

    pDatLibFiles = pTmpDst = ALLOC (wLen);
    if (!pDatLibFiles){
	FREE (pSrcBuf);
	return ERR_NOMEM;
    }
    pBufEnd = pSrcBuf + wLen;

    /* Read in entire file into temporary source buffer to avoid
     * repeated file i/o operations.
     */
    if ((int)FREAD (fh, pSrcBuf, wLen) != wLen)
	return ERR_READINGDATFILE;
    FCLOSE (fh);

    /* Now parse source buffer into destination buffer into format:
     *	   file1\0file2\0file3\0....fileN\0\0
     */
    while (pTmpSrc < pBufEnd){
	switch (c = *pTmpSrc++){
	    case ' ' :
	    case '\r':
	    case '\t':
		 break;      /* ignore whitespace */

	    case '\n':
	    case EOF :
		 *pTmpDst++ = '\0';
		 break;      /* terminate current filename */

	    default:
		 *pTmpDst++ = (char)c;
	}
    }

    /* add the additional final NULL to signal end of list */
    *pTmpDst = '\0';

    /* discard temporary source buffer */
    FREE (pSrcBuf);
}


/**************************************************************************
 *
 * BOOL PASCAL DoVerInstall (LPSTR lpszSrcPath, LPSTR lpszDstFile
 *			     LPSTR lpszDstPath)
 *
 * Uses the VER library to determine the appropriate directory to copy the
 * file to, and copies the file over. If the file sxists in the source
 * directory is lz-compressed and in LZ-renamed format, the destination file
 * is expanded and renamed correctly.
 *
 * ENTRY   : LPSTR lpszSrcPath - Source directory name.
 *	     LPSTR lpszDstFile - Destination filename.
 *	     LPSTR lpszDstPath - Target pathname.
 *
 * EXIT    : TRUE  - OK to copy over library file
 *	     FALSE - version mismatch. Don't copy file
 *	     Otherwise a ERR_ code to indicate an installation error has occured
 *
 **************************************************************************/
BOOL PASCAL DoVerInstall (LPSTR lpszSrcPath,
			  LPSTR lpszDstFile,
			  LPSTR lpszDstPath)
{
    static char szCurDir [_MAX_PATH];
    static char szDstDir [_MAX_PATH];
    static char szTmpFile[_MAX_PATH];
    WORD wRet;
    int wCopy = TRUE;
    DWORD dwRet;
    WORD wCurDirLen = sizeof(szCurDir);
    WORD wDstDirLen = sizeof(szDstDir);
    WORD wTmpFileLen = sizeof(szTmpFile);

    wRet = VerFindFile (VFFF_ISSHAREDFILE,
			lpszDstFile,
			lpszDstPath,
			lpszDstPath,
			szCurDir,
			&wCurDirLen,
			szDstDir,
			&wDstDirLen);

    /*
     * The LZ-routine called by VerInstallFile looks for a renamed file
     * (i.e with the "_" in it's extension if the actual file is not
     * found in the source directory).
     */
    dwRet = VerInstallFile ( 0,
			     lpszDstFile,
			     lpszDstFile,
			     lpszSrcPath,
			     szDstDir,
			     szCurDir,
			     szTmpFile,
			     &wTmpFileLen);

    /* Compare return value against all defined bit values and take
     * appropriate action on each.
     */
    if (dwRet & (VIF_MISMATCH|VIF_SRCOLD|VIF_DIFFLANG|VIF_DIFFCODEPG|\
		      VIF_DIFFTYPE | VIF_CANNOTREADDST)){


	 wCopy = FALSE; 	     /* a mismatch , don't copy */
    }
    /* The remaining flags (if any) represent errors. Depending on broad
     * classification of these, return a suitable INSTVER error code
     */
    else if (dwRet & (VIF_WRITEPROT | VIF_FILEINUSE | VIF_ACCESSVIOLATION|
		      VIF_SHARINGVIOLATION | VIF_CANNOTDELETE |\
		      VIF_CANNOTREADSRC))

	 wCopy = ERR_CANNOTREADSRC;

    else if (dwRet & VIF_OUTOFSPACE)

	 wCopy =  ERR_OUTOFSPACE;

    else if (dwRet & VIF_OUTOFMEMORY)

	 wCopy =  ERR_NOMEM;

    else if (dwRet & (VIF_CANNOTCREATE | VIF_CANNOTRENAME | VIF_TEMPFILE))

	 wCopy =  ERR_CREATINGFILE;

    if (wCopy <= 0) {

	/* delete the temp. file if one has been left behind */
	if (dwRet & VIF_TEMPFILE) {
	     if (!SLASH(szDstDir[_fstrlen (szDstDir) - 1]))
		 _fstrcat (szDstDir, "\\");
	     _fstrcat (szDstDir, szTmpFile);
	     DosDelete ((LPSTR)szDstDir);
	}
    }

    return wCopy;
}
/**************************************************************************
 *
 * void PASCAL CleanUp (void)
 *
 * Frees allocated buffers
 *
 * ENTRY   : none
 * EXIT    : none
 * WARNING : none
 *
 **************************************************************************/
void PASCAL CleanUp (void)
{
    if (pDatLibFiles){

	/* free the DAT file buffer */
	FREE (pDatLibFiles);
	pDatLibFiles = NULL;
    }
}
