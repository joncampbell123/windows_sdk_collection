
/*
   LSA Sample program.

   Give an account the right to logon  as a service.
*/

#ifndef UNICODE
#define UNICODE
#endif // UNICODE

#include <windows.h>
#include <ntsecapi.h>


#include <stdio.h>


void
InitLsaString(
    PLSA_UNICODE_STRING LsaString,
    LPWSTR String
    )
{
    DWORD StringLength;
    if (String == NULL)
    {
        LsaString->Buffer = NULL;
        LsaString->Length = 0;
        LsaString->MaximumLength = 0;
        return;
    }

    StringLength = wcslen(String);
    LsaString->Buffer = String;
    LsaString->Length = (USHORT) StringLength * sizeof(WCHAR);
    LsaString->MaximumLength = (USHORT) (StringLength + 1) * sizeof(WCHAR); 
}

DWORD
OpenPolicy(
    LPWSTR ServerName,
    DWORD DesiredAccess,
    PLSA_HANDLE PolicyHandle
    )
{
    DWORD Error;
    LSA_OBJECT_ATTRIBUTES ObjectAttributes;
    LSA_UNICODE_STRING ServerString;
    PLSA_UNICODE_STRING Server = NULL;

    //
    // Always initialize the object attributse to all zeroes
    //

    ZeroMemory(
        &ObjectAttributes,
        sizeof(ObjectAttributes)
        );

    if (ServerName != NULL)
    {
        //
        // Make a LSA_UNICODE_STRING out of the LPWSTR passed in
        //
    
        InitLsaString(
            &ServerString,
            ServerName
            );
        Server = &ServerString;
    
    }
    //
    // Attempt to open the policy for all access
    //

    Error = LsaNtStatusToWinError(
                LsaOpenPolicy(
                    Server,
                    &ObjectAttributes,
                    DesiredAccess,
                    PolicyHandle
                    ) );
    if (Error != 0)
    {
        printf("Failed to open policy: error 0x%x\n",Error);
    }

    return(Error);


}



PSID
GetAccountSid(
    LSA_HANDLE PolicyHandle,
    LPWSTR AccountName
    )
{
    PLSA_TRANSLATED_SID TranslatedSid;
    PSID Sid;
    PSID DomainSid;
    PLSA_REFERENCED_DOMAIN_LIST Domains;
    DWORD Error;
    LSA_UNICODE_STRING AccountString;
    DWORD NewSidLength;
    DWORD SubAuthorityCount;

    //
    // Convert the string to a LSA_UNICODE_STRING
    //

    InitLsaString(
        &AccountString,
        AccountName
        );

    //
    // Call the LSA to lookup the name
    //

    Error = LsaNtStatusToWinError(
                LsaLookupNames(
                    PolicyHandle,
                    1,
                    &AccountString,
                    &Domains,
                    &TranslatedSid
                    ) );
    if (Error != 0)
    {
        printf("Could not map name %ws\n",AccountName);
        return(NULL);
    }

    //
    // Build a SID from the Domain SID and account RID
    //

    DomainSid = Domains->Domains[TranslatedSid->DomainIndex].Sid;
    //
    // Compute the length of the new SID.  This is the length required for the
    // number of subauthorities in the domain sid plus one for the user RID.
    //

    SubAuthorityCount = *GetSidSubAuthorityCount(DomainSid);
    NewSidLength = GetSidLengthRequired( (UCHAR) (SubAuthorityCount + 1) );

    Sid = LocalAlloc(0,NewSidLength);

    if (Sid == NULL)
    {
        LsaFreeMemory(Domains);
        LsaFreeMemory(TranslatedSid);
        return(NULL);
    }

    //
    // Build the SID by copying the domain SID and, increasing the sub-
    // authority count in the new sid by one, and setting the last
    // subauthority to be the relative id of the user.
    //

    CopySid(
        NewSidLength,
        Sid,
        DomainSid
        );


    *GetSidSubAuthorityCount(Sid) = (UCHAR) SubAuthorityCount + 1;
    *GetSidSubAuthority(Sid, SubAuthorityCount) = TranslatedSid->RelativeId;
    LsaFreeMemory(Domains);
    LsaFreeMemory(TranslatedSid);

    return(Sid);
            
}

DWORD
AddUserRightToAccount(
    LSA_HANDLE PolicyHandle,
    LPWSTR AccountName,
    LPWSTR UserRightName
    )
{
    DWORD Error;
    PSID AccountSid;
    LSA_UNICODE_STRING UserRightString;

    AccountSid = GetAccountSid(
            PolicyHandle,
            AccountName
            );
    if (AccountSid == NULL)
    {
        printf("Failed to get account sid\n");
        return(0xc0000001);
    }

    //
    // Create a LSA_UNICODE_STRING for the right name
    //

    InitLsaString(
        &UserRightString,
        UserRightName
        );

    Error = LsaNtStatusToWinError(
                LsaAddAccountRights(
                    PolicyHandle,
                    AccountSid,
                    &UserRightString,
                    1
                    ) );
    if (Error != 0)
    {
        printf("Failed to add user right: 0x%x\n",Error);
    }

    LocalFree(AccountSid);
    return(Error);
            

}



void _cdecl
main(int argc, char *argv[])
{
    DWORD Error;
    LSA_HANDLE PolicyHandle = NULL;

    Error = OpenPolicy(NULL, POLICY_ALL_ACCESS,&PolicyHandle);
    if (Error != 0)
    {
        return;
    }


    Error = AddUserRightToAccount(
                PolicyHandle,
                TEXT("SampleUser"),
                SE_SERVICE_LOGON_NAME
                );


}

