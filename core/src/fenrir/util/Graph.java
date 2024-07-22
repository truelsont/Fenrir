package fenrir.util;

import java.util.*;

public class Graph<T> {

	private HashMap<T, Set<T>> adjList;
	private HashMap<T, Set<Edge<T>>> pointToEdges; 

	public Graph() {
		this.adjList = new HashMap<>();
		this.pointToEdges = new HashMap<>(); 
		
		assert(false); 
		//need to the do the point to edges stuff 
	}

	public void addVertex(T vertex) {
		adjList.putIfAbsent(vertex, new HashSet<>());
	}

	public void removeVertex(T vertex) {
		adjList.values().forEach(e -> e.remove(vertex));
		adjList.remove(vertex);
	}

	public Set<T> getNeighbors(T vertex) {
		return adjList.getOrDefault(vertex, null);
	}
	
	
	
	public void addEdge(Edge<T> e) {
		if (e == null) {
			return;
		}
		T src = e.getSrc();
		T dst = e.getDst();
		if (src == null || dst == null) {
			return;
		}
		this.addVertex(src);
		this.addVertex(dst);
		
		adjList.get(src).add(dst);
		if (e.isUndirected()) {
			adjList.get(dst).add(src);
		}
	}

	public void addOwnership(T vertex, Edge<T> edge) {
		pointToEdges.putIfAbsent(vertex, new HashSet<Edge<T>>());
		pointToEdges.get(vertex).add(edge);
	}

	public void removeEdge(T source, T destination) {
		Set<T> eV1 = adjList.get(source);
		Set<T> eV2 = adjList.get(destination);
		if (eV1 != null)
			eV1.remove(destination);
		if (eV2 != null)
			eV2.remove(source);
	}
	
	public Set<Edge<T>> edgeRepresentation(){
		Set<Edge<T>> edgeList = new HashSet<>(); 
		
		for(T src: adjList.keySet()) {
			Set<T> dsts = adjList.get(src); 
			for(T dst:dsts) {
				if(dst == src) {continue;}
				Edge<T> e = new Edge<T>(src, dst, false); 
				edgeList.add(e); 
				
			}
			
		}
		return edgeList; 
	}

	public Set<T> getVertices() {
		return adjList.keySet();
	}

	public Set<T> getAdjacentVertices(T vertex) {
		return adjList.get(vertex);
	}

	public boolean hasVertex(T vertex) {
		return adjList.containsKey(vertex);
	}

	public boolean hasEdge(T source, T destination) {
		return adjList.containsKey(source) && adjList.get(source).contains(destination);
	}
}
