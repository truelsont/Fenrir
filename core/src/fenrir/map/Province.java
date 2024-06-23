package fenrir.map;
import java.io.*;

import com.badlogic.gdx.graphics.glutils.ShapeRenderer;
import com.badlogic.gdx.graphics.glutils.ShapeRenderer.ShapeType;

public class Province implements Serializable {

	
	private static final long serialVersionUID = 1L;
	String provinceName; 
	long provinceOwner; 
	long provinceOccupier;
	
	private float[] vertices;
	private int[] color;

	public Province(float [] vertices) throws Exception {
		if(vertices == null || vertices.length < 3) {
			throw new Exception("Province needs proper args"); 
		}
		
		this.vertices = vertices; 
		this.color = new int[]{0,0,0,0};  
		}
	
	public void render(ShapeRenderer shapeRenderer) {
		shapeRenderer.end(); 
		shapeRenderer.begin(ShapeType.Line); 
		shapeRenderer.setColor(255,0,0,0);
		shapeRenderer.polygon(this.vertices);
		shapeRenderer.end(); 
	}
	

}
