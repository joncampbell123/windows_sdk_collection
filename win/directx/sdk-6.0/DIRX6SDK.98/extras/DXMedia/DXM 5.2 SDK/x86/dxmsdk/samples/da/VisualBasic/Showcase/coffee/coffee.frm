VERSION 5.00
Object = "{34F681D0-3640-11CF-9294-00AA00B8A733}#1.0#0"; "danim.dll"
Begin VB.Form coffee 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Coffee"
   ClientHeight    =   3585
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   7260
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   3585
   ScaleWidth      =   7260
   StartUpPosition =   3  'Windows Default
   Begin DirectAnimationCtl.DAViewerControlWindowed DAViewerControlWindowed 
      Height          =   3375
      Left            =   120
      OleObjectBlob   =   "coffee.frx":0000
      TabIndex        =   0
      Top             =   120
      Width           =   7095
   End
End
Attribute VB_Name = "coffee"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
' Visual Basic verion of Coffee
Dim mediaBase, geoBase, imgBase, sndBase As String
Private Sub Form_Load()
  mediaBase = CurDir + "\..\..\..\..\..\Media\"
  geoBase = mediaBase + "geometry\"
  imgBase = mediaBase + "image\"
  sndBase = mediaBase + "sound\"
  
  'Set the background
  Set imgBackGround = ImportImage(imgBase + "clouds_coffee.gif")

  'Create the final montage by layering the espresso machine, the cups and the steam
  'on top of each other.
  Set finalMtg = UnionMontage(UnionMontage(steamMontage(), _
    montage()), machineMontage())

  'Create the final image.
  Set finalImage = Overlay(finalMtg.Render(), Overlay(beans(), imgBackGround))
  Set finalImage = Overlay(finalImage, SolidColorImage(White))
  
  DAViewerControlWindowed.UpdateInterval = 0.2
  
  'Display the final image.
  DAViewerControlWindowed.Image = finalImage
  
  'Set the sound.
  DAViewerControlWindowed.sound = Mix(sound().Pan(-1), sound().Pan(1))

  'Start the animation.
  DAViewerControlWindowed.Start
End Sub
Function sound()
  'This function creates the sound, which starts out as silence, and then
  'changes to steam.mp2 when the image is clicked.
  Set steamDurationConst = DANumber(7.25)
  Set steamSound = ImportSound(sndBase + "steam.mp2").sound

  Set s0 = CreateObject("DirectAnimation.DASound")
    s0.Init DAStatics.Until(Silence, LeftButtonDown, _
      DAStatics.Until(steamSound.Gain(0.85), TimerAnim(steamDurationConst), s0))

  Set sound = s0
End Function
Function montage()
  'This function creates a montage of five cups, which rotate around the espresso
  'machine.  The orbit is constructed by the orbitCup function.
  pi = 3.14159265359
  total = 5
  Set cupImageX = EmptyMontage

  For i = 0 To total
    Set cupImageX = UnionMontage(cupImageX, orbitCup(Add(Mul(DANumber(i), _
      Mul(DANumber(2), Div(DANumber(pi), DANumber(total)))), LocalTime)))
  Next
  Set montage = cupImageX
End Function
Function beans()
  'This function creates the beans you see in the background.  Two images,
  'bean1.gif and bean2.gif are imported, and then moved across the screen
  'while being rotated.
  Set delay = DANumber(0.5)
  Set Size = DANumber(0.5)
  Set initBean1 = ImportImage(imgBase + "bean1.gif")
  Set initBean2 = ImportImage(imgBase + "bean2.gif")

  Set image0 = CreateObject("DirectAnimation.DAImage")
  Set image1 = CreateObject("DirectAnimation.DAImage")
  image0.Init DAStatics.Until(initBean1, TimerAnim(delay), image1)
  image1.Init DAStatics.Until(initBean2, TimerAnim(delay), image0)

  Set bean1 = image0.Transform(Scale2UniformAnim(Size))
  Set bean2 = image1.Transform(Scale2UniformAnim(Size))

  Set beans = Overlay(bean1.Transform(Translate2(-0.01, -0.01)), _
    bean2.Transform(Translate2(0.01, 0)))

  Set rain = beans.Tile()

  Set motion = Mul(LocalTime, Mul(DANumber(2), _
    Div(DANumber(0.03), DANumber(4))))

  Set beans = rain.Transform(Translate2Anim(Neg(motion), Neg(motion)))
End Function
Function machineMontage()
  'This function displays the espresso machine.
  Set steamDurationConst = DANumber(7.25)
  Set espreso1 = ImportImage(imgBase + "espreso1.gif")
  Set espreso2 = ImportImage(imgBase + "espreso2.gif")

  Set image5 = CreateObject("DirectAnimation.DAImage")
    image5.Init DAStatics.Until(espreso1, LeftButtonDown, _
      DAStatics.Until(espreso2, TimerAnim(steamDurationConst), image5))

  Set machineMontage = ImageMontage(image5, 0)
End Function
Function steamMontage()
  'This function displays the steam you see when you click on the image.
  Set steamDurationConst = DANumber(7.25)
  Dim steamImages(4)
  Set steamImages(0) = ImportImage(imgBase + "steam_1.gif")
  Set steamImages(1) = ImportImage(imgBase + "steam_2.gif")
  Set steamImages(2) = ImportImage(imgBase + "steam_3.gif")
  Set steamImages(3) = ImportImage(imgBase + "steam_4.gif")
  Set steamImages(4) = ImportImage(imgBase + "steam_5.gif")

  Set steamLen = DANumber(4)

  Set condition = GT(Add(Div(Mul(LocalTime, steamLen), _
    steamDurationConst), DANumber(1)), steamLen)

  Set result2 = Add(Div(Mul(LocalTime, steamLen), _
    steamDurationConst), DANumber(1))

  Set Index = Cond(condition, steamLen, result2)

  Set a = DAStatics.Array(steamImages)

  Set s0 = CreateObject("DirectAnimation.DAImage")
    s0.Init DAStatics.Until(EmptyImage, LeftButtonDown, _
      DAStatics.Until(a.NthAnim(Index), TimerAnim(steamDurationConst), s0))

  Set image1 = s0.Transform(Translate2(-0.0085, 0.002))

  Set steamMontage = ImageMontage(image1, -0.0001)
  End Function
Function orbitCup(angle)
  pi = 3.14159265359
  Set pos = Point3(0, 0.05, 0)
  Set pos = pos.Transform(Compose3(Rotate3Anim(XVector3, Mul(DANumber(7), _
    Div(DANumber(pi), DANumber(16)))), Rotate3Anim(ZVector3, angle)))

  Set cupAngle = LocalTime

  Set imageXX = cupImage(cupAngle).Transform(Compose2(Translate2Anim(pos.X, pos.Y), _
    Scale2UniformAnim(DAStatics.Sub(DANumber(1), _
      Mul(DAStatics.Abs(DAStatics.Cos(Div(angle, _
        DANumber(2)))), DANumber(0.5))))))

  Set orbitCup = ImageMontageAnim(imageXX, Neg(pos.Z))
End Function
Function cupImage(cupAngle)
  pi = 3.14159265359
  Dim cupImages
  cupImages = Array(ImportImage(imgBase + "cup1.gif"), _
    ImportImage(imgBase + "cup2.gif"), ImportImage(imgBase + "cup3.gif"), _
      ImportImage(imgBase + "cup4.gif"), ImportImage(imgBase + "cup5.gif"), _
        ImportImage(imgBase + "cup6.gif"), ImportImage(imgBase + "cup7.gif"), _
          ImportImage(imgBase + "cup8.gif"))

  Set Number = DANumber(7)
  Set Index = Add(DAStatics.Mod(Mul(Number, Div(cupAngle, _
    Mul(DANumber(2), DANumber(pi)))), Number), DANumber(1))

  Set a = DAStatics.Array(cupImages)

  Set cupImage = a.NthAnim(Index)
End Function
