// Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation,
// All rights reserved.

// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#ifndef __AFX_H__
#define __AFX_H__

#define _MFC_VER 0x0100 /* Microsoft Foundation Classes */
#define _AFX     1      /* Microsoft Application Framework Classes */

#ifndef __cplusplus
#error Microsoft Foundation Classes require C++ compilation (use a .cpp suffix)
#endif

/////////////////////////////////////////////////////////////////////////////
// Classes declared in this file
//   in addition to standard primitive data types and various helper macros

struct CRuntimeClass;                 // object type information

class CObject;                        // the root of all objects classes

	class CException;                 // the root of all exceptions
		class CMemoryException;       // out-of-memory exception
		class CNotSupportedException; // feature not supported exception
		class CArchiveException;      // archive exception
		class CFileException;         // file exception

	class CFile;                      // raw binary file
		class CStdioFile;             // buffered stdio text/binary file
		class CMemFile;               // memory based file

// Non CObject classes
class CString;                        // growable string type
class CTimeSpan;                      // time/date difference
class CTime;                          // absolute time/date
struct CFileStatus;                   // file status information
struct CMemoryState;                  // diagnostic memory support

class CArchive;                       // object persistence tool
class CDumpContext;                   // object diagnostic dumping

/////////////////////////////////////////////////////////////////////////////
// Other includes from standard "C" runtimes


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef _NTWIN
#pragma warning(default: 4069)
#endif
#include <time.h>

/////////////////////////////////////////////////////////////////////////////
// Target version control

// For target version (one of)
//   _WINDOWS  : for Microsoft Windows target (defined by #include <afxwin.h>)
//   _DOS      : for Microsoft DOS (non Windows) target
//   _NTWIN    : with _WINDOWS is a Windows NT GUI application, by itself is a 
//               character application (only <afx.h> applies)
//
// Additional build options:
//   _DEBUG    : debug versions (full diagnostics)
//
// Internal version flags:
//   _DOSWIN   : for Microsoft Windows and DOS target code (internal)
//   _NEARDATA : ambient near data pointers needing far overloads

#if !defined(_WINDOWS) && !defined(_DOS) && !defined(_NTWIN)
#error Please define one of _WINDOWS or _DOS or _NTWIN
#endif

#if defined(_WINDOWS) && defined(_DOS)
#error Please define only one of _WINDOWS or _DOS
#endif

#ifndef _NTWIN
#ifdef _WINDOWS
#define _DOSWIN
#endif

#ifdef _DOS
#define _DOSWIN
#endif
#endif // _NTWIN

#if defined(_M_I86SM) || defined(_M_I86MM)
#define _NEARDATA
#endif

/////////////////////////////////////////////////////////////////////////////
// Standard preprocessor symbols:


#ifdef _DOSWIN
#ifndef PASCAL
#define PASCAL _pascal
#endif

#ifndef CDECL
#define CDECL _cdecl
#endif

#ifndef FAR
#define FAR _far
#endif

#ifndef NEAR
#define NEAR _near
#endif

#else

#include <excpt.h>
#include <windef.h>
#include <winbase.h>

#define BASED_CODE
#undef _NEARDATA

#ifndef PASCAL
#define PASCAL pascal
#endif

#ifndef CDECL
#define CDECL cdecl
#endif

#ifndef FAR
#define FAR far
#endif

#ifndef NEAR
#define NEAR near
#endif

#define _huge
#define huge
#define near
#define far

#define _fstrcpy strcpy
#define _fstrlen strlen
#define _fstrcmp strcmp
#define _fstrcat strcat
#define _fstrncpy strncpy
#define _fstrncmp strncmp
#define _fmemcpy memcpy
#define _fmalloc malloc
#define _frealloc realloc
#define _ffree free

// _export is not accepted by the NT compiler
#define __export
#define _export
#define export

#undef GetCurrentTime
inline unsigned long GetCurrentTime(void)
	{ return ::GetTickCount(); }

#endif

/////////////////////////////////////////////////////////////////////////////
// Basic types (from Windows)

typedef unsigned char  BYTE;   // 8-bit unsigned entity
typedef unsigned short WORD;   // 16-bit unsigned number
typedef unsigned int   UINT;   // machine sized unsigned number (preferred)
typedef long           LONG;   // 32-bit signed number
typedef unsigned long DWORD;   // 32-bit unsigned number
typedef int            BOOL;   // BOOLean (0 or !=0)

typedef void*      POSITION;   // abstract iteration position

// Standard constants
#define FALSE   0
#define TRUE    1
#define NULL    0

/////////////////////////////////////////////////////////////////////////////
// Diagnostic support


#ifdef _DEBUG

extern "C"
{
void CDECL  AfxTrace(const char* pszFormat, ...);
void PASCAL AfxAssertFailedLine(const char FAR* lpszFileName, int nLine);
void PASCAL AfxAssertValidObject(const CObject* pOb);
}
#define TRACE              ::AfxTrace
#define THIS_FILE          __FILE__
#define ASSERT(f)          ((f) ? (void)0 : \
								::AfxAssertFailedLine(THIS_FILE, __LINE__))
#define VERIFY(f)          ASSERT(f)
#define ASSERT_VALID(pOb)  (::AfxAssertValidObject(pOb))

#else

#define ASSERT(f)          ((void)0)
#define VERIFY(f)          ((void)(f))
#define ASSERT_VALID(pOb)  ((void)0)
inline void CDECL AfxTrace(const char* /* pszFormat */, ...) { }
#define TRACE              1 ? (void)0 : ::AfxTrace

#endif // _DEBUG

// Explicit extern for version API/Windows 3.0 loader problem
#ifdef _WINDOWS
extern "C" int FAR PASCAL __export _afx_version();
#else
extern "C" int FAR PASCAL _afx_version();
#endif

// Turn off warnings for /W4
// To resume any of these warning: #pragma warning(default: 4xxx)
// which should be placed after the AFX include files
#ifndef ALL_WARNINGS
#pragma warning(disable: 4001)  // nameless unions are part of C++
#pragma warning(disable: 4134)  // message map member fxn casts
#pragma warning(disable: 4505)  // optimize away locals
#pragma warning(disable: 4510)  // default constructors are bad to have
#pragma warning(disable: 4511)  // private copy constructors are good to have
#pragma warning(disable: 4512)  // private operator= are good to have
#ifdef STRICT
#pragma warning(disable: 4305)  // STRICT handles are near*, integer truncation
#endif
#endif

/////////////////////////////////////////////////////////////////////////////
// Basic object model

struct CRuntimeClass
{
// Attributes
	const char* m_pszClassName;
	int m_nObjectSize;
	UINT m_wSchema; // schema number of the loaded class
	void (PASCAL *m_pfnConstruct)(void* p); // NULL => abstract class
	CRuntimeClass* m_pBaseClass;

// Operations
	CObject* CreateObject();
	BOOL ConstructObject(void* pThis);
	void Store(CArchive& ar);
	static CRuntimeClass* Load(CArchive& ar, UINT* pwSchema);

// Implementation
	static CRuntimeClass* pFirstClass; // start of class list
	CRuntimeClass* m_pNextClass;       // linked list of registered classes
};


/////////////////////////////////////////////////////////////////////////////
// class CObject is the root of all compliant objects

#if defined(_M_I86MM)
// force vtables to be in far code segments for medium model
class FAR CObjectRoot
{
protected:
	virtual CRuntimeClass* GetRuntimeClass() NEAR const = 0;
};

#pragma warning(disable: 4149)  // don't warn for medium model change
class NEAR CObject : public CObjectRoot
#else
class CObject
#endif
{
public:

// Object model (types, destruction, allocation)
	virtual CRuntimeClass* GetRuntimeClass() const;
	virtual ~CObject();  // virtual destructors are necessary

	// Diagnostic allocations
	void* operator new(size_t, void* p);
	void* operator new(size_t nSize);
	void operator delete(void* p);

#ifdef _DEBUG
	// for file name/line number tracking using DEBUG_NEW
	void* operator new(size_t nSize, const char FAR* lpszFileName, int nLine);
#endif

	// Disable the copy constructor and assignment by default so you will get
	//   compiler errors instead of unexpected behaviour if you pass objects
	//   by value or assign objects.
protected:
	CObject();
private:
	CObject(const CObject& objectSrc);
	void operator=(const CObject& objectSrc);

// Attributes
public:
	BOOL IsSerializable() const;

// Overridables
	virtual void Serialize(CArchive& ar);

	// Diagnostic Support
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;

// Implementation
public:
	// dynamic type checking/construction support
	BOOL IsKindOf(const CRuntimeClass* pClass) const;
	static void PASCAL Construct(void* pMemory);

	static CRuntimeClass NEAR classCObject;
};

#if defined(_M_I86MM)
#pragma warning(default: 4149)  // base class now ambient
#endif


// Helper macros
#define RUNTIME_CLASS(class_name)  \
	(&class_name::class##class_name)

/////////////////////////////////////////////////////////////////////////////
// Helper macros for declaring compliant classes

#define DECLARE_DYNAMIC(class_name) \
public:                                 \
	static CRuntimeClass NEAR class##class_name; \
	virtual CRuntimeClass* GetRuntimeClass() const;

#define DECLARE_SERIAL(class_name)  \
	DECLARE_DYNAMIC(class_name)     \
	static void PASCAL Construct(void* p); \
	friend CArchive& operator>>(CArchive& ar, class_name* &pOb);

// generate static object constructor for class registration
struct NEAR CClassInit
	{ CClassInit(CRuntimeClass* pNewClass); };

#define IMPLEMENT_DYNAMIC(class_name, base_class_name)              \
	CRuntimeClass NEAR class_name::class##class_name = {            \
		#class_name, sizeof(class_name), 0xFFFF, NULL,              \
		  &base_class_name::class##base_class_name, NULL };         \
	static CClassInit _init_##class_name(&class_name::class##class_name);\
	CRuntimeClass* class_name::GetRuntimeClass() const                  \
		{ return &class_name::class##class_name; }                      \
// end of IMPLEMENT_DYNAMIC

#define IMPLEMENT_SERIAL(class_name, base_class_name, wSchema)  \
	void    PASCAL class_name::Construct(void* p)                   \
		{ new(p) class_name; }                                          \
	CRuntimeClass NEAR class_name::class##class_name = {                \
		#class_name, sizeof(class_name), wSchema,                       \
		  &class_name::Construct,                                       \
		  &base_class_name::class##base_class_name, NULL };                 \
	static CClassInit _init_##class_name(&class_name::class##class_name);\
	CRuntimeClass* class_name::GetRuntimeClass() const                  \
		{ return &class_name::class##class_name; }                      \
	CArchive& operator>>(CArchive& ar, class_name* &pOb) \
		{ ar >> (CObject*&) pOb; \
		  if ((pOb != NULL) && !pOb->IsKindOf(RUNTIME_CLASS(class_name))) \
			AfxThrowArchiveException(CArchiveException::badClass);\
		  return ar; }                                          \
// end of IMPLEMENT_SERIAL

/////////////////////////////////////////////////////////////////////////////
// setjmp for Windows and C++


#ifdef _NTWIN
#ifdef _M_MRX000
typedef  double  jmp_buf[16];
#else
typedef  int     jmp_buf[8];
#define  setjmp  _setjmp
#endif
#else
typedef  int  jmp_buf[9];
#endif

#if defined(_WINDOWS) && defined(_DOSWIN)
extern "C" int  far pascal Catch(int FAR*);
#define setjmp  ::Catch
#else
extern "C" int  __cdecl setjmp(jmp_buf);
#endif


/////////////////////////////////////////////////////////////////////////////
// Exceptions

typedef void (CDECL *AFX_TERM_PROC)();
struct CExceptionLink;

AFX_TERM_PROC AfxSetTerminate(AFX_TERM_PROC);

void CDECL AfxAbort();
void CDECL AfxTerminate();

class CException : public CObject
{
	// abstract class for dynamic type checking
	DECLARE_DYNAMIC(CException)
};

// Exception global state - never access directly
struct CExceptionContext
{
	CException* m_pCurrent;
	BOOL m_bDeleteWhenDone;
	CExceptionLink* m_pLinkTop;

	void Throw(CException* pNewException);
	void Throw(CException* pNewException, BOOL bShared);
	void ThrowLast();

	void Cleanup(); // call to free up exception
};
extern CExceptionContext NEAR afxExceptionContext;

// Placed on frame for EXCEPTION linkage
struct CExceptionLink
{
	CExceptionLink* m_pLinkPrev;// previous top, next in handler chain
	jmp_buf         m_jumpBuf;   // arg for setjmp/longjmp

	CExceptionLink(CExceptionLink* NEAR& rLinkTop);
	~CExceptionLink();
};

/////////////////////////////////////////////////////////////////////////////
// Exception helper macros

#define TRY \
	{ \
	CExceptionLink _afxExLink(afxExceptionContext.m_pLinkTop); \
	if (setjmp(_afxExLink.m_jumpBuf) == 0) \

#define CATCH(class, e) \
	else { if (afxExceptionContext.m_pCurrent->IsKindOf(RUNTIME_CLASS(class)))\
	{ class* e = (class*) afxExceptionContext.m_pCurrent;

#define AND_CATCH(class, e) \
	} else if (afxExceptionContext.m_pCurrent->IsKindOf(RUNTIME_CLASS(class)))\
	{ class* e = (class*) afxExceptionContext.m_pCurrent;

#define END_CATCH \
	} else { THROW(afxExceptionContext.m_pCurrent); } \
   afxExceptionContext.Cleanup(); } }

#define THROW(e) \
	afxExceptionContext.Throw(e);
#define THROW_LAST() \
	afxExceptionContext.ThrowLast();

/////////////////////////////////////////////////////////////////////////////
// Standard Exception classes

class CMemoryException : public CException
{
	DECLARE_DYNAMIC(CMemoryException)
public: 
	CMemoryException();
};

class CNotSupportedException : public CException
{
	DECLARE_DYNAMIC(CNotSupportedException)
public: 
	CNotSupportedException();
};

class CArchiveException : public CException
{
	DECLARE_DYNAMIC(CArchiveException)
public:
	enum {
		none,
		generic,
		readOnly,
		endOfFile,
		writeOnly,
		badIndex,
		badClass,
		badSchema
	};

// Constructor
	CArchiveException(int cause = CArchiveException::none);

// Attributes
	int m_cause;

#ifdef _DEBUG
	virtual void Dump(CDumpContext& dc) const;
#endif
};

class CFileException : public CException
{
	DECLARE_DYNAMIC(CFileException)

public:
	enum {
		none,
		generic,
		fileNotFound,
		badPath,
		tooManyOpenFiles,
		accessDenied,
		invalidFile,
		removeCurrentDir,
		directoryFull,
		badSeek,
		hardIO,
		sharingViolation,
		lockViolation,
		diskFull,
		endOfFile
	};

// Constructors

	CFileException(int cause = CFileException::none, LONG lOsError = -1);

// Attributes
	int  m_cause;
	LONG m_lOsError;

// Operations

	// convert a OS dependent error code to a Cause
	static int OsErrorToException(LONG lOsError);
	static int ErrnoToException(int nErrno);
	
	// helper functions to throw exception after converting to a Cause
	static void ThrowOsError(LONG lOsError);
	static void ThrowErrno(int nErrno);

#ifdef _DEBUG
	virtual void Dump(CDumpContext&) const;
#endif
};

/////////////////////////////////////////////////////////////////////////////
// Standard exception throws

void PASCAL AfxThrowMemoryException();
void PASCAL AfxThrowNotSupportedException();
void PASCAL AfxThrowArchiveException(int cause);
void PASCAL AfxThrowFileException(int cause, LONG lOsError = -1);


/////////////////////////////////////////////////////////////////////////////
// File - raw unbuffered disk file I/O 

class CFile : public CObject
{
	DECLARE_DYNAMIC(CFile)

public:
// Flag values
	enum OpenFlags {
		modeRead =          0x0000,
		modeWrite =         0x0001,
		modeReadWrite =     0x0002,
		shareCompat =       0x0000,
		shareExclusive =    0x0010,
		shareDenyWrite =    0x0020,
		shareDenyRead =     0x0030,
		shareDenyNone =     0x0040,
		modeNoInherit =     0x0080,
		modeCreate =        0x1000,
		typeText =          0x4000, // typeText and typeBinary are used in
		typeBinary =   (int)0x8000 // derived classes only
		};

	enum Attribute {
		normal =    0x00,
		readOnly =  0x01,
		hidden =    0x02,
		system =    0x04,
		volume =    0x08,
		directory = 0x10,
		archive =   0x20
		};

	enum SeekPosition { begin = 0x0, current = 0x1, end = 0x2 };

	enum {hFileNull = -1};

// Constructors
	CFile();
	CFile(int hFile);
	CFile(const char* pszFileName, UINT nOpenFlags);

// Attributes
	UINT m_hFile;

	virtual DWORD GetPosition() const;
	virtual BOOL GetStatus(CFileStatus& rStatus) const;

// Operations
	virtual BOOL Open(const char* pszFileName, UINT nOpenFlags, CFileException* pError = NULL);

	static void Rename(const char* pszOldName, const char* pszNewName);
	static void Remove(const char* pszFileName);
	static BOOL GetStatus(const char* pszFileName, CFileStatus& rStatus);
	static void SetStatus(const char* pszFileName, const CFileStatus& status);

	DWORD   SeekToEnd();
	void    SeekToBegin();

// Overridables
	virtual CFile* Duplicate() const;

	virtual LONG Seek(LONG lOff, UINT nFrom);
	virtual void SetLength(DWORD dwNewLen);
	virtual DWORD GetLength() const;

	virtual UINT Read(void FAR* lpBuf, UINT nCount);
	virtual void Write(const void FAR* lpBuf, UINT nCount);

	virtual void LockRange(DWORD dwPos, DWORD dwCount);
	virtual void UnlockRange(DWORD dwPos, DWORD dwCount);

	virtual void Flush();
	virtual void Close();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Implementation
	virtual ~CFile();
protected:
	BOOL m_bCloseOnDelete;
};

/////////////////////////////////////////////////////////////////////////////
// STDIO file implementation

class CStdioFile : public CFile
{
	DECLARE_DYNAMIC(CStdioFile)

public:
// Constructors
	CStdioFile();
	CStdioFile(FILE* pOpenStream);
	CStdioFile(const char* pszFileName, UINT nOpenFlags);

// Attributes
	FILE* m_pStream;    // stdio FILE
						// m_hFile from base class is _fileno(m_pStream)

	virtual DWORD GetPosition() const;

// Overridables
	virtual BOOL Open(const char* pszFileName, UINT nOpenFlags, CFileException* pError = NULL);
	virtual UINT Read(void FAR* lpBuf, UINT nCount);
	virtual void Write(const void FAR* lpBuf, UINT nCount);
	virtual LONG Seek(LONG lOff, UINT nFrom);
	virtual void Flush();
	virtual void Close();

	virtual void WriteString(const char FAR* lpsz); // write a string, like "C" fputs
	virtual char FAR* ReadString(char FAR* lpsz, UINT nMax); // like "C" fgets

	// Unsupported APIs
	virtual CFile* Duplicate() const;
	virtual void LockRange(DWORD dwPos, DWORD dwCount);
	virtual void UnlockRange(DWORD dwPos, DWORD dwCount);


#ifdef _DEBUG
	void Dump(CDumpContext& dc) const;
#endif
// Implementation
	virtual ~CStdioFile();
};

////////////////////////////////////////////////////////////////////////////
// Memory based file implementation

class CMemFile : public CFile
{
	DECLARE_DYNAMIC(CMemFile)

public:
// Constructors
	CMemFile(UINT nGrowBytes = 1024);

// Attributes
	virtual DWORD GetPosition() const;
	virtual BOOL GetStatus(CFileStatus& rStatus) const;

// Overridables
	virtual LONG Seek(LONG lOff, UINT nFrom);
	virtual void SetLength(DWORD dwNewLen);
	virtual UINT Read(void FAR* lpBuf, UINT nCount);
	virtual void Write(const void FAR* lpBuf, UINT nCount);
	virtual void Flush();
	virtual void Close();

#ifdef _DEBUG
	virtual void Dump(CDumpContext& dc) const;
	virtual void AssertValid() const;
#endif

	// Unsupported APIs
	virtual CFile* Duplicate() const;
	virtual void LockRange(DWORD dwPos, DWORD dwCount);
	virtual void UnlockRange(DWORD dwPos, DWORD dwCount);


// Implementation
	virtual ~CMemFile();

protected:
	virtual BYTE FAR* Alloc(UINT nBytes);
	virtual BYTE FAR* Realloc(BYTE FAR* lpMem, UINT nBytes);
	virtual BYTE FAR* Memcpy(BYTE FAR* lpMemTarget, const BYTE FAR* lpMemSource, UINT nCount);
	virtual void Free(BYTE FAR* lpMem);
	virtual void GrowFile(DWORD dwNewLen);

	UINT m_nGrowBytes;
	UINT m_nPosition;
	UINT m_nBufferSize;
	UINT m_nFileSize;
	BYTE FAR* m_lpBuffer;
};

/////////////////////////////////////////////////////////////////////////////
// Strings

class CString
{
public:

// Constructors
	CString();
	CString(const CString& stringSrc);
	CString(char ch, int nRepeat = 1);
	CString(const char* psz);
	CString(const char* pch, int nLength);
#ifdef _NEARDATA
	// Additional versions for far string data
	CString(const char FAR* lpsz);
	CString(const char FAR* lpch, int nLength);
#endif
	~CString();

// Attributes & Operations

	// as an array of characters
	int GetLength() const;
	BOOL IsEmpty() const;
	void Empty();  // free up the data

	char GetAt(int nIndex) const;       // 0 based
	char operator[](int nIndex) const;  // same as GetAt
	void SetAt(int nIndex, char ch);
	operator const char*() const;       // as a C string

	// overloaded assignment
	const CString& operator=(const CString& stringSrc);
	const CString& operator=(char ch);
	const CString& operator=(const char* psz);

	// string concatenation
	const CString& operator+=(const CString& string);
	const CString& operator+=(char ch);
	const CString& operator+=(const char* psz);

	friend CString operator+(const CString& string1, const CString& string2);
	friend CString operator+(const CString& string, char ch);
	friend CString operator+(char ch, const CString& string);
	friend CString operator+(const CString& string, const char* psz);
	friend CString operator+(const char* psz, const CString& string);

	// string comparison
	int Compare(const char* psz) const;         // straight character
	int CompareNoCase(const char* psz) const;   // ignore case
	int Collate(const char* psz) const;         // NLS aware

	// simple sub-string extraction
	CString Mid(int nFirst, int nCount) const;
	CString Mid(int nFirst) const;
	CString Left(int nCount) const;
	CString Right(int nCount) const;

	CString SpanIncluding(const char* pszCharSet) const;
	CString SpanExcluding(const char* pszCharSet) const;

	// upper/lower/reverse conversion
	void MakeUpper();
	void MakeLower();
	void MakeReverse();

	// searching (return starting index, or -1 if not found)
	// look for a single character match
	int Find(char ch) const;                    // like "C" strchr
	int ReverseFind(char ch) const;
	int FindOneOf(const char* pszCharSet) const;

	// look for a specific sub-string
	int Find(const char* pszSub) const;         // like "C" strstr

	// input and output
#ifdef _DEBUG
	friend CDumpContext& operator<<(CDumpContext&, const CString& string);
#endif
	friend CArchive& operator<<(CArchive& ar, const CString& string);
	friend CArchive& operator>>(CArchive& ar, CString& string);

	// Windows support
#ifdef _WINDOWS
	BOOL LoadString(UINT nID);          // load from string resource
										// 255 chars max
	// ANSI<->OEM support (convert string in place)
	void AnsiToOem();
	void OemToAnsi();
#endif //_WINDOWS

	// Access to string implementation buffer as "C" character array
	char* GetBuffer(int nMinBufLength);
	void ReleaseBuffer(int nNewLength = -1);
	char* GetBufferSetLength(int nNewLength);

// Implementation
protected:
	// lengths/sizes in characters
	//  (note: an extra character is always allocated)
	char* m_pchData;            // actual string (zero terminated)
	int m_nDataLength;          // does not include terminating 0
	int m_nAllocLength;         // does not include terminating 0

	// implementation helpers
	void Init();
	void AllocCopy(CString& dest, int nCopyLen, int nCopyIndex, int nExtraLen) const;
	void AllocBuffer(int nLen);
	void AssignCopy(int nSrcLen, const char* pszSrcData);
	void ConcatCopy(int nSrc1Len, const char* pszSrc1Data, int nSrc2Len, const char* pszSrc2Data);
	void ConcatInPlace(int nSrcLen, const char* pszSrcData);
};


// Compare helpers
BOOL operator==(const CString& s1, const CString& s2);
BOOL operator==(const CString& s1, const char* s2);
BOOL operator==(const char* s1, const CString& s2);
BOOL operator!=(const CString& s1, const CString& s2);
BOOL operator!=(const CString& s1, const char* s2);
BOOL operator!=(const char* s1, const CString& s2);
BOOL operator<(const CString& s1, const CString& s2);
BOOL operator<(const CString& s1, const char* s2);
BOOL operator<(const char* s1, const CString& s2);
BOOL operator>(const CString& s1, const CString& s2);
BOOL operator>(const CString& s1, const char* s2);
BOOL operator>(const char* s1, const CString& s2);
BOOL operator<=(const CString& s1, const CString& s2);
BOOL operator<=(const CString& s1, const char* s2);
BOOL operator<=(const char* s1, const CString& s2);
BOOL operator>=(const CString& s1, const CString& s2);
BOOL operator>=(const CString& s1, const char* s2);
BOOL operator>=(const char* s1, const CString& s2);

/////////////////////////////////////////////////////////////////////////////
// CTimeSpan and CTime

class CTimeSpan
{
public:

// Constructors
	CTimeSpan();
	CTimeSpan(time_t time);
	CTimeSpan(LONG lDays, int nHours, int nMins, int nSecs);

	CTimeSpan(const CTimeSpan& timeSpanSrc);
	const CTimeSpan& operator=(const CTimeSpan& timeSpanSrc);

// Attributes
	// extract parts
	LONG GetDays() const;   // total # of days
	LONG GetTotalHours() const;
	int GetHours() const;
	LONG GetTotalMinutes() const;
	int GetMinutes() const;
	LONG GetTotalSeconds() const;
	int GetSeconds() const;

// Operations
	// time math
	CTimeSpan operator-(CTimeSpan timeSpan) const;
	CTimeSpan operator+(CTimeSpan timeSpan) const;
	const CTimeSpan& operator+=(CTimeSpan timeSpan);
	const CTimeSpan& operator-=(CTimeSpan timeSpan);
	BOOL operator==(CTimeSpan timeSpan) const;
	BOOL operator!=(CTimeSpan timeSpan) const;
	BOOL operator<(CTimeSpan timeSpan) const;
	BOOL operator>(CTimeSpan timeSpan) const;
	BOOL operator<=(CTimeSpan timeSpan) const;
	BOOL operator>=(CTimeSpan timeSpan) const;

#ifndef _WINDLL
	CString Format(const char* pFormat);
#endif //!_WINDLL

	// serialization
#ifdef _DEBUG
	friend CDumpContext& operator<<(CDumpContext& dc, CTimeSpan timeSpan);
#endif
	friend CArchive& operator<<(CArchive& ar, CTimeSpan timeSpan);
	friend CArchive& operator>>(CArchive& ar, CTimeSpan& rtimeSpan);

private:
	time_t m_timeSpan;
	friend class CTime;
};

#ifdef _NTWIN
struct _SYSTEMTIME;
struct _FILETIME;
#endif

class CTime
{
public:

// Constructors
	static  CTime GetCurrentTime();

	CTime();
	CTime(time_t time);
	CTime(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec);
	CTime(WORD wDosDate, WORD wDosTime);
	CTime(const CTime& timeSrc);

#ifdef _NTWIN
	CTime(const _SYSTEMTIME& sysTime);
	CTime(const _FILETIME& fileTime);
#endif

	const CTime& operator=(const CTime& timeSrc);
	const CTime& operator=(time_t t);

// Attributes
	struct tm* GetGmtTm(struct tm* ptm = NULL) const;
	struct tm* GetLocalTm(struct tm* ptm = NULL) const;

	time_t  GetTime() const;
	int GetYear() const;
	int GetMonth() const;       // month of year (1 = Jan)
	int GetDay() const;         // day of month
	int GetHour() const;
	int GetMinute() const;
	int GetSecond() const;
	int GetDayOfWeek() const;   // 1=Sun, 2=Mon, ..., 7=Sat

// Operations
	// time math
	CTimeSpan operator-(CTime time) const;
	CTime operator-(CTimeSpan timeSpan) const;
	CTime operator+(CTimeSpan timeSpan) const;
	const CTime& operator+=(CTimeSpan timeSpan);
	const CTime& operator-=(CTimeSpan timeSpan);
	BOOL operator==(CTime time) const;
	BOOL operator!=(CTime time) const;
	BOOL operator<(CTime time) const;
	BOOL operator>(CTime time) const;
	BOOL operator<=(CTime time) const;
	BOOL operator>=(CTime time) const;

	// formatting using "C" strftime
#ifndef _WINDLL
	CString Format(const char* pFormat);
	CString FormatGmt(const char* pFormat);
#endif //!_WINDLL

	// serialization
#ifdef _DEBUG
	friend CDumpContext& operator<<(CDumpContext& dc, CTime time);
#endif
	friend CArchive& operator<<(CArchive& ar, CTime time);
	friend CArchive& operator>>(CArchive& ar, CTime& rtime);

private:
	time_t  m_time;
};

/////////////////////////////////////////////////////////////////////////////
// File status

struct CFileStatus
{
	CTime m_ctime;          // creation date/time of file
	CTime m_mtime;          // last modification date/time of file
	CTime m_atime;          // last access date/time of file
	LONG m_size;            // logical size of file in bytes
	BYTE m_attribute;       // logical OR of CFile::Attribute enum values
	BYTE _m_padding;        // pad the structure to a WORD
	char m_szFullName[_MAX_PATH]; // absolute path name

#ifdef _DEBUG
	void Dump(CDumpContext& dc) const;
#endif
};


/////////////////////////////////////////////////////////////////////////////
// Diagnostic memory management routines

#ifdef _DEBUG

extern "C" BOOL AfxDiagnosticInit(void);

// Memory tracking allocation
void* operator new(size_t nSize, const char FAR* lpszFileName, int nLine);
#define DEBUG_NEW new(THIS_FILE, __LINE__)

// Low level sanity checks for memory blocks
extern "C" BOOL FAR PASCAL AfxIsValidAddress(const void FAR* lp, 
			UINT nBytes, BOOL bReadWrite = TRUE);
#ifdef _NEARDATA
inline BOOL PASCAL AfxIsValidAddress(const void NEAR* np, 
		UINT nBytes, BOOL bReadWrite = TRUE)
	{ return np != NULL && ::AfxIsValidAddress((const void FAR*)np, 
		nBytes, bReadWrite); }
#endif

// Return TRUE if valid memory block of nBytes
BOOL PASCAL AfxIsMemoryBlock(const void* p, UINT nBytes, LONG* plRequestNumber = NULL);

// Return TRUE if memory is sane or print out what is wrong
BOOL PASCAL AfxCheckMemory();

// Options for tuning the allocation diagnostics
extern "C" 
{
	extern int afxMemDF;    // global variable for easy setting in debugger
}

enum AfxMemDF // memory debug/diagnostic flags
{
	allocMemDF          = 0x01,         // turn on debugging allocator
	delayFreeMemDF      = 0x02,         // delay freeing memory memory
	checkAlwaysMemDF	= 0x04			// AfxCheckMemory on every alloc/free
};

// turn on/off tracking for a short while
BOOL PASCAL AfxEnableMemoryTracking(BOOL bTrack);

// Memory allocator failure simulation and control (_DEBUG only)

// A failure hook returns whether to permit allocation
typedef BOOL (PASCAL * AFX_ALLOC_HOOK)(size_t nSize, BOOL bObject, LONG lRequestNumber);

// Set new hook, return old (never NULL)
AFX_ALLOC_HOOK AfxSetAllocHook(AFX_ALLOC_HOOK pfnAllocHook);

// Debugger hook on specified allocation request
void PASCAL AfxSetAllocStop(LONG lRequestNumber);

// Memory state for snapshots/leak detection
struct CMemoryState
{
// Attributes
	enum blockUsage
	{
		freeBlock,    // not used
		objectBlock,  // contains a CObject derived class object
		bitBlock,     // contains ::operator new data
		nBlockUseMax  // total number of usages
	};

	struct CBlockHeader* m_pBlockHeader;
	LONG m_lCounts[nBlockUseMax];
	LONG m_lSizes[nBlockUseMax];
	LONG m_lHighWaterCount;
	LONG m_lTotalCount;

	CMemoryState();

// Operations
	void Checkpoint();  // fill with current state
	BOOL Difference(const CMemoryState& oldState,
					const CMemoryState& newState);  // fill with difference

	// Output to afxDump
	void DumpStatistics() const;
	void DumpAllObjectsSince() const;
};

// Enumerate allocated objects or runtime classes
void PASCAL AfxDoForAllObjects(void (*pfn)(CObject* pObject, void* pContext), void* pContext);
void PASCAL AfxDoForAllClasses(void (*pfn)(const CRuntimeClass* pClass, void* pContext), void* pContext);

#else

// NonDebug version that assume everything is OK
#define DEBUG_NEW new
#define AfxCheckMemory()                TRUE
#define AfxIsMemoryBlock(pBuf, nBytes)  TRUE

#endif // _DEBUG

/////////////////////////////////////////////////////////////////////////////
// Archives for serializing CObject data

// needed for implementation
class CPtrArray;
class CMapPtrToWord;

class CArchive
{
public:
// Flag values
	enum Mode { store = 0, load = 1 };

	CArchive(CFile* pFile, UINT nMode, int nBufSize = 512, void FAR* lpBuf = NULL);
	~CArchive();

// Attributes
	BOOL IsLoading() const;
	BOOL IsStoring() const;
	CFile* GetFile() const;

// Operations
	UINT Read(void FAR* lpBuf, UINT nMax);
	void Write(const void FAR* lpBuf, UINT nMax);
	void Flush();
	void Close();

protected:
	void FillBuffer(UINT nBytesNeeded);
	CObject* ReadObject(const CRuntimeClass* pClass);
	void WriteObject(const CObject* pOb);

public:
	// Object I/O is pointer based to avoid added construction overhead.
	// Use the Serialize member function directly for embedded objects.
	friend CArchive& operator<<(CArchive& ar, const CObject* pOb);

	friend CArchive& operator>>(CArchive& ar, CObject*& pOb);
	friend CArchive& operator>>(CArchive& ar, const CObject*& pOb);

	// insertion operations
	// NOTE: operators available only for fixed size types for portability
	CArchive& operator<<(BYTE by);
	CArchive& operator<<(WORD w);
	CArchive& operator<<(LONG l);
	CArchive& operator<<(DWORD dw);

	// extraction operations
	// NOTE: operators available only for fixed size types for portability
	CArchive& operator>>(BYTE& by);
	CArchive& operator>>(WORD& w);
	CArchive& operator>>(DWORD& dw);
	CArchive& operator>>(LONG& l);

// Implementation
protected:
	// archive objects cannot be copied or assigned
	CArchive(const CArchive& arSrc);
	void operator=(const CArchive& arSrc);

	BOOL m_nMode;
	BOOL m_bUserBuf;
	int m_nBufSize;
	CFile* m_pFile;
	BYTE FAR* m_lpBufCur;
	BYTE FAR* m_lpBufMax;
	BYTE FAR* m_lpBufStart;

	UINT m_nMapCount;   // count and map used when storing
	union
	{
		CPtrArray* m_pLoadArray;
		CMapPtrToWord* m_pStoreMap;
	};
};


/////////////////////////////////////////////////////////////////////////////
// Diagnostic dumping

class CDumpContext
{
public:
	CDumpContext(CFile* pFile);

// Attributes
	int GetDepth() const;      // 0 => this object, 1 => children objects
	void SetDepth(int nNewDepth);

// Operations
	CDumpContext& operator<<(const char FAR* lpsz);
	CDumpContext& operator<<(const void FAR* lp);
#ifdef _NEARDATA
	CDumpContext& operator<<(const void NEAR* np);
#endif
	CDumpContext& operator<<(const CObject* pOb);
	CDumpContext& operator<<(const CObject& ob);
	CDumpContext& operator<<(BYTE by);
	CDumpContext& operator<<(WORD w);
	CDumpContext& operator<<(UINT u);
	CDumpContext& operator<<(LONG l);
	CDumpContext& operator<<(DWORD dw);
	CDumpContext& operator<<(int n);
	void HexDump(const char* pszLine, BYTE* pby, int nBytes, int nWidth);
	void Flush();

// Implementation
protected:
	// dump context objects cannot be copied or assigned
	CDumpContext(const CDumpContext& dcSrc);
	void operator=(const CDumpContext& dcSrc);
	void OutputString(const char FAR* lpsz);

	int m_nDepth;

public:
	CFile* m_pFile;
};

#ifdef _DEBUG
extern CDumpContext& afxDump;
extern "C" BOOL afxTraceEnabled;
#endif

/////////////////////////////////////////////////////////////////////////////
// Other implementation helpers

#define BEFORE_START_POSITION ((void*)(void NEAR*)-1)
#define _AFX_FP_OFF(thing) (*((UINT*)&(thing)))
#define _AFX_FP_SEG(lp) (*((UINT*)&(lp)+1))


/////////////////////////////////////////////////////////////////////////////
// Swap tuning for AFX library

// Use BASED_CODE so that it may be easily redefined for tuning purposes

//  Data defined using this modifier is placed in the current
//  code segment, which is modified with #pragma code_seg
#ifndef BASED_CODE
#define BASED_CODE __based(__segname("_CODE"))
#endif

#ifndef _NTWIN
#if defined(_M_I86MM) || defined(_M_I86LM) // far code
#define AFX_CORE_SEG  "AFX_CORE_TEXT" /* core functionality */
#define AFX_AUX_SEG   "AFX_AUX_TEXT"  /* auxilliary functionality */
#define AFX_COLL_SEG  "AFX_COLL_TEXT" /* collections */
#define AFX_OLE_SEG   "AFX_OLE_TEXT"  /* OLE support */
#endif
#endif

/////////////////////////////////////////////////////////////////////////////
// Inline function declarations

#include "afx.inl"

#endif // __AFX_H__
