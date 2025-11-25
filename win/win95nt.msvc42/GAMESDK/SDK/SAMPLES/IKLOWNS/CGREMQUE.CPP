/*===========================================================================*\
|
|  File:        cgremque.cpp
|
|  Description: 
|   Routines to create and manage received remote actions
|       
|-----------------------------------------------------------------------------
|
|  Copyright (C) 1995-1996 Microsoft Corporation.  All Rights Reserved.
|
|  Written by Moss Bay Engineering, Inc. under contract to Microsoft Corporation
|
\*===========================================================================*/

/**************************************************************************

    (C) Copyright 1995-1996 Microsoft Corp.  All rights reserved.

    You have a royalty-free right to use, modify, reproduce and 
    distribute the Sample Files (and/or any modified version) in 
    any way you find useful, provided that you agree that 
    Microsoft has no warranty obligations or liability for any 
    Sample Application Files which are modified. 

    we do not recomend you base your game on IKlowns, start with one of
    the other simpler sample apps in the GDK

 **************************************************************************/

//** include files **
#ifndef __WATCOMC__
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <windowsx.h>
#include "linklist.h"
#include "dplay.h"
#include "cgremote.h"

//** local definitions **

// Entry for object action in action queue
typedef struct 
{
    ACTION  Action;
    DWORD   nBytes;
    void    *Data;
} ACTION_ENTRY;

//** external functions **

//** external data **

//** public data **
//** private data **

// Master list of objects controlled by remote peers
static CLinkedList  RemoteObjectList;

//** public functions **
//** private functions **

// ----------------------------------------------------------
// CreateRemoteObjectQueue - register a new object and create an
//  action queue for it!
// ----------------------------------------------------------
CLinkedList *CreateRemoteObjectQueue(
    REMOTE_OBJECT   *pObj,  // unique object id 
    void    *Data       // game character this pointer
)
{
    if (pObj == NULL)
        return(NULL);

    // Create an object entry for the master list
    REMOTE_DATA *pRemoteEntry    = new REMOTE_DATA;

    // Initialize the object entry and add it to the list!
    memcpy(&pRemoteEntry->RemObj, pObj, sizeof(REMOTE_OBJECT));
    pRemoteEntry->Data = Data;
    pRemoteEntry->ActionList = new CLinkedList;
    RemoteObjectList.Add((void *)pRemoteEntry);

    return(pRemoteEntry->ActionList);
}   

// ----------------------------------------------------------
// FindRemoteObjectEntry - search list of registered objects 
//  and return its data
// ----------------------------------------------------------
REMOTE_DATA *FindRemoteObjectEntry(
    REMOTE_OBJECT   *pObj       // unique object id
)
{
    REMOTE_DATA *pRemoteEntry=NULL;

    // Beware of wise guys!
    if (pObj == NULL)
        return(NULL);

    // Search master object list for desired object entry
    pRemoteEntry = (REMOTE_DATA *)RemoteObjectList.GetFirst();
    while (pRemoteEntry != NULL)
    {
        if (memcmp(&pRemoteEntry->RemObj, pObj, sizeof(REMOTE_OBJECT)) == 0) 
        {
            break;          
        }
        pRemoteEntry = (REMOTE_DATA *) RemoteObjectList.GetNext();
    }
    return(pRemoteEntry);
}   

// ----------------------------------------------------------
// DestroyRemoteObjectQueue - destroy an objects action queue
//  and remove it from the list of known objects.
// ----------------------------------------------------------
void DestroyRemoteObjectQueue(
    REMOTE_OBJECT   *pObj       // unique object id
)
{
    REMOTE_DATA *pRemoteEntry;

    // If there is an entry in the master list, remove it!
    pRemoteEntry = FindRemoteObjectEntry(pObj);
    if (pRemoteEntry != NULL)
    {
        RemoteObjectList.Remove((void *)pRemoteEntry);
        delete pRemoteEntry;
    }
}
    
// ----------------------------------------------------------
// GetRemoteObjectQueue - get a pointer to the action queue
// ----------------------------------------------------------
CLinkedList *GetRemoteObjectQueue(
    REMOTE_OBJECT *pObj     // unique object id
)
{
    REMOTE_DATA *pRemoteEntry;

    // Look up object in master list and return a ptr to 
    // its action queue
    pRemoteEntry = FindRemoteObjectEntry(pObj);
    if (pRemoteEntry != NULL)
        return(pRemoteEntry->ActionList);
    else
        return(NULL);
    
}   

// ----------------------------------------------------------
// GetRemoteObjectData - retrieve an objects data value
// ----------------------------------------------------------
void *GetRemoteObjectData(
    REMOTE_OBJECT *pObj     // unique object id
)
{
    REMOTE_DATA *pRemoteEntry;

    // Look up object in master list and return its data value
    pRemoteEntry = FindRemoteObjectEntry(pObj);
    if (pRemoteEntry != NULL)
        return(pRemoteEntry->Data);
    else
        return(NULL);
    
}   

// ----------------------------------------------------------
// QueueRemoteAction - place an action onto the objects queue
// ----------------------------------------------------------
BOOL QueueRemoteAction(
    CLinkedList *pQueue,    // object's action queue 
    ACTION      Action,     // action code
    void        *Data,      // action data 
    DWORD       nBytes      // size of action data
)
{
    // Be sure action queue is valid!
    if (pQueue == NULL)
        return(FALSE);

    // Place a new action entry on the queue
    ACTION_ENTRY    *pAction = new ACTION_ENTRY;

    pAction->Action = Action;
    pAction->nBytes = nBytes;
    pAction->Data = Data; 
    pQueue->Append((void *)pAction);
    return(TRUE);
}   

// ----------------------------------------------------------
// GetNextRemoteAction - get the next action on the objects queue
// ----------------------------------------------------------
ACTION GetNextRemoteAction(
    CLinkedList *pQueue,    // object's action queue
    void        *&Data,     // ptr to a data ptr
    DWORD       &nBytes     // size of action data
)
{
    ACTION_ENTRY    *pAction;
    ACTION      Action=-1;

    // Be sure action queue is valid
    if (pQueue == NULL)
        return(-1);
    
    // Take first action off queue
    pAction = (ACTION_ENTRY *)pQueue->RemoveFirst();

    // If there is an action, set information for caller 
    if (pAction != NULL)
    {
        Data = pAction->Data;
        nBytes = pAction->nBytes;
        Action = pAction->Action;

        // Dispose of the action entry memory, the caller should
        // delete the actual data itself (using ReleaseRemoteData) 
        delete pAction;
    }

    return(Action);
}   
