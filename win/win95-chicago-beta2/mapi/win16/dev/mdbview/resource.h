/*********************************************************************/
/*
 -  resource.h
 -  Copyright (C) 1994 Microsoft Corporation
 -
 *  Purpose:
 *      Defines the resources for the viewer application.
 */
/*********************************************************************/


/********************** Resource ID's ********************************/


/* Menu ID's */ 
      
#define IDM_EXIT                    1000
#define IDM_ABOUT                   1001
#define IDM_LOGON                   1002
#define IDM_LOGOFF                  1003
#define IDM_OPENMDB                 1004
#define IDM_GETREC                  1005
#define IDM_GETSTATUSTBL            1006
#define IDM_GETSTORESTBL            1007
#define IDM_LOGOFFMDB               1008
#define IDM_OPENROOT                1009
#define IDM_ENUMADRTYPES            1010
#define IDM_QUERYIDENTITY           1011
#define IDM_QUERYDEFMSGOPTS         1012
#define IDM_OPENSTATUS              1013
#define IDM_SESGETLASTERROR         1016
#define IDM_ADDRESS                 1017
#define IDM_CREATEONEOFF            1018
#define IDM_QUERYDEFRECIPOPT        1020
#define IDM_RESOLVENAME             1022
#define IDM_REGSTATUS               1023
#define IDM_REGSESSION              1024
#define IDM_REGMDB                  1025
#define IDM_REGTABLE                1026
#define IDM_MDBPROPS                1027
#define IDM_ABPROPS                 1028
#define IDM_IPMSUBTREE              1029
#define IDM_IPMOUTBOX               1030
#define IDM_IPMWASTEBASKET          1031
#define IDM_IPMSENTMAIL             1032
#define IDM_REGAB                   1033
#define IDM_REGON                   1034
#define IDM_GETRECFLDTBL            1035
#define IDM_SUPPORTMASK             1036



/* Property ID's */

#define IDC_TYPES                   200
#define IDC_VALUES                  201
#define IDC_DATA                    202
#define IDC_STRING                  204
#define IDC_HEX                     205
#define IDC_DECIMAL                 206
#define IDT_DISPLAY                 207
#define IDC_HELP                    208
#define IDT_PROPS                   211
#define IDC_PROPS                   212


/* Common to all Dlgs */

#define IDC_COPYTODEST              421
#define IDC_MAPILOGCLEAR            422
#define IDC_MAPILOG                 423
#define IDT_MAPILOG                 424


/* Msg ID's */

#define Msg                         600
#define IDC_MSGCREATE               601
#define IDC_MSGABORT                602
#define IDC_MSGCOPY                 603
#define IDC_MSGDELETE               604
#define IDC_MSGMODIFY               607
#define IDC_MSGSUBMIT               608
#define IDC_MSGREAD                 609
#define IDT_MSGCURRENT              610
#define IDC_MSGCURRENT              611
#define IDT_MSGCHILD                612
#define IDC_MSGCHILD                613
#define IDT_MSGATTACH               614
#define IDC_MSGATTACH               615
#define IDC_MSGOPENATTACH           616
#define IDC_MSGRECIPIENTS           617
#define IDT_MSGRECIPIENTS           618
#define IDC_MSGVIEWRECIP            619
#define IDT_MSGOPERATIONS           622
#define IDC_MSGRECIPTBL             623
#define IDC_MSGPROP                 624
#define IDC_MSG_MSGOPTS             625
#define IDC_MSG_RECIPOPTS           605

/* Fld ID's */

#define Fld                         400
#define IDT_FLDCURRENT              401
#define IDC_FLDCURRENT              402
#define IDC_FLDCHILD                403
#define IDT_FLDCHILD                404
#define IDT_FLDMESSAGES             405
#define IDC_FLDMESSAGES             406
#define IDC_FLDCREATE               407
#define IDC_FLDCOPYFOLDER           408

#define IDC_FLDDELETE               409
#define IDC_FLDSETREC               410
#define IDC_FLDOPENFLD              411
#define IDC_FLDOPENMSG              412
#define IDT_FLDOPERATIONS           413
#define IDC_FLDCONTTABLE            414
#define IDC_FLDHEIRTABLE            415

#define IDC_MDBPROP                 416
#define IDC_FLDPROP                 417
#define IDC_FLD_GETSEARCH           418
#define IDC_FLD_SETSEARCH           419



#define IDC_FLDSETMSGSTATUS         425
#define IDC_FLDGETMSGSTATUS         426
#define IDC_FLDSAVECONTENTSSORT     427


/* ATTACH ID's */

#define ATTACH                      700
#define IDT_ATTATTACH               701
#define IDC_ATTATTACH               702
#define IDT_ATTCURRENT              703
#define IDC_ATTCURRENT              704
#define IDC_ATTOPEN                 705
#define IDC_ATTSAVE                 706
#define IDC_ATTDELETE               707
#define IDC_ATTCREATE               708
#define IDT_ATTOPERATIONS           709
#define IDC_ATTTBL                  710  
#define IDC_ATTPROP                 711
#define IDC_ATTOPENASMSG            712
     
/* About box Dialog ID's */

#define AboutBox                    100

/* Operation Dialog ID's */

#define OPERATIONS                  800
#define IDT_OPSCB1                  801
#define IDC_OPSCB1                  802
#define IDT_OPSCB2                  803
#define IDC_OPSCB2                  804
#define IDT_OPSCB3                  805
#define IDC_OPSCB3                  806
#define IDT_OPSEDIT1                807
#define IDC_OPSEDIT1                808
#define IDT_OPSEDIT2                809
#define IDC_OPSEDIT2                810
#define IDT_OPSEDIT3                811
#define IDC_OPSEDIT3                812
#define IDT_OPSFLAGS                813
#define IDC_OPSFLAG1                814
#define IDC_OPSFLAG2                815
#define IDC_OPSFLAG3                816
#define IDC_OPSFLAG4                817
#define IDC_OPSHELP                 818
#define IDT_FLAGS                   819
#define IDC_OPSFLAG5                820
#define IDC_OPSFLAG6                821


#define RESTRICTION                 300
#define IDC_RESTRICTION             301
#define IDT_RESTRICTION             30
#define IDT_SEARCHSTATE             30



/* from table viewer dlg */
#define IDC_AND                         1115
#define IDC_OR                          1116
#define IDC_NEGATE1                     1117
#define IDC_RESTYPE1                    1118
#define IDC_PROPTAG1                    1119
#define IDC_RELATIONSHIP1               1120
#define IDC_VALUE1                      1121
#define IDC_NEGATE2                     1122
#define IDC_RESTYPE2                    1123
#define IDC_PROPTAG2                    1124
#define IDC_RELATIONSHIP2               1125
#define IDC_VALUE2                      1126
#define IDC_NEGATE3                     1127
#define IDC_PROPTAG3                    1128
#define IDC_RESTYPE3                    1129
#define IDC_RELATIONSHIP3               1130
#define IDT_VALUE3                      2014
#define IDC_VALUE3                      1147
#define IDC_SUBRES1                     1132
#define IDC_SUBRES2                     1133
#define IDC_SUBRES3                     1134
#define IDT_RESTYPE1                    2003
#define IDT_PROPTAG1                    2004
#define IDT_RELATIONSHIP1               2005
#define IDT_VALUE1                      2006
#define IDT_RESTYPE2                    2007
#define IDT_PROPTAG2                    2008
#define IDT_RELATIONSHIP2               2009
#define IDT_VALUE2                      2010
#define IDT_RESTYPE3                    2011
#define IDT_PROPTAG3                    2012
#define IDT_RELATIONSHIP3               2013
#define IDT_VALUE3                      2014
#define IDG_COMBINATION                 3005
#define IDG_RES1                        3006
#define IDG_RES2                        3007
#define IDG_RES3                        3008
#define IDD_RESDLG                      6000
#define IDC_PROPTAG12                   1168
#define IDC_PROPTAG22                   1169
#define IDC_PROPTAG32                   1170

/***** Store logon dlg *****/

#define STORELOGON                  500
#define IDC_ALL_STORES              501
#define IDC_READONLY                503
#define IDC_OPENSTORE               504
#define IDC_SETDEFAULTSTORE         505
#define IDT_STORE_PROVIDER          506
#define IDT_STORE_DISPLAY_NAME      507

/****** EnumAdrTypes dlg ****/

#define IDC_ENUMADRTYPES            900
#define IDC_ENUM                    901
#define IDT_ENUM                    902
#define IDT_ENUM_CTYPES             903

/****** QueryDefaultMsgOptions dlg *****/

#define QUERYDEFAULTMSGOPTS         1200
#define IDT_QD_PROPS                1201
#define IDT_QD_CVALS                1202
#define IDC_QD_PROPS                1203


#define ENTRYLIST                   1100
#define IDC_EL_CLEAR                1101
#define IDT_EL_LIST                 1102
#define IDT_EL_CVALS                1103
#define IDC_EL_LIST                 1104

#define ADRLIST                     1300
#define IDT_ADRLIST                 1301
#define IDT_CENTRIES                1302
#define IDC_ADRLIST                 1303

/* accept restriction display */

#define ACCEPTRES                       1401
#define IDMODIFY                        1402
#define IDT_ACCEPTRES                   1403
#define IDC_ACCEPTRES                   1404
#define IDC_MSGFLAGS                606
#define IDT_MSGFLAGS                620
#define IDC_FLD_ASS                 428
#define IDC_FLDOPENASS              429
#define IDT_FLD_ASS                 430
#define IDC_FLDEMPTYFOLDER          431
#define PROP_DISPLAY                1400
#define IDC_GENERIC                 1405
#define IDT_PROP_DISPLAY            1406
#define IDC_PROP_DISPLAY            1407
#define IDT_PROP_DISPLAY_CVALS      1409

#define NOTIFICATION                1500
#define IDT_NOTIF_DISPLAY           1501
#define IDT_NOTIF_CNOTIF            1502
#define IDC_NOTIF_DISPLAY           1503
#define ADVISENOTIF                 1600
#define IDC_ADVISE_EVENT1           1601
#define IDT_ADVISE_EVENT1           1602
#define IDT_ADVISE_EVENT2           1604
#define IDC_ADVISE_EVENT2           1605
#define IDT_ADVISE_EVENT3           1606
#define IDC_ADVISE_EVENT3           1607
#define IDT_ADVISE_CONTEXT          1609
#define IDC_ADVISE_CONTEXT          1614
#define IDC_ADVISE_EVENT4           1615
#define IDT_ADVISE_EVENT4           1616
#define IDT_ADVISE_EVENT5           1617
#define IDC_ADVISE_EVENT5           1618

#define NOTIF                       1700
#define IDT_N_ULCONNECT             1701
#define IDT_N_CONTEXT               1702
#define IDC_N_UNADVISE              1703
#define IDC_N_NEWADVISE             1704
#define IDC_N_DISPLAY               1705
#define IDC_FLDNOTIFFLD             432
#define IDC_FLDNOTIFHEIR            433
#define IDC_FLDNOITIFASS            434
#define IDC_FLDNOTIFCONT            435
#define IDC_FLDNOTIFASS             436
#define IDC_MSGNOTIFRECIP           621
#define IDC_MSGNOTIFATT             626
#define IDC_MSGNOTIFMSG             627
#define IDT_ATTMETHOD               713
#define IDC_ATTMETHOD               714
#define IDT_MSGPRIORITY             628
#define IDT_FLDTYPE                 437
#define IDC_FLDTYPE                 438
#define IDC_MSGPRIORITY             629
#define IDC_MSGSHOWFORM             630

#define SUPPORTMASK                 1800
#define IDC_SUPPORT_MASK            1801
#define IDT_SUPPORT_MASK            1802
