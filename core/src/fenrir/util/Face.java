package fenrir.util;

import java.util.HashSet;
import java.util.Set;

public class Face<T> {
    private final T center;
    private Set<Edge<T>> edges;
    private Set<Face<T>> neighbors;
    private Set<T> vertices;

    public Face(T center) {
    	this.center = center;
        this.edges = new HashSet<>();
        this.neighbors = new HashSet<>();
        this.vertices = new HashSet<>();
    }
    
   
    
    // Getters and Setters
    public T getCenter() {
        return center;
    }

    public Set<Edge<T>> getEdges() {
        return edges;
    }

    public void addEdge(Edge<T> edge) {
        this.edges.add(edge);
        if (edge.getSrc() != null) {
            this.vertices.add(edge.getSrc());
        }
        if (edge.getDst() != null) {
            this.vertices.add(edge.getDst());
        }
    }
    
    public boolean isConnected() {
    	assert(false); 
    	return false; 
    }

    public Set<Face<T>> getNeighbors() {
        return neighbors;
    }

    public void addNeighbor(Face<T> neighbor) {
        this.neighbors.add(neighbor);
    }
    
	public Set<T> getVertices() {
		return vertices;
	}


}
