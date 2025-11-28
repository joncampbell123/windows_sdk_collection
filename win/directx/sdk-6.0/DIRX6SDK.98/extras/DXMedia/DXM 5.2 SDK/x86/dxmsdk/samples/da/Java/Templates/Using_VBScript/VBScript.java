// <Tutorial Section=1.0 Title="Using VBScript with DirectAnimation">
// 
/**Java applets can be driven just like ActiveX controls. The script can<BR>
read and write the public variables of the applet, as well as call its<BR>
public methods (including passing parameters and reading return values).

Note that only the Applet-derived class is directly accessible from<BR>
the scripting language. If your Java applet includes other classes that<BR>
you want to be available to the scripting language, you must define<BR>
public methods in your Applet-derived class that delegate to those<BR>
classes. Note, in order to communicate from the applet back to the script,<BR>
you will need to make use of the Dispatch methods in the Java SDK.<BR>
Please refer to the java SDK com.ms.com.Dispatch documentation.<BR>

**/

// </Tutorial>
import com.ms.dxmedia.*;
import java.net.*;        // For using URLs

// <Tutorial Section=3.0 Title="Changing the Color of the Sphere">
// Here in the VBScript class, you see the R,G, and B data members. The 
// updateColor method just passes the data along to the VBScriptModel model 
// class, where it will be used to change the color of the sphere.
public class VBScript extends DXMApplet {
  private VBScriptModel _Model;
  VBScript() { 
    _Model = new VBScriptModel();
    setModel(_Model); 
  }
  // Public method accessable to VBScript subroutines in Using_VBScript.html
  public void updateColor() { 
    _Model.updateColor(R,G,B); 
  }
  // Public method accessable to VBScript subroutines in Using_VBScript.html
  public double R,G,B;
}
// </Tutorial>

// <Tutorial Section=3.2>
// Set up a simple model consisting of a sphere whose color can be switched 
// upon notification.
class VBScriptModel extends Model {

  public void createModel(BvrsToRun blist)	
  {
    URL geoBase = buildURL(getImportBase(), "geometry/");

    // Create the sphere's initial color.
    ColorBvr initClr = colorRgb(1,1,1);

		// Create a switcher behavior that will change whenever the scrollbars
    // update the color.
    sphereClrSw = new ModifiableBehavior(initClr);

    // Create a sphere and apply sphereClrSw's behavior to it
    GeometryBvr sphereGeo = 
	    importGeometry(buildURL(geoBase,"sphere.x"));	
    GeometryBvr coloredSphereGeo =
	    sphereGeo.diffuseColor((ColorBvr)sphereClrSw.getBvr());

		// Create a camera and light to render sphere.
    CameraBvr  Cam = 
      perspectiveCamera(toBvr(1),toBvr(0)).transform(			    
		    translate(0,0,50) );						
    GeometryBvr lightGeo = 
	    directionalLight.transform(
		    translate(0,0,4));

		// Display the color changing sphere.
    setImage(overlay(
      union(coloredSphereGeo, lightGeo).render(Cam),
	    solidColorImage(white)));
  }
  // </Tutorial>

  // <Tutorial Section=3.1>
  // The VBScript class accesses this method to update the color of the sphere.
  // This is done by switching sphereClrSw to a color based on the R,G, and B
  // values obtained from the scrollbars.
  public void updateColor(double R, double G, double B) {
    ColorBvr newClr = colorRgb(toBvr(R),toBvr(G),toBvr(B));
    sphereClrSw.switchTo(newClr);
  }
  // </Tutorial>

  private ModifiableBehavior sphereClrSw;
}

