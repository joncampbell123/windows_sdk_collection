/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
****************************************************************************/

#include <basedef.h>
#include <vmm.h>

typedef	DWORD			DEVNODE;	// Devnode.

//
// block used for references...
//

typedef struct _REFERENCE {
    NDIS_SPIN_LOCK SpinLock;    //  4 Bytes
    USHORT ReferenceCount;      //  2 Bytes
    BOOLEAN Closing;            //  1 Byte
    UCHAR   Reserved;           //  1 Byte Padding
} REFERENCE, * PREFERENCE;

typedef struct _WRAPPER_MAC_BLOCK      WRAPPER_MAC_BLOCK, * PWRAPPER_MAC_BLOCK;
typedef struct _WRAPPER_ADAPTER_BLOCK  WRAPPER_ADAPTER_BLOCK, * PWRAPPER_ADAPTER_BLOCK;
typedef struct _WRAPPER_PROTOCOL_BLOCK WRAPPER_PROTOCOL_BLOCK, * PWRAPPER_PROTOCOL_BLOCK;
typedef struct _WRAPPER_OPEN_BLOCK     WRAPPER_OPEN_BLOCK, * PWRAPPER_OPEN_BLOCK;
typedef struct _MODULE_LIST             MODULE_LIST, *PMODULE_LIST;


struct _MODULE_LIST {
    NDIS_STRING nsModuleName;
    PMODULE_LIST pNext;
};

//
// one of these per MAC
//

struct _WRAPPER_MAC_BLOCK {
    UINT Signature;                     // debugging signature NMAC  
    PWRAPPER_MAC_BLOCK pNext;              // Used by System for queing all macs
    PWRAPPER_ADAPTER_BLOCK AdapterQueue;   // queue of adapters for this MAC
    REFERENCE Ref;                      // contains spinlock for AdapterQueue
    UINT Length;                        // of this WRAPPER_MAC_BLOCK structure
    NDIS_HANDLE MacMacContext;          // Mac handle (added 4/18 GJOY)
    NDIS_HANDLE NdisWrapperHandle;      //Added 8/25/93 AviB
    DWORD  dwPnpFlags;
    UINT nLoadCount;
    MODULE_LIST ModuleList;             // list of the LE module names for this mac
    BOOLEAN IsWidget;                    // Is this a widget driver?
    BOOLEAN Reserved;                    // to force function table in MacCharacteristics to be dword aligned
    NDIS_MAC_CHARACTERISTICS MacCharacteristics;    // handler addresses
};


//
// one of these per adapter registered on a MAC
//

// WARNING: AdapterInfo structure must follow the ADAPTER_BLOCK contiguously in
//          memory.  If not, it will mess up the NdisEnumAdapters routine.


struct _WRAPPER_ADAPTER_BLOCK {
    UINT Signature;                     //Debugging Signature NADP
    PWRAPPER_ADAPTER_BLOCK pNext;          // Used by Wrapper
    PWRAPPER_MAC_BLOCK MacHandle;          // pointer to our MAC block
    NDIS_HANDLE MacAdapterContext;      // context when calling MacOpenAdapter
    NDIS_STRING AdapterName;            // adapter name string
    PWRAPPER_OPEN_BLOCK OpenQueue;         // queue of opens for this adapter
    PWRAPPER_ADAPTER_BLOCK NextAdapter;    // used by MAC's AdapterQueue
    REFERENCE Ref;                      // contains spinlock for OpenQueue
    ADAPTER_SHUTDOWN_HANDLER ShutdownHandler;
    PVOID ShutdownContext;
    NDIS_HANDLE DmaHandle;              // handle to DMA channel (if needed)
    PNDIS_ADAPTER_INFORMATION AdapterInfoP;

    /* PnP stuff */
    DEVNODE dnDevNode;                  // Device node passed fron CM
    UINT bDriver;                       // type of driver Flag (STATIC, DYN, LOAD_SYN)
    UINT dwPnpFlags;                    // PnP flags (Removed,...)
    NDIS_STRING nsMacName;              // Mac Name
    NDIS_STRING nsAdapterRegName;       // Adapter name in registry
//    NDIS_STRING nsNdis2Name;            // Adapter's NDIS2 name
//    NDIS_STRING nsOdiName;              // Adapter's ODI name
};

typedef struct _WRAPPER_BIND_UNBIND_CONTEXT{
    NDIS_STATUS     Status;
    NDIS_STATUS     ErrorStatus;
    UINT            Complete;
    } WRAPPER_BIND_UNBIND_CONTEXT, *PWRAPPER_BIND_UNBIND_CONTEXT;
    

//
// one of these per open on an adapter/protocol
//

struct _WRAPPER_OPEN_BLOCK {
    UINT Signature;                         // signature for debugging
    PWRAPPER_MAC_BLOCK MacHandle;           // pointer to our MAC
    NDIS_HANDLE MacBindingHandle;           // context when calling MacXX funcs
    PWRAPPER_ADAPTER_BLOCK AdapterHandle;   // pointer to our adapter
    DEVNODE dnAdapterNode;                  // Adapter Dev node
    PWRAPPER_PROTOCOL_BLOCK ProtocolHandle; // pointer to our protocol
    DEVNODE dnProtocolNode;                 // Protocol Dev Node
    NDIS_HANDLE ProtocolBindingContext;     // context when calling ProtXX funcs
    PWRAPPER_BIND_UNBIND_CONTEXT BindUnbindContext; // context when calling ProtocolBind/Unbind
    PWRAPPER_OPEN_BLOCK AdapterNextOpen;    // used by adapter's OpenQueue
    PWRAPPER_OPEN_BLOCK ProtocolNextOpen;   // used by protocol's OpenQueue
    BOOLEAN Closing;                        // TRUE when removing this struct
    NDIS_HANDLE CloseRequestHandle;         // 0 indicates an internal close
    NDIS_SPIN_LOCK SpinLock;                // guards Closing
    DWORD dwPnpFlags;   
};

//
// one of these per protocol registered
//

struct _WRAPPER_PROTOCOL_BLOCK {
    UINT Signature;                 	// debug Signature NPRT
    PWRAPPER_PROTOCOL_BLOCK pNext;      // Used by Wrapper
    PWRAPPER_OPEN_BLOCK OpenQueue;      // queue of opens for this protocol
    REFERENCE Ref;                      // contains spinlock for OpenQueue
    UINT Length;                        // of this WRAPPER_PROTOCOL_BLOCK struct
    UINT nLoadCount;
    MODULE_LIST ModuleList;             // list of the LE module names for this Protocol
    DEVNODE dnCurrentDevNode;           // Current Dev Node for Protocol
    USHORT Reserved;                    // to force function table in ProtocolCharacteristics to be dword aligned
    NDIS_PROTOCOL_CHARACTERISTICS ProtocolCharacteristics;  // handler addresses
};

#define FilterIndicateReceive	NdisIndicateReceive
#define FilterIndicateReceiveComplete	NdisIndicateReceiveComplete
#define NdisDereferenceProtocol(ProtP) { \
    if (DereferenceRef(&(ProtP)->Ref)) { \
        C_HeapFree((UINT)(ProtP),0); \
    } \
}
extern VOID
AllocateString(
    PNDIS_STRING dest,
    PUCHAR src
    );
#define INTERNAL_CDECL __cdecl
INTERNAL
UINT INTERNAL_CDECL
C_PageAllocate(
    UINT nPages,
    UINT pType,
    UINT VMHandle,
    UINT AlignMask,
    UINT minPhys,
    UINT maxPhys,
    UINT *PhysAddrPTR,
    UINT flags,
    UINT *handle
    ); // Returns the linear address of the allocated block

INTERNAL
UINT INTERNAL_CDECL
C_PageFree(
    UINT hmem,
    UINT flags
    );

INTERNAL
UINT INTERNAL_CDECL
C_HeapAllocate(
    UINT nbytes,
    UINT flags
    ); // Returns the linear address of the allocated block

INTERNAL
UINT INTERNAL_CDECL
C_HeapFree(
    UINT hAddress,
    UINT flags
    );

INTERNAL
UINT INTERNAL_CDECL NdisGetPhysPageInfo(UINT, UINT, UINT);
