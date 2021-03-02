package laya.game.browser;
import java.io.File;

import android.content.pm.ApplicationInfo;
import android.content.res.AssetManager;
import android.util.Log;
public class GL2JNILib
{
	public static boolean g_bInitialized=false;
	/*static 
	{
		 //System.loadLibrary("turbojpeg");
		 System.loadLibrary("gl2jni");
		 g_bInitialized = true;
	}
	*/
	
	//kuo add code for plugin implement 
	//{{begin
	public static boolean initNativeLibrary(String library,boolean plugin)
	{
		if(!plugin)
		{
			try
			{
				System.loadLibrary(library);
				g_bInitialized = true;
			}
			catch(Exception e)
			{
				e.printStackTrace();
				g_bInitialized=false;
			}
		}
		else
		{
    		try
			{
				System.load(library);
				g_bInitialized=true;
			}
			catch(Exception e)
			{
				g_bInitialized=false;
				e.printStackTrace();
			}
		}
		return g_bInitialized;
	}
	//}}end
	
	//------------------------------------------------------------------------------
	public static int getAndroidSDKVersion() 
	{ 
		int version=0; 
		try 
		{ 
			version = Integer.valueOf(android.os.Build.VERSION.SDK); 
		}
		catch (NumberFormatException e) 
		{ 
			Log.e(e.toString(), null);
		} 
		return version; 
	}
	//------------------------------------------------------------------------------
	//启动前的配置
	public static native void configSetURL(String p_strUrl);
	
	public static native void configSetIsPlug(boolean p_bIsPlug);
	
	//
	public static native void configSetParamExt(String p_strExt);
	
	//初始化
	public static native void InitDLib( AssetManager p_am, String p_strAssetRoot, String p_strCachePath);
	
	//touch
	public static native void handleTouch( int type,int id,int x,int y );

	public static native void handleKeyEvent(int keyCode, int actionType);
	
	public static native void handleJoystickEvent(float THUMBL_xOffset,float THUMBL_yOffset,float THUMBR_xOffset,float THUMBR_yOffset,float LT_Offset,float RT_Offset);
	//初始化下载管理器
	//@param[in] p_bforceLocal
	//@param[in] p_pszRoot  保存下载文件的地方，/data/data/laya.game.browser/cache/LayaCache
	//@param[in] p_am
	//@param[in] p_strAssetRoot 相对于assets目录的路径，可以是网站的hash，现在是固定值
	public static native void InitDownLoadManager( int p_bForceLocal,String p_pszRoot, AssetManager p_am, String p_strAssetRoot );
	
	public static native void SetLocalStoragePath( String p_strLocalStorage );
	 
	//释放gl资源
	public static native void ReleaseDLib();
	
	//退出。（进程不一定退出）
	public static native void OnAppDestroy();
	
	//通知c应用暂停，一般是进入后台。可能需要做一些恢复设备的准备工作
	public static native void OnAppPause();
	 
	//渲染部分的初始化，必须在GLES环境准备好之后再做
	//通知c glcontext已经建立或者已经恢复，需要做一些恢复工作。恢复：shader，texture，FBO...
	public static native void OnGLReady(int width,int height);
	
	//通知app又进入前台了。与上面不同的是不保证context有效了
	public static native void OnAppResume();
	 
	//测试用渲染update
	public static native void onDrawFrame();
	
	public static native void exportObjectToC(String objName, Object expObj );
	
	public static native void exportStaticMethodToC( String packcls, String funName, int p_nKey, String funDesc);
	
	public static native void closeExternalWebView();
	
	public static native void editBoxOnInput( String p_sEvent );
	
	public static native void alertCallback();
	
	public static native void onSensorChanged( float arc );
	
	public static native void reloadJS();
	
	public static native void onRunCmd(int cmd, int param1, int param2 );
	
	//执行一段js代码或者函数
	//参数是一段js代码。
	public static native void RunJS( String jsstring );
	
	//java给runtime发消息。params是一个JSON对象
	public static native void postMsgToRuntime(String msg, String params);
	
	public static native void audioMusicPlayEnd();
	
	public static native void networkChanged( int p_nNetworkType );
	//market 回调-----------------------------------
	public static native void onMarketInit( String p_sTokenDir );
	public static native void loginCallback( String jsonParam);
	public static native void authorizeCallback( String jsonParam);
	public static native void refreshTokenCallback( String jsonParam);
	public static native void rechargeEvent( String jsonParam );
	public static native void inputChange(int keyCode);
	
	public static native void onShareAndFeed( String jsonParam);
	public static native void onGetGameFriends( String jsonParam);
	public static native void onSendToDesktop( String jsonParam );
	public static native void onLogout( String jsonParam );
	public static native void onTopicCircle( String jsonParam );
	public static native void onInviteCallback(String jsonParam);

	public static native void onSwitchUserCallback(String jsonParam);
	public static native void onEnterPlatformCallback(String jsonParam);
	public static native void onEnterBBSCallback(String jsonParam);
	public static native void onEnterFeedbackCallback(String jsonParam);
	public static native void onEnterAccountMgrCallback(String jsonParam);
	public static native void onBuyPropsCallback(String jsonParam);
	public static native void onSetRechargeInfoCallback(String jsonParam);
	public static native void onSendMessageToPlatformCallback(String jsonParam);
	
	public static native void onGetUserInfoCallback(String jsonParam);
	public static native void onGetAvailableLoginTypeCallback(String jsonParam);
}
