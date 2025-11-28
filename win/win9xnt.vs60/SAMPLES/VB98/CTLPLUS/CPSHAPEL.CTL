VERSION 5.00
Begin VB.UserControl ShapeLabel 
   BackStyle       =   0  'Transparent
   ClientHeight    =   1575
   ClientLeft      =   0
   ClientTop       =   0
   ClientWidth     =   2820
   ClipControls    =   0   'False
   ForwardFocus    =   -1  'True
   PropertyPages   =   "CPShapeL.ctx":0000
   ScaleHeight     =   1575
   ScaleWidth      =   2820
   Begin VB.Label lblCaption 
      Alignment       =   2  'Center
      AutoSize        =   -1  'True
      BackStyle       =   0  'Transparent
      Caption         =   "Caption"
      Height          =   195
      Left            =   840
      TabIndex        =   0
      Top             =   360
      Width           =   555
   End
   Begin VB.Shape shpBack 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H000000FF&
      FillStyle       =   0  'Solid
      Height          =   735
      Left            =   600
      Shape           =   2  'Oval
      Top             =   360
      Width           =   1575
   End
End
Attribute VB_Name = "ShapeLabel"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = True
Attribute VB_Ext_KEY = "PropPageWizardRun" ,"Yes"
Option Explicit
' ShapeLabel Notes:
'
' 1) Nearly all of this code was generated
'    by the ActiveX Control Interface
'    Wizard.  Where code has been added or
'    modified manually, it is commented.
' 2) The UserControl's ForwardFocus property
'    was set to True, to make focus work as
'    it does for ordinary Label controls.
' 3)
'

Const RESIZE_AdjustX As Single = 0.07
Const RESIZE_AdjustY As Single = 0.03

' Storage for property values.  (Most property
'   values for ShapeLabel are stored in properties
'   of the UserControl or its constituent controls.)
Private m_Alignment As AlignmentConstants

'Event Declarations:
Event Click() 'MappingInfo=UserControl,UserControl,-1,Click
Attribute Click.VB_Description = "Occurs when the user presses and then releases a mouse button over an object."
Event DblClick() 'MappingInfo=UserControl,UserControl,-1,DblClick
Event MouseDown(Button As Integer, Shift As Integer, X As Single, Y As Single) 'MappingInfo=UserControl,UserControl,-1,MouseDown
Attribute MouseDown.VB_Description = "Occurs when the user presses the mouse button while an object has the focus."
Event MouseMove(Button As Integer, Shift As Integer, X As Single, Y As Single) 'MappingInfo=UserControl,UserControl,-1,MouseMove
Attribute MouseMove.VB_Description = "Occurs when the user moves the mouse."
Event MouseUp(Button As Integer, Shift As Integer, X As Single, Y As Single) 'MappingInfo=UserControl,UserControl,-1,MouseUp
Attribute MouseUp.VB_Description = "Occurs when the user releases the mouse button while an object has the focus."

' The BackColor property was manually remapped
'   to the Shape control's FillColor property,
'   because that's what fills in the shape
'   that appears as ShapeLabel's background.
'
Public Property Get BackColor() As OLE_COLOR
Attribute BackColor.VB_Description = "Returns/sets the background color used to display text and graphics in an object."
    BackColor = shpBack.FillColor
End Property

Public Property Let BackColor(ByVal New_BackColor As OLE_COLOR)
    shpBack.FillColor() = New_BackColor
    PropertyChanged "BackColor"
End Property

' ForeColor is mapped to the Label control's
'   ForeColor, because ShapeLabel's ForeColor
'   should control the font color.  The Label
'   control's background is Transparent, so
'   the BackColor doesn't matter.
'
'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
'MappingInfo=lblCaption,lblCaption,-1,ForeColor
Public Property Get ForeColor() As OLE_COLOR
Attribute ForeColor.VB_Description = "Returns/sets the foreground color used to display text and graphics in an object."
    ForeColor = lblCaption.ForeColor
End Property

Public Property Let ForeColor(ByVal New_ForeColor As OLE_COLOR)
    lblCaption.ForeColor() = New_ForeColor
    PropertyChanged "ForeColor"
End Property

' To work the same way other controls' Enabled
'   properties do, Enabled must have the correct
'   Procedure ID.  The Interface Wizard doesn't
'   set this; it must be done manually.  Use
'   the Property Attributes dialog, accessed
'   from the Tools menu, to set Procedure ID
'   to Enabled for the Enabled property.  The
'   Procedure ID box is on the Advanced section
'   of the dialog.  Select Enabled in the Name
'   box to view attributes for the Enabled
'   property.
'
'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
'MappingInfo=UserControl,UserControl,-1,Enabled
Public Property Get Enabled() As Boolean
Attribute Enabled.VB_Description = "Returns/sets a value that determines whether an object can respond to user-generated events."
Attribute Enabled.VB_UserMemId = -514
    Enabled = UserControl.Enabled
End Property

Public Property Let Enabled(ByVal New_Enabled As Boolean)
    UserControl.Enabled() = New_Enabled
    PropertyChanged "Enabled"
End Property

'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
'MappingInfo=lblCaption,lblCaption,-1,Font
Public Property Get Font() As Font
Attribute Font.VB_Description = "Returns a Font object."
Attribute Font.VB_UserMemId = -512
    Set Font = lblCaption.Font
End Property

Public Property Set Font(ByVal New_Font As Font)
    Set lblCaption.Font = New_Font
    PropertyChanged "Font"
    ' Manually added: Changing the font
    '   may require adjusting the position
    '   of the Label control.
    Call UserControl_Resize
End Property

' Manually added property type BorderStyleConstants.
'
'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
'MappingInfo=shpBack,shpBack,-1,BorderStyle
Public Property Get BorderStyle() As BorderStyleConstants
Attribute BorderStyle.VB_Description = "Returns/sets the border style for an object."
    BorderStyle = shpBack.BorderStyle
End Property
Public Property Let BorderStyle(ByVal New_BorderStyle As BorderStyleConstants)
    ' Validation supplied by shpBack.
    shpBack.BorderStyle() = New_BorderStyle
    PropertyChanged "BorderStyle"
End Property

'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
'MappingInfo=UserControl,UserControl,-1,Refresh
Public Sub Refresh()
Attribute Refresh.VB_Description = "Forces a complete repaint of a object."
    UserControl.Refresh
End Sub

' This is an example of double mapping.
'   In order for ShapeLabel's Click event
'   to work properly, it must be raised
'   when the user clicks on the label, as
'   well as when she clicks on the body
'   of the control.  The Interface Wizard
'   doesn't generate code for double
'   mapping.
Private Sub lblCaption_Click()
    RaiseEvent Click
End Sub
Private Sub UserControl_Click()
    RaiseEvent Click
End Sub

' Manually added mapping for Label's
'   DblClick event.
Private Sub lblCaption_DblClick()
    RaiseEvent DblClick
End Sub
Private Sub UserControl_DblClick()
    RaiseEvent DblClick
End Sub

' Manually added mapping for Label's
'   MouseDown event.
Private Sub lblCaption_MouseDown(Button As Integer, Shift As Integer, X As Single, Y As Single)
    ' See comment in lblCaption_MouseMove.
    RaiseEvent MouseDown(Button, Shift, _
        ScaleX(X + lblCaption.Left, vbTwips, vbContainerPosition), _
        ScaleY(Y + lblCaption.Height, vbTwips, vbContainerPosition))
End Sub
Private Sub UserControl_MouseDown(Button As Integer, Shift As Integer, X As Single, Y As Single)
    ' See comment in UserConrol_MouseMove.
    RaiseEvent MouseDown(Button, Shift, _
        ScaleX(X, vbTwips, vbContainerPosition), _
        ScaleY(Y, vbTwips, vbContainerPosition))
End Sub

' Manually added mapping for Label's
'   MouseMove event.
Private Sub lblCaption_MouseMove(Button As Integer, Shift As Integer, X As Single, Y As Single)
    ' The mouse position (X, Y) must be translated
    '   into the container's coordinates.  Since
    '   it's relative to the Label, it must first
    '   be translated into UserControl coordinates,
    '   by adding lblCaption.Top and .Left.  (These
    '   can be added because the ScaleMode of the
    '   UserControl is Twips, the same as the
    '   coordinates of the Label.  If this were not
    '   so, another conversion would be required.)
    '
    RaiseEvent MouseMove(Button, Shift, _
        ScaleX(X + lblCaption.Left, vbTwips, vbContainerPosition), _
        ScaleY(Y + lblCaption.Height, vbTwips, vbContainerPosition))
End Sub
Private Sub UserControl_MouseMove(Button As Integer, Shift As Integer, X As Single, Y As Single)
    ' The mouse position (X, Y) must be translated
    '   into the container's coordinates (which
    '   might, for example, be pixels).
    '
    ' Note that we could make this more general by
    '   replacing vbTwips with UserControl.ScaleMode.
    '   That way it would always work, even if we
    '   later changed the ScaleMode.  However, this
    '   would mean two method calls instead of two
    '   constants.  On the theory that MouseMove
    '   should be as fast as possible, vbTwips is
    '   used here instead.  (Of course, if you change
    '   the ScaleMode at run time, then you must use
    '   UserControl.ScaleMode instead of vbTwips!)
    '
    RaiseEvent MouseMove(Button, Shift, _
        ScaleX(X, vbTwips, vbContainerPosition), _
        ScaleY(Y, vbTwips, vbContainerPosition))
End Sub

' Manually added mapping for Label's
'   MouseUp event.
Private Sub lblCaption_MouseUp(Button As Integer, Shift As Integer, X As Single, Y As Single)
    ' See comment in lblCaption_MouseMove.
    RaiseEvent MouseUp(Button, Shift, _
        ScaleX(X + lblCaption.Left, vbTwips, vbContainerPosition), _
        ScaleY(Y + lblCaption.Height, vbTwips, vbContainerPosition))
End Sub
Private Sub UserControl_MouseUp(Button As Integer, Shift As Integer, X As Single, Y As Single)
    ' See comment in UserControl_MouseMove.
    RaiseEvent MouseUp(Button, Shift, _
        ScaleX(X, vbTwips, vbContainerPosition), _
        ScaleY(Y, vbTwips, vbContainerPosition))
End Sub

' Manually added property type AlignmentConstants.
'
'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
'MappingInfo=lblCaption,lblCaption,-1,Alignment
Public Property Get Alignment() As AlignmentConstants
Attribute Alignment.VB_Description = "Returns/sets the alignment of a CheckBox or OptionButton, or a control's text."
    Alignment = m_Alignment
End Property

Public Property Let Alignment(ByVal New_Alignment As AlignmentConstants)
    ' Alignment isn't stored in a constituent
    '   control property, so we have to supply
    '   our own validation code.
    Select Case New_Alignment
        Case vbCenter
        Case vbLeftJustify
        Case vbRightJustify
        Case Else
            ' Invalid Property Value
            Err.Raise 380
    ' If you break here while running ShapeLabel,
    '   right-click in the code window, select
    '   Toggle from the context menu, and then
    '   select Break on Unhandled Errors.  You
    '   can then press F5 to continue running
    '   the demo.
    End Select
    m_Alignment = New_Alignment
    PropertyChanged "Alignment"
    ' Changing alignment can affect positions
    '   of constituent controls.
    Call UserControl_Resize
End Property

' Property type (OLE_COLOR) for BorderColor
'   had to be added manually.
'
'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
'MappingInfo=shpBack,shpBack,-1,BorderColor
Public Property Get BorderColor() As OLE_COLOR
Attribute BorderColor.VB_Description = "Returns/sets the color of an object's border."
    BorderColor = shpBack.BorderColor
End Property

Public Property Let BorderColor(ByVal New_BorderColor As OLE_COLOR)
    shpBack.BorderColor() = New_BorderColor
    PropertyChanged "BorderColor"
End Property

'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
'MappingInfo=shpBack,shpBack,-1,BorderWidth
Public Property Get BorderWidth() As Integer
Attribute BorderWidth.VB_Description = "Returns or sets the width of a control's border."
    BorderWidth = shpBack.BorderWidth
End Property

Public Property Let BorderWidth(ByVal New_BorderWidth As Integer)
    shpBack.BorderWidth() = New_BorderWidth
    PropertyChanged "BorderWidth"
End Property

'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
'MappingInfo=lblCaption,lblCaption,-1,Caption
Public Property Get Caption() As String
Attribute Caption.VB_Description = "Returns/sets the text displayed in an object's title bar or below an object's icon."
    Caption = lblCaption.Caption
End Property

Public Property Let Caption(ByVal New_Caption As String)
    lblCaption.Caption() = New_Caption
    PropertyChanged "Caption"
    ' Manually added: Changing the caption
    '   may require adjusting the position
    '   of the Label control.
    Call UserControl_Resize
End Property

'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
'MappingInfo=UserControl,UserControl,-1,hDC
Public Property Get hDC() As Long
Attribute hDC.VB_Description = "Returns a handle (from Microsoft Windows) to the object's device context."
    hDC = UserControl.hDC
End Property

'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
'MappingInfo=UserControl,UserControl,-1,hWnd
Public Property Get hWnd() As Long
Attribute hWnd.VB_Description = "Returns a handle (from Microsoft Windows) to an object's window."
    hWnd = UserControl.hWnd
End Property

'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
'MappingInfo=UserControl,UserControl,-1,MouseIcon
Public Property Get MouseIcon() As Picture
Attribute MouseIcon.VB_Description = "Sets a custom mouse icon."
    Set MouseIcon = UserControl.MouseIcon
End Property

Public Property Set MouseIcon(ByVal New_MouseIcon As Picture)
    Set UserControl.MouseIcon = New_MouseIcon
    PropertyChanged "MouseIcon"
End Property

' Manually added property type MousePointerConstants.
'
'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
'MappingInfo=UserControl,UserControl,-1,MousePointer
Public Property Get MousePointer() As MousePointerConstants
Attribute MousePointer.VB_Description = "Returns/sets the type of mouse pointer displayed when over part of an object."
    MousePointer = UserControl.MousePointer
End Property

Public Property Let MousePointer(ByVal New_MousePointer As MousePointerConstants)
    ' Validation is supplied by UserControl.
    UserControl.MousePointer() = New_MousePointer
    PropertyChanged "MousePointer"
End Property

' Manually added property type ShapeConstants.
'
'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
'MappingInfo=shpBack,shpBack,-1,Shape
Public Property Get Shape() As ShapeConstants
Attribute Shape.VB_Description = "Returns/sets a value indicating the appearance of a control."
    Shape = shpBack.Shape
End Property

Public Property Let Shape(ByVal New_Shape As ShapeConstants)
    ' Validation is provided by shpBack.
    shpBack.Shape() = New_Shape
    PropertyChanged "Shape"
End Property

'Load property values from storage
Private Sub UserControl_ReadProperties(PropBag As PropertyBag)

    shpBack.BackColor = PropBag.ReadProperty("BackColor", &H80000005)
    lblCaption.ForeColor = PropBag.ReadProperty("ForeColor", &H80000012)
    UserControl.Enabled = PropBag.ReadProperty("Enabled", True)
    Set Font = PropBag.ReadProperty("Font", Ambient.Font)
    shpBack.BorderStyle = PropBag.ReadProperty("BorderStyle", 0)
    m_Alignment = PropBag.ReadProperty("Alignment", 2)
    shpBack.BorderColor = PropBag.ReadProperty("BorderColor", -2147483640)
    shpBack.BorderWidth = PropBag.ReadProperty("BorderWidth", 1)
    lblCaption.Caption = PropBag.ReadProperty("Caption", "Caption")
    Set MouseIcon = PropBag.ReadProperty("MouseIcon", Nothing)
    UserControl.MousePointer = PropBag.ReadProperty("MousePointer", 0)
    shpBack.Shape = PropBag.ReadProperty("Shape", 2)
End Sub

Private Sub UserControl_Resize()
    Dim sngCaptionTop As Single
    Dim sngCaptionLeft As Single
    
    ' The Shape control that provides the
    '   background for ShapeLabel is resized
    '   to cover the whole control.
    shpBack.Move 0, 0, ScaleWidth, ScaleHeight
    ' The Label control that displays ShapeLabel's
    '   caption is placed according to the value
    '   of the Alignment property.
    Select Case Alignment
        Case vbCenter
            sngCaptionLeft = (ScaleWidth - lblCaption.Width) / 2
        Case vbLeftJustify
            sngCaptionLeft = RESIZE_AdjustX * ScaleWidth
        Case vbRightJustify
            sngCaptionLeft = ScaleWidth - lblCaption.Width - RESIZE_AdjustX * ScaleWidth
    End Select
    ' A VerticalAlignment property would
    '   work similarly; it would require
    '   its own Enum.
    sngCaptionTop = (ScaleHeight - lblCaption.Height) / 2 - RESIZE_AdjustY * ScaleHeight
    '
    lblCaption.Move sngCaptionLeft, sngCaptionTop
End Sub

'Write property values to storage
Private Sub UserControl_WriteProperties(PropBag As PropertyBag)
    Call PropBag.WriteProperty("BackColor", shpBack.BackColor, &H80000005)
    Call PropBag.WriteProperty("ForeColor", lblCaption.ForeColor, &H80000012)
    Call PropBag.WriteProperty("Enabled", UserControl.Enabled, True)
    Call PropBag.WriteProperty("Font", Font, Ambient.Font)
    Call PropBag.WriteProperty("BorderStyle", shpBack.BorderStyle, 0)
    Call PropBag.WriteProperty("Alignment", m_Alignment, 2)
    Call PropBag.WriteProperty("BorderColor", shpBack.BorderColor, -2147483640)
    Call PropBag.WriteProperty("BorderWidth", shpBack.BorderWidth, 1)
    Call PropBag.WriteProperty("Caption", lblCaption.Caption, "Caption")
    Call PropBag.WriteProperty("MouseIcon", MouseIcon, Nothing)
    Call PropBag.WriteProperty("MousePointer", UserControl.MousePointer, 0)
    Call PropBag.WriteProperty("Shape", shpBack.Shape, 2)
End Sub

