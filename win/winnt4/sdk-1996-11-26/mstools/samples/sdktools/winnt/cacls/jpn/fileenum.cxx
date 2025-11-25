//+------------------------------------------------------------------
//
// Copyright (C) 1995, Microsoft Corporation.
//
// File:        FileEnum.cxx
//
// Contents:    class encapsulating file enumeration, including a deep option
//
// Classes:     CFileEnumeration
//
// History:     Nov-93      DaveMont         Created.
//
//-------------------------------------------------------------------

#include <t2.hxx>
#include <FileEnum.hxx>
#if DBG
extern ULONG Debug;
#endif
//+---------------------------------------------------------------------------
//
//  Member:     CFileEnumerate::CFileEnumerate, public
//
//  Synopsis:   initializes data members, constructor will not throw
//
//  Arguments:  IN [fdeep] - TRUE = go into sub-directories
//
//----------------------------------------------------------------------------
CFileEnumerate::CFileEnumerate(BOOL fdeep)
    : _fdeep(fdeep),
      _findeep(FALSE),
      _froot(FALSE),
      _fcannotaccess(FALSE),
      _pcfe(NULL),
      _pwfileposition(NULL),
      _handle(INVALID_HANDLE_VALUE)
{
    ENUMERATE_RETURNS((stderr, "CFileEnumerate ctor\n"))
}
//+---------------------------------------------------------------------------
//
//  Member:     Dtor, public
//
//  Synopsis:   closes handles
//
//  Arguments:  none
//
//----------------------------------------------------------------------------
CFileEnumerate::~CFileEnumerate()
{
    if (_handle != INVALID_HANDLE_VALUE)
        FindClose(_handle);
#ifdef KKBUGFIX
    ENUMERATE_RETURNS((stderr, "CFileEnumerate dtor (%S)\n", _wpath))
#else
    ENUMERATE_RETURNS((stderr, "CFileEnumerate dtor (%ws)\n", _wpath))
#endif
}
//+---------------------------------------------------------------------------
//
//  Member:     CFileEnumerate::Init, public
//
//  Synopsis:   Init must be called before any other methods - this
//              is not enforced. converts a ASCII file/path to a UNICODE
//              file/path, and gets the first file in the enumeration
//
//  Arguments:  IN  [filename]  - the path/file to enumerate
//              OUT [wfilename] - first file in the enumeration
//              OUT [fdir]      - TRUE = returned file is a directory
//
//----------------------------------------------------------------------------
ULONG CFileEnumerate::Init(CHAR *filename, WCHAR **wfilename, BOOL *fdir)
{
    // Initialize the file name

    if (filename && (strlen(filename) < MAX_PATH))
    {
        // make it wchar
        WCHAR winfilename[MAX_PATH];

#ifdef DBCS
        if (MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED,
                                filename, -1,
                                winfilename, sizeof(winfilename)) == 0)
            return(ERROR_INVALID_NAME);
#else
        for (_pwfileposition = winfilename; *_pwfileposition = (WCHAR)(*filename);_pwfileposition++,filename++);
#endif

        // finish initialization

        return(_ialize(winfilename, wfilename, fdir));
    }
    ENUMERATE_FAIL((stderr, "Init bad file name: %ld\n",ERROR_INVALID_NAME))
    return(ERROR_INVALID_NAME);
}
//+---------------------------------------------------------------------------
//
//  Member:     CFileEnumerate::Init, public
//
//  Synopsis:   Same as previous, except takes UNICODE file/path as input
//
//  Arguments:  IN  [filename]  - the path/file to enumerate
//              OUT [wfilename] - first file in the enumeration
//              OUT [fdir]      - TRUE = returned file is a directory
//
//----------------------------------------------------------------------------
ULONG CFileEnumerate::Init(WCHAR *filename, WCHAR **wfilename, BOOL *fdir)
{
    // Initialize the file name

    if (filename && (wcslen(filename) < MAX_PATH))
    {
        return(_ialize(filename, wfilename, fdir));
    }
    ENUMERATE_FAIL((stderr, "Init bad file name: %ld\n",ERROR_INVALID_NAME))
    return(ERROR_INVALID_NAME);
}
//+---------------------------------------------------------------------------
//
//  Member:     CFileEnumerate::_ialize, private
//
//  Synopsis:   finishes initialization and starts search for first file in
//              the enumeration
//
//  Arguments:  OUT [wfilename] - first file in the enumeration
//              OUT [fdir]      - TRUE = returned file is a directory
//
//----------------------------------------------------------------------------
ULONG CFileEnumerate::_ialize(WCHAR *winfilename, WCHAR **wfilename, BOOL *fdir)
{
#ifdef KKBUGFIX
    ENUMERATE_RETURNS((stderr, "Init start, path =  %S\n", winfilename))
#else
    ENUMERATE_RETURNS((stderr, "Init start, path =  %ws\n", winfilename))
#endif
    ULONG ret = ERROR_SUCCESS;

#ifdef KKBUGFIX
    ENUMERATE_STAT((stderr, "start path = %S\n",winfilename))
#else
    ENUMERATE_STAT((stderr, "start path = %ws\n",winfilename))
#endif

    // save the location of the filename or wildcards

    ULONG cwcharcount;

    if (!(cwcharcount = GetFullPathName(winfilename,
                                       MAX_PATH,
                                       _wpath,
                                       &_pwfileposition)))
    {
        return(ERROR_INVALID_NAME);
    }

#ifdef KKBUGFIX
    ENUMERATE_STAT((stderr, "got full path name = %S, filename = (%S), total chars = %d\n",_wpath, _pwfileposition, cwcharcount))
#else
    ENUMERATE_STAT((stderr, "got full path name = %ws, filename = (%ws), total chars = %d\n",_wpath, _pwfileposition, cwcharcount))
#endif

    // if the filepart (_pwfileposition) is NULL, then the name must end in a slash.
    // add a *

    if (NULL == _pwfileposition)
    {
       _pwfileposition = (WCHAR *)Add2Ptr(_wpath,wcslen(_wpath)*sizeof(WCHAR));
    }

    // save the filename/wildcards

    wcscpy(_wwildcards, _pwfileposition);

#ifdef KKBUGFIX
    ENUMERATE_EXTRA((stderr, "wild cards = %S\n",_wwildcards))
#else
    ENUMERATE_EXTRA((stderr, "wild cards = %ws\n",_wwildcards))
#endif

    // if we are at a root (path ends in :\)

    if ( (_wpath[wcslen(_wpath) - 1] == L'\\') &&
         (wcslen(_wpath) > 1) &&
         (_wpath[wcslen(_wpath) - 2] == L':') )
    {
        _wfd.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
        _wfd.cFileName[0] = L'\0';
        *wfilename = _wpath;
        *fdir = TRUE;
        _froot = TRUE;
    } else
    {
    // check to see if we can iterate through files
        if ( (INVALID_HANDLE_VALUE == ( _handle = FindFirstFile(_wpath, &_wfd) ) ) )
        {
            ret = GetLastError();
            _fcannotaccess = (ERROR_ACCESS_DENIED == ret);
            
            ENUMERATE_FAIL((stderr, "find first returned: %ld\n",ret))
        }
        if (ERROR_SUCCESS == ret)
        {
            // reject . & .. filenames (go on to next file )
        
            if ( (0 == wcscmp(_wfd.cFileName, L".")) ||
                 (0 == wcscmp(_wfd.cFileName, L"..")) )
            {
                ret = _NextLocal(wfilename,fdir);
            } else
            {
                // return the current directory
        
                if (_wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                    *fdir = TRUE;
                else
                    *fdir = FALSE;
        
                // add the filename to the path so the whole thing is returned
        
                wcscpy(_pwfileposition, _wfd.cFileName);
        
                *wfilename = _wpath;
            }
        }

#ifdef KKBUGFIX
        ENUMERATE_STAT((stderr, "next filename = %S\n", *wfilename))
#else
        ENUMERATE_STAT((stderr, "next filename = %ws\n", *wfilename))
#endif
    }

    // if we are going deep and we did not find a file yet:

    if ( _fdeep && ( ( ERROR_NO_MORE_FILES == ret ) ||
                     ( ERROR_FILE_NOT_FOUND == ret ) ) )
    {
        if (_handle != INVALID_HANDLE_VALUE)
        {
            FindClose(_handle);
            _handle = INVALID_HANDLE_VALUE;
        }
        ret = _InitDir(wfilename, fdir);
    }

#ifdef KKBUGFIX
    ENUMERATE_RETURNS((stderr, "Init returning  =  %S(%ld)\n\n", *wfilename, ret))
#else
    ENUMERATE_RETURNS((stderr, "Init returning  =  %ws(%ld)\n\n", *wfilename, ret))
#endif
    return(ret);
}
//+---------------------------------------------------------------------------
//
//  Member:     CFileEnumerate::Next, public
//
//  Synopsis:   finds the next file in the enumeration
//
//  Arguments:  OUT [wfilename] - first file in the enumeration
//              OUT [fdir]      - TRUE = returned file is a directory
//
//----------------------------------------------------------------------------
ULONG CFileEnumerate::Next(WCHAR **wfilename, BOOL *fdir)
{
#ifdef KKBUGFIX
    ENUMERATE_RETURNS((stderr, "Next start, path =  %S\n", _wpath))
#else
    ENUMERATE_RETURNS((stderr, "Next start, path =  %ws\n", _wpath))
#endif
    ULONG ret = ERROR_NO_MORE_FILES;

    // if we failed to initialize with an ERROR_ACCESS_DENIED, then exit
    if (_fcannotaccess)
        return(ERROR_NO_MORE_FILES);

    // if we are not in deep

    if (!_findeep)
    {
        if (!_froot)
           ret = _NextLocal(wfilename, fdir);

        // if we ran out of files and we are going deep:

        if ( _fdeep &&
             ( ( ERROR_NO_MORE_FILES == ret ) ||
               ( ERROR_FILE_NOT_FOUND == ret ) || _froot ) )
        {
            if (_handle != INVALID_HANDLE_VALUE)
            {
                FindClose(_handle);
                _handle = INVALID_HANDLE_VALUE;
            }
            ret = _InitDir(wfilename, fdir);
            _froot = FALSE; // (we are past the root now)
        }

    } else
    {
        // if we are already down a directory (and in deep)

        if (_pcfe)
        {
            if (ERROR_SUCCESS != (ret = _pcfe->Next(wfilename, fdir)))
            {
                if (ERROR_ACCESS_DENIED != ret)
                {
                    delete _pcfe;
                    _pcfe = NULL;
                }
            }
        }

        // we need to go to the next directory in the current dir

        if (ERROR_NO_MORE_FILES == ret)
        {
            ret = _NextDir(wfilename, fdir);
        }
    }
#ifdef KKBUGFIX
    ENUMERATE_RETURNS((stderr, "Next returning  =  %S(%ld)\n\n", *wfilename, ret))
#else
    ENUMERATE_RETURNS((stderr, "Next returning  =  %ws(%ld)\n\n", *wfilename, ret))
#endif
    return(ret);
}
//+---------------------------------------------------------------------------
//
//  Member:     CFileEnumerate::_NextLocal, private
//
//  Synopsis:   searchs for the next file in the current directory
//
//  Arguments:  OUT [wfilename] - first file in the enumeration
//              OUT [fdir]      - TRUE = returned file is a directory
//
//----------------------------------------------------------------------------
ULONG CFileEnumerate::_NextLocal(WCHAR **wfilename, BOOL *fdir)
{
#ifdef KKBUGFIX
    ENUMERATE_RETURNS((stderr, "_NextLocal start, path =  %S\n", _wpath))
#else
    ENUMERATE_RETURNS((stderr, "_NextLocal start, path =  %ws\n", _wpath))
#endif
    ULONG ret = ERROR_SUCCESS;

    // ensure that we have a valid handle for a findnextfile

    if (INVALID_HANDLE_VALUE == _handle)
    {
        ret = ERROR_INVALID_HANDLE;
    } else
    {
        do
        {
            if (!FindNextFile(_handle, &_wfd))
            {
                ret = GetLastError();
                ENUMERATE_FAIL((stderr, "find next returned: %ld\n",ret))
            } else
                ret = ERROR_SUCCESS;
        }
        while ( (ERROR_SUCCESS == ret) &&
                ( (0 == wcscmp(_wfd.cFileName, L".")) ||
                  (0 == wcscmp(_wfd.cFileName, L"..")) ) );


        // if we found a file

        if (ERROR_SUCCESS == ret)
        {
            // return the directory attrib.

            if (_wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                *fdir = TRUE;
            else
                *fdir = FALSE;

            // add the filename to the path so the whole thing is returned

            wcscpy(_pwfileposition, _wfd.cFileName);

            *wfilename = _wpath;

#ifdef KKBUGFIX
            ENUMERATE_STAT((stderr, "next filename = %S\n", *wfilename))
#else
            ENUMERATE_STAT((stderr, "next filename = %ws\n", *wfilename))
#endif
        }
    }
#ifdef KKBUGFIX
    ENUMERATE_RETURNS((stderr, "_NextLocal returning  =  %S(%ld)\n", *wfilename, ret))
#else
    ENUMERATE_RETURNS((stderr, "_NextLocal returning  =  %ws(%ld)\n", *wfilename, ret))
#endif

    return(ret);
}
//+---------------------------------------------------------------------------
//
//  Member:     CFileEnumerate::_InitDir, private
//
//  Synopsis:   (only called if going deep)
//              goes down a directory (and thus causing a new CFileEnumerator
//              to be created, or re-initializies
//
//  Arguments:  OUT [wfilename] - first file in the enumeration
//              OUT [fdir]      - TRUE = returned file is a directory
//
//----------------------------------------------------------------------------
ULONG CFileEnumerate::_InitDir(WCHAR **wfilename, BOOL *fdir)
{
#ifdef KKBUGFIX
    ENUMERATE_RETURNS((stderr, "_InitDir start, path =  %S\n", _wpath))
#else
    ENUMERATE_RETURNS((stderr, "_InitDir start, path =  %ws\n", _wpath))
#endif
    ULONG ret = ERROR_SUCCESS;

    // check and see if a directory was entered as the filename

    if ( (0 == wcsicmp(_wwildcards, _wfd.cFileName)) &&
         (_wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
    {
#ifdef KKBUGFIX
        ENUMERATE_EXTRA((stderr, "first file matched directory = %S\n", _wpath))
#else
        ENUMERATE_EXTRA((stderr, "first file matched directory = %ws\n", _wpath))
#endif
        _pwfileposition += wcslen(_wfd.cFileName);
        wcscpy(_pwfileposition, L"\\*.*");
        _pwfileposition++;
        wcscpy(_wwildcards, L"*.*");
#ifdef KKBUGFIX
        ENUMERATE_EXTRA((stderr, "      path = %S\n",_wpath))
        ENUMERATE_EXTRA((stderr, "wild cards = %S\n",_wwildcards))
#else
        ENUMERATE_EXTRA((stderr, "      path = %ws\n",_wpath))
        ENUMERATE_EXTRA((stderr, "wild cards = %ws\n",_wwildcards))
#endif

        WCHAR winfilename[MAX_PATH];
        wcscpy(winfilename, _wpath);

        ret = _ialize(winfilename, wfilename, fdir);
    } else
    {

        // we are in deep

        _findeep = TRUE;

        // search thru all directories

        wcscpy(_pwfileposition, L"*.*");

        if (INVALID_HANDLE_VALUE == ( _handle = FindFirstFile(_wpath, &_wfd) ))
        {
            ret = GetLastError();
            ENUMERATE_FAIL((stderr, "find first (dir) returned: %ld\n",ret))
        } else
        {
            if ( !(_wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ||
                 (0 == wcscmp(_wfd.cFileName, L".")) ||
                 (0 == wcscmp(_wfd.cFileName, L"..")) )
            {
                ret = _NextDir(wfilename, fdir);
            } else
            {
                // if we have a sub directory, go down it

                ret = _DownDir(wfilename, fdir);

                // if we found nothing in that first sub directory, go the the next one

                if ( (ERROR_NO_MORE_FILES == ret ) ||
                     (ERROR_FILE_NOT_FOUND == ret ) )
                {
                    ret = _NextDir(wfilename, fdir);
                }
            }
        }
    }
#ifdef KKBUGFIX
    ENUMERATE_RETURNS((stderr, "_InitDir returning  =  %S(%ld)\n", *wfilename, ret))
#else
    ENUMERATE_RETURNS((stderr, "_InitDir returning  =  %ws(%ld)\n", *wfilename, ret))
#endif

    return(ret);
}
//+---------------------------------------------------------------------------
//
//  Member:     CFileEnumerate::_NextDir, private
//
//  Synopsis:   (only called if going deep)
//              finds the next sub-directory from the current directory,
//              and then goes down into that directory
//
//  Arguments:  OUT [wfilename] - first file in the enumeration
//              OUT [fdir]      - TRUE = returned file is a directory
//
//----------------------------------------------------------------------------
ULONG CFileEnumerate::_NextDir(WCHAR **wfilename, BOOL *fdir)
{
#ifdef KKBUGFIX
    ENUMERATE_RETURNS((stderr, "_NextDir start, path =  %S\n", _wpath))
#else
    ENUMERATE_RETURNS((stderr, "_NextDir start, path =  %ws\n", _wpath))
#endif
    ULONG ret = ERROR_SUCCESS;

    // skip the . & .. & files we cannot access

    if (INVALID_HANDLE_VALUE == _handle)
    {
        ret = ERROR_INVALID_HANDLE;
    } else
    {
        do
        {
            do
            {
                if (!FindNextFile(_handle, &_wfd))
                {
                    ret = GetLastError();
                    ENUMERATE_FAIL((stderr, "find next returned: %ld\n",ret))
                } else
                    ret = ERROR_SUCCESS;
            }
            while ( (ERROR_SUCCESS == ret) &&
                    ( !(_wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ||
                      (0 == wcscmp(_wfd.cFileName, L".")) ||
                      (0 == wcscmp(_wfd.cFileName, L"..")) ) );

            // if we found a directory

            if (ERROR_SUCCESS == ret)
            {
                ret = _DownDir(wfilename, fdir);
            } else
            {
                // out of subdirectories to search, break out of the loop
                break;
            }
        }
        while (( ERROR_NO_MORE_FILES == ret) || (ERROR_FILE_NOT_FOUND == ret));
    }
#ifdef KKBUGFIX
    ENUMERATE_RETURNS((stderr, "_NextDir returning  =  %S(%ld)\n", *wfilename, ret))
#else
    ENUMERATE_RETURNS((stderr, "_NextDir returning  =  %ws(%ld)\n", *wfilename, ret))
#endif

    return(ret);
}
//+---------------------------------------------------------------------------
//
//  Member:     CFileEnumerate::_DownDir, private
//
//  Synopsis:   (only called if going deep)
//              creates a new CFileEnumerator for a sub-directory
//
//  Arguments:  OUT [wfilename] - first file in the enumeration
//              OUT [fdir]      - TRUE = returned file is a directory
//
//----------------------------------------------------------------------------
ULONG CFileEnumerate::_DownDir(WCHAR **wfilename, BOOL *fdir)
{
#ifdef KKBUGFIX
    ENUMERATE_RETURNS((stderr, "_DownDir start, path =  %S\n", _wpath))
#else
    ENUMERATE_RETURNS((stderr, "_DownDir start, path =  %ws\n", _wpath))
#endif
    ULONG ret;

    // make a new file enumerator class (this one) We should only go down
    // 8 directories at most.

    _pcfe = new CFileEnumerate(_fdeep);

    // add the wildcards to the end of the directory we are going down

    wcscpy(_pwfileposition, _wfd.cFileName);
    wcscat(_pwfileposition, L"\\");
    wcscat(_pwfileposition, _wwildcards);

    // start it up and see if we find a match

    if (ERROR_SUCCESS != (ret = _pcfe->Init(_wpath, wfilename, fdir)))
    {
        if (ERROR_ACCESS_DENIED != ret)
        {
            delete _pcfe;
            _pcfe = NULL;
        }
    }
#ifdef KKBUGFIX
    ENUMERATE_RETURNS((stderr, "_DownDir returning  =  %S(%ld)\n", *wfilename, ret))
#else
    ENUMERATE_RETURNS((stderr, "_DownDir returning  =  %ws(%ld)\n", *wfilename, ret))
#endif
    return(ret);
}

