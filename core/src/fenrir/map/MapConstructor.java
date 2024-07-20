package fenrir.map;

import org.json.JSONObject;

public class MapConstructor {

	// need to add logging here
	public MapConstructor(String mapConfigPath) {
		JSONObject mapConfigParams = null;

		try {
			mapConfigParams = new MapConfig(mapConfigPath).loadConfig();
		} catch (Exception e) {
			System.out.println("Error loading map configuration file, loading default");
			e.printStackTrace();
			mapConfigParams = new MapConfig(mapConfigPath).getDefaultConfig();
		} finally {
			System.out.println("Map configuration loaded successfully");
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

}
