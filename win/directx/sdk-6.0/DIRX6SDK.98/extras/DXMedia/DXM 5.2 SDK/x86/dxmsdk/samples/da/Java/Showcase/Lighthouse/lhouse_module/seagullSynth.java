// Copyright (c) 1997 Microsoft Corporation

package lhouse_module;

import com.ms.dxmedia.*;
import java.net.*;
import java_utility.*;

public class seagullSynth extends Statics {
	Vector3Bvr _birdPath;
	public seagullSynth(Vector3Bvr path) { _birdPath = path; }
	public seagullSynth() { _birdPath = zeroVector3; }

	public GeometryBvr getGeo(boolean usePath, URL importBase) {
		GeometryBvr  birdBodyGeo = importGeometry(buildURL(importBase, "geometry/birdbody.x"));
    birdBodyGeo = birdBodyGeo.texture(importImage(buildURL(importBase, "image/bird.gif")));
    birdBodyGeo = birdBodyGeo.transform(rotate(yVector3, toBvr(Math.PI)));
    GeometryBvr  leftWingGeo = importGeometry(buildURL(importBase, "geometry/leftwing.x"));
    leftWingGeo = leftWingGeo.texture(importImage(buildURL(importBase, "image/wing.gif")));
		GeometryBvr  rightWingGeo = importGeometry(buildURL(importBase, "geometry/rightwing.x"));
    rightWingGeo = rightWingGeo.texture(importImage(buildURL(importBase, "image/wing.gif")));
    
		Vector3Bvr  direction = derivative(_birdPath);
		Vector3Bvr  futureDirection = (Vector3Bvr)
			direction.substituteTime(add(localTime, toBvr(0.1)));		
		Vector3Bvr runDirection = (Vector3Bvr)direction.runOnce();

		NumberBvr  angle = mul(toBvr(30), cross(direction, futureDirection).getY());
		NumberBvr  roll = DxmNumber.clamp( angle, neg(toBvr(Math.PI/8)), toBvr(Math.PI/8) );			

		NumberBvr  speed = (NumberBvr) cond( toBvr(usePath), 
			mul(toBvr(10), add(direction.length(), direction.getY())), toBvr(5) );

		NumberBvr  wingSpeed = (NumberBvr) 
			cond( gte(speed, toBvr(0)), speed, toBvr(0));
	
		NumberBvr  wingAngle = integral(wingSpeed);

		GeometryBvr lighting = 		  
			union( directionalLight.transform(rotate(xVector3, toBvr(-Math.PI/2) )), 
			union( directionalLight.transform(rotate(xVector3, toBvr( Math.PI/2) )),
			union( directionalLight.transform(rotate(yVector3, toBvr( Math.PI/3) )),
				   directionalLight.transform(rotate(yVector3, toBvr(-Math.PI/3) )) )));

		GeometryBvr	leftWing = 
			leftWingGeo.transform(rotate(zVector3, mul(sin(wingAngle), toBvr(Math.PI/8)) ));

		GeometryBvr rightWing = 
			rightWingGeo.transform(rotate(zVector3, mul(neg(sin(wingAngle)), toBvr(Math.PI/8)) ));

		GeometryBvr	birdGeo =
			union( lighting,
			union( birdBodyGeo,
			union( leftWing,
				   rightWing )));

		GeometryBvr seagull = (GeometryBvr)                       
			cond( lte( runDirection.length(), toBvr(.001) ),
				birdGeo.transform( compose(compose( 
					translate(_birdPath), rotate(zVector3, roll)), scale3(toBvr(2))) ),
				birdGeo.transform(
					compose(
						lookAtFrom(
							add(add( origin3, runDirection), _birdPath ),
							add( origin3, _birdPath ),
							yVector3
						),
						compose(rotate(zVector3, roll), scale3(toBvr(3))) 
					)
				)
			);
		return seagull;
	}

	public SoundBvr getSound(URL importBase) {
		SoundBvr seagullSnd = importSound(buildURL(importBase, "seagull.mp2"),null);
		periodicSound squawk1 = new periodicSound(new squawk1());
		periodicSound squawk2 = new periodicSound(new squawk2()); 

		// Sounds - spatialized using left and right ear positions
		Vector3Bvr earPosL = vector3(toBvr(-0.25), toBvr(0.5), toBvr(1.0));
		Vector3Bvr earPosR = vector3(toBvr(0.25), toBvr(0.5), toBvr(1.0));
		NumberBvr gullDistanceL = sub(_birdPath, earPosL).length();
		NumberBvr gullDistanceR = sub(_birdPath, earPosR).length();

		NumberBvr gullGainL = add(toBvr(0.7), div(sub(toBvr(1.0),
			sub(gullDistanceL,toBvr(.5))), toBvr(3)));
		NumberBvr gullGainR = add(toBvr(0.7), div(sub(toBvr(1.0),
			sub(gullDistanceR,toBvr(.5))), toBvr(3)));

		SoundBvr  gullSound = 
			mix(
				squawk1.Sound(seagullSnd).gain(gullGainL).pan(toBvr(-1)), 
				squawk2.Sound(seagullSnd).gain(gullGainR).pan(toBvr( 1))
			);
		return gullSound;
	}
}

class squawk1 extends Statics implements algoFunc {	 
	public Behavior periodFunc() {		// periodicity
		return add(toBvr(1), toBvr(5*Math.random())); }
	public Behavior gainFunc() {		// gain
		return add(toBvr(0.6), toBvr(0.2*Math.random())); }	
	public Behavior rateFunc() { return toBvr(0.8); } // rate
		
}

class squawk2 extends Statics implements algoFunc {	 
	public Behavior periodFunc() {		// periodicity
		return add(toBvr(1), toBvr(3*Math.random()) ); }
	public Behavior gainFunc() {    // gain
		return toBvr(0.8); }	
	public Behavior rateFunc() { return toBvr(1.1); } // rate
}
