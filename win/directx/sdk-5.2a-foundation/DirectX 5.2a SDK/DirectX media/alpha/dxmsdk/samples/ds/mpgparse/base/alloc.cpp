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
/*  Sequential allocator methods */
#include <streams.h>
#include <alloc.h>

CSequentialAllocator::CSequentialAllocator(
    LPUNKNOWN  pUnk,
    HRESULT   *phr
) : CMemAllocator(TEXT("CSequential Allocator"), pUnk, phr),
    m_pbNext(m_pBuffer),
    m_parSamples(NULL),
    m_lValid(0),
    m_pbStartValid(NULL)
{
}

CSequentialAllocator::~CSequentialAllocator()
{
    if (m_parSamples != NULL) {
        delete [] m_parSamples;
    }
}

STDMETHODIMP CSequentialAllocator::GetBuffer(
    IMediaSample **ppBuffer,
    REFERENCE_TIME * pStartTime,
    REFERENCE_TIME * pEndTime,
    DWORD dwFlags
)
{
    /*  Like the normal version except we will only allocate the NEXT
        buffer
    */

    UNREFERENCED_PARAMETER(pStartTime);
    UNREFERENCED_PARAMETER(pEndTime);
    UNREFERENCED_PARAMETER(dwFlags);
    CMediaSample *pSample = NULL;

    *ppBuffer = NULL;
    for (;;)
    {
	{  // scope for lock
	    CAutoLock cObjectLock(this);

	    /* Check we are committed */
	    if (!m_bCommitted) {
		return VFW_E_NOT_COMMITTED;
	    }
            /* Check if the one we want is there */
            CMediaSample *pSearch = m_lFree.Head();
            while (pSearch) {
                PBYTE pbBuffer;
                pSearch->GetPointer(&pbBuffer);
                if (pbBuffer == m_pbNext) {
                    m_lFree.Remove(pSearch);
                    pSample = pSearch;
                    ASSERT(m_lSize == pSample->GetSize());

                    //  In case an error occurs and GetBuffer is not
                    //  followed by AddBuffer fix m_pbStartValid
                    if (m_lValid == 0) {
                        m_pbStartValid = pbBuffer;
                    }

                    m_pbNext += m_lSize;
                    if (m_pbNext == m_pBuffer + m_lSize * m_lCount) {
                        m_pbNext = m_pBuffer;
                    }
                    break;
                } else {
                    pSearch = m_lFree.Next(pSearch);
                }
            }
            if (pSample == NULL) {
                /*  If there were some samples but just not ours someone
                    else may be waiting
                */
                if (m_lFree.GetCount() != 0) {
                    NotifySample();
                }
                SetWaiting();
            }
	}

	/* If we didn't get a sample then wait for the list to signal */

	if (pSample) {
	    break;
	}
        ASSERT(m_hSem != NULL);
        DbgLog((LOG_TRACE, 4, TEXT("Waiting - %d buffers available"),
                m_lFree.GetCount()));
	WaitForSingleObject(m_hSem, INFINITE);
    }

#ifdef VFW_S_CANT_CUE
    /* Addref the buffer up to one. On release
       back to zero instead of being deleted, it will requeue itself by
       calling the ReleaseBuffer member function. NOTE the owner of a
       media sample must always be derived from CBaseAllocator */


    ASSERT(pSample->m_cRef == 0);
    pSample->m_cRef = 1;
    *ppBuffer = pSample;
#else
    /* This QueryInterface should addref the buffer up to one. On release
       back to zero instead of being deleted, it will requeue itself by
       calling the ReleaseBuffer member function. NOTE the owner of a
       media sample must always be derived from CBaseAllocator */

    HRESULT hr = pSample->QueryInterface(IID_IMediaSample, (void **)ppBuffer);

    /* For each sample outstanding, we need to AddRef ourselves on his behalf
       he cannot do it, as there is no correct ordering of his release and his
       call to ReleaseBuffer as both could destroy him. We release this count
       in ReleaseBuffer, called when the sample's count drops to zero */

    AddRef();
#endif
    return NOERROR;
}

HRESULT CSequentialAllocator::Alloc()
{
    CAutoLock lck(this);
    if (m_parSamples != NULL) {
        delete [] m_parSamples;
    }
    m_parSamples = new LPCMEDIASAMPLE[m_lCount];
    if (m_parSamples == NULL) {
        return E_OUTOFMEMORY;
    }
    HRESULT hr = CMemAllocator::Alloc();

    if (S_OK == hr) {
        ASSERT(m_lCount == m_lFree.GetCount());

        /* Find the smallest */
        CMediaSample *pSample = m_lFree.Head();

        m_pBuffer = (PBYTE)(DWORD)-1;
        for (; pSample != NULL; pSample = m_lFree.Next(pSample)) {
            PBYTE pbTemp;
            pSample->GetPointer(&pbTemp);
            if (m_pBuffer > pbTemp) {
                m_pBuffer = pbTemp;
            }
        }

        pSample = m_lFree.Head();
        for ( ;pSample != NULL; pSample = m_lFree.Next(pSample)) {
            PBYTE pbTemp;
            pSample->GetPointer(&pbTemp);
            m_parSamples[BufferIndex(pbTemp)] = pSample;
        }
    }
    m_pbStartValid = m_pBuffer;
    m_pbNext       = m_pBuffer;
    ASSERT(m_lValid == 0);
    return hr;
}


/*  Get buffer index */
int CSequentialAllocator::BufferIndex(PBYTE pbBuffer)
{
    int iPos = (pbBuffer - m_pBuffer) / m_lSize;
    ASSERT(iPos <= m_lCount);
    return iPos;
}
/*  Given an address get the IMediaSample pointer -
    NB needs optimizing
*/
CMediaSample *CSequentialAllocator::SampleFromBuffer(PBYTE pBuffer)
{
    return m_parSamples[BufferIndex(pBuffer)];
}

/*  Add a buffer to the valid list */
void CSequentialAllocator::AddBuffer(CMediaSample *pSample)
{
    /*  Don't get fooled by 0 length buffers ! */
    if (pSample->GetActualDataLength() == 0) {
        return;
    }
    pSample->AddRef();
#ifdef DEBUG
    PBYTE pbBuffer;
    pSample->GetPointer(&pbBuffer);
    ASSERT(m_pbStartValid + m_lValid == pbBuffer);
#endif
    m_lValid += pSample->GetActualDataLength();
}

/*  Step through valid data */
HRESULT CSequentialAllocator::Advance(LONG lAdvance)
{
    ASSERT(lAdvance <= m_lValid);

    /*  For every sample boundary we step over we should Release()
        a buffer
    */
    DbgLog((LOG_TRACE, 4, TEXT("Advance(%d) - valid %d"), lAdvance, m_lValid));
    int iStart = BufferIndex(m_pbStartValid);
    int iEnd = BufferIndex(m_pbStartValid + lAdvance);
    m_lValid -= lAdvance;
    m_pbStartValid += lAdvance;

    /*  If we're at the end and the last buffer wasn't full move on
        to the next
    */
    if (m_lValid == 0 &&
        (m_pbStartValid - m_pBuffer) % m_lSize != 0) {
        iEnd++;
        m_pbStartValid = m_pBuffer + m_lSize * iEnd;
    }
    while (iStart != iEnd) {
        m_parSamples[iStart]->Release();

        iStart++;
    }


    ASSERT(m_lValid <= (m_lCount * m_lSize) / 2);

    /*
        If we're already into the last buffer and about to wrap
        NOTE m_Valid CAN be >  m_lSize - hit it with a huge
        PROGRAM_STREAM_DIRECTORY packet (can be almost 64K)
    */
    if (m_pbStartValid + m_lValid == m_pBuffer + m_lSize * m_lCount) {
        return Wrap();
    } else {
        return S_OK;
    }
}

/*  Get the valid part */
PBYTE CSequentialAllocator::GetValid(LONG *plValid)
{
    *plValid = m_lValid;
    return m_pbStartValid;
}

/*  Wrap end to go back to start */
HRESULT CSequentialAllocator::Wrap(void)
{
    if (m_lValid != 0) {

        /*  Make sure the copy will work */
        ASSERT(m_lValid <= (m_lSize * m_lCount) / 2);
        IMediaSample *pSample;

        /*  These samples will be AddRef'd already */
        int nBuffers = (m_lValid + m_lSize - 1) / m_lSize;
        int iFirst = BufferIndex(m_pbStartValid);
        int iLast = BufferIndex(m_pbStartValid + m_lValid);
        if (0 == (m_pbStartValid + m_lValid - m_pBuffer) % m_lSize) {
            iLast--;
        }
        ASSERT(nBuffers > 0);
        ASSERT(iFirst <= iLast);

        ASSERT(nBuffers <= m_lCount / 2);
        for (int i = 0; i < nBuffers; i++) {
            HRESULT hr = GetBuffer(&pSample, NULL, NULL, 0);
            if (FAILED(hr)) {
                return hr;
            }
            ASSERT(pSample == m_parSamples[i]);
        }

        /*  Now copy the data back to the start */
        CopyMemory((PVOID)(m_pBuffer + m_lSize * nBuffers - m_lValid),
                   (PVOID)m_pbStartValid,
                   m_lValid);
        m_pbStartValid = m_pBuffer + m_lSize * nBuffers - m_lValid;

        /*  Release the last buffers since we've effectively
            transferred the ref count to the new buffers with GetBuffer
        */
        for ( i = iFirst; i <= iLast; i++) {
            m_parSamples[i]->Release();
        }
    } else {
        m_pbStartValid = m_pBuffer;
        m_pbNext = m_pBuffer;
    }
    return S_OK;
}

/*  Flush the allocator - just discard all the data in it */
void CSequentialAllocator::Flush()
{
    Advance(m_lValid);
}
