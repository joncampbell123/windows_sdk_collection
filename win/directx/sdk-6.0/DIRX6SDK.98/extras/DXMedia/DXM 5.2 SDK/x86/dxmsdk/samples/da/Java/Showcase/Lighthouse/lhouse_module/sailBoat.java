// Copyright (c) 1997 Microsoft Corporation

// The sail boat travels a spline path with its speed based on current wind speed
package lhouse_module;

import com.ms.dxmedia.*;
import java.net.*;
import java_utility.*;

public class sailBoat extends Statics {
	public static GeometryBvr getGeo(NumberBvr windSpeed, URL importBase) {
		GeometryBvr boatGeo = importGeometry(buildURL(importBase, "sailboat.x"));
    boatGeo = boatGeo.transform(rotate(yVector3, toBvr(Math.PI)));
		
		Vector3Bvr figure8 = DxmVector3.figure8(); // spline creation utility

    Vector3Bvr boatPos1 = 
			figure8.transform(           
        compose(translate(toBvr(0), toBvr(0), toBvr(-15)),
          compose(scale(toBvr(5), toBvr(0), toBvr(20)),
					  rotate(yVector3, toBvr(Math.PI / 2)))));  

		Vector3Bvr boatPos = (Vector3Bvr)boatPos1.substituteTime(div(localTime,toBvr(0.75)));
		
		NumberBvr temp1_1 = sin(localTime);
		NumberBvr temp1_2 = toBvr(Math.PI/48);
		NumberBvr temp1_3 = mul(windSpeed,toBvr(5));

		NumberBvr boatHeel = mul(temp1_1,mul(temp1_2,temp1_3));

//		NumberBvr boatHeel = mul(mul( sin(localTime), toBvr(Math.PI/48)), windSpeed);
		GeometryBvr heelingBoat =
			boatGeo.transform(rotate(zVector3, boatHeel));

		NumberBvr boatTime = integral(add(windSpeed,toBvr(0.1)));
   
		GeometryBvr boatLight = 
			union(directionalLight.lightColor(colorRgb(toBvr(.25),toBvr(.25),toBvr(.25))),
				  ambientLight.lightColor(colorRgb(toBvr(.5),toBvr(.5),toBvr(.5))));

		Vector3Bvr boatDir = derivative(boatPos);
		GeometryBvr boat = 
			heelingBoat.transform(
				lookAtFrom(
					add(add( origin3, boatDir), boatPos),
					  add( origin3, boatPos), yVector3));				

		GeometryBvr movingBoat =
			union(boat.transform(translate(toBvr(12.5), toBvr(-7), toBvr(-40))),boatLight);

		GeometryBvr sailboat = (GeometryBvr)
			movingBoat.substituteTime(boatTime);

		return sailboat;
	}
}