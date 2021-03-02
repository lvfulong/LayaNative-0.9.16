package com.layabox.layaboxbrowser;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.TextView;

import com.layabox.utils.Config;

import org.xwalk.core.XWalkInitializer;
import org.xwalk.core.XWalkUpdater;
import org.xwalk.core.XWalkView;

import static org.xwalk.core.extension.JsStubGenerator.TAG;

public class XWalkDownloadActivity extends Activity
        implements XWalkInitializer.XWalkInitListener,
        XWalkUpdater.XWalkBackgroundUpdateListener {

    XWalkInitializer mXWalkInitializer;
    XWalkUpdater mXWalkUpdater;
    XWalkView mXWalkView;
    FrameLayout mLoadingLayout;
    ImageView pLogoImage;
    private long mStartTime = 0;
    private TextView mLoadingText;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        Log.e("", "onCreate: XWalkDownloadActivity");
        mXWalkInitializer = new XWalkInitializer(this, this);
        mXWalkInitializer.initAsync();

        FrameLayout.LayoutParams params = new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT);
//        mLoadingLayout = new FrameLayout(this);
//        mLoadingLayout.setLayoutParams(params);
//        mLoadingLayout.setBackgroundColor(Color.WHITE);
//        pLogoImage = new ImageView(this);
//        pLogoImage.setLayoutParams(params);
//
//        pLogoImage.setImageDrawable(getResources().getDrawable(R.drawable.splash_landscape));
//        mLoadingLayout.addView(pLogoImage,params);
        setContentView(R.layout.main_loading);
        mLoadingText = (TextView) findViewById(R.id.pluginLoadingText);
        mStartTime = System.currentTimeMillis();
    }

    @Override
    protected void onResume() {
        super.onResume();
        mXWalkInitializer.initAsync();
    }

    @Override
    public void onXWalkInitStarted() {
    }

    @Override
    public void onXWalkInitCancelled() {
        finish();
    }

    @Override
    public void onXWalkInitFailed() {
        // Initialization failed. Trigger the Crosswalk runtime download
        if (mXWalkUpdater == null) {
            mXWalkUpdater = new XWalkUpdater(this, this);
        }
        mXWalkUpdater.updateXWalkRuntime();
    }

    @Override
    public void onXWalkInitCompleted() {
        // Initialization successfully, ready to invoke any XWalk embedded API
        //mXWalkView.load("file:///android_asset/create_window_1.html", null);
        if( mStartTime<=0 ){
            return;
        }else if(System.currentTimeMillis() - mStartTime< Config.LauncherLoadingMinTime){
            new Handler().postDelayed(new Runnable() {
                @Override
                public void run() {
                    onXWalkInitCompleted();
                }
            },System.currentTimeMillis() - mStartTime);
            return;
        }
        Intent intent =new Intent(XWalkDownloadActivity.this,MainActivity.class);
        startActivity(intent);
        finish();
        mStartTime = 0;
    }

    @Override
    public void onXWalkUpdateStarted() {
    }

    @Override
    public void onXWalkUpdateProgress(int percentage) {
        Log.d(TAG, "XWalkUpdate progress: " + percentage);
        if( mLoadingText != null ) {
            mLoadingText.setText("loading..." + percentage + "%");
        }
    }

    @Override
    public void onXWalkUpdateCancelled() {
        finish();
    }

    @Override
    public void onXWalkUpdateFailed() {
        finish();
    }

    @Override
    public void onXWalkUpdateCompleted() {
        // Crosswalk Runtime update finished, re-init again.
        mXWalkInitializer.initAsync();
    }
}
