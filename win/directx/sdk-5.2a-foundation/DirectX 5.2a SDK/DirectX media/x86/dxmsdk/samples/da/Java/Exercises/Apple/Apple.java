// This applet illustrates importing and displaying an image
//
// Copyright (c) 1997 Microsoft Corporation

import com.ms.dxmedia.*;
import java.net.*;

class AppleModel extends Model {

  // Create the image in the createModel method.
  public void createModel(BvrsToRun blist) {
    // Build up a URL to import relative to.
    URL mediaBase = getImportBase();
    URL imgBase = buildURL(mediaBase, "Image/");    

    // Import a few bitmaps bitmap.
    ImageBvr pictImg = importImage(buildURL(imgBase, "foliageUtah.jpg"));
    ImageBvr bgImg = importImage(buildURL(imgBase, "sandbg1.jpg"));
    ImageBvr appleImg = importImage(buildURL(imgBase, "apple.gif"));

    // construct an apple sprite with a time varying opacity
    appleImg = appleImg.opacity(abs(sin(localTime)));

    // a transform for sinosoidal oscillation
    Transform2Bvr apple1Tf = rotate(mul(sin(localTime), toBvr(Math.PI/6)));     
    // another for sinosoidal translation
    Transform2Bvr apple2Tf = translate(mul(sin(localTime), toBvr(1.5 * cm)), toBvr(0));

    // construct an apple that oscillates both raidally and linearly
    appleImg = appleImg.transform(apple1Tf);
    appleImg = appleImg.transform(apple2Tf);

    // tile the background sprite, overlay it with a picture and the 
    // animate apple
    setImage(overlay(overlay(appleImg, pictImg), bgImg.tile()));
  }
}

public class Apple extends DXMApplet {
  public void init() {
    super.init() ;
    setModel(new AppleModel());
  }
}
