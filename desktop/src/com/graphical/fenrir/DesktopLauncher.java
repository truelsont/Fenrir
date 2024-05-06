package com.graphical.fenrir;

import com.badlogic.gdx.backends.lwjgl3.Lwjgl3Application;
import com.badlogic.gdx.backends.lwjgl3.Lwjgl3ApplicationConfiguration;
import com.graphical.fenrir.*; 

// Please note that on macOS your application needs to be started with the -XstartOnFirstThread JVM argument
public class DesktopLauncher {
	public static void main (String[] arg) {
		Lwjgl3ApplicationConfiguration config = new Lwjgl3ApplicationConfiguration();
		
		
		config.setForegroundFPS(60);
		config.setTitle("Fenrir");
		config.setWindowedMode(880,400); 
		config.useVsync(true); 
		new Lwjgl3Application(new Fenrir(), config);
	}
}
