'-----------------------------------------------------------------------------
' File: Chart.cs
'
' Desc: Chart control
'
' Copyright (c) 1997-2001 Microsoft Corporation. All rights reserved
'-----------------------------------------------------------------------------
Imports System
Imports System.Collections
Imports System.Drawing
Imports System.Drawing.Drawing2D
Imports System.Windows.Forms

Namespace ActionBasic
    '/ <summary>
    '/ Summary description for Chart.
    '/ </summary>
    Public Class Chart
        Inherits System.Windows.Forms.Panel
        Private GutterSize As Integer = 10 ' Chart display constants
        Private CellSize As Integer = 15

        ' Stored data to be drawn
        Private ActionNames As [String]() = Nothing
        Private DeviceStates As ArrayList = Nothing

        Private RefreshNeeded As Boolean = True

        Private ChartRect As New Rectangle(0, 0, 0, 0) ' Bounding RECT for the chart (grid and titles)
        Private GridRect As New Rectangle(0, 0, 0, 0) ' Bounding RECT for the grid
        Private ActiveBrush = New SolidBrush(Color.FromKnownColor(KnownColor.Highlight))
        Private UnmappedBrush = New HatchBrush(HatchStyle.BackwardDiagonal, Color.FromKnownColor(KnownColor.Highlight), Color.FromKnownColor(KnownColor.Control))
        Private InactiveBrush = New SolidBrush(Color.FromKnownColor(KnownColor.Control))
        Private GridPen As New Pen(Color.FromKnownColor(KnownColor.ControlDark))
        Private AxisPen As New Pen(Color.FromKnownColor(KnownColor.Control))

        Private ActionFont As New Font("arial", 9, FontStyle.Regular, GraphicsUnit.Pixel, 0, True)
        Private DeviceFont As New Font("arial", 9, FontStyle.Regular, GraphicsUnit.Pixel, 0, False)

        Public Sub New()
            SetStyle(ControlStyles.UserPaint, True)
            SetStyle(ControlStyles.AllPaintingInWmPaint, True)
            SetStyle(ControlStyles.DoubleBuffer, True)
        End Sub 'New

        Public Property ColumnTitles() As [String]()
            Get
                Return ActionNames
            End Get
            Set(ByVal Value As [String]())
                ActionNames = Value
                RefreshNeeded = True
                Invalidate()
            End Set
        End Property

        Public Property RowData() As ArrayList
            Get
                Return DeviceStates
            End Get
            Set(ByVal Value As ArrayList)
                DeviceStates = Value
                RefreshNeeded = True
                Invalidate()
            End Set
        End Property

        Public Sub UpdateData()
            Dim iY As Integer = GridRect.Top

            Dim state As DeviceState
            For Each state In DeviceStates
                Dim rc As New Rectangle(GridRect.Left + 3, iY + 3, 10, 10)

                Dim i As Integer
                For i = 0 To state.InputState.Length - 1
                    If state.InputState(i) <> state.PaintState(i) Then
                        Invalidate(rc)
                        End If
                        rc.Offset(CellSize, 0)
                Next i
                iY += CellSize
            Next state
            PaintChart(CreateGraphics())
        End Sub 'UpdateData

        Protected Sub PaintChart(ByVal g As Graphics)
            
            Dim iY As Integer = GridRect.Top
            Dim state As DeviceState

            For Each state In DeviceStates
                Dim rc As New Rectangle(GridRect.Left + 3, iY + 3, 10, 10)

                Dim i As Integer
                For i = 0 To state.InputState.Length - 1

                    If state.IsMapped(i) = False Then
                        g.FillRectangle(UnmappedBrush, rc)
                    ElseIf state.InputState(i) <> 0 Then
                        g.FillRectangle(ActiveBrush, rc)

                        If CType(i, GameActions) = GameActions.Walk Then
                            ' Scale the axis data to the size of the cell 
                            Dim tempX As Integer = rc.Left + 1 + (state.InputState(i) + 100) / 25

                            g.DrawLine(AxisPen, New Point(tempX, rc.Bottom - 2), New Point(tempX, rc.Top))
                            g.DrawLine(AxisPen, New Point(tempX - 1, rc.Top + 2), New Point(tempX + 2, rc.Top + 2))
                            g.DrawLine(AxisPen, New Point(tempX - 2, rc.Top + 3), New Point(tempX + 3, rc.Top + 3))
                        End If
                    Else
                        g.FillRectangle(InactiveBrush, rc)
                    End If

                    state.PaintState(i) = state.InputState(i)
                    rc.Offset(CellSize, 0)
                Next i

                iY += CellSize
            Next state
        End Sub 'PaintChart

        Protected Sub PaintLegend(ByVal g As Graphics)
            ' Paint the legend
            g.DrawString("Inactive", DeviceFont, Brushes.Black, 25, 8)
            g.DrawRectangle(GridPen, 8, 8, 12, 12)

            g.DrawString("Active", DeviceFont, Brushes.Black, 25, 24)
            g.DrawRectangle(GridPen, 8, 24, 12, 12)
            g.FillRectangle(ActiveBrush, 10, 26, 9, 9)

            g.DrawString("Unmapped", DeviceFont, Brushes.Black, 25, 40)
            g.DrawRectangle(GridPen, 8, 40, 12, 12)
            g.FillRectangle(UnmappedBrush, 10, 42, 9, 9)
        End Sub 'PaintLegend

        Protected Sub PaintGrid(ByVal g As Graphics)
            Dim iX As Integer = GridRect.Left
            Dim iY As Integer = GridRect.Top - GutterSize

            Dim formatAction As New StringFormat(StringFormatFlags.DirectionVertical)
            formatAction.Alignment = StringAlignment.Far
            Dim action As [String]
            For Each action In ActionNames
                g.DrawString(action, ActionFont, Brushes.Black, iX + 2, iY, formatAction)
                g.DrawRectangle(Pens.Gray, iX, GridRect.Top, CellSize, GridRect.Bottom - GridRect.Top)

                iX += CellSize
            Next action

            iY = GridRect.Top

            Dim state As DeviceState
            For Each state In DeviceStates
                g.DrawString(state.Name, DeviceFont, Brushes.Black, GridRect.Left - GutterSize, iY + 1, New StringFormat(StringFormatFlags.DirectionRightToLeft))
                g.DrawRectangle(Pens.Gray, GridRect.Left, iY, GridRect.Right - GridRect.Left, CellSize)

                iY += CellSize
            Next state
        End Sub 'PaintGrid

        '/ <summary>
        '/ Override of the paint method. All of the code in this method is
        '/ devoted to correctly positioning the chart and drawing labels.
        '/ </summary>
        '/ <seealso>PaintChart</seealso>
        '/ <param name="e">Paint Argumenets</param>
        Protected Overrides Sub OnPaint(ByVal e As PaintEventArgs)
            MyBase.OnPaint(e)

            Dim g As Graphics = e.Graphics

            If Nothing Is ActionNames Or Nothing Is DeviceStates Then
                Return
            End If

            Try
                If RefreshNeeded Then
                    RefreshChart(g)
                End If
                PaintLegend(g)
                PaintGrid(g)
                PaintChart(g)
            Catch i As InvalidOperationException
                OnPaint(e)
                Application.DoEvents()
            End Try
        End Sub 'OnPaint

        Private Sub RefreshChart(ByVal g As Graphics)
            RefreshNeeded = False

            Dim iMaxActionSize As Integer = 0
            Dim iMaxDeviceSize As Integer = 0

            ' Determine the largest action name size
            Dim name As String
            For Each name In ActionNames
                Dim size As Size = g.MeasureString(name, ActionFont).ToSize()
                iMaxActionSize = Math.Max(iMaxActionSize, size.Width)
            Next name

            ' Determine the largest device name size
            Dim state As DeviceState
            For Each state In DeviceStates
                Dim size As Size = g.MeasureString(state.Name, DeviceFont).ToSize()
                iMaxDeviceSize = Math.Max(iMaxDeviceSize, size.Width)
            Next state

            ' Determine the bounding rectangle for the chart and grid
            GridRect.Width = CellSize * ActionNames.Length
            GridRect.Height = CellSize * DeviceStates.Count

            ChartRect.Width = GridRect.Width + iMaxDeviceSize + GutterSize
            ChartRect.Height = GridRect.Height + iMaxActionSize + GutterSize

            ChartRect.X = (Me.Width - ChartRect.Width) / 2
            ChartRect.Y = (Me.Height - ChartRect.Height) / 2

            GridRect.X = ChartRect.X + iMaxDeviceSize + GutterSize
            GridRect.Y = ChartRect.Y + iMaxActionSize + GutterSize
        End Sub 'RefreshChart
    End Class 'Chart
End Namespace 'ActionBasic