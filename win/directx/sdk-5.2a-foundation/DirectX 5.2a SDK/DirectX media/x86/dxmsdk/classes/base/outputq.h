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

//
//  outputq.h
//
//  Defines the COutputQueue class
//
//  Makes a queue of samples and sends them
//  to an output pin
//
//  The class will optionally send the samples to the pin directly
//
//

typedef CGenericList<IMediaSample> CSampleList;

class COutputQueue : public CCritSec
{
public:
    //  Constructor
    COutputQueue(IPin      *pInputPin,          //  Pin to send stuff to
                 HRESULT   *phr,                //  'Return code'
                 BOOL       bAuto = TRUE,       //  Ask pin if blocks
                 BOOL       bQueue = TRUE,      //  Send through queue (ignored if
                                                //  bAuto set)
                 LONG       lBatchSize = 1,     //  Batch
                 BOOL       bBatchExact = FALSE,//  Batch exactly to BatchSize
                 LONG       lListSize =         //  Likely number in the list
                                DEFAULTCACHE,
                 DWORD      dwPriority =        //  Priority of thread to create
                                THREAD_PRIORITY_NORMAL
                );
    ~COutputQueue();

    // enter flush state - discard all data
    void BeginFlush();      // Begin flushing samples

    // re-enable receives (pass this downstream)
    void EndFlush();        // Complete flush of samples - downstream
                            // pin guaranteed not to block at this stage

    void EOS();             // Call this on End of stream

    void SendAnyway();      // Send batched samples anyway (if bBatchExact set)

    void NewSegment(
            REFERENCE_TIME tStart,
            REFERENCE_TIME tStop,
            double dRate);

    HRESULT Receive(IMediaSample *pSample);

    // do something with these media samples
    HRESULT ReceiveMultiple (
        IMediaSample **pSamples,
        long nSamples,
        long *nSamplesProcessed);

    void Reset();           // Reset m_hr ready for more data

    //  See if its idle or not
    BOOL IsIdle();

protected:
    static DWORD WINAPI InitialThreadProc(LPVOID pv);
    DWORD ThreadProc();
    BOOL  IsQueued()
    {
        return m_List != NULL;
    };

    //  The critical section MUST be held when this is called
    void QueueSample(IMediaSample *pSample);

    BOOL IsSpecialSample(IMediaSample *pSample)
    {
        return (DWORD)pSample > 0xFFFFFFF0;
    };

    //  Remove and Release() batched and queued samples
    void FreeSamples();

    //  Notify the thread there is something to do
    void NotifyThread();


protected:
    //  Queue 'messages'
    #define SEND_PACKET      ((IMediaSample *)0xFFFFFFFE)  // Send batch
    #define EOS_PACKET       ((IMediaSample *)0xFFFFFFFD)  // End of stream
    #define RESET_PACKET     ((IMediaSample *)0xFFFFFFFC)  // Reset m_hr
    #define NEW_SEGMENT      ((IMediaSample *)0xFFFFFFFB)  // send NewSegment

    // new segment packet is always followed by one of these
    struct NewSegmentPacket {
        REFERENCE_TIME tStart;
        REFERENCE_TIME tStop;
        double dRate;
    };

    // Remember input stuff
    IPin          * const m_pPin;
    IMemInputPin  *       m_pInputPin;
    BOOL            const m_bBatchExact;
    LONG            const m_lBatchSize;

    CSampleList   *       m_List;
    HANDLE                m_hSem;
    CAMEvent                m_evFlushComplete;
    HANDLE                m_hThread;
    IMediaSample  **      m_ppSamples;
    LONG                  m_nBatched;

    //  Wait optimization
    LONG                  m_lWaiting;
    //  Flush synchronization
    BOOL                  m_bFlushing;
    BOOL                  m_bFlushed;

    //  Terminate now
    BOOL                  m_bTerminate;

    //  Send anyway flag for batching
    BOOL                  m_bSendAnyway;

    //  Deferred 'return code'
    BOOL volatile         m_hr;
};

