package fenrir.util.vornoi.beachline;
import java.util.Optional; 

public class InsertionResult {
    public InsertionResult(Optional<LeafBeachNode> splitLeaf, LeafBeachNode newLeaf) {
		super();
		this.splitLeaf = splitLeaf;
		this.newLeaf = newLeaf;
	}
	public Optional<LeafBeachNode> splitLeaf;
    public LeafBeachNode newLeaf;
    
}
