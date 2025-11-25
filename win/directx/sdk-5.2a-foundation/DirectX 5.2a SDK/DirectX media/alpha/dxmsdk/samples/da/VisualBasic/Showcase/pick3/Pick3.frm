VERSION 5.00
Object = "{34F681D0-3640-11CF-9294-00AA00B8A733}#1.0#0"; "danim.dll"
Begin VB.Form Picking 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Picking"
   ClientHeight    =   4665
   ClientLeft      =   30
   ClientTop       =   270
   ClientWidth     =   5055
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   4665
   ScaleWidth      =   5055
   StartUpPosition =   3  'Windows Default
   Begin DirectAnimationCtl.DAViewerControlWindowed DAViewerControlWindowed 
      Height          =   4455
      Left            =   120
      OleObjectBlob   =   "Pick3.frx":0000
      TabIndex        =   0
      Top             =   120
      Width           =   4815
   End
End
Attribute VB_Name = "Picking"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'Pick3 Visual Basic Sample
Private Sub Form_Load()
  pi = 3.1459

  Dim size As DATransform3
  Set size = Scale3Uniform(0.25)

  Dim speed As DANumber
  Set speed = DANumber(0.07)
  
  ' Set up relative paths for media imports.  Does not work in VB
  ' debug.  Create executable.
  Dim mediaBase, geoBase, imgBase As String
  mediaBase = CurDir + "\..\..\..\..\..\Media\"
  geoBase = mediaBase + "geometry\"
  imgBase = mediaBase + "image\"
  
  'Import the geometries.
  Dim rawCube As DAGeometry
  Set rawCube = ImportGeometry(geoBase + "cube.x").Transform(size)
  
  Dim rawCylinder As DAGeometry
  Set rawCylinder = ImportGeometry(geoBase + "cylinder.x").Transform(size)
  
  Dim rawCone As DAGeometry
  Set rawCone = ImportGeometry(geoBase + "cone.x").Transform(size)
  
  'Import background.
  Dim stillSky As DAImage
  Set stillSky = ImportImage(imgBase + "cldtile.jpg")
  
  'Make the geometries pickable.
  Set cone1 = activate(rawCone, Green)
  Set cube1 = activate(rawCube, Magenta)
  Set cube2 = activate(rawCube, ColorHslAnim(Div(LocalTime, DANumber(8)), DANumber(1), DANumber(0.5)))
  Set cylinder = activate(rawCylinder, ColorRgb(0.8, 0.4, 0.4))
  
  'Construct the final geometry, scale and rotate it.
  Set multigeo = UnionGeometry(cone1.Transform(Translate3(0, 1, 0)), _
    UnionGeometry(cube1.Transform(Translate3(0, 0, 1)), _
    UnionGeometry(cube2.Transform(Translate3(0, 0, -1)), cylinder)))
    
  Set X = Add(DAStatics.Abs(DAStatics.Sin(Mul(LocalTime, _
    DANumber(0.2)))), DANumber(0.5))
  Set Y = Add(DAStatics.Abs(DAStatics.Sin(Mul(LocalTime, _
    DANumber(0.26)))), DANumber(0.5))
  Set Z = Add(DAStatics.Abs(DAStatics.Sin(Mul(LocalTime, _
    DANumber(0.14)))), DANumber(0.5))
     
  Set geo = multigeo.Transform(Scale3Anim(X, Y, Z))

  Set maxSky = stillSky.BoundingBox().Max()
  
  Set tiledSky = stillSky.Tile()
  Set movingSky = tiledSky.Transform(Translate2Anim(Mul(LocalTime, _
    Div(maxSky.X, DANumber(8))), Mul(LocalTime, Div(maxSky.X, DANumber(16)))))

  Set movingGeoImg = geometryImage(geo.Transform(Compose3(Rotate3Anim(ZVector3, _
    Mul(speed, Mul(LocalTime(), DANumber(1.9)))), _
      Rotate3Anim(YVector3, Mul(speed, Mul(LocalTime(), DANumber(pi)))))), speed)
  
  Set fs = DefaultFont.size(14).Color(Black)
  Set titleIm = StringImage("Left Click on an Object", fs).Transform(Translate2(0, 0.04))
    
  DAViewerControlWindowed.UpdateInterval = 0.2
  
  'Display the final image.
  DAViewerControlWindowed.Image = Overlay(titleIm, Overlay(movingGeoImg, movingSky))
  
  'Start the animation.
  DAViewerControlWindowed.Start
End Sub

Function activate(unpickedGeo As DAGeometry, col As DAColor) As DAGeometry
  Dim pickGeo As DAPickableResult
  Set pickGeo = unpickedGeo.Pickable()
  
  Dim pickEvent As DAEvent
  Set pickEvent = AndEvent(LeftButtonDown, pickGeo.pickEvent)
  
  Dim numcyc As DANumber
  Set numcyc = CreateObject("DirectAnimation.DANumber")
  numcyc.Init DAStatics.Until(DANumber(0), pickEvent, DAStatics.Until(DANumber(1), pickEvent, numcyc))
  
  Dim colcyc As DAColor
  Set colcyc = CreateObject("DirectAnimation.DAColor")
  colcyc.Init DAStatics.Until(White, pickEvent, DAStatics.Until(col, pickEvent, colcyc))

  Dim xf As DATransform3
  Set xf = Rotate3Anim(XVector3, Integral(numcyc))
  
  Set activate = pickGeo.Geometry.DiffuseColor(colcyc).Transform(xf)
End Function
Function geometryImage(geo As DAGeometry, speed As DANumber) As DAImage
  Dim scaleFactor As DANumber
  Set scaleFactor = DANumber(0.02)
  
  Dim perspTransform As DATransform3
  Set perspTransform = CreateObject("DirectAnimation.DATransform3")
    perspTransform.Init DAStatics.Until(Compose3(Rotate3Anim(XVector3, _
      Mul(speed, LocalTime)), Translate3(0, 0, 0.2)), RightButtonDown, _
        DAStatics.Until(Rotate3Anim(XVector3, Mul(speed, LocalTime)), _
          RightButtonDown, perspTransform))

  Set light = UnionGeometry(DirectionalLight.Transform(perspTransform), _
    DirectionalLight)
    
  Dim strcyl As DAString
  Set strcyl = CreateObject("DirectAnimation.DAString")
  strcyl.Init DAStatics.Until(DAString("Perspective - Right Click to Switch"), _
    RightButtonDown, DAStatics.Until(DAString("Parallel - Right Click to Switch"), _
      RightButtonDown, strcyl))
      
  Dim perspectiveCam As DACamera
  Set perspectiveCam = PerspectiveCamera(1, 0).Transform(Compose3(Rotate3Anim(XVector3, _
    Mul(speed, LocalTime)), Translate3(0, 0, 0.2)))

  Dim parallelCam As DACamera
  Set parallelCam = ParallelCamera(1).Transform(Rotate3Anim(XVector3, _
    Mul(speed, LocalTime)))
  
  Dim camera As DACamera
  Set camera = CreateObject("DirectAnimation.DACamera")
  camera.Init DAStatics.Until(perspectiveCam, RightButtonDown, _
    DAStatics.Until(parallelCam, RightButtonDown, camera))
  
  Dim fs As DAFontStyle
  Set fs = DefaultFont.size(14).Color(Red)
  
  Dim txtIm, xltTxt As DAImage
  Set txtIm = StringImageAnim(strcyl, fs)
  Set xltTxt = txtIm.Transform(Translate2(0, -0.045))
    
  Set geometryImg = UnionGeometry(geo.Transform(Scale3UniformAnim(scaleFactor)), _
    light).Render(camera)
  
  Set geometryImage = Overlay(xltTxt, geometryImg)
End Function
