//
// DxmNumber: NumberBvr creation utilities
// Copyright (c) 1997, Microsoft Corporation
//
package java_utility;
import com.ms.dxmedia.*;

public class DxmNumber extends Statics {

  // smooth0to1: constructs a number which goes from zero
  // to one over the period of seconds specified by rate
  public static NumberBvr smooth0to1(NumberBvr rate) {
    NumberBvr val = (NumberBvr)cond(lt(div(localTime, rate), toBvr(1)),
                                    div(localTime,rate),
                                    toBvr(1));
    return val;
  }

  // clamp: clamps a number behavior between a minimum and maximum range
  public static NumberBvr clamp(NumberBvr val, NumberBvr minRange, NumberBvr maxRange) {
    return (NumberBvr)cond(gte(val,maxRange), maxRange,
                           cond(lte(val,minRange), minRange, val));
  }   

  // maxNumBvr: returns max(x, y)
  public static NumberBvr max(NumberBvr x, NumberBvr y) {
    return (NumberBvr) cond(gt(y, x), y, x);
  }

  // minNumBvr: returns min(x, y)
  public static NumberBvr min(NumberBvr x, NumberBvr y) {
    return (NumberBvr) cond(lt(y, x), y, x);
  }
}
