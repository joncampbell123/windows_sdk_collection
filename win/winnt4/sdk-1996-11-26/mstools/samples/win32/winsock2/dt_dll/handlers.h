/*++
  
  Copyright (c) 1996 Microsoft Corporation.

  Copyright (c) 1995 Intel Corp
  
  Module Name:
  
    handlers.h
  
  Abstract:
  
    Contains handler function prototypes and typedefs for handlers.cpp.
  
--*/

//
// Typedefs
//

// This typedef defines a pointer to a handler function.  See
// dt_dll.cpp for examples of how this is used. 
typedef BOOL (CALLBACK * LPFNDTHANDLER)(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

typedef LPFNDTHANDLER *LPLPFNDTHANDLER;



//
// Function Prototypes
//

BOOL
DTHandlerInit(
    OUT LPLPFNDTHANDLER HandlerFuncTable,
    int                 NumEntries);


BOOL CALLBACK
DTHandler_accept(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_bind(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_closesocket(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_connect(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_getpeername(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_getsockname(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_getsockopt(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_htonl(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_htons(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_ioctlsocket(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_listen(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_ntohl(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_ntohs(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_recv(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_recvfrom(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_select(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_send(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_sendto(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_setsockopt(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_shutdown(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_socket(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSAAccept(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSAAsyncSelect(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSACancelBlockingCall(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSACleanup(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSACloseEvent(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSAConnect(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSACreateEvent(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSADuplicateSocketA(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSADuplicateSocketW(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSAEnumNetworkEvents(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSAEnumProtocolsA(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSAEnumProtocolsW(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSAEventSelect(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSAGetLastError(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSAGetOverlappedResult(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSAGetQOSByName(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSAHtonl(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSAHtons(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSAIoctl(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSAIsBlocking(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSAJoinLeaf(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSANtohl(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSANtohs(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSARecv(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSARecvDisconnect(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSARecvFrom(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSAResetEvent(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSASend(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSASendDisconnect(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSASendTo(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSASetBlockingHook(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSASetEvent(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSASetLastError(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSASocketA(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSASocketW(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSAStartup(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSAUnhookBlockingHook(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSAWaitForMultipleEvents(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_gethostbyaddr(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_gethostbyname(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_gethostname(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_getprotobyname(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_getprotobynumber(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_getservbyname(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_getservbyport(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_inet_addr(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_inet_ntoa(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSAAsyncGetHostByAddr(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSAAsyncGetHostByName(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSAAsyncGetProtoByName(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSAAsyncGetProtoByNumber(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSAAsyncGetServByName(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSAAsyncGetServByPort(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSACancelAsyncRequest(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSPAccept(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSPAsyncSelect(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSPBind(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSPCancelBlockingCall(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSPCleanup(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSPCloseSocket(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSPConnect(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSPDuplicateSocket(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSPEnumNetworkEvents(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSPEventSelect(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSPGetOverlappedResult(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSPGetPeerName(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);


BOOL CALLBACK
DTHandler_WSPGetSockName(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSPGetSockOpt(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSPGetQOSByName(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSPIoctl(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSPJoinLeaf(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSPListen(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSPRecv(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSPRecvDisconnect(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSPRecvFrom(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSPSelect(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSPSend(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSPSendDisconnect(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSPSendTo(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSPSetSockOpt(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSPShutdown(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSPSocket(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSPStartup(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WPUCloseEvent(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WPUCloseSocketHandle(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WPUCreateEvent(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WPUCreateSocketHandle(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSCDeinstallProvider(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSCInstallProvider(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WPUModifyIFSHandle(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WPUQueryBlockingCallback(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WPUQuerySocketHandleContext(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WPUQueueApc(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WPUResetEvent(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WPUSetEvent(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WPUFDIsSet(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WPUGetProviderPath(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WPUPostMessage(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler___WSAFDIsSet(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);

BOOL CALLBACK
DTHandler_WSCEnumProtocols(
    IN     va_list vl,
    IN OUT LPVOID  ReturnValue,
    IN     LPSTR   LibraryName,
    OUT    char    *Buffer,
    IN     int     Index,
    IN     int     BufLen,
    IN     BOOL    PreOrPost);
