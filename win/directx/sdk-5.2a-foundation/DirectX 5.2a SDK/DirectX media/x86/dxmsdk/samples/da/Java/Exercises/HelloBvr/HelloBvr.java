//
// This applet displays a hello message on a blue background,
// with a time varying color.
//
// Copyright (c) 1997 Microsoft Corporation

import com.ms.dxmedia.*;

// The model you set in this class is the model that will be displayed.
public class HelloBvr extends DXMApplet {
  public void init() {
    super.init() ;
    setModel(new HelloBvrModel());
  }
}

// The createModel method in this class
// is where you construct your animation.
class HelloBvrModel extends Model {

  // The mode is a hello message with a time varying
  // color over a solid blue background.
  public void createModel(BvrsToRun blist)
  {
    // construct a color in the HSL space with a time varying hue
    ColorBvr textClr = colorHsl(localTime, toBvr(0.5), toBvr(0.5));
    // apply the color to the font.
    FontStyleBvr fs = defaultFont.color(textClr);  
    ImageBvr helloImg = stringImage(toBvr("Hello, World"), fs);
    ImageBvr backImg = solidColorImage(blue);
    
    setImage(overlay(helloImg, backImg));
  }
}
