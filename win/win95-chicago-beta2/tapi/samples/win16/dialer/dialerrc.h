// 
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR 
// PURPOSE.
// 
// Copyright 1993-1995 Microsoft Corporation, all rights reserved.
//
/****************************************************************************/
/*
    Dialer ---- Windows TAPI sample application created as an illustration of the usage of Windows TAPI
    
    Dialer does the following 3 things :
    
    (1) initiates/drops calls
    (2) handles simple TAPI request for other application initiating/dropping calls on their behalf
    (3) monitors incoming/outgoing calls and keeps a call log based on the user's request.
    
    dialerrc.h : contains dialer's resource related constant declarations.
*/


/****************************************************************************/
/* Constants needed from Chicago Internal (not defined in 16-bit SDK)       */
/****************************************************************************/

#ifndef DS_3DLOOK
#define DS_3DLOOK           0x00000004L
#endif
#ifndef DS_CONTEXTHELP
#define DS_CONTEXTHELP      0x00002000L
#endif
#ifndef SS_ETCHEDHORZ
#define SS_ETCHEDHORZ       0x00000010L
#endif
#ifndef WS_EX_APPWINDOW
#define WS_EX_APPWINDOW     0x00040000L
#endif


/****************************************************************************/
/* Dialer application level resource definitions */
/****************************************************************************/

#define icoDialer                           1
#define icoLineBusy                         116

#define aidDialer                           117

/****************************************************************************/
/* dialog definitions */    
/****************************************************************************/
#define didHelp                             10

/* main dialog window */    
#define dlgDialer                           200

#define didDialerBtnDial                    IDOK
#define didDialerComboNumDial               (dlgDialer+1)  

#define didDialerBtnPad1                      (didDialerComboNumDial+1)
#define didDialerBtnPad2                      (didDialerBtnPad1+1)
#define didDialerBtnPad3                      (didDialerBtnPad2+1)
#define didDialerBtnPad4                      (didDialerBtnPad3+1)
#define didDialerBtnPad5                      (didDialerBtnPad4+1)
#define didDialerBtnPad6                      (didDialerBtnPad5+1)
#define didDialerBtnPad7                      (didDialerBtnPad6+1)
#define didDialerBtnPad8                      (didDialerBtnPad7+1)
#define didDialerBtnPad9                      (didDialerBtnPad8+1)
#define didDialerBtnPad0                      (didDialerBtnPad9+1)
#define didDialerBtnPadStar                   (didDialerBtnPad0+1)
#define didDialerBtnPadPound                  (didDialerBtnPadStar+1)
#define didDialerBtnPadFirst                  (didDialerBtnPad1)
#define didDialerBtnPadLast                   (didDialerBtnPadPound)

#define didDialerBtnSpeedDial1                (didDialerBtnPadPound+1)
#define didDialerBtnSpeedDial2              (didDialerBtnSpeedDial1+1)
#define didDialerBtnSpeedDial3              (didDialerBtnSpeedDial2+1)
#define didDialerBtnSpeedDial4              (didDialerBtnSpeedDial3+1)
#define didDialerBtnSpeedDial5              (didDialerBtnSpeedDial4+1)
#define didDialerBtnSpeedDial6              (didDialerBtnSpeedDial5+1)
#define didDialerBtnSpeedDial7              (didDialerBtnSpeedDial6+1) 
#define didDialerBtnSpeedDial8              (didDialerBtnSpeedDial7+1)
#define didDialerBtnSpeedDialFirst          (didDialerBtnSpeedDial1)
#define didDialerBtnSpeedDialLast           (didDialerBtnSpeedDial8)

#define didDialerGrpSpeedDial               (didDialerBtnSpeedDialLast+1)
#define didDialerSTextDial                  (didDialerGrpSpeedDial+1)
#define didDialerRectSeperator              (didDialerSTextDial+1)
#define didDialerBtnSpeedText1				(didDialerRectSeperator+1)
#define didDialerBtnSpeedText2				(didDialerBtnSpeedText1+1)
#define didDialerBtnSpeedText3				(didDialerBtnSpeedText2+1)
#define didDialerBtnSpeedText4				(didDialerBtnSpeedText3+1)
#define didDialerBtnSpeedText5				(didDialerBtnSpeedText4+1)
#define didDialerBtnSpeedText6				(didDialerBtnSpeedText5+1)
#define didDialerBtnSpeedText7				(didDialerBtnSpeedText6+1)
#define didDialerBtnSpeedText8				(didDialerBtnSpeedText7+1)

/********************************************************/
/* Prog speed dial buttons dialog window */  
#define dlgProgSD                       300

#define didProgSDBtnSpeedDial1          (dlgProgSD+1)
#define didProgSDBtnSpeedDial2            (didProgSDBtnSpeedDial1+1)
#define didProgSDBtnSpeedDial3            (didProgSDBtnSpeedDial2+1)
#define didProgSDBtnSpeedDial4            (didProgSDBtnSpeedDial3+1)
#define didProgSDBtnSpeedDial5            (didProgSDBtnSpeedDial4+1)
#define didProgSDBtnSpeedDial6            (didProgSDBtnSpeedDial5+1)
#define didProgSDBtnSpeedDial7            (didProgSDBtnSpeedDial6+1)
#define didProgSDBtnSpeedDial8            (didProgSDBtnSpeedDial7+1)
#define didProgSDBtnSpeedDialFirst      didProgSDBtnSpeedDial1
#define didProgSDBtnSpeedDialLast       didProgSDBtnSpeedDial8

#define didProgSDSTextSpeedDial1          (didProgSDBtnSpeedDialLast+1)
#define didProgSDSTextSpeedDial2          (didProgSDSTextSpeedDial1+1)
#define didProgSDSTextSpeedDial3          (didProgSDSTextSpeedDial2+1)
#define didProgSDSTextSpeedDial4          (didProgSDSTextSpeedDial3+1)
#define didProgSDSTextSpeedDial5          (didProgSDSTextSpeedDial4+1)
#define didProgSDSTextSpeedDial6          (didProgSDSTextSpeedDial5+1)
#define didProgSDSTextSpeedDial7          (didProgSDSTextSpeedDial6+1)
#define didProgSDSTextSpeedDial8          (didProgSDSTextSpeedDial7+1)
#define didProgSDSTextSpeedDialFirst    didProgSDSTextSpeedDial1
#define didProgSDSTextSpeedDialLast     didProgSDSTextSpeedDial8

#define didProgSDEditName                 (didProgSDSTextSpeedDialLast+1)
#define didProgSDEditNumber               (didProgSDEditName+1)

#define didProgSDSTextTop                 (didProgSDEditNumber+1)
#define didProgSDSTextBottom              (didProgSDSTextTop+1)
#define didProgSDSTextName                (didProgSDSTextBottom+1)
#define didProgSDSTextNumber              (didProgSDSTextName+1)
#define didProgSDRectSeperator            (didProgSDSTextNumber+1)

#define didProgSDBtnSpeedText1			  (didProgSDRectSeperator+1)
#define didProgSDBtnSpeedText2			  (didProgSDBtnSpeedText1+1)
#define didProgSDBtnSpeedText3			  (didProgSDBtnSpeedText2+1)
#define didProgSDBtnSpeedText4			  (didProgSDBtnSpeedText3+1)
#define didProgSDBtnSpeedText5			  (didProgSDBtnSpeedText4+1)
#define didProgSDBtnSpeedText6			  (didProgSDBtnSpeedText5+1)
#define didProgSDBtnSpeedText7			  (didProgSDBtnSpeedText6+1)
#define didProgSDBtnSpeedText8			  (didProgSDBtnSpeedText7+1)

/********************************************************/
/* Prgoram single speed dial button dialog window */
#define dlgProgSDB                          400

#define didProgSDBBtnSaveDial               (dlgProgSDB + 1)
#define didProgSDBEditName                (didProgSDBBtnSaveDial + 1)
#define didProgSDBEditNumber              (didProgSDBEditName + 1)

#define didProgSDBSText                   (didProgSDBEditNumber + 1)
#define didProgSDBSTextName               (didProgSDBSText + 1)
#define didProgSDBSTextNumber             (didProgSDBSTextName + 1)

/********************************************************/
/* "Dialing Options..." dialog window */
#define dlgDialingOption                    500

#define didDialingOptionLBoxLine            (dlgDialingOption + 1)
#define didDialingOptionLBoxAddress         (didDialingOptionLBoxLine + 1)
#define didDialingOptionLBoxFirst           (didDialingOptionLBoxLine)
#define didDialingOptionLBoxLast            (didDialingOptionLBoxAddress)

#define didDialingOptionChkBoxSTAPIVoice    (didDialingOptionLBoxAddress + 1)

#define didDialingOptionSTextLine           (didDialingOptionChkBoxSTAPIVoice + 2)
#define didDialingOptionSTextAddress        (didDialingOptionSTextLine + 1)

#define didDialingOptionBtnProperties		(didDialingOptionSTextAddress + 1)

/********************************************************/
/* Call Log Options dialog window */
#define dlgLogOption                    600

#define didLogOptionChkBoxIncoming      (dlgLogOption + 1)
#define didLogOptionChkBoxOutgoing        (didLogOptionChkBoxIncoming + 1)

#define didLogOptionGrpBox                (didLogOptionChkBoxOutgoing + 1)

/********************************************************/
/* "About Dialer..." dialog window */
#define dlgAbout                        700

#define didAboutIcon                    (dlgAbout + 1)
#define didAboutSTextTitle                (didAboutIcon + 1)
#define didAboutSTextVersion              (didAboutSTextTitle + 1)
#define didAboutSTextWinMode              (didAboutSTextVersion + 1)
#define didAboutSTextFreeMem              (didAboutSTextWinMode + 1)
#define didAboutSTextResource             (didAboutSTextFreeMem + 1)
#define didAboutSTextCopyright            (didAboutSTextResource + 1)

/********************************************************/
/* Call log dialog window */
#define dlgCallLog                      800
#define didCallLogSTextLog              (dlgCallLog + 1)

/********************************************************/
/* Line In Use dialog window */
#define dlgLineInUse                    900

#define didLineInUseIcon                (dlgLineInUse + 1)
#define didLineInUseSText1                (didLineInUseIcon + 1)
#define didLineInUseSText2                (didLineInUseSText1 + 1)

/********************************************************/
/* Line In Use dialog window */
#define dlgChangeOption                1000

#define didChangeOptionEditBoxNumber        (dlgChangeOption + 1)
#define didChangeOptionBtnDialHelper        (didChangeOptionEditBoxNumber + 1)
#define didChangeOptionSTextNumber          (didChangeOptionBtnDialHelper + 1)

/********************************************************/
/* Call status dialog window */
#define dlgCallStatus                   1100 

#define didCallStatusSTextNameNum       (dlgCallStatus + 1)
#define didCallStatusSTextLocation        (didCallStatusSTextNameNum + 1)
#define didCallStatusSTextCallingCard     (didCallStatusSTextLocation + 1)
#define didCallStatusSTextTranslatedNum   (didCallStatusSTextCallingCard + 1)

#define didCallStatusBtnOption            (didCallStatusSTextTranslatedNum + 1)

#define didCallStatusEditBoxLogName       (didCallStatusBtnOption + 1)
#define didCallStatusSTextLogName         (didCallStatusEditBoxLogName + 1)

/********************************************************/
/* Dialing Prompt dialog window */
#define dlgDialingPrompt                1200

#define didDialingPromptSText           (dlgDialingPrompt + 1)
#define didDialingPromptBtnOption         (didDialingPromptSText + 1)

/********************************************************/
/* Call Failed dialog window */
#define dlgDisconnectedError            1300

#define didDisconnectedErrorSTextErr    (dlgDisconnectedError + 1)

/****************************************************************************/
/* menu definitions */
/****************************************************************************/

#define midFileExit                     1000
#define midEditCut                      (midFileExit + 1)
#define midEditCopy                     (midEditCut + 1)
#define midEditPaste                    (midEditCopy + 1)
#define midEditDelete                   (midEditPaste + 1)
#define midEditSpeedDialButtons         (midEditDelete + 1)
#define midOptionsDialing               (midEditSpeedDialButtons + 1)
#define midOptionsLog                   (midOptionsDialing + 1) 
#define midSetupLocation                (midOptionsLog + 1)
#define midOptionsViewLog               (midSetupLocation + 1)
#define midHelpContents                 (midOptionsViewLog + 1)
#define midHelpAbout                    (midHelpContents + 1)
#define midLogDial                      (midHelpAbout + 1)
#define midAccelSelectNumToDial         (midLogDial + 1)
#define midAccelHelp                    (midAccelSelectNumToDial + 1)

#define midHelpCallLog                  (midAccelHelp + 1)
#define midHelpWhatThis					(midHelpCallLog + 1)
#define menuCallLog                     104
#define menuDialer                      118

/****************************************************************************/
/* string definitions */
/****************************************************************************/

/********************************************************/
/* app string */

#define ikszAppName             901
#define ikszAppFriendlyName     902

/********************************************************/
/* dialer.ini access strings */

#define ikszNull                        -1

#define ikszSecPreference               982
#define ikszFieldDialerWndLT              983
#define ikszDialerWndLTDefault            984
#define ikszFieldPreferedLine           985
#define ikszPreferedLineDefault         986
#define ikszFieldPreferedAddress        987
#define ikszPreferedAddressDefault      988

#define ikszSecSpeedDialSettings        981
#define ikszFieldSDNameEmpty            909
#define ikszFieldSDName1                910
#define ikszFieldSDName2                911
#define ikszFieldSDName3                912
#define ikszFieldSDName4                913
#define ikszFieldSDName5                914
#define ikszFieldSDName6                915
#define ikszFieldSDName7                916
#define ikszFieldSDName8                917
#define ikszFieldSDName9                918
#define ikszFieldSDName10               919

#define ikszFieldSDNumber1              1523
#define ikszFieldSDNumber2              1524
#define ikszFieldSDNumber3              1525
#define ikszFieldSDNumber4              1526
#define ikszFieldSDNumber5              1527
#define ikszFieldSDNumber6              1528
#define ikszFieldSDNumber7              1529
#define ikszFieldSDNumber8              1530
#define ikszFieldSDNumber9              1531
#define ikszFieldSDNumber10             1532

#define ikszSecCallLogging              980
#define ikszFieldCLIncoming             920
#define ikszFieldCLOutgoing             921
#define ikszFieldCLVisible              922
#define ikszFieldCLWinPos               923
#define ikszFieldCLWinDimension         924

#define ikszSecLastDialNumber           1501
#define ikszFieldLastDialedNumber1      1502
#define ikszFieldLastDialedNumber2      1503
#define ikszFieldLastDialedNumber3      1504
#define ikszFieldLastDialedNumber4      1505
#define ikszFieldLastDialedNumber5      1506
#define ikszFieldLastDialedNumber6      1507
#define ikszFieldLastDialedNumber7      1508
#define ikszFieldLastDialedNumber8      1509
#define ikszFieldLastDialedNumber9      1510
#define ikszFieldLastDialedNumber10     1511
#define ikszFieldLastDialedNumber11     1512
#define ikszFieldLastDialedNumber12     1513
#define ikszFieldLastDialedNumber13     1514
#define ikszFieldLastDialedNumber14     1515
#define ikszFieldLastDialedNumber15     1516
#define ikszFieldLastDialedNumber16     1517
#define ikszFieldLastDialedNumber17     1518
#define ikszFieldLastDialedNumber18     1519
#define ikszFieldLastDialedNumber19     1520
#define ikszFieldLastDialedNumber20     1521
#define ikszFieldLastDialedNumberFirst  ikszFieldLastDialedNumber1
#define ikszFieldLastDialedNumberLast   ikszFieldLastDialedNumber20

/********************************************************/
/* dialog related strings */

#define ikszCallStatusBtnHangup         951
#define ikszCallStatusBtnOK             1012
#define ikszCallStatusDialing           1002
#define ikszCallStatusConntected        1003
#define ikszCallStatusDisconnected      1004
#define ikszCallStatusNameFormatDialing 1005
#define ikszCallStatusNameFormatConnected 1006
#define ikszCallStatusNameFormatDisconnected 1007
#define ikszCallStatusLocationFormat    1008
#define ikszCallStatusCallingCardFormat 1009
#define ikszCallStatusTranslatedNumberFormat 1010
#define ikszCallStatusNameUnknown       1543

#define ikszUnknown                      1018

#define ikszDialerClipbrdFormatName         1522

#define ikszOptionsMenuShowLog          1533
#define ikszOptionsMenuHideLog          1534

#define ikszAboutModeStandard           931
#define ikszAboutModeEnhanced           932
#define ikszAboutModeWLO                933
#define ikszAboutModeUndef              934

#define ikszDialingPromptSTextBilling   1535
#define ikszDialingPromptSTextTone      1536
#define ikszDialingPromptSTextQuite     1537
#define ikszDialingPromptSTextPrompt    1538

/********************************************************/
/* error strings */

#define ikszErrDefault                  940
#define ikszErrOOM                      941
#define ikszErrTAPI                     942
#define ikszErrNoVoiceLine              943
#define ikszErrLineInitWrongDrivers     949
#define ikszErrNoVoiceLine              943
#define ikszErrInvalAddress             945
#define ikszErrAddrBlocked              950
#define ikszErrBillingRejected          952
#define ikszErrResUnavail               956
#define ikszErrLineInitBadIniFile       1013
#define ikszErrLineInitNoDriver         1014
#define ikszErrBadTAPIAddr              1015
#define ikszErrNoMultipleInstance       1020
#define ikszErrLineClose                1553

#define ikszWarningTitle                977
#define ikszWarningRegisterSTapi        978
#define ikszWarning911                  1544
#define ikszWarningTapiReInit           1552

#define ikszDisconnectedReject          1545
#define ikszDisconnectedBusy            1546
#define ikszDisconnectedNoAnswer        1547
#define ikszDisconnectedCantDo          1548
#define ikszDisconnectedNetwork         1549
#define ikszDisconnectedIncompatible    1550
#define ikszErrInvalCallState           1551
#define ikszDisconnectedNoDialTone      1554

/********************************************************/
/* misc strings */
    
#define ikszCallLogFile                 953
#define ikszCallLogLineName             954
#define ikszCallLogAddrName             955
 
#define ikszCallLogTo                   974
#define ikszCallLogFrom                 975
#define ikszCallLogMinute               976
#define ikszCallLogNumber               973
