package demo;

import android.app.Application;

//import com.squareup.leakcanary.LeakCanary;

/**
 * Created by user on 2016/4/21.
 */
public class DemoApplication extends Application {
	@Override
	public void onCreate() {
		super.onCreate();
//		LeakCanary.install(this);
	}
}
