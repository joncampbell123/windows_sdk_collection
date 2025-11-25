//
// periodicSound: Creates a periodically repeating sound with algoFunc properties
// Copyright (c) 1997, Microsoft Corporation
//
// The periodicSound method is used to create rich and interesting sounds using
// native DirectMedia sound behaviors: gain, loop, restart, and frequency.
// It creates a periodicaly restarting sound with the sound, gain and rate 
// functions specified in the implementation of the algorithmic method interface.
//
package java_utility;
import com.ms.dxmedia.*;

public class periodicSound extends Statics
{	
	public periodicSound(algoFunc dynamicVals) { dynaVals = dynamicVals; }

	private algoFunc dynaVals;

	public SoundBvr Sound(SoundBvr initSound) { 
		SoundBvr dynaSnd = 
			initSound.rate((NumberBvr)dynaVals.rateFunc())
				.gain((NumberBvr)dynaVals.gainFunc());		
		Behavior justOne[] = { dynaSnd };
		Cycler cyc = new Cycler( justOne, Trigger() );
		SoundBvr endSnd = (SoundBvr)cyc.getBvr();
		return endSnd;
	}

	public DXMEvent Trigger() { 
		return timer((NumberBvr)dynaVals.periodFunc());
	}
}

