package layaair.game.browser;

import android.app.Activity;
import android.os.Handler;
import android.os.Looper;

import com.google.zxing.integration.android.IntentIntegrator;

/**
 * Created by lvfulong on 2018/1/16.
 */

public class ScriptBridge {
    public static Handler m_Handler = new Handler(Looper.getMainLooper());
    public static Activity mMainActivity = null;
    public static void scan() {
        m_Handler.post(
                new Runnable() {
                    public void run() {
                        new IntentIntegrator(mMainActivity).setCaptureActivity(ScanActivity.class).initiateScan();
                    }
                });
    }
    public static void onScanResult(String result) {
        ExportJavaFunction.CallBackToJS(ScriptBridge.class,"scan",result);
    }
}
