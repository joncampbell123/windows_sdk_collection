// <Tutorial Section=1.0 Title="Extended Applet">
/**
This applet illustrates the construction of a DxM model including: <BR>
1) importing an image and a sound <BR>
2) using the height and width of the applet for scaling the image to fit
   properly <BR>
3) animating the image and looping the sound <BR>
4) setting the resulting image and sound to be presented <BR>
5) constructing a DxM applet including the proper steps for initialization
<BR>
**/
// </Tutorial>


// <Tutorial Section=1.1>
import com.ms.dxmedia.*;        // direct animation libraries
import java.awt.*;              // for getting the applet dimension
import java.net.*;              // for URLs

// This class extends the DXMApplet class.  The model you set in this class,
// by calling the setModel() method, is the model that will be displayed.
public class ExtendedApplet extends DXMApplet {

  // The start() and stop() methods of DXMApplet restart and stop the
  // DirectX Media animation loop already, so you don't need to implement
  // these methods if you don't have other activities you need to stop and
  // restart.

  // Set the model in the init() method.
  public void init() {

    // Always call the super classes init first to ensure codeBase is set.
    super.init() ;

    // Now set the model.
    setModel(new ExtendedModel(this));
  }
}

// This class extends the Model class.  The createModel method in this class
// is where you construct your animation.  This example will make use of
// two of the primary media types supported by DirectAnimation: image and sound.
class ExtendedModel extends Model {

  // Get the size of the applet in the constructor, and store it in the
  // member variables.
  ExtendedModel(DXMApplet dxma) {

    // Get the size of the applet.
   Dimension dim = dxma.getSize();

    // The base unit of measure for DirectAnimation is the meter.
    // Convert the size into meters by multiplying it with the pixelBvr.
    _halfWidthNum = mul(toBvr(dim.width*0.5),pixelBvr);
    _halfHeightNum = mul(toBvr(dim.height*0.5),pixelBvr);

    // Notice the use of the toBvr() call. This helper function is used
    // extensively in DirectAnimation to convert Java primitive data types to 
    // DxM behaviors. As you learn more about the system you will begin to appreciate the
    // appreciate the power of behaviors, which can be time-varying values.
  }

  // Create the animation in the createModel method.
  public void createModel(BvrsToRun blist)
  {
    // Build up a URL to import relative to.
    URL mediaBase = getImportBase();
    URL imgBase = buildURL(mediaBase, "image/");
    URL sndBase = buildURL(mediaBase, "sound/");

    // Create an image behavior by importing a bitmap.
    ImageBvr img = importImage(buildURL(imgBase, "phantom.jpg"));

    // Create a sound behavior by importing a wave file.
    SoundBvr snd = importSound(buildURL(sndBase, "earth.mp2"), null);

    // Find the length of the diagonal of the imported image, we'll use
    // it later to scale the image to fit in the viewport.
    // halfDiagonalNum = sqrt(xNum * xNum + yNum * yNum);
    Point2Bvr maxPt2 = img.boundingBox().getMax();
    NumberBvr xNum = maxPt2.getX();
    NumberBvr yNum = maxPt2.getY();
    NumberBvr halfDiagonalNum = sqrt(add(mul(xNum, xNum), mul(yNum, yNum)));

    // Find the smaller of the width and height.
    NumberBvr minHalfExtNum = (NumberBvr)cond(lt(_halfWidthNum, _halfHeightNum),
                                              _halfWidthNum, _halfHeightNum);

    // Create an image that fits in the viewport.
    ImageBvr scaledImg = img.transform(scale2(div(minHalfExtNum,
                                                  halfDiagonalNum)));

    // Create a rotating image
    ImageBvr rotatingImg = scaledImg.transform(rotate(localTime));

    // Set the image that actually gets displayed using setImage()
    setImage(overlay(rotatingImg, solidColorImage(black)));

    // Create a sound that loops continuously.
    SoundBvr loopingSnd = snd.loop();

    // And set the sound that gets played using setSound()
    setSound(loopingSnd);
  }

  private NumberBvr _halfWidthNum, _halfHeightNum;
}

// </Tutorial>
