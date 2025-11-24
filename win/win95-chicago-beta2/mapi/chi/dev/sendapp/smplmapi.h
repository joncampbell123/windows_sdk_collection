/*
 -  S M P L M A P I . H
 -  Copyright (C) 1994 Microsoft Corporation
 -
 *  Purpose:
 *      Header for SMPLMAPI.CPP
 *
 */

extern BOOL InitMapiDll();
extern void DeinitSimpleMAPI(void);

typedef ULONG (FAR PASCAL *LPFNMAPILOGON)(ULONG ulUIParam, LPSTR lpszProfileName,
    LPSTR lpszPassword, FLAGS flFlags, ULONG ulReserved, LPLHANDLE lplhSession);

typedef ULONG (FAR PASCAL *LPFNMAPILOGOFF)(LHANDLE lhSession, ULONG ulUIParam, 
    FLAGS flFlags, ULONG ulReserved);

typedef ULONG (FAR PASCAL *LPFNMAPISENDMAIL)(LHANDLE lhSession, ULONG ulUIParam,
    lpMapiMessage lpMessage, FLAGS flFlags, ULONG ulReserved);

typedef ULONG (FAR PASCAL *LPFNMAPISENDDOCUMENTS)(ULONG ulUIParam, LPSTR lpszDelimChar,
    LPSTR lpszFilePaths, LPSTR lpszFileNames, ULONG ulReserved);

typedef ULONG (FAR PASCAL *LPFNMAPIFINDNEXT)(LHANDLE lhSession, ULONG ulUIParam,
    LPSTR lpszMessageType, LPSTR lpszSeedMessageID, FLAGS flFlags,
    ULONG ulReserved, LPSTR lpszMessageID);

typedef ULONG (FAR PASCAL *LPFNMAPIREADMAIL)(LHANDLE lhSession, ULONG ulUIParam,
    LPSTR lpszMessageID, FLAGS flFlags, ULONG ulReserved,
    lpMapiMessage FAR *lppMessage);

typedef ULONG (FAR PASCAL *LPFNMAPISAVEMAIL)(LHANDLE lhSession, ULONG ulUIParam,
    lpMapiMessage lpMessage, FLAGS flFlags, ULONG ulReserved,
    LPSTR lpszMessageID);

typedef ULONG (FAR PASCAL *LPFNMAPIDELETEMAIL)(LHANDLE lhSession, ULONG ulUIParam,
    LPSTR lpszMessageID, FLAGS flFlags, ULONG ulReserved);

typedef ULONG (FAR PASCAL *LPFNMAPIFREEBUFFER)(LPVOID pv);

typedef ULONG (FAR PASCAL *LPFNMAPIADDRESS)(LHANDLE lhSession, ULONG ulUIParam,
    LPSTR lpszCaption, ULONG nEditFields, LPSTR lpszLabels, ULONG nRecips,
    lpMapiRecipDesc lpRecips, FLAGS flFlags, ULONG ulReserved,
    LPULONG lpnNewRecips, lpMapiRecipDesc FAR *lppNewRecips);

typedef ULONG (FAR PASCAL *LPFNMAPIDETAILS)(LHANDLE lhSession, ULONG ulUIParam,
    lpMapiRecipDesc lpRecip, FLAGS flFlags, ULONG ulReserved);

typedef ULONG (FAR PASCAL *LPFNMAPIRESOLVENAME)(LHANDLE lhSession, ULONG ulUIParam,
    LPSTR lpszName, FLAGS flFlags, ULONG ulReserved,
    lpMapiRecipDesc FAR *lppRecip);

extern LPFNMAPILOGON lpfnMAPILogon;
extern LPFNMAPILOGOFF lpfnMAPILogoff;
extern LPFNMAPISENDMAIL lpfnMAPISendMail;
extern LPFNMAPISENDDOCUMENTS lpfnMAPISendDocuments;
extern LPFNMAPIFINDNEXT lpfnMAPIFindNext;
extern LPFNMAPIREADMAIL lpfnMAPIReadMail;
extern LPFNMAPISAVEMAIL lpfnMAPISaveMail;
extern LPFNMAPIDELETEMAIL lpfnMAPIDeleteMail;
extern LPFNMAPIFREEBUFFER lpfnMAPIFreeBuffer;
extern LPFNMAPIADDRESS lpfnMAPIAddress;
extern LPFNMAPIDETAILS lpfnMAPIDetails;
extern LPFNMAPIRESOLVENAME lpfnMAPIResolveName;

#undef MAPILogon
#undef MAPILogoff
#undef MAPISendMail
#undef MAPISendDocuments
#undef MAPIFindNext
#undef MAPIReadMail
#undef MAPISaveMail
#undef MAPIDeleteMail
#undef MAPIFreeBuffer
#undef MAPIAddress
#undef MAPIDetails
#undef MAPIResolveName

#define MAPILogon           (*lpfnMAPILogon)
#define MAPILogoff          (*lpfnMAPILogoff)
#define MAPISendMail        (*lpfnMAPISendMail)
#define MAPISendDocuments   (*lpfnMAPISendDocuments)
#define MAPIFindNext        (*lpfnMAPIFindNext)
#define MAPIReadMail        (*lpfnMAPIReadMail)
#define MAPISaveMail        (*lpfnMAPISaveMail)
#define MAPIDeleteMail      (*lpfnMAPIDeleteMail)
#define MAPIFreeBuffer      (*lpfnMAPIFreeBuffer)
#define MAPIAddress         (*lpfnMAPIAddress)
#define MAPIDetails         (*lpfnMAPIDetails)
#define MAPIResolveName     (*lpfnMAPIResolveName)


