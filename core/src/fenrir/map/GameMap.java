package fenrir.map;




import java.io.Serializable;
import java.util.*; 



import com.badlogic.gdx.graphics.g3d.ModelInstance;
import com.badlogic.gdx.graphics.glutils.ShapeRenderer;
import com.badlogic.gdx.graphics.glutils.ShapeRenderer.ShapeType;
import com.badlogic.gdx.Game;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.graphics.GL20;
import com.badlogic.gdx.graphics.g3d.Model;
import com.badlogic.gdx.math.Vector3;

import fenrir.graphics.Fenrir;



/*
 * This class is the primary controller for the game maps 
 * 
 * 
 * 
 * 
 */
public class GameMap implements Serializable{

	private static final long serialVersionUID = 1L;
	public List<Province> provinces;  
	private Fenrir fenrir; 
	

	public GameMap(){

	}
	
	public void render() {
		
	}

}
	