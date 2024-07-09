package fenrir.util.vornoi.beachline;

import fenrir.util.Point;

public class InnerBeachNode extends BeachNode {

	private BeachNode leftChild;
	private BeachNode rightChild;

	InnerBeachNode() {

	}

	public InnerBeachNode(BeachNode leftChild, BeachNode rightChild) {
		setLeftChild(leftChild);
		setRightChild(rightChild);
	}

	@Override
	public InsertionResult insertArc(Point site) {
		// Find leafs represented by this inner node
		Point l = leftChild.getRightmostLeaf().getSite();
		Point r = rightChild.getLeftmostLeaf().getSite();

		// Transform coordinate to local coords
		double lxOld = l.x;
		r = new Point(r.x - l.x, r.y - site.y);
		l = new Point(0, l.y - site.y);

		// Compute intersection of parabolas
		double x;
		if (Double.compare(l.y, r.y) == 0) {
			x = r.x / 2.0;
		} else if (l.y == 0.0) {
			x = l.x;
		} else if (r.y == 0.0) {
			x = r.x;
		} else {
			x = (l.y * r.x - Math.sqrt(l.y * r.y * ((l.y - r.y) * (l.y - r.y)  + (r.x) * (r.x ))) / (l.y - r.y));
		}

		x += lxOld;

		return site.x < x ? leftChild.insertArc(site) : rightChild.insertArc(site);
	}

	@Override
	public LeafBeachNode getLeftmostLeaf() {
		return leftChild.getLeftmostLeaf();
	}

	@Override
	public LeafBeachNode getRightmostLeaf() {
		return rightChild.getRightmostLeaf();
	}

	void setLeftChild(BeachNode leftChild) {
		this.leftChild = leftChild;
		leftChild.setParent(this);
	}

	void setRightChild(BeachNode rightChild) {
		this.rightChild = rightChild;
		rightChild.setParent(this);
	}

	public BeachNode getLeftChild() {
		return leftChild;
	}

	public BeachNode getRightChild() {
		return rightChild;
	}

}