VERSION 5.00
Begin VB.Form frmSetup1 
   AutoRedraw      =   -1  'True
   BackColor       =   &H00400000&
   BorderStyle     =   0  'None
   Caption         =   "VB5 Setup Toolkit"
   ClientHeight    =   1770
   ClientLeft      =   225
   ClientTop       =   1590
   ClientWidth     =   7950
   ClipControls    =   0   'False
   DrawStyle       =   5  'Transparent
   FillStyle       =   0  'Solid
   BeginProperty Font 
      Name            =   "Times New Roman"
      Size            =   24
      Charset         =   0
      Weight          =   700
      Underline       =   0   'False
      Italic          =   -1  'True
      Strikethrough   =   0   'False
   EndProperty
   ForeColor       =   &H00000000&
   Icon            =   "setup1.frx":0000
   LinkMode        =   1  'Source
   LockControls    =   -1  'True
   MaxButton       =   0   'False
   MinButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   118
   ScaleMode       =   3  'Pixel
   ScaleWidth      =   530
   WhatsThisHelp   =   -1  'True
   WindowState     =   2  'Maximized
   Begin VB.Label lblModify 
      AutoSize        =   -1  'True
      BorderStyle     =   1  'Fixed Single
      Caption         =   $"setup1.frx":0442
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   450
      Left            =   15
      TabIndex        =   1
      Top             =   15
      Visible         =   0   'False
      Width           =   7860
      WordWrap        =   -1  'True
   End
   Begin VB.Label lblDDE 
      AutoSize        =   -1  'True
      BorderStyle     =   1  'Fixed Single
      Caption         =   "This label is used for DDE connection to the Program Manager"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Left            =   15
      TabIndex        =   0
      Top             =   1515
      Visible         =   0   'False
      Width           =   4485
   End
End
Attribute VB_Name = "frmSetup1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Option Compare Text

'
' Can't put this is a resource because it indicated resource load failure, must localize separately
'
Const mstrRESOURCELOADFAIL$ = "An error occurred while initializing string resources used by Setup."

'-----------------------------------------------------------
' SUB: DrawBackGround
'
' Draws the 'blue wash' screen and prints the 'shadowed'
' app setup title
'-----------------------------------------------------------
'
Private Sub DrawBackGround()
    Const intBLUESTART% = 255
    Const intBLUEEND% = 0
    Const intBANDHEIGHT% = 2
    Const intSHADOWSTART% = 8
    Const intSHADOWCOLOR% = 0
    Const intTEXTSTART% = 4
    Const intTEXTCOLOR% = 15
    Const intRed% = 1
    Const intGreen% = 2
    Const intBlue% = 4
    Const intBackRed% = 8
    Const intBackGreen% = 16
    Const intBackBlue% = 32
    Dim sngBlueCur As Single
    Dim sngBlueStep As Single
    Dim intFormHeight As Integer
    Dim intFormWidth As Integer
    Dim intY As Integer
    Dim iColor As Integer
    Dim iRed As Single, iBlue As Single, iGreen As Single
    
    '
    'Get system values for height and width
    '
    intFormHeight = ScaleHeight
    intFormWidth = ScaleWidth

    If ReadIniFile(gstrSetupInfoFile, gstrINI_SETUP, gstrINI_COLOR) = vbNullString Then
        iColor = intBlue
    Else
        iColor = CInt(ReadIniFile(gstrSetupInfoFile, gstrINI_SETUP, gstrINI_COLOR))
    End If
    'Calculate step size and blue start value
    '
    sngBlueStep = intBANDHEIGHT * (intBLUEEND - intBLUESTART) / intFormHeight
    sngBlueCur = intBLUESTART

    '
    'Paint blue screen
    '
    For intY = 0 To intFormHeight Step intBANDHEIGHT
        If iColor And intBlue Then iBlue = sngBlueCur
        If iColor And intRed Then iRed = sngBlueCur
        If iColor And intGreen Then iGreen = sngBlueCur
        If iColor And intBackBlue Then iBlue = 255 - sngBlueCur
        If iColor And intBackRed Then iRed = 255 - sngBlueCur
        If iColor And intBackGreen Then iGreen = 255 - sngBlueCur
        Line (-1, intY - 1)-(intFormWidth, intY + intBANDHEIGHT), RGB(iRed, iGreen, iBlue), BF
        sngBlueCur = sngBlueCur + sngBlueStep
    Next intY

    '
    'Print 'shadowed' appname
    '
    CurrentX = intSHADOWSTART
    CurrentY = intSHADOWSTART
    ForeColor = QBColor(intSHADOWCOLOR)
    Print Caption
    CurrentX = intTEXTSTART
    CurrentY = intTEXTSTART
    ForeColor = QBColor(intTEXTCOLOR)
    Print Caption
End Sub
Private Sub Form_Load()
'
' Most of the work for Setup1 takes place in Form_Load()
' and is mostly driven by the information found in the
' SETUP.LST file.  To customize the Setup1 functionality,
' you will generally want to modify SETUP.LST.
' Particularly, information regarding the files you are
' installing is all stored in SETUP.LST.  The only
' exceptions are the Remote Automation files RacMgr32.Exe
' and AutMgr32.Exe which require special handling below
' with regards to installing their icons in a special
' program group.
'
' Some customization can also be done by editing the code
' below in Form_Load or in other parts of this program.
' Places that are more likely to need customization are
' documented with suggestions and examples in the code.
'
    Const strEXT_GRP$ = "GRP"                               'extension for progman group
    Const SW_HIDE = 0

    Dim strGroupName As String                              'Name of Program Group
    Dim sFile As FILEINFO                                   'first Files= line info
    Dim oFont As StdFont
    
    gfRegDAO = False
    
    On Error GoTo MainError

    SetFormFont Me
    'All the controls and the form are sharing the
    'same font object, so create a new font object
    'for the form so that the appearance of all the
    'controls are not changed also
    Set oFont = New StdFont
    With oFont
        .Size = 24
        .Bold = True
        .Italic = True
        .Charset = Me.lblModify.Font.Charset
        .Name = Me.lblModify.Font.Name
    End With
    Set Me.Font = oFont
    '
    'Initialize string resources used by global vars and forms/controls
    '
    GetStrings
    
    '
    'Get Windows, Windows\Fonts, and Windows\System directories
    '
    gstrWinDir = GetWindowsDir()
    gstrWinSysDir = GetWindowsSysDir()
    gstrFontDir = GetWindowsFontDir()

    '
    ' If the Windows System directory is a subdirectory of the
    ' Windows directory, the proper place for installation of
    ' files specified in the setup.lst as $(WinSysDest) is always
    ' the Windows \System directory.  If the Windows \System
    ' directory is *not* a subdirectory of the Windows directory,
    ' then the user is running a shared version of Windows.  In
    ' this case, if the user does not have write access to the
    ' shared system directory, we change the system files
    ' destination to the windows directory
    '
    If InStr(gstrWinSysDir, gstrWinDir) = 0 Then
        If WriteAccess(gstrWinSysDir) = False Then
            gstrWinSysDir = gstrWinDir
        End If
    End If

    '
    ' The command-line arguments must be processed as early
    ' as possible, because without them it is impossible to
    ' call the app removal program to clean up after an aborted
    ' setup.
    '
    ProcessCommandLine Command$, gfSilent, gstrSilentLog, gfSMS, gstrMIFFile, gstrSrcPath, gstrAppRemovalLog, gstrAppRemovalEXE
    gfNoUserInput = (gfSilent Or gfSMS)
    
    AddDirSep gstrSrcPath

    '
    ' The Setup Bootstrapper (SETUP.EXE) copies SETUP1.EXE and SETUP.LST to
    ' the end user's windows directory.  Information required for setup such
    ' as setup flags and fileinfo is read from the copy of SETUP.LST found in
    ' that directory.
    '
    gstrSetupInfoFile = gstrWinDir & gstrFILE_SETUP
    'Get the Appname (this will be shown on the blue wash screen)
    gstrAppName = ReadIniFile(gstrSetupInfoFile, gstrINI_SETUP, gstrINI_APPNAME)
    gintCabs = CInt(ReadIniFile(gstrSetupInfoFile, gstrINI_BOOT, gstrINI_CABS))
    If gstrAppName = vbNullString Then
        MsgError ResolveResString(resNOSETUPLST), vbOKOnly Or vbCritical, gstrSETMSG
        gstrTitle = ResolveResString(resSETUP, "|1", gstrAppName)
        ExitSetup Me, gintRET_FATAL
    End If
    
    gstrAppExe = ReadIniFile(gstrSetupInfoFile, gstrINI_SETUP, gstrINI_APPEXE)
    gstrTitle = ResolveResString(resSETUP, "|1", gstrAppName)
    If gfSilent Then LogSilentMsg gstrTitle & vbCrLf

    Dim lChar As Long
    
    gsTEMPDIR = String$(255, 0)
    lChar = GetTempPath(255, gsTEMPDIR)
    gsTEMPDIR = Left(gsTEMPDIR, lChar)
    AddDirSep gstrSrcPath
    gsCABNAME = gstrSrcPath & ReadIniFile(gstrSetupInfoFile, gstrINI_BOOT, gstrINI_CABNAME)
    gsCABNAME = GetShortPathName(gsCABNAME)
    gsCABNAME = gstrWinDir & BaseName(gsCABNAME)
    gsTEMPDIR = gsTEMPDIR & ReadIniFile(gstrSetupInfoFile, gstrINI_BOOT, gsINI_TEMPDIR)
    AddDirSep gsTEMPDIR
    '
    ' Display the background "blue-wash" setup screen as soon as we get the title
    '
    ShowMainForm

    '
    ' Display the welcome dialog
    '
    ShowWelcomeForm

    
    '
    ' If this flag is set, then the default destination directory is used
    ' without question, and the user is never given a chance to change it.
    ' This is intended for installing an .EXE/.DLL as a component rather
    ' than as an application in an application directory.  In this case,
    ' having an application directory does not really make sense.
    '
    If ReadIniFile(gstrSetupInfoFile, gstrINI_SETUP, gstrINI_FORCEUSEDEFDEST) = "1" Then
        gfForceUseDefDest = True
    End If
    
    '
    ' Read default destination directory.  If the name specified conflicts
    ' with the name of a file, then prompt for a new default directory
    '
    gstrDestDir = ResolveDestDir(ReadIniFile(gstrSetupInfoFile, gstrINI_SETUP, gstrINI_APPDIR))
    While FileExists(gstrDestDir) = True Or gstrDestDir = vbNullString
        If MsgError(ResolveResString(resBADDEFDIR), vbOKCancel Or vbQuestion, gstrSETMSG) = vbCancel Then
            ExitSetup Me, gintRET_FATAL
        End If
        
        If gfNoUserInput = True Then
            ExitSetup Me, gintRET_FATAL
        Else
            ShowPathDialog gstrDIR_DEST
        End If
    Wend

    '
    ' Ensure a trailing backslash on the destination directory
    '
    AddDirSep gstrDestDir

    Do
        '
        ' Display install button and default directory.  The user
        ' can change the destination directory from here.
        '
        ShowBeginForm

        '
        ' This would be a good place to display an option dialog, allowing the user
        ' a chance to select installation options: samples, docs, help files, etc.
        ' Results of this dialog would be checked in the loop below
        '
        'ShowOptionsDialog (Function you could write with option check boxes, etc.)
        '

        '
        ' Initialize "table" of drives used and disk space array
        '
        InitDiskInfo

        SetMousePtr vbHourglass
        ShowStaticMessageDialog ResolveResString(resDISKSPACE)

        '
        ' For every section in SETUP.LST that will be installed, call CalcDiskSpace
        ' with the name of the section
        '
        CalcDiskSpace gstrINI_FILES
        'CalcDiskSpace "MySection"
        'CalcDiskSpace "MyOtherSection"
        '
        ' If you created an options dialog, you need to check results here to
        ' determine whether disk space needs to be calculated (if the option(s)
        ' will be installed)
        '
        'If chkInstallSamples.Value = TRUE then
        '    CalcDiskSpace "Samples"
        'End If
        '

        HideStaticMessageDialog
        SetMousePtr vbDefault

    '
    ' After all CalcDiskSpace calls are complete, call CheckDiskSpace to check
    ' the results and display warning form (if necessary).  If the user wants
    ' to try another destination directory (or cleanup and retry) then
    ' CheckDiskSpace will return False
    '
    Loop While CheckDiskSpace() = False
    
    '
    ' Starts logging to the setup logfile (will be used for application removal)
    '
    EnableLogging gstrAppRemovalLog
    '
    ' Should go ahead and force the application directory to be created,
    ' since the application removal logfile will later be copied there.
    '
    MakePath gstrDestDir, False 'User may not ignore errors here
    
    '
    ' Create the main program group if one is wanted/needed.
    '
    Const fDefCreateGroupUnderWin95 = False
    '
    ' If fDefCreateGroupUnderWin95 is set to False (this is the default), then no
    ' program group will be created under Win95 unless it is absolutely necessary.
    '
    ' By default under Windows 95, no group should be created, and the
    ' single program icon should be placed directly under the
    ' Start>Programs menu (unless there are other, user-defined icons to create
    '
    Dim cIcons As Integer            ' Count of how many icons are required.
    Dim cGroups As Integer           ' Count of how many groups are required.
    '
    ' Read through the SETUP.LST file and determine how many icons are needed.
    '
    cIcons = CountIcons(gsICONGROUP)
    cGroups = CountGroups(gsICONGROUP)
    '
    ' Do the same for other sections in SETUP.LST if you've added your own.
    '
    'cIcons = cIcons + CountIcons("MySection")
    'cIcons = cIcons + CountIcons("MyOtherSection")
    
    '
    ' The following variable determines whether or not we create a program
    ' group for icons.  It is controlled by fNoGroupUnderWin95,
    ' fAdditionalIcons, and FTreatAsWin95().
    '
    Dim fCreateGroup As Boolean
    If TreatAsWin95() Then
        '
        ' Win95 only:
        ' We create a program group only if we have additional icons besides
        ' the application executable (if any), or if fDefCreateGroupUnderWin95
        ' has been set to True to override this default behavior.
        '
        fCreateGroup = (cGroups > 0)
    Else
        '
        ' Win32 NT only:
        ' We must always create a Program Manager group
        ' because we always create an icon for the application removal program.
        '
        fCreateGroup = True
    End If
    
    Dim iLoop As Integer
    
    If fCreateGroup Then
        For iLoop = 0 To cGroups - 1
            strGroupName = ""
            If (GetGroup(gsICONGROUP, iLoop) = gsSTARTMENUKEY) Or (GetGroup(gsICONGROUP, iLoop) = gsPROGMENUKEY) Then
                'Skip these, they're not needed.
            Else
                strGroupName = frmGroup.GroupName(frmSetup1, GetGroup(gsICONGROUP, iLoop), GetPrivate(gsICONGROUP, iLoop), GetStart(gsICONGROUP, iLoop))
                If GetGroup(gsICONGROUP, iLoop) <> strGroupName Then SetGroup gsICONGROUP, iLoop, strGroupName
            End If
            fMainGroupWasCreated = True
        Next
    End If
    
    ' Before we begin copying files, check for mdac_typ
    ' and if we find it, spawn that off first.  We will tell
    ' it to never reboot, and check at the end to see if we need to.
    DoEvents
    If CheckDataAccess Then
        'We need to install data access.  Display message.
        ShowStaticMessageDialog ResolveResString(resINSTALLADO)
        InstallDataAccess
        HideStaticMessageDialog
    End If

    '
    ' Show copy form and set copy gauge percentage to zero
    '
    SetMousePtr vbHourglass
    ShowCopyDialog
    UpdateStatus frmCopy.picStatus, 0, True

    '
    ' Always start with Disk #1
    '
    gintCurrentDisk = 1
    '
    ' For every section in SETUP.LST that needs to be installed, call CopySection
    ' with the name of the section
    '
    
    CopySection gstrINI_FILES
    'CopySection "MySection"
    'CopySection "MyOtherSection"
        
    '
    ' If you created an options dialog, you need to check results here to
    ' determine whether to copy the files in the particular section(s).
    '
    'If chkInstallSamples.Value = TRUE then
    '    CopySection "Samples"
    'End If
    '

    UpdateStatus frmCopy.picStatus, 1, True
    
    HideCopyDialog

    '
    ' If we installed AXDIST.EXE, we now need to run it
    ' so it will install any additional files it contains.
    '
    If gfAXDist = True Then
        '
        'Synchronously shell out and run the utility with the correct switches
        '
        If FileExists(gstrAXDISTInstallPath) Then
            SyncShell gstrAXDISTInstallPath, INFINITE, , True
        End If
    End If
    '
    '
    ' If we installed WINt351.EXE, we now need to run it
    ' so it will install any additional files it contains.
    '
    If gfWINt351 = True Then
        '
        'Synchronously shell out and run the utility with the correct switches
        '
        If FileExists(gstrWINt351InstallPath) Then
            SyncShell gstrWINt351InstallPath, INFINITE, , True
        End If
    End If
    '
    ' Now, do all the 'invisible' update things that are required
    '
    SetMousePtr vbDefault
    ShowStaticMessageDialog ResolveResString(resUPDATING)

    '
    ' Register all the files that have been saved in the registration array.  The
    ' CopySection API adds a registration entry (when required) if a file is copied.
    '
    RegisterFiles
    
    '
    ' Register all the licenses that appear in the [Licenses] section of
    ' Setup.lst.
    '
    RegisterLicenses
    
    '
    ' If any DAO files were installed, we need to add some special
    ' keys to the registry to support it so that links will work
    ' in OLE Database fields.
    '
    If gfRegDAO = True Then
        RegisterDAO
    End If
    '
    ' Create program icons (or links, i.e. shortcuts).
    '
    If (fMainGroupWasCreated = True) Or ((cIcons > 0) And TreatAsWin95()) Then
        ShowStaticMessageDialog ResolveResString(resPROGMAN)
        CreateIcons gsICONGROUP
        '
        ' Do the same for other sections in SETUP.LST if you've added your own.
        '
        'CreateIcons "MySection"
        'CreateIcons "MyOtherSection"
        '
    End If
    '
    ' Create a separate program group and icons for the Remote Automation
    ' Connection Manager and the Automation Manager, if either has been
    ' installed.
    ' This program group is entirely separate from the one created for the
    ' application program (if any), because it will be shared by all
    ' VB applications which install them.
    '
    ' NOTE: This is NOT the place to install additional icons.  This is
    ' NOTE: handled after the Remote Automation icons have been created.
    '
    ShowStaticMessageDialog ResolveResString(resPROGMAN)
    If gsDest.strAUTMGR32 <> "" Or gsDest.strRACMGR32 <> "" Then
        'At least one of these programs was installed.  Go ahead
        'and create the program group.
        Dim strRemAutGroupName As String
        
        strRemAutGroupName = ResolveResString(resREMAUTGROUPNAME)
        '
        ' Create the group for the Remote Automation Icons.  Note that
        ' since the user cannot choose the name of this group, there is
        ' no way at this point to correct an error if one occurs.  Therefore,
        ' fCreateOSProgramGroup will abort setup, without returning, if there
        ' is an error.
        '
        fCreateOSProgramGroup frmSetup1, strRemAutGroupName, False, False

        'Now create the icons for AUTMGR32.EXE and RACMGR32.EXE
        If gsDest.strRACMGR32 <> "" Then
            CreateOSLink frmSetup1, strRemAutGroupName, gsDest.strRACMGR32, "", ResolveResString(resRACMGR32ICON), True, gsPROGMENUKEY, False
        End If
        If gsDest.strAUTMGR32 <> "" Then
            CreateOSLink frmSetup1, strRemAutGroupName, gsDest.strAUTMGR32, "", ResolveResString(resAUTMGR32ICON), True, gsPROGMENUKEY, False
        End If
    End If

    '
    'Register the per-app path
    '
    If gstrAppExe <> "" Then
        Dim strPerAppPath As String
        strPerAppPath = ReadIniFile(gstrSetupInfoFile, gstrINI_SETUP, gstrINI_APPPATH)
        AddPerAppPath gstrAppExe, gsDest.strAppDir, strPerAppPath
    End If

ExitSetup:
    HideStaticMessageDialog
    RestoreProgMan
    If fWithinAction() Then
        'By now, all logging actions should have been either aborted or committed.
        MsgError ResolveResString(resSTILLWITHINACTION), vbExclamation Or vbOKOnly, gstrTitle
        ExitSetup Me, gintRET_FATAL
    End If
    MoveAppRemovalFiles strGroupName
    
    ExitSetup Me, gintRET_FINISHEDSUCCESS

MainError:
    Dim iRet As Integer
    iRet = MsgError(Error$ & vbLf & vbLf & ResolveResString(resUNEXPECTED), vbRetryCancel Or vbExclamation, gstrTitle)
    If gfNoUserInput Then iRet = vbCancel
    Select Case iRet
        Case vbRetry
            Resume
        Case vbCancel
            ExitSetup Me, gintRET_ABORT
            Resume
        'End Case
    End Select
End Sub

'-----------------------------------------------------------
' SUB: HideCopyDialog
'
' Unloads the copy files status form
'-----------------------------------------------------------
'
Private Sub HideCopyDialog()
    Unload frmCopy
End Sub

'-----------------------------------------------------------
' SUB: HideStaticMessageDialog
'
' Unloads the setup messages form
'-----------------------------------------------------------
'
Private Sub HideStaticMessageDialog()
    Unload frmMessage
End Sub

'-----------------------------------------------------------
' SUB: ShowBeginForm
'
' Displays the begin setup form
'-----------------------------------------------------------
'
Private Sub ShowBeginForm()
    If gfNoUserInput Then
        If IsValidDestDir(gstrDestDir) = False Then
            ExitSetup frmSetup1, gintRET_FATAL
        End If
    Else
        frmBegin.Show vbModal
    End If
End Sub

'-----------------------------------------------------------
' SUB: ShowCopyDialog
'
' Displays the copy files status form
'-----------------------------------------------------------
'
Private Sub ShowCopyDialog()
    CenterForm frmCopy
    frmCopy.Show
    frmCopy.Refresh
    If gfNoUserInput = True Then
        frmCopy.cmdExit.Visible = False
    Else
        frmCopy.cmdExit.SetFocus
    End If
End Sub

'-----------------------------------------------------------
' SUB: ShowMainForm
'
' Displays the main setup 'blue wash' form
'-----------------------------------------------------------
'
Private Sub ShowMainForm()
    Me.Caption = gstrTitle
    Me.Show
    DrawBackGround
    Me.Refresh
End Sub

'-----------------------------------------------------------
' SUB: ShowStaticMessageDialog
'
' Displays a setup message in a 'box' of the appropriate
' size for the message
'
' IN: [strMessage] - message to display
'-----------------------------------------------------------
'
Private Sub ShowStaticMessageDialog(ByVal strMessage As String)
    Dim frm As Form

    Set frm = frmMessage
    frm.lblMsg.Caption = strMessage

    '
    'Default height is twice the height of the setup icon.
    'If the height of the message text is greater, then
    'increase the form height to the label height plus
    'half an icon height
    '
    frm.ScaleHeight = frm.imgMsg.Height * 2
    If frm.lblMsg.Height > frm.ScaleHeight Then
        frm.ScaleHeight = frm.lblMsg.Height + frm.imgMsg.Height * 0.5
    End If

    '
    'Vertically center the icon and label within the form
    '
    frm.imgMsg.Top = frm.ScaleHeight / 2 - frm.imgMsg.Height / 2
    frm.lblMsg.Top = frm.ScaleHeight / 2 - frm.lblMsg.Height / 2

    CenterForm frm

    frm.Show
    frm.Refresh
End Sub

'-----------------------------------------------------------
' SUB: ShowWelcomeForm
'
' Displays the welcome to setup form
'-----------------------------------------------------------
'
Private Sub ShowWelcomeForm()
    If Not gfNoUserInput Then
        frmWelcome.Show vbModal
    End If
End Sub

'-----------------------------------------------------------
' SUB: GetStrings
'
' Loads string resources into global vars and forms/controls
'-----------------------------------------------------------
'
Private Sub GetStrings()
    On Error GoTo GSErr
    
    gstrSETMSG = ResolveResString(resSETMSG)
    
    Exit Sub
    
GSErr:
    MsgError mstrRESOURCELOADFAIL, vbCritical Or vbOKOnly, vbNullString
    ExitSetup Me, gintRET_FATAL
End Sub

Private Sub Form_Unload(Cancel As Integer)
    'Get rid of the cab file in the windows dir (if it exists).
    Dim lCount As Long
    Dim sCab As String
    Dim sTemp As String
    
    lCount = 0
    'Get rid of the cab file in the windows dir (if it exists).
    Do
        If gintCabs = 1 Then
            sCab = gstrWinDir
            AddDirSep sCab
            sCab = sCab & BaseName(gsCABNAME)
            If FileExists(sCab) Then Kill sCab
            Exit Do
        End If
        lCount = lCount + 1
        sCab = gstrWinDir
        AddDirSep sCab
        sTemp = Left(gsCABNAME, Len(gsCABNAME) - 5) & CStr(lCount) & gstrSEP_EXT & gsINI_CABNAME
        sCab = sCab & BaseName(sTemp)
        If FileExists(sCab) Then
            Kill sCab
        Else
            Exit Do
        End If
    Loop
End Sub
