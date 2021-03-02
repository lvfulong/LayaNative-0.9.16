package com.layabox.browser.reflect;
import android.content.Context;
import android.os.Handler;

import com.layabox.browser.ILBSSDKCallback;
import com.layabox.browser.ILBSSDKInterface;
import com.layabox.browser.IWebView;
import com.layabox.browser.LBSMarket;

import org.json.JSONException;
import org.json.JSONObject;
import org.xwalk.core.JavascriptInterface;

/**
 * Created by hugao on 2016/10/9.
 */

public class SnowFightPaySDKJS {

    public Handler m_Handler = new Handler();

    private Context ctx;
    private IWebView webView;
    private ILBSSDKInterface mSDKInterface;
    public SnowFightPaySDKJS(Context ctx,IWebView webView)
    {
        this.ctx=ctx;
        this.webView=webView;
        mSDKInterface = new LBSMarket();
        mSDKInterface.onCreate(ctx);
    }
    @JavascriptInterface
    @android.webkit.JavascriptInterface
    public void login(){
        if( mSDKInterface==null ){
            return;
        }
        mSDKInterface.login(ctx,null, new ILBSSDKCallback() {
            @Override
            public void onSuccess(String s) {
                try {
                    JSONObject obj = new JSONObject(s);
                    JSONObject res = new JSONObject();
                    res.put("result",SDK_STATUS.STATUS_SUCCESS);
                    res.put("unionUserId",obj.optString("unionUserId"));
                    runWebviewJS_login(res);
                } catch (JSONException e) {
                    e.printStackTrace();
                }
            }

            @Override
            public void onFailed(String s) {
                JSONObject res = getSDKResult(SDK_STATUS.STATUS_FAILED,"failed");
                runWebviewJS_login(res);
            }

            @Override
            public void onCancel(String s) {
                JSONObject res = getSDKResult(SDK_STATUS.STATUS_CANCEL,"cancel");
                runWebviewJS_login(res);
            }
        });
    }
    @JavascriptInterface
    @android.webkit.JavascriptInterface
    public void pay(final String jsonStr){
        mSDKInterface.pay(ctx, jsonStr, new ILBSSDKCallback() {
            @Override
            public void onSuccess(String s) {
                JSONObject res = getSDKResult(SDK_STATUS.STATUS_SUCCESS,"success");
                runWebviewJS_pay(res);
            }

            @Override
            public void onFailed(String s) {
                JSONObject res = getSDKResult(SDK_STATUS.STATUS_FAILED,"failed");
                runWebviewJS_pay(res);
            }

            @Override
            public void onCancel(String s) {
                JSONObject res = getSDKResult(SDK_STATUS.STATUS_FAILED,"cancel");
                runWebviewJS_pay(res);
            }
        });
    }


    public void onDestroy(){
        mSDKInterface.onDestroy(ctx);
    }
    private void runWebviewJS_login(JSONObject obj){
        if( obj == null ){
            return;
        }
        webView.loadUrl("javascript:SnowFightPaySDK.loginResult(" + obj + ")");
    }
    private void runWebviewJS_pay(JSONObject obj){
        if( obj == null ){
            return;
        }
        webView.loadUrl("javascript:SnowFightPaySDK.payResult("+obj+")");
    }
    private JSONObject getSDKResult(int status, String message){
        JSONObject _jobj = new JSONObject();
        try {
            _jobj.put("result", status);
            _jobj.put("desc", message);
        } catch (JSONException e) {
            e.printStackTrace();
        }
        return _jobj;
    }
}
