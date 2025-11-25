// Copyright (c) 1997 Microsoft Corporation

package lhouse_module;

import com.ms.dxmedia.*;
import java.net.*;
import java_utility.*;

// <Tutorial Section=4.0 Title="Section4: Including 3D Geometries">
// Now we want to create a weather vane, sailboat, and seagull which will look
// like they are part of the 2D scene we created with our cellImages.  In this
// part of the tutorial, the process of creating the weather vane will be
// explained. The sailboat and seagull, although more complex, are created in
// similar fashion.
//
// Here we have a class called weatherVane with a static method called getGeo,
// which returns a GeometryBvr.  This kind of class can be refered to as a
// DirectAnimation module because it describes a set of media, in this case
// geometries, and behaviors built around that media, that form a more complex
// DirectAnimation behavior, in this case a GeometryBvr.
public class weatherVane extends Statics  {
  public static GeometryBvr getGeo(NumberBvr windAngle, 
    NumberBvr windDistance, URL importBase)  {

    // Here we import pieces of geometry such as scoops and arrows, which will
    // be used to construct the weather vane.
    GeometryBvr nsewGeo  = importGeometry(buildURL(importBase, "nsew.x"));
    GeometryBvr arrowGeo = importGeometry(buildURL(importBase, "arrow.x"));
    GeometryBvr scoopGeo = importGeometry(buildURL(importBase, "scoops.x"));

    // The arrows are going to rotate based on the wind angle.
    GeometryBvr arrow = arrowGeo.transform(rotate(yVector3, windAngle));

    // The scoops will spin based on the wind speed.
    GeometryBvr scoops =
      scoopGeo.transform(
        compose(rotate(yVector3, mul(toBvr(8),windDistance)), 
          scale3(toBvr(1.25))));
     
    // Combine these to form the complete weather vane (our first
    // DirectAnimation module).
    return union(arrow, union(scoops, union(nsewGeo, directionalLight)));
  }
}

// </Tutorial>
