package fenrir.util.vornoi;

import fenrir.util.Triangle; 
import fenrir.util.Edge;
import fenrir.util.Point2D;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

public class BowyerWatson {



	public static Set<Triangle> bowyerWatson(Collection<Point2D> points) {
		Set<Triangle> triangulation = new HashSet<>();

		// Create a super-triangle that encompasses all the points
		Triangle superTriangle = new Triangle(new Point2D(-1, -1), new Point2D(1600, -1),
				new Point2D(-1, 1200));
		triangulation.add(superTriangle);

		for (Point2D point : points) {
			Set<Triangle> badTriangles = new HashSet<>();

			for (Triangle triangle : triangulation) {
				if (triangle.isInsideCircumcircle(point)) {
					badTriangles.add(triangle);
				}
			}

			Set<Edge<Point2D>> polygon = new HashSet<>();
			for (Triangle triangle : badTriangles) {
				for (Edge<Point2D> edge : triangle.getEdges()) {
					boolean shared = false;
					for (Triangle other : badTriangles) {
						if (other != triangle && other.getEdges().contains(edge)) {
							shared = true;
							break;
						}
					}
					if (!shared) {
						polygon.add(edge);
					}
				}
			}

			for (Triangle triangle : badTriangles) {
				triangulation.remove(triangle);
			}

			for (Edge<Point2D> edge : polygon) {
				Triangle newTri = new Triangle(edge.getSrc(), edge.getDst(), point);
				triangulation.add(newTri);
			}
		}

		Set<Triangle> toRemove = new HashSet<>();
		for (Triangle triangle : triangulation) {
			if (triangle.containsVertex(superTriangle.getP1()) || triangle.containsVertex(superTriangle.getP2())
					|| triangle.containsVertex(superTriangle.getP3())) {
				toRemove.add(triangle);
			}
		}
		triangulation.removeAll(toRemove);

		return triangulation;
	}

	public static void main(String[] args) {
		List<Point2D> pointList = new ArrayList<>();
		pointList.add(new Point2D(0, 0));
		pointList.add(new Point2D(1, 0));
		pointList.add(new Point2D(0, 1));
		pointList.add(new Point2D(1, 1));

		Set<Triangle> result = bowyerWatson(pointList);


	}
}