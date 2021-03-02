package com.layabox.browser.reflect;
import android.os.Handler;
import android.util.Log;

import com.layabox.browser.IWebView;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import org.xml.sax.SAXException;
import org.xwalk.core.JavascriptInterface;

import java.io.IOException;
import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.HashMap;

import javax.xml.parsers.ParserConfigurationException;

/**
 * Created by hugao on 2016/10/9.
 */

public class LayaBoxWebViewJS {
    public static HashMap<Integer,Object> id2objMap=new HashMap<Integer, Object>();
    public static HashMap<Object,Integer> obj2idMap=new HashMap<Object, Integer>();
    public Handler m_Handler = new Handler();

    private IWebView webView;

    public LayaBoxWebViewJS(IWebView webView)
    {
        this.webView=webView;
    }

    private void newObj(Class[] signs, Object[] params,int callerId,Class cls)
    {
        Constructor con;
        try {
            con = cls.getConstructor(signs);
            try {
                Object obj;
                try {
                    obj = con.newInstance(params);
                    id2objMap.put(callerId,obj);
                    obj2idMap.put(obj,callerId);
                } catch (IllegalAccessException e) {
                    e.printStackTrace();
                } catch (InvocationTargetException e) {
                    e.printStackTrace();
                }
            } catch (InstantiationException e) {
                e.printStackTrace();
            }
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        }
    }

    private Object _callMethod(Class[] signs, Object[] params,String methodName,Object caller,Class cls)
    {
        Object result;
        if(cls==null)
        {
            Log.e("",">>>>>>>>>>>>>>>>>>>>null");
            return "";
        }
        try {
            Method method = cls.getMethod(methodName, signs);
            try {
                result= method.invoke(caller, params);
            }
            catch (Exception e)
            {
                return e.toString();
            }
        } catch (NoSuchMethodException e) {
            return e.toString();
        }
        return result;
    }

    @JavascriptInterface
    @android.webkit.JavascriptInterface
    public String callMethod(int callerId,boolean isSyn,String className,String methodName,String paramStr) {
        Object []param;
        className=className.replaceAll("/",".");
        if(paramStr==null)
            param=new Object[0];
        else {
            JSONArray ja = null;
            try {
                ja = new JSONArray(paramStr);
            } catch (JSONException e) {
                e.printStackTrace();
            }
            int paramLen= ja != null ? ja.length() : 0;
            param=new Object[paramLen];
            for(int i=0;i<paramLen;i++)
            {
                try {
                    param[i]=ja.get(i);
                } catch (JSONException e) {
                    e.printStackTrace();
                }
            }
        }
        Class [] temp=new Class[param.length];
        Object returnValue=null;
        int i=0;
        for (Object t:param) {
            Class c=t.getClass();
            if(c==Integer.class){
                c=int.class;
            }
            else if(c==Float.class){
                c=float.class;
            }
            else if(c==Double.class) {
                c=float.class;
                param[i]=Float.parseFloat(param[i].toString());
            }
            else if(c==Boolean.class) {
                c=boolean.class;
            }
            temp[i]=c;
            i++;
        }
        Object caller=id2objMap.get(callerId);
        Class cls;
        try {
            cls = caller!=null?caller.getClass():Class.forName(className);
            if("<init>".equals(methodName))
            {
                newObj(temp,param,callerId,cls);
            }
            else {
                returnValue = _callMethod(temp, param, methodName,caller,cls);
            }
        } catch (ClassNotFoundException e) {
            returnValue=e.toString();
        }
        JSONObject result=new JSONObject();
        try {
            result.put("v",returnValue);
        } catch (JSONException e) {
            e.printStackTrace();
        }
        return  result.toString();
    }

    @JavascriptInterface
    @android.webkit.JavascriptInterface
    public void openGame(final String gameInfo)
    {
        m_Handler.post(new Runnable() {
            @Override
            public void run() {
                try {
                    JSONObject a=new JSONObject(gameInfo);
                    webView.openGame(a);
                } catch (JSONException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    @JavascriptInterface
    @android.webkit.JavascriptInterface
    public void postMessage(final String  parms,final String targetOrigin) throws ParserConfigurationException, SAXException, IOException {
        m_Handler.post(new Runnable() {
            @Override
            public void run() {
                webView.postMessage(parms,targetOrigin);
            }
        });
    }

    @JavascriptInterface
    @android.webkit.JavascriptInterface
    public void hideWebview(final boolean  hide) throws ParserConfigurationException, SAXException, IOException {
        m_Handler.post(new Runnable() {
            @Override
            public void run() {
                webView.getWebViewListener().hideWebview(hide);
                Log.e("",">>>>>>>>>>>>>>>>>>>>hideWebview");
            }
        });
    }
    @JavascriptInterface
    @android.webkit.JavascriptInterface
    public void log(final String  log) throws ParserConfigurationException, SAXException, IOException {
        m_Handler.post(new Runnable() {
            @Override
            public void run() {
                Log.e("",log);
            }
        });
    }
    @JavascriptInterface
    @android.webkit.JavascriptInterface
    public void dispatchFunctionWithCallBack(final String  methodName,final String paramJSON) throws ParserConfigurationException, SAXException, IOException {
        m_Handler.post(new Runnable() {
            @Override
            public void run() {
                Log.e("","dispatchFunctionWithCallBack not implement");
            }
        });
    }
}
