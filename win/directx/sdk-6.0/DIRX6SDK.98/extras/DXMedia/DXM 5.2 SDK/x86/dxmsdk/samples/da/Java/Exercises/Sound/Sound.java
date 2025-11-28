// This applet illustrates importing, scale to fit and rotating an image
//
// Copyright (c) 1997 Microsoft Corporation

import com.ms.dxmedia.*;
import java.net.*;

class SoundModel extends Model {

  // Create the animation in the createModel method.
  public void createModel(BvrsToRun blist) {
    // Build up a URL to import relative to.
    URL mediaBase = getImportBase();
    URL imgBase = buildURL(mediaBase, "image/");    
    URL sndBase = buildURL(mediaBase, "sound/");

    // Create an image behavior by importing a bitmap.
    ImageBvr img = importImage(buildURL(imgBase, "phantom.jpg"));
    // Create a sound behavior by importing a wave file.
    SoundBvr snd = importSound(buildURL(sndBase, "earth.mp2"), null);

    // make the image wiggle
    img = img.transform(rotate(mul(sin(localTime), toBvr(0.25))));
    // make the image oscillate
    img = img.transform(translate(vector2(mul(sin(localTime), 
                          toBvr(2*cm)), toBvr(0))));

    setImage(overlay(img, solidColorImage(black)));

    // Create a sound that loops continuously.
    snd = snd.loop().pan(sin(localTime));

    // And set the sound that gets played using setSound()
    setSound(snd);
  }
}

public class Sound extends DXMApplet {
  public void init() {
    super.init() ;
    setModel(new SoundModel());
  }
}
