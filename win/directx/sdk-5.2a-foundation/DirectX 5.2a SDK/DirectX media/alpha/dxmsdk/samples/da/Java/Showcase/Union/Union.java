
// Copyright (c) 1997 Microsoft Corporation

import com.ms.dxmedia.*;
import java.net.*;

class UnionModel extends Model {

  public static ImageBvr geometryImage(GeometryBvr geo) {
      CameraBvr cam =
          perspectiveCamera(toBvr(1),toBvr(0)).transform(translate(0,0,2));
      Transform2Bvr sc = scale2(0.1);
      
      return geo.render(cam).transform(sc);
  }

  public static ColorBvr myColor(double hue,
                                  double saturationRate) {
      NumberBvr sat = add(mul(sin(mul(localTime,
                                      toBvr(saturationRate))),
                              toBvr(0.5)),
                          toBvr(0.5));

      return colorHsl(toBvr(hue), sat, toBvr(0.5));
  }

  public void createModel(BvrsToRun blst) {

      // Make these relative
	    URL mediaBase = getImportBase();
      URL geoBase = buildURL(mediaBase,"geometry/");
      URL sndBase = buildURL(mediaBase,"sound/");
      
      GeometryBvr rawSphere = importGeometry(buildURL(geoBase,"sphere.x"));
      GeometryBvr rawCube = importGeometry(buildURL(geoBase,"cube.x"));
      GeometryBvr rawCylinder = importGeometry(buildURL(geoBase,"cylinder.x"));
      GeometryBvr rawCone = importGeometry(buildURL(geoBase,"cone.x"));
      // Import a sound, supply null as the second argument since we don't
      // want a length. 
      SoundBvr snd = importSound(buildURL(sndBase,"etherial.mp2"), null).loop();

      Transform3Bvr scaler = scale3(0.25);

      GeometryBvr sphere   = rawSphere.transform(scaler);
      GeometryBvr cube     = rawCube.transform(scaler);
      GeometryBvr cone     = rawCone.transform(scaler);
      GeometryBvr cylinder = rawCylinder.transform(scaler);

      GeometryBvr xcone    = cone.transform(rotate(xVector3,
                                                   toBvr(Math.PI)));

      ColorBvr col1 = myColor(0, 2);
      ColorBvr col2 = myColor(0.25, 3);
      ColorBvr col3 = myColor(0.5, 4);
      ColorBvr col4 = myColor(0.75, 1);

      // Shapes to start with (many with time varying colors)
      GeometryBvr shapes[] = { sphere.diffuseColor(col1),
                               sphere.diffuseColor(col1),
                               cone.diffuseColor(col2),
                               xcone.diffuseColor(col2),
                               cube.diffuseColor(col3),
                               cube.diffuseColor(col3),
                               cylinder.diffuseColor(col4) };

      // Places to translate them to.
      double tls[][] = { { 0.75, 0, 0 },
                         { -0.75, 0, 0 },
                         { 0, 0.75, 0 },
                         { 0, -0.75, 0 },
                         { 0, 0, 0.75 },
                         { 0, 0, -0.75 },
                         { 0, 0, 0 } };

      // Go ahead and accumulate up the translated geometry by cycling
      // through the arrays.
      GeometryBvr geo = emptyGeometry;
      for (int i = 0; i < shapes.length; i++) {
          Transform3Bvr xf = translate(tls[i][0],tls[i][1],tls[i][2]);
          geo = union(geo, shapes[i].transform(xf));
      }

      // Set the whole thing in motion
      Transform3Bvr xf =
          compose(rotate(xVector3, localTime),
            compose(rotate(zVector3, mul(localTime, toBvr(1.9))),
              compose(rotate(yVector3, mul(localTime, toBvr(Math.PI))),
                scale3(0.75))));

      geo = geo.transform(xf);

      // Render into an image, and overlay atop a solid image.
      ImageBvr geoIm = geometryImage(union(geo, directionalLight));

      ImageBvr model = overlay(geoIm, solidColorImage(white));
      
      setImage(model);
      setSound(snd);
  }
}

public class Union extends DXMApplet {
    public Union() { setModel(new UnionModel()) ; }
}



