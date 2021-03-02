package com.layabox.config;

import java.io.IOException;
import java.io.InputStream;
import java.util.Properties;

public class config 
{
	private static config ms_config = null;
	public String	m_sLayaHallUrl = null;

	public static config GetInstance()
	{
		if( ms_config == null )
		{
			ms_config = new config();
		}
		return ms_config;
	}

	public static void  DelInstance()
	{
		ms_config=null;
	}
	public config()
	{
		
	}
	public boolean init( InputStream pInputStream )
	{
		Properties pro=new Properties();
		try {
			if(pInputStream==null)
				return false;
			pro.load(pInputStream);
			m_sLayaHallUrl=pro.getProperty("LayaHallUrl");
			return true;
		} catch (IOException e) {
			e.printStackTrace();
			return false;
		}
	}
}
