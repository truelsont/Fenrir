package fenrir.util.vornoi;

import java.util.*;
import java.util.stream.Stream;

import fenrir.util.Vector;
import fenrir.util.vornoi.beachline.Beachline;
import fenrir.util.vornoi.event.Event;
import fenrir.util.vornoi.event.SiteEvent;
import fenrir.util.vornoi.graph.VornoiEdge;
import fenrir.util.vornoi.graph.VornoiGraph;
import fenrir.util.Graph;
import fenrir.util.Point;

public class Vornoi {
	private VornoiGraph graph;

	public Vornoi() {
		this.graph = new VornoiGraph();
	}

	public void vornoiDiagram(Collection<Vector> centroids, Vector corner, Vector bounds) {

		// Vornoi will only work one dim we can project back to ndim
		int SUPPORTED_DIM = 2;

		// Input checking
		Collection<Point> points = new LinkedList<>();
		try {
			if (corner.getDim() != SUPPORTED_DIM) {
				return;
			}
			if (bounds.getDim() != SUPPORTED_DIM) {
				return;
			}
			for (Vector centroid : centroids) {
				if (centroid.getDim() != SUPPORTED_DIM) {
					return;  
				}

				Point p = new Point(centroid.getValue(0), centroid.getValue(1));
				points.add(p);
			}

		} catch (Exception e) {
			e.printStackTrace();
		}

		// run algo
		fortunesAlgorithm(points);

	}

	private void fortunesAlgorithm(Collection<Point> points) {
		PriorityQueue<Event> queue = new PriorityQueue<Event>();
		points.stream().map(SiteEvent::new).forEach(queue::offer);
		points.forEach(graph::addSite);

		Beachline beachline = new Beachline();
		double sweep = Double.MAX_VALUE;
		while (!queue.isEmpty()) {
			Event e = queue.peek();
			assert e.getPoint().y <= sweep;
			e.handle(queue, beachline, graph);
			queue.remove(e);
			sweep = e.getPoint().y;
		}
	}
	
	public Graph<Point> geVornoiGraph(){
		
		Graph<Point> extractedGraph = new Graph<Point>(); 
		
		List<VornoiEdge> edges = (this.graph.getEdges().stream()).toList();
		for(VornoiEdge ve : edges) {
			if(ve.getA() == null || ve.getB() == null) {continue;}
			if(ve.getA().getLocation() == null || ve.getB().getLocation() == null) {continue;}
			extractedGraph.addEdge(ve.getA().getLocation(), ve.getB().getLocation()); 
		}
			
		return extractedGraph;  
	}
	
	

}
