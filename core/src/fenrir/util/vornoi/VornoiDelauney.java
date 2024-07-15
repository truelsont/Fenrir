package fenrir.util.vornoi;

import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.Map;
import java.util.Set;

import fenrir.util.Graph;
import fenrir.util.Point;
import fenrir.util.Triangle;
import fenrir.util.Edge;

public class VornoiDelauney {
	
	private Graph<Point> vornoiGraph; 

	public VornoiDelauney(Collection<Point> points) {
		this.vornoiGraph = new Graph<Point>(); 
		Set<Triangle> delauneyTriangles = BowyerWatson.bowyerWatson(points); 
		
		/*
		for(Triangle tri: delauneyTriangles) {
			vornoiGraph.addEdge(tri.getP1(), tri.getP2(), false);
			vornoiGraph.addEdge(tri.getP1(), tri.getP2(), false);
vornoiGraph.addEdge(tri.getP1(), tri.getP2(), false);
		}
		*/
		
		
		
		
		
		
		Map<Edge<Point>, Collection<Point>> edgeToCircumcircle = new HashMap<>(); 
		
		for(Triangle tri: delauneyTriangles) {
			Point circumcenter = tri.getCircumcenter(); 
			if(circumcenter == null) {continue;}
			
			for(Edge<Point> e: tri.getEdges()) {
				edgeToCircumcircle.putIfAbsent(e, new HashSet<Point>()); 
				edgeToCircumcircle.get(e).add(circumcenter); 
			}
		}
		
		
		for(Edge<Point> edge: edgeToCircumcircle.keySet()) {
			Collection<Point> neighbors = edgeToCircumcircle.get(edge); 
			
            if (neighbors.size() == 1) {
                Point[] singleNeighbor = neighbors.toArray(new Point[0]);
                Point circumcenter = singleNeighbor[0];
                Point midPoint = new Point(
                    (edge.getSrc().getX() + edge.getDst().getX()) / 2,
                    (edge.getSrc().getY() + edge.getDst().getX()) / 2
                );
                Point direction = new Point(
                    (edge.getDst().getY() - edge.getSrc().getY()),
                    -edge.getDst().getX() + edge.getSrc().getX()
                );

                double scale = 1000.0;
                Point farPoint = new Point(
                    (int)(midPoint.x + direction.x * scale),
                    (int)(midPoint.y + direction.y * scale)
                );

                this.vornoiGraph.addEdge(circumcenter, farPoint, false);
            } else if (neighbors.size() == 2) {
                Point[] pair = neighbors.toArray(new Point[0]);
                this.vornoiGraph.addEdge(pair[0], pair[1], false);
            }
			
		}
		
	}

	public Graph<Point> retrieveGraph(Point origin, Point dims) {
		// TODO Auto-generated method stub
		return this.vornoiGraph; 
	}

}
