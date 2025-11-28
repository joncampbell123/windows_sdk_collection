// <Tutorial Section=1.0 Title="Using DirectAnimation to Jump to another Website">
//
// There are two main pieces to any DirectAnimation applet, the DXMApplet 
// class and the Model class. To use the DirectAnimation classes, import 
// the media libraries like this:
import com.ms.dxmedia.*;
import java.awt.*;
import java.net.*;     // gets URL.
import java.applet.*;  // gets getAppletContext.

public class JumpURL extends DXMApplet  {

  public void init()  {
    super.init() ;
    JumpURLModel newJump = new JumpURLModel();
    setModel(newJump);
		// Call AppletContext() and set _getAppContext to its return.  
		// _getAppContext will be used to call showDocument().
    newJump.appLink(getAppletContext());
  }
}

// Then create a class that extends the Model class.  In this class the 
// createModel method is where you construct your animation.  This example 
// will make use of two of the primary media types supported by 
// DirectAnimation: images and sound.
class JumpURLModel extends Model  {

  public void createModel(BvrsToRun blist)  {
    // Create bases for importing images and sounds.
    URL mediaBase = getImportBase();
    URL imgBase = buildURL(mediaBase,"image/");
    URL sndBase = buildURL(mediaBase,"sound/");

    // Import seattle.jpg.
    ImageBvr seattleImg = importImage(buildURL(imgBase,"seattle.jpg"));

    // Import the sound that will be played when the mouse moves over 
	// seattleImg.
	SoundBvr pickSnd = importSound(buildURL(sndBase,
	  "etherial.mp2"),null).loop();
  
    // Make seattleImg pickable.
    PickableImage pickPImg = new PickableImage(seattleImg,true);

    // Create an event that will trigger when the mouse moves over detectImg.
    DXMEvent pickEv = pickPImg.getPickEvent();

    // Get the image part of pickPImg.
    ImageBvr pickImg = pickPImg.getImageBvr();

    // Create a string that contains the URL to Seattle Sidewalk.
    StringBvr urlStr = toBvr("http://seattle.sidewalk.com");

	// Create the tip box (tipImg), which consists of a black 
	// backgroud (tipBackIm), and white text (tipTxtIm).
	FontStyleBvr font = font("Arial",12,white).bold();
	ImageBvr tipTxtImg = stringImage(
	  "Click the red dot to go to Seattle Sidewalk", font);

    Bbox2Bvr txtBB = tipTxtImg.boundingBox();

    ImageBvr tipBackImg = solidColorImage(black)
      .crop(txtBB.getMin(),txtBB.getMax());
		
	ImageBvr tipImg = overlay(tipTxtImg,tipBackImg);

	// Create a detectable image (jumpImg)and crop it to 6 by 6 mm.
	// This image will be made pickable and used to jump to Seattle Sidewalk.
    ImageBvr jumpImg = solidColorImage(red).crop(point2(-0.003,-0.003),
      point2(0.003,0.003));

		jumpImg = jumpImg.transform(translate(toBvr(0),
			mul(toBvr(-50),pixelBvr)));

		// Make detectImg pickable.
    PickableImage jumpPImg = new PickableImage(jumpImg,true);

    // Create an event that will trigger when the mouse moves over jumpImg.
    DXMEvent jumpEv = jumpPImg.getPickEvent();

    // Create an event that will trigger when both the mouse moves over 
    // jumpImg, and the left mouse button is clicked.
    DXMEvent jumpClickEv = andEvent(leftButtonDown, jumpEv);

    // Get the image part of jumpPImg.
    ImageBvr jumpPointImg = jumpPImg.getImageBvr();

    // Combine the pickable images with the tip box and pair them
    // with sound.
    TupleBvr compTup1 = pairBvr(pickPImg.getImageBvr(),silence);
    ImageBvr finalImg = overlay(jumpPointImg,overlay(tipImg,pickImg));
    TupleBvr compTup2 = pairBvr(finalImg,pickSnd);

    // Create a behavior (pickTup) that start out as a detectable empty
    // image with no sound.  When the mouse moves over it, the sound
    // changes to pickSnd, and the tip box is displayed.  When the mouse is
    // no longer over it, it returns to the original behavior.
    TupleBvr pickedTup = TupleBvr.newUninitBvr(pairBvr(emptyImage,silence));
    pickedTup.init(until(compTup1,pickEv,until(compTup2,
      notEvent(pickEv),pickedTup)));

    // Create a behavior that starts out as pickedTup, and that jumps to the
    // Seattle Sidewalk site when the user clicks on pickPImg using the left
    // mouse button.
    TupleBvr finalTup = (TupleBvr)untilNotify(pickedTup,
      jumpClickEv,new JumpURLLink(urlStr));

    // Display the final image.
    setImage(overlay((ImageBvr)finalTup.nth(0),
      overlay(seattleImg,solidColorImage(black))));

    // Start the sound.
    setSound(mix((SoundBvr)finalTup.nth(1),silence));  
  }

  public void appLink(AppletContext appGet)  {
    _getAppContext = appGet;
  }
  public static AppletContext _getAppContext;
}

class JumpURLLink extends Statics implements UntilNotifier  {

  public JumpURLLink(StringBvr url)  {
		// Convert urlStr into an URL.
    try  {
      _clickDest = new URL((String) url.extract());
    }
    catch(MalformedURLException mal)  {
      System.out.println("Malformed URL: Check Applet Tag.");
    }
  }

  public Behavior notify(Object eventData,
    Behavior currentRunningBvr, BvrsToRun btr)  { 
    // Use the Java showDocument method to load the Seattle Sidewalk page
    // in the parent frame.
    JumpURLModel._getAppContext.showDocument(_clickDest,"_parent");

    // Return currentRunningBvr to keep the UntilNotify happy.
    return currentRunningBvr;
  }
  URL _clickDest;
}
// Thats all there is to it.  Happy animating...
// </Tutorial>