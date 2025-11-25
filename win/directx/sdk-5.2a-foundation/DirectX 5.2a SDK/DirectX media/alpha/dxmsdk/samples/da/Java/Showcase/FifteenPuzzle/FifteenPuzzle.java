//  
// DirectAnimation SDK sample  
//
// <Tutorial Section=1.0 Title="FifteenPuzzle Tutorial Introduction">
/**
 The classic fifteen-puzzle in DirectAnimation.  Note that the <br>
 underlying image can be *any* image (time-varying, interactive, video-  <br>
 based, from a geometry, etc), also the puzzle supports a mode where  <br>
 it is textured on an animate 3D object while it preserves its interactivity. 

 The main features highlighted in this sample include: <br>
 - using a time-varying, interactive image as a texture <br>
 - getting events to DirectAnimation from VBScript <br>
 - rendering geometry into an image <br>
 - overlaying text on video <br>
 - using reactive sound as feedback to user input <br>


**/
// </Tutorial>

// <Tutorial Section=12.0 Title="Boiler Plates">
// imports   
import java.applet.*;
import java.awt.*;
import java.net.*;
import com.ms.dxmedia.*;

public class FifteenPuzzle extends DXMApplet {
  public void init() {
    super.init() ;
    _model = new FifteenPuzzleModel(getCanvas());
    setModel(_model);
  }

  // NOTE: You should use getModel instead of storing the model.
  // If you store the model, you need to set it to null at destroy
  // to break COM cycles due to current VM implementation of COM.
  public void destroy() {
    super.destroy();
    _model = null;
  }
  
// </Tutorial>

// <Tutorial Section=2.1>

  // These are DXMApplet public methods that are visible to VBScript and are 
  // used as conduits for VBScript button events to 
  // triggering AppTriggerEvents in DirectAnimation.
  public void toggleGeomProjection() { _model.toggleGeomProjection(); }
  public void toggleImageUsed() { _model.toggleImageUsed(); }
  public void resetPuzzle() { _model.resetPuzzle(); } 

// </Tutorial>

// <Tutorial Section=12.1>

  private FifteenPuzzleModel _model;
}

class FifteenPuzzleModel extends Model {

  FifteenPuzzleModel(Viewer viewedOn) {
    // Hang onto this for synchronization.
    _viewedOn = viewedOn;
  }
  
// </Tutorial>

// <Tutorial Section=3.0 Title="Construct the Cycling Image for the Puzzle">

  // Create a unit-sized image to divide up into the puzzle.
  // The image cycles through four images upon toggleImage events.
  ImageBvr createUnitImageForPuzzle() {

    // Create the different images that will be used in the puzzle:

    // time-varying solid-colored image for background
    ImageBvr bkgnd = createBackgroundColorCycler();
    // image of time-varying elapsed time in seconds
    ImageBvr counterImg = createCounterImage();
    // first possible image for puzzle, time over colored background
    ImageBvr unitIm1 = overlay(counterImg, bkgnd);
    // second possibility, based on rendered geometry
    ImageBvr unitIm2 = createGeoImage();
    // third possibility, based in imported video
    ImageBvr unitIm3 = createMovieImage();
    // forth possibility, time in second over the video
    ImageBvr unitIm4 = overlay(counterImg, unitIm3);

    // Have these image behaviors run just once, so 
    // that they don't get restarted when events happen.
    unitIm1 = (ImageBvr)unitIm1.runOnce();
    unitIm2 = (ImageBvr)unitIm2.runOnce();
    unitIm3 = (ImageBvr)unitIm3.runOnce();
    unitIm4 = (ImageBvr)unitIm4.runOnce();
      
    // Cycle between the four images on occurence of this app event.
    ImageBvr imageToShow = ImageBvr.newUninitBvr();
    _toggleImageUsed = new AppTriggeredEvent();
    imageToShow.init( until(unitIm1,
                        _toggleImageUsed, until(unitIm2,
                          _toggleImageUsed, until(unitIm3,
                            _toggleImageUsed, until(unitIm4,
                              _toggleImageUsed, imageToShow)))));
    return imageToShow;
  }

// </Tutorial>

// <Tutorial Section=4.0 Title="Construct a 3-D rendered Image">

  // Constructs one of the puzzle images by rendering
  // three intertwined and animate instances of a doughnut.  
  public ImageBvr createGeoImage() {

    // construct a color with a time- varying hue
    NumberBvr hue1 = mul(localTime, toBvr(0.2));
    ColorBvr col = colorHsl(hue1, toBvr(0.5), toBvr(0.5));

    // import a half doughnut in the positive Y half space
    GeometryBvr halfDonut =
      importGeometry(buildURL(_geoBase,"hlfdonut.x"));
    halfDonut = halfDonut.transform(rotate(zVector3, toBvr(Math.PI)));
    // Construct a complete doughnut by combining with the second half.
    // The resulting doughnut is centered around the origin and is of
    // [-1,1] extent in X and Y.
    GeometryBvr donut = union(halfDonut,
              halfDonut.transform(rotate(xVector3, toBvr(Math.PI))));
    // The first doughnut rotates around the Y axis and 
    // has the time-varying color. 
    GeometryBvr donut0 =
      donut.transform(compose(rotate(yVector3, localTime), 
                              scale3(toBvr(0.3)))).diffuseColor(col);
    // The second doughnut is based on the first one scaled down by 25%, 
    // shifted by 0.2 in positive Z,
    // and animated at a 30% faster rate than the first one.
    GeometryBvr donut1 = (GeometryBvr)
      donut0.transform(compose(translate(0,0,0.2),
                               scale3(0.75)))
            // when the first doughnut progresses by one unit,
            // this second doughnut progresses by 1.3 units.
            .substituteTime(mul(localTime, toBvr(1.3)));      
    // The third doughnut is based on the first one scaled down by 25%, 
    // shifted by 0.2 in negative Z,
    // and animated at a 40% slower rate than the first one.
    GeometryBvr donut2 = (GeometryBvr)
      donut0.transform(compose(translate(0,0,-0.2),
        scale3(0.75))).substituteTime(mul(localTime, toBvr(0.6)));

    // Construct a 3-D rotational transform with a time-varying 
    // axis of rotation and a time-varying angle of rotation.
    Transform3Bvr xf = rotate(vector3(sin(localTime),
      cos(localTime), sin(mul(toBvr(1.34), localTime))),
        mul(localTime, toBvr(0.62)));

    // The geometry for the image is the aggregation of the
    // three doughnuts from above with the animate rotational transform applied.
    GeometryBvr donuts = union(donut0, union(donut1, donut2)).transform(xf);
          
    // a camera with projection point at 3 and near plane at 2
    CameraBvr cam = perspectiveCamera(toBvr(3),toBvr(2));

    GeometryBvr geoWithLight = union(donuts, directionalLight);
      
    ImageBvr im = geoWithLight.render(cam);
    // This translates the image so that its lower left corner 
    // is approximately at the origin. This approximation allows 
    // the image (doughnuts really) to get clipped slightly.
    im = im.transform(translate(0.5,0.5));

    return overlay(im, solidColorImage(white));      
  }
  
// </Tutorial>
  
// <Tutorial Section=5.0 Title="Construct an Image of an Elapsed Time Counter">

  // Returns an elapsed-seconds counter as an image.
  ImageBvr createCounterImage() {
    // Convert localTime to a string and ignore fractional part.
    FontStyleBvr fs = defaultFont.color(white).bold();
      
    // render into an image  
    ImageBvr underlyingImage = stringImage(localTime.toString(toBvr(0)),fs);
    // Scale into a unit square [0,1]. Note that as the size of the
    // the string grows, for example going from 1 digit to 2, the scale
    // adapts to fit the resulting image in the unit square.
    return underlyingImage.mapToUnitSquare();
  }

  // This is a time-varying solid-colored image, infinite in extent.
  ImageBvr createBackgroundColorCycler() {
      NumberBvr hue1 = mul(localTime, toBvr(0.2));
      ColorBvr col1 = colorHsl(hue1, toBvr(0.5), toBvr(0.5));      
      return solidColorImage(col1);
  }
  
// </Tutorial>
  
// <Tutorial Section=6.0 Title="Construct an Image based on an Imported Video">

  // This returns an imported video as a continually looped image behavior. 
  ImageBvr createMovieImage() {

    SoundBvr [] snd = { null } ;
    ImageBvr [] unitIm = { null } ;
    NumberBvr len = importMovie(buildURL(_movieBase,"movie.avi"),
                 unitIm, snd);
          // loop it, by using the mod function
          unitIm[0] = (ImageBvr)unitIm[0].substituteTime(mod(localTime, len ));
    // scale into unit square
    return unitIm[0].mapToUnitSquare();
  }
 
// </Tutorial>
  
// <Tutorial Section=7.0 Title="Construct an Image of a Textured Rotating Fan">

  // Constructs the image of a rotating two-bladed fan, and
  // textures the given image on all of its four faces.
  public ImageBvr textureOnFan(ImageBvr unitImForTexture) {

    // quad is [-1,+1]x[-1,+1] with texture coordinates [0,1]x[0,1]
    GeometryBvr faceObj = importGeometry(buildURL(_geoBase,"quad.x"));
	faceObj = faceObj.transform(rotate(zVector3, toBvr(Math.PI/2)));

    // Replicate to give a textured backside as well.
    faceObj = union(faceObj,
               faceObj.transform(rotate(yVector3, toBvr(Math.PI))));    
    // Replicate both sides with a 90 degree rotation to make a fan.  
    faceObj = union(faceObj,
                faceObj.transform(rotate(yVector3, toBvr(Math.PI/2))));

    // construct a rotation around the Y axis  
    Transform3Bvr xf = rotate(yVector3, mul(localTime, toBvr(0.4)));
    // construct a rotating fan with the texture
    faceObj = faceObj.texture(unitImForTexture).transform(xf);
    // construct a camera with projection point at 3 and near plane at 2      
    CameraBvr cam = perspectiveCamera(toBvr(3),toBvr(2));
    // use a light and obtain an image
    GeometryBvr geoWithLight = union(faceObj, directionalLight);      
    ImageBvr im = geoWithLight.render(cam);

    return im.transform(compose(translate(0.5, 0.5),
                                scale2(0.4)));
  }
 
// </Tutorial>
  
// <Tutorial Section=8.0 Title="Tie it together and Construct the Model">

  public void createModel(BvrsToRun blst) {

    // set the URL bases for the media imports
    _mediaBase = getImportBase();
    _sndBase = buildURL(_mediaBase,"sound/");
    _movieBase = buildURL(_mediaBase,"movie/");
    _geoBase = buildURL(_mediaBase,"geometry/");

    // create the reactive cycling image for the puzzle
    ImageBvr unitIm = createUnitImageForPuzzle();

    // tile the image of the puzzle for playability
    _puzz = new PuzzleLogic(unitIm, _viewedOn, _mediaBase);
    puzzleIm = _puzz.getImage();

    // Give the puzzle a black background, 
    // so the empty square will be black.
    puzzleIm = overlay(puzzleIm, solidColorImage(black));

    // Establish this as a runOnce bvr, so that when the code 
    // switches modes, the puzzle doesn't restart.
    puzzleIm = (ImageBvr)puzzleIm.runOnce();
      
    // Now, create two images, one based on direct display and 
    // one based on texturing on the fan:

    // Translate the image down to center, and scale it down to 6 cms.
    ImageBvr directDisplayIm =
          puzzleIm.transform(compose(scale2(0.06),
                     translate(-0.5,-0.5)));

    // Texture the image on a 3-D rotating fan
    ImageBvr projectedGeo = textureOnFan(puzzleIm);
    projectedGeo =
          projectedGeo.transform(compose(scale2(0.07),
                         translate(-0.5,-0.5)));
    projectedGeo = overlay(projectedGeo, solidColorImage(black));

    // Cycle through 3-D and 2-D versions upon the toggle geometry event.
    ImageBvr imageToShow = ImageBvr.newUninitBvr();
    _toggleGeomProjection = new AppTriggeredEvent();
    imageToShow.init( until(directDisplayIm,
                         _toggleGeomProjection, until(projectedGeo,
                           _toggleGeomProjection, imageToShow)));
                  
    // These are the resultant image and sound that constitute the model.
    setImage(imageToShow);      
    setSound(_puzz.getSound());      
  }
 
// </Tutorial>
  public void cleanup() {
    super.cleanup();
    puzzleIm = null;
    _puzz.cleanup();
        _puzz=null;
        _viewedOn = null;
        _toggleGeomProjection = null;
        _toggleImageUsed = null;
  }

// <Tutorial Section=12.2>

  // Methods to trigger the AppTriggeredEvents, 
  // are called from DXMApplet.
  public void toggleGeomProjection() { _toggleGeomProjection.trigger(); }
  public void toggleImageUsed() { _toggleImageUsed.trigger(); }
  public void resetPuzzle() { _puzz.resetImage(); }

  // Use this event which originates from VBScript to toggle 
  // between direct image and fan.
  AppTriggeredEvent _toggleGeomProjection;
  // Use this event which originates from VBScript to toggle
  // between the different images for the puzzle.
  AppTriggeredEvent _toggleImageUsed;
  PuzzleLogic _puzz;
  Viewer _viewedOn;
  URL _mediaBase;
  URL _sndBase;
  URL _movieBase;
  URL _geoBase;
  ImageBvr puzzleIm;
}

// </Tutorial>
  
// <Tutorial Section=11.0 Title="High Level Structure for Interactivity and Logic">

// Contains the logic for doing the puzzle.
class PuzzleLogic extends Statics {
    
  // Take a unit image and chop it up into the pieces of the puzzle.
  public PuzzleLogic(ImageBvr unitSourceIm,
                     Viewer viewedOn,
                     URL importBase) {

    _viewedOn = viewedOn;     // for synchronization
    _squares = 3;
    _inc = 1.0 / _squares;

    _unitSourceIm = unitSourceIm;

    // The image is a switcher to facilitate the resetting
    // of the image; see the resetImage method below.
    ImageBvr initialImage = createImage();
    _imageSwitcher = new ModifiableBehavior(initialImage);
    _image = (ImageBvr)_imageSwitcher.getBvr();

    // will switch to these sounds as appropriate.      
    _successSound = importSound(buildURL(importBase,
                      "sound/butin.wav"), null);
    _failureSound = importSound(buildURL(importBase,
                      "sound/deflate.mp2"), null);
    // the puzzle sound is a switcher used in setHitSound below.
    _soundSwitcher = new ModifiableBehavior(silence);
    _sound = (SoundBvr)_soundSwitcher.getBvr();
  }

  // Just recreate the image and switch to it.  Synchronize with
  // the viewer so the code doesn't vie for the processor with it.
  public void resetImage() {
    synchronized (_viewedOn) {
      _imageSwitcher.switchTo( createImage() );
    }
  }

  // straightforward hit or fail sounds are generated.
  public void setHitSound(boolean success) {
    _soundSwitcher.switchTo(success ? _successSound : _failureSound);
  }
// </Tutorial>
  
// <Tutorial Section=9.0 Title="Constructing the Interactive Image of the Puzzle">

  // Construct the puzzle image as a composition of its
  // distinct parts.
  ImageBvr createImage() {

    int i;
    int j;
    double x = 0;
    double y = 0;
    ImageBvr totalIm = emptyImage;

    // build up the cells
    for (i = 0; i < _squares; i++, y += _inc) {
      x = 0;
      for (j = 0; j < _squares; j++, x += _inc) {

        // Skip the last one, to leave empty space.
        if (i == _squares - 1 && j == _squares - 1) continue;

        // Crop to get next cell and move lower left corner to the origin.
        ImageBvr croppedIm = _unitSourceIm.
                   crop(point2(toBvr(x), toBvr(y)),
                        point2(toBvr(x + _inc), toBvr(y + _inc)));
        croppedIm = croppedIm.transform(translate(toBvr(-x), toBvr(-y)));

        // make pickable
        PickableImage pick = new PickableImage(croppedIm);
        ImageBvr pickableCroppedIm = pick.getImageBvr();
        // extract probe event and create pick event
        DXMEvent probedEvent = pick.getPickEvent();
        DXMEvent pickedEvent = andEvent(leftButtonDown, probedEvent);

        // Initially translate back to original space.
        Transform2Bvr initXf = translate(toBvr(x), toBvr(y));
        // Allow transform to change as it's picked.
        
        Transform2Bvr xf = (Transform2Bvr)
          untilNotify(initXf, pickedEvent,
            new ImagePicker(i, j, pickedEvent, this));

        // The entire cell is the transform applied to the cell
        // at the origin.
        ImageBvr movingIm = pickableCroppedIm.transform(xf);

        // accumulate the cells of the puzzle
        totalIm = overlay(movingIm, totalIm);
      }   // first for
    }     // second for

    // Establish initial empty cell.
    _emptyRow = _squares - 1;
    _emptyCol = _squares - 1;

    return totalIm;
      
  } 

// </Tutorial>
  
// <Tutorial Section=11.1 Title="Constructing the Interactive Image of the Puzzle">

  public ImageBvr getImage() { return _image; }
  public SoundBvr getSound() { return _sound; }

  public void cleanup() {
    _successSound = _failureSound = _sound = null;
    _soundSwitcher = _imageSwitcher = null;
    _unitSourceIm = _image = null;
    _viewedOn = null;
  }

  int      _squares;
  int      _emptyRow;
  int      _emptyCol;
  double   _inc;
  SoundBvr _successSound;
  SoundBvr _failureSound;
  ModifiableBehavior _soundSwitcher;

  ImageBvr _unitSourceIm;
  ImageBvr _image;
  ModifiableBehavior _imageSwitcher;
  SoundBvr _sound;
  Viewer   _viewedOn;
}

// </Tutorial>
  
// <Tutorial Section=10.0 Title="The Notification Object upon Puzzle Events">

// This object accepts a user event, and if it's a legal move
// (cell that is adjacent to the empty location), then the cell
// is transitioned and success sound is emitted, else illegal move
// sound is emitted.
class ImagePicker extends Statics implements UntilNotifier {
  public ImagePicker(int row,
                     int col,
                     DXMEvent ev,
                     PuzzleLogic puzz) {
        _row = row;
        _col = col;
        _ev = ev;
        _puzz = puzz;
  }

  // invoked through the untilNotify construct
  public Behavior notify(Object eventData,
                         Behavior currentRunningBvr,
                         BvrsToRun lst) {
      
    Behavior xf = currentRunningBvr;
    int r = _puzz._emptyRow;
    int c = _puzz._emptyCol;

    // check to see if this is a legal move
    if (((Math.abs(_row - r) == 1) && _col == c) ||
        ((Math.abs(_col - c) == 1) && _row == r)) {

      // find cell positions before and after the move          
      Point2Bvr start = point2(toBvr(_col * _puzz._inc),
                                 toBvr(_row * _puzz._inc));
      Point2Bvr finish = point2(toBvr(c * _puzz._inc),
                                  toBvr(r * _puzz._inc));

      // Build up a transform to move from orig to final in 0.5 second
      double transitionTime = 0.5;  

      // the vector from start to finish
      Vector2Bvr delta = sub(finish, start);

      // pos begins at start and transitions to finish in transitionTime seconds
      Point2Bvr pos = add(start, delta.mul(div(localTime, toBvr(transitionTime))));

      // extract a translation from moving position
      Transform2Bvr movingXf = translate(sub(pos, origin2));

      // Use this transform, and after the elapsed time freeze
      // in place using a snapshot event.
      xf = untilEx(movingXf,
        timer(toBvr(transitionTime)).snapshotEvent(movingXf));
      // an alternative is: ,translate(finish.getX(), finish.getY()));

      // Set the new location of the empty cell
      _puzz._emptyCol = _col;
      _puzz._emptyRow = _row;

      // Set what position this cell is at now
      _col = c;
      _row = r;

      // emit success sound
      _puzz.setHitSound(true);     
      
    } else {

      // emit illegal move sound
      _puzz.setHitSound(false);
    }
      // Return this new transform, waiting again for the pick event. 
      return untilNotify(xf, _ev, this);
  }

  int _row;
  int _col;
  DXMEvent _ev;
  PuzzleLogic _puzz;    
}
// </Tutorial>

