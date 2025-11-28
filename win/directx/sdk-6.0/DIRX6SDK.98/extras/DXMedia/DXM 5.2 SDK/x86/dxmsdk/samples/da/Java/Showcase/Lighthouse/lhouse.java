// lighthouse.java 
// Club Microsoft Lighthouse Demo
// 
// <Tutorial Section=1.0 Title="Section1: Club Microsoft Lighthouse Tutorial Introduction">
/**
  This is one of the premier demonstrations of the kinds of integration and
the possibilities available when using the DirectAnimation
libraries. The main feature of this sample is the use of audio and graphics
that are both controlled by a common animation description (the wind speed). It
integrates 3-D models (the bird, the boat, and the weather vane).  It also has
more traditional sprite animation (the surf, the trees, the sky, and the boy
with the kite).  In addition, the sample makes use of synthetic audio, where
sound seeds are dynamically manipulated and mixed together randomly for the
both the ocean and seagull ambient sounds.  This gives a rich audio environment
which can respond to the user-controlled wind speed without having to download
a long audio track.  Events are used to synchronize the surf
animation with the ocean surf sound.  In addition, the seagull's sound is 
spatialized based on its 3-D location, and it has a flying behavior which is
both autonomous and interactive (when dragging the bird).  All three forms
of media are coordinated and influenced by weather intensity, which is driven
from VBScript by a slider.

  The tutorial leads you through the process of creating this complex animation, 
step-by-step.  Section 2 illustrates how to use VBScript to make the scrollbar control
the wind speed.  Section 3 describes how animated cell images are constructed using
a helper class.  Section 4 describes the process of integrating 3-D geometries into
the sample.  Section 5 illustrates the synthetic audio helper class and how it has been used
to create some handy synthetic modules.  Finally, Section 6 shows how to make the seagull
interact with the mouse.<BR><BR>
**/
// </Tutorial>

import com.ms.dxmedia.*;  // gets DirectX Media environment
import java.net.*;
import java_utility.*;   // get cellImage1 and helper funcs
import lhouse_module.*;   // gets oceanSynth,seagull,sailboat and weatherVane

public class lhouse extends DXMApplet  {
	private lhouseModel _model;

  public lhouse()  { 
		_model = new lhouseModel();
		setModel(_model); 
  }

  public void setWindSpeed(int val)  {     
		_model.setWindSpeed(val); 
  }
}

class lhouseModel extends Model  {
  public grabbedBird _grabbedNotify;
  public thrownBird  _thrownNotify;

  // <Tutorial Section=2.1>
  // Here, back in the lhouseModel class, declare a number of variables that
  // will be used throughout the sample.  These variables are declared as 
  // switchers so that the behavior (Bvr) they contain (a number) can be 
  // updated from an external method or a notifier.
  public ModifiableBehavior _windSpeed;  // Normalized value between 0 and 1
  public ModifiableBehavior _windMode;
  public ModifiableBehavior _breezeInterval;
  public DXMEvent _grabEvent;
  public Vector3Bvr  _originalPath;
  public Vector3Bvr _actualPath;

  // This is the method that the applet will call to update the wind speed.
  // Notice the switchTo call that swaps the current Bvr for a new one.
  // Note: to convert a primitive Java data types (such as integers) to behaviors
  // use the toBvr() helper method.
  public void setWindSpeed(int val)  { 
    _windSpeed.switchTo( div(toBvr(val),toBvr(10)) );
    _windMode.switchTo(mul(add(toBvr(val),toBvr(1)),toBvr(0.9)));
  }

  // The createModel method, tells DirectAnimation how to construct a 
  // run time model for the animation. 
  public void createModel(BvrsToRun blist)  {

    // Begin the demo code by initializing switchers and
    // giving them their default number behaviors. For convenience
    // get the actual Bvr out of the switcher and store it as 
    // the variable windSpeed.  Also, create a windDistance value based
    // on the windSpeed (this will be used to move the clouds later on).
    // The most interesting variable here is the WindAngle which is a 
    // value that changes randomly over time.  The example on this page
    // allows you to see the changing values of these numbers as you move
    // the slider.
    _windSpeed = new ModifiableBehavior(toBvr(0));
    _windMode = new ModifiableBehavior(toBvr(0));
    _breezeInterval = new ModifiableBehavior(toBvr(2));   

    NumberBvr windSpeed = (NumberBvr)_windSpeed.getBvr();
    NumberBvr windMode = (NumberBvr)_windMode.getBvr();
    NumberBvr windDistance = integral(windSpeed);
    NumberBvr windAngle = generateWindAngle(blist);
    NumberBvr random = seededRandom(.11);
    blist.add(random);

    // </Tutorial>

    // <Tutorial Section=3.0 Title="Section3: Including Cell Based Images">
    // Here, begin importing the raw images.  If the image has multiple cells
    // make use of the cellImage1 utility.
    ImageBvr clouds,scene;
    cellImage1 kite,ocean1,ocean2,palm1,palm2,palm3,beachboy;
                
    URL mediaBase = getImportBase();
    URL imgBase = buildURL(mediaBase, "image/");
    URL geoBase = buildURL(mediaBase, "geometry/");
    URL sndBase = buildURL(mediaBase, "sound/");
		
    kite     = new cellImage1(buildURL(imgBase, "kite.gif"),4);
    scene    = importImage(buildURL(imgBase, "scene.jpg"));
    ocean1   = new cellImage1(buildURL(imgBase, "ocean.gif"),4);
    ocean2   = new cellImage1(buildURL(imgBase, "ocean2.gif"),4);
    palm1    = new cellImage1(buildURL(imgBase, "palm1.gif"),5);
    palm2    = new cellImage1(buildURL(imgBase, "palm2.gif"),5);
    palm3    = new cellImage1(buildURL(imgBase, "palm3.gif"),5);
    clouds   = importImage(buildURL(imgBase, "clouds.gif"));
    beachboy = new cellImage1(buildURL(imgBase, "beachboy.gif"),2);

    // Here, use the synthetic ocean sounds that are explained in Section 5.
    // The event caused by a crashing wave is used to create an ocean index.
    // An animateTrees method is called to generate psuedo-random tree
    // indexes. These indexes will be used with the showIndex method to
    // determine which tree cell to display. 
    oceanSynth ocean = new oceanSynth(add(toBvr(.2),windSpeed));
    NumberBvr oceanIndex = animateOcean(ocean.soundEvent());
    NumberBvr treeIndex1 = animateTrees(windSpeed,   1);
    NumberBvr treeIndex2 = animateTrees(windSpeed, .88);
    NumberBvr treeIndex3 = animateTrees(windSpeed, .71);

    // </Tutorial>

    // <Tutorial Section=4.1>
    // Back in the lhouseModel class, use the getGeo() model to construct
    // the vane, and transform it to match the perspective of the scene.
    GeometryBvr vane = 
      weatherVane.getGeo(windAngle,windDistance, geoBase).transform(
        compose(translate(toBvr(-1.1),toBvr(-0.5),toBvr(0)),
          compose(rotate(xVector3, toBvr(Math.PI/12)),
            compose(rotate(yVector3, toBvr(Math.PI/4)),
              scale3(toBvr(0.6))))));

    // </Tutorial>

    // <Tutorial Section=3.3>
    // Here, construct moving clouds by translating an infinitely tiled
    // image of clouds by the windDistance.
    Bbox2Bvr BB = clouds.boundingBox();

    Vector2Bvr cloudPos = 
      vector2(neg(mod(div(windDistance, toBvr(200)), 
        sub(BB.getMax().getX(), BB.getMin().getX()) )), toBvr(0));

    ImageBvr xfClouds = clouds.transform(translate(cloudPos));

    ImageBvr movingClouds = xfClouds.tile().crop(BB.getMin(), BB.getMax());

    // </Tutorial>

    Point2Bvr kitePos = (Point2Bvr)
      add( point2(mul(toBvr(-220),pixelBvr), mul(toBvr(100),pixelBvr)),                     
        vector2(mul(add( mul( toBvr(30), mul(windSpeed,sin(localTime))), 
          mul(toBvr(4),random)), pixelBvr), 
            mul(add( mul( toBvr(10), 
	      mul(windSpeed,sin(add(localTime,toBvr(5))))),
                mul(toBvr(4),random)), pixelBvr))).runOnce();

    Point2Bvr bboyPos  = point2( mul(toBvr(-45),pixelBvr), mul(toBvr(-125),pixelBvr));
    Point2Bvr startPos = add( kitePos, vector2(mul(toBvr(34),pixelBvr), mul(toBvr(10),pixelBvr)) );
    Point2Bvr endPos   = add( bboyPos, vector2(mul(toBvr(-17),pixelBvr), mul(toBvr(16),pixelBvr)) ); 
    Point2Bvr halfPos  = add( startPos, sub(endPos, startPos).mul(toBvr(.5)));
                
    NumberBvr[] knots = { toBvr(0), toBvr(0), toBvr(0), 
			toBvr(1), toBvr(2), toBvr(2), toBvr(2) };

    NumberBvr[] weights = { toBvr(1), toBvr(1), toBvr(1), toBvr(1), toBvr(1) };

    Point2Bvr[] points =  {
      endPos,
      add(add( halfPos, sub(endPos, halfPos).mul(toBvr(.5)) ),
      vector2(mul(mul(toBvr(15),pixelBvr), sin(localTime)), toBvr(0)) ),
      halfPos,
      add(add( startPos, sub(halfPos, startPos).mul(toBvr(.5)) ),
      vector2(mul(mul(toBvr(-25),pixelBvr), sin(localTime)), toBvr(0)) ),
      startPos 
    };

    Path2Bvr stringPth2 = cubicBSplinePath(points, knots);
    LineStyleBvr stringLn = defaultLineStyle.width(toBvr(0.1*mm));
    ColorBvr stringCol = colorRgb(toBvr(140/256),toBvr(181/256),toBvr(241/256));
    ImageBvr kiteString = stringPth2.draw(stringLn.color(stringCol));

    // <Tutorial Section=3.4>
    // Here, create an array of all the time-varying sprite images, using
    // loopStrip and showIndex where appropiate.
    ImageBvr[] sprites = {             
      kite.loopStrip(add(windSpeed,toBvr(.2))), 
      scene,
      ocean1.showIndex(oceanIndex),
      ocean2.showIndex(oceanIndex),
      palm1.showIndex(treeIndex1),
      palm2.showIndex(treeIndex2),
      palm3.showIndex(treeIndex3),
      movingClouds,
      beachboy.loopStrip(toBvr(1))
    };
		
    // Also create an array of points which will be used as the initial
    // positions of the above mentioned time-varying sprites. 
    Point2Bvr[] spritePos = { 
      kitePos,
      point2(toBvr(         0),toBvr(         0)),
      point2(mul(toBvr(  78),pixelBvr),mul(toBvr(-100),pixelBvr)),
      point2(mul(toBvr(-252),pixelBvr),mul(toBvr( -21),pixelBvr)),
      point2(mul(toBvr(-250),pixelBvr),mul(toBvr(  20),pixelBvr)),
      point2(mul(toBvr(-158),pixelBvr),mul(toBvr(   2),pixelBvr)),
      point2(mul(toBvr( -98),pixelBvr),mul(toBvr(  -1),pixelBvr)),
      point2(toBvr(         0),mul(toBvr( 106),pixelBvr)),
      bboyPos
    };

    // Then apply the sprite translation to all the sprites, to move them
    // to their initial location.
    ImageBvr[] iSprites = new ImageBvr[9];
      for(int i=0; i<9; i++) 
        iSprites[i] = sprites[i]
          .transform(translate(sub(spritePos[i],origin2)));

    // Overlay all the 2-D sprites (the first overlay is at the top
    // and the last will be at the bottom).  This new composite is made 
    // pickable for later use.
    PickableImage allSprites = new PickableImage(
      overlay(iSprites[0], overlay(iSprites[8], overlay(kiteString,
        overlay(iSprites[4], overlay(iSprites[5], overlay(iSprites[6],
          overlay(iSprites[7], overlay(iSprites[2], 
            overlay(iSprites[3], iSprites[1] ))))))))));

    // Finally, obtain the upper right corner of the composite, which will
    // be used to scale the geometries.
    Point2Bvr imageUR = allSprites.getImageBvr().boundingBox().getMax();

    // </Tutorial>

    // <Tutorial Section=6.1>
    // The original path is then constructed with the help of the figure8
    // utility method.  This method uses the spline method to return a path
    // in the form of a figure8.
    Vector3Bvr subPath = 
      (Vector3Bvr)(add( DxmVector3.figure8(),
        vector3(sin(localTime), 
          sin(mul(localTime, toBvr(.25))),
            sin(mul(localTime, toBvr(.33))))
	      .mul(toBvr(.25)) )).runOnce();
                                
    _originalPath = (Vector3Bvr)
      subPath.transform(
        compose( rotate(xVector3, toBvr(Math.PI/8)), 
          rotate(yVector3, toBvr(Math.PI/2)) ))
          .substituteTime(div(localTime, toBvr(2)));

    // Next an event is created that triggers when the left mouse button is
    // pressed on the pickable image that was created in section 3.
    _grabEvent = andEvent(leftButtonDown, allSprites.getPickEvent()); 

    // Construct two class objects.  Their classes handle the behavior of the
    // bird when it is grabbed, and when it is released.
    _grabbedNotify = new grabbedBird(this);
    _thrownNotify = new thrownBird(this);

    // </Tutorial>

    // <Tutorial Section=6.4>
    // Create a behavior that starts out as _original path, and then changes to
    // the behavior constructed in the grabbedBird class when _grabEvent is
    // triggered.
    Vector3Bvr finalPath = (Vector3Bvr)untilNotify(
      _originalPath, _grabEvent, _grabbedNotify);

    // Create an event that gets triggered when the left mouse button is
    // released.  Also create an event that snapshots the value of finalPath
    // when throwEvent is triggered.
    DXMEvent throwEvent = leftButtonUp;
    DXMEvent throwSnapEvent = throwEvent.snapshotEvent(finalPath);

    // Create a behavior that starts out as finalPath, and then changes to
    // the behavior constructed in the thrownBird class when throwSnapEvent is
    // triggered.  This is the composite seagull path that allows the user to 
    // grab it, drag it, and release it.
    _actualPath = (Vector3Bvr)untilNotify(
      finalPath, throwSnapEvent, _thrownNotify);
    // </Tutorial>

    // <Tutorial Section=6.0 Title="Section6: Making the seagull react to the mouse">
    // This section describes how to make the seagull react to the user's
    // mouse.  When the user clicks and holds down the left mouse button,
    // the seagull can be dragged across the screen.  When the mouse button is
    // released, the seagull returns to its original path.
    // First construct the seagull in the same manner as the weather vane in
    // Section 4, and apply its flight path to it.
    seagullSynth seagull = new seagullSynth(_actualPath);        
    GeometryBvr gull = seagull.getGeo(true, mediaBase)
      .transform(translate(toBvr(0), toBvr(.5), toBvr(0)));

    // </Tutorial>

    // <Tutorial Section=4.3>
    // Overlay all of the rendered geometry images over the image composed of
    // all the sprites.
    ImageBvr composite =
      overlay(geometryImage(gull, imageUR), 
        overlay(geometryImage(sailBoat.getGeo(windSpeed, geoBase), imageUR),                     
          overlay(geometryImage(vane, imageUR), allSprites.getImageBvr())));

    // </Tutorial>

    // To see a script version of animated trees, comment out the setImage(composite) line,
    // and activate the line below.

    //setImage(overlay(finalPalm,solidColorImage(white)));

    setImage(composite);

    // <Tutorial Section=5.0 Title="Section5: Using Synthetic Audio">
    // Here, compose the audio for this demo.  Both the ocean and seagull
    // classes have static methods for defining the sound.  This tutorial 
    // discusses how the sound is generated for the ocean module.
    setSound(mix(ocean.getSound(sndBase), seagull.getSound(sndBase))); 

    // </Tutorial>
  }

  // <Tutorial Section=3.2>
  // Here is how these two methods operate.
  private NumberBvr animateOcean(DXMEvent ev)  {

    //Sequence for clam ocean.
    Behavior[] calm = { toBvr(0), toBvr(1) }; 

    //Sequence for crashing wave.
    Behavior[] wave = { toBvr(2), toBvr(3), toBvr(2) };

    // Cycles through the calm values every 0.5 seconds.
    Cycler calmCycle = new Cycler(calm, timer(toBvr(.5)));

    // Cycles through the calm values every 0.25 seconds.
    Cycler crashCycle = new Cycler(wave, timer(toBvr(.25)));

    // Use an uninitialized behavior so it can refer to itself.
    NumberBvr animateOcean = NumberBvr.newUninitBvr();

    // The behavior is calmCycle until ev, then crashCycle for 0.75 seconds,
    // and then back to the initial state.
    animateOcean.init((NumberBvr)
      until(calmCycle.getBvr(), ev, 
        until(crashCycle.getBvr(), timer(toBvr(.75)), animateOcean)));

    return animateOcean;
  }

  private NumberBvr animateTrees(NumberBvr windSpeed, double seed)  {

    // A jitter method gives a semi-random value that oscillates between
    // zero and one.
    NumberBvr jitter = 
      floor(mod(
        add(DxmNumber.smooth0to1(mul(sub(toBvr(1.2),windSpeed), toBvr(10*seed))),                                         
          (NumberBvr)_breezeInterval.getBvr()),
            toBvr(2)));

    // As the wind speed increases, the trees tend to bend back.  To get this
    // effect increase the cell image used.
    NumberBvr normalIndex = floor(mul(toBvr(3), windSpeed));

    // The final index jiggles the bent tree.
    return add(normalIndex, jitter);
  }
  // </Tutorial>

  // <Tutorial Section=4.2>
  // In order to use the geometry in the scene, convert the geometry
  // to an image.  Here, define a helper method that takes any geometry,
  // and the upper right corner of the scene.  It then return a rendered and
  // scaled image.
  private ImageBvr geometryImage(GeometryBvr geo, Point2Bvr UR)  {
    CameraBvr camera = 
      perspectiveCamera(toBvr(1),toBvr(0))
        .transform(translate(toBvr(0), toBvr(0), toBvr(2)));
		
    NumberBvr scaleFactor = (NumberBvr) 
      cond(lte(UR.getX(), UR.getY()), UR.getX(), UR.getY());
		
    return geo.render(camera).transform(scale2(mul(toBvr(2.8), scaleFactor)));                      
  }

  // </Tutorial>

  // <Tutorial Section=2.2>
  // Above, you saw the use of the generateWindAngle to actually get the random
  // value, here is what the method does:
  private NumberBvr generateWindAngle(BvrsToRun blist)  {
    ModifiableBehavior breezeAngle = new ModifiableBehavior(toBvr(0));

    // Explicitly start a seededRandom behavior.
    NumberBvr random = seededRandom(.27);  
    blist.add(random);

    // Create an event which triggers after each breeze interval passes.
    DXMEvent timerEv = timer((NumberBvr)_breezeInterval.getBvr());

    // Attach the random value as eventData.
    timerEv = timerEv.snapshotEvent(random); 
		
    // You want a number behavior that refers to itself, so use newUninitBvr().		
    NumberBvr loop = NumberBvr.newUninitBvr();

    // To get a randomly changing breeze, create a method that will 
    // get called every time the timerEv event occurs, then smoothly
    // transitions our current breezeAngle to a new random breezeAngle.
    // This is the purpose of the newBreeze class, which extends UntilNotifier.
    // Pass it the values it will need to construct and return a new behavior.
    UntilNotifier newBreeze = new Breeze((NumberBvr)_windSpeed.getBvr(),
      _breezeInterval, breezeAngle, loop);

    NumberBvr angle = (NumberBvr)breezeAngle.getBvr();

    timerEv = andEvent(timerEv, timerEv.snapshotEvent(angle));

    // Now, initialize the loop to the value that is the current breezeAngle,
    // until timerEv, when the newBreeze notify method is called.
    loop.init((NumberBvr)untilNotify(angle, timerEv, newBreeze));
                
    // return this value as the actual breeze angle.		  
    return loop; 
  }

  // </Tutorial>

  private ImageBvr cropStrip(ImageBvr img, NumberBvr index, NumberBvr frames)  {
    Bbox2Bvr stripBBx2 = img.boundingBox();
    Point2Bvr minPt2 = stripBBx2.getMin();
    Point2Bvr maxPt2 = stripBBx2.getMax();
    NumberBvr ttlHghtNum = sub(maxPt2.getY(), minPt2.getY());
    NumberBvr frmHghtNum = div(ttlHghtNum, frames);

    Point2Bvr crop1Pt2 = point2(minPt2.getX(), 
      div(neg(frmHghtNum),toBvr(2)));
    Point2Bvr crop2Pt2 = point2(maxPt2.getX(), 
      div(frmHghtNum,toBvr(2)));

    NumberBvr temp1_1 = add(ttlHghtNum, frmHghtNum);
    NumberBvr temp1_2 = mul(index, frmHghtNum);
    NumberBvr temp1_3 = div(temp1_1,toBvr(2));
    NumberBvr temp1_4 = sub(temp1_2, temp1_3);

    Transform2Bvr temp = translate(toBvr(0), temp1_4);

    ImageBvr temp2 = img.transform(temp);
	
    return temp2.crop(crop1Pt2, crop2Pt2);
  }

  private ImageBvr animate(ImageBvr img, NumberBvr frames, NumberBvr index)  {
    BooleanBvr condBool = lte(frames,toBvr(1));

    NumberBvr temp1 = floor(index);
    NumberBvr temp2 = mod(temp1, frames);
    NumberBvr temp3 = add(temp2, toBvr(1));

    ImageBvr cropImg = cropStrip(img, temp3  ,frames);

    return (ImageBvr)cond(condBool,img,cropImg);
  }

  private ImageBvr applySettings(ImageBvr img, Point2Bvr pt, 
    NumberBvr frames, NumberBvr index)  {
    return animate(img, frames, index).transform(translate(sub(pt, origin2)));
  }

  private NumberBvr treeFrame (NumberBvr windMode, NumberBvr num)  {
    NumberBvr clamp1Num = mul(div(sub(windMode,toBvr(2)),toBvr(8)),toBvr(3));

    NumberBvr random = seededRandom(0.1);


    // added extra tweak (*0.9) to stretch the sine function out.
    NumberBvr condNum = sin(mul(mul(mul(windMode,random),num),toBvr(0.9)));

    BooleanBvr condBool = lte(condNum,toBvr(0));
    
    NumberBvr jitterNum = (NumberBvr)cond(condBool,toBvr(0),toBvr(1));

    return add(DxmNumber.clamp(clamp1Num, toBvr(0), toBvr(3)),jitterNum);
  }
}

// <Tutorial Section=2.3>
// The callback serves an important function in DirectAnimation. It allows
// the reconstruction of a running behavior based on its current value and
// the time that it was triggered.  In the breeze class, the sample uses the 
// constructor to pass it all the values it needs to update the current
// behavior.
class Breeze extends Statics implements UntilNotifier {
  private ModifiableBehavior _breezeInterval;
  private ModifiableBehavior _breezeAngle;
  private NumberBvr _windSpeed;
  private NumberBvr _loop;
  
  public Breeze(NumberBvr windSpeed, ModifiableBehavior breezeInterval, 
		ModifiableBehavior breezeAngle, NumberBvr loop)  {
    _windSpeed = windSpeed;
    _breezeInterval = breezeInterval; 
    _breezeAngle = breezeAngle; 
    _loop = loop;
   }

  // Now in the notify method, you can do some actual work.
  public Behavior notify(Object eventData, Behavior curBvr, BvrsToRun lst)  { 
		
    // First snapshot (freeze) the random value attached as eventData.
    NumberBvr random = (NumberBvr)((PairObject)eventData).getFirst(); 
		
    // Also snapshot the current breezeAngle based on the event time.
    NumberBvr oldAngle = (NumberBvr)((PairObject)eventData).getSecond(); 
		
    // Calculate the difference delta between the two and transition the 
    // breezeAngle from one to the other over a period of two seconds.
    NumberBvr deltaAngle = mul(toBvr(2), sub(random,toBvr(.5)));

    _breezeInterval.switchTo(mul(toBvr(5), random));
    _breezeAngle.switchTo(add(oldAngle,deltaAngle));

    NumberBvr changingAngle = 
      add(oldAngle, mul(deltaAngle, DxmNumber.smooth0to1(toBvr(2))));

    return (NumberBvr)until(changingAngle, timer(toBvr(2)), _loop);                           
  }
}
// </Tutorial>

// <Tutorial Section=6.2>
// The grabbedBird is a callback just like the Breeze class in Section 2.
// It links the movement of the bird to the position of the mouse pointer.
class grabbedBird extends Statics implements UntilNotifier  {
  private lhouseModel _model;     

  public grabbedBird(lhouseModel model)  { 
    _model = model; 
  }

  // Now in the notify method you can do some actual work.
  public Behavior notify(Object eventData, Behavior curBvr, BvrsToRun lst)  {                                            
    // Grab the position of the mouse pointer from the eventData tree.
    Vector2Bvr offset = 
      (Vector2Bvr)((PairObject)((PairObject)eventData)
        .getSecond()).getSecond();
		
    Vector3Bvr offset3d = 
      vector3( offset.getX(), mul(toBvr(.75), offset.getY()), 
      mul(toBvr(-.25),offset.getY()) ).mul(toBvr(30));
		
    Vector3Bvr newPath = add((Vector3Bvr)curBvr, offset3d);

    return newPath;
  }
}

// </Tutorial>

// <Tutorial Section=6.3>
// The thrownBird is a callback just like the Breeze class in Section 2.
// It returns the seagull to its original path when it's released by the user.
class thrownBird extends Statics implements UntilNotifier  {
  private lhouseModel _model;     
	
  public thrownBird(lhouseModel model)  {
    _model = model; 
  }

  // Now in the notify method you can do some actual work.
  public Behavior notify(Object eventData, Behavior curBvr, BvrsToRun lst)  {
    // Obtain actualPath from the lhouseModel class. 
    Vector3Bvr originalPath = _model._actualPath;

    // Obtain the initial position of the seagull.
    Vector3Bvr pathVector = (Vector3Bvr)originalPath
      .substituteTime(toBvr(0));

    // Create a 3-D vector that contains the postion of the finalPath vector
    // (in class lhouseModel) when throwEvent is triggered.
    Vector3Bvr throwVector = (Vector3Bvr)eventData;

    Vector3Bvr throwDirection = derivative((Vector3Bvr)curBvr);
    Vector3Bvr pathDirection = vector3(toBvr(-1), toBvr(0), toBvr(0));
			
    Vector3Bvr[] points = {
      throwVector,
      add(throwVector, throwDirection),
      add(add(throwVector, pathVector).div(toBvr(2)), 
      sub(throwDirection, pathDirection)),
      sub(pathVector, pathDirection.div(toBvr(3))),
      pathVector                  
    };
			
    NumberBvr[] knots = { toBvr(0), toBvr(0), toBvr(0),
      toBvr(1), toBvr(2), toBvr(2), toBvr(2) };
    NumberBvr[] weights = { toBvr(1), toBvr(1), 
      toBvr(1), toBvr(1), toBvr(1) };

    // Create a path from the point of where the seagull was released,
    // to its original starting point.
    Vector3Bvr spline = bSpline(3, knots, points, weights,localTime);
		
    // Let the seagull fly the path in two seconds, and then return to its
    // original path.
    Vector3Bvr throwPath = (Vector3Bvr)
      until( spline, timer(toBvr(2)),originalPath);
		
    return throwPath;                                                               
  }
}

// </Tutorial>

