//
// This applet illustrates a simple hello world example,
// it displays a hello message on a blue background.
//
// Copyright (c) 1997 Microsoft Corporation

import com.ms.dxmedia.*;

// This class extends the DXMApplet class.  The model you set in this class,
// by calling the setModel() method, is the model that will be displayed.
public class Hello extends DXMApplet {
  public void init() {
    super.init() ;
    setModel(new HelloModel());
  }
}

// This class extends the Model class.  The createModel method in this class
// is where you construct your animation.
class HelloModel extends Model {

  // We'll just display a hello message over a solid blue background.
  public void createModel(BvrsToRun blist)
  {
    FontStyleBvr fs = defaultFont.color(black);
    ImageBvr helloImg = stringImage(toBvr("Hello, World"), fs);
    ImageBvr backImg = solidColorImage(blue);
    // setImage() sets the image that actually gets displayed 
    setImage(overlay(helloImg, backImg));
  }
}
