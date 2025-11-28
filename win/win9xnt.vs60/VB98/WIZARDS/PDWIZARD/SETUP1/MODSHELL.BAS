Attribute VB_Name = "modShell"
Option Explicit

Public Enum SpecialFolderIDs
    sfidDESKTOP = &H0
    sfidPROGRAMS = &H2
    sfidPERSONAL = &H5
    sfidFAVORITES = &H6
    sfidSTARTUP = &H7
    sfidRECENT = &H8
    sfidSENDTO = &H9
    sfidSTARTMENU = &HB
    sfidDESKTOPDIRECTORY = &H10
    sfidNETHOOD = &H13
    sfidFONTS = &H14
    sfidTEMPLATES = &H15
    sfidCOMMON_STARTMENU = &H16
    sfidCOMMON_PROGRAMS = &H17
    sfidCOMMON_STARTUP = &H18
    sfidCOMMON_DESKTOPDIRECTORY = &H19
    sfidAPPDATA = &H1A
    sfidPRINTHOOD = &H1B
    sfidProgramFiles = &H10000
    sfidCommonFiles = &H10001
End Enum

Public Declare Function SHGetSpecialFolderLocation Lib "shell32" (ByVal hwndOwner As Long, ByVal nFolder As SpecialFolderIDs, ByRef pIdl As Long) As Long
Public Declare Function SHGetPathFromIDListA Lib "shell32" (ByVal pIdl As Long, ByVal pszPath As String) As Long
Public Declare Function SHGetDesktopFolder Lib "shell32" (ByRef pshf As IVBShellFolder) As Long
Public Declare Function SHGetMalloc Lib "shell32" (ByRef pMalloc As IVBMalloc) As Long

' SHGetSpecialFolderLocation successful rtn val
Public Const NOERROR = 0

Public Const CSIDL_DESKTOP = &H0
Public Const CSIDL_PROGRAMS = &H2
Public Const CSIDL_CONTROLS = &H3
Public Const CSIDL_PRINTERS = &H4
Public Const CSIDL_PERSONAL = &H5   ' (Documents folder)
Public Const CSIDL_FAVORITES = &H6
Public Const CSIDL_STARTUP = &H7
Public Const CSIDL_RECENT = &H8   ' (Recent folder)
Public Const CSIDL_SENDTO = &H9
Public Const CSIDL_BITBUCKET = &HA
Public Const CSIDL_STARTMENU = &HB
Public Const CSIDL_DESKTOPDIRECTORY = &H10
Public Const CSIDL_DRIVES = &H11
Public Const CSIDL_NETWORK = &H12
Public Const CSIDL_NETHOOD = &H13
Public Const CSIDL_FONTS = &H14
Public Const CSIDL_TEMPLATES = &H15   ' (ShellNew folder)


