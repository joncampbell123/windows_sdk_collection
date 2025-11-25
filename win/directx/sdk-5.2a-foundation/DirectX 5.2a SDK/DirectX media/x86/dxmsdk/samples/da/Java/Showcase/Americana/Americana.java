// Texture.java - The textured americana demo
// DirectX Media Java Demo 
//
// Copyright (c) 1997 Microsoft Corporation
import com.ms.dxmedia.*;
import java.awt.*;
import java.applet.*;
import java.net.*;
import java_utility.*;

class AmericanaModel extends Model {

    // Public vars.
  public AppTriggeredEvent toggleGeometryEvent;
  public AppTriggeredEvent toggleImageEvent;
  public AppTriggeredEvent spinGeometryEvent;
  public AppTriggeredEvent toggleBackgroundEvent;
  public AppTriggeredEvent toggleShapeEvent;

    ImageBvr _apple;
    ImageBvr _flag;
    ImageBvr _button;
    ImageBvr _background;
    Point2Bvr _buttonVec;
    SoundBvr _snd;
    SoundBvr _grabSnd;
    SoundBvr _releaseSnd;
        
    NumberBvr zero = toBvr(0);
    NumberBvr one = toBvr(1);
                
  public void createModel(BvrsToRun blst) {
      // Set up Events
      toggleGeometryEvent       = new AppTriggeredEvent();
      toggleImageEvent          = new AppTriggeredEvent();
      spinGeometryEvent         = new AppTriggeredEvent();
      toggleBackgroundEvent     = new AppTriggeredEvent();
      toggleShapeEvent          = new AppTriggeredEvent();

      String soundBase = "sound/";
      String imageBase = "image/";
      String geoBase = "geometry/";                

      URL ib = getImportBase();
      
      // import media
      _grabSnd = importSound(buildURL(ib, soundBase + "butin.wav") ,null);
      _releaseSnd = importSound(buildURL(ib, soundBase + "butnout.mp2"),null);
                
      _apple = importImage(buildURL(ib, imageBase + "apple.gif"));
      ImageBvr rawFlag = importImage(buildURL(ib, imageBase + "usaflag.gif"));
      _button = importImage(buildURL(ib, imageBase + "eyeball2.gif"));
      ImageBvr cherries = importImage(buildURL(ib, imageBase + "cherries.gif"));
      ImageBvr rosewood = importImage(buildURL(ib, imageBase + "rosewood.gif"));
      ImageBvr gumballs = importImage(buildURL(ib, imageBase + "gumballs.gif"));

      GeometryBvr cube  = importGeometry(buildURL(ib, geoBase + "cube.x"));
      GeometryBvr sphere = importGeometry(buildURL(ib, geoBase + "sphere.x"));
      GeometryBvr cone  = importGeometry(buildURL(ib, geoBase + "cone.x"));
      GeometryBvr cannon =
          (importGeometry(buildURL(ib, geoBase + "cylinder.x")));

      // Making the texture
                
      _flag = rawFlag.transform(scale2(add(mul(abs(sin(localTime)),
                                               toBvr(0.5)),
                                           toBvr(0.5))));

      ImageBvr IBvrs[]  = {cherries,rosewood,gumballs};
      Cycler cylImage   = new Cycler(IBvrs, toggleBackgroundEvent);
      _background = (ImageBvr)cylImage.getBvr();
      _snd = silence;

      ImageBvr compositeImage = overlay(CreateMasterImage(),_background);
                
      // Making the geometry
      GeometryBvr GBvrs[]    = {cube,sphere,cone,cannon};
      Cycler cylGeometry           = new Cycler(GBvrs, toggleShapeEvent);
      GeometryBvr origGeo    = (GeometryBvr)cylGeometry.getBvr();

      GeometryBvr g                = union(ambientLight, origGeo.texture(compositeImage.mapToUnitSquare()));
      GeometryBvr       positionedGeo = g.transform(scale3(2));

      NumberBvr moverVel = NumberBvr.newUninitBvr();
      moverVel.init(until(zero, spinGeometryEvent,
                       until(one, spinGeometryEvent, moverVel)));
      NumberBvr mover = integral(moverVel);

      Transform3Bvr rotationXf = rotate(vector3(sin(mover), cos(mover), sin(mul(toBvr(1.34),mover))), mul(mover,toBvr(0.32)));


      GeometryBvr scene         = positionedGeo.transform(compose(scale3(0.65),rotationXf));

      Transform3Bvr viewerXf    = translate(zero,zero,toBvr(0.2));

      // putting it all together
      GeometryBvr GBvrs2[]       = {scene, emptyGeometry};
      Cycler cylGeometry2                = new Cycler(GBvrs2, toggleGeometryEvent);
      GeometryBvr geoImgToShow = (GeometryBvr)cylGeometry2.getBvr();

      ImageBvr IBvrs2[] = {(ImageBvr) compositeImage.runOnce(), emptyImage};
      Cycler cylImage2           = new Cycler(IBvrs2, toggleImageEvent);
      ImageBvr imgToShow         = (ImageBvr)cylImage2.getBvr();

      ImageBvr leftImage         = imgToShow.transform(translate(mul(toBvr(-128),pixelBvr),zero));
      ImageBvr textureImage         = compositeImage.transform(translate(mul(toBvr(-128),pixelBvr),zero));
 
			// Camera
      CameraBvr  Camera = (perspectiveCamera(one,zero)).transform(translate(zero,zero,toBvr(100)));
                                                                                                                                                                                        
      // create the rightImage
      ImageBvr rightImage = (geoImgToShow.transform(translate(toBvr(4),zero,zero))).texture(textureImage.mapToUnitSquare()).render(Camera);

      setSound(_snd);
      
      setImage(overlay(rightImage,
                       overlay(leftImage,
                               solidColorImage(white))));
  }

    ImageBvr CreateMasterImage()  {
        NumberBvr corner = toBvr(0.01);
        NumberBvr negCorner = neg(corner);
        Transform2Bvr xf1       = translate(negCorner, negCorner);
        Transform2Bvr xf2   = translate(corner, negCorner);
        Transform2Bvr xf3   = translate(negCorner, corner);
        //Transform2Bvr xf4       = translate(corner,corner);

        ImageBvr imButton       = makeOne(_button,1);
        ImageBvr imApple        = makeOne(_apple,0);
        ImageBvr imFlag         = makeOne(_flag,0);


        Point2Bvr minPt         = _background.boundingBox().getMin();
        Point2Bvr maxPt         = _background.boundingBox().getMax();

        NumberBvr maxx          = maxPt.getX();
        NumberBvr maxy          = maxPt.getY();

        NumberBvr xFrac         = div(add(_buttonVec.getX(),maxx),mul(toBvr(2),maxx));
        NumberBvr yFrac         = div(add(_buttonVec.getY(),maxy),mul(toBvr(2),maxy));


        NumberBvr opacFactor= xFrac;
        NumberBvr magFactor = add(one,sub(yFrac, toBvr(0.5)));

        ImageBvr img        = overlay(imButton.transform(xf1),
                                          overlay((imApple.opacity(opacFactor)).transform(xf2),
                                                  (imFlag.transform(compose(xf3,scale(magFactor,one))))));

        return(img);                                                                                      
        
    }

    // Make a small square pickable image at the orgin that follows the
    // mouse when selected, and stops after that
    ImageBvr makeOne(ImageBvr img,int index) {

		  DraggableImage grabImg = new DraggableImage(img, origin2);

      SoundBvr gSnd = SoundBvr.newUninitBvr();
      SoundBvr rSnd = SoundBvr.newUninitBvr();
      gSnd.init(until(silence, grabImg.getGrabEvent(),
				until(_grabSnd, leftButtonUp, gSnd)));
			rSnd.init(until(silence, leftButtonUp,
				until(_releaseSnd, grabImg.getGrabEvent(), rSnd)));
			_snd = mix(mix(gSnd, rSnd), _snd);

      // wait for the first pick.
			if(index == 1)
				_buttonVec = grabImg.getPointBvr();

			return grabImg.getImageBvr();
    }
	public void cleanup() {
    super.cleanup();
	grabImg = null;
  }
  DraggableImage grabImg;
}

public class Americana extends DXMApplet {
  public Button toggleGeometryButton      = new Button("Toggle Geometry");
  public Button toggleImageButton         = new Button("Toggle Image");
  public Button cycleShapesButton         = new Button("Cycle Shapes");
  public Button cycleBackgoundButton      = new Button("Cycle Background");
  public Button spinGeometryButton        = new Button("Spin Geometry");

    AmericanaModel tt;

  public void init() {
      // add the buttons on the page
      add(toggleGeometryButton);
      add(toggleImageButton);
      add(cycleShapesButton);
      add(cycleBackgoundButton);
      add(spinGeometryButton);
      super.init();
  }

    public Americana() { setModel(tt = new AmericanaModel()); }
        
  public boolean action( Event evtObj, Object arg) {
        
      if(arg.equals("Toggle Geometry")) {
          tt.toggleGeometryEvent.trigger();
          return true;
      }
      if(arg.equals("Toggle Image")) {
          tt.toggleImageEvent.trigger();
          return true;
      } 
      if(arg.equals("Cycle Shapes")) {
          tt.toggleShapeEvent.trigger();
          return true;
      }
      if(arg.equals("Cycle Background")) {
          tt.toggleBackgroundEvent.trigger();
          return true;
      }
      if(arg.equals("Spin Geometry")) {
          tt.spinGeometryEvent.trigger();
          return true;
      }
      return false;
  }

}
