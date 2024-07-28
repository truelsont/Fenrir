package fenrir.map;
import java.io.*;
import java.util.*;

public class Province implements Serializable {


	private static final long serialVersionUID = 1L;
	String provinceName; 
	ProvinceEnvType provinceEnvType; 

	public enum ProvinceEnvType{
		OCEAN, 
		COAST, 
		LAND,
	}

	public Province(String ProvinceName, ProvinceEnvType enviromentalType) {
		this.provinceName = ProvinceName; 	
		this.provinceEnvType = enviromentalType; 
	}

}
