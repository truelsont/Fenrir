package fenrir.util;


public class Point2D {
	
	double EPSILON = 1e-3; 
	private double x;
    private double y;

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
	
	// noramlzies the smallest coord to 1
	public void extend() {
		if(this.x == 0 && this.y == 0){
			return; 
		}
		
		double factor = 1;//Math.min(Math.abs(x), Math.abs(y)); 
		this.x = this.x / factor;
		this.y = this.y / factor;
		return; 

		
	}

	public double distance(Point2D intersection, boolean normalize) {
		double x = Math.pow(this.x - intersection.x, 2);
		double y = Math.pow(this.y - intersection.y, 2);

		if (normalize) {
			return Math.sqrt(x + y);
		}
		return x + y; 
	}
}