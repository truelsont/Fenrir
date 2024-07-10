package fenrir.util;

import java.util.Objects;

public class Edge<T>{

	private T src; 
	private T dst; 
		
	public Edge(T src, T dst) {
		this.src = src; 
		this.dst = dst; 
	}

	@Override
	public int hashCode() {
		return Objects.hash(dst, src);
	}

	@Override
	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		Edge other = (Edge) obj;
		return Objects.equals(dst, other.dst) && Objects.equals(src, other.src);
	}
	
	
	
	
}
