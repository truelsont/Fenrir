package fenrir.util.vornoi.beachline;
import fenrir.util.Point;
import fenrir.util.vornoi.*;

import java.util.Optional;
import java.util.stream.Stream;

public class Beachline {

	private final InnerBeachNode rootContainer = new InnerBeachNode();

	public InsertionResult insertArc(Point newSite) {
		BeachNode root = getRoot();
		if (root != null) {
			return root.insertArc(newSite);
		} else {
			LeafBeachNode l = new LeafBeachNode(newSite);
			setRoot(l);
			return new InsertionResult(Optional.empty(), l);
		}
	}

	BeachNode getRoot() {
		return rootContainer.getLeftChild();
	}

	void setRoot(BeachNode n) {
		rootContainer.setLeftChild(n);
	}

	@Override
	public String toString() {
		StringBuilder sb = new StringBuilder();
		sb.append("Beachline(");
		if (getRoot() != null) {
			Optional<LeafBeachNode> current = Optional.of(getRoot().getLeftmostLeaf());
			while (current.isPresent()) {
				sb.append(current.get().getSite()).append(',');
				current = current.flatMap(LeafBeachNode::getRightNeighbor);
			}
		}
		sb.append(")");
		return sb.toString();
	}
}