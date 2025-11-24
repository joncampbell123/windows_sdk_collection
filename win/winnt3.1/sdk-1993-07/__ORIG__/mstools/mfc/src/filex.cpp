// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "afx.h"

#pragma hdrstop

#include <errno.h>

#ifdef AFX_AUX_SEG
#pragma code_seg(AFX_AUX_SEG)
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#ifdef _DEBUG

// character strings to use for dumping CFileException
static char BASED_CODE szNone[] = "none";
static char BASED_CODE szGeneric[] = "generic";
static char BASED_CODE szFileNotFound[] = "fileNotFound";
static char BASED_CODE szBadPath[] = "badPath";
static char BASED_CODE szTooManyOpenFiles[] = "tooManyOpenFiles";
static char BASED_CODE szAccessDenied[] = "accessDenied";
static char BASED_CODE szInvalidFile[] = "invalidFile";
static char BASED_CODE szRemoveCurrentDir[] = "removeCurrentDir";
static char BASED_CODE szDirectoryFull[] = "directoryFull";
static char BASED_CODE szBadSeek[] = "badSeek";
static char BASED_CODE szHardIO[] = "hardIO";
static char BASED_CODE szSharingViolation[] = "sharingViolation";
static char BASED_CODE szLockViolation[] = "lockViolation";
static char BASED_CODE szDiskFull[] = "diskFull";
static char BASED_CODE szEndOfFile[] = "endOfFile";

static char FAR* BASED_CODE rgszCFileExceptionCause[] =
{
	szNone,
	szGeneric,
	szFileNotFound,
	szBadPath,
	szTooManyOpenFiles,
	szAccessDenied,
	szInvalidFile,
	szRemoveCurrentDir,
	szDirectoryFull,
	szBadSeek,
	szHardIO,
	szSharingViolation,
	szLockViolation,
	szDiskFull,
	szEndOfFile,
};

static char BASED_CODE szUnknown[] = "unknown";
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileException

IMPLEMENT_DYNAMIC(CFileException, CException)

void
CFileException::ThrowOsError(LONG lOsError)
{
	if (lOsError != 0)
		AfxThrowFileException(CFileException::OsErrorToException(lOsError),
			lOsError);
}

void
CFileException::ThrowErrno(int nErrno)
{
	if (nErrno != 0)
		AfxThrowFileException(CFileException::ErrnoToException(nErrno),
			_doserrno);
}


#ifdef _DEBUG
void
CFileException::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);
	dc << " m_cause = ";

	if (m_cause >= 0 &&
		m_cause < sizeof(rgszCFileExceptionCause) / sizeof(char FAR*))
	{
		dc << rgszCFileExceptionCause[m_cause];
	}
	else
	{
		dc << szUnknown;
	}
	dc << ", lOsError = " << m_lOsError;
}
#endif

void
PASCAL AfxThrowFileException(int cause, LONG lOsError)
{
#ifdef _DEBUG
	TRACE("CFile exception: ");
	if (cause >= 0 &&
		cause < sizeof(rgszCFileExceptionCause) / sizeof(char FAR*))
	{
		afxDump << (char FAR*)rgszCFileExceptionCause[cause];
	}
	else
	{
		afxDump << (char FAR*)szUnknown;
	}
	afxDump << ", OS error information = " << (void FAR*)lOsError << "\n";
#endif
	THROW(new CFileException(cause, lOsError));
}


int
CFileException::ErrnoToException(int nErrno)
{
	switch(nErrno)
	{
		case EPERM:
		case EACCES:
						return CFileException::accessDenied;
		case EBADF:     return CFileException::invalidFile;
		case EDEADLOCK: return CFileException::sharingViolation;
		case EMFILE:    return CFileException::tooManyOpenFiles;
		case ENOENT:
		case ENFILE:
						return CFileException::fileNotFound;
		case ENOSPC:    return CFileException::diskFull;
		case EINVAL:
		case EIO:       return CFileException::hardIO;
		default:
						return CFileException::generic;
	}
}


#ifdef _DOSWIN
int
CFileException::OsErrorToException(LONG lOsErr)
{
	switch ((UINT)lOsErr)
	{
		// DOS Error codes
		case 0x1: return CFileException::generic;
		case 0x2: return CFileException::fileNotFound;
		case 0x3: return CFileException::badPath;
		case 0x4: return CFileException::tooManyOpenFiles;
		case 0x5: return CFileException::accessDenied;
		case 0x6: return CFileException::invalidFile;
		case 0xf: return CFileException::badPath;
		case 0x10: return CFileException::removeCurrentDir;
		case 0x12: return CFileException::directoryFull;
		case 0x19: return CFileException::badSeek;
		case 0x1d: return CFileException::hardIO;
		case 0x1e: return CFileException::hardIO;
		case 0x1f: return CFileException::hardIO;
		case 0x58: return CFileException::hardIO;
		case 0x20: return CFileException::sharingViolation;
		case 0x21: return CFileException::lockViolation;
		case 0x23: return CFileException::tooManyOpenFiles;
		case 0x24: return CFileException::sharingViolation;
		case 0x41: return CFileException::accessDenied;
		case 0x43: return CFileException::fileNotFound;
		case 0x52: return CFileException::accessDenied;
		default: return CFileException::generic;
	}
}
#endif // _WINDOWS

#ifdef _NTWIN
#include "winerror.h"
int
CFileException::OsErrorToException(LONG lOsErr)
{
	switch ((UINT)lOsErr)
	{
	// NT Error codes
		case NO_ERROR:			return CFileException::none;
		case ERROR_FILE_NOT_FOUND:	return CFileException::fileNotFound;
		case ERROR_PATH_NOT_FOUND:	return CFileException::badPath;
		case ERROR_TOO_MANY_OPEN_FILES:	return CFileException::tooManyOpenFiles;
		case ERROR_ACCESS_DENIED:	return CFileException::accessDenied;
		case ERROR_INVALID_HANDLE:	return CFileException::fileNotFound;
		case ERROR_BAD_FORMAT:		return CFileException::invalidFile;
		case ERROR_INVALID_ACCESS:	return CFileException::accessDenied;
		case ERROR_INVALID_DRIVE:	return CFileException::badPath;
		case ERROR_CURRENT_DIRECTORY:	return CFileException::removeCurrentDir;
		case ERROR_NOT_SAME_DEVICE:	return CFileException::badPath;
		case ERROR_NO_MORE_FILES:	return CFileException::fileNotFound;
		case ERROR_WRITE_PROTECT:	return CFileException::accessDenied;
		case ERROR_BAD_UNIT:		return CFileException::hardIO;
		case ERROR_NOT_READY:		return CFileException::hardIO;
		case ERROR_BAD_COMMAND:		return CFileException::hardIO;
		case ERROR_CRC:			return CFileException::hardIO;
		case ERROR_BAD_LENGTH:		return CFileException::badSeek;
		case ERROR_SEEK:		return CFileException::badSeek;
		case ERROR_NOT_DOS_DISK:	return CFileException::invalidFile;
		case ERROR_SECTOR_NOT_FOUND:	return CFileException::badSeek;
		case ERROR_WRITE_FAULT:		return CFileException::accessDenied;
		case ERROR_READ_FAULT:		return CFileException::badSeek;
		case ERROR_SHARING_VIOLATION:	return CFileException::sharingViolation;
		case ERROR_NOT_LOCKED: 		return CFileException::lockViolation;
		case ERROR_LOCK_VIOLATION:	return CFileException::lockViolation;
		case ERROR_WRONG_DISK:		return CFileException::badPath;
		case ERROR_SHARING_BUFFER_EXCEEDED:	return CFileException::tooManyOpenFiles;
		case ERROR_HANDLE_EOF:		return CFileException::endOfFile;
		case ERROR_HANDLE_DISK_FULL:	return CFileException::diskFull;
		case ERROR_DUP_NAME:		return CFileException::badPath;
		case ERROR_BAD_NETPATH:		return CFileException::badPath;
		case ERROR_NETWORK_BUSY:	return CFileException::accessDenied;
		case ERROR_DEV_NOT_EXIST:	return CFileException::badPath;
		case ERROR_ADAP_HDW_ERR:	return CFileException::hardIO;
		case ERROR_BAD_NET_RESP:	return CFileException::accessDenied;
		case ERROR_UNEXP_NET_ERR:	return CFileException::hardIO;
		case ERROR_BAD_REM_ADAP:	return CFileException::invalidFile;
		case ERROR_NO_SPOOL_SPACE:	return CFileException::directoryFull;
		case ERROR_NETNAME_DELETED:	return CFileException::accessDenied;
		case ERROR_NETWORK_ACCESS_DENIED:	return CFileException::accessDenied;
		case ERROR_BAD_DEV_TYPE:	return CFileException::invalidFile;
		case ERROR_BAD_NET_NAME:	return CFileException::badPath;
		case ERROR_TOO_MANY_NAMES:	return CFileException::tooManyOpenFiles;
		case ERROR_SHARING_PAUSED:	return CFileException::badPath;
		case ERROR_REQ_NOT_ACCEP:	return CFileException::accessDenied;
		case ERROR_FILE_EXISTS:		return CFileException::accessDenied;
		case ERROR_CANNOT_MAKE:		return CFileException::accessDenied;
		case ERROR_ALREADY_ASSIGNED:	return CFileException::badPath;
		case ERROR_INVALID_PASSWORD:	return CFileException::accessDenied;
		case ERROR_NET_WRITE_FAULT:	return CFileException::hardIO;
		case ERROR_DISK_CHANGE:		return CFileException::fileNotFound;
		case ERROR_DRIVE_LOCKED:	return CFileException::lockViolation;
		case ERROR_BUFFER_OVERFLOW:	return CFileException::badPath;
		case ERROR_DISK_FULL:		return CFileException::diskFull;
		case ERROR_NO_MORE_SEARCH_HANDLES:	return CFileException::tooManyOpenFiles;
		case ERROR_INVALID_TARGET_HANDLE:	return CFileException::invalidFile;
		case ERROR_INVALID_CATEGORY:	return CFileException::hardIO;
		case ERROR_INVALID_NAME:	return CFileException::badPath;
		case ERROR_INVALID_LEVEL:	return CFileException::badPath;
		case ERROR_NO_VOLUME_LABEL:	return CFileException::badPath;
		case ERROR_NEGATIVE_SEEK:	return CFileException::badSeek;
		case ERROR_SEEK_ON_DEVICE:	return CFileException::badSeek;
		case ERROR_DIR_NOT_ROOT:	return CFileException::badPath;
		case ERROR_DIR_NOT_EMPTY:	return CFileException::removeCurrentDir;
		case ERROR_LABEL_TOO_LONG:	return CFileException::badPath;
		case ERROR_BAD_PATHNAME:	return CFileException::badPath;
		case ERROR_LOCK_FAILED:		return CFileException::lockViolation;
		case ERROR_BUSY:		return CFileException::accessDenied;
		case ERROR_INVALID_ORDINAL:	return CFileException::invalidFile;
		case ERROR_ALREADY_EXISTS:	return CFileException::accessDenied;
		case ERROR_INVALID_EXE_SIGNATURE:	return CFileException::invalidFile;
		case ERROR_BAD_EXE_FORMAT:	return CFileException::invalidFile;
		case ERROR_FILENAME_EXCED_RANGE:	return CFileException::badPath;
		case ERROR_META_EXPANSION_TOO_LONG:	return CFileException::badPath;
		case ERROR_DIRECTORY:		return CFileException::badPath;
		case ERROR_OPERATION_ABORTED:	return CFileException::hardIO;
		case ERROR_IO_INCOMPLETE:	return CFileException::hardIO;
		case ERROR_IO_PENDING:		return CFileException::hardIO;
		case ERROR_SWAPERROR:		return CFileException::accessDenied;
		default: 			return CFileException::generic;
	}
}
#endif // _NTWIN


