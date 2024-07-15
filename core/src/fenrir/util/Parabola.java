package fenrir.util;

public class Parabola {
	public double a,b,c; 

	public Parabola(Point focus, double directrixY) {
		this.a = focus.getX();
		this.b = focus.getY(); 
		this.c = directrixY ;
	}

}
