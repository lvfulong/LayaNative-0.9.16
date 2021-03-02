package demo;

import android.app.Activity;
import android.app.Dialog;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.Gravity;
import android.view.SurfaceView;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.webkit.ValueCallback;
import android.widget.FrameLayout;

import com.laya.iexternalinterface.IPluginListener;
import com.laya.plugin.ILayaBoxPlugin;
import com.laya.plugin.LayaPluginManager;
import com.layabox.utils.LogUtil;

import org.json.JSONException;
import org.json.JSONObject;

import FloatMenu.MyWindowManager;


public class LayaPluginActivity extends Activity implements IPluginListener {

	LoadingView mLoadingView = null;
	private int mnOrientation = ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE;
	public final static Uri BROWSER_CONTENT_URI = Uri.parse("content://com.layabox.provider.BrowserProvider");
	private String mOrientation = "landscape";
	private boolean mIsFullScreen = true;
	private String mStartPlugin = "";
	private ILayaBoxPlugin mPlugin = null;
	private FrameLayout loadingLayout = null;    // loading条的layout
	private FrameLayout gameLayout = null;    //	游戏的layout
	private LoadingView curLoading = null;
	private SurfaceView mViewForAvoidingBlink = null;
	private String mOption = null;
	private String gameId = "";
	private String mGameName = "";
	private String mGameUrl = "";
	private String mGameIcon = "";
	private int mSpID = 0;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		mLoadingView = new LoadingView(this);
		Intent i = getIntent();

		mIsFullScreen = i.getBooleanExtra("IsFullScreen", true);
		mOrientation = i.getStringExtra("Orientation");
		mStartPlugin = i.getStringExtra("StartPlugin");
		mSpID = i.getIntExtra("spid", 0);

		mOption = i.getStringExtra("Option");
		String[] optionGrp = mOption.split(";");

		for (String op : optionGrp) {
			String tmp[] = op.split(",");
			switch (tmp[0]) {
				case "gameId":
					gameId = tmp[1];
					break;
				case "gameName":
					mGameName = tmp[1];
					break;
				case "gameIcon":
					mGameIcon = tmp[1];
					break;
				case "gameUrl":
					mGameUrl = tmp[1];
					break;
			}
		}

		if (mOrientation.equals("portrait")) {
			mnOrientation = ActivityInfo.SCREEN_ORIENTATION_PORTRAIT;
		} else if (mOrientation.equals("landscape"))
			mnOrientation = ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE;

		if (this.getRequestedOrientation() != mnOrientation) {
			this.setRequestedOrientation(mnOrientation);
		}
		if (mIsFullScreen) {
			getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
					WindowManager.LayoutParams.FLAG_FULLSCREEN);
		}

		if (curLoading == null)
			curLoading = new LoadingView(LayaPluginActivity.this);
		showPopwindow();
	}

	private Dialog mLoadingDialog = null;

	public void showPopwindow() {
		FrameLayout.LayoutParams localLayoutParams = new FrameLayout.LayoutParams(
				WindowManager.LayoutParams.MATCH_PARENT,
				WindowManager.LayoutParams.MATCH_PARENT, Gravity.TOP);
		if (gameLayout == null) {
			gameLayout = new FrameLayout(this);
			gameLayout.setFocusableInTouchMode(true);
			this.addContentView(gameLayout, localLayoutParams);
			mViewForAvoidingBlink = new SurfaceView(this);
			gameLayout.addView(mViewForAvoidingBlink);
			gameLayout.post(new Runnable() {
				@Override
				public void run() {
					// TODO Auto-generated method stub
					gameLayout.removeView(mViewForAvoidingBlink);
				}
			});

		}
		if (loadingLayout == null) {
			if (curLoading.getParent() != null)
				((FrameLayout) curLoading.getParent()).removeView(curLoading);
			loadingLayout = new FrameLayout(this);
			loadingLayout.setLayoutParams(localLayoutParams);
			loadingLayout.setFocusableInTouchMode(true);
			loadingLayout.addView(curLoading);
			this.addContentView(loadingLayout, localLayoutParams);

		}
		Handler popupHandler = new Handler() {
			@Override
			public void handleMessage(Message msg) {
				switch (msg.what) {
					case 0:
						loadingLayout.setVisibility(View.VISIBLE);

						JSONObject extJson = new JSONObject();
						try {
							extJson.put("gameId", "100");
						} catch (JSONException e) {
							e.printStackTrace();
						}
						LayaPluginManager.getInstance().Initialization(mSpID, extJson, LayaPluginActivity.this, new ValueCallback<Byte>() {
							@Override
							public void onReceiveValue(Byte value) {
								if (value == LayaPluginManager.CONFIG_INIT_OK) {
									mPlugin = LayaPluginManager.getInstance().GetLayaBoxPlugin(mStartPlugin, LayaPluginActivity.this, curLoading, LayaPluginActivity.this);
									Log.e("", "LayaPluginActivity options = " + mOption + " plugin = " + mPlugin);
									String[] optionGrp = mOption.split(";");
									for (String op : optionGrp) {
										String tmp[] = op.split(",");
										Log.e("", "LayaPluginActivity SetOption key = " + tmp[0] + " value=" + tmp[1]);
										mPlugin.SetOption(tmp[0], tmp[1]);
									}
									mPlugin.SetScreenOrientation(mOrientation);
									mPlugin.SetFullScreen(mIsFullScreen);
//									mPlugin.SetExternalPorxy(new PluginSDK(LayaPluginActivity.this));
									Log.e("LayaPluginActivity", "LayaPluginActivity start game!!");
									mPlugin.Start(mStartPlugin);
									//显示悬浮按钮的操作，下面是以悬浮按钮的事例
									MyWindowManager.createSmallWindow(LayaPluginActivity.this);
								}
							}
						});
						break;
				}
			}
		};
		popupHandler.sendEmptyMessageDelayed(0, 100);

	}

	public void setLoadingExtra(int progress) {
		curLoading.setLoadingExtra(progress);
	}

	public void cancelPopupwindow() {
		LogUtil.e("", "cancelPopupwindow");
		loadingLayout.setVisibility(View.GONE);
		gameLayout.setVisibility(View.VISIBLE);
	}

	@Override
	protected void onPause() {
		super.onPause();
		LayaPluginManager.getInstance().onPause();
	}

	@Override
	protected void onResume() {
		super.onResume();
		LayaPluginManager.getInstance().onResume();
		//LayaPluginManager.getInstance().
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
		loadingLayout = null;
		curLoading = null;

		LayaPluginManager.getInstance().onExitGame();
		LayaPluginManager.getInstance().onDestroy();
		// 关闭悬浮按钮
		MyWindowManager.removeSmallWindow(this);
		android.os.Process.killProcess(android.os.Process.myPid());
		Log.d("", ">>>>>>>>>>>>>>>Plugin onDestroy");
	}

	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		super.onConfigurationChanged(newConfig);
		LayaPluginManager.getInstance().onConfigurationChanged(newConfig);
	}

	@Override
	public void onBackPressed() {
		super.onBackPressed();
		this.finish();
		return;
	}

	@Override
	public void Plugin_Finish() {

	}

	@Override
	public void onNoEnoughSpace() {

	}

	/**
	 * 插件开始运行
	 * gamePlugin：插件实例
	 * view:显示游戏的view
	 */
	@Override
	public void Plugin_Start(Object gamePlugin, View _view) {
		FrameLayout.LayoutParams localLayoutParams = new FrameLayout.LayoutParams(
				WindowManager.LayoutParams.MATCH_PARENT,
				WindowManager.LayoutParams.MATCH_PARENT, Gravity.BOTTOM);
		gameLayout.addView(_view);

		if (this.getRequestedOrientation() != mnOrientation) {
			this.setRequestedOrientation(mnOrientation);
		}
		SharedPreferences perfer = PreferenceManager.getDefaultSharedPreferences(this);
		boolean curFull = perfer.getBoolean("fullscreen", false);
		if (!curFull && mIsFullScreen)
			this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
					WindowManager.LayoutParams.FLAG_FULLSCREEN);
		else if (curFull && !mIsFullScreen) {
			quitFullScreen();
		}
		cancelPopupwindow();
	}

	private void quitFullScreen() {
		final WindowManager.LayoutParams attrs = this.getWindow().getAttributes();
		attrs.flags &= (~WindowManager.LayoutParams.FLAG_FULLSCREEN);
		this.getWindow().setAttributes(attrs);
		this.getWindow().clearFlags(WindowManager.LayoutParams.FLAG_LAYOUT_NO_LIMITS);
	}
}
