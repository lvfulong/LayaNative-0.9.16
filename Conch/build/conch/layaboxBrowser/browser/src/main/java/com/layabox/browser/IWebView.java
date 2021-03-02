package com.layabox.browser;

import android.content.Intent;

import org.json.JSONObject;

/**
 * Created by hugao on 2016/10/9.
 */

public interface IWebView {
    void onActivityResult(int requestCode, int resultCode, Intent data);
    void onNewIntent(Intent intent);
    void onDestroy();
    void loadUrl(String url);
    void runJS(String jsContent);
    void postMessage(String messageJSON,String targetOrigin);
    void setWebViewListener(IWebViewListener listener);
    void openGame(JSONObject gameInfo);
    void closeGame();
    void resumeTimers();
    void setVisibility(int visibility);
    void updateDcc(JSONObject gameInfo);
    IWebViewListener getWebViewListener();
}
