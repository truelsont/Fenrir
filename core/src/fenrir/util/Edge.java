package fenrir.util;

import java.util.Objects;

public class Edge<T>{

	private T src; 
	private T dst; 
	private boolean undirected; 
		
	public Edge(T src, T dst, boolean undirected) {
		this.src = src; 
		this.dst = dst; 
		this.undirected = undirected; 
	}

	@Override
	public int hashCode() {
		return Objects.hash(dst) + Objects.hash(src);
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
		return (Objects.equals(dst, other.dst) && Objects.equals(src, other.src))
				|| (this.undirected && (Objects.equals(src, other.dst) && Objects.equals(dst, other.src))); 
	}

	public T getSrc() {
		return src;
	}

	public void setSrc(T src) {
		this.src = src;
	}

	public T getDst() {
		return dst;
	}

	public void setDst(T dst) {
		this.dst = dst;
	}
	
	
	
	
}
