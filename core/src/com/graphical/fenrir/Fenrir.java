package com.graphical.fenrir;

import com.badlogic.gdx.Game;
import com.badlogic.gdx.graphics.OrthographicCamera;
import com.badlogic.gdx.graphics.glutils.ShapeRenderer;
import com.badlogic.gdx.utils.viewport.FitViewport;

import fenrir.config.*;

public class Fenrir extends Game {
	public final static float WIDTH = 800;
	public final static float HEIGHT = (16f / 9f) * WIDTH;

	public FitViewport viewport;
	public OrthographicCamera camera;
	public ShapeRenderer shape;

	@Override
	public void create() {
		// setScreen(new MainMenu(this));
		shape = new ShapeRenderer();
		camera = new OrthographicCamera();
		viewport = new FitViewport(WIDTH, HEIGHT, camera);
		
		
		setScreen(new MainMenu(this)); 

	}

	@Override
	public void resize(int width, int height) {
		viewport.update(width, height);
		
	}

	
}








