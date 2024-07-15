package fenrir.util;

public class Vector {
	private final double[] elements;

	public Vector(double[] elements) {
		this.elements = elements.clone();
	}

	public int getDim() throws Exception {
		if (this.elements == null || this.elements.length < 1) {
			throw new Exception("Can't have an empty or null array");
		}
		return this.elements.length;
	}

	private void validateSameLength(Vector other) throws Exception {
		if (this.elements.length != other.elements.length) {
			throw new Exception("Vectors must be same length");
		}
	}

	public Vector subtract(Vector other) throws Exception {
		validateSameLength(other);
		int n = this.getDim();
		double[] result = new double[n];

		for (int i = 0; i < n; i++) {
			result[i] = this.elements[i] - other.elements[i];
		}

		return new Vector(result);

	}

	public Vector add(Vector other) throws Exception {
		validateSameLength(other);
		int n = this.getDim();
		double[] result = new double[n];

		for (int i = 0; i < n; i++) {
			result[i] = this.elements[i] + other.elements[i];
		}

		return new Vector(result);

	}

	public double dotProduct(Vector other) throws Exception {
		validateSameLength(other);
		int n = this.getDim();
		double result = 0;

		for (int i = 0; i < n; i++) {
			double prod = this.elements[i] * other.elements[i];
			result += prod * prod;
		}

		return result;

	}

	public static double euclideanDistance(Vector a, Vector b, boolean noSqrt) throws Exception {
		// don't use methods slows it down ; unless want to inline
		a.validateSameLength(b);
		double dotProduct = 0d;

		int n = a.getDim();
		for (int i = 0; i < n; i++) {
			double prod = a.elements[i] - b.elements[i];
			dotProduct += prod * prod;
		}

		if (noSqrt) {
			return dotProduct;
		}
		return Math.sqrt(dotProduct);
		// maybe do the quake hack?
	}

	public static double manhattanDistance(Vector a, Vector b) throws Exception {
		// don't use methods slows it down ; unless want to inline
		a.validateSameLength(b);
		double dist = 0d;

		int n = a.getDim();
		for (int i = 0; i < n; i++) {
			double diff = a.elements[i] - b.elements[i];
			dist += Math.abs(diff);
		}

		return dist;

	}
	
	public Point toPoint() {
		if (this.elements.length != 2){
			return null; 
		}
		
		return new Point(elements[0], elements[1]); 
	}

	public String toString() {
		StringBuilder sb = new StringBuilder();
		sb.append("[");
		for (int i = 0; i < elements.length; i++) {
			sb.append(elements[i]);
			if (i < elements.length - 1) {
				sb.append(", ");
			}
		}
		sb.append("]");
		return sb.toString();
	}
	
	public double getValue(int axis) throws Exception {
		if(axis >= this.elements.length) {
			throw new Exception("axis must be size of dims"); 
		}
		
		return this.elements[axis]; 
	}
}
