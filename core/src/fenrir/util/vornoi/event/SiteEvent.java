package fenrir.util.vornoi.event;

import java.util.Collection;

import fenrir.util.Point;
import fenrir.util.vornoi.beachline.*;
import fenrir.util.vornoi.graph.VornoiEdge;
import fenrir.util.vornoi.graph.VornoiGraph;

public class SiteEvent extends Event {

    public SiteEvent(Point point) {
        super(point);
    }

    @Override
    public void handle(Collection<Event> eventQueue, Beachline beachline, VornoiGraph graph) {
        InsertionResult result = beachline.insertArc(getPoint());
        result.splitLeaf.ifPresent(l -> graph.addEdge(new VornoiEdge(l.getSite(), getPoint())));
        result.splitLeaf.ifPresent(l -> l.getSubscribers().forEach(eventQueue::remove));
        result.newLeaf.addCircleEvents(eventQueue::add, getPoint().y);
    }
}