package laya.game.PlatformInterface;

public class LayaPlatformFactory 
{
	static private LayaPlatformFactory sInstance;
	static public LayaPlatformFactory GetInstance()
	{
		if(sInstance == null) sInstance = new LayaPlatformFactory();
		return sInstance;
	}
	public LayaPlatformInterface CreateInterface(String className) 
	{
		LayaPlatformInterface curInstance = null;
		try 
		{
			curInstance = (LayaPlatformInterface) Class.forName(className).newInstance();
		} 
		catch (Exception e) 
		{
			e.printStackTrace();
		} 
		return curInstance;
	}
}
