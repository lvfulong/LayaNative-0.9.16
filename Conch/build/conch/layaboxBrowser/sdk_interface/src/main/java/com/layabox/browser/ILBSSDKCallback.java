package com.layabox.browser;

/**
 * Created by mayukai on 2016/12/21.
 * Function:
 */

public abstract interface ILBSSDKCallback {
	public class SDK_STATUS{
		static public int STATUS_SUCCESS = 0;
		static public int STATUS_FAILED = -1;
		static public int STATUS_CANCEL = -2;
	}
	public void onSuccess(String strJson);
	public void onFailed(String strJson);
	public void onCancel(String strJson);
}
