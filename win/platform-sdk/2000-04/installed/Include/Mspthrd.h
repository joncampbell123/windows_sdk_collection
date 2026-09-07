/*++

Copyright (c) 1998-1999 Microsoft Corporation

Module Name:

    mspthrd.h

Abstract:

    Definitions for MSP thread management classes.

--*/

#ifndef __MSPTHRD_H
#define __MSPTHRD_H

//
// Commands that the worker worker thread can handle.
//

typedef enum
{
    WORK_ITEM,          // process a work item
    STOP,               // kill the worker thread.

} COMMAND;

typedef struct
{
    COMMAND                cmd;
    LPTHREAD_START_ROUTINE pfn;
    PVOID                  pContext;
    HANDLE                 hEvent;

} COMMAND_NODE;

typedef struct
{
    LIST_ENTRY  link;
    COMMAND_NODE node;

} COMMAND_QUEUE_ITEM;

class CMSPThread
{
public:
    CMSPThread()
    {
        InitializeListHead(&m_CommandQueue);

        m_hCommandEvent = NULL;
        m_hThread       = NULL;

        m_iStartCount = 0;
    }

    ~CMSPThread() { };

    HRESULT Start();
    HRESULT Stop();

    // Shutdown is used to clean up the thread unconditionally. This can be
    // used as an alternative to matched Start() / Stop() calls.

    HRESULT Shutdown();

    HRESULT ThreadProc();

    HRESULT QueueWorkItem(
        LPTHREAD_START_ROUTINE Function,
        PVOID Context,
        BOOL  fSynchronous
        );

private:
    BOOL SignalThreadProc() { return SetEvent(m_hCommandEvent); }

private:

    CMSPCritSection         m_CountLock;     // Protects start count
    CMSPCritSection         m_QueueLock;     // Protects command queue
    int                     m_iStartCount;   // number of times we've been
                                             // started minus number of times
                                             // we've been stopped. If == 0
                                             // then we actually stop thread.
    LIST_ENTRY              m_CommandQueue;  // Queue of commands for thread
                                             // to process.
    HANDLE                  m_hCommandEvent; // Signaled to tell us to do
                                             // something.

    HANDLE                  m_hThread;       // The thread handle. We need to
                                             // save it so that we can wait
                                             // for it when stopping the
                                             // thread.
};

extern CMSPThread g_Thread;

#endif // __MSPTHRD_H

// eof
