package fenrir.util.vornoi;

import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.Map;
import java.util.Set;

import fenrir.util.Graph;
import fenrir.util.Point2D;
import fenrir.util.Triangle;
import fenrir.util.Edge;

public class VornoiDelauney extends Vornoi {
	
	public VornoiDelauney(Collection<Point2D> points) {
		Set<Triangle> delauneyTriangles = BowyerWatson.bowyerWatson(points); 
		Map<Edge<Point2D>, Collection<Point2D>> edgeToCircumcircle = new HashMap<>(); 
		
		for(Triangle tri: delauneyTriangles) {
			Point2D circumcenter = tri.getCircumcenter(); 
			if(circumcenter == null) {continue;}
			
			for(Edge<Point2D> e: tri.getEdges()) {
				edgeToCircumcircle.putIfAbsent(e, new HashSet<Point2D>()); 
				edgeToCircumcircle.get(e).add(circumcenter); 
			}
		}
		
		
		for(Edge<Point2D> edge: edgeToCircumcircle.keySet()) {
			Collection<Point2D> neighbors = edgeToCircumcircle.get(edge); 
			
            if (neighbors.size() == 1) {
                Point2D[] singleNeighbor = neighbors.toArray(new Point2D[0]);
                Point2D circumcenter = singleNeighbor[0];
                Point2D midPoint = new Point2D(
                    (edge.getSrc().getX() + edge.getDst().getX()) / 2,
                    (edge.getSrc().getY() + edge.getDst().getX()) / 2
                );
                Point2D direction = new Point2D(
                    (edge.getDst().getY() - edge.getSrc().getY()),
                    -edge.getDst().getX() + edge.getSrc().getX()
                );

                double scale = 1000.0;
                Point2D farPoint = new Point2D(
                    (int)(midPoint.x + direction.x * scale),
                    (int)(midPoint.y + direction.y * scale)
                );

                this.vornoiGraph.addEdge(circumcenter, farPoint, false);
            } else if (neighbors.size() == 2) {
                Point2D[] pair = neighbors.toArray(new Point2D[0]);
                this.vornoiGraph.addEdge(pair[0], pair[1], false);
            }
			
		}
		
	}
	
	


}
