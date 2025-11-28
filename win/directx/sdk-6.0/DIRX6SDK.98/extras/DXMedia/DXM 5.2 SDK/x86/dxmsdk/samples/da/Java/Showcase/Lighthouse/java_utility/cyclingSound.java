//
// cyclingSound: Creates a looped sound with algoFunc properties
// Copyright (c) 1997, Microsoft Corporation
//
// The cyclingSound method is used to create rich and interesting sounds using
// native DirectMedia sound behaviors: gain, loop, restart, and frequency.
// It Creates a looped sound with the sound, gain and rate methods specified
// in the implementation of the algorithmic method interface
//
package java_utility;
import com.ms.dxmedia.*;

public class cyclingSound 
{
	public cyclingSound(algoFunc dynamicVals) { dynaVals = dynamicVals; }

	private algoFunc dynaVals;

	public SoundBvr Sound(SoundBvr initSound) { 
		SoundBvr dynaSnd = 
			initSound.rate((NumberBvr)dynaVals.rateFunc()).
				gain((NumberBvr)dynaVals.gainFunc()).loop();
		return dynaSnd;
	}	
}

