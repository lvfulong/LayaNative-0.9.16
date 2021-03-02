package com.layabox.browser;

import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.util.Log;
import android.view.View;
import android.webkit.JsResult;
import android.webkit.WebChromeClient;
import android.webkit.WebResourceResponse;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;

import com.layabox.browser.reflect.LayaBoxWebViewJS;

import org.json.JSONObject;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;

/**
 * Created by hugao on 2016/10/12.
 */

public class NormalWebView extends WebViewBase implements  IWebView {
    private WebView webView;
    private WebViewClient webViewClient;

    class MyWebViewClient extends  WebViewClient{
        public WebResourceResponse shouldInterceptRequest(WebView view, String sUrl)
        {
            return  super.shouldInterceptRequest(view, sUrl);
        }

        //------------------------------------------------------------------------------
        public boolean shouldOverrideUrlLoading( WebView view, String sUrl )
        {
            return true;
        }
        //------------------------------------------------------------------------------
        public void onPageStarted(WebView view, String url, Bitmap favicon) {
            Log.d("LayaWebView","onPageStarted url="+url);
            super.onPageStarted(view, url, favicon);
        }
        //------------------------------------------------------------------------------
        public void onLoadResource(WebView view, String url)
        {
            super.onLoadResource(view, url);
        }
        //------------------------------------------------------------------------------

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

        public void onPageFinished(android.webkit.WebView view, java.lang.String url)
        {
            String s = loadFromAsset("initWebView.js");
            view.loadUrl("javascript:"+s);
        }
    }


    class LayaWebChromeClient extends WebChromeClient
    {
        public boolean onJsAlert(WebView view, String url, String message, JsResult result) {
            Log.e( "LayaWebView","onJsAlert message=" + message );
            return  false;
        }
    }
    public NormalWebView(Context context)
    {
        super(context);
        webView=new WebView(context);
        webViewClient = new MyWebViewClient();
        webView = new WebView( context );
        webView.setWebViewClient(webViewClient);
        webView.setWebChromeClient(new LayaWebChromeClient());
        WebSettings settings=webView.getSettings();
        webView.getSettings().setJavaScriptEnabled(true);
        settings.setDefaultTextEncodingName("GBK");
        settings.setDomStorageEnabled(true);
        settings.setAppCacheMaxSize(1024 * 1024 * 8);
        String appCachePath = context.getApplicationContext().getCacheDir().getAbsolutePath()+"/webviewCache";
        settings.setAppCachePath(appCachePath);
        settings.setAllowFileAccess(true);
        settings.setAppCacheEnabled(true);
        webView.setScrollBarStyle(View.SCROLLBARS_INSIDE_OVERLAY);
        this.addView(webView);
        webView.addJavascriptInterface(new LayaBoxWebViewJS(this), "runtime");
    }
    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {

    }

    @Override
    public void onNewIntent(Intent intent) {

    }

    @Override
    public void onDestroy() {

    }

    public  void resumeTimers()
    {

    }

    @Override
    public void setVisibility(int visibility) {
        webView.setVisibility(visibility);
    }

    @Override
    public void loadUrl(String url) {
        webView.loadUrl(url);
    }

    @Override
    public void runJS(String jsContent) {
        webView.loadUrl("javascript:"+jsContent);
    }
    @Override
    public void updateDcc(JSONObject gameInfo){}
}
