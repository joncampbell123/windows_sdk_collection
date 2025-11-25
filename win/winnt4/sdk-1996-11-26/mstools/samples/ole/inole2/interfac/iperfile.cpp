/*
 * IPERFILE.CPP
 *
 * Template IPersistFile interface implementation.
 *
 * Copyright (c)1993-1996 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#include "iperfile.h"


/*
 * CImpIPersistFile:CImpIPersistFile
 * CImpIPersistFile::~CImpIPersistFile
 *
 * Constructor Parameters:
 *  pObj            LPVOID pointing to the object we live in.
 *  pUnkOuter       LPUNKNOWN of the controlling unknown.
 */

CImpIPersistFile::CImpIPersistFile(LPVOID pObj, LPUNKNOWN pUnkOuter)
    {
    m_cRef=0;
    m_pObj=pObj;
    m_pUnkOuter=pUnkOuter;
    return;
    }


CImpIPersistFile::~CImpIPersistFile(void)
    {
    return;
    }




/*
 * CImpIPersistFile::QueryInterface
 * CImpIPersistFile::AddRef
 * CImpIPersistFile::Release
 *
 * Purpose:
 *  Delegating IUnknown members for CImpIPersistFile.
 */

STDMETHODIMP CImpIPersistFile::QueryInterface(REFIID riid
    , LPVOID *ppv)
    {
    return m_pUnkOuter->QueryInterface(riid, ppv);
    }

STDMETHODIMP_(ULONG) CImpIPersistFile::AddRef(void)
    {
    ++m_cRef;
    return m_pUnkOuter->AddRef();
    }

STDMETHODIMP_(ULONG) CImpIPersistFile::Release(void)
    {
    --m_cRef;
    return m_pUnkOuter->Release();
    }





/*
 * CImpIPersistFile::GetClassID
 *
 * Purpose:
 *  Returns the CLSID of the file represented by this interface.
 *
 * Parameters:
 *  pClsID          LPCLSID in which to store our CLSID.
 *
 * Return Value:
 *  HRESULT         NOERROR or a general error value.
 */

STDMETHODIMP CImpIPersistFile::GetClassID(LPCLSID pClsID)
    {
    return NOERROR;
    }





/*
 * CImpIPersistFile::IsDirty
 *
 * Purpose:
 *  Tells the caller if we have made changes to this object since
 *  it was loaded or initialized new.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  HRESULT         Contains S_OK if we ARE dirty, S_FALSE if
 *                  NOT dirty.
 */

STDMETHODIMP CImpIPersistFile::IsDirty(void)
    {
    return ResultFromScode(S_FALSE);
    }








/*
 * CImpIPersistFile::Load
 *
 * Purpose:
 *  Asks the server to load the document for the given filename.
 *
 * Parameters:
 *  pszFile         LPCSTR of the filename to load.
 *  grfMode         DWORD flags to use when opening the file.
 *
 * Return Value:
 *  HRESULT         NOERROR or a general error value.
 */

STDMETHODIMP CImpIPersistFile::Load(LPCSTR pszFile, DWORD grfMode)
    {
    return ResultFromScode(E_NOTIMPL);
    }





/*
 * CImpIPersistFile::Save
 *
 * Purpose:
 *  Instructs the server to write the current file into a new
 *  filename, possibly then using that filename as the current one.
 *
 * Parameters:
 *  pszFile         LPCSTR of the file into which we save.  If NULL,
 *                  this means save the current file.
 *  fRemember       BOOL indicating if we're to use this filename as
 *                  the current file now (Save As instead of Save
 *                  Copy As).
 *
 * Return Value:
 *  HRESULT         NOERROR or a general error value.
 */

STDMETHODIMP CImpIPersistFile::Save(LPCSTR pszFile, BOOL fRemember)
    {
    return ResultFromScode(E_NOTIMPL);
    }








/*
 * CImpIPersistFile::SaveCompleted
 *
 * Purpose:
 *  Informs us that the operation that called Save is now finished
 *  and we can access the file again.
 *
 * Parameters:
 *  pszFile         LPCSTR of the file in which we can start
 *                  writing again.
 *
 * Return Value:
 *  HRESULT         NOERROR or a general error value.
 */

STDMETHODIMP CImpIPersistFile::SaveCompleted(LPCSTR pszFile)
    {
    return NOERROR;
    }





/*
 * CImpIPersistFile::GetCurFile
 *
 * Purpose:
 *  Returns the current filename.
 *
 * Parameters:
 *  ppszFile        LPTSTR * into which we store a pointer to
 *                  the filename that should be allocated with the
 *                  shared IMalloc.
 *
 * Return Value:
 *  HRESULT         NOERROR or a general error value.
 */

STDMETHODIMP CImpIPersistFile::GetCurFile(LPTSTR *ppszFile)
    {
    return ResultFromScode(E_NOTIMPL);
    }
