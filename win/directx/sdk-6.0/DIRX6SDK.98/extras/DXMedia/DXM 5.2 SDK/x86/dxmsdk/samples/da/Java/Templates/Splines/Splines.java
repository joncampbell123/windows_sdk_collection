//
// <Tutorial Section=1.0 Title="Interactive Spline">
//
/**
 Demonstrates the support for splines in DirectAnimation, including: <br>
 - the correspondence between a path and behavior cubic splines, <br>
 - the use of draggable points for making the spline interactive <br>
 - the use of the derivative operation for obtaining the spline tangent <br>
 - animating a sprite along the spline with the orientation of the tangent <br>

**/

// </Tutorial>

// <Tutorial Section=5.0 Title="Boiler Plates">

import com.ms.dxmedia.*;    // All direct Animation classes
import java.net.*;          // the URL class 
import java.awt.*;          // the Dimension class
import java_utility.*;           // the DraggableImage class

// Splines is an applet that invokes SplinesModel to construct an
// interactive spline with the specified number of polynomial pieces
public class Splines extends DXMApplet {
  public void init() {
    super.init() ;
    setModel (new SplinesModel(this, 4));
  }
}

//
class SplinesModel extends Model {

  SplinesModel(DXMApplet dxma, int numPolys) {
    _numPolys = numPolys;

    // Get the size of the viewport.
    Dimension dim = dxma.getSize();
    // The base unit of measure for DirectX Media is the meter.
    // Convert the size into meters by multiplying it with the pixelBvr.
    _halfWidth = mul(toBvr(dim.width*0.5),pixelBvr);
    _halfHeight = mul(toBvr(dim.height*0.5),pixelBvr);
  }  

// </Tutorial>

  public void createModel(BvrsToRun listBvrs) {
    
// <Tutorial Section=3.0 Title="Travelling Car along the Spline">
    // build a URL to import images
    URL imgBase = buildURL(getImportBase(),"image/");
// </Tutorial>

// <Tutorial Section=2.0 Title="Construct an Interactive Spline">
//
// Construct a cubic BSpline with initial control points that are
// colinear and equally spaced. The points are draggable for manipulating
// the shape of the spline.
// The spline is a uniform BSpline with standard end conditions,
// where end points are interpolated.
// The only parameter that is needed is the
// number of polynomial pieces that are desired in the spline.

    // number of control points is related to the number of polynomials
    int numPts = _numPolys + 3;      // number of control points >= 4

    // vertically center initial control points 
    double initY = 0;
    NumberBvr initX = mul(neg(_halfWidth),toBvr(0.7));    // 15% from left side
    NumberBvr incX = div(mul(_halfWidth,toBvr(1.4)),toBvr((numPts - 1))); // use 70% of range

    // for the control points of the spline
    Point2Bvr[] ptsPt2 = new Point2Bvr[numPts];
    // 2 more knots than control points since a cubic
    NumberBvr[] knotsNum = new NumberBvr[numPts + 2];	
    Point2Bvr initPt2;      // initial position of a draggable
    DraggableButton draggable;    // holder of a draggable point
    ImageBvr ptsImg = emptyImage;  // the accumulated image

    // construct the control points and the knot vector
    for (int i = 0; i < numPts; i++) {
      initPt2 = point2(add(initX,mul(toBvr(i),incX)), toBvr(initY));
      draggable = new DraggableButton(initPt2);
      ptsPt2[i] = draggable.getPointBvr();
      ptsImg = overlay(ptsImg, draggable.getImageBvr());
      knotsNum[i] = toBvr(i-2);      // we want the first 3 knots to be 0, see next
    }

    // Set first and last two knots to achieve multiplicity 3,
    // this makes the spline pass through the first and last control points.
    knotsNum[0] =  knotsNum[1] = toBvr(0);    
    knotsNum[numPts] =  knotsNum[numPts + 1] = knotsNum[numPts - 1];


    // generate images of the spline and its control polygon
    Path2Bvr cubicCurvePath = cubicBSplinePath(ptsPt2, knotsNum);
    Path2Bvr controlPolyPath = polyline(ptsPt2);
    LineStyleBvr lLnS = defaultLineStyle.width(toBvr(0.5*mm));
    ImageBvr cubicCurveImg = cubicCurvePath.draw(lLnS.color(green));
    ImageBvr controlPolyImg = controlPolyPath.draw(lLnS.color(white));

// </Tutorial>

// <Tutorial Section=3.1>

    // Create an image behavior of a car by importing a bitmap.
    ImageBvr car1Img = importImage(buildURL(imgBase, "car3.gif"));
    // Car2 is the mirror image of car1 along Y; 
    // use this later to keep the sprite upright.
    ImageBvr car2Img = car1Img.transform(scale(1,-1));

    // Get an image moving back and forth along the spline,
    // the spline is well defined between the third knot from the begining
    // (= 0) and the third knot from the end since it is a cubic.
    NumberBvr limitNum = knotsNum[numPts - 1]; // third knot from end
    
    // Evaluator spans the parameter space of the spline and then
    // returns to the beginning. It then repeats.
    NumberBvr evaluatorNum = NumberBvr.newUninitBvr();
    evaluatorNum.init(until(mod(localTime, limitNum),
                            timer(limitNum),
                              until(sub(limitNum, mod(localTime, limitNum)),
                                timer(limitNum),
                                  evaluatorNum)));

    // Construct a point2 behavior that goes back and forth along the spline.
    Point2Bvr splPt2 =
          bSpline(DEGREE, knotsNum, ptsPt2, null, evaluatorNum);
 
    // Speed is the speed of traversal of the spline;
    // it switches among three values upon rightButtonDown.
    NumberBvr speedNum = NumberBvr.newUninitBvr();
    speedNum.init ( until(toBvr(0.6),
                         rightButtonDown,
                         until(toBvr(1.2),
                               rightButtonDown,
                               until(toBvr(1.8),
                                     rightButtonDown,
                                     speedNum))));
    
    // Construct a new point2 behavior that goes back and forth at
    // the rate of "speed" along the original spline. 
    // Integrate the speed and invoke substituteTime.
    // The runOnce construct is necessary so that the integration does not get
    // restarted everytime the rightButtonDown event occurs.
    splPt2 = (Point2Bvr)splPt2.substituteTime(integral((NumberBvr)speedNum.runOnce()));
    
    // Orient the car based on the spline.

    // The tangent to the spline is its derivative.
    // Notice that the derivative of a point behavior is a vector behavior.
    Vector2Bvr tangentVec2 = derivative(splPt2);
    // Extract the angle between the tangent and the X axis; 
    // it is in [-PI, +PI] range.
    NumberBvr angleNum = tangentVec2.getPolarCoordAngle();
    // The quadrant in which the tangent lies is in [0, 1, -2, -1] CCW
    NumberBvr quadrentNum = floor(div(angleNum, toBvr(Math.PI/2)));
    // carImg is always upright as the traversal of the spline takes place
    ImageBvr carImg = (ImageBvr)cond(or(eq(quadrentNum, toBvr(0)),
                                      eq(quadrentNum, toBvr(-1))),
                          car1Img,
                          car2Img); 

    // construct a car that travels along the spline always upright
    ImageBvr movingCarImg = carImg.transform(compose(translate(sub(splPt2, origin2)), 
                                                     rotate(angleNum)));
    
    // Finally, combine all these parts into the final model.
    setImage(overlay(movingCarImg,
               overlay(cubicCurveImg, 
                 overlay(ptsImg, 
                   overlay(controlPolyImg, solidColorImage(blue))))));
  }
  
  private static int DEGREE = 3;  // only cubic pathes are supported
  private int _numPolys;          // number of polynomials in the spline
  
// </Tutorial>

// <Tutorial Section=5.1>
  private NumberBvr _halfWidth;      // half width of viewport
  private NumberBvr _halfHeight;     // half height of viewport

// </Tutorial>
}

// <Tutorial Section=4.0 Title="Using Draggables for Control Points">

//
// Here, use the draggable class from the utility library.
// Construct a draggable based on a small (4 mm) cube with a reactive color
// the color is green while dragged and red otherwise. Provide accessors to get
// back the image and the position point of the draggable.
//

class DraggableButton extends Statics {

  public DraggableButton(Point2Bvr initPt2) {

    NumberBvr halfDimNum = toBvr(2*mm); 
    ColorBvr cClr = ColorBvr.newUninitBvr();
    ImageBvr blockImg = solidColorImage(cClr).
      crop(point2(neg(halfDimNum), neg(halfDimNum)), point2(halfDimNum, halfDimNum));
        
    _dragImg = new DraggableImage(blockImg, initPt2);     

    cClr.init(until(red, _dragImg.getGrabEvent(), 
               until(green, _dragImg.getReleaseEvent(), cClr))); 
  }
  
  public ImageBvr getImageBvr() { return _dragImg.getImageBvr(); }
  public Point2Bvr getPointBvr() { return _dragImg.getPointBvr(); }
  DraggableImage _dragImg;  
}

// </Tutorial>