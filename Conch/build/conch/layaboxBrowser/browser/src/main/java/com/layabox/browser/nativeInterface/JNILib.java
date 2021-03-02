package com.layabox.browser.nativeInterface;

import android.content.res.AssetManager;

/**
 * Created by kippahu on 2016/11/11.
 */

public class JNILib {
    public static native void InitDLib(AssetManager p_am, String strAssetRootPath, String strCachePath, String strHallUrl);
    public static native byte[] interceptLoadRequest(String strUrl);
    public static native void saveCache(String strUrl,byte[] data);
    public static native String preUpdateDcc(String strUrl,String strUrlDomain);
    public static native void updateDccClearAssetsid(String strUrl,String strUrlDomain);
    public static native void doUpdateDcc(String strUrl,String strUrlDomain,String txtdcc,String assetsidStr);
}
