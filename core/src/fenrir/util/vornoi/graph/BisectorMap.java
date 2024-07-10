package fenrir.util.vornoi.graph;


import java.util.Collection;
import java.util.HashMap;
import java.util.Map;
import java.util.stream.Stream;

import fenrir.util.Point;

public class BisectorMap {

    private final Map<Bisector, VornoiEdge> data = new HashMap<>();

    void put(Point a, Point b, VornoiEdge e) {
        assert !data.containsKey(new Bisector(a, b));
        data.put(new Bisector(a, b), e);
    }

    VornoiEdge get(Point a, Point b) {
        return data.get(new Bisector(a, b));
    }

    Collection<VornoiEdge> values() {
        return data.values();
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;

        BisectorMap that = (BisectorMap) o;

        return data.size() == that.data.size() && data.keySet().stream().allMatch(that.data.keySet()::contains);
    }

    @Override
    public int hashCode() {
        return data.hashCode();
    }

    public Stream<VornoiEdge> stream() {
        return data.values().stream();
    }

    private static class Bisector {
        private final Point a, b;
        
        public Bisector(Point a, Point b) {
        	this.a = a; 
        	this.b = b;
        }

        @Override
        public boolean equals(Object o) {
            if (this == o) return true;
            if (o == null || getClass() != o.getClass()) return false;
            Bisector bisector = (Bisector) o;
            return (a.equals(bisector.a) && b.equals(bisector.b)) || (a.equals(bisector.b) && b.equals(bisector.a));
        }

        @Override
        public int hashCode() {
            return a.hashCode() + b.hashCode();
        }
    }
}