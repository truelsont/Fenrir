package fenrir.util.Voronoi;

import fenrir.util.Graph;
import fenrir.util.Point2D;
import java.util.Map;
import fenrir.util.Face;

public abstract class Voronoi {
	protected Graph<Point2D> VoronoiGraph; 
	protected Graph<Face<Point2D>> VoronoiFacesGraph; 
	
	public Voronoi() {
		this.VoronoiGraph = new Graph<>(); 
	}
	
	public Graph<Point2D> getGraph(){
		return this.VoronoiGraph; 
	}
	
	public Graph<Face<Point2D>> getVoronoiFaces() {
		return VoronoiFacesGraph; 
	}

	public abstract void floydRelaxations();
}
