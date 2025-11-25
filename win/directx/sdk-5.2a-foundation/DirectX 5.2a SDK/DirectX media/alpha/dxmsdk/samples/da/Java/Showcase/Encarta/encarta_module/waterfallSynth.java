// Copyright (c) 1997 Microsoft Corporation


package encarta_module;

import com.ms.dxmedia.*;
import java.net.*;
import java_utility.*;

public class waterfallSynth extends Statics {
	public static SoundBvr getSound(URL importBase) {			
		SoundBvr waterSnd = importSound(buildURL(importBase, "wfall1.mp2"),null);
		cyclingSound  falls1 = new cyclingSound(new falls1());
		cyclingSound  falls2 = new cyclingSound(new falls2());

		return mix(falls1.Sound(waterSnd).pan(-0.5),
				   falls2.Sound(waterSnd).pan(0.5) );
	}
}

class falls1 implements algoFunc {
	public Behavior periodFunc() { return Statics.toBvr(0.1);} // not used
	public Behavior gainFunc() { return Statics.toBvr(0.7); }
	public Behavior rateFunc() { return Statics.toBvr(1.2); }
}

class falls2 implements algoFunc {
	public Behavior periodFunc() { return Statics.toBvr(0.2); } // not used
	public Behavior gainFunc() { return Statics.toBvr(0.7); }
	public Behavior rateFunc() { return Statics.toBvr(1.4); }
}
