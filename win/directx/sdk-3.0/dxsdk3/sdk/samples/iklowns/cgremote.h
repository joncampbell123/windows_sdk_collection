/*===========================================================================*\
|
|  File:        cgremote.h
|
|  Description: 
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

#ifndef _CGREMOTE_H
#define _CGREMOTE_H
#include "linklist.h"
#include "dplay.h"

#define MAX_OBJ_NAME    9

typedef int ACTION;
typedef char OBJ_ID[MAX_OBJ_NAME];

typedef struct 
{
        BYTE    InstanceID;
        BYTE    OwnerID;
    OBJ_ID  ObjectID;
} REMOTE_OBJECT;

#include "cgremque.h"

#define CREATE_OBJECT   0x81
#define DESTROY_OBJECT  0x82

REMOTE_OBJECT *CreateRemotePeers(
    char    *name, 
    DWORD   InstanceID
);

inline ACTION PollRemoteAction(
    REMOTE_OBJECT *pObj,
    void *&Data,
    DWORD &nDataSize
)
{
    return(GetNextRemoteAction(GetRemoteObjectQueue(pObj), Data, nDataSize));
}

BOOL SendRemoteAction(
    REMOTE_OBJECT *pObj,
    ACTION Action, 
    void *Data,
    DWORD nDataSize
);

void ReleaseRemoteData(void *);

BOOL DestroyRemotePeer(
    REMOTE_OBJECT *pObj
);

BOOL RemoteCreateLobby(void);
BOOL RemoteConnect(REFGUID pGuid, LPSTR FullName, LPSTR NickName);
BOOL RemoteCreate(REFGUID pGuid, LPSTR FullName, LPSTR NickName);

class CGameInput;


void SetCurrentLevel(void   *);

#endif

