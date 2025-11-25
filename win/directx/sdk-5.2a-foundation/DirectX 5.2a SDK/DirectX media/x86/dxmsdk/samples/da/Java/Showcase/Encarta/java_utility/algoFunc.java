//
// algoFunc: Interface for specifying algorithmic sound properties
// Copyright (c) 1997, Microsoft Corporation
//
// The following interface is used by periodic sound and cycling sound
// to create rich and interesting sounds using native DirectMedia sound 
// behaviors: gain, loop, restart, and frequency.  
// 
package java_utility;
import com.ms.dxmedia.*;

public interface algoFunc  {
  public Behavior periodFunc(); // not used for cyclingSound
	public Behavior gainFunc();
	public Behavior rateFunc();
}