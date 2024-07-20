package fenrir.util;

import fenrir.util.Point2D;

public class BoundingBox<T> {

	private T topCorner;
	private T dimensions;

	public BoundingBox(T topCorner, T dimensions) {
		this.topCorner = topCorner;
		this.dimensions = dimensions;
	}

	public boolean contains(T point) {
		if (point == null) {
			return false;
		}

		if (point.getClass() != topCorner.getClass() || point.getClass() != dimensions.getClass()) {
			return false;
		}

		if (point instanceof Point2D) {
			Point2D p = (Point2D) point;
			return p.getX() >= ((Point2D) topCorner).getX()
					&& p.getX() <= ((Point2D) topCorner).getX() + ((Point2D) dimensions).getX()
					&& p.getY() >= ((Point2D) topCorner).getY()
					&& p.getY() <= ((Point2D) topCorner).getY() + ((Point2D) dimensions).getY();
		}
		return false;
	}

	public T getTopCorner() {
		return topCorner;
	}

	public void setTopCorner(T topCorner) {
		this.topCorner = topCorner;
	}

	public T getDimensions() {
		return dimensions;
	}

	public void setDimensions(T dimensions) {
		this.dimensions = dimensions;
	}
	
	

}
