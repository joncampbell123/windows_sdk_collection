// <Tutorial Section=1.0 Title="Button that Reacts to a Mouse">
//
/** This applet shows a black button that starts to change colors with time<BR>
when the user's mouse moves over it. When the user clicks on the button with<BR>
the left mouse button, the applet takes a snapshot of the time-varying color.<BR>
When the user moves the mouse away from the button, its color returns to black.

This applet illustrates the following:<BR><BR>
- creating a solid-colored square.<BR>
- making that square react to mouse over and left mouse button clicks.<BR>
<BR>
**/
import com.ms.dxmedia.*;    // All DirectAnimation classes

// ButtonPick is an applet that invokes ButtonPickModel which constructs a
// solid-colored square.
public class ButtonPick extends DXMApplet {
  public void init() {
    super.init() ;
    setModel (new ButtonPickModel());
  }
}

// In the ButtonPickModel class the createModel method is where you construct 
// your animation.
class ButtonPickModel extends Model {

  public void createModel(BvrsToRun blist)
  {
    // Create a detectable image and crop it to 100 by 100 pixels.
    ImageBvr detectImg = detectableEmptyImage.
      crop(point2(mul(toBvr(-50),pixelBvr), mul(toBvr(-50),pixelBvr)),
        point2(mul(toBvr(50),pixelBvr), mul(toBvr(50),pixelBvr)));

    // Make the above created image pickable.
    PickableImage pickPImg = new PickableImage(detectImg);

    // Create an event that will trigger when the mouse moves over detectImg.
    DXMEvent pickEv = pickPImg.getPickEvent();

    // Create an event that will trigger when both the mouse moves over 
    // detectImg, and the left mouse button is clicked.
    DXMEvent clickEv = andEvent(leftButtonDown, pickEv);

    // Create a color behavior that changes its hue with time.
    ColorBvr changeClr = colorHsl(add(mul(sin(
      localTime),toBvr(0.5)),toBvr(0.5)),
        toBvr(0.5),toBvr(0.5));

    // Create a color behavior that starts out as black, changes to changeClr 
    // when the mouse moves over detectImg (pickEv), and returns to black 
    // after the mouse is no longer over detectImg (notEvent(pickEv)).
    ColorBvr overClr = ColorBvr.newUninitBvr();
    overClr.init(until(
      black,pickEv,until(changeClr,notEvent(pickEv),overClr)));

    // Create an event that takes a snapshot of overClr when detectImg is
    // clicked with the left mouse button.
    DXMEvent snapEv = clickEv.snapshotEvent(overClr);

    // Create two color behaviors.  One (snapClr) that starts out as overClr,
    // and changes to whatever color overClr is when detectImg is clicked 
    // with the left mouse button. The second (totalClr) starts out as 
    // snapClr, and changes to black when the mouse is no longer over 
    // detectImg.
    snapClr = (ColorBvr)untilNotify(overClr,snapEv,
      new ButtonPickSnapshot());

    ColorBvr totalClr = ColorBvr.newUninitBvr();
    totalClr.init(until(
      snapClr,notEvent(pickEv),totalClr));

    // Create a solid color image, crop it to 100 by 100 pixels,
    // and apply the color behavior of totalClr to it.
    ImageBvr initImg = solidColorImage(totalClr).
      crop(point2(mul(toBvr(-50),pixelBvr), mul(toBvr(-50),pixelBvr)),
        point2(mul(toBvr(50),pixelBvr), mul(toBvr(50),pixelBvr)));

    // Put the pickable Image on top of initImg.
    ImageBvr finalImg = overlay(pickPImg.getImageBvr(),initImg);
    
    //Display the image on a blue background.
    setImage(overlay(finalImg, solidColorImage(blue)));  
  }
  public void cleanup() {
    super.cleanup();
	snapClr = null;
  }
  ColorBvr snapClr;
}

class ButtonPickSnapshot extends Statics implements UntilNotifier {
  
  public Behavior notify(Object eventData,
    Behavior currentRunningBvr, BvrsToRun btr) {   

    // Returns the color of overClr when the left mouse button is clicked
    // on detectImg.
    return (ColorBvr)eventData;
  }
}

// Thats all there is to it.  Happy animating...
// </Tutorial>
