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
import fenrir.util.BoundingBox;
import fenrir.util.Edge;

public class VornoiDelauney extends Vornoi {

	private Set<Point2D> boundaryPoints;

	public VornoiDelauney() {
		super();
		this.boundaryPoints = new HashSet<>();

	}

	public VornoiDelauney(Collection<Point2D> points, BoundingBox<Point2D> bounds) {
		points.removeIf(p -> !bounds.contains(p));

		Set<Triangle> delauneyTriangles = BowyerWatson.bowyerWatson(points);
		Map<Edge<Point2D>, Collection<Point2D>> edgeToCircumcircle = new HashMap<>();

		for (Triangle tri : delauneyTriangles) {
			Point2D circumcenter = tri.getCircumcenter();
			if (circumcenter == null) {
				continue;
			}

			for (Edge<Point2D> e : tri.getEdges()) {
				edgeToCircumcircle.putIfAbsent(e, new HashSet<Point2D>());
				edgeToCircumcircle.get(e).add(circumcenter);
			}
		}

		for (Edge<Point2D> edge : edgeToCircumcircle.keySet()) {
			Collection<Point2D> neighbors = edgeToCircumcircle.get(edge);

			if (neighbors.size() == 1) {
				Point2D[] singleNeighbor = neighbors.toArray(new Point2D[0]);
				Point2D circumcenter = singleNeighbor[0];
				Point2D midPoint = new Point2D((edge.getSrc().getX() + edge.getDst().getX()) / 2,
						(edge.getSrc().getY() + edge.getDst().getX()) / 2);
				Point2D direction = new Point2D((edge.getDst().getY() - edge.getSrc().getY()),
						-edge.getDst().getX() + edge.getSrc().getX());
				direction.extend(); 
				
				double scale = (bounds.getDimensions().getX() * bounds.getDimensions().getY() + 
								bounds.getDimensions().getY() * bounds.getDimensions().getY());
						
				Point2D farPoint = new Point2D((int) (midPoint.getX() + direction.getX() * scale),
						(int) (midPoint.getY() + direction.getY() * scale));

				Edge<Point2D> newEdge = clipEdge(circumcenter, farPoint, bounds);
				if (newEdge == null) {
					continue;
				}
				this.vornoiGraph.addEdge(newEdge);
				this.vornoiGraph.addOwnership(edge.getSrc(), newEdge);
				this.vornoiGraph.addOwnership(edge.getDst(), newEdge);

			} else if (neighbors.size() == 2) {
				Point2D[] pair = neighbors.toArray(new Point2D[0]);

				Point2D p1 = pair[0];
				Point2D p2 = pair[1];

				Edge<Point2D> newEdge = clipEdge(p1, p2, bounds);
				if (newEdge == null) {
					continue;
				}

				this.vornoiGraph.addEdge(newEdge);
				this.vornoiGraph.addOwnership(edge.getSrc(), newEdge);
				this.vornoiGraph.addOwnership(edge.getDst(), newEdge);
			}

		}

	}

	public Edge<Point2D> clipEdge(Point2D src, Point2D dst, BoundingBox<Point2D> bounds) {
		//return new Edge<>(src, dst, false); 

		// Check if the edge is completely outside the bounds
		if (!bounds.contains(src) && !bounds.contains(dst)) {
			return null;
		}
		if (bounds.contains(src) && bounds.contains(dst)) {
			return new Edge<>(src, dst, false);
		}

		if (!bounds.contains(src)) {
			return clipEdge(dst, src, bounds);
		}

	    // Now we know that src is inside and dst is outside
	    Point2D newDst = getIntersection(src, dst, bounds);

	    if (newDst != null) {
	        return new Edge<>(src, newDst, false);
	    }
		return null;

	}


public Point2D getIntersection(Point2D src, Point2D dst, BoundingBox<Point2D> bounds) {
    Point2D topCorner = bounds.getTopCorner();
    Point2D dimensions = bounds.getDimensions();

    double x1 = src.getX();
    double y1 = src.getY();
    double x2 = dst.getX();
    double y2 = dst.getY();

    // Calculate the direction of the line
    Point2D direction = new Point2D(x2 - x1, y2 - y1);

    // Check each side of the bounding box for intersection
    double[] xBounds = {topCorner.getX(), topCorner.getX() + dimensions.getX()};
    double[] yBounds = {topCorner.getY(), topCorner.getY() + dimensions.getY()};

    Point2D closestIntersection = null;
    double smallestT = Double.MAX_VALUE;

    for (double x : xBounds) {
        double t = (x - x1) / direction.getX();
        double y = y1 + t * direction.getY();
        if (t > 0 && y >= yBounds[0] && y <= yBounds[1] && t < smallestT) {
            closestIntersection = new Point2D(x, y);
            smallestT = t;
        }
    }

    for (double y : yBounds) {
        double t = (y - y1) / direction.getY();
        double x = x1 + t * direction.getX();
        if (t > 0 && x >= xBounds[0] && x <= xBounds[1] && t < smallestT) {
            closestIntersection = new Point2D(x, y);
            smallestT = t;
        }
    }

    // Return the intersection point with the smallest positive t
    return closestIntersection;
}


}
