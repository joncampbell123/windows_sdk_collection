//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992 - 1997  Microsoft Corporation.  All Rights Reserved.
//
//--------------------------------------------------------------------------;

// Non MFC based generic cache class, January 1995

/* This class implements a simple cache. A cache object is instantiated
   with the number of items it is to hold. An item is a pointer to an
   object derived from CBaseObject (helps reduce memory leaks). The cache
   can then have objects added to it and removed from it. The cache size
   is fixed at construction time and may therefore run out or be flooded.
   If it runs out it returns a NULL pointer, if it fills up it also returns
   a NULL pointer instead of a pointer to the object just inserted */

#include <streams.h>

/* Constructor creates an array of pointers (to CBaseObject derived objects)
   and then zero fills each position. The cache cannot be resized dynamically
   so if the number of objects that you use varies widely during the lifetime
   of the cache then there will be some inefficiency. Making the cache resize
   would be an expensive operation especially with so little to be gained */

CCache::CCache(TCHAR *pName, INT iItems) :
    CBaseObject(pName),
    m_iCacheSize(iItems),
    m_iUsed(0)
{
    ASSERT(iItems > 0);

    /* Create the array of pointers and set the cache size, if this does not
       succeed then it should probably throw an exception otherwise there is
       no way to propogate the error condition back to creating object */

    m_ppObjects = (CBaseObject **) new BYTE[iItems * sizeof(CBaseObject *)];
    ASSERT(m_ppObjects);

    ZeroMemory((PVOID) m_ppObjects,iItems * sizeof(CBaseObject *));
}


/* Destructor causes the cached objects to be deleted (if not already done)
   and then deletes the (variable length) array that was used to hold them */

CCache::~CCache()
{
    RemoveAll();
    delete m_ppObjects;
}


/* Add an item to the cache, if no space is found for it then we return NULL
   otherwise we put the object in the free space. The object is meant to be
   as small and lightweight as possible so it does not do any multithread
   protection. This is assumed to be handled by the owning (list) object */

CBaseObject *CCache::AddToCache(CBaseObject *pObject)
{
    /* Have we any room for this object */

    if (m_iUsed == m_iCacheSize) {
        return NULL;
    }

    /* Add the object to the end of the cache */

    m_ppObjects[m_iUsed] = pObject;
    m_iUsed++;
    return pObject;
}


/* This is called to retrieve one of the objects we were given to look after
   if there are none available then we return NULL otherwise the return the
   object. NOTE we do not do any processing on the object so it will be in
   exactly the same state when it is handed back as when it is installed */

CBaseObject *CCache::RemoveFromCache()
{
    /* Are all the cache positions empty */

    if (m_iUsed == 0) {
        return NULL;
    }

    /* Return the last object put into the cache */

    m_iUsed--;
    ASSERT(m_ppObjects[m_iUsed]);
    return m_ppObjects[m_iUsed];
}


/* Delete all the objects held in the cache, simply scans the list of objects
   in the cache and deletes each of them in turn. The list class uses a cache
   to manage node objects that are frequently created and destroyed and when
   the list is finally deleted we will also go at which point we also delete
   any remaining node objects. The objects must be derived from CBaseObject */

void CCache::RemoveAll()
{
    while (m_iUsed--) {
        ASSERT(m_ppObjects[m_iUsed]);
        delete m_ppObjects[m_iUsed];
    }
}
