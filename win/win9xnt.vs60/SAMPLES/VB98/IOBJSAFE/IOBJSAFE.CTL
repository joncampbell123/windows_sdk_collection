VERSION 5.00
Begin VB.UserControl UCObjectSafety 
   BackColor       =   &H00C00000&
   BorderStyle     =   1  'Fixed Single
   ClientHeight    =   525
   ClientLeft      =   0
   ClientTop       =   0
   ClientWidth     =   4170
   EditAtDesignTime=   -1  'True
   FillColor       =   &H00C00000&
   ForeColor       =   &H00C00000&
   ScaleHeight     =   525
   ScaleWidth      =   4170
   Begin VB.Label Label1 
      AutoSize        =   -1  'True
      BorderStyle     =   1  'Fixed Single
      Caption         =   "IObjectSafety Interface Sample Control"
      BeginProperty Font 
         Name            =   "Tahoma"
         Size            =   9.75
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   300
      Left            =   120
      TabIndex        =   0
      Top             =   90
      Width           =   3900
   End
End
Attribute VB_Name = "UCObjectSafety"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = True
'------------------------------------------------------------------------
'     Copyright © 1997 Microsoft Corporation. All rights reserved.
'You have a royalty-free right to use, modify, reproduce and distribute
'the Sample Application Files (and/or any modified version) in any way
'you find useful, provided that you agree that Microsoft has no warranty,
'     obligations or liability for any Sample Application Files.
'------------------------------------------------------------------------
Option Explicit
Implements IObjectSafety

Private m_Safety                As Boolean
Private m_fMakeSafeForScripting As Boolean

'------------------------------------------------------------
Private Sub IObjectSafety_GetInterfaceSafetyOptions(ByVal riid As Long, pdwSupportedOptions As Long, pdwEnabledOptions As Long)
'------------------------------------------------------------
    Dim IID         As String                           ' interface id string
'------------------------------------------------------------
    ' set supported object safety features...
    pdwSupportedOptions = INTERFACESAFE_FOR_UNTRUSTED_CALLER Or INTERFACESAFE_FOR_UNTRUSTED_DATA
                          
    IID = GetIIDFromPTR(riid)                           ' get interface id string from pointer
    
    Select Case IID                                     ' determine interface requesting settings
    Case IID_IDispatch                                  ' interface IDispatch.
        ' if this control is Safe For Initializing _
          then set the INTERFACESAFE_FOR_UNTRUSTED_DATA flag
        pdwEnabledOptions = INTERFACESAFE_FOR_UNTRUSTED_DATA    ' set enabled feature flag
        Exit Sub                                        ' exit and return success
    Case IID_IPersistStorage, IID_IPersistStream, IID_IPersistPropertyBag
        ' if this control is Safe For Scripting or can be made safe for _
          scripting then set the INTERFACESAFE_FOR_UNTRUSTED_CALLER flag
        pdwEnabledOptions = pdwEnabledOptions Or INTERFACESAFE_FOR_UNTRUSTED_CALLER     ' set enabled feature flag
        Exit Sub                                        ' return success
    Case Else                                           ' unknown interface requested.
        Err.Raise E_NOINTERFACE                         ' safety options requested for interface are not supported.
    End Select
        
    Err.Raise E_FAIL                                    ' the safety setting for this interface isn't supported
'------------------------------------------------------------
End Sub
'------------------------------------------------------------

'------------------------------------------------------------
Private Sub IObjectSafety_SetInterfaceSafetyOptions(ByVal riid As Long, ByVal dwOptionsSetMask As Long, ByVal dwEnabledOptions As Long)
'------------------------------------------------------------
    Dim fSettings   As Long                             ' safety settings flag
    Dim IID         As String                           ' interface id string
'------------------------------------------------------------
    fSettings = (dwEnabledOptions And dwOptionsSetMask) ' get safety settings flag
    IID = GetIIDFromPTR(riid)                           ' get interface id string from pointer
    
    Select Case IID                                     ' determine interface requesting settings
    Case IID_IDispatch                                  ' interface IDispatch.
        ' ************************************************************
        ' if this control isn't or can't be made safe for scripting,
        ' uncomment the following line of code to return E_FAIL
        ' ************************************************************
        ' Err.Raise E_FAIL                              ' return error if control is not safe for scripting.
        
        If (fSettings = INTERFACESAFE_FOR_UNTRUSTED_CALLER) Then
        ' if this control is not considered safe for scripting, _
          but could be made safe for scripting by disabling its _
          unsafe features.  Use this flag to show that this control _
          isto required to be safe for scripting.
            m_fMakeSafeForScripting = True              ' set to true, control must be safe for scripting.
            Exit Sub                                    ' return success
        End If
    Case IID_IPersistStorage, IID_IPersistStream, IID_IPersistPropertyBag
        ' ************************************************************
        ' If this control is never safe for initialization then
        ' IObjectSafety should not be implemented.
        ' ************************************************************
        If (fSettings = INTERFACESAFE_FOR_UNTRUSTED_DATA) Then
            ' if this control is always safe for initialization
            Exit Sub                                    ' return success
        End If
    Case Else                                           ' unknown interface requested.
        Err.Raise E_NOINTERFACE                         ' safety options requested for interface are not supported.
    End Select
    
    Err.Raise E_FAIL                                    ' the safety option an interface isn't supported
'------------------------------------------------------------
End Sub
'------------------------------------------------------------

'------------------------------------------------------------
Public Property Get Safe() As Boolean
'------------------------------------------------------------
' This property is considered safe for scripting and may _
  be used safely by any host container.
'------------------------------------------------------------
    MsgBox "Method Call:: Property Get Safe"
    Safe = m_Safety                                     ' return boolean data value
'------------------------------------------------------------
End Property
'------------------------------------------------------------

'------------------------------------------------------------
Public Property Let Safe(ByVal IsSafe As Boolean)
'------------------------------------------------------------
' This property is considered safe for scripting and may _
  be used safely by any host container.
'------------------------------------------------------------
    MsgBox "Method Call:: Property Let Safe"
    m_Safety = IsSafe                                   ' set boolean data value
'------------------------------------------------------------
End Property
'------------------------------------------------------------

'------------------------------------------------------------
Public Property Get UnSafe() As Boolean
'------------------------------------------------------------
' This property is unsafe for scripting but may be requested
' to be safe by the host container.  If safety is required then
' this function needs to be disabled when it gets called.
'------------------------------------------------------------
    If m_fMakeSafeForScripting Then                     ' is object required to be safe?
        Err.Raise E_FAIL                                ' return error: this property isn't safe.
        Exit Property                                   ' return
    Else
        MsgBox "Method Call:: Property Get UnSafe"
        Safe = m_Safety                                 ' return boolean data value
    End If
'------------------------------------------------------------
End Property
'------------------------------------------------------------

'------------------------------------------------------------
Public Property Let UnSafe(ByVal IsSafe As Boolean)
'------------------------------------------------------------
' This property is unsafe for scripting but may be requested
' to be safe by the host container.  If safety is required then
' this function needs to be disabled when it gets called.
'------------------------------------------------------------
    If m_fMakeSafeForScripting Then                     ' is object required to be safe?
        Err.Raise E_FAIL                                ' return error: this property isn't safe.
        Exit Property                                   ' return
    Else
        MsgBox "Method Call:: Property Let UnSafe"
        m_Safety = IsSafe                               ' set boolean data value
    End If
'------------------------------------------------------------
End Property
'------------------------------------------------------------

'------------------------------------------------------------
Public Function ASafeFunctionToCall(ByVal lparm As Long) As Long
'------------------------------------------------------------
' This function is considered safe for scripting and may _
  be called safely in script.
'------------------------------------------------------------
    MsgBox "Method Call:: Function ASafeFunctionToCall"
    ASafeFunctionToCall = 1                             ' return some value
'------------------------------------------------------------
End Function
'------------------------------------------------------------

'------------------------------------------------------------
Public Function AnUnSafeFunction(ByVal badparm As Long) As Long
'------------------------------------------------------------
' This function is considered unsafe for scripting and should _
  never be called script.  If necessary return a runtime error _
  to disable this function.
'------------------------------------------------------------
    If m_fMakeSafeForScripting Then                     ' is control required to be safe?
        Err.Raise E_FAIL                                ' return error: this function isn't safe.
        Exit Function                                   ' return
    Else
        MsgBox "Method Call:: Function AnUnSafeFunction"
        AnUnSafeFunction = 32                           ' return some value
    End If
'------------------------------------------------------------
End Function
'------------------------------------------------------------
