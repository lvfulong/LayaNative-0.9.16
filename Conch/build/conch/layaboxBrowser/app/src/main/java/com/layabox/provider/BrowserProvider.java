package com.layabox.provider;

import android.content.ContentProvider;
import android.content.ContentValues;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;

import com.layabox.layaboxbrowser.MainActivity;

/**
 * Created by hugao on 2016/10/14.
 */

public class BrowserProvider extends ContentProvider {
    @Override
    public boolean onCreate() {
        return false;
    }

    @Override
    public Cursor query(Uri uri, String[] projection, String selection, String[] selectionArgs, String sortOrder) {
        return null;
    }

    @Override
    public String getType(Uri uri) {
        return null;
    }

    @Override
    public Uri insert(Uri uri, ContentValues values) {
        return null;
    }

    @Override
    public Bundle call(String method, String arg, Bundle extras) {
        // 当被调用“更新状态”的时候
        MainActivity.instanse.invokeMethod(method,extras);
        if ("UPDATE_STATUS".equals(method)) {
            Log.e("Launcher", "update status: " + arg);
        }

        return null;
    }

    @Override
    public int delete(Uri uri, String selection, String[] selectionArgs) {
        return 0;
    }

    @Override
    public int update(Uri uri, ContentValues values, String selection, String[] selectionArgs) {
        return 0;
    }
}
