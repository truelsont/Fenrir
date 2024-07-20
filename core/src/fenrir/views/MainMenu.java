package fenrir.views;

import com.badlogic.gdx.Game;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.Screen;
import com.badlogic.gdx.graphics.GL20;
import com.badlogic.gdx.graphics.OrthographicCamera;
import com.badlogic.gdx.graphics.Pixmap;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.graphics.glutils.ShapeRenderer.ShapeType;

import fenrir.graphics.Fenrir;


public class MainMenu implements Screen {

    private Fenrir fenrir;
    private OrthographicCamera camera;
    private SpriteBatch batch;
    private Texture background;

    public MainMenu(Fenrir fenrir) {
        this.fenrir = fenrir;
    }

    @Override
    public void show() {
        
    }

    @Override
    public void render(float delta) {
    	//fenrir.shape.setProjectionMatrix(fenrir.camera.combined); 
    	//fenrir.shape.begin(ShapeType.Line); 
    	//fenrir.shape.rect(0,0,100,100); 
    	//fenrir.shape.end(); 
    	
		 
    }

    @Override
    public void resize(int width, int height) {
    }

    @Override
    public void pause() {
    }

    @Override
    public void resume() {
    }

    @Override
    public void hide() {
    }

    @Override
    public void dispose() {
        batch.dispose();
        background.dispose();
    }
}
