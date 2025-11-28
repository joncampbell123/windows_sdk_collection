VERSION 5.00
Object = "{34F681D0-3640-11CF-9294-00AA00B8A733}#1.0#0"; "danim.dll"
Begin VB.Form mscubes 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Mscubes"
   ClientHeight    =   4080
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   8505
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   4080
   ScaleWidth      =   8505
   StartUpPosition =   3  'Windows Default
   Begin DirectAnimationCtl.DAViewerControlWindowed DAViewerControlWindowed 
      Height          =   3855
      Left            =   120
      OleObjectBlob   =   "mscubes.frx":0000
      TabIndex        =   0
      Top             =   120
      Width           =   8295
   End
End
Attribute VB_Name = "mscubes"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
' Visual Basic verion of Mscubes
Private Sub Form_Load()
  Set changingRate = CreateObject("DirectAnimation.DANumber")
    changingRate.Init DAStatics.Until(DANumber(0.2), LeftButtonDown, _
      DAStatics.Until(DANumber(0), LeftButtonDown, changingRate))
      
  ' Set up relative paths for media imports.  Does not work in VB
  ' debug.  Create executable.
  Dim mediaBase, geoBase, imgBase, midiBase As String
  mediaBase = CurDir + "\..\..\..\..\..\Media\"
  geoBase = mediaBase + "geometry\"
  imgBase = mediaBase + "image\"
  midiBase = mediaBase + "midi\"
  
  'Import the needed geometry & images & sound.
  Set geo = ImportGeometry(geoBase + "cube.x")
  Set importSnd = ImportSound(midiBase + "circus.mid")
  Set backgroundSnd = importSnd.Sound.RepeatForever()

  Dim IELogos(8)
  Set IELogos(0) = ImportImage(imgBase + "clogo1.gif")
  Set IELogos(1) = ImportImage(imgBase + "clogo2.gif")
  Set IELogos(2) = ImportImage(imgBase + "clogo3.gif")
  Set IELogos(3) = ImportImage(imgBase + "clogo4.gif")
  Set IELogos(4) = ImportImage(imgBase + "clogo5.gif")
  Set IELogos(5) = ImportImage(imgBase + "clogo6.gif")
  Set IELogos(6) = ImportImage(imgBase + "clogo7.gif")
  Set IELogos(7) = ImportImage(imgBase + "clogo8.gif")
  Set IELogos(8) = ImportImage(imgBase + "clogo9.gif")

  'Call the Movie(), which cycles through the IELogos images every second.
  Set imageCube = Movie(IELogos)
  
  'Set lights.
  Set pLight = PointLight.Transform(Translate3(0, 0, 4))
  Set dLight = DirectionalLight.Transform(Rotate3(YVector3, 0.5))
    
  'Manipulate the geometry.
  Set geo = geo.Transform(Scale3Uniform(0.5)).Texture(imageCube.MapToUnitSquare())

  'Create the four cubes and set them to rotate.
  Set srate = Mul(LocalTime, DANumber(9))
  Set cube1 = geo.Transform(Compose3(Translate3(0, 0, 2), _
    Rotate3Anim(Vector3(0, 1, 0), Mul(DANumber(0.04), srate))))
  Set cube2 = geo.Transform(Compose3(Translate3(0, 0, -2), _
    Rotate3Anim(Vector3(0, 1, 1), Mul(DANumber(0.03), srate))))
  Set cube3 = geo.Transform(Compose3(Translate3(2, 0, 0), _
    Rotate3Anim(Vector3(0, 0, 1), Mul(DANumber(0.02), srate))))
  Set cube4 = geo.Transform(Compose3(Translate3(-2, 0, 0), _
    Rotate3Anim(Vector3(1, 1, 1), Mul(DANumber(0.03), srate))))
    
  'Do the final prep work on the model.
  Set finalgeo = UnionGeometry(cube1, UnionGeometry(cube2, UnionGeometry(cube3, cube4)))
  Set Camera = PerspectiveCamera(5.5, 4.5).Transform(Scale3(45, 45, 1))
    
  'Make the complete image rotate.
  Set finalgeo = finalgeo.Transform(Rotate3Anim(YVector3, Integral(changingRate)))
  Set finalgeo = UnionGeometry(finalgeo, pLight)
    
  'Render and display the image.
  Set rendered_geo = finalgeo.Render(Camera)
  
  DAViewerControlWindowed.UpdateInterval = 0.2
  
  'Set the image.
  DAViewerControlWindowed.Image = Overlay(rendered_geo, DAStatics.SolidColorImage(White))
  
  'Set the sound.
  DAViewerControlWindowed.Sound = backgroundSnd
  
  'Start the animation.
  DAViewerControlWindowed.Start
End Sub

Function Movie(IELogos)
  Set movieArray = DAStatics.DAArray(IELogos)
  Set Movie = movieArray.NthAnim(DAStatics.Mod(LocalTime, DANumber(9)))
End Function









