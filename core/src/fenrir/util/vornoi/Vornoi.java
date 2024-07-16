package fenrir.util.vornoi;

import fenrir.util.Graph;
import fenrir.util.Point2D;

public abstract class Vornoi {
	protected Graph<Point2D> vornoiGraph; 
	
	public Vornoi() {
		this.vornoiGraph = new Graph<>(); 
	}
	
	public Graph<Point2D> getGraph(){
		return this.vornoiGraph; 
	}

}
