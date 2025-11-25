// Copyright (c) 1997 Microsoft Corporation

import com.ms.dxmedia.*;
import java.net.*; //added to support URL's

// <Tutorial Section=2.0 Title="Section2: Walking in the 3D space">
// This section will guide you through the process of creating a mini solar
// system and using the mouse drag to walk in the 3D space.

class MiniSolar extends Statics {

  // These four helper functions are used to create the mini solar system.
  Transform3Bvr rotateWithFrequency(double frequency) {
    return rotate(yVector3, mul(toBvr(2 * Math.PI * frequency), localTime));
  }

  GeometryBvr orbit(double frequency, double radius, GeometryBvr geo) {
    return geo.transform(compose(rotateWithFrequency(frequency),
                                 translate(toBvr(radius), bvr0, bvr0)));
  }

  GeometryBvr body(ImageBvr map, double size) {
    GeometryBvr sphere = importGeometry(buildURL(_geoBase,"sphere.x"), emptyGeometry, null, null, null);
    return sphere.transform(scale3(toBvr(size))).
                  texture(map.mapToUnitSquare());
  }

  GeometryBvr rotatingBody(GeometryBvr geo, double frequency) {
    return geo.transform(rotateWithFrequency(frequency));
  }

  MiniSolar(URL imgBase, URL sndBase, URL geoBase) {
	_imgBase = imgBase;
	_sndBase = sndBase;
	_geoBase = geoBase;
    // Create the sun, earth, and moon.  The sunSystem is the union of the
    // three objects.
    GeometryBvr moon = body(importImage(buildURL(_imgBase,"moon.jpg"), emptyImage, null, null, null), 0.5);
    GeometryBvr earth = body(importImage(buildURL(_imgBase,"earth.jpg"), emptyImage, null, null, null), 1.0);
    GeometryBvr sun = body(importImage(buildURL(_imgBase,"sun.jpg"), emptyImage, null, null, null), 2.0);

    GeometryBvr earthSystem = union(rotatingBody(earth, 1),
                                    orbit(1.0/4.0, 2.0, rotatingBody(moon,
                                                                     1.0/28.0)));
    GeometryBvr sunSystem = union(rotatingBody(sun, 0),
                                  orbit(1.0/14.0, 6.0, earthSystem));

    // The reset button reset the camera to its default position.  This is
    // handy when people get lost in space.  Put the reset button at the pixel
    // position of (0, -130).
    ImageBvr imgReset = importImage(buildURL(_imgBase,"reset.gif"), emptyImage, null, null, null);
    PickableImage pimReset = new PickableImage(
                                 imgReset.transform(translate(bvr0, 
																 mul(toBvr(-130),pixelBvr))));
    _resetSolarEv = new AppTriggeredEvent();
    DXMEvent resetWalkEv = orEvent(andEvent(leftButtonDown, pimReset.getPickEvent()),
                                   _resetSolarEv);

    // The camera is set by the walk object, which we'll create later.
    CameraBvr cam = CameraBvr.newUninitBvr();

    // We want all the geometries to fit in the view window.  So we'll
    // calculate the size of the sunSystem.
    // The radius of the sunSystem system is 8 unit,
    // the radius of the moon is 0.25 unit,
    // give 1 unit of space for the margin,
    // So the size of the sunSystem is 2(8+0.25)+1 = 17.5.
    // Scale the model to fit the window, then rotate the model about the
    // x axis a little so it looks like we're looking at the model from above.
    Transform3Bvr xf = compose(rotate(xVector3, toBvr(Math.PI/6)),
                               scale3(mul(mul(toBvr(300),pixelBvr), toBvr(1.0/17.5))));

    ImageBvr imgSolar = overlay(union(sunSystem.transform(xf), directionalLight).render(cam),
                           importImage(buildURL(_imgBase,"stars.jpg"), emptyImage, null, null, null));

    // Make the solar image pickable and use it as the walk event.
    PickableImage pimSolar = new PickableImage(imgSolar);
    DXMEvent walkEv = andEvent(leftButtonDown, pimSolar.getPickEvent());
    Walk walk = new Walk(walkEv, resetWalkEv);
    cam.init(walk._cam);

    GeometryBvr minisolar = union(sunSystem,
                                  soundSource(importSound(buildURL(_sndBase,"moon.mp2"), null, silence, null, null, null).loop()));
    _snd = minisolar.render(walk._mic);
    _img = overlay(pimReset.getImageBvr(), pimSolar.getImageBvr());
  }

  public void cleanup() {
    _resetSolarEv = null;
    _snd = null;
    _img = null;
    _imgBase= null;
    _sndBase= null;
    _geoBase= null;
  }

  static AppTriggeredEvent _resetSolarEv;
  final static NumberBvr bvr0 = toBvr(0);
  SoundBvr _snd;
  ImageBvr _img;
  URL _imgBase;
  URL _sndBase;
  URL _geoBase;

}

// The Walk class calculates the camera behavior based on the walkEv and
// resetWalkEv.
class Walk extends Statics implements UntilNotifier {
  Walk(DXMEvent walkEv, DXMEvent resetWalkEv) {

    _walkEv = walkEv;
    _stopEv = leftButtonUp;
    
    _walking = false;

    Transform3Bvr camxf = (Transform3Bvr)untilNotify(identityTransform3,
                                                     _walkEv,
                                                     this);

    // Whenever the resetWalkEv occurs, we'll restart the camera transform
    // behavior, effectively setting it to the identity transform.  Since we
    // don't want xfResettable to ever restart again, we apply runOnce()
    // to it.  This is so that if a client of the MiniSolar class makes
    // a reactive behavior out of a MiniSolar object, the camera position
    // will not be reset unexpectively.
    Transform3Bvr xfResettable = Transform3Bvr.newUninitBvr();
    xfResettable.init(until(camxf, resetWalkEv, xfResettable));

    Transform3Bvr xfRunOnce = (Transform3Bvr)xfResettable.runOnce();

    _cam = perspectiveCamera(toBvr(2), toBvr(1.9)).transform(xfRunOnce);
    _mic = defaultMicrophone.transform(compose(translate(MiniSolar.bvr0, MiniSolar.bvr0, toBvr(2)), xfRunOnce));
  }

  public Behavior notify(Object evData,
                         Behavior currRunningBvr,
                         BvrsToRun blst) {

    Transform3Bvr currXf = (Transform3Bvr)currRunningBvr;

    if (_walking == false) {

      // Pull apart the pair that comes from andEvent and from the
      // pick event pair, ultimately get at the local mouse
      // coord behavior.
      PairObject andEventPair  = (PairObject)evData;
      PairObject pickPair   = (PairObject)(andEventPair.getSecond());
      Vector2Bvr localMouse = (Vector2Bvr)(pickPair.getSecond());

      // Set the state to saying that we're walking.
      _walking = true;

      // Use y position as depth navigation, x position as width navigation
      NumberBvr zVec = localMouse.getY();
      NumberBvr xVec = localMouse.getX();

      Transform3Bvr xf = compose(translate(xVec, MiniSolar.bvr0,
                                           neg(mul(toBvr(30), zVec))),
                                           currXf);

      return untilNotify(xf, _stopEv.snapshotEvent(xf), this);

    } else {

      // Releasing.  Freeze the camera where it is, and go
      // back to waiting for the walk event.
      Transform3Bvr xf = (Transform3Bvr) evData;

      // Set the state to saying we're waiting to walk again.
      _walking = false;

      return untilNotify(xf, _walkEv, this);
    }
  }

  public void cleanup() {
    _walkEv = null; 
	_stopEv = null;
    _cam = null;
    _mic = null;
  }

  DXMEvent          _walkEv, _stopEv;
  boolean           _walking;
  CameraBvr         _cam;
  MicrophoneBvr     _mic;
}
// </Tutorial>
