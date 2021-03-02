package com.layabox.browser;

import android.content.Context;

/**
 * Created by hugao on 2016/10/12.
 */

public class WebViewFactory {
    public static final int WEBVIEW_XWALK = 1;
    public static final int WEBVIEW_NORMAL = 2;
    public static final int WEBVIEW_TBS=3;
    public static IWebView createWebView(int type, Context context)
    {
        switch (type)
        {
            case WEBVIEW_NORMAL:
                return new NormalWebView(context);
            case WEBVIEW_XWALK:
                return new LayaBoxWebView(context);
            case WEBVIEW_TBS:
                return new NormalWebView(context);
            default:
                return new NormalWebView(context);
        }
    }
}
