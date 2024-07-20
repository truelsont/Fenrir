
package fenrir.map;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import org.json.JSONObject;

public class MapConfig {

	// The path to the configuration file
	private String configFilePath;

	// Default values for the map configuration
	private String DEFAULT_CREATE_FLAG = "RANDOM_CREATE"; 
	private int DEFAULT_WIDTH = 800;
	private int DEFAULT_HEIGHT = 600;
	private int DEFAULT_POINT_COUNT = DEFAULT_WIDTH * DEFAULT_HEIGHT / 10000;
	private String DEFAULT_OUTPUT_PATH = "assets/maps/";

	public MapConfig(String configFilePath) {
		this.configFilePath = configFilePath;
	}

	private String readFile(String filename) throws IOException {
		return new String(Files.readAllBytes(Paths.get(filename)));
	}
	
	private JSONObject parseJson(String json) {
		return new JSONObject(json);
	}

	// This method sets default values for the map configuration if they are not		
	// present in the configuration file.
	private void setDefaultValues(JSONObject mapJson) {
		
		if (!mapJson.has("createFlag")) {
			mapJson.put("creatFlag", this.DEFAULT_CREATE_FLAG);
		}
		if (!mapJson.has("width")) {
			mapJson.put("width", this.DEFAULT_WIDTH);
			mapJson.put("height", this.DEFAULT_HEIGHT);
		}

		if (!mapJson.has("pointCount")) {
			mapJson.put("pointCount", this.DEFAULT_POINT_COUNT);
		}

		if (!mapJson.has("outputPath")) {
			mapJson.put("outputPath", this.DEFAULT_OUTPUT_PATH);
		}
	}

	/**
	 * This method loads the map configuration from the configuration file and
	 * returns it as a JSON object. If the configuration file is missing some
	 * values, it sets default values for them.
	 * 
	 * @return The JSON object representing the map configuration
	 * @throws IOException If an error occurs while reading the configuration file
	 */
	public JSONObject loadConfig() throws IOException {
		JSONObject mapJson = null;
		String configContent = readFile(this.configFilePath);
		JSONObject configJson = parseJson(configContent);
		String mapConfigPath = configJson.getString("mapConfig");

		String mapContent = readFile(mapConfigPath);
		mapJson = parseJson(mapContent);

		setDefaultValues(mapJson);
		return mapJson;
	}
	
	public JSONObject getDefaultConfig() {
		JSONObject mapJson = new JSONObject();
		setDefaultValues(mapJson); 
		return mapJson;
	}
}
