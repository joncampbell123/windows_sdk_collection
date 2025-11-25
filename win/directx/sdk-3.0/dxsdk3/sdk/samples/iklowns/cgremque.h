/*===========================================================================*\
|
|  File:        cgremque.h
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

#ifndef _CGREMQUE_H
#define _CGREMQUE_H

// Entry for object in master object list
typedef struct 
{
    REMOTE_OBJECT   RemObj;
    void        *Data;
    CLinkedList *ActionList;
} REMOTE_DATA;

CLinkedList *CreateRemoteObjectQueue(REMOTE_OBJECT *, void *);
void DestroyRemoteObjectQueue(REMOTE_OBJECT *);
CLinkedList *GetRemoteObjectQueue(REMOTE_OBJECT *);

ACTION GetNextRemoteAction(CLinkedList *, void *&, DWORD &);
BOOL QueueRemoteAction(CLinkedList *, ACTION, void *, DWORD);

REMOTE_DATA *FindRemoteObjectEntry(REMOTE_OBJECT *);
#endif
