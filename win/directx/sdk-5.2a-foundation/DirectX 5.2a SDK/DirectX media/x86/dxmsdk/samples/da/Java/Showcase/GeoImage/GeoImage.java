
// 
// <Tutorial Section=1 Title="GeoImage Tutorial Introduction">
//
/** 
  GeoImage imports a list of pictures and corresponding labels.
  It textures a picture on a rectangle while preserving aspect ratio
  (the picture is called the GeoImage). The texturing takes place on both
  sides of the rectangle so that the picture is seen from either side.
  The sample animates the geoimage in 3-D, punctuates when the geoimage is in
  the image plane with an audible tick, uses arrow keys and the space
  bar to cycle through the pictures and corresponding labels, and renders
  the original picture as a bitmap, the animate geoimage, and the label.

  The key DirectAnimation points that are demonstrated include: <br>
  - aggregate events using orEvent <br>
  - creating a counter with application event data and the untilNotify method<br>
  - using number behaviors as indexes to arrays of behaviors <br>
  - newUninitbvr and init for looping transform and sound <br>
  - texturing images on rectangles while preserving aspect ratio <br>
  - 2-D/3-D interplay, via the relation of geoimage with picture <br>
  - using sound to punctuate instances in the animation <br>
  - rendering text into images <br>

 
**/

// </Tutorial>          

// <Tutorial Section=7.0 Title="Boiler Plates">
// imports
import java.awt.*;        // use Event and Dimension 
import com.ms.dxmedia.*;  // DirectAnimation classes
import java.net.*;        // building URLs

public class GeoImage extends DXMApplet {
  public void init() {
    super.init() ;
    setModel(new GeoImageModel(this));
  }
}

class GeoImageModel extends Model { 

  // stores the height of the applet viewport in the member variable. 
  private NumberBvr _halfHeightNum;

  GeoImageModel(DXMApplet dxma) {
    Dimension dim = dxma.getSize();
    // The base unit of measure for DirectAnimation is the meter.
    // Convert the size into meters by multiplying it with the pixelBvr.
    _halfHeightNum = mul(toBvr(dim.height*0.5),pixelBvr);
  }
  
// </Tutorial>

  // return max(x, y)
  public static NumberBvr maxNumBvr(NumberBvr x, NumberBvr y) {
    return (NumberBvr) cond(gt(y, x), y, x);
  }        

// <Tutorial Section=7.1>
  
  public void createModel(BvrsToRun blst) {

// </Tutorial>  

// <Tutorial Section=6.0 Title="Load the Images and Specify the Labels">

    // set the URL bases for importing the media
    URL mediaBase = getImportBase();
    URL imgBase = buildURL(mediaBase,"image/");
    URL sndBase = buildURL(mediaBase,"sound/");
    URL geoBase = buildURL(mediaBase,"geometry/");


    // a [-1, 1] square in X and Y with [0, 1] texture coordinates for
    // both of its faces.
    GeometryBvr square = 
        importGeometry(buildURL(geoBase, "square.x"));

    // import a list of images, some of these images could have been .gif images
    ImageBvr pics[] = { 
      importImage(buildURL(imgBase, "phantom.jpg")),
      importImage(buildURL(imgBase, "hiddenBeachSS.jpg")),
      importImage(buildURL(imgBase, "hiddenBeachSG.jpg")),
      importImage(buildURL(imgBase, "jotunheimen1.jpg")),
      importImage(buildURL(imgBase, "redWoodCar.jpg")),
      importImage(buildURL(imgBase, "wenatcheeCascades.jpg")),
      importImage(buildURL(imgBase, "tulipsHol1.jpg")),
      importImage(buildURL(imgBase, "tulipsHol2.jpg")),
      importImage(buildURL(imgBase, "yosemiteCreek.jpg")), 
      importImage(buildURL(imgBase, "kidsUtah.jpg")),
      importImage(buildURL(imgBase, "foliageUtah.jpg"))};
    // construct a corresponding array of behaviors
    ArrayBvr pictures = new ArrayBvr(pics);

    // ditto for corresponding labels
    StringBvr lbls[] = {
      toBvr("Phantom Is., Crater Lk., Oregon"),
      toBvr("Hidden Beach, Klamath, California"),
      toBvr("Hidden Beach, Klamath, California"),
      toBvr("Jotenheimen, Norway"),
      toBvr("Redwoods N.P., California"),
      toBvr("Wenatchee, Cascades, Washington"),
      toBvr("Tulip Garden, Holland"),
      toBvr("Tulip Garden, Holland"),
      toBvr("Yosemite, California"),
      toBvr("Centerville, Utah"),
      toBvr("Bountiful, Utah")};
    ArrayBvr lables = new ArrayBvr(lbls);

    // Construct a NumberBvr counter object, and use it as an interactive 
    // index to select a picture and a corresponding lable.
    NumberBvr numPics = lables.length();
    index = (NumberBvr) (new Counter(numPics)).getBvr().runOnce();
    ImageBvr picture = (ImageBvr) pictures.nth(index);
    StringBvr lable = (StringBvr) lables.nth(index);
// </Tutorial>

// <Tutorial Section=5.0 Title="Texture the Picture while Preserving Aspect Ratio">

    // Extract the extent of the picture and scale the square
    // into a rectangle with the same extent, to preserve the
    // picture's aspect ratios upon texturing
    Bbox2Bvr pictureBbox = picture.boundingBox();
    Point2Bvr ll = pictureBbox.getMin();
    Point2Bvr ur = pictureBbox.getMax();
    NumberBvr xUpperRight = ur.getX();
    NumberBvr yUpperRight = ur.getY();
    Transform3Bvr scaleSquare = scale(xUpperRight, yUpperRight, toBvr(1));
    NumberBvr maxHalfExtent = maxNumBvr(xUpperRight, yUpperRight);

    // GeoImage has the property that when it lies in the image plane
    // it will have dimensions equal to its bitmap counterpart.
    GeometryBvr geoImage = square.transform(scaleSquare).
    // The picture is normalized before texturing since the
    // square's texture coordinates assume a unit square texture.
      texture(picture.mapToUnitSquare());

// </Tutorial>

// <Tutorial Section=3.0 Title="The 3-D animation">
    
    // the length of the first segment of the animation in seconds
    NumberBvr period = toBvr(6);
    // time-varying angle, does a complete cycle per period
    NumberBvr aniAngle = mul(localTime, mul(toBvr(2), 
      div(toBvr(Math.PI), period)));

    // A translation between front and back Z limits used for 
    // travel of the center of the geometric image.
    // The limits are related to the extent of the picture.
    NumberBvr distOfTravel = mul(toBvr(20), maxHalfExtent); 
    Transform3Bvr trans = translate(toBvr(0), toBvr(0), 
      mul(neg(distOfTravel), sin(aniAngle)));
    // a rotation with rate "aniAngle" around a time varying axis
    Transform3Bvr rot = rotate(vector3(sin(aniAngle), cos(aniAngle), 
      div(sin(aniAngle), toBvr(2))), aniAngle);
        
    // tumbling in 3-D animation, performs a complete cycle in one "period":
    Transform3Bvr rot1 = compose(trans, rot);

    // open page animation, opens in one quarter of "period":
    Transform3Bvr rot2 = 
        compose(translate(neg(xUpperRight), toBvr(0), toBvr(0)), 
          compose(rotate(yVector3, neg(aniAngle)),
            translate(xUpperRight, toBvr(0), toBvr(0))));

    // close page animation, closes in one quarter of "period":
    Transform3Bvr rot3 = 
        compose(translate(neg(xUpperRight), toBvr(0), toBvr(0)), 
          // the -PI/2 is to pick up from where the page open animation left of
          compose(rotate(yVector3, add(div(neg(toBvr(Math.PI)), toBvr(2)), aniAngle)),
            translate(xUpperRight, toBvr(0), toBvr(0))));

    // the cyclic animation of the geoimage:
    //              - tumbling in 3-D in one "period"
    //              - open page in quarter of "period"
    //              - close page in quarter of "period"
    NumberBvr quarterPeriod = div(period, toBvr(4));
    Transform3Bvr loopingTrans = Transform3Bvr.newUninitBvr();
    loopingTrans.init(until(rot1, timer(period), 
      until(rot2, timer(quarterPeriod),
        until(rot3, timer(quarterPeriod), loopingTrans))));       

    GeometryBvr loopingGeoImage = geoImage.transform(loopingTrans);
// </Tutorial>

// <Tutorial Section=4.0 Title="Sound Punctuation & Rendering the Label">

    // Generate a tick everytime the geoimage lies in the image plane.
    // At that point the original picture (bitmap in image plane)
    // coincides with geoimage (a 3-D rendering of a textured rectangle).
    // GeoImage's projection is larger than the picture when closer than the
    // image plane and its projection is smaller when it's further away.

    // import the ping sound.
    SoundBvr ping = importSound(buildURL(sndBase, "butin.wav"), null);

    // Could have set the looping sound like the transform above;
    // Instead, demonstrate the cycler.
    // alternative:
    //  NumberBvr halfPeriod = div(period, toBvr(2));
    //      SoundBvr loopingSound = SoundBvr.newUninitBvr();
    //  loopingSound.init(until(ping, timer(halfPeriod), loopingSound));
    NumberBvr halfPeriod = div(period, toBvr(2));
    Behavior sounds[] = { ping };
    Cycler cyl = new Cycler(sounds, timer(halfPeriod));
    SoundBvr loopingSound = (SoundBvr)(cyl.getBvr());

    // label is yellow, bold, centered horizontally, and 15% above bottom
    Transform2Bvr lablePos = translate(toBvr(0), mul(toBvr(-0.85), _halfHeightNum));
    
		FontStyleBvr fs = defaultFont.color(yellow).bold();
      
    ImageBvr lableImage = stringImage(lable, fs).transform(lablePos);
		
// </Tutorial>

// <Tutorial Section=7.2>

        // Don't want part of geoimage to be clipped when its center
        // is at distOfTravel, so set the near plane slightly further than that
        // and set the projection point a bit further than the near plane.
    NumberBvr nearDist = add(distOfTravel, mul(toBvr(2.5), maxHalfExtent));
    CameraBvr camera = perspectiveCamera(add(nearDist, 
      mul(maxHalfExtent, toBvr(10))), nearDist);
    // turn on the lights and render geoimage into an image
    GeometryBvr lights = ambientLight; 
    GeometryBvr scene = union(lights, loopingGeoImage);
    ImageBvr renderedGeo = scene.render(camera);

    // finally set what's to be displayed on a blue background
    ImageBvr model = overlay(renderedGeo, 
      overlay(lableImage,
        overlay(picture, solidColorImage(blue))));

    // the sound behavior to be presented
    setSound(loopingSound);

    // the image behavior to be rendered
    setImage(model);

  }
  public void cleanup() {
    super.cleanup();
	index = null;
  }

  NumberBvr index;
// </Tutorial>
}

// <Tutorial Section=2.0 Title="Interacting with geoImage">
 
// This class produces a bidirectional counter that cycles in a given range
// upon button presses. 
class Counter extends Statics implements UntilNotifier {

  // constructor for a counter object
  public Counter(NumberBvr limit) {
    _counterLimit = limit;
    // upon a skip event invoke the notifier
    _value = (NumberBvr)untilNotify(_initValue, skip, this);
  }       

  public Behavior notify(Object eventData, Behavior previous, BvrsToRun lst) {
    // extract the application data (an integer), into a number behavior
    NumberBvr adjustment = toBvr(((Integer)eventData).intValue());
    // use it as an inc/decrement
    NumberBvr value = add((NumberBvr)previous, adjustment);
    // cycle between 0 and (limit - 1)
    value = (NumberBvr)cond(eq(value, toBvr(-1)), 
      sub(_counterLimit, toBvr(1)),         // if -1, wrap around
        cond(eq(value, _counterLimit), toBvr(0), value)); // if limit, wrap around
    // repeat with the new value
    return untilNotify(value, skip, this);
  }

  public NumberBvr getBvr () {
    return _value;
  }

  // Key events with corresponding event data.
  // For non-ASCII keys use the Java designation for the key.
  private static final DXMEvent skipForward1 = keyDown(Event.RIGHT).attachData(new Integer(1));
  private static final DXMEvent skipBack1 = keyDown(Event.LEFT).attachData(new Integer(-1));

  // For ASCII keys use the ASCII letter between single quotes.
  private static final DXMEvent skipForward2 = keyDown(' ').attachData(new Integer(1));

  // There is also a timeout event for automatically switching between pictures
  // if no manual key presses are issued.
  private static final DXMEvent auto = timer(toBvr(8)).attachData(new Integer(1));

  // Aggregate the four events above into a single event.
  private static final DXMEvent skip = orEvent(skipForward1, 
                         orEvent(skipBack1,
                           orEvent(skipForward2, auto)));
  // These are aspects of the counter.
  private NumberBvr _counterLimit;
  private NumberBvr _initValue = toBvr(0); // start counting at 0
  private NumberBvr _value;                // current value of counter

}
// </Tutorial>

