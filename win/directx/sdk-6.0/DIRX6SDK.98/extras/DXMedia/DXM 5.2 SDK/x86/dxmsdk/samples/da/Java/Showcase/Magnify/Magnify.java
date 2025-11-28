// Magnify.java
// DirectAnimation Java Demo 
//
// Copyright (c) 1997 Microsoft Corporation

import com.ms.dxmedia.*;
import java.awt.*;
import java.net.*; //added to support URL's

class MagnifyTest extends Model implements UntilNotifier 
{
    GeometryBvr     cylinder;
    GeometryBvr     cube;
    GeometryBvr     cone;
    ImageBvr cloudImage;
    ImageBvr waterImage;
    CameraBvr theCamera;
    
    // string pairs
    StringBvr actionsTxt[] = {toBvr("DirectAnimation"),
                            toBvr("Left Click"),
                            toBvr("Right Click"),
                            toBvr("Mouse Moves"),
                            toBvr("Space Bar"),
                            toBvr("Up Arrow"),
                            toBvr("Down Arrow"),
                            toBvr("Right Arrow"),
                            toBvr("Left Arrow")};


    StringBvr resultsTxt[] = {toBvr("Magnifier"),
                            toBvr("To Speed Up"),
                            toBvr("To Slow Down"),
                            toBvr("Minifies"),
                            toBvr("Textures"),
                            toBvr("Enlarges Window"),
                            toBvr("Shrinks Window"),
                            toBvr("Magnifies"),
                            toBvr("Minifies")};
         
    ArrayBvr actionsArray = new ArrayBvr(actionsTxt);
    ArrayBvr resultArray  = new ArrayBvr(resultsTxt);
    NumberBvr speedNumber;
    DXMEvent  changeSpeedEvent;

  public void createModel(BvrsToRun blst) {

    URL mediaBase = getImportBase();
    URL imgBase = buildURL(mediaBase,"image/");
    URL geoBase = buildURL(mediaBase,"geometry/");
    URL sndBase = buildURL(mediaBase,"sound/");

        // import sound  
        SoundBvr switchSound    =
            importSound(buildURL(sndBase,"etherial.mp2"),null).loop();

        // import geometry 
        GeometryBvr sphere      =
            importGeometry(buildURL(geoBase,"sphere.x"));

        cube = importGeometry(buildURL(geoBase,"cube.x"));
			  cylinder = 
          importGeometry(buildURL(geoBase,"cylinder.x"));
			  cone = 
          importGeometry(buildURL(geoBase,"cone.x"));

        // import images
        cloudImage              =
            importImage(buildURL(imgBase,"lake.jpg"));
				//cloudImage = cloudImage.transform(scale2(toBvr(1.25)));
        waterImage              =
            importImage(buildURL(imgBase,"mshingle.jpg"));
        ImageBvr starsImage     =
            importImage(buildURL(imgBase,"stars.jpg"));

        ImageBvr lightningImage =
            importImage(buildURL(imgBase,"metalic.jpg")).tile();

        // Crop the lightning image to the same size as the applet.
        // We get these pixel values from the HTML page.
        NumberBvr halfX = mul(toBvr(386/2),pixelBvr);
        NumberBvr halfY = mul(toBvr(256/2),pixelBvr);
        lightningImage = lightningImage.crop(point2(neg(halfX),neg(halfY)),
                                             point2(halfX,halfY));
                
                
        // time varying transforms 
        Transform3Bvr funkyXf1 =
            compose(scale(add(div(abs(sin(div(localTime,toBvr(5)))), toBvr(2)),toBvr(0.3)),
                          add(div(abs(sin(div(localTime,toBvr(7)))), toBvr(2)),toBvr(0.3)),
                          add(div(abs(sin(div(localTime,toBvr(9)))), toBvr(2)),toBvr(0.3))),
                    compose(rotate(yVector3,div(localTime,toBvr(4))),
                            rotate(zVector3,div(localTime,toBvr(7)))));

        Transform3Bvr funkyXf2 =
            compose(scale(add(abs(sin(add(div(localTime,toBvr(9)),div(toBvr(Math.PI),toBvr(3))))),toBvr(0.3)),
                          add(abs(sin(add(div(localTime,toBvr(11)),div(toBvr(Math.PI),toBvr(2))))),toBvr(0.3)),
                          add(abs(sin(div(localTime,toBvr(17)))),toBvr(0.3))),
                    compose(rotate(xVector3, div(localTime,toBvr(8))),
                            compose(rotate(yVector3, div(localTime, toBvr(9))),
                                    rotate(zVector3, div(localTime, toBvr(11))))));

        // create the needed camera
        theCamera = parallelCamera(toBvr(1)).
            transform(compose(translate(toBvr(0),toBvr(0),toBvr(0.1)),
                              scale(toBvr(1),toBvr(1),toBvr(1e8))));


        DXMEvent evLeftButton  = leftButtonDown.attachData(new Integer(1));
        DXMEvent evRightButton = rightButtonDown.attachData(new Integer(-1));
        changeSpeedEvent       = orEvent(evLeftButton,evRightButton);
        speedNumber            = (NumberBvr) untilNotify(toBvr(-1),changeSpeedEvent, this);
                
        // Sound
        SoundBvr snd1 = switchSound.rate(switchTime());
        SoundBvr snd  = mix(snd1.pan(toBvr(-1.0)),
                            (snd1.rate(toBvr(1.1))).pan(toBvr(1.0)));

        // time varing color
        ColorBvr col = colorHsl(div(localTime,toBvr(30)),
                                add(mod(div(localTime,toBvr(20)),toBvr(0.5)),toBvr(0.5)),
                                toBvr(0.5));

        // ... and its RGB complement
        ColorBvr invCol    = colorRgb(sub(toBvr(1),col.getRed()),
                                      sub(toBvr(1),col.getGreen()),
                                      sub(toBvr(1),col.getBlue()));

        ImageBvr textImage = textStr(col,invCol);

        // Initial "source" image is the text over the geometry over the
        // background.  "Start" all the images so they don't reset when
        // switched to.
        Point2Bvr lightnngMin = (lightningImage.boundingBox()).getMin();
        Point2Bvr lightnngMax = (lightningImage.boundingBox()).getMax();

        ImageBvr sourceImg1   = overlay(textImage,
                                        overlay(geometryImage(invCol, switchTime()),
                                                lightningImage));

        // next source image is the initial textured onto an object, then rendered
        Point2Bvr cloudMin        = (cloudImage.boundingBox()).getMin();
        Point2Bvr cloudMax        = (cloudImage.boundingBox()).getMax();

        ImageBvr sourceImg2   = textureIt(sourceImg1.crop(lightnngMin,lightnngMax),
                                          compose(scale3(toBvr(0.05)),funkyXf1),
                                          sphere,
                                          overlay(cloudImage, solidColorImage(white)));

        ImageBvr sourceImg3       = textureIt(sourceImg2.crop(cloudMin,cloudMax),
                                              compose(scale3(toBvr(0.025)),funkyXf2),
                                              cube,
                                              overlay(starsImage, solidColorImage(white)));


        // Source to really uses swaps between these three
        // set up the Cycle behavior
        ImageBvr behaviors[] = {sourceImg1,sourceImg2,sourceImg3};
        Cycler  cyl          = new Cycler(behaviors,keyDown(32) );
        ImageBvr sourceImage = (ImageBvr)cyl.getBvr();

        // Come up with magnification ranges and factors based on keystokes
        NumberBvr magnifyRange     = add(toBvr(0.4),sub(keyIntegral(Event.UP,toBvr(0.1)),keyIntegral(Event.DOWN,toBvr(0.1))));
        NumberBvr magnifyFactor    = add(toBvr(2.0),sub(keyIntegral(Event.RIGHT,toBvr(0.5)),keyIntegral(Event.LEFT,toBvr(0.5))));


        // Magnifying image relies on magIM func applies to original image and
        // reactive parameters to change magnification attributes.
        ImageBvr magnifyingImage = magIm(sourceImage, magnifyFactor, magnifyRange);

        // Total model is the magnifying glass over the image being magnified,
        // and the same sound on each channel, with one slightly faster than
        // the other, just for kicks
        setSound(snd); 

        setImage(overlay(magnifyingImage,overlay(sourceImage,solidColorImage(white))));
                
    }       
	public void cleanup() {
      super.cleanup();
      speedNumber=null;
	}


    NumberBvr keyIntegral(int key, NumberBvr amtPerSec)
    {
        NumberBvr change = (NumberBvr) cond(keyState(key),amtPerSec,toBvr(0));
        return(integral(change));
    }


    // Speed
    NumberBvr switchTime()
    {       
        return(pow(toBvr(2),speedNumber));
    }

    // Take a str pair and two colors, and render an image consisting
    // of one over the other
    ImageBvr textStr(ColorBvr col1, ColorBvr col2)
    {
			NumberBvr index         = mod(localTime,toBvr(resultsTxt.length));
      FontStyleBvr fs1 = defaultFont.color(col1).bold();
      FontStyleBvr fs2 = defaultFont.color(col2).bold();
      
      ImageBvr im1 = stringImage((StringBvr)actionsArray.nth(index), fs1);
      ImageBvr im2 = stringImage((StringBvr)resultArray.nth(index), fs2);
        
      Point2Bvr ext1          = (im1.boundingBox()).getMax();
      ImageBvr composite = overlay(im1,im2.transform(translate(toBvr(0),mul(neg(ext1.getY()),toBvr(2)))));

        return(composite.transform(scale2(2.0)));
    }

    // Build up the geometry to be displayed
    ImageBvr geometryImage(ColorBvr col, NumberBvr velocity)
    {
                
        // Raw geometries
        GeometryBvr rg1 = cylinder.texture(waterImage.mapToUnitSquare());
        GeometryBvr rg2 = cube.texture(cloudImage.mapToUnitSquare());
        GeometryBvr rg3 = cone.diffuseColor(col);

        // Use integral to determine angle since the velocity may be
        // changing and we want to avoid jumps and jerks.
        NumberBvr angle = integral(velocity);

        // Arrange along the circle, with different rotational characteristics
        GeometryBvr g1 = make(angle,                            div(angle,toBvr(1.15)), 
                              div(angle,toBvr(1.3)),div(toBvr(Math.PI),toBvr(2)), rg1);
        GeometryBvr g2 = make(div(angle,toBvr(2)),  div(angle,toBvr(1.8)), 
                              angle,                            add(div(toBvr(Math.PI),toBvr(2)), mul(toBvr(2),div(toBvr(Math.PI),toBvr(3)))), rg2);
        GeometryBvr g3 = make(angle,                        div(angle,toBvr(3.2)), 
                              div(angle,toBvr(4)),  add(div(toBvr(Math.PI),toBvr(2)), mul(toBvr(4),div(toBvr(Math.PI),toBvr(3)))), rg3);
                
        // ... composed into a single result and set into a circular motion
        GeometryBvr circlingGeo = (union(g1,union(g2,g3))).transform(rotate(zVector3, div(localTime,toBvr(10))));

        // return the resultant image, also thowing in a light
        return( (union(circlingGeo,directionalLight)).render(theCamera));
 
    }

    // takes x,y,z angles, rotation around the containing circle, and an underlying geo, and 
    // the positions the geo accordingly
    GeometryBvr make(NumberBvr xAng, NumberBvr yAng, NumberBvr zAng, NumberBvr rot, GeometryBvr geo)
    {
        NumberBvr radius = toBvr(0.03);
        NumberBvr xXlt   = mul(cos(rot),radius);
        NumberBvr yXlt   = mul(sin(rot),radius);

        return(geo.transform(compose(translate(xXlt,yXlt,toBvr(0)),
                                     compose(rotate(xVector3, xAng),
                                             compose(rotate(yVector3, yAng),
                                                     compose(rotate(zVector3, zAng),
                                                             scale3(toBvr(0.01))))))));
    }

    // helper function to take an image, texture it onto a transforming geo, render
    // the results, and put it over a background
    ImageBvr textureIt( ImageBvr orig, Transform3Bvr xf, GeometryBvr textureOnto, ImageBvr background)
    {
        GeometryBvr texturedGeo =union((textureOnto.texture(orig.mapToUnitSquare())).transform(xf),
                                       directionalLight);

        return(overlay(texturedGeo.render(theCamera), background));
    }

    // now create a magnifying image by taking an image, a magnification
    // factor, and a range of coverage...
    ImageBvr magIm(ImageBvr im, NumberBvr magFac, NumberBvr rangePct)
    {


        // Always follow the mouse, and determine extent based on the range percentage
        Vector2Bvr extent    = sub(point2(0.05,0.05),origin2).mul(toBvr(0.7));
        Vector2Bvr magExtent = extent.mul(rangePct);

        // Create a orange-ish background to put the mag image over...this gives
        // the appearance of a border
        Vector2Bvr bkgndExtent = magExtent.mul(toBvr(1.1));
        ImageBvr bkgnd             = solidColorImage(colorRgb(0.8,0.5,0.2)).crop(sub(mousePosition, bkgndExtent),
                                                                                                      add(mousePosition, bkgndExtent));

        // Create magnified image by scaling, cropping, and placing over "border" background
        return(overlay(scaleAroundPoint(im,mousePosition, magFac).crop(sub(mousePosition,magExtent),add(mousePosition,magExtent)),
                       bkgnd));
                                          
    }

    // scales about a particular point
    ImageBvr scaleAroundPoint(ImageBvr im, Point2Bvr center, NumberBvr magFac)
    {
        return(im.transform(compose(translate(sub(center,origin2)),
                                    compose(scale2(magFac),
                                            translate(sub(origin2,center))))));
    }

  // Handle the change the speed, via the mouse buttons
  public Behavior notify(Object eventData,
                         Behavior currentRunningBvr,
                         BvrsToRun blst) {

      NumberBvr currentNum = (NumberBvr)currentRunningBvr;
      int increment            = ((Integer)eventData).intValue();     
      NumberBvr newValue   = add(currentNum,toBvr(increment));
      return untilNotify(newValue,changeSpeedEvent,this);
  }
  
}

                
public class Magnify extends DXMApplet {
        public Magnify() { setModel(new MagnifyTest()); }
}
