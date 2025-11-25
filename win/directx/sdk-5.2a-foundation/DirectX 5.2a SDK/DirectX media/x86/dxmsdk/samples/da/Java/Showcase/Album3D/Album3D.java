
//
// <Tutorial Section=1.0 Title="Album3D Tutorial Introduction">
//
/**
  Album3D imports images with corresponding labels and presents the images
  as a photo album in 3-D. It also allows the interactive flipping of pages and 
  rotation of the view point, the former by using the left and right arrows, and the
  latter by using the up and down arrows.
  The flipping pages have corresponding stereo sound effects.

  The key feature of this sample is a state machine used to control
  and coordinate the animation of the album and the corresponding sound effects. 
  The state machine has five states
  and is used to control and coordinate four behaviors, including: indexes to the
  left and right pages of the album (NumberBvrs), the actual center page 
  (GeometryBvr), and the sound effect (SoundBvr). The state machine flows between
  the different states when specific events occur, and it switches the above four behaviors
  into new behaviors, accordingly. This is done through the two forms of the "until" method.

  The example suggests a systematic way to implement state machines in DirectAnmiation,
  based on mixed media behaviors. In addition, the approach lends itself to automatic generation
  of behavior-controlling state machines through visual authoring tools.

**/

// </Tutorial>

// <Tutorial Section=7.0 Title="Boiler Plates">

// imports
import com.ms.dxmedia.*;    // all DirectAnimation classes
import java.awt.*;          // Dimension class
import java.net.*;          // URL class

// An applet that instantiates the Album3D model
public class Album3D extends DXMApplet {
  public void init() {
    super.init() ;
    setModel (new A3DM(this)) ;
  }
}

class A3DM extends Model {

  A3DM(DXMApplet dxma) {
    Dimension dim = dxma.getSize();
    // The base unit of measure in DirectAnimation is the meter.
    // Convert the size into meters by multiplying it with the pixelBvr.
    _halfHeight = mul(toBvr(dim.height*0.5),pixelBvr);
    _halfWidth = mul(toBvr(dim.width*0.5),pixelBvr);
  }

// </Tutorial>

// <Tutorial Section=5.0 Title="Utility Functions">

  // Takes an index to a page and a direction of motion. 
  // Returns the given page animated in the proper direction,
  // both left and right page flips are supported.
  public static GeometryBvr flip(NumberBvr ind, int dir) {
    // orientation of page flip depends on dir
    Transform3Bvr flipTrans = rotate(yVector3, mul(toBvr(dir),  
                                mul(toBvr(flipRate), localTime)));
    // starting angle of page depends on dir
    GeometryBvr page = ((GeometryBvr)picFrames.nth(ind)).transform(
      (Transform3Bvr)cond(toBvr(dir == +1), leftTrans, rightTrans));
    return page.transform(flipTrans);
  }
  
  // Flipping a page commences with startSnd and transitions to flipSound,
  // stereo sound is used to reflect the direction of the page flip.
  public static SoundBvr flippingSnd(int dir) {
    double panRate = 2/period;  // in period time it goes from 0 to 2
    return (SoundBvr)until(startSnd.pan(-1 * dir), // in -dir speaker
      //timer(startLen[0]),   
      timer(toBvr(1.0)),      // substitute for startLen
        flipSnd.loop().pan(mul(toBvr(dir),   // varries from -dir to +dir.
          sub(mul(toBvr(panRate), localTime), toBvr(1)))));
  } 

  
  // Pastes a picture on a unit-sized page while preserving its
  // aspect ratio. The picture is left-centered. The extra  
  // space is filled with paper texture.
  public static ImageBvr normalPic(ImageBvr pic) {
    Bbox2Bvr picBbox = pic.boundingBox();
    Point2Bvr ur = picBbox.getMax();
    NumberBvr dim = maxNumBvr(ur.getX(), ur.getY());
    ImageBvr croppedTex = paperTex.tile().crop(point2(neg(dim), neg(dim)),
                                               point2(dim, dim));
    return overlay(pic, croppedTex).mapToUnitSquare();
  }

  // Constructs a number behavior which is a certain number 
  // or 0 depending on if the given key is pressed or not.
  static NumberBvr keyIntegral(int key) {  
    NumberBvr cAngleRate = toBvr(Math.PI/8);    // rate of motion of camera angle
    NumberBvr factor = (NumberBvr)cond(keyState(key), cAngleRate, toBvr(0));
    return integral(factor);
  }

  // Takes a page of the album and renders it into an image
  // based on consistent camera and lights. Animates the camera
  // based on the up and down arrow keys.
  static ImageBvr renderPage(GeometryBvr page) {
    // create an interactive angle to animate the camera
    NumberBvr initialAngle = toBvr(Math.PI/20);
    NumberBvr cameraAngle = add(initialAngle, 
      sub(keyIntegral(Event.UP), 
        keyIntegral(Event.DOWN)));
    // use the angle to rotate the camera
    Transform3Bvr cameraTrans = rotate(xVector3, cameraAngle);
    CameraBvr camera = perspectiveCamera(toBvr(5), 
      toBvr(4)).transform(cameraTrans);
    // scale the resulting image, map 1 unit to 50 pixels     
    Transform2Bvr scaleTrans = scale2(mul(toBvr(PAGESIZE),pixelBvr)); 
    GeometryBvr scene = union(ambientLight, page);
    return scene.render(camera).transform(scaleTrans);
  }  

// </Tutorial>

  // render a string into an image, at a specific location and color
  public static ImageBvr renderStr(StringBvr str,
                                   ColorBvr clr,
                                   NumberBvr xPos, NumberBvr yPos) {
    Transform2Bvr pos = translate(xPos, yPos);
    FontStyleBvr fs = defaultFont.bold().color(clr);
      
    ImageBvr textIm = stringImage(str, fs).transform(pos);

		return textIm;
  }

  // returns max(x, y)
  public static NumberBvr maxNumBvr(NumberBvr x, NumberBvr y) {   
    return (NumberBvr) cond(gt(y, x), y, x);
  }

// <Tutorial Section=3.0 Title="The State Machine">

/**
  The state machine has five states controlled by two events, one for left page flips  
  and one for right flips. The machine controls and coordinates the pages of
  the album and the sound effects of the album. The sample uses notification
  objects because of the cyclic dependency in the finite state machine
  (not because of event data, which is the second purpose for using notification
  objects). Notification objects, by providing a delayed execution construct, 
  make it possible to construct cyclic calling dependencies without infinite looping.
  The sample also uses the until2 method instead of untilNotify since it allows the
  aggregatation of two notification events each with a different notification method; 
  see the opened state below. 
  
  Note that both cyclic dependency and branching from one
  state into two or more states upon different events are commonly
  occuring patterns in general state machines. Hence the example below
  has the building blocks that are needed for typical state machines.
**/

  // This state is the entry point into the state machine
  // and is also returned to upon flipping the 
  // front page back (effectively going back to the start state). 
  // It sets the left page to blank, the right page to
  // the front page of the album, and it emits the page-flip completion sound
  // through the right speaker.
  // The state waits on the leftEventNotify event upon which it 
  // invokes the transLeft notification object which implements the
  // "transition left" state.          
  public static TupleBvr frontState () {  
    NumberBvr leftInd = toBvr(0);
    Behavior newBvrs[] = { leftInd, add(leftInd, toBvr(1)), 
                          emptyGeometry,  endSnd.pan(+1)};
    TupleBvr tuple =  new TupleBvr(newBvrs);      
    return (TupleBvr)untilEx(tuple, leftEventNotify);
  }
        
  // This state is entered upon the end of a page flip
  // that causes the album to be properly opened
  // (i.e., not in a boundary condition). It waits on either a left or a right
  // event, at which point it invokes the transLeft or transRight
  // notification objects, respectively, which carry it through either 
  // the left transition or to right
  // transition state. It accepts an index to the
  // left page plus the side to which the previous page was flipped to,
  // in order to emit the page flip termination sound.
  public static TupleBvr opened (NumberBvr leftInd, int side) { 
    Behavior newBvrs[] = {leftInd , add(leftInd, toBvr(1)), 
                          emptyGeometry, endSnd.pan(toBvr(side))};         
    TupleBvr tuple =  new TupleBvr(newBvrs);
    return (TupleBvr)untilEx(tuple, orEvent(rightEventNotify, 
                           leftEventNotify));
  }
 
  // symmetric to frontState
  public static  TupleBvr backState () {    
    NumberBvr leftInd = toBvr(A3DM.numPics);
    Behavior newBvrs[] = {leftInd, add(leftInd, toBvr(1)), 
                          emptyGeometry, endSnd.pan(-1)};   
    TupleBvr tuple =  new TupleBvr(newBvrs);      
    return (TupleBvr)untilEx(tuple, rightEventNotify);
  }         

// </Tutorial>  

// <Tutorial Section=7.1>

  // Here we create the Album3D model.
  public void createModel(BvrsToRun blst) {

// </Tutorial> 

// <Tutorial Section=4.0 Title="Importing and Defining the Media Parts">
    
    // Build URL bases for importing images, geometries, and sounds.
    URL mediaBase = getImportBase();
    URL imgBase = buildURL(mediaBase,"image/");
    URL geoBase = buildURL(mediaBase,"geometry/");
    URL sndBase = buildURL(mediaBase,"sound/");

    // Load the sound files that will be used for sound effects
    startSnd = importSound(buildURL(sndBase,"inflate.mp2"), startLen);
    flipSnd = importSound(buildURL(sndBase,"clock1.mp2"), null);
    endSnd = importSound(buildURL(sndBase,"deflate.mp2"), null);

    // This image will be tiled into the paper texture.
    paperTex = importImage(buildURL(imgBase,"sandbg1.jpg"));

    // Synthetically construct the front and back cover pages of the Album.      
    ColorBvr labelColor = red;
    ImageBvr front1 = renderStr(toBvr("Outdoors"), labelColor, toBvr(0), toBvr(0.003));     
    ImageBvr front2 = renderStr(toBvr("Album"), labelColor, toBvr(0), toBvr(-0.003)); 
    ImageBvr front = overlay(front1, front2);   
    ImageBvr back = renderStr(toBvr("The End"), labelColor, toBvr(0), toBvr(0)); 

    // Import a list of images, some of these could have been .gif images.
    // We add the front and back covers appropriately, 
    // and we add empty images to pair with them.
    ImageBvr pics[] = {emptyImage, front,
                      importImage(buildURL(imgBase,"phantom.jpg")),
                      importImage(buildURL(imgBase,"hiddenBeachSS.jpg")),
                      importImage(buildURL(imgBase,"hiddenBeachSG.jpg")),
                      importImage(buildURL(imgBase,"jotunheimen1.jpg")),
                      importImage(buildURL(imgBase,"redWoodCar.jpg")),
                      importImage(buildURL(imgBase,"wenatcheeCascades.jpg")),
                      importImage(buildURL(imgBase,"tulipsHol1.jpg")),                                                       
                      importImage(buildURL(imgBase,"tulipsHol2.jpg")),
                      importImage(buildURL(imgBase,"yosemiteCreek.jpg")), 
                      importImage(buildURL(imgBase,"kidsUtah.jpg")),
                      importImage(buildURL(imgBase,"foliageUtah.jpg")),
                      back, emptyImage};

    // Ditto for corresponding labels. No labels for cover pages nor for empty ones.
    StringBvr lbls[] = {toBvr(""), toBvr(""), 
                        toBvr("Crater Lk., Oregon"),
                        toBvr("Klamath, California"),
                        toBvr("Klamath, California"),
                        toBvr("Jotenheimen, Norway"),
                        toBvr("Redwoods N.P., California"),
                        toBvr("Cascades, Washington"),
                        toBvr("Tulip Garden, Holland"),
                        toBvr("Tulip Garden, Holland"),
                        toBvr("Yosemite, California"),
                        toBvr("Centerville, Utah"),
                        toBvr("Bountiful, Utah"),
                        toBvr(""), toBvr("")};
    ArrayBvr labels = new ArrayBvr(lbls);

    numPics = lbls.length - 2;   // don't count the emptyImages

    // a [-1, 1] square in X and Y with [0, 1] texture coordinates for
    // both faces.
    GeometryBvr square = importGeometry(buildURL(geoBase,"square.x"));

    // frame is a [0, 2]x[-1, 1] square, on right hand side of Y axis
    GeometryBvr frame = square.transform(translate(1,0,0));
  
    // creates an array for the pages that are counter part to the pictures.
    GeometryBvr frames[] = new GeometryBvr[numPics + 2];

    // first and last correspond to the empty pictures
    frames[0] = emptyGeometry;                      
    frames[numPics + 1] = emptyGeometry;
    // the rest correspond to the pictures
    for (int i=1; i<=numPics; i++) 
      frames[i] = frame.texture(normalPic(pics[i]));
    // Construct a Behavior Array of the pages, so we can use NumberBvr indices
    picFrames = new ArrayBvr(frames); 

    // demarcates the line around which pages pivot
    NumberBvr lineThickness = toBvr(6 * mm);      // 3% of page size
    // Make the square very thin in the X direction.
    // We use a square to represent the line in 3D space. 
    GeometryBvr pivotLine = square.diffuseColor(red).
                              transform(scale(lineThickness, toBvr(1), toBvr(1)));
// </Tutorial>

// <Tutorial Section=2.0 Title="The State and Structure of the Album">
        
    // Kick-off the state machine.
    // The "state" is a tuple with two number behaviors, one each for the left and
    // right page indexes, a geometry behavior for the center page,
    // and a sound behavior for the audio effects.
    TupleBvr state = frontState();

    // extract the individual behaviors from the tuple
    leftInd =     (NumberBvr) state.nth(0);
    rightInd =    (NumberBvr) state.nth(1);
    centerPage =  (GeometryBvr) state.nth(2);
    snd =         (SoundBvr) state.nth(3);
 
    // extract the left and right pages and labels from their indices
    GeometryBvr leftPage = ((GeometryBvr)picFrames.nth(leftInd)).transform(leftTrans);
    GeometryBvr rightPage = ((GeometryBvr)picFrames.nth(rightInd)).transform(rightTrans);
    StringBvr lPageLabel = (StringBvr)labels.nth(leftInd);
    StringBvr rPageLabel = (StringBvr)labels.nth(rightInd);

    // now construct the album from the pages and the pivot line
    ImageBvr album = overlay(renderPage(pivotLine),
                       overlay(renderPage(centerPage),
                         overlay(renderPage(rightPage),
                                 renderPage(leftPage)))); 

    // Here, construct the two lables and position them under the album. 
    // Position them relative to the viewport to be at 12.5% from the bottom.
    ImageBvr lLabel = renderStr(lPageLabel, labelColor,
                                neg(div(_halfWidth,toBvr(2))),
																mul(toBvr(-0.75),_halfHeight));      
    ImageBvr rLabel = renderStr(rPageLabel, labelColor,
                                div(_halfWidth,toBvr(2)),
																mul(toBvr(-0.75),_halfHeight));
    // The model is the album with the lables and a blue background.
    ImageBvr model = overlay(album, 
                       overlay(lLabel,
                         overlay(rLabel, solidColorImage(blue))));
    setSound(snd);
    setImage(model);

// </Tutorial>

// <Tutorial Section=7.2>
  }

  // stores the height of the applet viewport in the member variable. 
  private NumberBvr _halfHeight;
  private NumberBvr _halfWidth;

// </Tutorial>

// <Tutorial Section=6.0 Title="General Declarations">

  static final int PAGESIZE = 60;     // Album page size in pixels

  static final double period = 3;               // duration of a page flip in seconds   
  static final double tiltAngle = Math.PI / 4;  // angle at which resting pages sit
  static final double flipRate = (Math.PI - 2*tiltAngle)/period; // the rate of a page flip
  
  // album pages are slightly off horizontal
  // resting position of left page
  static final Transform3Bvr leftTrans = rotate(yVector3, toBvr(-Math.PI + tiltAngle));
  // resting position of right page
  static final Transform3Bvr rightTrans = rotate(yVector3, toBvr(-tiltAngle));  
  
  static int numPics;                   // the number of pages in the album     
   
  // the three sound files that will be imported 
  // the length of the start sound       
  static NumberBvr startLen[] = new NumberBvr[1];         
  // at the begining of a page flip       
  static SoundBvr startSnd;
  // during a page flip
  static SoundBvr flipSnd;
  // at the end of a page flip
  static SoundBvr endSnd;
  
  // the texture used for the album paper
  static ImageBvr paperTex;
  static ArrayBvr picFrames;            // the pages of the album

  // The state of the album consists of four behaviors
  static NumberBvr leftInd;             // index to left page
  static NumberBvr rightInd;            // index to right page
  static GeometryBvr centerPage;        // the actual center page
  static SoundBvr snd;                  // sound associated with page flipping

// </Tutorial>

// <Tutorial Section=3.2>

  // interaction events
  // Right and left arrows to flip pages.
  static final DXMEvent rightEvent = keyDown(Event.RIGHT);
  static final DXMEvent leftEvent = keyDown(Event.LEFT);
  // signals the conclusion of a page flip animation 
  static final DXMEvent endFlip = timer(toBvr(period));
  // instantiate the notifier objects
  static final TransRight transRight = new TransRight();
  static final TransLeft transLeft = new TransLeft();
  // Define notify events based on the previous objects,
  // these events get used in the state machine.
  static final DXMEvent rightEventNotify = rightEvent.notifyEvent(transRight);
  static final DXMEvent leftEventNotify = leftEvent.notifyEvent(transLeft);

// </Tutorial>
}

// <Tutorial Section=3.1>

// This notifier object flips the center page to the left side until the
// completion of the page flip. At that point if we've flipped 
// the last page then we go to the back state, else we go to
// the opened state, which is the nominal one.
// Notice how we extract the present left page index from the "previous"
// parameter of the "notify" method.
class TransLeft extends Statics implements UntilNotifier {
  public Behavior notify(Object eventData, Behavior previous, BvrsToRun blst) {
    NumberBvr leftInd = (NumberBvr)((TupleBvr)previous).nth(0);
    Behavior newBvrs[] = {leftInd, add(leftInd, toBvr(2)), 
                          A3DM.flip(add(leftInd, toBvr(1)), -1), A3DM.flippingSnd(-1)};
    return (TupleBvr) until(new TupleBvr(newBvrs), A3DM.endFlip,
                        cond(eq(leftInd, toBvr(A3DM.numPics - 1)),
                               A3DM.backState(),
                               A3DM.opened(add(leftInd, toBvr(1)), -1)));      
  }
}

// symmetric to TransLeft 
class TransRight extends Statics implements UntilNotifier {  
  public  Behavior notify(Object eventData, Behavior previous, BvrsToRun blst) {
    NumberBvr leftInd = sub((NumberBvr)((TupleBvr)previous).nth(0), toBvr(1));
    Behavior newBvrs[] = {leftInd, add(leftInd, toBvr(2)),
                          A3DM.flip(add(leftInd, toBvr(1)), +1), A3DM.flippingSnd(+1)};
    return (TupleBvr) until(new TupleBvr(newBvrs), A3DM.endFlip,
                        cond(eq(leftInd, toBvr(0)),
                               A3DM.frontState(),
                               A3DM.opened(leftInd, +1)));
  }
}
// </Tutorial>
