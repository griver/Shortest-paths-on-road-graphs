function drawPath(lon1, lat1, lon2, lat2) {                                
	var points = new Array(
		new OpenLayers.Geometry.Point(lon1, lat1),
		new OpenLayers.Geometry.Point(lon2, lat2)
	);

	var line = new OpenLayers.Geometry.LineString(points);

	var style = { 
				strokeColor: '#0000ff', 
		        strokeOpacity: 0.5,
		        strokeWidth: 5
	};

	var lineFeature = new OpenLayers.Feature.Vector(line, null, style);
	vectorLayer.removeAllFeatures();
	vectorLayer.addFeatures([lineFeature]);
}

function drawPolyPath(arr) {   
	var points = new Array();
	
	for (i = 0; i < arr.length; i += 2) {
		points.push(new OpenLayers.Geometry.Point(arr[i], arr[i + 1]).transform(
			new OpenLayers.Projection("EPSG:4326"),
			new OpenLayers.Projection("EPSG:900913")
		))
	}

	var line = new OpenLayers.Geometry.LineString(points);

	var style = { 
				strokeColor: '#0000ff', 
		        strokeOpacity: 0.5,
		        strokeWidth: 5
	};

	var lineFeature = new OpenLayers.Feature.Vector(line, null, style);
	vectorLayer.removeAllFeatures();
	vectorLayer.addFeatures([lineFeature]);
}

function drawPointsArray(arr) {   
	pointsLayer.removeAllFeatures();
	var points = new Array();
	
	var style = {
		pointRadius: 8,
		fillColor: "red"
	};
	
	for (i = 0; i < arr.length; i += 2) {
		points.push(new OpenLayers.Geometry.Point(arr[i], arr[i + 1]).transform(
			new OpenLayers.Projection("EPSG:4326"),
			new OpenLayers.Projection("EPSG:900913")
		));
	}
	
	var multip = new OpenLayers.Geometry.MultiPoint(points)
	var pointFeature = new OpenLayers.Feature.Vector(multip, null, style);
	pointsLayer.addFeatures([pointFeature]);
}
