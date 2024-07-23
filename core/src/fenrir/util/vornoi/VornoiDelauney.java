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
	
	// input points
	//private final Set<Point2D> points;
	
	// input bounding box
	//private final BoundingBox<Point2D> bounds;

	public VornoiDelauney(Collection<Point2D> points, BoundingBox<Point2D> bounds) {
		super();

		if (points == null || bounds == null) {
			throw new IllegalArgumentException("VornoiDelauney needs proper args");
		}
		
		//points = (HashSet<Point2D> ) points.clone();  
		//bounds = (BoundingBox<Point2D>) bounds.copy(); 
		
		if(points == null || bounds == null) {
			throw new IllegalArgumentException("VornoiDelauney couldn't clone args" );
		}

		for (Point2D p : points) {
			if (p == null) {
				throw new IllegalArgumentException("VornoiDelauney needs non-null points");
			}
		}

		points.removeIf(p -> !bounds.contains(p));
		
		if (points.size() < 3) {
			throw new IllegalArgumentException("VornoiDelauney needs at least 3 points not on boundary");
		}

		this.boundaryPoints = new HashSet<>();
		this.bisectorMap = new HashMap<>();
		this.vornoiFaces = new HashMap<>();
		this.pointToFaces = new HashMap<>();

        //this.points = points;
        //this.bounds = bounds;

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
				
			    // Check if the source of the new edge is not in the boundary
			    if (!bounds.contains(newEdge.getSrc())) {
			        // If so, add it to the boundary
			        boundaryPoints.add(newEdge.getSrc());
			    }

			    // Check if the destination of the new edge is not in the boundary
			    if (!bounds.contains(newEdge.getDst())) {
			        // If so, add it to the boundary
			        boundaryPoints.add(newEdge.getDst());
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

		// create boundary edges
		Set<Edge<Point2D>> boundaryEdges = new HashSet<>();

		for (int i = 0; i < sortedPoints.size(); i++) {
			Point2D point1 = sortedPoints.get(i);
			Point2D point2 = sortedPoints.get((i + 1) % sortedPoints.size());
			Edge<Point2D> newEdge = new Edge<>(point1, point2, false);
			this.vornoiGraph.addEdge(newEdge);
			boundaryEdges.add(newEdge);
		}

		createBoundaryFaces(boundaryEdges);
	}

	private void createBoundaryFaces(Set<Edge<Point2D>> boundaryEdges) {

		Set<Edge<Point2D>> explored = new HashSet<>();
		for (Edge<Point2D> newEdge : boundaryEdges) {
			if (explored.contains(newEdge)) {
				continue;
			}

			Point2D point1 = newEdge.getSrc();
			Point2D point2 = newEdge.getDst();
			
			Set<Face<Point2D>> faces1 = pointToFaces.get(point1);
			Set<Face<Point2D>> faces2 = pointToFaces.get(point2);

			Set<Edge<Point2D>> edgesToClassify = new HashSet<>();

			explored.add(newEdge);

			while (faces1 == null && point1 != point2) {
				Set<Edge<Point2D>> edges = vornoiGraph.getEdges(point1);

				boolean deadend = true;
				for (Edge<Point2D> edge : edges) {
					if (!explored.contains(edge)) {
						explored.add(edge);
						edgesToClassify.add(edge);

						point1 = getOtherEndpoint(edge, point1);
						faces1 = pointToFaces.get(point1);
						deadend = false;
					}
				}

				if (deadend) {
					break;
				}

			}

			while (faces2 == null && !point1.equals(point2)) {
			    Set<Edge<Point2D>> edges = vornoiGraph.getEdges(point2);

			    boolean deadend = true;
			    for (Edge<Point2D> edge : edges) {
			        if (!explored.contains(edge)) {
			            explored.add(edge);
			            edgesToClassify.add(edge);

			            point2 = getOtherEndpoint(edge, point2);
			            faces2 = pointToFaces.get(point2);
			            deadend = false;
			        }
			    }

			    if (deadend) {
			        break;
			    }
			}
			if(faces1 == null && faces2 == null) {continue;}
			Set<Face<Point2D>> intersection = new HashSet<>(faces1);
			intersection.retainAll(faces2);

			if (intersection.size() >= 1) {
				Face<Point2D> face = intersection.iterator().next();

				for (Edge<Point2D> e : edgesToClassify) {
					face.addEdge(e);
					
					pointToFaces.putIfAbsent(e.getSrc(), new HashSet<>()); 
					pointToFaces.putIfAbsent(e.getDst(), new HashSet<>()); 

					pointToFaces.get(e.getSrc()).add(face);
					pointToFaces.get(e.getDst()).add(face);
				}
			}else {
			    // Create a new face
				assert(false); 
				Point2D p = new Point2D(0d,0d); 
			    Face<Point2D> newFace = new Face<>(p);

			    // Add all the edges to the new face
			    for (Edge<Point2D> e : edgesToClassify) {
			        newFace.addEdge(e);

			        // Add the new face to the pointToFaces map for each of its endpoints
			        pointToFaces.get(e.getSrc()).add(newFace);
			        pointToFaces.get(e.getDst()).add(newFace);
			    }

			    // Add the new face to the vornoiFaces map
			    vornoiFaces.put(newFace.getCenter(), newFace);
			}

		}
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
