/*++

Copyright (c) 1997-1999 Microsoft Corporation

Module Name:

    MSPStrm.h

Abstract:

    Definitions for CMSPStream class.

--*/
#ifndef _MSPSTRM_H_
#define _MSPSTRM_H_


/*++

Class Description:

    Represents a stream in a call.

--*/

#define STRM_INITIAL            0x00000000
#define STRM_TERMINALSELECTED   0x00000001
#define STRM_CONFIGURED         0x00000002
#define STRM_RUNNING            0x00000004
#define STRM_PAUSED             0x00000008
#define STRM_STOPPED            0x00000010

class ATL_NO_VTABLE CMSPStream : 
    public CComObjectRootEx<CComMultiThreadModelNoCS>,
    public IDispatchImpl<ITStream, &IID_ITStream, &LIBID_TAPI3Lib>
{
public:

BEGIN_COM_MAP(CMSPStream)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(ITStream)
    COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, m_pFTM)
END_COM_MAP()

DECLARE_GET_CONTROLLING_UNKNOWN()

    CMSPStream(); 
    ~CMSPStream();

// methods of the CComObject
    virtual void FinalRelease();

// ITStream methods, called by the app.
    STDMETHOD (get_MediaType) (
        OUT     long *                  plMediaType
        );

    STDMETHOD (get_Direction) (
        OUT     TERMINAL_DIRECTION *    pTerminalDirection
        );

    STDMETHOD (get_Name) (
        OUT     BSTR *                  ppName
        ) = 0;

    STDMETHOD (SelectTerminal) (
        IN      ITTerminal *            pTerminal
        );

    STDMETHOD (UnselectTerminal) (
        IN     ITTerminal *             pTerminal
        );

    STDMETHOD (EnumerateTerminals) (
        OUT     IEnumTerminal **        ppEnumTerminal
        );

    STDMETHOD (get_Terminals) (
        OUT     VARIANT *               pTerminals
        );

    STDMETHOD (StartStream) ();

    STDMETHOD (PauseStream) ();

    STDMETHOD (StopStream) ();

// methods called by the MSPCall object.
    virtual HRESULT Init(
        IN     HANDLE                   hAddress,
        IN     CMSPCallBase *           pMSPCall,
        IN     IMediaEvent *            pGraph,
        IN     DWORD                    dwMediaType,
        IN     TERMINAL_DIRECTION       Direction
        );

    virtual HRESULT ShutDown();

    virtual HRESULT GetState(
        OUT     DWORD *                  pdwStatus
        ) { return E_NOTIMPL; } // BUGBUG not actually implemented here

    virtual HRESULT HandleTSPData(
        IN     BYTE *                   pData,
        IN     DWORD                    dwSize
        );

    virtual HRESULT ProcessGraphEvent(
        IN  long lEventCode,
        IN  long lParam1,
        IN  long lParam2
        );

protected:
    // Pointer to the free threaded marshaler.
    IUnknown *                  m_pFTM;

    // The current state of the stream.
    DWORD                       m_dwState;

    // The media type of this stream. Audio, video, or others.
    DWORD                       m_dwMediaType;

    // The direction of this stream. Incoming or outgoing.
    TERMINAL_DIRECTION          m_Direction;

    // The address on which this stream is being used.
    HANDLE                      m_hAddress;

    // The reference to the call object.
    CMSPCallBase *              m_pMSPCall;

    // The pointers to the graph object interfaces.
    IGraphBuilder *             m_pIGraphBuilder;
    IMediaControl *             m_pIMediaControl;

    // The list of stream objects in the call.
    CMSPArray <ITTerminal *>    m_Terminals;

    // The lock that protects the stream object. The stream object 
    // should never acquire the lock and then call a MSPCall method 
    // that might lock.
    CMSPCritSection             m_lock;
};

#endif // __MSPSTRM_H_
