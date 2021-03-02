package demo;

import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.webkit.ValueCallback;

import com.layabox.ibridge.IExternalRuntimeProxy;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

//渠道需要重写此接口中的方法
public class PluginSDK implements IExternalRuntimeProxy {

    private final String TAG = "PluginSDK";
//    private static PluginSDK instance = null;
    private com.layabox.layaboxbrowser.MainActivity mCtx;
    
    public PluginSDK(com.layabox.layaboxbrowser.MainActivity ctx){
    	this.mCtx = ctx;
    }
    
//    public static PluginSDK getInstance(){
//    	if( instance == null )
//    		instance = new PluginSDK();
//    	return instance;
//    }
    @Override
    public void Login(Context context, JSONObject jsonObj, ValueCallback<JSONObject> callback) {
        Log.e(TAG,"layabox Login param = "+jsonObj.toString());
        
        //String url = "{\"login\":{\"status\":0,userInfo:{\"nickName\":\"test\",\"id\":\"123fdsr44rer\",\"photo\":\"http://test.com/a.jpg\",\"sptoken\":\"34gdfg5tdsf45wyu553435\"}}}";
        
        try {
            JSONObject result = new JSONObject();
            result.put("status", 0);
            result.put("unionUserID", "1123fff4");
            result.put("nickName", "中南海");
            result.put("photo", "http://www.xxx.com/xxx.jpg");
            result.put("sptoken", "33ffffh54444dddd");
            result.put("msg", "success");
            callback.onReceiveValue(result);
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void Pay(Context context,JSONObject jsonObj, ValueCallback<JSONObject> callback) {
        Log.e(TAG,"layabox Pay param = "+jsonObj.toString());
        JSONObject result = new JSONObject();
        try {
            result.put("status", 0);
            result.put("msg", "success");
        } catch (JSONException e) {
            e.printStackTrace();
        }
        callback.onReceiveValue(result);
    }

    @Override
    public void PushIcon(Context context,JSONObject jsonObj, ValueCallback<JSONObject> callback) {
        Log.e(TAG,"layabox PushIcon param = "+jsonObj.toString());

        JSONObject result = new JSONObject();
        try {
            result.put("status", 0);
            result.put("msg", "success");
        } catch (JSONException e) {
            e.printStackTrace();
        }
        callback.onReceiveValue(result);
    }

    @Override
    public void Share(Context context,JSONObject jsonObj, ValueCallback<JSONObject> callback) {
        Log.e(TAG,"layabox Share param = "+jsonObj.toString());
        
        JSONObject result = new JSONObject();
        try {
            result.put("status", 0);
            result.put("msg", "success");
        } catch (JSONException e) {
            e.printStackTrace();
        }
        callback.onReceiveValue(result);
    }
    @Override
    public void Logout(Context context,JSONObject jsonObj, ValueCallback<JSONObject> callback) {
        Log.e(TAG,"layabox Logout param = "+jsonObj.toString());
        
        JSONObject result = new JSONObject();
        try {
            result.put("status", 0);
            result.put("msg", "success");
        } catch (JSONException e) {
            e.printStackTrace();
        }
        callback.onReceiveValue(result);        
    }
    @Override
    public void OpenBBS(Context context,JSONObject jsonObj, ValueCallback<JSONObject> callback) {
        Log.e(TAG,"layabox OpenBBS param = "+jsonObj.toString());
        JSONObject result = new JSONObject();
        try {
            result.put("status", 0);
            result.put("msg", "success");
        } catch (JSONException e) {
            e.printStackTrace();
        }
        callback.onReceiveValue(result);
    }
    @Override
    public void GetFriendsList(Context context,JSONObject jsonObj,
            ValueCallback<JSONObject> callback) {
        Log.e(TAG,"layabox GetFriendsList param = "+jsonObj.toString());
        JSONObject result = new JSONObject();
        JSONArray array = new JSONArray();
        try {
            
            JSONObject p1 = new JSONObject();
            p1.put("userId", "1111111");
            p1.put("nickName", "xiaoming");
            p1.put("photo", "http://xxx.com/xxx.jpg");
            p1.put("sex", "0");
            
            JSONObject p2 = new JSONObject();
            p2.put("userId", "1111111");
            p2.put("nickName", "xiaoming");
            p2.put("photo", "http://xxx.com/xxx.jpg");
            p2.put("sex", "0");
            array.put(p1);
            array.put(p2);
            
            result.put("status", 0);
            result.put("msg", "success");
            result.put("friends", array);
        } catch (JSONException e) {
            e.printStackTrace();
        }
        callback.onReceiveValue(result);
        
    }
    @Override
    public void SendMessageToPlatform(Context context,JSONObject jsonObj,
            ValueCallback<JSONObject> callback) {
        Log.e(TAG,"layabox SendMessageToPlatform param = "+jsonObj.toString());   
        
        if(jsonObj.has("type")){
    		String result = jsonObj.optString("type");
    		if( result.equalsIgnoreCase("ge_load_game_end") ){	//	可以去掉loadingview
//    			if( LayaPluginActivity.newActivity != null )
    			if(mCtx != null)
//    				LayaPluginActivity.newActivity.cancelPopupwindow();
    				mCtx.cancelPopupwindow();
    		}else if(result.equalsIgnoreCase("ge_loading_progress")){
    			String process = jsonObj.optString("progress");
    			int nProcess = Integer.parseInt(process);
//    			if( LayaPluginActivity.newActivity != null )
    			if(mCtx != null)
//    				LayaPluginActivity.newActivity.setLoadingExtra(nProcess);
    				mCtx.setLoadingExtra(nProcess);
    		}
            else if("runJS".equalsIgnoreCase(result))
            {
                Bundle bound=new Bundle();
                String jsContent=null;
                try {
                    String mName= jsonObj.getString("fName");
                    String mValue=jsonObj.getString("value");
                    jsContent=mName+"(\""+mValue+"\")";
                } catch (JSONException e) {
                    e.printStackTrace();
                }
                bound.putString("value",jsContent);
                mCtx.invokeMethod("runJS",bound);
//                context.getContentResolver().call(BROWSER_CONTENT_URI,"runJS",null,bound);
                return;
            }
    	}
        
        
        JSONObject result = new JSONObject();
        try {
            result.put("status", 0);
            result.put("msg", "success");
        } catch (JSONException e) {
            e.printStackTrace();
        }
        callback.onReceiveValue(result);
    }

}
