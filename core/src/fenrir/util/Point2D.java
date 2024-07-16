package fenrir.util;


public class Point2D {
	
	double EPSILON = 1e-6; 
    public final double x;
    public final double y;

    public Point2D(double x, double y) {
        this.x = x;
        this.y = y;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;

        Point2D point = (Point2D) o;
        return Math.abs(x - point.x) <= EPSILON && Math.abs(y - point.y) <= EPSILON;
    }


    @Override
    public String toString() {
        return String.format("(%.2f,%.2f)", x, y);
    }

	public double getX() {
		return x;
	}

	public double getY() {
		return y;
	}
	
	public static Point2D midpoint(Point2D a, Point2D b) {
		double x = (a.getX() + b.getX()) / 2; 
		double y = (a.getY() + b.getY()) / 2; 
		
		return new Point2D(x, y); 
		
		
	}
}