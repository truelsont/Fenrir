package fenrir.map;

import javax.swing.*;

import fenrir.util.Point;
import fenrir.util.Vector;
import fenrir.util.vornoi.Vornoi;
import fenrir.util.vornoi.graph.VornoiGraph;

import java.awt.*;
import java.awt.geom.Line2D;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.List;
import java.util.Random;
import java.util.Set; 

public class MapConstructor extends JPanel {

	private Collection<Vector> points;

	public MapConstructor() {
		points = generateRandomPoints(5); // Generate 10 random points
	}

	// Generate random points
	private Collection<Vector> generateRandomPoints(int numberOfPoints) {
		List<Vector> points = new ArrayList<>();
		Random rand = new Random();
		for (int i = 0; i < numberOfPoints; i++) {
			double[] coords = new double[] { rand.nextInt(800), rand.nextInt(600) };
			points.add(new Vector(coords));
		}
		return points;
	}

	// Draw the points and Voronoi diagram
	@Override
	protected void paintComponent(Graphics g) {
		
		Collection<Vector> points = generateRandomPoints(10); 
		
		super.paintComponent(g);
		Graphics2D g2 = (Graphics2D) g;
		g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);

		// Draw Voronoi diagram
		Vornoi vornoi = new Vornoi();
		
		
		
		double[] cornerCoords = new double[] { 0d, 0d };
		double[] screenDims = new double[] { 0d, 0d };
		Vector cornerVector = new Vector(cornerCoords);
		Vector screenVector = new Vector(screenDims);
		Collection<Point> computedVornoi = vornoi.vornoiDiagram(points, cornerVector, screenVector);
		if (computedVornoi == null) {
			return;
		}
		
		for (Point src : vertices) {
			Set<Point> getNeighbors = computedVornoi.getNeighbors(src);
			for (Point dst : vertices) {
				g2.drawLine((int)src.x, (int)src.y, (int)dst.x, (int)dst.y);
			}

		}

		g2.setColor(Color.RED);
		for (Vector point : points) {
			try {
				g2.fillOval((int) point.getValue(0) - 10, (int) point.getValue(1) - 10, 20, 20);
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} finally {
				;
			}
		}
	}

	// Create and display the JFrame
	private static void createAndShowGUI() {
		JFrame frame = new JFrame("Voronoi Diagram");
		MapConstructor panel = new MapConstructor();
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.add(panel);
		frame.setSize(800, 600);
		frame.setVisible(true);
	}

	public static void main(String[] args) {
		SwingUtilities.invokeLater(MapConstructor::createAndShowGUI);
	}
}
