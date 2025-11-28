// <Tutorial Section=1.0 Title="Basic Applet">
/**
This applet illustrates the proper steps to construct and initialize
a simple DirectAnimation applet.
<BR>
**/
// </Tutorial>
//

// <Tutorial Section=1.1>
import com.ms.dxmedia.*;            // direct animation libraries.

// This class extends the DXMApplet class.  The model you set in this class,
// by calling the setModel() method, is the model that will be displayed.
public class BasicApplet extends DXMApplet {

  // The start() and stop() methods of DXMApplet restart and stop the
  // DirectX Media animation loop already, so you don't need to implement
  // these methods if you don't have other activities you need to stop and
  // restart.

  // Set the model in the init() method.
  public void init() {

    // Always call the super classes init first to ensure codeBase is set.
    super.init() ;

    // Now set the model.
    setModel(new BasicModel());
  }
}

// This class extends the Model class.  The createModel method in this class
// is where you construct your animation.
class BasicModel extends Model {

  // Create the animation in the createModel method.  In this example, we'll
  // just display a solid blue image.
  public void createModel(BvrsToRun blist)
  {
    // Set the image that actually gets displayed using setImage()
    setImage(solidColorImage(blue));
  }
}

// </Tutorial>
