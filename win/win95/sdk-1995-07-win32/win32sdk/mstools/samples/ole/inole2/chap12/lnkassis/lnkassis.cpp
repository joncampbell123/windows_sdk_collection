/*
 * LNKASSIS.CPP
 * Links Assistant Chapter 12
 *
 * Implementation of the CLinks object with the IOleUILinkContainer
 * interface to assist handling the Links dialog for linking
 * containers.
 *
 * Copyright (c)1993-1995 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#include "lnkassis.h"

#define W2A(w, a, cb)     WideCharToMultiByte(                              \
                                               CP_ACP,                      \
                                               0,                           \
                                               w,                           \
                                               -1,                          \
                                               a,                           \
                                               cb,                          \
                                               NULL,                        \
                                               NULL)

#define A2W(a, w, cb)     MultiByteToWideChar(                              \
                                               CP_ACP,                      \
                                               0,                           \
                                               a,                           \
                                               -1,                          \
                                               w,                           \
                                               cb)

/*
 * CLinks::CLinks
 * CLinks::~CLinks
 *
 * Parameters (Constructor):
 *  pfnDestroy      PFNDESTROYED to call when object is destroyed.
 */

CLinks::CLinks(PFNDESTROYED pfnDestroy)
    {
    m_cRef=0;
    m_pfnDestroy=pfnDestroy;
    return;
    }


CLinks::~CLinks(void)
    {
    return;
    }




/*
 * CLinks::QueryInterface
 * CLinks::AddRef
 * CLinks::Release
 *
 * Purpose:
 *  IUnknown members for CLinks object.
 */

STDMETHODIMP CLinks::QueryInterface(REFIID riid, PPVOID ppv)
    {
    *ppv=NULL;

    if (IID_IUnknown==riid || IID_IOleUILinkContainer==riid)
        {
        *ppv=this;
        AddRef();
        return NOERROR;
        }

    return ResultFromScode(E_NOINTERFACE);
    }


STDMETHODIMP_(ULONG) CLinks::AddRef(void)
    {
    return ++m_cRef;
    }


STDMETHODIMP_(ULONG) CLinks::Release(void)
    {
    ULONG       cRefT;

    cRefT=--m_cRef;

    if (0L==m_cRef)
        {
        if (NULL!=m_pfnDestroy)
            (*m_pfnDestroy)();

        delete this;
        }

    return cRefT;
    }





/*
 * CLinks::GetNextLink
 *
 * Purpose:
 *  Function to fill out the IOleUILinkContainer interface.
 *  Does nothing.
 *
 * Parameters:
 *  dwLink          DWORD ignored.
 *
 * Return Value:
 *  DWORD           Alwaus 0L
 *
 */

STDMETHODIMP_(DWORD) CLinks::GetNextLink(DWORD dwLink)
    {
    return 0L;
    }





/*
 * CLinks::SetLinkUpdateOptions
 *
 * Purpose:
 *  Calls IOleLink::SetUpdateOptions for the object identified by
 *  dwLink.
 *
 * Parameters:
 *  dwLink          DWORD, an IOleLink pointer to the object
 *                  affected.
 *  dwOptions       DWORD containing the new options.
 *
 * Return Value:
 *  HRESULT         Return value of IOleLink::SetUpdateOptions.
 */

STDMETHODIMP CLinks::SetLinkUpdateOptions(DWORD dwLink
    , DWORD dwOptions)
    {
    LPOLELINK       pIOleLink=(LPOLELINK)dwLink;

    if (NULL==pIOleLink)
        return ResultFromScode(E_FAIL);

    return pIOleLink->SetUpdateOptions(dwOptions);
    }





/*
 * CLinks::GetLinkUpdateOptions
 *
 * Purpose:
 *  Call IOleLink::GetUpdateOptions for the object identified by
 *  dwLink.
 *
 * Parameters:
 *  dwLink          DWORD, an IOleLink pointer to the object
 *                  affected.
 *  pdwOptions      LPDWORD in which to store the options.
 *
 * Return Value:
 *  HRESULT         Return value of IOleLink::GetUpdateOptions
 */

STDMETHODIMP CLinks::GetLinkUpdateOptions(DWORD dwLink
    , LPDWORD pdwOptions)
    {
    LPOLELINK       pIOleLink=(LPOLELINK)dwLink;

    if (NULL==pIOleLink)
        return ResultFromScode(E_FAIL);

    return pIOleLink->GetUpdateOptions(pdwOptions);
    }





/*
 * CLinks::SetLinkSource
 *
 * Purpose:
 *  Changes the moniker to which an object is linked.
 *
 * Parameters:
 *  dwLink          DWORD, an IOleLink pointer to the object
 *                  affected.
 *  pszName         LPTSTR to the displayable name of the source.
 *  cchName         ULONG length of the file portaion of pszName
 *  pchEaten        ULONG * in which to return the number of
 *                  characters used in parsing pszDisplayName.
 *  fValidate       BOOL indicating if we're to validate that the
 *                  source exists first.
 *
 * Return Value:
 *  HRESULT         If successful, NOERROR indicates that the link
 *                  is available, S_FALSE to indicate it's not.
 *                  This information is later required in
 *                  GetLinkSource.  E_FAIL on failure.
 */

STDMETHODIMP CLinks::SetLinkSource(DWORD dwLink, LPTSTR pszName
    , ULONG cchName, ULONG *pchEaten, BOOL fValidate)
    {
    LPOLELINK       pIOleLink=(LPOLELINK)dwLink;
    HRESULT         hr;
    CLSID           clsID=CLSID_NULL;
    LPMONIKER       pmk=NULL;
    BOOL            fAvail=FALSE;

#if defined (WIN32) && !defined (UNICODE)
    OLECHAR         szwTemp[MAX_PATH];

    A2W(pszName, szwTemp, MAX_PATH);
    pszName = (LPTSTR) szwTemp;
#endif

    if (fValidate)
        {
        //Check things out and get a moniker and CLSID.
        if (!FValidateLinkSource((LPOLESTR)pszName, pchEaten, &pmk, &clsID))
            return ResultFromScode(E_FAIL);

        //If we got a CLSID, then we found the source.
        if (CLSID_NULL!=clsID)
            fAvail=TRUE;
        }
    else
        {
        if (!FCreateNewSourceMoniker((LPOLESTR)pszName, cchName, &pmk))
            return ResultFromScode(E_FAIL);
        }

    if (NULL==pIOleLink)
        {
        pmk->Release();
        return ResultFromScode(E_FAIL);
        }

    if (NULL!=pmk)
        {
        hr=pIOleLink->SetSourceMoniker(pmk, clsID);
        pmk->Release();
        }
    else
    {
        hr=pIOleLink->SetSourceDisplayName((LPOLESTR)pszName);
    }

    if (FAILED(hr))
        return hr;

    return fAvail ? NOERROR : ResultFromScode(S_FALSE);
    }


/* GetFirstMoniker
** ---------------
**    return the first piece of a moniker.
**
**    NOTE: if the given moniker is not a generic composite moniker,
**    then an AddRef'ed pointer to the given moniker is returned.
**
**    Copied from OleStd code.
*/
STDAPI_(LPMONIKER) GetFirstMoniker(LPMONIKER lpmk)
{
   LPMONIKER       lpmkFirst = NULL;
   LPENUMMONIKER   lpenumMoniker;
   DWORD           dwMksys;
   HRESULT         hrErr;

   if (! lpmk)
      return NULL;

   if (lpmk->IsSystemMoniker((LPDWORD)&dwMksys) == NOERROR
      && dwMksys == MKSYS_GENERICCOMPOSITE) {

      /* OLE2NOTE: the moniker is a GenericCompositeMoniker.
      **    enumerate the moniker to pull off the first piece.
      */

      hrErr = lpmk->Enum(
            TRUE /* fForward */,
            (LPENUMMONIKER FAR*)&lpenumMoniker
      );
      if (hrErr != NOERROR)
         return NULL;    // ERROR: give up!

      hrErr = lpenumMoniker->Next(
            1,
            (LPMONIKER FAR*)&lpmkFirst,
            NULL
      );
      lpenumMoniker->Release();
      return lpmkFirst;

   } else {
      /* OLE2NOTE: the moniker is NOT a GenericCompositeMoniker.
      **    return an AddRef'ed pointer to the input moniker.
      */
      lpmk->AddRef();
      return lpmk;
   }
}


/* GetLenFilePrefixOfMoniker
** -------------------------
**    if the first piece of the Moniker is a FileMoniker, then return
**    the length of the filename string.
**
**    lpmk      pointer to moniker
**
**    Returns
**      0       if moniker does NOT start with a FileMoniker
**      uLen    string length of filename prefix of the display name
**              retrieved from the given (lpmk) moniker.
**
**  Copied from OleStd code.
*/
ULONG GetLenFilePrefixOfMoniker(LPMONIKER lpmk)
{
   LPMONIKER       lpmkFirst = NULL;
   DWORD           dwMksys;
   LPOLESTR        lpsz = NULL;
   LPBC            lpbc = NULL;
   ULONG           uLen = 0;
   HRESULT         hrErr;

   if (! lpmk)
      return 0;

   lpmkFirst = GetFirstMoniker(lpmk);
   if (lpmkFirst) {
      if ( (lpmkFirst->IsSystemMoniker(
                     (LPDWORD)&dwMksys) == NOERROR)
            && dwMksys == MKSYS_FILEMONIKER) {

         hrErr = CreateBindCtx(0, (LPBC FAR*)&lpbc);
         if (hrErr == NOERROR) {
            hrErr = lpmkFirst->GetDisplayName(
                  lpbc,
                  NULL,   /* pmkToLeft */
                  /*(LPOLESTR FAR*)*/&lpsz
            );
            if (hrErr == NOERROR && lpsz != NULL) {
               uLen = wcslen(lpsz);
               IMalloc * pMalloc;
               if (SUCCEEDED(CoGetMalloc(MEMCTX_TASK, &pMalloc)))
               {
                   pMalloc->Free(lpsz);
                   pMalloc->Release();
               }
            }
            lpbc->Release();
         }
      }
      lpmkFirst->Release();
   }
   return uLen;
}


/*
 * CLinks::GetLinkSource
 *
 * Purpose:
 *  Retrieves various strings and values for this link source.
 *
 * Parameters:
 *  dwLink          DWORD, an IOleLink pointer to the object
 *                  affected.
 *  ppszName        LPTSTR * in which to return the new source
 *                  name
 *  pcchName        ULONG * in which to return the length of
 *                  pszName
 *  ppszFullLink    LPTSTR * in which to return the full name of
 *                  the class of linked object.
 *  ppszShortLink   LPTSTR * in which to return the short name of
 *                  the class of linked object.
 *  pfSourceAvail   BOOL * ignored.
 *  pfSelected      BOOL * ignored.
 *
 * Return Value:
 *  HRESULT         NOERROR on success, error code otherwise.
 */

STDMETHODIMP CLinks::GetLinkSource(DWORD dwLink
    , LPTSTR *ppszName, ULONG *pcchName
    , LPTSTR *ppszFullLink, LPTSTR *ppszShortLink
    , BOOL *pfSourceAvail, BOOL *pfSelected)
    {
    LPOLELINK       pIOleLink=(LPOLELINK)dwLink;
    HRESULT         hr;
    LPOLEOBJECT     pIOleObject=NULL;
    LPMONIKER       pmk=NULL;
    LPMONIKER       pmkFirst=NULL;
    LPBC            pbc=NULL;

    if (NULL==pIOleLink)
        return ResultFromScode(E_FAIL);

    *ppszName=NULL;
    *pcchName=0;
    *ppszFullLink=NULL;
    *ppszShortLink=NULL;

    hr=pIOleLink->GetSourceMoniker(&pmk);

    if (SUCCEEDED(hr))
        {
        hr=pIOleLink->QueryInterface(IID_IOleObject
            , (PPVOID)&pIOleObject);

        if (SUCCEEDED(hr))
            {
            pIOleObject->GetUserType(USERCLASSTYPE_FULL
                , (LPOLESTR *) ppszFullLink);
            pIOleObject->GetUserType(USERCLASSTYPE_SHORT
                , (LPOLESTR *) ppszShortLink);
            pIOleObject->Release();
            }

        *pcchName=GetLenFilePrefixOfMoniker(pmk);
        pmk->Release();
        }
    hr = pIOleLink->GetSourceDisplayName((LPOLESTR *) ppszName);

#if defined (WIN32) && !defined (UNICODE)
    LPOLESTR        lpszwFullLink = (LPOLESTR)*ppszFullLink;
    LPOLESTR        lpszwShortLink = (LPOLESTR)*ppszShortLink;
    LPOLESTR        lpszwName =  (LPOLESTR)*ppszName;
    LPMALLOC        pMalloc;
    if (SUCCEEDED(CoGetMalloc(MEMCTX_TASK, &pMalloc)))
    {
        if (lpszwFullLink)
        {
            unsigned cch = wcslen(lpszwFullLink) + 1;
            *ppszFullLink = (LPTSTR)pMalloc->Alloc(cch);
            if (*ppszFullLink)
            {
                W2A(lpszwFullLink, *ppszFullLink, cch);
                pMalloc->Free(lpszwFullLink);
            }
        }
        if (lpszwShortLink)
        {
            unsigned cch = wcslen(lpszwShortLink) + 1;
            *ppszShortLink = (LPTSTR)pMalloc->Alloc(cch);
            if (*ppszShortLink)
            {
                W2A(lpszwShortLink, *ppszShortLink, cch);
                pMalloc->Free(lpszwShortLink);
            }
        }
        if (lpszwName)
        {
            unsigned cch = wcslen(lpszwName) + 1;
            *ppszName = (LPTSTR)pMalloc->Alloc(cch);
            if (*ppszName)
            {
                W2A(lpszwName, *ppszName, cch);
                pMalloc->Free(lpszwName);
            }
        }
        pMalloc->Release();
    }
#endif

    return hr;
    }





/*
 * CLinks::OpenLinkSource
 *
 * Purpose:
 *  Does nothing.  The container using this object is the only
 *  one that knows how to activate an object properly.
 *
 * Parameters:
 *  dwLink          DWORD ignored.
 *
 * Return Value:
 *  HRESULT         NOERROR
 */

STDMETHODIMP CLinks::OpenLinkSource(DWORD dwLink)
    {
    return NOERROR;
    }






/*
 * CLinks::UpdateLink
 *
 * Purpose:
 *  Updates the link for this object.
 *
 * Parameters:
 *  dwLink          DWORD, an IOleLink pointer to the object
 *                  affected.
 *  fErrorMessage   BOOL indicating if we can show errors.
 *  fErrorAction    BOOL making no sense whatsoever.
 *
 * Return Value:
 *  HRESULT         NOERROR if successful, error code otherwise. If
 *                  there is an error, the caller should set the
 *                  link availability flag to FALSE.  Otherwise set
 *                  to TRUE.
 */

STDMETHODIMP CLinks::UpdateLink(DWORD dwLink
    , BOOL fErrorMessage, BOOL fErrorAction)
    {
    LPOLELINK       pIOleLink=(LPOLELINK)dwLink;
    HRESULT         hr;
    LPOLEOBJECT     pIOleObject;

    if (NULL==pIOleLink)
        return ResultFromScode(E_FAIL);

    hr=pIOleLink->QueryInterface(IID_IOleObject
        , (PPVOID)&pIOleObject);

    if (FAILED(hr))
        return hr;

    hr=pIOleObject->IsUpToDate();

    if (NOERROR!=hr)
        {
        hr=pIOleObject->Update();

        if (FAILED(hr))
            return hr;
        }

    return NOERROR;
    }






/*
 * CLinks::CancelLink
 *
 * Purpose:
 *  Sets the source moniker in the link to NULL but does nothing
 *  else.  How the container decides to convert this to static
 *  is its choice.
 *
 * Parameters:
 *  dwLink          DWORD, an IOleLink pointer to the object
 *                  affected.
 *
 * Return Value:
 *  HRESULT         Standard.
 */

STDMETHODIMP CLinks::CancelLink(DWORD dwLink)
    {
    LPOLELINK       pIOleLink=(LPOLELINK)dwLink;

    if (NULL!=pIOleLink)
        return pIOleLink->SetSourceMoniker(NULL, CLSID_NULL);

    return NOERROR;
    }





//PROTECTED FUNCTIONS INTERNAL TO CLinks

/*
 * CLinks::FValidateLinkSource
 * (Protected)
 *
 * Purpose:
 *  Given a name of a link source retrieve a moniker for it and
 *  a CLSID if we can bind.
 *
 * Parameters:
 *  pszName         LPTSTR of the source
 *  pchEaten        ULONG * into which to return how many
 *                  characters we parse.
 *  ppmk            LPMONIKER * into which to store the moniker
 *  pclsID          LPCLSID into which to store the clsID.
 *
 * Return Value:
 *  BOOL            TRUE if *ppmk has a valid moniker,
 *                  FALSE otherwise.
 */

BOOL CLinks::FValidateLinkSource(LPOLESTR pszName
    , ULONG *pchEaten, LPMONIKER *ppmk, LPCLSID pclsID)
    {
    HRESULT     hr;
    LPBC        pbc=NULL;
    LPOLEOBJECT pIOleObject;

    *ppmk=NULL;
    *pclsID=CLSID_NULL;

    if (FAILED(CreateBindCtx(0, &pbc)))
        return FALSE;

    hr=MkParseDisplayName(pbc, pszName, pchEaten, ppmk);

    if (SUCCEEDED(hr))
        {
        /*
         * Now that we have a moniker for this new source, so try
         * binding to that source and get its CLSID.
         */
        hr=(*ppmk)->BindToObject(pbc, NULL, IID_IOleObject
            , (PPVOID)&pIOleObject);

        if (SUCCEEDED(hr))
            {
            pIOleObject->GetUserClassID(pclsID);
            pIOleObject->Release();
            }

        return TRUE;
        }

    pbc->Release();
    return FALSE;
    }




/*
 * CLinks::FCreateNewSourceMoniker
 * (Protected)
 *
 * Purpose:
 *  Given a name of a link source create a moniker for it.
 *
 * Parameters:
 *  pszName         LPTSTR of the source
 *  cchName         ULONG length of the filename in pszName.
 *  ppmk            LPMONIKER * into which to store the moniker
 *
 * Return Value:
 *  BOOL            TRUE if *ppmk has a valid moniker,
 *                  FALSE otherwise.
 */

BOOL CLinks::FCreateNewSourceMoniker(LPOLESTR pszName
    , ULONG cchName, LPMONIKER *ppmk)
    {
    OLECHAR     szName[MAX_PATH];
    LPMONIKER   pmkFile=NULL;
    LPMONIKER   pmkItem=NULL;

    *ppmk=NULL;
    wcsncpy(szName, pszName, (int)cchName+1);

    CreateFileMoniker(szName, &pmkFile);

    if (NULL==pmkFile)
        return FALSE;

    if (wcslen(pszName) > (int)cchName)
        {
        wcscpy(szName, pszName+cchName+1);
        CreateItemMoniker(L"\\", szName, &pmkItem);

        if (NULL!=pmkItem)
            {
            CreateGenericComposite(pmkFile, pmkItem, ppmk);
            pmkItem->Release();
            }

        pmkFile->Release();

        if (NULL==*ppmk)
            return FALSE;
        }
    else
        *ppmk=pmkFile;

    return TRUE;
    }
