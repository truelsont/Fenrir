package fenrir.map;

import javax.swing.*;

import fenrir.util.BoundingBox;
import fenrir.util.Edge;
import fenrir.util.Face;
import fenrir.util.Graph;
import fenrir.util.Point2D;
import fenrir.util.Triangle;
import fenrir.util.Vector;
import fenrir.util.vornoi.*;

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



public class MapConstructorDemo extends JPanel {

	private Collection<Point2D> points;

	public MapConstructorDemo() {
		points = generateRandomPoints(100); // Generate 10 random points
	}

	// Generate random points
	private ArrayList<Point2D> generateRandomPoints(int numberOfPoints) {
	    ArrayList<Point2D> points = new ArrayList<>();
	    // Seed the random number generator with a constant value
	    Random rand = new Random(12345);
	    for (int i = 0; i < numberOfPoints; i++) {
	        points.add(new Point2D(rand.nextInt(800), rand.nextInt(600)));
	    }
	    return points;
	}
	

private void renderDelauneyFaces(Graphics2D g2, VornoiDelauney V) {
    // Retrieve the Delauney faces
	Map<Point2D, Face<Point2D>> vornoiFaces = V.getVornoiFaces(); 
    Collection<Face<Point2D>> delauneyFaces = vornoiFaces.values(); 

    Random rand = new Random(12345);

    for (Face<Point2D> face : delauneyFaces) {
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

        // Generate a random color
        Color color = new Color(rand.nextInt(256), rand.nextInt(256), rand.nextInt(256));
        g2.setColor(color);
        g2.fill(poly);
        g2.setColor(Color.BLACK);
        g2.draw(poly);
    }
}
	
	

	// Draw the points and Voronoi diagram
	@Override
	protected void paintComponent(Graphics g) {
		int Xtranslate = 100; 
		int Ytranslate = 100;
		
		Collection<Point2D> points = this.generateRandomPoints(10000); // new ArrayList<>();
		super.paintComponent(g);
		Graphics2D g2 = (Graphics2D) g;
		g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
		BoundingBox<Point2D> bounds = new BoundingBox<>(new Point2D(0, 0), new Point2D(800, 600)); 
		VornoiDelauney V = new VornoiDelauney(points, bounds);
		Graph<Point2D> computedVornoi = V.getGraph();
		if (computedVornoi == null) {
			return;
		}
		/*

		for (Point2D src : computedVornoi.getVertices()) {
			Set<Point2D> neighbors = computedVornoi.getNeighbors(src);
			if (neighbors == null) {
				continue;
			}

			for (Point2D dst : neighbors) {

				g2.drawLine((int) src.getX() + Xtranslate, (int) src.getY() + Ytranslate, (int) dst.getX() + Xtranslate, (int) dst.getY() + Ytranslate);
			}

		}
		
		

		g2.setColor(Color.RED);
		for (Point2D point : points) {
			g2.fillOval((int) point.getX() - 10 + Xtranslate, (int) point.getY() - 10 + Ytranslate, 20, 20);

		}
		
	    g2.setColor(Color.YELLOW);
	    for (Point2D point : V.boundaryPoints) {
	        g2.fillOval((int) point.getX() - 10 + Xtranslate, (int) point.getY() - 10 + Ytranslate, 20, 20);
	    }*/
		
		
		
		renderDelauneyFaces(g2, V);

	}

	// Create and display the JFrame
	private static void createAndShowGUI() {
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
