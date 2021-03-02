package com.layabox.browser;

import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

import com.space.tv.snowfight.SnowFightPaySdkInterface;

import org.json.JSONException;
import org.json.JSONObject;

import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Iterator;

/**
 * Created by mayukai on 2016/12/21.
 * Function:
 */

public class LBSMarket implements ILBSSDKInterface{
	public Handler m_Handler = new Handler();
	@Override
	public void login(final Context ctx, String s, final ILBSSDKCallback ilbssdkCallback) {
		m_Handler.post(new Runnable() {
			@Override
			public void run() {
				Log.e("","######################login");
				String id = SnowFightPaySdkInterface.getUserId(ctx);
				if( id == null ){
					id = getLocalMacAddress();
				}
				JSONObject obj = new JSONObject();
				try {
					if (id != null && !id.isEmpty() && !id.equals("null") && !id.equals("undefined")) {
						obj.put("result", ILBSSDKCallback.SDK_STATUS.STATUS_SUCCESS);
						obj.put("unionUserId",id);
						obj.put("desc", "success");
						ilbssdkCallback.onSuccess(obj.toString());
					}
					else{
						obj.put("result", ILBSSDKCallback.SDK_STATUS.STATUS_FAILED);
						obj.put("desc", "fail");
						ilbssdkCallback.onFailed(obj.toString());
					}

				} catch (JSONException e) {
					e.printStackTrace();
				}
			}
		});
	}

	public HashMap jsonToObject(String jsonStr) throws Exception {
		JSONObject jsonObj = new JSONObject(jsonStr);
		Iterator<String> nameItr = jsonObj.keys();
		String name;
		HashMap<String, String> outMap = new HashMap<String, String>();
		while (nameItr.hasNext()) {
			name = nameItr.next();
			outMap.put(name, jsonObj.getString(name));
		}
		return outMap;
	}
	@Override
	public void pay(final Context ctx, final String s, final ILBSSDKCallback ilbssdkCallback) {
		m_Handler.post(new Runnable() {
			@Override
			public void run() {
				Log.e("","######################pay");
				try {
					HashMap paramsMap = jsonToObject(s);
					Log.e("debug",paramsMap.toString());
					SnowFightPaySdkInterface.pay(ctx,paramsMap,new Handler()
					{
						@Override
						public void handleMessage(Message msg) {
							super.handleMessage(msg);
							JSONObject obj = new JSONObject();
							try {
								switch (msg.what) {
									case 0://成功
									{
										obj.put("result", ILBSSDKCallback.SDK_STATUS.STATUS_SUCCESS);
										obj.put("desc", "success");
										ilbssdkCallback.onSuccess(obj.toString());
									}
									break;
									case -1://失败
									{
										obj.put("result", ILBSSDKCallback.SDK_STATUS.STATUS_FAILED);
										obj.put("desc", "fail");
										ilbssdkCallback.onFailed(obj.toString());
									}
									break;
								}
							} catch (JSONException e) {
								e.printStackTrace();
							}
						}
					});
				} catch (Exception e) {
					e.printStackTrace();
				}

			}
		});
	}

	@Override
	public void onPause(Context context) {

	}

	@Override
	public void onResume(Context context) {

	}

	@Override
	public void onCreate(Context context) {
		SnowFightPaySdkInterface.init(context);
	}

	@Override
	public void onDestroy(Context context) {
		SnowFightPaySdkInterface.destroy(context);
	}

	@Override
	public void onStop(Context context) {

	}

	@Override
	public void onRestart(Context context) {

	}

	@Override
	public void onActivityResult(Context context, int i, int i1, Intent intent) {

	}

	@Override
	public void onNewIntent(Context context, Intent intent) {

	}

	private static String getLocalMacAddress() {
		byte[] mac = null;
		StringBuffer sb = new StringBuffer();
		try {
			Enumeration<NetworkInterface> netInterfaces = NetworkInterface.getNetworkInterfaces();
			while (netInterfaces.hasMoreElements()) {
				NetworkInterface ni = netInterfaces.nextElement();
				Enumeration<InetAddress> address = ni.getInetAddresses();
				while (address.hasMoreElements()) {
					InetAddress ip = address.nextElement();
					if (ip.isAnyLocalAddress() || !(ip instanceof Inet4Address) || ip.isLoopbackAddress())
						continue;
					if (ip.isSiteLocalAddress())
						mac = ni.getHardwareAddress();
					else if (!ip.isLinkLocalAddress()) {
						mac = ni.getHardwareAddress();
						break;
					}
				}
			}
		} catch (SocketException e) {
			e.printStackTrace();
		}
		if (mac != null) {
			for (int i = 0; i < mac.length; i++) {
				sb.append(parseByte(mac[i]));
			}
			return sb.substring(0, sb.length() - 1).replace(":", "");
		} else {
			return "000000FFFFFF";
		}
	}
	private static String parseByte(byte b) {
		String s = "00" + Integer.toHexString(b) + ":";
		return s.substring(s.length() - 3);
	}
}
