package com.layabox.browser;

import android.webkit.ValueCallback;

import org.json.JSONObject;

/**
 * Created by hugao on 2016/10/11.
 */

public interface IWebViewListener {
    void openGame(JSONObject gameInfo, ValueCallback<Boolean> openCallback);
    void runJS(String jsContent);
    void hideWebview(boolean  hide);
}
