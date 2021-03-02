package com.layabox.utils;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.Log;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.net.URLConnection;

/**
 * Created by mayukai on 2016/12/7.
 */

public class Utils {
	public static void copyFromAssets(Context myContext, String assets_path) {
		AssetManager am = myContext.getAssets();
		try {
			String[] files = am.list(assets_path);
			for(String fileDic:files){
				String desDir = getPluginDir(myContext,fileDic);
				File dir = new File(desDir);
				// 如果目录不中存在，创建这个目录
				if (!dir.exists())
					dir.mkdirs();
				String soFileDir = assets_path+File.separatorChar+fileDic;
				String[] soFiles = am.list(soFileDir);//assets目录中的文件路径
				for(String soFileDic:soFiles){
					String fileName = desDir+File.separatorChar+soFileDic;//目标文件
					if (!(new File(fileName)).exists()) {
						InputStream is = myContext.getResources().getAssets()
								.open(soFileDir+File.separatorChar+soFileDic);
						FileOutputStream fos = new FileOutputStream(fileName);
						byte[] buffer = new byte[7168];
						int count = 0;
						while ((count = is.read(buffer)) > 0) {
							fos.write(buffer, 0, count);
						}
						fos.close();
						is.close();
					}
				}
			}
		} catch (IOException e) {
			e.printStackTrace();
		}

//		String filename = savePath + File.separatorChar + saveName;
//
//		try {
//			if (!(new File(filename)).exists()) {
//				InputStream is = myContext.getResources().getAssets()
//						.open(assets_path);
//				FileOutputStream fos = new FileOutputStream(filename);
//				byte[] buffer = new byte[7168];
//				int count = 0;
//				while ((count = is.read(buffer)) > 0) {
//					fos.write(buffer, 0, count);
//				}
//				fos.close();
//				is.close();
//			}
//		} catch (Exception e) {
//			e.printStackTrace();
//		}
	}
	public static String getPluginDir(Context _context,String pluginName){
		String _path = "";
		_path = _context.getFilesDir().getAbsolutePath()+ "/plugins/" +pluginName+File.separator;
		return _path;
	}



	private static Bitmap getImgFromNet(String pUrl){
		try {
			URL imageURl=new URL(pUrl);
			URLConnection con=imageURl.openConnection();
			con.connect();
			InputStream in=con.getInputStream();
			Bitmap bitmap= BitmapFactory.decodeStream(in);
			return bitmap;
		} catch (IOException e) {
			e.printStackTrace();
		}
		return null;
	}

	private static Intent getShortCutIntent(String pStartIntent, String pPackageName, String pStartClassName, String pOrientation, String pStartPlugin, GameInfo gameInfo, boolean pIsFullScreen){
		Intent intent = new Intent(pStartIntent);
		intent.addCategory("android.intent.category.DEFAULT");
		// 删除的应用程序的ComponentName，即应用程序包名+activity的名字
		intent.setComponent(new ComponentName(pPackageName,pStartClassName));
		intent.setFlags (Intent.FLAG_ACTIVITY_NEW_TASK);
		intent.putExtra("Orientation", pOrientation);
		intent.putExtra("StartPlugin", pStartPlugin);
		intent.putExtra("Option",gameInfo.getGameUrl());
		intent.putExtra("IsFullScreen", pIsFullScreen);
		return intent;
	}


	private static void addShortcutFromNet(Context ctx,String pShortCutName,Intent localIntent,Bitmap pBitmap) {
		Intent intent = new Intent("com.android.launcher.action.INSTALL_SHORTCUT");
		intent.putExtra(Intent.EXTRA_SHORTCUT_NAME, pShortCutName);
		// 是否可以有多个快捷方式的副本，参数如果是true就可以生成多个快捷方式，如果是false就不会重复添加
		intent.putExtra("duplicate", false);
		intent.putExtra(Intent.EXTRA_SHORTCUT_INTENT, localIntent);
		intent.putExtra(Intent.EXTRA_SHORTCUT_ICON, pBitmap);
//	      intent.putExtra(Intent.EXTRA_SHORTCUT_ICON_RESOURCE, Intent.ShortcutIconResource.fromContext(mContext, R.drawable.laya_bg_small));
		ctx.sendBroadcast(intent);
	}
	/**
	 *
	 * @param context
	 * @param mOrientation 横竖屏 landscape or ()
	 * @param mStartPlugin 引擎参数 layaStorePlugin
	 * @param gameInfo 游戏信息
	 * @param mIsFullScreen 设置全屏
	 */
	public static void sendToDeskTop(Context context,String mOrientation, String mStartPlugin, GameInfo gameInfo, boolean mIsFullScreen){
		String pClassname = "";
		Activity activity = (Activity) context;
		pClassname = activity.getClass().getName();
		Intent pIntent = getShortCutIntent(pClassname, context.getPackageName(), pClassname, mOrientation, mStartPlugin, gameInfo, mIsFullScreen);
		Bitmap pBitmap = getImgFromNet(gameInfo.getIconUrl());
		if (pBitmap != null) {
			addShortcutFromNet(context, gameInfo.getGameName(), pIntent, pBitmap);
		}else{
			Log.e(Config.TAG, "Utils.sendToDeskTop: bitmap is null,url = "+gameInfo.getIconUrl());
		}
	}
}
