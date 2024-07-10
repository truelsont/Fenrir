package fenrir.util.vornoi.graph;


import java.util.Collections;
import java.util.HashSet;
import java.util.Optional;
import java.util.Set;
import java.util.stream.Stream;

import fenrir.util.Point;

public class VornoiGraph {

    private final BisectorMap edges = new BisectorMap();
    private final Set<Point> sites = new HashSet<>();

    public void addEdge(VornoiEdge e) {
        edges.put(e.getSite1(), e.getSite2(), e);
    }

    /**
     * Gets the edge that bisects the space between the two sites if there is one.
     *
     * @param a point of the first site
     * @param b point of the second site
     * @return an Optional containing the bisecting edge or an empty Optional if none exist
     */
    public Optional<VornoiEdge> getEdgeBetweenSites(Point a, Point b) {
        return Optional.ofNullable(edges.get(a, b));
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append("Graph[");
        edges.values().stream().map(e -> String.format("(%s,%s),\n", e.getA(), e.getB())).sorted().forEachOrdered(sb::append);
        sb.append("]");
        return sb.toString();
    }

    public void addSite(Point newSite) {
        sites.add(newSite);
    }

    public Set<Point> getSitePoints() {
        return Collections.unmodifiableSet(sites);
    }

    public Stream<VornoiEdge> edgeStream() {
        return edges.stream();
    }
    
    public BisectorMap getEdges() {
    	return edges; 
    }

}