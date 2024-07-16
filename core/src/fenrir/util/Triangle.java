package fenrir.util;

import java.util.ArrayList;
import java.util.List;

public class Triangle {
	Point2D p1, p2, p3;
	// Constructors, getters, and other methods...

	public Triangle(Point2D p1, Point2D p2, Point2D p3) {
		this.p1 = p1;
		this.p2 = p2;
		this.p3 = p3;
	}

	public boolean contains(Point2D p) {
		double alpha = ((p2.getY() - p3.getY()) * (p.getX() - p3.getX())
				+ (p3.getX() - p2.getX()) * (p.getY() - p3.getY()))
				/ ((p2.getY() - p3.getY()) * (p1.getX() - p3.getX())
						+ (p3.getX() - p2.getX()) * (p1.getY() - p3.getY()));
		double beta = ((p3.getY() - p1.getY()) * (p.getX() - p3.getX())
				+ (p1.getX() - p3.getX()) * (p.getY() - p3.getY()))
				/ ((p2.getY() - p3.getY()) * (p1.getX() - p3.getX())
						+ (p3.getX() - p2.getX()) * (p1.getY() - p3.getY()));
		double gamma = 1.0 - alpha - beta;
		return alpha > 0 && beta > 0 && gamma > 0;
	}

	public boolean containsVertex(Point2D p) {
		return p1.equals(p) || p2.equals(p) || p3.equals(p);
	}
	
    public static boolean isCCW(Point2D p1, Point2D p2, Point2D p3) {
        double det = (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
        return det > 0;
    }
	

    public Point2D getCircumcenter() {
        double dA = p1.getX() * p1.getX() + p1.getY() * p1.getY();
        double dB = p2.getX() * p2.getX() + p2.getY() * p2.getY();
        double dC = p3.getX() * p3.getX() + p3.getY() * p3.getY();

        double aux1 = (dA * (p3.getY() - p2.getY()) + dB * (p1.getY() - p3.getY()) + dC * (p2.getY() - p1.getY()));
        double aux2 = -(dA * (p3.getX() - p2.getX()) + dB * (p1.getX() - p3.getX()) + dC * (p2.getX() - p1.getX()));
        double div = (2 * (p1.getX() * (p3.getY() - p2.getY()) + p2.getX() * (p1.getY() - p3.getY()) + p3.getX() * (p2.getY() - p1.getY())));

        if (div == 0) {
            return null; // Points are collinear
        }

        double centerX = aux1 / div;
        double centerY = aux2 / div;
        return new Point2D(centerX, centerY);
    }

	public boolean isInsideCircumcircle(Point2D p) {
		double ax = p1.getX() - p.getX();
		double ay = p1.getY() - p.getY();
		double bx = p2.getX() - p.getX();
		double by = p2.getY() - p.getY();
		double cx = p3.getX() - p.getX();
		double cy = p3.getY() - p.getY();

		double det_ab = ax * by - ay * bx;
		double det_bc = bx * cy - by * cx;
		double det_ca = cx * ay - cy * ax;

		double a_squared = ax * ax + ay * ay;
		double b_squared = bx * bx + by * by;
		double c_squared = cx * cx + cy * cy;

		double determinant = a_squared * det_bc + b_squared * det_ca + c_squared * det_ab;

		return isCCW(p1,p2,p3) && determinant > 0;
	}

	public List<Edge<Point2D>> getEdges() {
		List<Edge<Point2D>> edges = new ArrayList<>();
		edges.add(new Edge<Point2D>(p1, p2, true));
		edges.add(new Edge<Point2D>(p2, p3, true));
		edges.add(new Edge<Point2D>(p3, p1, true));
		return edges;
	}

	public Point2D getP1() {
		return p1;
	}

	public void setP1(Point2D p1) {
		this.p1 = p1;
	}

	public Point2D getP2() {
		return p2;
	}

	public void setP2(Point2D p2) {
		this.p2 = p2;
	}

	public Point2D getP3() {
		return p3;
	}

	public void setP3(Point2D p3) {
		this.p3 = p3;
	}

}