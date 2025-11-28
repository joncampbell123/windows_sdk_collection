// <Tutorial Section=1.0 Title="Using AFC Menus">
//
/** This sample shows how to use DirectAnimation with the Applet Foundation<BR>
Classes (AFC). The menu is created by using AFC methods.<BR>
The animation of the image (scaling, translating, rotating) is 
done in DirectAnimation.<BR>

It illustrates the following:<BR><BR>
- using DirectAnimation along with AFC.<BR>
- creating menus and buttons in AFC.<BR>
- The use of ModifiableBehaviors.<BR>
**/


import com.ms.dxmedia.*; // All DirectAnimation classes
import com.ms.ui.*;      // The AFC classes
import java.awt.*;       // The event handle classes
import java.net.URL;     // The Java URL classes

// Use AFC's AwtUIApplet to construct the applet.
public class AFCMenus extends AwtUIApplet  {
  public void init()  {
    setLayout(new FlowLayout(FlowLayout.LEFT,5,5));

    // Get the AFC (menu and button) part of the Applet.
    UIPanel ctrls = new AFCMenuControls();

    // Get the DirectAnimation part of the Applet.
    Canvas cv = new DXMImage();

    // Position both parts.
    add("North",ctrls);
    add("South",cv);

    // The resize the DirectAnimation part, otherwise it will take over the
    // entire viewport.
    cv.setSize(getSize().width - 25,
               getSize().height - 25);
    setBackground(Color.white);
  }
}

// The menu gets created in the AFCMenuControls Class.
class AFCMenuControls extends UIPanel  {

  // Create the menu by using AFC's UIMenuList and UIMenuButton 
  // classes.
  AFCMenuControls()  {

    setLayout(new UIGridLayout(1, 1));
    UIMenuList xformMenu = new UIMenuList();
    UIMenuButton xformButton = new UIMenuButton("Apply Transform", 
    UIPushButton.RAISED, xformMenu);
    add(xformButton);

    // Create the "Scale", "Translate", and "Rotate" menu items.
    _scale = new UIMenuItem("Scale");
    _translate = new UIMenuItem("Translate");
    _rotate = new UIMenuItem("Rotate");

    // Place the above mentioned items on the menu.
    xformMenu.add(_scale);
    xformMenu.add(_translate);
    xformMenu.add(_rotate);
  }

  // Handle relevant events when the user interacts with the menu.
  public boolean action(Event e, Object arg)  {
    if (arg == _scale)  {
      ImageModel.setXform(ImageModel.SCALE);
      return true;
    } else if (arg == _translate)  {
      ImageModel.setXform(ImageModel.TRANSLATE);
      return true;
    } else if (arg == _rotate)  {
      ImageModel.setXform(ImageModel.ROTATE);
      return true;
    }
    return false;
  }
  UIMenuItem _scale, _translate, _rotate;
}

class DXMImage extends DXMCanvas  {
  DXMImage()  {
    setModel(new ImageModel());
  }
}

// In the ImageModel class the createModel method is where you construct 
// your animation.
class ImageModel extends Model  {
  public void createModel(BvrsToRun bvrs)  {

    // Import the cube.
    URL mediaBase = getImportBase();
    URL imgBase = buildURL(mediaBase, "image/");
        ImageBvr img = importImage(buildURL(imgBase, "pretzel.gif"));

    // Create a ModifiableBehavior (_xf) which will be linked to the
    // menu item the user selects.  This behavior will then be used to
    // transform the image (img).
    _xf = new ModifiableBehavior(identityTransform2);

    // Apply _xf to img.
    img = img.transform((Transform2Bvr)_xf.getBvr());

    // Define the three transformations, that will be mapped to the respective
    // menu items.
    _scaleXf = scale2(add(toBvr(1), mul(sin(localTime), toBvr(0.5))));

    _translateXf = translate(mul(sin(localTime), 
                             mul(toBvr(100),pixelBvr)), toBvr(0));

    _rotateXf = rotate(localTime);

    // Display the image on a white background.
    setImage(overlay(img, solidColorImage(white)));
  }

  // This method is used in the AFCMenuControls class to change _xf.
  static void setXform(int xformType)  {
    switch (xformType)  {
    case SCALE:
      _xf.switchTo(_scaleXf);
      break;
    case TRANSLATE:
      _xf.switchTo(_translateXf);
      break;
    case ROTATE:
    default:
      _xf.switchTo(_rotateXf);
      break;
    }
  }

  static Transform2Bvr _scaleXf, _translateXf, _rotateXf;
  static ModifiableBehavior _xf;
  final static int SCALE = 1;
  final static int TRANSLATE = 2;
  final static int ROTATE = 3;
}

// That's all there is to it.  Happy animating...
// </Tutorial>
