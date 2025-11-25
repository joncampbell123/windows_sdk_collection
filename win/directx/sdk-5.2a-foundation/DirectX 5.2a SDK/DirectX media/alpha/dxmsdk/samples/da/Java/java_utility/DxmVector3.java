//
// DxmVector3: Vector3Bvr creation utilities
// Copyright (c) 1997, Microsoft Corporation
//
package java_utility;
import com.ms.dxmedia.*;

public class DxmVector3 extends Statics 
{       
    // figure8: creates a spline in the form of a figure8
  public static Vector3Bvr figure8() {
      Vector3Bvr[] points = {             
          vector3(toBvr(    1), toBvr(0), toBvr(    0)), 
          vector3(toBvr(    1), toBvr(0), toBvr(-0.25)),
          vector3(toBvr( 0.25), toBvr(0), toBvr(   -1)),
          vector3(toBvr(-0.25), toBvr(0), toBvr(    1)),
          vector3(toBvr(   -1), toBvr(0), toBvr(    0)),
          vector3(toBvr(-0.25), toBvr(0), toBvr(   -1)),
          vector3(toBvr( 0.25), toBvr(0), toBvr(    1)),
          vector3(toBvr(    1), toBvr(0), toBvr( 0.25)),
          vector3(toBvr(    1), toBvr(0), toBvr(    0))
      };

      NumberBvr[] knots = {             
          toBvr(0), toBvr(0), toBvr(0),
          toBvr(1), toBvr(2), toBvr(3), toBvr(4), toBvr(5),
          toBvr(6), toBvr(6), toBvr(6)
      };

      NumberBvr[] weights = {
          toBvr(1), toBvr(1), toBvr(1), 
          toBvr(1), toBvr(1), toBvr(1), 
          toBvr(1), toBvr(1), toBvr(1)
      };

      NumberBvr index = mod(div(localTime,toBvr(2)), toBvr(6));
      Vector3Bvr spline = bSpline(3, knots, points, weights, index);
      return spline;
  }
}
