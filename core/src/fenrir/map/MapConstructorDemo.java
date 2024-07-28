package fenrir.map;

import javax.swing.*;

import fenrir.util.BoundingBox;
import fenrir.util.Edge;
import fenrir.util.Face;
import fenrir.util.Graph;
import fenrir.util.Point2D;
import fenrir.util.Triangle;
import fenrir.util.Vector;
import fenrir.util.Voronoi.*;

import java.awt.*;
import java.awt.geom.Line2D;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Random;
import java.util.Set;
import java.util.HashMap;
import java.util.Queue;

public class MapConstructorDemo extends JPanel {

	private Set<Point2D> vertices;
	private Set<Point2D> points;
	private Set<Point2D> boundaryPoints;
	private Voronoi V;

	private Graph<Point2D> computedVoronoi;
	private int Xtranslate = 100;
	private int Ytranslate = 100;

	public MapConstructorDemo() {
		// points = generateRandomPoints(100);
		BoundingBox<Point2D> bbox = new BoundingBox<>(new Point2D(0, 0), new Point2D(800, 600));
		points = generateRandomPoints(1000);
		// points = generateClusteredPoints(1000, 10); // Generate 10 random points

		VoronoiDelauney V = new VoronoiDelauney(points, bbox);
		for (int i = 0; i < 1; i++) {
			V.floydRelaxations();
		}
		this.V = V;
		computedVoronoi = V.getGraph();

		// boundaryPoints = V.boundaryPoints;

	}

	public Map<Integer, Set<Face<Point2D>>> assignFacesToRegions(Graph<Face<Point2D>> voronoiFaces) {
		Map<Integer, Set<Face<Point2D>>> regionMap = new HashMap<>();
		Map<Face<Point2D>, Boolean> visited = new HashMap<>();
		Random rand = new Random();
		int numberOfRegions = 3; // Number of different regions

		// Initialize regions with random faces
		List<Face<Point2D>> faces = new ArrayList<>(voronoiFaces.getVertices());
		Collections.shuffle(faces, rand);

		// Initialize queues for each region
		List<Queue<Face<Point2D>>> regionQueues = new ArrayList<>();
		for (int i = 0; i < numberOfRegions; i++) {
			regionQueues.add(new LinkedList<>());
		}

		// Assign initial faces to each region
		for (int i = 0; i < numberOfRegions; i++) {
			for (Face<Point2D> face : faces) {
				if (!visited.containsKey(face)) {
					regionQueues.get(i).add(face);
					visited.put(face, true);
					regionMap.computeIfAbsent(i, k -> new HashSet<>()).add(face);
					break;
				}
			}
		}

		// Perform BFS for each region with random iterations
		boolean allAssigned = false;
		while (!allAssigned) {
			allAssigned = true;
			for (int i = 0; i < numberOfRegions; i++) {
				int iterations = rand.nextInt(10) + 1; // Random number of BFS iterations
				Queue<Face<Point2D>> queue = regionQueues.get(i);

				for (int j = 0; j < iterations && !queue.isEmpty(); j++) {
					Face<Point2D> currentFace = queue.poll();
					for (Face<Point2D> neighbor : voronoiFaces.getNeighbors(currentFace)) {
						if (!visited.containsKey(neighbor)) {
							visited.put(neighbor, true);
							queue.add(neighbor);
							regionMap.get(i).add(neighbor);
						}
					}
				}

				if (!queue.isEmpty()) {
					allAssigned = false;
				}
			}
		}

		// Check if all faces have been assigned to a region
		int extraRegions = numberOfRegions; 
		for (Face<Point2D> face : faces) {
			if (!visited.containsKey(face)) {
				// Assign the unvisited face to a random region
				int randomRegion = rand.nextInt(numberOfRegions);
				regionMap.computeIfAbsent(extraRegions, k -> new HashSet<>()).add(face);
				extraRegions++; 
			}
		}

		return regionMap;
	}

	// Generate random points
	private Set<Point2D> generateRandomPoints(int numberOfPoints) {
		Set<Point2D> points = new HashSet<>();
		// Seed the random number generator with a constant value
		Random rand = new Random(12345);
		for (int i = 0; i < numberOfPoints; i++) {
			points.add(new Point2D(rand.nextInt(800), rand.nextInt(600)));
		}
		return points;
	}

	// Generate clustered points with Gaussian distribution
	private Set<Point2D> generateClusteredPoints(int numberOfPoints, int numberOfClusters) {
		Set<Point2D> points = new HashSet<>();
		Random rand = new Random(12345);

		// Define cluster centers
		List<Point2D> clusterCenters = new ArrayList<>();
		for (int i = 0; i < numberOfClusters; i++) {
			clusterCenters.add(new Point2D(rand.nextInt(800), rand.nextInt(600)));
		}

		// Generate points around cluster centers using Gaussian distribution
		for (Point2D center : clusterCenters) {
			int cnt = (int) Math.floor(60d * rand.nextGaussian() + 100d);
			for (int i = 0; i < cnt; i++) {
				double x = center.getX() + rand.nextGaussian() * 200; // 50 is the standard deviation
				double y = center.getY() + rand.nextGaussian() * 50; // 50 is the standard deviation
				points.add(new Point2D(x, y));
			}
		}

		return points;
	}

	private void renderDelauneyFaces(Graphics2D g2, Voronoi V) {
		// Retrieve the Delauney faces
		Graph<Face<Point2D>> VoronoiFaces = V.getVoronoiFaces();
		Collection<Face<Point2D>> delauneyFaces = VoronoiFaces.getVertices();

		Random rand = new Random(12345);
		Map<Integer, Set<Face<Point2D>>> regionMap = assignFacesToRegions(VoronoiFaces);
		for (Integer regionId : regionMap.keySet()) {
			for (Face<Point2D> face : regionMap.get(regionId)) {
				// Order the points in CCW order
				List<Point2D> orderedPoints = new ArrayList<>();
				for (Edge<Point2D> edge : face.getEdges()) {
					orderedPoints.add(edge.getSrc());
					orderedPoints.add(edge.getDst());
				}

				// Sort the points based on their angle with respect to the center
				Point2D center = face.getCenter();
				Collections.sort(orderedPoints, (p1, p2) -> {
					double angle1 = Math.atan2(p1.getY() - center.getY(), p1.getX() - center.getX());
					double angle2 = Math.atan2(p2.getY() - center.getY(), p2.getX() - center.getX());
					return Double.compare(angle1, angle2);
				});

				// Create a polygon from the ordered points
				Polygon poly = new Polygon();
				for (Point2D point : orderedPoints) {
					poly.addPoint((int) point.getX() + 100, (int) point.getY() + 100);
				}

				switch (regionId) {
					case 0:
						g2.setColor(Color.GREEN);
						break;
					case 1:
						g2.setColor(Color.BLUE);
						break;
					case 2: 
						g2.setColor(Color.YELLOW); 
					default:
						//g2.setColor(Color.GRAY);
						break;
				}
				g2.fill(poly);
				g2.draw(poly);

				/* 
				for (Face<Point2D> dstFace : VoronoiFaces.getAdjacentVertices(face)) {
					if (dstFace != null) {
						Point2D srcCenter = face.getCenter();
						Point2D dstCenter = dstFace.getCenter();
						g2.setColor(Color.RED);
						g2.drawLine((int) srcCenter.getX() + 100, (int) srcCenter.getY() + 100,
								(int) dstCenter.getX() + 100, (int) dstCenter.getY() + 100);
					}
				}

				g2.setColor(Color.red);
				g2.fillOval((int) face.getCenter().getX() - 10 + Xtranslate, (int) face.getCenter().getY() - 10 +
							Ytranslate, 20, 20);*/
			}
		}

	}

	@Override
	protected void paintComponent(Graphics g) {
		super.paintComponent(g);
		Graphics2D g2 = (Graphics2D) g;

		// Draw Voronoi edges ; debugging stuff
		/*
		 * for (Point2D src : vertices) {
		 * Set<Point2D> neighbors = computedVoronoi.getNeighbors(src);
		 * if (neighbors == null) {
		 * continue;
		 * }
		 * 
		 * for (Point2D dst : neighbors) {
		 * g2.drawLine((int) src.getX() + Xtranslate, (int) src.getY() + Ytranslate,
		 * (int) dst.getX() + Xtranslate,
		 * (int) dst.getY() + Ytranslate);
		 * }
		 * }
		 * 
		 * // Draw points
		 * g2.setColor(Color.RED);
		 * for (Point2D point : points) {
		 * g2.fillOval((int) point.getX() - 10 + Xtranslate, (int) point.getY() - 10 +
		 * Ytranslate, 20, 20);
		 * }
		 */

		// Draw boundary points
		/*
		 * g2.setColor(Color.YELLOW);
		 * for (Point2D point : boundaryPoints) {
		 * g2.fillOval((int) point.getX() - 10 + Xtranslate, (int) point.getY() - 10 +
		 * Ytranslate, 20, 20);
		 * }
		 */
		// Render Delauney faces

		renderDelauneyFaces(g2, this.V);
	}

	// Create and display the JFrame
	private static void createAndShowGUI() {
		System.out.println("Created GUI on EDT? " + SwingUtilities.isEventDispatchThread());
		JFrame frame = new JFrame("Voronoi Diagram");
		MapConstructorDemo panel = new MapConstructorDemo();
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.add(panel);
		frame.setSize(800, 600);
		frame.setVisible(true);
	}

	public static void main(String[] args) {

		SwingUtilities.invokeLater(MapConstructorDemo::createAndShowGUI);

	}
}
