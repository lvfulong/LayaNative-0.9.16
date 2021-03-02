package com.layabox.browser;

import android.app.Activity;
import android.content.Context;
import android.content.res.AssetManager;
import android.webkit.ValueCallback;
import android.widget.FrameLayout;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

/**
 * Created by hugao on 2016/10/12.
 */

public class WebViewBase extends FrameLayout{
    public static final int STATU_NORMAL = 1;
    public static final int STATU_ONGAME = 2;
    public static final int GAME_UNKNOW = 0;
    public static final int GAME_HTML5 = 1;
    public static final int GAME_CONCH = 2;
    class MessageCache{
        String message;
        String target;
        MessageCache(String _msg,String _tgt){
            message = _msg;
            target = _tgt;
        }
    }
    /**
     * 上下文对象
     */
    protected Context context;
    /**
     * XWalkView对象
     */
    protected IWebViewListener webViewListener;
    protected int status;
    protected int preOri;
    protected int gameType = GAME_UNKNOW;
    protected AssetManager am;
    protected static Map<String,Integer> OriMap=new HashMap<String,Integer>();
    private ArrayList<MessageCache> mMessageCache = new ArrayList<>();


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
    public WebViewBase(Context context)
    {
        super(context);
        am=context.getAssets();
        this.context=context;
        preOri=((Activity)context).getRequestedOrientation();
        status=STATU_NORMAL;
    }

    public void setWebViewListener(IWebViewListener listener) {
        webViewListener=listener;
    }

    public void runJS(String jsContent) {

    }

    public void postMessage(String messageJSON, String targetOrigin) {
        if(gameType==GAME_HTML5) {
            runJS("window.frames[0].postMessage(JSON.parse(decodeURIComponent(\"" + messageJSON + "\")),\"" + targetOrigin + "\")");
        }
        else if(gameType==GAME_CONCH){
            if(webViewListener!=null) {
                webViewListener.runJS("window.postRuntimeMessage(\"" + messageJSON + "\",\"" + targetOrigin + "\")");
            }
        }else if(gameType==GAME_UNKNOW){
            mMessageCache.add(new MessageCache(messageJSON,targetOrigin));
        }
    }

    public void openGame(JSONObject gameInfo) {
        try {
            String engineName=gameInfo.getString("engineName");
            if("webview".equals(engineName)) {
                String orientation=gameInfo.getString("orientation");
                String gameUrl = gameInfo.getString("gameUrl");
                runJS("var a=window.LayaGameIFrame;a.setAttribute('src',\""+gameUrl+"\");" +
                        "a.style.setProperty('display','block');" +
                        "if(!a.parentNode)document.body.appendChild(a);");
                Integer ori=OriMap.get(orientation);
                if(ori!=null)
                    ((Activity)context).setRequestedOrientation(ori);
                status=STATU_ONGAME;
                gameType=GAME_HTML5;
                if(mMessageCache.size()>0){
                    for(MessageCache cache : mMessageCache){
                        postMessage(cache.message,cache.target);
                    }
                    mMessageCache.clear();
                }
            }
            else{
                if(webViewListener!=null) {
                    webViewListener.openGame(gameInfo, new ValueCallback<Boolean>() {
                        @Override
                        public void onReceiveValue(Boolean value) {
                            if(value){
                                gameType=GAME_CONCH;
                                if(mMessageCache.size()>0){
                                    for(MessageCache cache : mMessageCache){
                                        postMessage(cache.message,cache.target);
                                    }
                                    mMessageCache.clear();
                                }
                            }
                        }
                    });
                }
            }

        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    public void closeGame() {
        if(status==STATU_ONGAME) {
            runJS("var a=window.LayaGameIFrame;a.src='about:blank';try{ \n" +
                    "a.contentWindow.document.write(''); \n" +
                    "a.contentWindow.document.close(); \n" +
                    "}catch(e){};document.body.removeChild(a);");
            status=STATU_NORMAL;
            ((Activity)context).setRequestedOrientation(preOri);
            gameType= GAME_UNKNOW;
        }
    }
    public IWebViewListener getWebViewListener(){
        return webViewListener;
    }
}
