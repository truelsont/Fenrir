package fenrir.map;

import javax.swing.*;

import fenrir.util.BoundingBox;
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
import java.util.LinkedList;
import java.util.List;
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

	// Draw the points and Voronoi diagram
	@Override
	protected void paintComponent(Graphics g) {

		Collection<Point2D> points = this.generateRandomPoints(100); // new ArrayList<>();
		super.paintComponent(g);
		Graphics2D g2 = (Graphics2D) g;
		g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
		BoundingBox<Point2D> bounds = new BoundingBox<>(new Point2D(0, 0), new Point2D(800, 600)); 
		VornoiDelauney V = new VornoiDelauney(points, bounds);
		Graph<Point2D> computedVornoi = V.getGraph();
		if (computedVornoi == null) {
			return;
		}

		for (Point2D src : computedVornoi.getVertices()) {
			Set<Point2D> neighbors = computedVornoi.getNeighbors(src);
			if (neighbors == null) {
				continue;
			}

			for (Point2D dst : neighbors) {

				g2.drawLine((int) src.getX(), (int) src.getY(), (int) dst.getX(), (int) dst.getY());
			}

		}

		g2.setColor(Color.RED);
		for (Point2D point : points) {
			g2.fillOval((int) point.getX() - 10, (int) point.getY() - 10, 20, 20);

		}

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
