package fenrir.strategic;

import com.badlogic.gdx.graphics.g2d.Sprite;
import com.badlogic.gdx.math.Vector3;

public abstract class StratEntity extends Sprite{
	
	
	public int id; 
	
	public Vector3 position = new Vector3(); 
	public Vector3 velocity = new Vector3();
	public Vector3 orientation = new Vector3(); 
	
	
	public StratEntity() {
		// TODO Auto-generated constructor stub
	}
	
	public void draw() {
	
	}

}
