
#define  IDM_OPEN   100
#define  IDM_CLOSE  101
#define  IDM_ABOUT  102
#define  IDM_SEND   103
#define  IDM_READ   104
#define  IDM_RESET  105

#define  IDM_NO_FILTER      110
#define  IDM_DIRECTED       111
#define  IDM_MULTICAST      112
#define  IDM_BROADCAST      113
#define  IDM_ALL_MULTICAST  114
#define  IDM_PROMISCUOUS    115


typedef struct _ADAPTER {
    HANDLE      hFile;
    HANDLE      hEvent;
    SC_HANDLE   schSCManager;
    SC_HANDLE   ServiceHandle;
    TCHAR       AdapterName[64];

    HANDLE      hMem;
    LPBYTE      lpMem;
    HGLOBAL     hMem2;
    LPBYTE      lpMem2;
    ULONG       LastReadSize;
    UINT        BufferSize;
    } ADAPTER, *PADAPTER;


#define WM_DUMPCHANGE         0x8001








//
// Macro definition for defining IOCTL and FSCTL function control codes.  Note
// that function codes 0-2047 are reserved for Microsoft Corporation, and
// 2048-4095 are reserved for customers.
//

#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)

//
// Define the method codes for how buffers are passed for I/O and FS controls
//

#define METHOD_BUFFERED                 0
#define METHOD_IN_DIRECT                1
#define METHOD_OUT_DIRECT               2
#define METHOD_NEITHER                  3

//
// Define the access check value for any access
//
//
// The FILE_READ_ACCESS and FILE_WRITE_ACCESS constants are also defined in
// ntioapi.h as FILE_READ_DATA and FILE_WRITE_DATA. The values for these
// constants *MUST* always be in sync.
//


#define FILE_ANY_ACCESS                 0
#define FILE_READ_ACCESS          ( 0x0001 )    // file & pipe
#define FILE_WRITE_ACCESS         ( 0x0002 )    // file & pipe


#define FILE_DEVICE_PROTOCOL        0x8000

#define PACKET_SET                  0x00c0
#define PACKET_QUERY                0x0080

#define PACKET_FILTER               0x000e
#define PACKET_RESET                0x00ff



#define IOCTL_PROTOCOL_SET_FILTER   CTL_CODE(FILE_DEVICE_PROTOCOL, PACKET_SET | PACKET_FILTER, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PROTOCOL_GET_FILTER   CTL_CODE(FILE_DEVICE_PROTOCOL, PACKET_QUERY | PACKET_FILTER, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_PROTOCOL_RESET        CTL_CODE(FILE_DEVICE_PROTOCOL, PACKET_RESET, METHOD_BUFFERED, FILE_ANY_ACCESS)




#define NDIS_PACKET_TYPE_DIRECTED           0x0001
#define NDIS_PACKET_TYPE_MULTICAST          0x0002
#define NDIS_PACKET_TYPE_ALL_MULTICAST      0x0004
#define NDIS_PACKET_TYPE_BROADCAST          0x0008
#define NDIS_PACKET_TYPE_SOURCE_ROUTING     0x0010
#define NDIS_PACKET_TYPE_PROMISCUOUS        0x0020
#define NDIS_PACKET_TYPE_SMT                0x0040
#define NDIS_PACKET_TYPE_MAC_FRAME          0x8000
#define NDIS_PACKET_TYPE_FUNCTIONAL         0x4000
#define NDIS_PACKET_TYPE_ALL_FUNCTIONAL     0x2000
#define NDIS_PACKET_TYPE_GROUP              0x1000
