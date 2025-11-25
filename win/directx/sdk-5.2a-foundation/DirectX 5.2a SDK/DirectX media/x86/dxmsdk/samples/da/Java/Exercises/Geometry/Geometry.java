// This applet constructs a spinning cube with a time varying color.  It
// illustrates how one can import a geometry, manipulate it, and view it
// with a camera.
//
// Copyright (c) 1997 Microsoft Corporation

import com.ms.dxmedia.*;
import java.net.*;

// This class extends the Model class.  The createModel method in this class
// is where you construct your animation.
class GeometryModel extends Model {

  // Create the animation in the createModel method.
  public void createModel(BvrsToRun blist) {

    // Build up a URL to where the geometry resides (import relative to it).
    URL mediaBase = getImportBase();
    URL geoBase = buildURL(mediaBase, "geometry/");

    // Import a [-1,1]**3 cube, 2 meters in each dimension
    GeometryBvr cubeGeo = importGeometry(buildURL(geoBase, "cube.x"));

    // Spin it around the Z and Y axis.
    cubeGeo = cubeGeo.transform(compose(rotate(yVector3, localTime),
                        rotate(zVector3, mul(localTime, toBvr(1.3)))));

    // Make the color time-varying.
    cubeGeo = cubeGeo.diffuseColor(colorHsl(mul(localTime, toBvr(0.3)),
                             toBvr(0.8), toBvr(0.6)));

    // set a camera by specifying projection point and near clipping plane
    CameraBvr camera = perspectiveCamera(toBvr(5), toBvr(2));

    // render cube into an image
    ImageBvr cubeImg = union(cubeGeo, directionalLight).render(camera);
    // scale the image to fit in viewport, reduce size by 1/35th, about 3cms
    cubeImg = cubeImg.transform(scale2(toBvr(1.0/70.0)));

    // set model to the result.
    setImage(overlay(cubeImg, solidColorImage(blue)));
  }
}

public class Geometry extends DXMApplet {
  public void init() {
    super.init() ;
    setModel (new GeometryModel());
  }
}