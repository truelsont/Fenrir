package fenrir.util.vornoi;

import java.util.Arrays;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;

import fenrir.util.Graph;
import fenrir.util.Point2D;
import fenrir.util.Triangle;
import fenrir.util.BoundingBox;
import fenrir.util.Edge;
import fenrir.util.Face;

public class VornoiDelauney extends Vornoi {

	// this maps vornoi edges to the delauney edges that created them
	private Map<Edge<Point2D>, Edge<Point2D>> bisectorMap;

	// the vornoi graph, centroid to the edges that create it
	private Map<Point2D, Face<Point2D>> vornoiFaces;

	// the points to the faces they are on
	private Map<Point2D, Set<Face<Point2D>>> pointToFaces;

	// points on the bounding box
	private Set<Point2D> boundaryPoints;

	public VornoiDelauney(Collection<Point2D> points, BoundingBox<Point2D> bounds) {
		super();
		if (points == null || bounds == null) {
			throw new IllegalArgumentException("VornoiDelauney needs proper args");
		}
		if (points.size() < 3) {
			throw new IllegalArgumentException("VornoiDelauney needs at least 3 points");
		}

		this.boundaryPoints = new HashSet<>();
		this.bisectorMap = new HashMap<>();
		this.vornoiFaces = new HashMap<>();
		this.pointToFaces = new HashMap<>();

		points.removeIf(p -> !bounds.contains(p));

		calculateVornoi(points, bounds);
	}

	private void computeAbstractVornoi(Collection<Point2D> points, BoundingBox<Point2D> bounds) {
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

				double scale = (bounds.getDimensions().getX() * bounds.getDimensions().getY()
						+ bounds.getDimensions().getY() * bounds.getDimensions().getY());

				Point2D farPoint = new Point2D((int) (midPoint.getX() + direction.getX() * scale),
						(int) (midPoint.getY() + direction.getY() * scale));

				Edge<Point2D> newEdge = clipEdge(circumcenter, farPoint, bounds);
				if (newEdge == null) {
					continue;
				}

				boundaryPoints.add(newEdge.getDst());
				this.vornoiGraph.addEdge(newEdge);
				bisectorMap.put(newEdge, edge);

			} else if (neighbors.size() == 2) {
				Point2D[] pair = neighbors.toArray(new Point2D[0]);

				Point2D p1 = pair[0];
				Point2D p2 = pair[1];

				Edge<Point2D> newEdge = clipEdge(p1, p2, bounds);
				if (newEdge == null) {
					continue;
				}

				bisectorMap.put(newEdge, edge);
				this.vornoiGraph.addEdge(newEdge);

			}

		}

	}

	private void calculateVornoi(Collection<Point2D> points, BoundingBox<Point2D> bounds) {
		// compute the abstract vornoi graph
		computeAbstractVornoi(points, bounds);

		// compute the internal faces
		computeInternalVornoiFaces();

		// compute the boudary edges and the add to faces
		boundaryCleanup(bounds);

		// create a face graph
		computeFaceGraph();
	}

	private void computeFaceGraph() {
		// TODO Auto-generated method stub

	}

	/*
	 * This function computes the vornoi faces for the vornoi graph if they are non
	 * boundary
	 * 
	 * 
	 * 
	 */
	private void computeInternalVornoiFaces() {

		// Assign each edge to the centroid that created it
		for (Edge<Point2D> vornoiEdge : bisectorMap.keySet()) {
			Edge<Point2D> delauneyEdge = bisectorMap.get(vornoiEdge);
			if (delauneyEdge == null) {
				continue;
			}

			// If the edge is in bisectorMap, it's a valid edge
			// Store it in the Voronoi faces map
			Point2D centroid1 = delauneyEdge.getSrc();
			Point2D centroid2 = delauneyEdge.getDst();

			Face<Point2D> face1;
			Face<Point2D> face2;
			if (vornoiFaces.containsKey(centroid1)) {
				face1 = vornoiFaces.get(centroid1);
			} else {
				face1 = new Face<>(centroid1);
				vornoiFaces.put(centroid1, face1);
			}

			if (vornoiFaces.containsKey(centroid2)) {
				face2 = vornoiFaces.get(centroid2);
			} else {
				face2 = new Face<>(centroid2);
				vornoiFaces.put(centroid2, face2);
			}

			face1.addEdge(vornoiEdge);
			face2.addEdge(vornoiEdge);

			// Add the edge to the pointToFaces map for each of its endpoints
			if (pointToFaces.get(vornoiEdge.getSrc()) == null) {
				pointToFaces.put(vornoiEdge.getSrc(), new HashSet<>());
			}
			if (pointToFaces.get(vornoiEdge.getDst()) == null) {
				pointToFaces.put(vornoiEdge.getDst(), new HashSet<>());
			}
			pointToFaces.get(vornoiEdge.getSrc()).add(face1);
			pointToFaces.get(vornoiEdge.getSrc()).add(face2);
			pointToFaces.get(vornoiEdge.getDst()).add(face1);
			pointToFaces.get(vornoiEdge.getDst()).add(face2);

		}
	}

	private void boundaryCleanup(BoundingBox<Point2D> bounds) {
		// Get the corners of the bounding box
		Point2D[] corners = { bounds.getTopCorner(),
				new Point2D(bounds.getTopCorner().getX() + bounds.getDimensions().getX(), bounds.getTopCorner().getY()),
				new Point2D(bounds.getTopCorner().getX() + bounds.getDimensions().getX(),
						bounds.getTopCorner().getY() + bounds.getDimensions().getY()),
				new Point2D(bounds.getTopCorner().getX(),
						bounds.getTopCorner().getY() + bounds.getDimensions().getY()) };

		for (Point2D p : corners) {
			boundaryPoints.add(p);
		}

		Point2D dims = bounds.getDimensions();
		Point2D topCorner = bounds.getTopCorner();
		Point2D center = new Point2D(topCorner.getX() + dims.getX() / 2, topCorner.getY() + dims.getY() / 2);

		// sort the points CCW from the center
		LinkedList<Point2D> sortedPoints = new LinkedList<>(boundaryPoints);
		sortedPoints.sort((p1, p2) -> {
			double angle1 = Math.atan2(p1.getY() - center.getY(), p1.getX() - center.getX());
			double angle2 = Math.atan2(p2.getY() - center.getY(), p2.getX() - center.getX());
			return Double.compare(angle1, angle2);
		});

		for (int i = 0; i < sortedPoints.size(); i++) {
			Point2D point1 = sortedPoints.get(i);
			Point2D point2 = sortedPoints.get((i + 1) % sortedPoints.size());
			Edge<Point2D> newEdge = new Edge<>(point1, point2, false);
			this.vornoiGraph.addEdge(newEdge);

			// Find the faces associated with the endpoints of the edge

			Set<Face<Point2D>> faces1 = pointToFaces.get(point1);
			Set<Face<Point2D>> faces2 = pointToFaces.get(point2);

			// If both endpoints have faces
			if (faces1 != null && faces2 != null) {
				// Take the intersection of the faces
				Set<Face<Point2D>> intersection = new HashSet<>(faces1); // make a copy of set1
				intersection.retainAll(faces2);

				// If the intersection is a singleton, assign the edge to that face
				if (faces1.size() >= 1) {
					assert faces1.size() == 1;
					Face<Point2D> face = intersection.iterator().next();

					face.addEdge(newEdge);
					face.addEdge(newEdge);

					pointToFaces.get(point1).add(face);
					pointToFaces.get(point2).add(face);
				}
			}
			// If only one endpoint has a face, it's a corner
			else if (faces1 != null || faces2 != null) {

				/// !!!! need to fix this

				Point2D corner = faces1 != null ? point1 : point2;
				Set<Face<Point2D>> faces = faces1 != null ? faces1 : faces2;

				
				assert(false); 
				//impl and use this in the generic graph class
			    Set<Edge<Point2D>> allEdges = new HashSet<>(pointToEdges.get(corner));
			    allEdges.remove(newEdge);

			    // Iterate over the remaining edges
			    for (Edge<Point2D> otherEdge : allEdges) {
			        // Get the face associated with the other endpoint of the other edge
			        Point2D otherEndpoint = getOtherEndpoint(otherEdge, corner);
			        Set<Face<Point2D>> otherFaces = pointToFaces.get(otherEndpoint);

			        Set<Face<Point2D>> intersection = new HashSet<>(faces); // make a copy of set1
			        intersection.retainAll(otherFaces);

			        // If the intersection is a singleton, assign the edge to that face
			        if (intersection.size() == 1) {
			            Face<Point2D> face = intersection.iterator().next();

			            face.addEdge(newEdge);

			            pointToFaces.get(point1).add(face);
			            pointToFaces.get(point2).add(face);
			        }
			    }
			}
			// If neither endpoint has a face
			else {
				// Find the edges that connect to this edge
				List<Edge<Point2D>> connectedEdges = findConnectedEdges(newEdge);

				// Get the faces associated with the other endpoints of the connected edges
				Set<Face<Point2D>> connectedFaces1 = new HashSet<>();
				Set<Face<Point2D>> connectedFaces2 = new HashSet<>();
				for (Edge<Point2D> connectedEdge : connectedEdges) {
					Point2D otherEndpoint1 = getOtherEndpoint(connectedEdge, point1);
					Point2D otherEndpoint2 = getOtherEndpoint(connectedEdge, point2);
					if (pointToFaces.containsKey(otherEndpoint1)) {
						connectedFaces1.addAll(pointToFaces.get(otherEndpoint1));
					}
					if (pointToFaces.containsKey(otherEndpoint2)) {
						connectedFaces2.addAll(pointToFaces.get(otherEndpoint2));
					}
				}

				// Take the intersection of the faces
				connectedFaces1.retainAll(connectedFaces2);

				// If the intersection is a singleton, assign the edge to that face
				if (connectedFaces1.size() == 1) {
					Face<Point2D> face = connectedFaces1.iterator().next();
					face.addEdge(newEdge);
				}
			}

		}
	}

	private Edge<Point2D> findOtherEdge(Point2D corner, Edge<Point2D> edge) {
		// Iterate over all edges in the Voronoi graph
		for (Edge<Point2D> otherEdge : this.vornoiGraph.edgeRepresentation()) {
			// If the other edge is not the same as the given edge and it shares the corner
			if (!otherEdge.equals(edge) && (otherEdge.getSrc().equals(corner) || otherEdge.getDst().equals(corner))) {
				return otherEdge;
			}
		}
		return null;
	}

	private List<Edge<Point2D>> findConnectedEdges(Edge<Point2D> edge) {
		List<Edge<Point2D>> connectedEdges = new LinkedList<>();
		// Iterate over all edges in the Voronoi graph
		for (Edge<Point2D> otherEdge : this.vornoiGraph.edgeRepresentation()) {
			// If the other edge shares a point with the given edge
			if (otherEdge.getSrc().equals(edge.getSrc()) || otherEdge.getDst().equals(edge.getDst())
					|| otherEdge.getSrc().equals(edge.getDst()) || otherEdge.getDst().equals(edge.getSrc())) {
				connectedEdges.add(otherEdge);
			}
		}
		return connectedEdges;
	}

	private Point2D getOtherEndpoint(Edge<Point2D> edge, Point2D point) {
		// Return the other endpoint of the edge
		if (edge.getSrc().equals(point)) {
			return edge.getDst();
		} else {
			return edge.getSrc();
		}
	}

	public Edge<Point2D> clipEdge(Point2D src, Point2D dst, BoundingBox<Point2D> bounds) {

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
			Edge<Point2D> edge = new Edge<>(src, newDst, false);
			return edge;
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
		double[] xBounds = { topCorner.getX(), topCorner.getX() + dimensions.getX() };
		double[] yBounds = { topCorner.getY(), topCorner.getY() + dimensions.getY() };

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

	public Map<Point2D, Face<Point2D>> getVornoiFaces() {
		return vornoiFaces;
	}

}
