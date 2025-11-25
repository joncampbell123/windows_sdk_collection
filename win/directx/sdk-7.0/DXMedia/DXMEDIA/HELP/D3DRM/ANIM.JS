
function HdgrphControl(){
	HeadGraphAnim.innerHTML = ' &nbsp; <OBJECT ID="DAControl" STYLE="position:absolute; left:0; top:11;width:702;height:90" CLASSID="CLSID:B6FFC24C-7E13-11D0-9B47-00C04FC2F51D"></OBJECT>';
	HdgrphAnimate();
	window.setInterval("DestroyAnim()", 4300);
}

function DestroyAnim(){
	HeadGraphAnim.innerHTML = '';
}
function HdgrphAnimate(){
  // The DirectAnimation library
  m = DAControl.PixelLibrary;
  
  glowBase = m.importImage(sRelPath + "art/glow.gif");
  xImage = m.importImage(sRelPath + "art/newtransx.gif");
  xImage = xImage.Transform(m.Translate2(-303,0));

    glowPath1 = m.Polyline( new Array( 295,26, 305,26, 305,50, 310,57, 526,57, 
    543,40, 543,17, 682,17, 682,23 ) );

  glowPath2 = m.PolyLine( new Array( 44,41, 44,25, 52,16, 84,16, 106,16 ) );

  glowPath3 = m.PolyLine( new Array( 57,55, 69,55, 78,65, 92,65, 99,58,
    99,82, 95,85, 95,93, 132,93, 132,78,
    303,78, 315,66, 556,66, 556,17, 682,17,
    675,23 ) );

  glowPath4 = m.PolyLine( new Array( 29,55, 19,55, 19,31, 38,30, 38,21,
    46,16, 106,16 ) );

  glowPath5 = m.PolyLine( new Array( 45,62, 45,81, 37,81, 41,85, 46,85,   
  51,91, 68,91, 66,81, 90,81, 90,65, 94,65, 105,47 ) );

  glow1 = glowBase.Transform( moveGlow( glowPath1, 5 ) );
  glow2 = glowBase.Transform( moveGlow( glowPath2, 2 ) );
  glow3 = glowBase.Transform( moveGlow( glowPath3, 8 ) );
  glow4 = glowBase.Transform( moveGlow( glowPath4, 2.5 ) );
  glow5 = glowBase.Transform( moveGlow( glowPath5, 2 ) );
  
  finalImg = m.Overlay(xImage, m.Overlay( glow5, m.Overlay( glow4, 
    m.Overlay( glow3, m.Overlay( glow2, glow1)))));

  // set the model, an image, as the model to be displayed
  DAControl.Image = m.Until(finalImg, m.Timer(4), m.EmptyImage);//m.Overlay(finalImg, background);
  // set the background in case of a non-windowless browser (like IE3)
  DAControl.BackgroundImage = m.SolidColorImage(m.Blue);         
  DAControl.Start()
  }

  function moveGlow(path, speed)  {
    imDimX = 702;
    imDimY = 120;

    coordsXf = m.Translate2(-imDimX/2, -imDimY/2);
    path = path.Transform(coordsXf);
    return xf = m.FollowPath(path, speed).RepeatForever();
  }
