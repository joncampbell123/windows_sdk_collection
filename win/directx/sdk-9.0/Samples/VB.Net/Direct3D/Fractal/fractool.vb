'-----------------------------------------------------------------------------
' File: Fractool.vb
'
' Desc: Create fractals
'
' Copyright (c) 1997-2001 Microsoft Corporation. All rights reserved.
'-----------------------------------------------------------------------------
Imports System

Namespace FractalTool
    _

    '/ <summary>
    '/ This class generates a 2D array of elevation points using
    '/ midpoint displacement and random additions in two dimensions.
    '/ </summary>
    Public Class ElevationPoints
        'fractal terrain goes here. It is (2^maxlevel+1)^2 in bufferSize.
        Public points(,) As Double
        'These values govern the topology of the fractal mesh
        Private maxlevel As Integer
        Private addition As Boolean
        Private sigma As Double
        Private shape As Double

        'Gausian number generator.
        Private Gauss As FractalTool.GaussGen
        Private Function f3(ByVal delta As Double, ByVal x0 As Double, ByVal x1 As Double, ByVal x2 As Double) As Double
            Return (x0 + x1 + x2) / 3 + delta * Gauss.GaussianNumber
        End Function 'f3

        Private Function f4(ByVal delta As Double, ByVal x0 As Double, ByVal x1 As Double, ByVal x2 As Double, ByVal x3 As Double) As Double
            Return (x0 + x1 + x2 + x3) / 4 + delta * Gauss.GaussianNumber
        End Function 'f4

        '/ <summary>
        '/ Constrcutor. Pass in parameters
        '/ </summary>
        '/ <param name="newLevel"> Maxlevel : determines the bufferSize of the fractal mesh</param>
        '/ <param name="add"> Use random additions?</param>
        '/ <param name="sd"> sigma : initial standard deviation</param>
        '/ <param name="fdim"> fractal dimenion. Determines general shape of mesh</param>
        Public Sub New(ByVal newLevel As Integer, ByVal add As Boolean, ByVal sd As Double, ByVal fdim As Double)
            maxlevel = newLevel
            addition = add
            sigma = sd
            shape = fdim
        End Sub 'New

        '/ <summary>
        '/ Constructor : uses arbitrary defualts
        '/ </summary>
        Public Sub New()
            maxlevel = 5
            addition = True
            sigma = 0.5
            shape = 0.5
        End Sub 'New

        '/ <summary>
        '/ Generates a fractal mesh 2^maxelvel+1 in bufferSize
        '/ cribbed from "The Science of Fractal Images"
        '/ </summary>
        Public Sub CalcMidpointFM2D()
            Dim delta As Double 'tracks standard deviation
            Dim N, stage As Integer 'Integers
            Dim x, y, ourStep, d As Integer 'Array indices

            'Initialize gaussian number widget
            Gauss = New FractalTool.GaussGen()
            'Init stuff
            N = CInt(Math.Pow(2, maxlevel))
            delta = sigma
            'Allocate dump for data
            points = New Double(N + 1, N + 1) {}
            'Init starting corner points in grid
            points(0, 0) = delta * Gauss.GaussianNumber
            points(0, N) = delta * Gauss.GaussianNumber
            points(N, 0) = delta * Gauss.GaussianNumber
            points(N, N) = delta * Gauss.GaussianNumber
            ourStep = N
            d = N / 2
            stage = 1

            While stage <= maxlevel
                delta = delta * Math.Pow(0.5, 0.5 * shape)
                For x = d To N - d Step ourStep
                    For y = d To N - d Step ourStep
                        points(x, y) = f4(delta, points(x + d, y + d), points(x + d, y - d), points(x - d, y + d), points(x - d, y - d))
                    Next y
                Next x

                If addition Then
                    For x = 0 To N Step ourStep
                        For y = 0 To N Step ourStep
                            points(x, y) = points(x, y) + delta * Gauss.GaussianNumber
                        Next y
                    Next x
                End If
                delta = delta * Math.Pow(0.5, 0.5 * shape)

                For x = d To N - d Step ourStep
                    points(x, 0) = f3(delta, points(x + d, 0), points(x - d, 0), points(x, d))
                    points(x, N) = f3(delta, points(x + d, N), points(x - d, N), points(x, N - d))
                    points(0, x) = f3(delta, points(0, x + d), points(0, x - d), points(d, x))
                    points(N, x) = f3(delta, points(N, x + d), points(N, x - d), points(N - d, x))
                Next x

                For x = d To N - d Step ourStep
                    For y = ourStep To N - d Step ourStep
                        points(x, y) = f4(delta, points(x, y + d), points(x, y - d), points(x + d, y), points(x - d, y))
                    Next y
                Next x

                For x = ourStep To N - d Step ourStep
                    For y = d To N - d Step ourStep
                        points(x, y) = f4(delta, points(x, y + d), points(x, y - d), points(x + d, y), points(x - d, y))
                    Next y
                Next x

                If addition Then
                    For x = 0 To N Step ourStep
                        For y = 0 To N Step ourStep
                            points(x, y) = points(x, y) + delta * Gauss.GaussianNumber
                        Next y
                    Next x

                    For x = d To N - d Step ourStep
                        For y = d To N - d Step ourStep
                            points(x, y) = points(x, y) + delta * Gauss.GaussianNumber
                        Next y
                    Next x
                End If

                ourStep /= 2
                d /= 2
                stage += 1
            End While
        End Sub 'CalcMidpointFM2D
    End Class 'ElevationPoints
    _ 


    Class GaussGen
        Private Arand As Integer
        Private GaussAdd, numer, denom As Double
        Private rand As Random

        '/ <summary>
        '/ Constructor; Initialize the Gausian number system
        '/ </summary>
        '/ <param name="seed"></param>
        Public Sub New()
            rand = New Random()
            Arand = CInt(Math.Pow(2, 31)) - 1
            GaussAdd = Math.Sqrt(12)
            numer = GaussAdd + GaussAdd
            denom = CDbl(4) * Arand
        End Sub 'New 

        '/ <summary>
        '/ Return a Gaussian number
        '/ </summary>
        '/ <returns></returns>
        Public ReadOnly Property GaussianNumber() As Double
            Get
                Dim i As Integer
                Dim sum As Double = 0
                For i = 1 To 4
                    sum += rand.Next(Arand)
                Next i
                Return sum * numer / denom - GaussAdd
            End Get
        End Property
    End Class 'GaussGen 
End Namespace 'FractalTool







