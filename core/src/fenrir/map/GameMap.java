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
import com.graphical.fenrir.Fenrir;

public class GameMap implements Serializable{

	private static final long serialVersionUID = 1L;
	public List<Province> provinces;  
	private Fenrir fenrir; 
	

	@SuppressWarnings("unchecked")
	public GameMap(Fenrir fenrir, Map<String,Object > args) throws Exception {
		if(fenrir == null) {
			throw new Exception("need an instance of fenrir"); 
		}
		
		if(args != null) {
			
			//retrieves the provinces for the map
			if(args.containsKey("provinces")) {
				Object provinces = args.get("provinces"); 
				if(provinces instanceof List<?>) {
					List<?> tempList = (List<?>) provinces; 
					for(Object obj:tempList) {
						if(!(obj instanceof Province)) {
							throw new Exception("provinces arg can only contain provinces types"); 
						}
					}
				}else {
					throw new Exception("provinces arg need to be a List<Province> type"); 
				}
				
				this.provinces = (List<Province>) args.get("provinces"); 
			}
			
		}
		
		this.fenrir = fenrir; 
	}
	
	public void render() {
		
		Gdx.gl.glClear(GL20.GL_COLOR_BUFFER_BIT);
		ShapeRenderer shapeRenderer = this.fenrir.shape; 
		shapeRenderer.setProjectionMatrix(this.fenrir.camera.combined); 
		for(Province province:provinces) {	
			province.render(shapeRenderer);
		}
	}
	
	
	
	

	
}
	