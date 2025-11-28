Imports System
Imports System.Drawing
Imports System.ComponentModel
Imports System.Windows.Forms
Imports System.Threading
Imports System.Diagnostics
Imports System.IO
Imports Microsoft.DirectX
Imports Microsoft.DirectX.DirectInput
Imports Microsoft.VisualBasic

Namespace Scrawl
    '/ <summary>
    '/ Summary description for frmMain.
    '/ </summary>
    Public Class frmMain
        Inherits System.Windows.Forms.Form
        '/ <summary>
        '/ Required designer variable.
        '/ </summary>
        Private components As System.ComponentModel.Container = Nothing

        Private ApplicationMenu As ContextMenu = Nothing
        Private DataArrivalEvent As AutoResetEvent = Nothing
        Private ApplicationGraphics As Graphics = Nothing
        Private Const SampleBufferSize As Integer = 16
        Private ApplicationDevice As Device = Nothing
        Private Const ScrawlCYBitmap As Integer = 300
        Private Const ScawlCXBitmap As Integer = 512
        Private CurrentPoint As Point = New Point()  ' Virtual coordinates        
        Private DeviceThread As Thread = Nothing
        Private CursorBlank As Cursor = Nothing
        Private OldPoint As Point = New Point()
        Private Drawing As Boolean = False
        Private Sensitivity As Integer  ' Mouse sensitivity 
        Private dxFuzz As Integer  ' Leftover x-fuzz from scaling 
        Private dyFuzz As Integer  ' Leftover y-fuzz from scaling 

        Public Sub New()
            '
            ' Required for Windows Form Designer support
            '
            InitializeComponent()
        End Sub

        '/ <summary>
        '/ Clean up any resources being used.
        '/ </summary>
        Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
            If (disposing) Then
                If (Not Nothing Is components) Then
                    components.Dispose()
                End If
            End If
            MyBase.Dispose(disposing)

            DataArrivalEvent.Set()
        End Sub

#Region "Windows Form Designer generated code"
        '/ <summary>
        '/ Required method for Designer support - do not modify
        '/ the contents of me method with the code editor.
        '/ </summary>
        Private Sub InitializeComponent()

            ' 
            ' frmMain
            ' 
            Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
            Me.BackColor = System.Drawing.Color.White
            Me.ClientSize = New System.Drawing.Size(512, 294)
            Me.Cursor = System.Windows.Forms.Cursors.Cross
            Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle
            Me.MaximizeBox = False
            Me.Name = "frmMain"
            Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen
            Me.Text = "Scrawl"
        End Sub
#End Region

        '/ <summary>
        '/ The main entry point for the application.
        '/ </summary>        
        Public Shared Sub Main()
            Application.Run(New frmMain())
        End Sub

        Private Sub DataArrivalHandler()
            While Created
                DataArrivalEvent.WaitOne()

                If (Not Created) Then Exit While

                Dim col As BufferedDataCollection = Nothing

                Try
                    col = ApplicationDevice.GetBufferedData()
                Catch e As InputException
                    If (TypeOf e Is InputLostException Or TypeOf e Is AcquiredException) Then
                        SetAcquire(True)
                        GoTo Continue
                    End If
                End Try

                If (Nothing Is col) Then GoTo Continue

                Dim data As BufferedData
                For Each data In col
                    Select Case data.Offset

                        Case MouseOffset.X
                            UpdateCursorPosition(data.Data, 0)
                        Case MouseOffset.Y
                            UpdateCursorPosition(0, data.Data)
                        Case MouseOffset.Button0
                            If (0 <> (data.Data And &H80)) Then
                                Cursor = CursorBlank
                                CurrentPoint = PointToClient(Cursor.Position)
                                OldPoint = CurrentPoint
                                Drawing = True
                            Else
                                Cursor = Cursors.Cross
                                Cursor.Position = PointToScreen(CurrentPoint)
                                Drawing = False
                            End If
                        Case MouseOffset.Button1
                            SetAcquire(False)
                            ApplicationMenu.Show(Me, New Point(10, 10))
                    End Select
                Next
                If Drawing Then
                    ApplicationGraphics.DrawLine(New Pen(Color.Black), New Point(CurrentPoint.X, CurrentPoint.Y), OldPoint)
                    OldPoint = CurrentPoint
                End If
Continue:
            End While
        End Sub

        Private Sub UpdateCursorPosition(ByVal dx As Integer, ByVal dy As Integer)
            '-----------------------------------------------------------------------------
            ' Name: UpdateCursorPosition()
            ' Desc: Move our private cursor in the requested direction, subject
            '       to clipping, scaling, and all that other stuff.
            '
            '       This does not redraw the cursor.  You need to do that yourself.
            '-----------------------------------------------------------------------------

            ' Pick up any leftover fuzz from last time.  This is important
            ' when scaling down mouse motions.  Otherwise, the user can
            ' drag to the right extremely slow for the length of the table
            ' and not get anywhere.

            dx += dxFuzz
            dxFuzz = 0

            dy += dyFuzz
            dyFuzz = 0

            Select Case Sensitivity
                Case 1 ' High sensitivity: Magnify! 
                    dx *= 2
                    dy *= 2
                Case -1 ' Low sensitivity: Scale down 
                    dxFuzz = dx Mod 2  ' remember the fuzz for next time 
                    dyFuzz = dy Mod 2
                    dx /= 2
                    dy /= 2
            End Select

            CurrentPoint.X += dx
            CurrentPoint.Y += dy

            ' clip the cursor to our client area
            If (CurrentPoint.X < 0) Then CurrentPoint.X = 0

            If (CurrentPoint.X >= ScawlCXBitmap) Then CurrentPoint.X = ScawlCXBitmap - 1

            If (CurrentPoint.Y < 0) Then CurrentPoint.Y = 0

            If (CurrentPoint.Y >= ScrawlCYBitmap) Then CurrentPoint.Y = ScrawlCYBitmap - 1
        End Sub

        Private Sub frmMain_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles MyBase.Load
            ApplicationDevice = New Device(SystemGuid.Mouse)
            ApplicationDevice.SetCooperativeLevel(Me, CooperativeLevelFlags.Foreground Or CooperativeLevelFlags.NonExclusive)

            DataArrivalEvent = New AutoResetEvent(False)
            DeviceThread = New Thread(New ThreadStart(AddressOf DataArrivalHandler))
            DeviceThread.Start()

            ApplicationDevice.SetEventNotification(DataArrivalEvent)
            ApplicationDevice.Properties.BufferSize = SampleBufferSize
            SetAcquire(True)
            ApplicationGraphics = Me.CreateGraphics()

            Dim PopoutItem(2) As MenuItem
            PopoutItem(0) = New MenuItem("&Low" + ControlChars.Tab + "1", New EventHandler(AddressOf ContextSubMenuEvent))
            PopoutItem(1) = New MenuItem("&Normal" + ControlChars.Tab + "2", New EventHandler(AddressOf ContextSubMenuEvent))
            PopoutItem(2) = New MenuItem("&High" + ControlChars.Tab + "3", New EventHandler(AddressOf ContextSubMenuEvent))
            PopoutItem(1).Checked = True

            Dim item(2) As MenuItem
            item(0) = New MenuItem("&Exit", New EventHandler(AddressOf MenuEventExit))
            item(1) = New MenuItem("&Clear", New EventHandler(AddressOf MenuEventClear))
            item(2) = New MenuItem("&Sensitivity")
            item(2).MenuItems.AddRange(PopoutItem)

            ApplicationMenu = New ContextMenu(item)
            Me.ContextMenu = ApplicationMenu

            CursorBlank = New Cursor(DXUtil.SdkMediaPath + "blank.cur")
            Dim p As Point = New Point(Me.ClientRectangle.Bottom / 2, Me.ClientRectangle.Right / 2)
            Cursor.Position = PointToScreen(p)
            CurrentPoint = p
            OldPoint = p
        End Sub

        Private Sub MenuCreated(ByVal sender As Object, ByVal e As System.EventArgs)
            SetAcquire(False)
        End Sub

        Private Sub MenuEventExit(ByVal sender As Object, ByVal e As System.EventArgs)
            SetAcquire(False)
            Close()
        End Sub

        Private Sub MenuEventClear(ByVal sender As Object, ByVal e As System.EventArgs)
            OnClear()
            SetAcquire(True)
        End Sub

        Private Sub ContextSubMenuEvent(ByVal sender As Object, ByVal e As System.EventArgs)

            Dim item As MenuItem = sender
            Dim i As Integer = -1

            Dim m As MenuItem

            For Each m In ApplicationMenu.MenuItems(2).MenuItems
                If (m.Equals(item)) Then
                    m.Checked = True
                    Sensitivity = i
                Else
                    m.Checked = False
                End If
                i += 1
            Next
            item.Checked = True

            SetAcquire(True)
        End Sub

        Private Sub frmMain_Activated(ByVal sender As Object, ByVal e As System.EventArgs) Handles MyBase.Activated
            SetAcquire(True)
        End Sub

        Private Sub SetAcquire(ByVal state As Boolean)
            If (Not Nothing Is ApplicationDevice) Then
                If (state) Then
                    Try
                        ApplicationDevice.Acquire()
                    Catch i As InputException
                    End Try
                Else
                    Try
                        ApplicationDevice.Unacquire()
                    Catch i As InputException
                    End Try
                End If
            End If
        End Sub

        Private Sub OnClear()
            '-----------------------------------------------------------------------------
            ' Name: OnClear()
            ' Desc: Makes the form white
            '-----------------------------------------------------------------------------            
            ApplicationGraphics.Clear(Color.White)
        End Sub

        Private Sub frmMain_KeyUp(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles MyBase.KeyUp
            If (e.KeyCode = Keys.Escape) Then
                Close()
            End If
        End Sub
    End Class
End Namespace