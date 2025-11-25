// <Tutorial Section=1.0 Title="GeoApplet2: A Cube Spinning at 5 Frames Per Second">
/**
This applet constructs a spinning cube to fit in the view window.  It
illustrates how to import a geometry, manipulate it, and view it
with a camera.  Unlike GeoApplet1, which uses time-varying behaviors
for the rotation angle of the spinning cube, this applet uses switchers
to control these same parameters.  It also explicitly controls the sampling
and rendering of the model by overriding the tick() method.
<BR>
**/
// </Tutorial>


// <Tutorial Section=1.1>

import com.ms.dxmedia.*;            // direct animation libraries
import java.awt.*;                  // for getting the applet dimension
import java.net.*;                  // for URLs
import java.applet.Applet;

// Create a DXMCanvas object and add it to the applet.
public class GeoApplet2 extends Applet {
    public void init() {
        setLayout(new BorderLayout());
        GeoCanvas cv = new GeoCanvas(this);
        add("Center", cv);
    }
}

// Override the tick method in DXMCanvas class to control the frame
// rate.  Also create a switcher for each animating angle of the cube.
class GeoCanvas extends DXMCanvas {

  public GeoCanvas(GeoApplet2 geoa) {

    // The rotation angles of the cube are switchers.  Initialize each of
    // them to a zero number behavior.
    NumberBvr zeroNum = Statics.toBvr(0);
    _yAngleSw = new ModifiableBehavior(zeroNum);
    _zAngleSw = new ModifiableBehavior(zeroNum);

    // Now set the model.
    setModel(new GeoModel2(geoa, this));
  }

  // Override the tick() to control the frame generation explicitly.
  // Before calling super.tick(), update the values of the switchers
  // for the next frame.  This is called automatically by the system.
  public void tick() {

    // Calculate the next tick time.
    nextTickTime += 1/DESIREDFRAMERATE;

    // getCurrentTime returns the DirectAnimation global time.  It starts
    // at zero when the model is started.  Switch the rotating angle of
    // the cube based on the current time here.
    double currTime = getCurrentTime();
    _yAngleSw.switchTo(Statics.toBvr(currTime));
    _zAngleSw.switchTo(Statics.toBvr(currTime*1.3));

    // Explicitly sample and display the model.  Without this, you'd
    // see a blank window.
    super.tick();

    // Calculate how long the applet needs to sleep to achieve the desired 
    //  frame rate. sleep() takes milliseconds so multiply by 1000.
    long remainingTime = Math.round(1000*(nextTickTime - getCurrentTime()));

    // If you're running behind, sleep(0).
    if (remainingTime < 0) remainingTime = 0;

    try { Thread.sleep(remainingTime); } catch(InterruptedException e) {}
  }

  // This is an approximate frame rate.
  static final double DESIREDFRAMERATE = 5;
  double nextTickTime = 0;
  ModifiableBehavior _yAngleSw, _zAngleSw;
}

// This class extends the Model class.  The createModel method in this class
// is where you construct your animation.
class GeoModel2 extends Model {

  // Get the size of the applet in the constructor, and stores it in the
  // member variables.
  GeoModel2(GeoApplet2 geoa, GeoCanvas cv) {

    // Get the size of the applet.
    Dimension dim = geoa.getSize();

    // The base unit of measure for DirectAnimation is the meter.
    // Convert the size into meters by multiplying it with the pixelBvr.
    _halfWidthNum = mul(toBvr(dim.width*0.5),pixelBvr);
    _halfHeightNum = mul(toBvr(dim.height*0.5),pixelBvr);

    // The spinning angles of the cube are switchers defined in the
    // GeoApplet2 class.  Save the references away so you can use them
    // in the createModel() method.
    _yAngleNum = (NumberBvr)cv._yAngleSw.getBvr();
    _zAngleNum = (NumberBvr)cv._yAngleSw.getBvr();
  }

  // This function scales the geometry uniformly to fit in the unit box
  // and centers it at the origin.
  GeometryBvr mapToCenteredUnitBox(GeometryBvr geo) {

    // Find the extent and the center of the geometry.
    Bbox3Bvr bbx3 = geo.boundingBox();
    Point3Bvr minPt3 = bbx3.getMin();
    Point3Bvr maxPt3 = bbx3.getMax();

    Vector3Bvr extVec3 = sub(maxPt3, minPt3);
    Point3Bvr centerPt3 = add(minPt3, extVec3.div(toBvr(2)));

    NumberBvr maxExtNum = max(extVec3.getX(),
                              max(extVec3.getY(),
                                  extVec3.getZ()));

    // Move the object to the origin, and resize it.
    Transform3Bvr normalizingXf3 =
                     compose(scale3(div(toBvr(1), maxExtNum)),
                             translate(sub(origin3, centerPt3)));

    return geo.transform(normalizingXf3);
  }

  // Create the animation in the createModel method.
  public void createModel(BvrsToRun blist) {

    // Build up a URL to import relative to.
    URL mediaBase = getImportBase();
    URL geoBase = buildURL(mediaBase, "geometry/");

    // Import a cube, scale it to the unit cube and center it at the origin.
    // The resulting cube has a bounding box of -0.5 to 0.5 in all 3 dimensions.

    GeometryBvr geo = importGeometry(buildURL(geoBase, "cube.x"));
    geo = mapToCenteredUnitBox(geo);

    // Spin the cube around the origin.  Find the smaller of the halfWidth and
    // halfHeight of the view window, and use it to resize the cube.  The
    // rendered image for the cube will be about half the size of the viewport.
    // The part of the cube that's exactly on the viewing plane (Z = 0) will
    // be exactly half the size of the viewport, the part that's behind the
    // plane will be smaller, and the part that's in front will be bigger.
    // The rotation angles around the yVector and zVector are switchers.
    // The applet that displays the model updates their values frequently
    // to create a spinning cube.
    NumberBvr minHalfExtNum = min(_halfWidthNum, _halfHeightNum);
    geo = geo.transform(
                compose(scale3(minHalfExtNum),  // sacle to fit half viewport
                        compose(rotate(yVector3, _yAngleNum),
                                rotate(zVector3, _zAngleNum))));

    // The hue of the color is a time-varying behavior.  You can mix 
    // imperative (modifying the switchers and controlling the frame loop
    // explicitly) and declarative (define the behaviors at a higher level)
    // programming in DirectAnimation, and they'll work just fine.
    geo = geo.diffuseColor(colorHsl(mul(localTime, toBvr(0.3)),
                                    toBvr(0.8), toBvr(0.6)));

    // The near plane should be bigger than the max Z (in this case,
    // sqrt(2)*minHalfExtNum after the cube is scaled to fit half viewport)
    // of the bounding box of the spinning cube, so the cube doesn't get
    // clipped out  The projection point determines the amount of perspective
    // you'll see.  The farther away it is from the viewing plane, the less
    // perspective will result.  Set it to 5 times the near plane here.
    NumberBvr maxZNum = mul(sqrt(toBvr(2)), minHalfExtNum);
    NumberBvr projPointNum = mul(toBvr(5), maxZNum);
    CameraBvr cam = perspectiveCamera(projPointNum, maxZNum);

    // Overlay the cube on top of the color image background,
    // and get the image displayed.
    setImage(overlay(union(geo, directionalLight).render(cam),
                     solidColorImage(red)));
  }

  // max: returns max(x, y)
  public static NumberBvr max(NumberBvr x, NumberBvr y) {
    return (NumberBvr) cond(gt(y, x), y, x);
  }

  // min: returns min(x, y)
  public static NumberBvr min(NumberBvr x, NumberBvr y) {
    return (NumberBvr) cond(lt(y, x), y, x);
  }

  private NumberBvr _yAngleNum, _zAngleNum;
  private NumberBvr _halfWidthNum, _halfHeightNum;
}

// </Tutorial>
