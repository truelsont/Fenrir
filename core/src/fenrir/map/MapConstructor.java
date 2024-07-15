package fenrir.map;

import javax.swing.*;

import fenrir.util.Graph;
import fenrir.util.Point;
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

public class MapConstructor extends JPanel {

	private Collection<Point> points;

	public MapConstructor() {
		points = generateRandomPoints(100); // Generate 10 random points
	}

	// Generate random points
	private ArrayList<Point> generateRandomPoints(int numberOfPoints) {
		ArrayList<Point> points = new ArrayList<>();
		Random rand = new Random();
		for (int i = 0; i < numberOfPoints; i++) {
			points.add(new Point(rand.nextInt(800), rand.nextInt(600)));
		}
		return points;
	}

	// Draw the points and Voronoi diagram
	@Override
	protected void paintComponent(Graphics g) {
		
		Collection<Point> points = new ArrayList<>(); //this.generateRandomPoints(100); //new ArrayList<>(); 
		points.add(new Point(200,200)); 
		points.add(new Point(100,200)); 
		points.add(new Point(300,200)); 
		points.add(new Point(200,100)); 
		points.add(new Point(200,300)); 
		points.add(new Point(250,375)); 

		super.paintComponent(g);
		Graphics2D g2 = (Graphics2D) g;
		g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
		
	
		
		

		VornoiDelauney V = new VornoiDelauney(points); 
		Point origin = new Point(0, 0);
		Point dims = new Point(800, 600);
		Graph<Point> computedVornoi = V.retrieveGraph(origin, dims); 
		if (computedVornoi == null) {
			return;
		}

		for (Point src : computedVornoi.getVertices()) {
			Set<Point> neighbors = computedVornoi.getNeighbors(src);
			if (neighbors == null) {
				continue;
			}

			for (Point dst : neighbors) {


				g2.drawLine((int) src.x, (int) src.y, (int) dst.x, (int) dst.y);
			}

		}

		g2.setColor(Color.RED);
		for (Point point : points) {
			g2.fillOval((int) point.x - 10, (int) point.y - 10, 20, 20);
			
		}
		
		g2.setColor(Color.BLUE);
		Set<Triangle> tris = BowyerWatson.bowyerWatson(points); 
        for (Triangle tri : tris) {
            g2.drawLine((int) tri.getP1().getX(), (int) tri.getP1().getY(),
                        (int) tri.getP2().getX(), (int) tri.getP2().getY());
            g2.drawLine((int) tri.getP2().getX(), (int) tri.getP2().getY(),
                        (int) tri.getP3().getX(), (int) tri.getP3().getY());
            g2.drawLine((int) tri.getP3().getX(), (int) tri.getP3().getY(),
                        (int) tri.getP1().getX(), (int) tri.getP1().getY());
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
