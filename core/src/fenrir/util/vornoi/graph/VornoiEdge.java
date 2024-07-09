package fenrir.util.vornoi.graph;

import fenrir.util.Point;

public class VornoiEdge {
    private Point site1, site2;
    private Vertex a, b;
    
    public VornoiEdge(Point site1, Point site2) {
    	this.site1 = site1; 
    	this.site2 = site2; 
    }

    public void addVertex(Vertex v) {
        if (a == null) {
            a = v;
        } else if (b == null) {
            b = v;
        } else {
            throw new IllegalStateException("Trying to set a third vertex on an edge");
        }
    }

    @Override
    public String toString() {
        return "Edge("+a+", "+b+")";
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;

        VornoiEdge edge = (VornoiEdge) o;

        if (!site1.equals(edge.site1)) return false;
        if (!site2.equals(edge.site2)) return false;
        if (a != null ? !a.equals(edge.a) : edge.a != null) return false;
        return b != null ? b.equals(edge.b) : edge.b == null;
    }

    @Override
    public int hashCode() {
        int result = site1.hashCode();
        result = 31 * result + site2.hashCode();
        result = 31 * result + (a != null ? a.hashCode() : 0);
        result = 31 * result + (b != null ? b.hashCode() : 0);
        return result;
    }

	public Vertex getA() {
		return a;
	}

	public void setA(Vertex a) {
		this.a = a;
	}

	public Vertex getB() {
		return b;
	}

	public void setB(Vertex b) {
		this.b = b;
	}

	public Point getSite1() {
		return site1;
	}

	public Point getSite2() {
		return site2;
	}
}