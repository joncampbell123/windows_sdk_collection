// This applet illustrates importing, scale to fit and rotating an image
//
// Copyright (c) 1997 Microsoft Corporation

import com.ms.dxmedia.*;
import java.net.*;

class ImgModelEx extends Model {

  // Create the animation in the createModel method.
  public void createModel(BvrsToRun blist) {
    // Build up a URL to import relative to.
    URL mediaBase = getImportBase();
    URL imgBase = buildURL(mediaBase, "image/");    

    // Create an image behavior by importing a bitmap.
    ImageBvr img = importImage(buildURL(imgBase, "phantom.jpg"));

    // make the image wiggle
    img = img.transform(rotate(mul(sin(localTime), toBvr(0.25))));
    // make the image oscillate
    img = img.transform(translate(vector2(mul(sin(localTime), 
                          toBvr(2*cm)), toBvr(0))));

    setImage(overlay(img, solidColorImage(black)));
  }
}

public class Image extends DXMApplet {
  public void init() {
    super.init() ;
    setModel(new ImgModelEx());
  }
}
