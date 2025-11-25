// Pick3.java - Demonstrates 3D picking
// DirectX Media Java Demo 
//
// Copyright (c) 1997 Microsoft Corporation

import com.ms.dxmedia.*;
import java.net.*; //added to support URL's

class Pick3Test extends Model {

    NumberBvr speed = toBvr(0.07);

  public void createModel(BvrsToRun blst) {
      Transform3Bvr size  = scale3(0.25);

    URL mediaBase = getImportBase();
    URL geoBase = buildURL(mediaBase,"geometry/");
    URL imgBase = buildURL(mediaBase,"image/");

      // import the Geometry
      GeometryBvr rawCube     =
          importGeometry(buildURL(geoBase,"cube.x")).transform(size);
			GeometryBvr rawCylinder =
          importGeometry(buildURL(geoBase,"cylinder.x")).transform(size);
			GeometryBvr rawCone = 
				  importGeometry(buildURL(geoBase,"cone.x")).transform(size);

      // import images
      ImageBvr stillSky = importImage(buildURL(imgBase,"cldtile.jpg"));

      // activate the Geometry (make it pickable)
      GeometryBvr cone1 = activate(rawCone,green);
      GeometryBvr cube1 = activate(rawCube,magenta);
      GeometryBvr cube2 = activate(rawCube,colorHsl(div(localTime,toBvr(8)),
                                                    toBvr(1),
                                                    toBvr(0.5)));
      GeometryBvr cylinder = activate(rawCylinder,colorRgb(0.8,
                                                           0.4,
                                                           0.3));

      GeometryBvr multigeo =
          union(cone1.transform(translate(0,1,0)),
                union(cube1.transform(translate(0,0,1)),
                      union(cube2.transform(translate(0,0,-1)),
                            cylinder)));

      GeometryBvr geo =
          multigeo.transform(scale(add(abs(sin(mul(localTime,toBvr(0.2)))),toBvr(0.5)),
                                   add(abs(sin(mul(localTime,toBvr(0.26)))),toBvr(0.5)),
                                   add(abs(sin(mul(localTime,toBvr(0.14)))),toBvr(0.5))));
      
      Point2Bvr maxSky         = stillSky.boundingBox().getMax();

      ImageBvr tiledSky        = stillSky.tile();
      ImageBvr movingSky       =
          tiledSky.transform(translate(mul(localTime,div(maxSky.getX(),toBvr(8))),
                                       mul(localTime,div(maxSky.getX(),toBvr(16)))));

      ImageBvr movingGeoImg=
          geometryImage(geo.transform(compose(rotate(zVector3,mul(speed,mul(localTime,toBvr(1.9)))),
                                              rotate(yVector3,mul(speed,mul(localTime,toBvr(Math.PI)))))));

      FontStyleBvr fs = defaultFont.color(black);
      
      ImageBvr titleIm =
          stringImage(toBvr("Left Click On An Object"), fs)
             .transform(translate(0,0.025));
                
      // set the Image
      setImage(overlay(titleIm,overlay(movingGeoImg,movingSky)));
  }

    GeometryBvr activate(GeometryBvr unpickedGeo, ColorBvr col) {

        PickableGeometry pickGeo = new PickableGeometry(unpickedGeo);

        DXMEvent pickEvent =
            andEvent(leftButtonDown, pickGeo.getPickEvent());

        NumberBvr numcyc = NumberBvr.newUninitBvr();

        numcyc.init(until(toBvr(0), pickEvent,
                          until(toBvr(1), pickEvent, numcyc)));

        ColorBvr colcyc = ColorBvr.newUninitBvr();

        colcyc.init(until(white, pickEvent,
                          until(col, pickEvent, colcyc)));
                
        Transform3Bvr xf  = rotate(xVector3,integral(numcyc));

        return(pickGeo.getGeometryBvr().diffuseColor(colcyc).transform(xf));
    }

    ImageBvr geometryImage(GeometryBvr geo) {
                        
        NumberBvr scaleFactor   = toBvr(0.04);

        GeometryBvr light =
            union(directionalLight.transform(rotate(xVector3, div(toBvr(Math.PI),toBvr(2)))),
                  directionalLight);

        // allow change of transform3Bvr with rightMouseDown
        //Transform3Bvr perspTransform = Transform3Bvr.newUninitBvr();

        NumberBvr zero = toBvr(0);
        NumberBvr one = toBvr(1);

        //perspTransform.init(until(translate(0, 0, 0.2),
        //                          rightButtonDown,
        //                          until(compose(translate(0,0,0),
         //                                       scale(1,1,1e8)),
         //                               rightButtonDown, perspTransform)));
        
        // allow change of stringBvr with rightMouseDown
        StringBvr strcyl = StringBvr.newUninitBvr();
        
        strcyl.init(until(toBvr("Perspective - Right Click to Switch"),
                          rightButtonDown,
                          until(toBvr("Parallel - Right Click to Switch"),
                                rightButtonDown, strcyl)));
        
        // create the camera
        CameraBvr  perspectiveCam = (perspectiveCamera(one,zero)).transform(
					compose(rotate(xVector3,mul(speed,localTime)),translate(0, 0, 0.2)));

        CameraBvr  parallelCam = (parallelCamera(one)).transform(
					rotate(xVector3,mul(speed,localTime)));

				CameraBvr camera = CameraBvr.newUninitBvr();
				camera.init(until(perspectiveCam,rightButtonDown,until(parallelCam,
          rightButtonDown,camera)));

        FontStyleBvr fs = defaultFont.color(red);
        ImageBvr txtIm  = stringImage(strcyl, fs);
        ImageBvr xltTxt = txtIm.transform(translate(0, -0.025));

        return(overlay(xltTxt,union(geo.transform(scale3(scaleFactor)),
                                    light).render(camera)));                
    }
}
                                        
public class Pick3 extends DXMApplet {
        public Pick3() { setModel(new Pick3Test()); }
}
                                                         


