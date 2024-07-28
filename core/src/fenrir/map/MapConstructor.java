package fenrir.map;

import org.json.JSONObject;

public class MapConstructor {

	//Here are the geologic and enviormental attributes

	//Here are the political attributes

	//Here are the economic attributes

	//Here are the military attributes


	//Here are the 

	enum MapType {
		ENVIRONMENTAL,
		POLITICAL, 
		ECONOMIC,
		MILITARY
	}


	private GameMap map; 

	// need to add logging here
	public MapConstructor(String mapConfigPath) {
		JSONObject mapConfigParams = null;

		try {
			mapConfigParams = new MapConfig(mapConfigPath).loadConfig();
		} catch (Exception e) {
			System.out.println("Error loading map configuration file, loading default");
			e.printStackTrace();
			mapConfigParams = new MapConfig().loadDefaultConfig(); 
		} finally {
			System.out.println("Map configuration loaded successfully");
		}




		if (mapConfigParams.get('GEOLOGICAL_CONFIG') != null) {
			mapConfigParams.loadDefaultGeologicalConfig(); 
		}

		if (mapConfigParams.get('POLITICAL_CONFIG') != null) {
			mapConfigParams.loadDefaultPoliticalConfig(); 
		}



		switch (mapConfigParams.get("DEFAULT_CREATE_FLAG").toString()) {
		case "CREATE_RANDOM":
			createMap(mapConfigParams);
			break;
		default:
			System.out.println("Invalid flag in map configuration file");
			break;
		}
	}

	private GameMap createMap(JSONObject mapConfigParams) {
		// TODO Auto-generated method stub
		return null;
	}

	private GameMap retrieveMap(JSONObject mapConfigParams) {
		// TODO Auto-generated method stub
		return null;
	}

	public Map getMap(){
		return null; 
	}






}
