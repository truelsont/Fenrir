package fenrir.map;

import javax.swing.*;
import java.awt.*;
import java.awt.geom.Line2D;
import java.util.ArrayList;
import java.util.List;
import java.util.Random;





public class MapConstructor extends JPanel {

    private List<Point> points;
    

    public MapConstructor() {
        points = generateRandomPoints(100); // Generate 10 random points
    }

    // Generate random points
    private List<Point> generateRandomPoints(int numberOfPoints) {
        List<Point> points = new ArrayList<>();
        Random rand = new Random();
        for (int i = 0; i < numberOfPoints; i++) {
            points.add(new Point(rand.nextInt(800), rand.nextInt(600)));
        }
        return points;
    }

    // Draw the points and Voronoi diagram
    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        Graphics2D g2 = (Graphics2D) g;
        g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);



        // Draw Voronoi diagram
        
        g2.setColor(Color.BLACK);
        for (int i = 0; i < getWidth(); i++) {
            for (int j = 0; j < getHeight(); j++) {
                Point closestPoint = null;
                double minDistance = Double.MAX_VALUE;
                for (Point point : points) {
                    double distance = point.distance(i, j);
                    if (distance < minDistance) {
                        minDistance = distance;
                        closestPoint = point;
                    }
                }
                if (closestPoint != null) {
                    g2.draw(new Line2D.Double(i, j, closestPoint.x, closestPoint.y));
                }
            }
        }
        
        g2.setColor(Color.RED);
        for (Point point : points) {
            g2.fillOval(point.x - 10, point.y - 10, 20, 20);
        }
    }
    
    public Pair<List<Point>, List<Pair<Point,<Point>>> createVornoiVertices(List<Point> centroids){
    	List<Point> vornoiVertices = new ArrayList<>();  
    	
    	
    	return vernoiVertices
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
