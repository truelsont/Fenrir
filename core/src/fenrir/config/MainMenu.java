package fenrir.config;

import com.badlogic.gdx.Game;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.Screen;
import com.badlogic.gdx.graphics.GL20;
import com.badlogic.gdx.graphics.OrthographicCamera;
import com.badlogic.gdx.graphics.Pixmap;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;


public class MainMenu implements Screen {

    private Game game;
    private OrthographicCamera camera;
    private SpriteBatch batch;
    private Texture background;

    public MainMenu(Game game) {
        this.game = game;
    }

    @Override
    public void show() {
        camera = new OrthographicCamera();
        camera.setToOrtho(false, 800, 480);
        batch = new SpriteBatch();
        
        Pixmap pixmap = new Pixmap(800, 480, Pixmap.Format.RGBA8888);
        pixmap.setColor(0, 0, 0, 1); // Set color to black
        pixmap.fill(); // Fill the Pixmap with the black color

        // Create a Texture from the Pixmap
        background = new Texture(pixmap);

        // Dispose the Pixmap to free up memory
        pixmap.dispose();
    }

    @Override
    public void render(float delta) {
        Gdx.gl.glClearColor(0, 0, 0, 1); // Clear the screen with black color
        Gdx.gl.glClear(GL20.GL_COLOR_BUFFER_BIT);

        camera.update();
        batch.setProjectionMatrix(camera.combined);

        batch.begin();
        batch.draw(background, 0, 0, 800, 480); // Draw the black background
        // Draw menu items, buttons, etc.
        batch.end();

        if (Gdx.input.isTouched()) {
           // game.setScreen(new GameScreen(game)); // Switch to GameScreen when touched
           // dispose(); // Dispose resources
        }
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
