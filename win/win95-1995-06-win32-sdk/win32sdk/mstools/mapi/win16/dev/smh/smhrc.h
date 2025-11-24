/*
 *  S M H R C . H
 *
 *  Sample mail handling hook
 *  Resource IDs and configuration defines
 *
 *  Copyright 1992-95 Microsoft Corporation.  All Rights Reserved.
 */

#ifdef WIN16
#define DS_3DLOOK           0
#endif

/*
 *  Configuration Property Sheets
 */

#define SMH_ProviderName    100
#define SMH_GeneralTab      101
#define SMH_GeneralPage     102
#define SMH_FilterTab       103
#define SMH_FilterPage      104
#define SMH_ExclusionPage   105
#define SMH_ExclusionTab    106
#define SMH_FolderComment   107

#define ID_FilterOrder      200
#define ID_FilterOrderLB    201
#define ID_NewFilter        202
#define ID_RmvFilter        203
#define ID_EditFilter       204
#define ID_FilterDown       205
#define ID_FilterUp         206
#define ID_InboundGrp       207
#define ID_SentMailGrp      208
#define ID_DeletedGrp       209
#define ID_UpArrow          210
#define ID_UpArrowInv       211
#define ID_UpArrowDis       212
#define ID_DownArrow        213
#define ID_DownArrowInv     214
#define ID_DownArrowDis     215
#define ID_Exclusion        216
#define ID_ExclusionLB      217
#define ID_NewExclusion     218
#define ID_RmvExclusion     219

/*
 *  Configuration Dialogs
 */

#define SMH_FilterDesc      300

#define ID_Subject          300
#define ID_Sender           301
#define ID_AnyRecip         302
#define ID_ToRecip          303
#define ID_CcRecip          304
#define ID_BccRecip         305
#define ID_Body             306
#define ID_MsgClass         307
#define ID_HasAttach        308

#define ID_ValueTxt         320
#define ID_StoreTxt         321
#define ID_FolderTxt        322
#define ID_Value            323
#define ID_Store            324
#define ID_Folder           325
#define ID_ArchTarg         326
#define ID_ArchTargYr       327
#define ID_TypeGrp          328
#define ID_NameTxt          329
#define ID_Name             330
#define ID_TerminalTarg     331
#define ID_NotMatch         332
#define ID_FilterMsg        333
#define ID_DeleteMsg        334
#define ID_TargetGrp        335
#define ID_SoundTxt         336
#define ID_Sound            337

#define SMH_ExclusionEdit   340
#define ID_ExclusionText    340
#define ID_ExclusionClass   341

/*
 *  DLL Icon
 */

#define ID_Icon             431

/*
 *  Configuration Wizard
 */

#define WIZ_BASE            500
#define PAGE_INC             10
#define szWizardDialog      "SMH_WizardDialog"
#define ID_SentMailTxt      500
#define ID_SentMail         501
#define ID_SentMailYr       502
#define ID_DeletedTxt       510
#define ID_Deleted          511
#define ID_DeletedYr        512
#define ID_InboundTxt       520
#define ID_Inbound          521
#define ID_UnreadTxt        530
#define ID_UnreadTxt2       531
#define ID_Unread           532
