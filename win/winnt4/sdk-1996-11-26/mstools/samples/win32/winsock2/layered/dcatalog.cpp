/*++

     Copyright (c) 1996 Intel Corporation
     Copyright (c) 1996 Microsoft Corporation
     All Rights Reserved
     
     Permission is granted to use, copy and distribute this software and 
     its documentation for any purpose and without fee, provided, that 
     the above copyright notice and this statement appear in all copies. 
     Intel makes no representations about the suitability of this 
     software for any purpose.  This software is provided "AS IS."  
     
     Intel specifically disclaims all warranties, express or implied, 
     and all liability, including consequential and other indirect 
     damages, for the use of this software, including liability for 
     infringement of any proprietary rights, and including the 
     warranties of merchantability and fitness for a particular purpose. 
     Intel does not assume any responsibility for any errors which may 
     appear in this software nor any responsibility to update it.

     
Module Name:

    dcatalog.cpp

Abstract:

    This module contains the implementation of the dcatalog class. This class
    maintains a catalog of installed WinSock2 service providers.

Author:

    bugs@brandy.jf.intel.com
    
Revision History:

--*/

#include "precomp.h"


DCATALOG::DCATALOG()
/*++

Routine Description:

    Constructor for the DCATALOG object. Set member variables to known
    state. Initialization of the object is completed in Initialize().

Arguments:

    NONE.

Return Value:

    NONE.

--*/
{

    m_num_items = 0;
    m_catalog_initialized = FALSE;

    // initialize the critical section object
    InitializeCriticalSection( &m_catalog_lock );
    InitializeListHead( &m_protocol_list );
}

INT
DCATALOG::Initialize(
    )
/*++

Routine Description:

    Initialization routine for the DCATALOG object. Completes the
    initialization of the DCATALOG object.  This MUST be the first member
    fuction called after a DCATALOG object is created.

Arguments:

    NONE.

Return Value:

    NO_ERROR if the fuction succeeds else a winsock2 error code.

--*/
{
    LPWSAPROTOCOL_INFOW   ProtocolInfoBuff = NULL;
    DWORD                ProtocolInfoBuffSize = 0;
    PPROTO_CATALOG_ITEM  CatalogItem;
    INT                  ReturnCode;
    INT                  ErrorCode;
    INT                  EnumResult;
    INT                  Index;
    
    // Call WSCEnumProtocols with a zero length buffer so we know what size to
    // send in to get all the installed PROTOCOL_INFO structs.
    WSCEnumProtocols(
        NULL,                     // lpiProtocols
        ProtocolInfoBuff,         // lpProtocolBuffer
        & ProtocolInfoBuffSize,   // lpdwBufferLength
        & ErrorCode);             // lpErrno

    ReturnCode = WSA_NOT_ENOUGH_MEMORY;
    ProtocolInfoBuff = (LPWSAPROTOCOL_INFOW)new char[ProtocolInfoBuffSize];
    if (ProtocolInfoBuff){
        EnumResult = WSCEnumProtocols(
            NULL,                     // lpiProtocols
            ProtocolInfoBuff,         // lpProtocolBuffer
            & ProtocolInfoBuffSize,   // lpdwBufferLength
            & ErrorCode);
        
        ReturnCode = WSASYSNOTREADY;
        if (EnumResult != SOCKET_ERROR){
            for (Index=0; Index < EnumResult ; Index++){

                //Create a new catalog item for the PROTOCOL_INFO struct.
                CatalogItem = new PROTO_CATALOG_ITEM;
                if (CatalogItem){
                    
                    ReturnCode = CatalogItem->Initialize(
                        &ProtocolInfoBuff[Index]);
                    if (NO_ERROR == ReturnCode){

                        //Add the new catalog item to the catalog
                        AcquireCatalogLock();
                        AppendCatalogItem(
                            CatalogItem);
                        ReleaseCatalogLock();
                    } //if
                    else{
                        break;
                    } //else
                } //if
            } //for
        } //if
        delete(ProtocolInfoBuff);
    } //if
    return(ReturnCode);
}




DCATALOG::~DCATALOG()
/*++

Routine Description:

    This  function  destroys the catalog object.  It takes care of removing and
    destroying  all  of  the  catalog  entries  in  the catalog.  This includes
    destroying  all  of the DPROVIDER objects referenced by the catalog.  It is
    the  caller's responsibility to make sure that the DPROVIDER objects are no
    longer referenced.

Arguments:

    None

Return Value:

    None

Implementation Notes:

    for each catalog entry
        remove the entry
        get its DPROVIDER reference
        if reference is non-null
            Set providers for all entries with matching IDs null
            destroy the DPROVIDER
        endif
        destroy the entry
    end for
    deallocate the list head
    close the catalog registry mutex
--*/
{
    PLIST_ENTRY  this_linkage;
    PPROTO_CATALOG_ITEM  this_item;
    PDPROVIDER  this_provider;

    DEBUGF(
        DBG_TRACE,
        ("Catalog destructor\n"));

    AcquireCatalogLock();

    while ((this_linkage = m_protocol_list.Flink) != & m_protocol_list) {
        this_item = CONTAINING_RECORD(
            this_linkage,        // address
            PROTO_CATALOG_ITEM,  // type
            m_CatalogLinkage     // field
            );
        RemoveCatalogItem(
            this_item  // CatalogItem
            );
        this_provider = this_item->GetProvider();
        delete this_provider;
        delete this_item;
    }  // while (get entry linkage)

    assert( m_num_items == 0 );
    
    ReleaseCatalogLock();
    DeleteCriticalSection( &m_catalog_lock );

}  // ~DCATALOG




VOID
DCATALOG::EnumerateCatalogItems(
    IN CATALOGITERATION  Iteration,
    IN DWORD             PassBack
    )
/*++

Routine Description:

    This  procedure enumerates all of the DPROTO_CATALOG_ITEM structures in the
    catalog  by  calling  the indicated iteration procedure once for each item.
    The called procedure can stop the iteration early by returning FALSE.

    Note  that  the DPROVIDER associated with an enumerated DPROTO_CATALOG_ITEM
    may  be  NULL.   To retrieve DPROTO_CATALOG_ITEM structure that has had its
    DPROVIDER      loaded      and      initialized,      you      can      use
    GetCatalogItemFromCatalogEntryId.

Arguments:

    Iteration - Supplies   a  reference  to  the  catalog  iteration  procedure
                supplied by the client.

    PassBack  - Supplies  a  value uninterpreted by this procedure.  This value
                is  passed  unmodified to the catalog iteration procedure.  The
                client can use this value to carry context between the original
                call site and the iteration procedure.

Return Value:

    None
--*/
{
    PLIST_ENTRY         ListMember;
    PPROTO_CATALOG_ITEM CatalogEntry;
    BOOL                enumerate_more;

    assert(Iteration != NULL);

    enumerate_more = TRUE;

    AcquireCatalogLock();

    ListMember = m_protocol_list.Flink;

    while (enumerate_more && (ListMember != & m_protocol_list)) {
        CatalogEntry = CONTAINING_RECORD(
            ListMember,
            PROTO_CATALOG_ITEM,
            m_CatalogLinkage);
        ListMember = ListMember->Flink;
        enumerate_more = (* Iteration) (
            PassBack,     // PassBack
            CatalogEntry  // CatalogEntry
            );
    } //while

    ReleaseCatalogLock();

}  // EnumerateCatalogItems



INT
DCATALOG::FindNextProviderInChain(
        IN  LPWSAPROTOCOL_INFOW lpLocalProtocolInfo,
        OUT LPDWORD             LocalProviderCatalogEntryId,
        OUT LPDWORD             NextProviderCatalogEntryId
    )
/*++

Routine Description:

    This  procedure find the catalog entry ID for the provider below this
    provider in the protocol chain.
    
Arguments:
    lpLocalProtocolInfo - A pointer to the WSAPROTOCOL_INFO struct for the
                          current protocol chain.

    LocalCatalogEntryId - A pointer to a DWORD for this provider in the
                          protocol chain.                       
Return Value:

    NO_ERROR if the next provider is located else a valid winsock2 error
    code.
--*/
{
    // ******
    // NOTE:
    // This define is used to distinguish our PROTOCOL_INFO from the others
    // installed on the system. The signature should have been in the
    // dwProviderReserved field of the PROTOCOL_INFO when the this provider was
    // installed.  This needs to change when this provider is updated to 2.2.0
    // or later where there will be a GUID in the PROTOCOL_INFO struct for this
    // purpose.
    
#define PROVIDER_SIGNATURE 0xdeadbabe
    
    PLIST_ENTRY         ListMember;
    PPROTO_CATALOG_ITEM CatalogEntry;
    LPWSAPROTOCOL_INFOW ProtocolInfo;
    INT                 Index;
    BOOL                Continue =FALSE;
    INT                 ReturnValue =WSAEFAULT;
    
    AcquireCatalogLock();

    //
    // Find our PROTOCOL_INFO
    //
    ListMember = m_protocol_list.Flink;

    while (ListMember != & m_protocol_list) {
        CatalogEntry = CONTAINING_RECORD(
            ListMember,
            PROTO_CATALOG_ITEM,
            m_CatalogLinkage);
        ListMember = ListMember->Flink;
        ProtocolInfo = CatalogEntry->GetProtocolInfo();
        if (ProtocolInfo->dwProviderReserved == PROVIDER_SIGNATURE ){
            *LocalProviderCatalogEntryId = ProtocolInfo->dwCatalogEntryId;
            Continue = TRUE;
            break;
        } //if
    } //while

    if (Continue){
        //
        // Get the next providers CatalogEntryId from the protocol chain
        //
        for (Index=0;
             Index < lpLocalProtocolInfo->ProtocolChain.ChainLen;
             Index++){
            if (lpLocalProtocolInfo->ProtocolChain.ChainEntries[Index] ==
                *LocalProviderCatalogEntryId){
                *NextProviderCatalogEntryId =
                    lpLocalProtocolInfo->ProtocolChain.ChainEntries[Index+1];
                ReturnValue = NO_ERROR;
                break;
            } //if
        } //for
    } //if
    ReleaseCatalogLock();
    return(ReturnValue);
}



INT
DCATALOG::GetCatalogItemFromCatalogEntryId(
    IN  DWORD                     CatalogEntryId,
    OUT PPROTO_CATALOG_ITEM FAR * CatalogItem
    )
/*++

Routine Description:

    This  procedure  retrieves  a  reference  to a catalog item given a catalog
    entry ID to search for.

    The operation takes care of creating, initializing, and setting a DPROVIDER
    object  for the retrieved catalog item if necessary.  This includes setting
    the DPROVIDER object in all catalog entries for the same provider.

Arguments:

    CatalogEntryId  - Supplies The ID of a catalog entry to be searched for.

    CatalogItem     - Returns a reference to the catalog item with the matching
                      catalog entry ID if it is found, otherwise returns NULL.

Return Value:

  The  function  returns  NO_ERROR  if  successful, otherwise it returns an
  appropriate WinSock error code.
--*/
{
    PLIST_ENTRY         ListMember;
    INT                 ReturnCode=SOCKET_ERROR;
    PPROTO_CATALOG_ITEM CatalogEntry;
    BOOL                Found=FALSE;
    PDPROVIDER          LocalProvider;

    assert(CatalogItem != NULL);

    // Prepare for early error return
    * CatalogItem = NULL;

    AcquireCatalogLock();

    ListMember = m_protocol_list.Flink;

    while (! Found && (ListMember != & m_protocol_list)) {
        
        CatalogEntry = CONTAINING_RECORD(
            ListMember,
            PROTO_CATALOG_ITEM,
            m_CatalogLinkage);
        ListMember = ListMember->Flink;
        if (CatalogEntry->GetProtocolInfo()->dwCatalogEntryId ==
            CatalogEntryId) {
            
            Found = TRUE;
            if (CatalogEntry->GetProvider() == NULL) {
                
                ReturnCode = LoadProvider(
                    CatalogEntry,    // CatalogEntry
                    & LocalProvider  // Provider
                    );
                if (ReturnCode != NO_ERROR) {
                    
                    DEBUGF(
                        DBG_ERR,
                        ("Error (%lu) loading chosen provider\n",
                        ReturnCode));
                    ReleaseCatalogLock();
                    return ReturnCode;
                }
                CatalogEntry->SetProvider(
                    LocalProvider);
                
            }  // if provider is NULL
            * CatalogItem = CatalogEntry;
            ReturnCode = NO_ERROR;
        } //if
    } //while

    // If we could not find a matching catalog entry Id
    if (!Found) {
        * CatalogItem = NULL;
        ReturnCode = WSAEINVAL;
    } //if

    ReleaseCatalogLock();
    return(ReturnCode);
}  // GetCatalogItemFromCatalogEntryId



INT
DCATALOG::LoadProvider(
    IN PPROTO_CATALOG_ITEM CatalogEntry,
    OUT PDPROVIDER FAR* Provider
    )
/*++

Routine Description:

    Load   the   provider  described  by  CatalogEntry. 

Arguments:

    CatalogEntry - Supplies  a reference to a protocol catalog entry describing
                   the provider to load.

    Provider     - Returns a reference to the newly loaded provider object.

Return Value:

    The  function  returns NO_ERROR if successful, otherwise it returns an
    appropriate WinSock error code.
--*/
{
    INT ReturnCode = WSA_NOT_ENOUGH_MEMORY;
    PDPROVIDER LocalProvider;

    assert(CatalogEntry != NULL);
    assert(Provider != NULL);

    *Provider = NULL;

    LocalProvider = new(DPROVIDER);
    if (LocalProvider) {

        ReturnCode = LocalProvider->Initialize(
            CatalogEntry->GetLibraryPath(),
            CatalogEntry->GetProtocolInfo()
            );
        if (NO_ERROR == ReturnCode) {
            *Provider = LocalProvider;
        } //if
        else {
            delete(LocalProvider);
        } //else
    } //if
    return(ReturnCode);
}  // LoadProvider



VOID
DCATALOG::AppendCatalogItem(
    IN  PPROTO_CATALOG_ITEM  CatalogItem
    )
/*++

Routine Description:

    This procedure appends a catalog item to the end of the (in-memory) catalog
    object.   It becomes the last item in the catalog.

Arguments:

    CatalogItem - Supplies a reference to the catalog item to be added.

Return Value:

    None
--*/
{
    assert(CatalogItem != NULL);

    InsertTailList(
        & m_protocol_list,               // ListHead
        & CatalogItem->m_CatalogLinkage  // Entry
       );
    m_num_items++;
}  // AppendCatalogItem



VOID
DCATALOG::RemoveCatalogItem(
    IN  PPROTO_CATALOG_ITEM  CatalogItem
    )
/*++

Routine Description:

    This  procedure removes a catalog item from the (in-memory) catalog object.
    The catalog information in the registry is NOT updated.

Arguments:

    CatalogItem - Supplies a reference to the catalog item to be removed.

Return Value:

    None
--*/
{
    assert(CatalogItem != NULL);

    RemoveEntryList(
        & CatalogItem->m_CatalogLinkage  // Entry
        );
    assert(m_num_items > 0);
    m_num_items--;
}  // RemoveCatalogItem
