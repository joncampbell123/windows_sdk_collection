// Copyright (c) 1997 Microsoft Corporation


package encarta_module;

import com.ms.dxmedia.*;
import java.net.*;
import java_utility.*;

public class parrotSynth extends Statics {
	public static SoundBvr getSound(URL importBase) {		
		SoundBvr birdSnd	 = importSound(buildURL(importBase, "jbird1.mp2"),null);
		SoundBvr contBirdSnd = birdSnd.loop();

		periodicSound parrot1 = new periodicSound(new parrot1());
		periodicSound parrot2 = new periodicSound(new parrot2());
		periodicSound parrot3 = new periodicSound(new parrot2());

		return mix(mix( parrot1.Sound(birdSnd).pan(-0.2),
						parrot2.Sound(birdSnd).pan(0.2) ),
						parrot3.Sound(birdSnd).pan(0.2) );

	}
}

class parrot1 implements algoFunc {
	public Behavior periodFunc() {
		return Statics.toBvr(5+3*Math.random()); 
	} 
	public Behavior gainFunc() { 
		return Statics.toBvr(.6+.3*Math.random()); 
	}
	public Behavior rateFunc() { 
		return Statics.toBvr(.8+.1*Math.random()); 
	}
}

class parrot2 implements algoFunc {
	public Behavior periodFunc() { 
		return Statics.toBvr(4+3*Math.random());
	} 
	public Behavior gainFunc() { 
		return Statics.toBvr(.6+0.3*Math.random());
	}
	public Behavior rateFunc() { 
		return Statics.toBvr(.8+.1*Math.random()); }
}

