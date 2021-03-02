package demo;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.webkit.ValueCallback;

import com.laya.plugin.LayaPluginManager;
import com.laya.plugin.PluginProxy;

public class PluginLauncher {
    private String mOrientation="landscape";
    private boolean mIsFullScreen = true;
    private String mOption="";
    private Activity mMainActivity = null;
    private Handler mHandler = null;
    private LayaPluginManager mPluginManager = null;
	private int mSpID = 0;
    public PluginLauncher(Activity _activity){
        mMainActivity = _activity;
        //mPluginManager = LayaPluginManager.getInstance();
        
    }
    public void initialize(int spid,ValueCallback<Byte> callback){
    	mSpID = spid;
    	//mPluginManager.Initialization(spid,null,mMainActivity,callback);
    }
    //  缓存目录是否存在
    public boolean cacheExist(String pluginName, String gameName)
    {
        return PluginProxy.cacheExist(pluginName, gameName);
    }
    //  删除缓存目录
    public void deleteCache(String pluginName, String gameName)
    {
        PluginProxy.deleteCache(pluginName, gameName);
    }
    //  设置游戏引擎接收的参数
    public void setOption(String key, String value)
    {
        mOption+=key+","+value+";";
    }
    //  设置横竖屏("landscape":横屏,"portrait":竖屏)
    public void setScreenOrientation(String _orientation)
    {
        mOrientation = _orientation;
        //this.m_pluginProxy.setScreenOrientation(_orientation);
    }
    //  设置全屏
    public void setFullScreen(boolean isFull)
    {
        mIsFullScreen = isFull;
        //this.m_pluginProxy.setFullScreen(isFull);
    }
    //  启动引擎 
    public void start(final String pluginName)
    {
        if( mHandler == null )
            mHandler = new Handler(Looper.getMainLooper());
        final String _option = mOption.substring(0, mOption.length()-1);

        LayaPluginManager.getInstance().StopBackgroundDownload(new ValueCallback<Boolean>() {
            @Override
            public void onReceiveValue(Boolean value) {
                mHandler.post(new Runnable() {
                    
                    @Override
                    public void run() {
                        Intent intent = new Intent();
                        intent.setAction ("demo.LayaPluginActivity"); 
                        intent.setFlags (Intent.FLAG_ACTIVITY_NEW_TASK); // required when starting from Application
                        intent.putExtra("Orientation", mOrientation);
                        intent.putExtra("StartPlugin", pluginName);
                        intent.putExtra("Option",_option);
                        intent.putExtra("spid", mSpID);
                        intent.putExtra("IsFullScreen", mIsFullScreen);
                        
                        mMainActivity.startActivity(intent);
                    }
                }); 
            }
        });        
        mOption = "";
    }
	public void onDestroy() {
		if(mPluginManager != null)
			mPluginManager.destroy();
	}

    public void invokeMethod(String method, Bundle parm)
    {
        LayaPluginManager.getInstance().invokeMethod(method,parm);
    }
}
