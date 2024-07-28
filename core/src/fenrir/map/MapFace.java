package fenrir.map;


import fenrir.util.Face;
import fenrir.util.Point2D;

public class MapFace {

	//this is the face that is being represented
	public final Face<Point2D> face;
	public double denisty; 
	public double elevation;
	public ClimateType climate;


	//these are default but should be recomputed after the init pass for a unit interval
	static double MIN_ELEVATION = -10000d; 
	static double MAX_ELEVATION = 10000d;
	static double SEA_ELEVATION_RATIO = 0.5; 
	
	static enum ClimateType {
		OCEAN, LAND; 
	}




	public enum FaceType{
		ENVIRONMENT, 
		POLITICAL,
		ECONOMIC
	}


	public MapFace(Face<Point2D> face) {
		this.face = face;
	}

	public void render(FaceType type){
		switch(type){
		case ENVIRONMENT:
			//render the environment
			break;
		default: 
			throw new IllegalArgumentException("FaceType not recognized");
		}
	}

}
