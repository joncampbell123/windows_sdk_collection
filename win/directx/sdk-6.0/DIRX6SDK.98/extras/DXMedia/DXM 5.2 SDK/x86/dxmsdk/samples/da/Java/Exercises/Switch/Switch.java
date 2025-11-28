
// Copyright (c) 1997 Microsoft Corporation

import java.awt.*;
import java.applet.*;
import java.net.*;
import com.ms.dxmedia.*;

// Constructs a model where the image and the sound are switchers,
// the model provides set methods that switches its constituent
// sound and image into new values, upon external events.
class SwitchModel extends Model {
  public void createModel(BvrsToRun blst) {
    // Build up a URL to import relative to.
    URL mediaBase = getImportBase();
    URL imgBase = buildURL(mediaBase, "image/");    
    URL sndBase = buildURL(mediaBase, "sound/");    

    // Import the image and sound values.
    _canyonImg = importImage(buildURL(imgBase, "foliageUtah.jpg"));
    _kidsImg = importImage(buildURL(imgBase, "kidsUtah.jpg"));
    _oceanImg = importImage(buildURL(imgBase, "hiddenBeachSG.jpg"));
    _canyonSnd = importSound(buildURL(sndBase, "earth.mp2"), null).loop();
    _kidsSnd = importSound(buildURL(sndBase, "torain.mp2"), null).loop();
    _oceanSnd = importSound(buildURL(sndBase, "wave.mp2"), null).loop();

    // initialize the switchers, and extract their behaviors
    _imgSw = new ModifiableBehavior(_canyonImg);
    ImageBvr _img = (ImageBvr)_imgSw.getBvr();
    _sndSw = new ModifiableBehavior(_canyonSnd);
    SoundBvr _snd = (SoundBvr)_sndSw.getBvr();
      
    // set the image and sounds of the model to the switch behaviors
    setImage(overlay(_img, solidColorImage(blue)));
    setSound(_snd.gain(0.4));
  }

  // these methods switch the behaviors to new values.
  // the new values are started at the time of switching.
  public void setCanyon() {
    _imgSw.switchTo(_canyonImg);
    _sndSw.switchTo(_canyonSnd);
  }

  public void setKids() {
    _imgSw.switchTo(_kidsImg);
    _sndSw.switchTo(_kidsSnd);
  }

  public void setOcean() {
    _imgSw.switchTo(_oceanImg);
    _sndSw.switchTo(_oceanSnd);
  }

  // the switchers
  private ModifiableBehavior _imgSw;
  private ModifiableBehavior _sndSw;

  // the media values
  private ImageBvr _canyonImg;
  private ImageBvr _kidsImg;
  private ImageBvr _oceanImg;
  private SoundBvr _canyonSnd;
  private SoundBvr _kidsSnd;
  private SoundBvr _oceanSnd;
}

// the applet is a DA canvas and a panel of buttons
public class Switch extends Applet {
  public void init() {
    setLayout(new BorderLayout());
    DaSwitch DaCv = new DaSwitch();
    Panel ctrls = new AwtControls(DaCv._model);
    add("Center", DaCv);
    add("North", ctrls);
  }
}

// the DA canvas
class DaSwitch extends DXMCanvas {
  DaSwitch() {
    _model = new SwitchModel();
    setModel(_model);
  }
  SwitchModel _model;
}

// a panel with three buttons,
// it calls the model's corresponding set methods 
// upon button events.
class AwtControls extends Panel {

  // Place the scrollbar at the center, then two buttons on either side.
  AwtControls(SwitchModel switchModel) {
    _switchModel = switchModel;
    setLayout(new FlowLayout());
    Panel buttons = new Panel();
    buttons.add(new Button("Canyon"));
    buttons.add(new Button("Kids"));
    buttons.add(new Button("Ocean"));
    add(buttons);
  }

  // The event handler when any buttons are pressed.
  public boolean handleEvent(Event ev) {
    switch (ev.id) {
      case Event.ACTION_EVENT:
        if (ev.arg == "Canyon") {
          _switchModel.setCanyon();
          return true;
        } else if (ev.arg == "Kids") {
          _switchModel.setKids();
          return true;
        } else if (ev.arg == "Ocean") {
          _switchModel.setOcean();
          return true;
        }
    }
    return false;
  }
  SwitchModel _switchModel;
}
