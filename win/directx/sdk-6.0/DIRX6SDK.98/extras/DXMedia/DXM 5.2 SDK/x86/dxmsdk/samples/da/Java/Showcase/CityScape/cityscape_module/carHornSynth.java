package cityscape_module;

import com.ms.dxmedia.*;
import java.net.*;
import java_utility.*;

public class carHornSynth extends Statics  {
	private periodicSound _carhorn1;
	private periodicSound _carhorn2;
	
	public carHornSynth()  {
		_carhorn1 = new periodicSound(new carhorn1());
		_carhorn2 = new periodicSound(new carhorn2());
	}

	public SoundBvr getSound(URL importBase)  { 		
		SoundBvr carhorn = importSound(buildURL(importBase, "carhorn1.mp2"),null);
		return mix(_carhorn1.Sound(carhorn), _carhorn2.Sound(carhorn));
	}

	public DXMEvent soundEvent()  { 
		return _carhorn1.Trigger(); 
	}
}

class carhorn1 extends Statics implements algoFunc
{
	public Behavior periodFunc()  {
		return add(toBvr(1),mul(toBvr(10),toBvr(Math.random()))); 
	}

	public Behavior gainFunc()  {		
		return add(toBvr(0.6),mul(toBvr(0.2),toBvr(Math.random()))); 
	}	

	public Behavior rateFunc()  {
		return add(toBvr(0.8),mul(toBvr(0.3),toBvr(Math.random()))); 
	}
}

class carhorn2 extends Statics implements algoFunc
{
	public Behavior periodFunc()  {
		return add(toBvr(5),mul(toBvr(15),toBvr(Math.random()))); 
	}

	public Behavior gainFunc()  {
		return add(toBvr(0.8),mul(toBvr(0.2),toBvr(Math.random()))); 
	}	

	public Behavior rateFunc()  {
		return add(toBvr(0.8),mul(toBvr(0.3),toBvr(Math.random()))); 
	}
}
