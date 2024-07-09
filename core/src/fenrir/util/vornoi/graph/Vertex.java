package fenrir.util.vornoi.graph;

import fenrir.util.Point;

public class Vertex {
    private Point location;
    
    public Vertex(Point location) {
    	this.location = location; 
    }
    
    
    @Override
    public String toString() {
        return location.toString();
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;

        Vertex vertex = (Vertex) o;

        return location.equals(vertex.location);
    }

    @Override
    public int hashCode() {
        return location.hashCode();
    }
}