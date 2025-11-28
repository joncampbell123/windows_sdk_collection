// Copyright (c) 1997 Microsoft Corporation

package lhouse_module;

import com.ms.dxmedia.*;
import java.net.*;
import java_utility.*;

// <Tutorial Section=5.2>
// After implementing all the algorithmic descriptions for the sounds, 
// define the oceanSynth class.  The constructor for this class creates 
// periodicSounds and cyclingSounds based on those descriptors.  For more info
// on periodSound and cyclingSound examine their Java source code in the 
// Lighthouse/utility directory.
public class oceanSynth extends Statics  {
	
  public oceanSynth(NumberBvr roughness)  {
    _wave1 = new periodicSound(new wave1(roughness));
		_wave2 = new periodicSound(new wave2(roughness));
		_surf1 = new cyclingSound(new surf1(roughness));
		_surf2 = new cyclingSound(new surf2(roughness));
	}

// </Tutorial>

  // <Tutorial Section=5.3>
  // Now put the sound together.  First import the surf and wave components
  // (30K and 60K) each.  Then query the sound method for each algoSound, 
  // passing it the actual wav files to use.  This will return sound behaviors
  // that can be mixed together to give the final sounds.
  public SoundBvr getSound(URL importBase)  { 		
    SoundBvr wave = importSound(buildURL(importBase, "wave.mp2"),null);

    SoundBvr surf = importSound(buildURL(importBase, "surf.mp2"),null);

    return mix(_wave1.Sound(wave), mix(_wave2.Sound(wave), 
      mix(_surf1.Sound(surf), _surf2.Sound(surf))));
  }

  // </Tutorial>

  // <Tutorial Section=5.3>
  // Finally, you need to know when there is acrashing wave, so you can animate
  //it.  This will trigger a sound event when either wave1 or wave2 occur.
  public DXMEvent soundEvent()  { 
    return orEvent(_wave1.Trigger(), _wave2.Trigger()); 
  }

  // </Tutorial>

  private periodicSound _wave1,_wave2;
	private cyclingSound _surf1,_surf2;
}

class wave1 extends Statics implements algoFunc {
  private NumberBvr _roughness; 
  public wave1(NumberBvr roughness)  { 
    _roughness = roughness; 
  }
  public Behavior periodFunc()  {  // periodicity
    return add(toBvr(5),mul(toBvr(4),sub(toBvr(1),_roughness))); 
  }
  public Behavior gainFunc()  {  // gain
    return add(toBvr(0.7),mul(toBvr(0.3),_roughness)); 
  }	
  public Behavior rateFunc()  {  // rate
    return add(toBvr(.55),add(div(sub(toBvr(1),_roughness),
      toBvr(4)), mul(toBvr(.5), toBvr(Math.random())))); }
}

class wave2 extends Statics implements algoFunc  {
  private NumberBvr _roughness; 
  public wave2(NumberBvr roughness)  { 
    _roughness = roughness; 
  }
  public Behavior periodFunc()  { 
    return add(toBvr(6),mul(toBvr(4),sub(toBvr(1),_roughness))); 
  } 
  public Behavior gainFunc()  { 
    return add(toBvr(0.5), mul(toBvr(.5),_roughness)); 
  }
  public Behavior rateFunc()  { 
    return add(toBvr(.5),add(div(sub(toBvr(1),_roughness),
      toBvr(4)), mul(toBvr(.5), toBvr(Math.random()))));  
  }
}

// <Tutorial Section=5.1>
// The ocean module makes use of periodic and cycling sounds played by
// parametrically controlling the rate, gain, and periodicity.  By describing
// how these parameters vary in time with methods, relatively small sound
// bites can form the basis for rich dynamic sounds. The first implementation
// of the algorithmic interface defines the characteristics of the pounding
// surf.  Define the roughness (based on the wind speed), gain and rate.
class surf1 extends Statics implements algoFunc  {
  private NumberBvr _roughness; 
  public surf1(NumberBvr roughness)  { 
    _roughness = roughness; 
  }
  public Behavior periodFunc()  { 
    return toBvr(0); 
  } 
  public Behavior gainFunc()  { 
    return add(toBvr(0.45),mul(toBvr(.4),_roughness)); 
  }
  public Behavior rateFunc()  { 
    return add(toBvr(1), sub(toBvr(.5),_roughness)); 
  }
}

// </Tutorial>

class surf2 extends Statics implements algoFunc  {
  private NumberBvr _roughness; 
  public surf2(NumberBvr roughness)  { 
    _roughness = roughness; 
  }
  public Behavior periodFunc()  { 
    return toBvr(0); 
  } 
  public Behavior gainFunc()  { 
    return add(toBvr(0.4), mul(toBvr(.35),_roughness)); 
  }
  public Behavior rateFunc()  { 
    return add(toBvr(.8), sub(toBvr(.5),_roughness)); 
  }
}