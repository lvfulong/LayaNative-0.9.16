package com.layabox.browser;

import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.util.Log;
import android.webkit.WebSettings;

import com.layabox.browser.nativeInterface.JNILib;
import com.layabox.browser.reflect.LayaBoxWebViewJS;
import com.layabox.browser.reflect.SnowFightPaySDKJS;
import com.layabox.browser.utils.Utils;

import org.json.JSONObject;
import org.xwalk.core.XWalkResourceClient;
import org.xwalk.core.XWalkSettings;
import org.xwalk.core.XWalkUIClient;
import org.xwalk.core.XWalkView;
import org.xwalk.core.XWalkWebResourceRequest;
import org.xwalk.core.XWalkWebResourceResponse;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.FileNameMap;
import java.net.URL;
import java.net.URLConnection;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.IntBuffer;
import java.util.Date;

//import org.xwalk.core.internal.XWalkSettingsInternal;
//import org.xwalk.core.internal.XWalkViewBridge;
/**
 * Created by hugao on 2016/10/8.
 */
public class LayaBoxWebView extends WebViewBase implements IWebView {
    /**
     * XWalkView对象
     */
    static {
        System.loadLibrary("common");
    }

    private final SnowFightPaySDKJS mSnowFightSDK;
    private Handler handler;
    private Context mContext;

    private XWalkView webView;
    private String appCachePath;
    private XWalkResourceClient webViewClient;

    public LayaBoxWebView(Context context) {
        super(context);
        mContext = context;
        handler=new Handler();
        webView=new XWalkView(context);
        appCachePath = context.getApplicationContext().getCacheDir().getAbsolutePath()+"/webviewCache";

       /* Method _getBridge;
        try {
            _getBridge = XWalkView.class.getDeclaredMethod("getBridge");
            _getBridge.setAccessible(true);
            XWalkViewBridge xWalkViewBridge = null;
            {
                xWalkViewBridge = (XWalkViewBridge) _getBridge.invoke(webView);
                XWalkSettingsInternal xWalkSettings1 = xWalkViewBridge.getSettings();
                xWalkSettings1.setCacheMode(WebSettings.LOAD_CACHE_ELSE_NETWORK);
                //  String appCachePath = context.getApplicationContext().getCacheDir().getAbsolutePath()+"/webviewCache";
                //xWalkSettings1.setAppCacheEnabled(true); //设置为可以缓存
                //xWalkSettings1.setAppCachePath(appCachePath);//设置缓存路径
                //xWalkSettings1.setAllowFileAccess(true);
                //xWalkSettings1.setDatabaseEnabled(true);
            }
        }
        catch (Exception ex)
        {
            //Log.e(">>>>>>>>>>>>>>>>>>>>",ex.getMessage());
        }*/
        mSnowFightSDK = new SnowFightPaySDKJS(mContext,this);
        XWalkSettings xWalkSettings=webView.getSettings();
        xWalkSettings.setJavaScriptEnabled(true);
        webView.addJavascriptInterface(new LayaBoxWebViewJS(this),"runtime");
        webView.addJavascriptInterface(mSnowFightSDK,"SnowFightPaySDK");
        //设置 缓存模式

        // 开启 DOM storage API 功能
        // mXWalkView.getSettings().set
        xWalkSettings.setCacheMode(WebSettings.LOAD_CACHE_ELSE_NETWORK);
        xWalkSettings.setDomStorageEnabled(true);
        webViewClient=new MyResourceClient(webView);
        webView.setResourceClient(webViewClient);
        webView.setUIClient(new MyUIClient(webView));
        
        this.addView(webView);
    }
    @Override
    public void openGame(JSONObject gameInfo) {
        try {
            if (gameInfo.getString("engineName").equals("layaAirPlugin")) {
                super.openGame(gameInfo);
             }
            else {
                updateDcc(gameInfo);
            }
            }catch (Exception ex){
               ex.printStackTrace();
            }
    }
    public void openGameSuper(JSONObject gameInfo) {
        super.openGame(gameInfo);
    }
    @Override
    public void updateDcc(JSONObject gameInfo){
        try {
            new MyThread(this,gameInfo).start();
        }catch (Exception ex){
            Log.e(">>>>>>>>>>>>>>>>>>>>",ex.getMessage());
        }
    }

    private class MyThread extends Thread {
        private LayaBoxWebView webView;
        private JSONObject gameInfo;
        public MyThread(LayaBoxWebView webView,JSONObject gameInfo) {
            this.webView = webView;
            this.gameInfo = gameInfo;
        }
        public void run() {
            try {
                String gameUrl = gameInfo.getString("gameUrl");
                URL u = new URL(gameUrl);
                String urlpath = "";
                if (u.getPort() !=-1 ) {
                    urlpath = String.format("%s://%s:%s/", u.getProtocol(), u.getHost(), u.getPort());
                } else {
                    urlpath = String.format("%s://%s/", u.getProtocol(), u.getHost());
                }
                Log.e("[cache]", "update dcc start " + gameUrl);
                String curassets = JNILib.preUpdateDcc(gameUrl, urlpath);
                Date time = new Date();
                String asidUrl = String.format("%supdate/assetsid.txt?rand=%f", urlpath, Math.random() * time.getTime());
                Log.e("[cache debug] asidUrl ", asidUrl);
                String assetsidStr = "";
                byte[] assetsidStream = Utils.downloadFile(asidUrl);
                if (assetsidStream != null) {
                    assetsidStr = new String(assetsidStream,"UTF-8");
                }
                Log.e("[cache] assetsid ", assetsidStr);
                if (assetsidStr == null || assetsidStr.isEmpty()) {
                    JNILib.updateDccClearAssetsid(gameUrl, urlpath);
                } else {
                    if (!curassets.equals(assetsidStr)) {
                        String txtdcc = "";
                        String filetableUrl = String.format("%supdate/filetable.bin", urlpath);
                        byte[] filetablebyte = Utils.downloadFile(filetableUrl);
                        if (filetablebyte == null) {
                            Log.e("[cache] ", "filetable.bin download error");
                        } else if (filetablebyte.length % 8 != 0) {
                            Log.e("[cache] ", "filetable.bin download error");
                        } else {
                            ByteBuffer b=ByteBuffer.wrap(filetablebyte).order(ByteOrder.nativeOrder());
                            IntBuffer v = b.asIntBuffer();
                            //Log.e("[cache] ", new String(filetablebyte,"UTF-8"));
                            //Log.e("[cache] ", ""+v.get(0));
                            //Log.e("[cache] ", ""+v.get(1));
                            if (v.get(0) !=  0xffeeddcc || v.get(1) != 1) {
                                Log.e("[cache] ", "dcc.bin file err!");
                            }else
                            {
                                if (v.get(2) == 0x00ffffff)
                                {
                                    int stp = (4 + 8) / 2;
                                    byte[] md5byte = new byte[32];
                                    for (int i = 0; i < 32; i++){
                                        md5byte[i] = b.get(16+i);
                                    }

                                    String so = new String(md5byte,"UTF-8");

                                    Log.e("[cache] ", "--------------------------------------------");
                                    Log.e("[cache] ", "so=" + so);
                                    Log.e("[cache] ", "netid=" + assetsidStr);
                                    if (so.equals(assetsidStr)) {
                                        for (int ii = stp, isz = v.capacity() / 2; ii < isz; ii++)
                                            txtdcc += Integer.toHexString(v.get(ii * 2)) + " " + Integer.toHexString(v.get(ii * 2 + 1)) + "\n";
                                    }
                                    Log.e("[cache] ssssss", "cnm");
                                    Log.e("[cache] ", txtdcc);
                                }
                                else {
                                    Log.e("[cache] ", "error old format unsupport");
                                }
                           }
                            if (txtdcc.length() > 0) {
                                JNILib.doUpdateDcc(gameUrl, urlpath, txtdcc, assetsidStr);
                            }
                        }
                    }
                }
                handler.post
                        (
                                new Runnable()
                                {
                                    public void run()
                                    {
                                        try {
                                            if (gameInfo.getString("engineName").equals("hall")) {
                                                webView.loadUrl(gameInfo.getString("gameUrl"));
                                            } else {
                                                webView.openGameSuper(gameInfo);
                                            }
                                            }catch (Exception ex){
                                                Log.e(">>>>>>>>>>>>>>>>>>>>",ex.getMessage());
                                            }
                                    }
                               }
                        );

           }catch (Exception ex){
               Log.e(">>>>>>>>>>>>>>>>>>>>",ex.getMessage());
            }
        }
    }
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        webView.onActivityResult(requestCode,resultCode,data);
    }

    public void onNewIntent(Intent intent) {
        webView.onNewIntent(intent);
    }

    @Override
    public void onDestroy() {
        if( mSnowFightSDK != null){
            mSnowFightSDK.onDestroy();
        }
    }

    public void loadUrl(String url)
    {
        webView.load(url,null);
    }


    @Override
    public void setVisibility(int visibility) {
        webView.setVisibility(visibility);
    }

    public void resumeTimers()
    {
        webView.resumeTimers();
    }

    @Override
    public void runJS(String jsContent)
    {
        webView.evaluateJavascript(jsContent,null);
    }


    class MyResourceClient extends XWalkResourceClient {
        MyResourceClient(XWalkView view) {
            super(view);
        }

        public void onDocumentLoadedInFrame(XWalkView view, long frameId) {
            int a=10;
        }


        public void onReceivedResponseHeaders(XWalkView view, XWalkWebResourceRequest request, XWalkWebResourceResponse response)
        {
            String d= request.getUrl().toString();
            InputStream data= response.getData();
            String type=response.getMimeType();
            String  encoding =response.getEncoding();
        }

        public  String getMimeType(String fileUrl)
                throws java.io.IOException
        {
            FileNameMap fileNameMap = URLConnection.getFileNameMap();
            String type = fileNameMap.getContentTypeFor(fileUrl);

            return type;
        }

        @Override
        public XWalkWebResourceResponse shouldInterceptLoadRequest(XWalkView view, XWalkWebResourceRequest request){
            // Handle it here.
//            if("http://10.10.30.29:9876/testMange.js?v=1".equals(url))
//            {
//                InputStream is=null;
//                try {
//                    is=am.open("testMange.js");
//                } catch (IOException e) {
//                    e.printStackTrace();
//                }
//                return new WebResourceResponse("application/x-javascript","UTF-8",is);
//            }
//            else {
            //createXWalkWebResourceResponse()
            String url=request.getUrl().toString();
//            if(url.indexOf("txt")!=-1)
//            {
//                while (true)
//                {
//                    int a=10;
//                }
//            }

//            String localUrl=appCachePath+url.substring(url.lastIndexOf("/"));
//            File temp=new File(localUrl);
//            if(!temp.exists())
//            {
//                Utils.downloadFile(url, localUrl);
//                //if(url.indexOf("txt")!=-1)
//
//            }
//            XWalkWebResourceResponse result= null;
//            try {
//                result = createXWalkWebResourceResponse("","UTF-8",new FileInputStream(temp));
//            } catch (FileNotFoundException e) {
//                e.printStackTrace();
//            }

            byte[] data = JNILib.interceptLoadRequest(url);
            if(data != null && data.length > 0){
                Log.e("[cache] cache hit ",url);
                String mineType = "";
                try {
                    mineType = getMimeType(url);
                }catch (IOException e)
                {
                    e.printStackTrace();
                }
                return createXWalkWebResourceResponse(mineType,"UTF-8",new ByteArrayInputStream(data));
            }
            Log.e("[cache] cache miss ",url);
            //return super.shouldInterceptLoadRequest(view,request);
            return  null;
        }
        @Override
        public void onReceivedResponseData(XWalkView view, String url, byte[] data) {
            //Log.e("[cache] write cache ", url);
            //Log.e("[cache] write cache ", new String(data, "UTF-8"));
            JNILib.saveCache(url, data);
            //super.onReceivedResponseData(view,url,data);
        }
        @Override
        public boolean shouldOverrideUrlLoading(XWalkView view, String url) {
            int a=10;
            return false;
        }

        @Override
        public void onLoadFinished (XWalkView view, String url)
        {
            int a=10;
        }
        @Override
        public void onLoadStarted (XWalkView view, String url)
        {
            int a=10;
        }
        @Override
        public void onProgressChanged (XWalkView view, int progressInPercent)
        {
            int a=10;
        }
        @Override
        public  void doUpdateVisitedHistory(XWalkView view, String url, boolean isReload)
        {
            String s = loadFromAsset("initWebView.js");
            view.evaluateJavascript(s, null);
            super.doUpdateVisitedHistory( view,  url,  isReload);
        }
        public String loadFromAsset(String url)
        {
            String sBuffer="";
            try {
                InputStream in= am.open(url);
                int ch;
                ByteArrayOutputStream out = new ByteArrayOutputStream(); //实现了一个输出流
                while((ch=in.read())!=-1) {
                    out.write(ch); //将指定的字节写入此 byte 数组输出流
                }
                byte[] buff = out.toByteArray();//以 byte 数组的形式返回此输出流的当前内容
                out.close(); //关闭流
                in.close(); //关闭流
                String content = new String(buff, "UTF-8"); //设置字符串编码
                sBuffer+=content;

            } catch (IOException e) {
                e.printStackTrace();
            }
            return sBuffer;
        }
    }

    class MyUIClient extends XWalkUIClient {
        MyUIClient(XWalkView view) {
            super(view);
        }

        @Override
        public  void onReceivedTitle(XWalkView view, String title)
        {
            //String s = loadFromAsset("initWebView.js");
            //view.evaluateJavascript(s, null);
            super.onReceivedTitle(view,title);
        }

        @Override
        public void onFullscreenToggled(XWalkView view, boolean enterFullscreen) {
            // Handle it here.
            // ...
            //this.onPageLoadStarted();
        }
        @Override
        public  void onPageLoadStarted(XWalkView view,String url)
        {


        }
        public String loadFromAsset(String url)
        {
            String sBuffer="";
            try {
                InputStream in= am.open(url);
                int ch;
                ByteArrayOutputStream out = new ByteArrayOutputStream(); //实现了一个输出流
                while((ch=in.read())!=-1) {
                    out.write(ch); //将指定的字节写入此 byte 数组输出流
                }
                byte[] buff = out.toByteArray();//以 byte 数组的形式返回此输出流的当前内容
                out.close(); //关闭流
                in.close(); //关闭流
                String content = new String(buff, "UTF-8"); //设置字符串编码
                sBuffer+=content;

            } catch (IOException e) {
                e.printStackTrace();
            }
            return sBuffer;
        }

        public void onPageLoadStopped(XWalkView view, String url, XWalkUIClient.LoadStatus status) {
            //String s = loadFromAsset("initWebView.js");
            //view.evaluateJavascript(s, null);
            super.onPageLoadStopped(view,url,status);
        }

    }
}
