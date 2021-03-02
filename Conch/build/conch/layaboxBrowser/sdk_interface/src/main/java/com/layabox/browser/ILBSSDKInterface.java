package com.layabox.browser;

import android.content.Context;
import android.content.Intent;

/**
 * Created by mayukai on 2016/12/21.
 * Function:
 */

public abstract interface ILBSSDKInterface {
	public void login(Context ctx,String strJson, ILBSSDKCallback callback);
	public void pay(Context ctx,String strJson, ILBSSDKCallback callback);

	public void onPause(Context ctx);
	public void onResume(Context ctx);
	public void onCreate(Context ctx);
	public void onDestroy(Context ctx);
	public void onStop(Context context);
	public void onRestart(Context context);
	public void onActivityResult(Context context,int i, int i1, Intent intent);
	public void onNewIntent(Context context,Intent intent);
}
