package com.graphical.fenrir;

import java.util.*;


import com.badlogic.gdx.Game;
import com.badlogic.gdx.graphics.OrthographicCamera;
import com.badlogic.gdx.graphics.glutils.ShapeRenderer;
import com.badlogic.gdx.utils.viewport.FitViewport;

import fenrir.config.*;
import fenrir.map.*;

public final class Fenrir extends Game {
	
	
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
		
		float[] verts = {0,0,0,100,50,50}; 
		Province province = null;
		try {
			province = new Province(verts);
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		Map<String, Object> args = new HashMap<>(); 
		List<Province> provinces = new LinkedList<>(); 
		provinces.add(province); 
		
		
		args.put("provinces",(Object)provinces); 
		
		GameMap gamemap = null; 
		try {
			gamemap = new GameMap(this,args);
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return; 
		}
		
		
		gamemap.render();
		
		
		
		//setScreen(new MainMenu(this)); 

	}

	@Override
	public void resize(int width, int height) {
		viewport.update(width, height);
	}
	
	

	
}








