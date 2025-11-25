// This applet illustrates importing and displaying an image
//
// Copyright (c) 1997 Microsoft Corporation

import com.ms.dxmedia.*;
import java.net.*;

class ImportModel extends Model {

  // Create the image in the createModel method.
  public void createModel(BvrsToRun blist) {
    // Build up a URL to import relative to.
    URL mediaBase = getImportBase();
    URL imgBase = buildURL(mediaBase, "image/");    

    // Create an image behavior by importing a bitmap.
    ImageBvr img = importImage(buildURL(imgBase, "foliageUtah.jpg"));

    setImage(overlay(img, solidColorImage(blue)));
  }
}

public class Import extends DXMApplet {
  public void init() {
    super.init() ;
    setModel(new ImportModel());
  }
}
