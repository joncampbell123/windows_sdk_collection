/*********************************************************************/
/*
 -  resource.h
 -  Copyright (C) 1995 Microsoft Corporation
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
#define IDC_HELP1                   208
#define IDT_PROPS                   211
#define IDC_PROPS                   212


/* Common to all Dlgs */

#define IDC_COPYTODEST              421
#define IDC_MAPILOGCLEAR            422
#define IDC_MAPILOG                 423
#define IDT_MAPILOG                 424


#define IDT_FUNCTIONS               420
#define IDC_FUNCTIONS               441
#define IDC_SPECIALPROPS            442
#define IDC_CALLFUNC                444
#define IDC_PROPINTERFACE           445


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
#define IDC_MSG_X400_PROPS          632

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
#define IDC_FLDCONTTABLE            414
#define IDC_FLDHEIRTABLE            415

#define IDC_MDBPROP                 416
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


/********** SPECIAL FOLDER PROPS *********/
#define FLD_SPECIAL                 2100
#define IDT_DISPLAY_NAME            2101
#define IDT_DISPLAY_NAME1           2102

#define IDT_OBJECT_TYPE             2103
#define IDT_OBJECT_TYPE1            2104

#define IDT_FOLDER_TYPE             2105
#define IDT_FOLDER_TYPE1            2106

#define IDT_ACCESS                  2107
#define IDT_ACCESS1                 2108

#define IDT_ACCESS_LEVEL            2109
#define IDT_ACCESS_LEVEL1           2110

#define IDT_DISPLAY_TYPE            2111
#define IDT_DISPLAY_TYPE1           2112

#define IDT_SUBFOLDERS              2113
#define IDT_SUBFOLDERS1             2114

#define IDT_STATUS                  2115
#define IDT_STATUS1                 2116
#define IDT_STATUS2                 2132
#define IDT_STATUS3                 2133
#define IDT_STATUS4                 2134

#define MSG_SPECIAL                 2200
#define IDT_MESSAGE_FLAGS           2201
#define IDT_MESSAGE_FLAGS1          2202
#define IDT_MESSAGE_FLAGS2          2203
#define IDT_MESSAGE_FLAGS3          2204
#define IDT_MESSAGE_FLAGS4          2205
#define IDT_MESSAGE_FLAGS5          2206
#define IDT_MESSAGE_FLAGS6          2207
#define IDT_MESSAGE_FLAGS7          2208
#define IDT_MESSAGE_FLAGS8          2209
#define IDT_SUBJECT                 2210
#define IDT_SUBJECT1                2211
#define IDT_MESSAGE_CLASS           2212
#define IDT_MESSAGE_CLASS1          2213
#define IDT_HASATTACH               2214
#define IDT_HASATTACH1              2215
#define IDT_ACCESS2                 2216
#define IDT_ACCESS3                 2217
#define IDT_ACCESS4                 2218
#define IDT_ACCESS5                 2219
#define IDT_ACCESS6                 2220

#define ATTACH_SPECIAL              2300
#define IDT_ATTACH_METHOD           2301
#define IDT_ATTACH_METHOD1          2302


#define X400_IPM_SPECIAL            2000
#define IDT_AUTO_FORWARDED          2001
#define IDT_AUTO_FORWARDED1         2002
#define IDT_IMPORTANCE              2015
#define IDT_IMPORTANCE1             2016
#define IDT_INCOMPLETE_COPY         2017
#define IDT_INCOMPLETE_COPY1        2018
#define IDT_SENSITIVITY             2019
#define IDT_SENSITIVITY1            2020
#define IDT_SEARCH_KEY              2021
#define IDT_SEARCH_KEY1             2022
#define IDT_EXPIRY_TIME             2025
#define IDT_EXPIRY_TIME1            2026
#define IDT_PRIORITY                2027
#define IDT_PRIORITY1               2028
#define IDT_READ_RECEIPT_REQUESTED  2029
#define IDT_READ_RECEIPT_REQUESTED1 2030

#define X400_IPN_SPECIAL            1900
#define IDT_ORIGINAL_SEARCH_KEY     1901
#define IDT_ORIGINAL_SEARCH_KEY1    1902
#define IDT_CONVERSION_EITS         1903
#define IDT_CONVERSION_EITS1        1904
#define IDT_ORIGINALLY_INTENDED_RECIPIENT_NAME 1905
#define IDT_ORIGINALLY_INTENDED_RECIPIENT_NAME1 1906
#define IDT_SENDER_NAME             1907
#define IDT_SENDER_NAME1            1908
#define IDT_SENDER_EMAIL_ADDRESS    1909
#define IDT_SENDER_EMAIL_ADDRESS1   1910
#define IDT_SENDER_ADDRTYPE         1911
#define IDT_SENDER_ADDRTYPE1        1912
#define IDT_REPORT_TEXT             1913
#define IDT_REPORT_TEXT1            1914
#define IDT_REPORT_TIME             1915
#define IDT_REPORT_TIME1            1916
#define IDT_ACKNOWLEGEMENT_MODE     1917
#define IDT_ACKNOWLEGEMENT_MODE1    1918
#define IDT_RECEIPT_TIME            1919
#define IDT_RECEIPT_TIME1           1920
#define IDT_AUTO_FORWARD_COMMENT    1921
#define IDT_AUTO_FORWARD_COMMENT1   1922
