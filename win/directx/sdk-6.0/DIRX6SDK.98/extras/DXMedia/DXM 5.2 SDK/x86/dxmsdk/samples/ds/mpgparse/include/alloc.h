//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1996 - 1997  Microsoft Corporation.  All Rights Reserved.
//
//--------------------------------------------------------------------------;
/*  Allocator for sequential buffers
    Like CBaseAllocator BUT always allocates the next buffer
*/

typedef CMediaSample *LPCMEDIASAMPLE;

class CSequentialAllocator : public CMemAllocator
{
public:
    CSequentialAllocator(
        LPUNKNOWN  pUnk,
        HRESULT   *phr
    );

    ~CSequentialAllocator();

    STDMETHODIMP GetBuffer(
        IMediaSample **ppBuffer,
        REFERENCE_TIME * pStartTime,
        REFERENCE_TIME * pEndTime,
        DWORD dwFlags);

    HRESULT Alloc();

    /*  Get buffer index */
    int BufferIndex(PBYTE pbBuffer);

    /*  Given an address get the IMediaSample pointer -
        NB needs optimizing
    */
    CMediaSample *SampleFromBuffer(PBYTE pBuffer);

    /*  Add a buffer to the valid list */
    void AddBuffer(CMediaSample *pSample);

    /*  Step through valid data */
    HRESULT Advance(LONG lAdvance);

    /*  Get the valid part */
    PBYTE GetValid(LONG *plValid);

    /*  Wrap end to go back to start */
    HRESULT Wrap(void);

    /*  Flush the allocator - just discard all the data in it */
    void Flush();

private:
    PBYTE           m_pbNext;
    LPCMEDIASAMPLE *m_parSamples;

    /*  Simple wrap around buffer stuff */
    LONG            m_lValid;
    PBYTE           m_pbStartValid;
    PBYTE           m_pBuffer;  /* Copy of CMemAllocator's which is private */
};

/*  Allocator for subsamples */
class CSubAllocator : public CBaseAllocator
{
public:
    CSubAllocator(
        LPUNKNOWN  pUnk,
        HRESULT   *phr,
        CSequentialAllocator *pAlloc
    ) : CBaseAllocator(NAME("CSubAllocator"), pUnk, phr),
        m_pAlloc(pAlloc)
    {
    }

    CMediaSample *GetSample(PBYTE pbData, DWORD dwLen)
    {
        HRESULT hr = S_OK;
        CMediaSample *pSample = new CMediaSample(
                                        NAME("CMediaSample"),
                                        this,
                                        &hr,
                                        pbData,
                                        dwLen);
        if (pSample != NULL) {

            /*  We only need to lock the first buffer because
                the super allocator allocates samples sequentially
                so it can't allocate subsequent samples until
                the first one has been freed
            */
            m_pAlloc->SampleFromBuffer(pbData)->AddRef();

            /*  AddRef() ourselves too to conform to the rules */
            pSample->AddRef();

            /*  Make sure WE don't go away too ! */
            AddRef();
        }
        return pSample;
    }

    STDMETHODIMP ReleaseBuffer(IMediaSample * pSample)
    {
        /*  Free the superallocator's buffer */
        CMediaSample *pMediaSample = (CMediaSample *)pSample;
        PBYTE pBuffer;
        pSample->GetPointer(&pBuffer);
        m_pAlloc->SampleFromBuffer(pBuffer)->Release();
        delete pMediaSample;

        Release();
        return NOERROR;
    }


    /*  Must override Free() */
    void Free() {}

private:
    CSequentialAllocator *const m_pAlloc;
};


/*  Track samples in buffer */
