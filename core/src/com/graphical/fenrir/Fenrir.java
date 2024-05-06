package com.graphical.fenrir;

import com.badlogic.gdx.Game;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.utils.ScreenUtils;
import fenrir.config.*;

public class Fenrir extends Game{
	SpriteBatch batch;
	Texture img;
	
	@Override
	public void create () {
		setScreen(new MainMenu(this)); 
	}

}
