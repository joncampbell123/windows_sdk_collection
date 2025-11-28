//
// This applet displays a hello message on a blue background,
// with a color that cycles between time varying and red upon 
// left button events.
//
// Copyright (c) 1997 Microsoft Corporation

import com.ms.dxmedia.*;

// The createModel method in this class
// is where you construct your animation.
class HelloCycleModel extends Model {

  // The mode is a hello message with a time varying
  // color over a solid blue background.
  public void createModel(BvrsToRun blist)
  {
    // construct a color in the HSL space with a time varying hue
    ColorBvr textBvrClr = colorHsl(localTime, toBvr(0.5), toBvr(0.5));

    // construct a cyclic behavior that goes from time varying
    // to red and back upon left button presses.
    ColorBvr textClr = ColorBvr.newUninitBvr();
    textClr.init(until(textBvrClr, leftButtonDown, 
      until(red, leftButtonDown, textClr)));

    // apply the color to the font.
    FontStyleBvr fs = defaultFont.color(textClr);
      
    ImageBvr helloImg = stringImage(toBvr("Hello, World"), fs);

    ImageBvr backImg = solidColorImage(blue);
    
    setImage(overlay(helloImg, backImg));
  }
}

// The model you set in this class is the model that will be displayed.
public class HelloCycle extends DXMApplet {
  public void init() {
    super.init() ;
    setModel(new HelloCycleModel());
  }
}