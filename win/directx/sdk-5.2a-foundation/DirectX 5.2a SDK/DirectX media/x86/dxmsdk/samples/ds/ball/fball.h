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

// Generates a movie on the fly of a bouncing ball...

// The class managing the output pin
class CBallStream;

// Main object for a bouncing ball filter
class CBouncingBall : public CSource
{

public:

    // The only allowed way to create Bouncing balls!
    static CUnknown * WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT *phr);

private:

    // It is only allowed to to create these objects with CreateInstance
    CBouncingBall(LPUNKNOWN lpunk, HRESULT *phr);

}; // CBouncingBall


// CBallStream manages the data flow from the output pin.
class CBallStream : public CSourceStream
{

public:

    CBallStream(HRESULT *phr, CBouncingBall *pParent, LPCWSTR pPinName);
    ~CBallStream();

    // plots a ball into the supplied video frame
    HRESULT FillBuffer(IMediaSample *pms);

    // Ask for buffers of the size appropriate to the agreed media type
    HRESULT DecideBufferSize(IMemAllocator *pIMemAlloc,
                             ALLOCATOR_PROPERTIES *pProperties);

    // Set the agreed media type, and set up the necessary ball parameters
    HRESULT SetMediaType(const CMediaType *pMediaType);

    // Because we calculate the ball there is no reason why we
    // can't calculate it in any one of a set of formats...
    HRESULT CheckMediaType(const CMediaType *pMediaType);
    HRESULT GetMediaType(int iPosition, CMediaType *pmt);

    // Resets the stream time to zero
    HRESULT OnThreadCreate(void);

    // Quality control notifications sent to us
    STDMETHODIMP Notify(IBaseFilter * pSender, Quality q);

private:

    int m_iImageHeight;	                // The current image height
    int m_iImageWidth;	                // And current image width
    int m_iRepeatTime;                  // Time in msec between frames
    const int m_iDefaultRepeatTime;     // Initial m_iRepeatTime
    BYTE m_BallPixel[4];	        // Represents one coloured ball
    int	m_iPixelSize;	                // The pixel size in bytes
    PALETTEENTRY m_Palette[256];	// The optimal palette for the image
    CCritSec m_cSharedState;	        // Lock on m_rtSampleTime and m_Ball
    BOOL m_bZeroMemory;                 // Do we need to clear the buffer
    CRefTime m_rtSampleTime;	        // The time stamp for each sample
    CBall *m_Ball;	                // The current ball object

    // set up the palette appropriately
    enum Colour {Red, Blue, Green, Yellow};
    HRESULT SetPaletteEntries(Colour colour);

}; // CBallStream
	
