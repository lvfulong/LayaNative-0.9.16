package com.layabox.layaboxbrowser;

import android.content.ContentResolver;
import android.content.Intent;
import android.content.SharedPreferences;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.webkit.ValueCallback;
import android.widget.FrameLayout;
import android.widget.Toast;

//import com.google.android.gms.appindexing.Action;
//import com.google.android.gms.appindexing.AppIndex;
//import com.google.android.gms.appindexing.Thing;
//import com.google.android.gms.common.api.GoogleApiClient;
import com.laya.iexternalinterface.IPluginListener;
import com.laya.plugin.ILayaBoxPlugin;
import com.laya.plugin.LayaPluginManager;
import com.layabox.browser.IWebView;
import com.layabox.browser.IWebViewListener;
import com.layabox.browser.WebViewFactory;
import com.layabox.browser.nativeInterface.JNILib;
import com.layabox.config.config;

import org.json.JSONException;
import org.json.JSONObject;
import org.xwalk.core.XWalkActivity;

import java.io.File;
import java.io.InputStream;
import java.util.HashMap;
import java.util.Map;

import demo.PluginLauncher;
import demo.PluginSDK;

//import demo.PluginLauncher;

public class MainActivity extends XWalkActivity implements IWebViewListener,IPluginListener {
    ContentResolver contentProvider;
    public final static Uri BROWSER_CONTENT_URI = Uri.parse("content://com.layabox.provider.GameProvider");
    PluginLauncher launcher;
    IWebView webView;
    int mSpID = 103;
    public static MainActivity instanse;
    Handler handler = new Handler();
    private FrameLayout gameLayout = null;	//	游戏的layout
    private FrameLayout webViewLayout = null;	//	webview的layout
    /**
     * ATTENTION: This was auto-generated to implement the App Indexing API.
     * See https://g.co/AppIndexing/AndroidStudio for more information.
     */
//    private GoogleApiClient client;

    private ILayaBoxPlugin mPlugin;
    private long 			m_nBackPressTime=0;
    private SurfaceView mViewForAvoidingBlink;
    private String url;
    protected String conchOriention;
    protected static Map<String,Integer> OriMap= new HashMap<>();
    static
    {
        OriMap.put("landscape",0);
        OriMap.put("portrait",1);
        OriMap.put("user",2);
        OriMap.put("behind",3);
        OriMap.put("sensor",4);
        OriMap.put("nosensor",5);
        OriMap.put("sensor_landscape",6);
        OriMap.put("sensor_portrait",7);
        OriMap.put("reverse_landscape",8);
        OriMap.put("reverse_portrait",9);
        OriMap.put("full_sensor",10);
    }

    private boolean mIsFullScreen = true;
    private boolean mIsPluginShow = false;
    private boolean mHideWebview = false;
    private ValueCallback<Boolean> mOpenGameCallback;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
//        SnowFightPaySdkInterface.init(this);
        Log.e("", "onCreate: MainActivity");
        instanse = this;
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        launcher = new PluginLauncher(MainActivity.this);
        launcher.initialize(mSpID, new ValueCallback<Byte>() {
            @Override
            public void onReceiveValue(Byte aByte) {
            }
        });
        webView = WebViewFactory.createWebView(WebViewFactory.WEBVIEW_XWALK, this);

        String strLayaCache = this.getCacheDir().toString() + "/LayaCache";
        File cacheFolder = new File(strLayaCache);
        if (!cacheFolder.exists()) {
            cacheFolder.mkdir();
        }

        try{
            InputStream is = this.getResources().getAssets().open("config.ini");
            config.GetInstance().init( is );
        }catch(Exception e){
            Log.e("","打开配置文件错误。");
        }
        JNILib.InitDLib(this.getAssets(),"cache", strLayaCache, config.GetInstance().m_sLayaHallUrl);
        JSONObject gameInfo = new JSONObject();
        try {
            gameInfo.put("engineName", "hall");
            gameInfo.put("gameUrl",config.GetInstance().m_sLayaHallUrl);
        } catch (JSONException e) {
            e.printStackTrace();
        }
        webView.updateDcc(gameInfo);

        webView.setWebViewListener(this);

        contentProvider = getContentResolver();
        // ATTENTION: This was auto-generated to implement the App Indexing API.
        // See https://g.co/AppIndexing/AndroidStudio for more information.
//        client = new GoogleApiClient.Builder(this).addApi(AppIndex.API).build();

//        Utils.copyFromAssets(this,"laya_so");

        FrameLayout.LayoutParams localLayoutParams = new FrameLayout.LayoutParams(
                WindowManager.LayoutParams.MATCH_PARENT,
                WindowManager.LayoutParams.MATCH_PARENT, Gravity.TOP);
        if( gameLayout == null ){
            gameLayout = new FrameLayout(this);
            gameLayout.setFocusableInTouchMode(true);
            this.addContentView(gameLayout, localLayoutParams);
            mViewForAvoidingBlink=new SurfaceView(this);
            gameLayout.addView(mViewForAvoidingBlink);
            gameLayout.setVisibility(View.GONE);
            gameLayout.post(new Runnable() {
                @Override
                public void run() {
                    gameLayout.removeView(mViewForAvoidingBlink);
                }
            });
        }
        if( webViewLayout == null ){
            webViewLayout = new FrameLayout(this);
            webViewLayout.setBackgroundColor(0xffff0000);
            webViewLayout.setLayoutParams(localLayoutParams);
            webViewLayout.setFocusableInTouchMode(true);
            webViewLayout.addView((View) webView,new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
            this.addContentView(webViewLayout, localLayoutParams);
        }
    }
    public void onPause() {
        super.onPause();
        webView.resumeTimers();
        LayaPluginManager.getInstance().onPause();
    }
    @Override
    protected void onRestart() {
        super.onRestart();
    }
    public void onBackPressed() {
//        super.onBackPressed();
        //webView.closeGame();
        long curtm = System.currentTimeMillis();
        int MaxDelay = 3500; // Toast.LENGTH_LONG 对应的时间
        if (m_nBackPressTime == 0 || (m_nBackPressTime > 0 && curtm - m_nBackPressTime > MaxDelay)) {
            m_nBackPressTime = System.currentTimeMillis();
            String strMsg = getString(R.string.laya_ZaiAnTuiChu);
            Toast.makeText(this, strMsg, Toast.LENGTH_LONG).show();
        } else {
            // 退出
            //mLayaEngine.onDestroy();
            System.exit(0);
        }
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        webView.onActivityResult(requestCode, resultCode, data);
//        if (mLayaEngine != null) {
//            ((LayaWrapper) mLayaEngine).onActivityResult(requestCode, resultCode, data);
//        }
        super.onActivityResult(requestCode, resultCode, data);
    }

    @Override
    public void onNewIntent(Intent intent) {
        webView.onNewIntent(intent);
        super.onNewIntent(intent);
    }
    @Override
    protected void onDestroy(){
        super.onDestroy();
        if(webView!=null) {
            webView.onDestroy();
        }
//        SnowFightPaySdkInterface.destroy(this);
        LayaPluginManager.getInstance().onDestroy();
    }
    @Override
    protected void onResume() {
        super.onResume();
        LayaPluginManager.getInstance().onResume();
    }

    public boolean onKeyUp(int keyCode, KeyEvent e) {
        return super.onKeyUp(keyCode, e);
    }
    public void hideWebview(boolean hide){
        if (hide){
            mHideWebview = true;
            if (launcher != null && mIsPluginShow){
                int ori=OriMap.get(conchOriention);
                this.setRequestedOrientation(ori);
                gameLayout.setVisibility(View.VISIBLE);
                webView.setVisibility(View.GONE);
                webViewLayout.setVisibility(View.GONE);
                mHideWebview = false;
            }
        }
        else{
            gameLayout.setVisibility(View.GONE);
            webView.setVisibility(View.VISIBLE);
            webViewLayout.setVisibility(View.VISIBLE);
        }
    }
    @Override
    public void openGame(JSONObject gameInfo, ValueCallback<Boolean> openCallback) {
        mOpenGameCallback = openCallback;
        try {
            final String gameUrl = gameInfo.getString("gameUrl");
            final String engineName = gameInfo.getString("engineName");
            conchOriention = gameInfo.getString("orientation");
            JSONObject extJson=new JSONObject();
            try {
                extJson.put("gameId", "100");
            } catch (JSONException e) {
                e.printStackTrace();
            }
            LayaPluginManager.getInstance().Initialization(mSpID,extJson , this, new ValueCallback<Byte>() {
                @Override
                public void onReceiveValue(Byte value) {
                    if(value == LayaPluginManager.CONFIG_INIT_OK){
                        mPlugin = LayaPluginManager.getInstance().GetLayaBoxPlugin(engineName, MainActivity.this, null, MainActivity.this);
                        mPlugin.SetOption("gameUrl", gameUrl);
                        mPlugin.SetScreenOrientation(conchOriention);
                        mPlugin.SetFullScreen(true);
                        mPlugin.SetExternalPorxy(new PluginSDK(MainActivity.this));
                        Log.e("LayaPluginActivity","LayaPluginActivity start game!!");
                        mPlugin.Start(engineName);
                        //显示悬浮按钮的操作，下面是以悬浮按钮的事例
//                        MyWindowManager.createSmallWindow(LayaPluginActivity.this);
                    }
                }
            });

        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void runJS(String jsContent) {
        Bundle value = new Bundle();
        value.putString("value",jsContent);
        LayaPluginManager.getInstance().invokeMethod("runJS", value);
    }

    public void invokeMethod(String method, Bundle parm) {
        if ("runJS".equals(method)) {
            final String value = parm.getString("value");
            handler.post(new Runnable() {
                @Override
                public void run() {
                    webView.runJS(value);
                }
            });
        }
    }

    /**
     * ATTENTION: This was auto-generated to implement the App Indexing API.
     * See https://g.co/AppIndexing/AndroidStudio for more information.
     */
//    public Action getIndexApiAction() {
//        Thing object = new Thing.Builder()
//                .setName("Main Page") // TODO: Define a title for the content shown.
//                // TODO: Make sure this auto-generated URL is correct.
//                .setUrl(Uri.parse("http://[ENTER-YOUR-URL-HERE]"))
//                .build();
//        return new Action.Builder(Action.TYPE_VIEW)
//                .setObject(object)
//                .setActionStatus(Action.STATUS_TYPE_COMPLETED)
//                .build();
//    }

    @Override
    public void onStart() {
        super.onStart();

        // ATTENTION: This was auto-generated to implement the App Indexing API.
        // See https://g.co/AppIndexing/AndroidStudio for more information.
//        client.connect();
//        AppIndex.AppIndexApi.start(client, getIndexApiAction());
    }

    @Override
    public void onStop() {
        super.onStop();
        // ATTENTION: This was auto-generated to implement the App Indexing API.
        // See https://g.co/AppIndexing/AndroidStudio for more information.
//        AppIndex.AppIndexApi.end(client, getIndexApiAction());
//        client.disconnect();
    }
    @Override
    public void onXWalkReady() {
        Log.e("", "XWalk ready ");
    }

    @Override
    public void Plugin_Start(Object o, View view) {
        FrameLayout.LayoutParams localLayoutParams = new FrameLayout.LayoutParams(
                WindowManager.LayoutParams.MATCH_PARENT,
                WindowManager.LayoutParams.MATCH_PARENT,Gravity.BOTTOM);
        gameLayout.addView(view);
        gameLayout.setVisibility(View.VISIBLE);
        SharedPreferences perfer = PreferenceManager.getDefaultSharedPreferences(this);
        boolean curFull = perfer.getBoolean("fullscreen", false);
        if(!curFull && mIsFullScreen)
            this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                    WindowManager.LayoutParams.FLAG_FULLSCREEN);
        else if( curFull && !mIsFullScreen ){
            quitFullScreen();
        }
        mIsPluginShow = true;
        if( mHideWebview ){
            hideWebview(true);
        }
        if( mOpenGameCallback != null ){
            mOpenGameCallback.onReceiveValue(true);
        }
    }
    private void quitFullScreen(){
        final WindowManager.LayoutParams attrs = this.getWindow().getAttributes();
        attrs.flags &= (~WindowManager.LayoutParams.FLAG_FULLSCREEN);
        this.getWindow().setAttributes(attrs);
        this.getWindow().clearFlags(WindowManager.LayoutParams.FLAG_LAYOUT_NO_LIMITS);
    }

    @Override
    public void Plugin_Finish() {

    }

    @Override
    public void onNoEnoughSpace() {

    }

    public void cancelPopupwindow() {
    }

    public void setLoadingExtra(int nProcess) {

    }
}
