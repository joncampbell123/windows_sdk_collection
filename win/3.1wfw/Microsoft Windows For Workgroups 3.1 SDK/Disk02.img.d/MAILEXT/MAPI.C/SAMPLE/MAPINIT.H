#define SZ_MAPILOGON "MAPILogon"
#define SZ_MAPILOGOFF "MAPILogoff"
#define SZ_MAPISENDMAIL "MAPISendMail"
#define SZ_MAPISENDDOC "MAPISendDocuments"
#define SZ_MAPIFINDNEXT "MAPIFindNext"
#define SZ_MAPIREADMAIL "MAPIReadMail"
#define SZ_MAPISAVEMAIL "MAPISaveMail"
#define SZ_MAPIDELMAIL "MAPIDeleteMail"
#define SZ_MAPIFREEBUFFER "MAPIFreeBuffer"
#define SZ_MAPIADDRESS "MAPIAddress"
#define SZ_MAPIDETAILS "MAPIDetails"
#define SZ_MAPIRESOLVENAME "MAPIResolveName"
#define MAPIDLL "MAPI.DLL"

#define ERR_LOAD_LIB  0x02
#define ERR_LOAD_FUNC 0x04

#ifdef MAIN

ULONG (FAR PASCAL *lpfnMAPILogon)(ULONG, LPSTR, LPSTR, FLAGS, ULONG, LPLHANDLE);

ULONG (FAR PASCAL *lpfnMAPILogoff)(LHANDLE, ULONG, FLAGS,ULONG);

ULONG (FAR PASCAL *lpfnMAPISendMail)(LHANDLE, ULONG, lpMapiMessage, FLAGS,
                                     ULONG);

ULONG (FAR PASCAL *lpfnMAPISendDocuments)(ULONG, LPSTR, LPSTR, LPSTR, ULONG);

ULONG (FAR PASCAL *lpfnMAPIFindNext)(LHANDLE, ULONG, LPSTR, LPSTR, FLAGS,
                                     ULONG, LPSTR);

ULONG (FAR PASCAL *lpfnMAPIReadMail)(LHANDLE, ULONG, LPSTR, FLAGS, ULONG,
                                     lpMapiMessage FAR *);

ULONG (FAR PASCAL *lpfnMAPISaveMail)(LHANDLE, ULONG, lpMapiMessage, FLAGS,
                                     ULONG, LPSTR);

ULONG (FAR PASCAL *lpfnMAPIDeleteMail)(LHANDLE, ULONG, LPSTR, FLAGS, ULONG);

ULONG (FAR PASCAL *lpfnMAPIFreeBuffer)(LPVOID);

ULONG (FAR PASCAL *lpfnMAPIAddress)(LHANDLE, ULONG, LPSTR, ULONG, LPSTR,
					                ULONG, lpMapiRecipDesc, FLAGS, ULONG,
                                    LPULONG, lpMapiRecipDesc FAR *);

ULONG (FAR PASCAL *lpfnMAPIDetails)(LHANDLE, ULONG,lpMapiRecipDesc, FLAGS,
                                    ULONG);

ULONG (FAR PASCAL *lpfnMAPIResolveName)(LHANDLE, ULONG, LPSTR, FLAGS,
						                ULONG, lpMapiRecipDesc FAR *);
#else
extern ULONG (FAR PASCAL *lpfnMAPILogon)(ULONG, LPSTR, LPSTR, FLAGS, ULONG,
                                         LPLHANDLE);

extern ULONG (FAR PASCAL *lpfnMAPILogoff)(LHANDLE, ULONG, FLAGS,ULONG);

extern ULONG (FAR PASCAL *lpfnMAPISendMail)(LHANDLE, ULONG, lpMapiMessage,
                                            FLAGS, ULONG);

extern ULONG (FAR PASCAL *lpfnMAPISendDocuments)(ULONG, LPSTR, LPSTR, LPSTR,
                                                 ULONG);

extern ULONG (FAR PASCAL *lpfnMAPIFindNext)(LHANDLE, ULONG, LPSTR, LPSTR,
                                            FLAGS, ULONG, LPSTR);

extern ULONG (FAR PASCAL *lpfnMAPIReadMail)(LHANDLE, ULONG, LPSTR, FLAGS,
                                            ULONG, lpMapiMessage FAR *);

extern ULONG (FAR PASCAL *lpfnMAPISaveMail)(LHANDLE, ULONG, lpMapiMessage,
                                            FLAGS, ULONG, LPSTR);

extern ULONG (FAR PASCAL *lpfnMAPIDeleteMail)(LHANDLE, ULONG, LPSTR, FLAGS,
                                              ULONG);

extern ULONG (FAR PASCAL *lpfnMAPIFreeBuffer)(LPVOID);

extern ULONG (FAR PASCAL *lpfnMAPIAddress)(LHANDLE, ULONG, LPSTR, ULONG,
                                           LPSTR, ULONG, lpMapiRecipDesc,
                                           FLAGS, ULONG, LPULONG,
                                           lpMapiRecipDesc FAR *);

extern ULONG (FAR PASCAL *lpfnMAPIDetails)(LHANDLE, ULONG,lpMapiRecipDesc,
                                           FLAGS, ULONG);

extern ULONG (FAR PASCAL *lpfnMAPIResolveName)(LHANDLE, ULONG, LPSTR,
                                               FLAGS, ULONG,
                                               lpMapiRecipDesc FAR *);
#endif

int FAR PASCAL InitMAPI(void);
int FAR PASCAL DeInitMAPI(void);