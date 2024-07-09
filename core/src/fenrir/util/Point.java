package fenrir.util;

import static fenrir.util.vornoi.Math.EPSILON;
import static fenrir.util.vornoi.Math.PRECISION;
import static java.lang.Math.abs;

public class Point {

    public final double x;
    public final double y;

    public Point(double x, double y) {
        this.x = x;
        this.y = y;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;

        Point point = (Point) o;
        return abs(x - point.x) <= EPSILON && abs(y - point.y) <= EPSILON;
    }

    @Override
    public int hashCode() {
        return (int) (x * PRECISION * 31) + (int) (y * PRECISION);
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
}