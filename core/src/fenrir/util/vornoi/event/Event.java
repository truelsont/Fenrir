package fenrir.util.vornoi.event;

import java.util.Collection;

import fenrir.util.Point;
import fenrir.util.vornoi.beachline.Beachline;
import fenrir.util.vornoi.graph.VornoiGraph;

public abstract class Event implements Comparable<Event> {

    public Event(Point point) {
		super();
		this.point = point;
	}

	private final Point point; 

    @Override
    public int compareTo(Event o) {
        return Double.compare(o.point.y, point.y);
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        Event event = (Event) o;
        return point.equals(event.point);
    }

    @Override
    public int hashCode() {
        return point.hashCode();
    }

    public abstract void handle(Collection<Event> eventQueue, Beachline beachline, VornoiGraph graph);

	public Point getPoint() {
		return point;
	}


}