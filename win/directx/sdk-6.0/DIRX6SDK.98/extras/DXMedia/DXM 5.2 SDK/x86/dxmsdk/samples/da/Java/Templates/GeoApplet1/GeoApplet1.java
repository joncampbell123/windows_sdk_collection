// <Tutorial Section=1.0 Title="GeoApplet1: A Spinning Cube">
/**
This applet constructs a spinning cube to fit in the view window.  It
illustrates how to import a geometry, manipulate it, and view it
with a camera.
<BR>
**/
// </Tutorial>
//

// <Tutorial Section=1.1>
import com.ms.dxmedia.*;        // direct animation libraries
import java.awt.*;              // for getting the applet dimension
import java.net.*;              // for URLs

// This class extends the DXMApplet class.  The model you set in this class,
// by calling the setModel() method, is the model that will be displayed.
public class GeoApplet1 extends DXMApplet {

  // Set the model in the init() method.
  public void init() {

    // Always call the super classes init first to ensure codeBase is set.
    super.init() ;

    // Now set the model.
    setModel (new GeoModel1(this));
  }
}

// This class extends the Model class.  The createModel method in this class
// is where you construct your animation.
class GeoModel1 extends Model {

  // Get the size of the applet in the constructor, and stores it in the
  // member variables.
  GeoModel1(DXMApplet dxma) {

    // Get the size of the applet.
    Dimension dim = dxma.getSize();

    // The base unit of measure for DirectAnimation is the meter.
    // Convert the size into meters by multiplying it with the pixelBvr.
    _halfWidthNum = mul(toBvr(dim.width*0.5),pixelBvr);
    _halfHeightNum = mul(toBvr(dim.height*0.5),pixelBvr);
  }

  // This function scales the geometry uniformly to fit in the unit box
  // and centers it at the origin.  The bounding box of the resulting
  // geometry will be -0.5 to 0.5 in its largest dimension.
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
    // Translate takes effect before scale is how compose works.
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
    // halfHeight of the view window, and use it to scale the cube so that the
    // rendered image for the cube will be about half the size of the viewport.
    // The part of the cube that's exactly on the viewing plane (Z = 0) will
    // be exactly half the size of the viewport, the part that's behind the
    // plane will be smaller, and the part that's in front will be bigger.
    NumberBvr minHalfExtNum = min(_halfWidthNum, _halfHeightNum);
    geo = geo.transform(compose(scale3(minHalfExtNum),
                          compose(rotate(yVector3, localTime),
                            rotate(zVector3, mul(localTime, toBvr(1.3))))));

    // Make the color time-varying.
    geo = geo.diffuseColor(colorHsl(mul(localTime, toBvr(0.3)),
                                    toBvr(0.8), toBvr(0.6)));

    // The near plane should be bigger than the max Z (in this case,
    // sqrt(2)*minHalfExtNum after the cube is scaled to fit half viewport)
    // of the bounding box of the spinning cube, so the cube doesn't get
    // clipped out. The projection point determines the amount of perspective
    // you'll see. The farther away it is from the viewing plane, the less
    // perspective will result.  Here, it's set to 5 times the near plane.
    NumberBvr maxZNum = mul(sqrt(toBvr(2)), minHalfExtNum);
    NumberBvr projPointNum = mul(toBvr(5), maxZNum);
    CameraBvr cam = perspectiveCamera(projPointNum, maxZNum);

    // Overlay the cube on top of the color image background,
    // and get the image displayed.
    setImage(overlay(union(geo, directionalLight).render(cam),
                     solidColorImage(blue)));
  }

  // max: returns max(x, y)
  public static NumberBvr max(NumberBvr x, NumberBvr y) {
    return (NumberBvr) cond(gt(y, x), y, x);
  }

  // min: returns min(x, y)
  public static NumberBvr min(NumberBvr x, NumberBvr y) {
    return (NumberBvr) cond(lt(y, x), y, x);
  }

  private NumberBvr _halfWidthNum, _halfHeightNum;
}
// </Tutorial>
