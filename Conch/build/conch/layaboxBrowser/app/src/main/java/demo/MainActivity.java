package demo;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.view.WindowManager;
import android.webkit.ValueCallback;
import android.widget.Button;
import android.widget.Toast;

import com.layabox.layaboxbrowser.R;
import com.laya.util.BaseActivity;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;


public class MainActivity extends BaseActivity implements OnClickListener {
    PluginLauncher launcher = null;
    @Override    
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().requestFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);

//	    _btnOpenStore.setVisibility(View.GONE);

        launcher = new PluginLauncher(MainActivity.this);
        launcher.initialize(56, new ValueCallback<Byte>() {
            @Override
            public void onReceiveValue(Byte aByte) {
                int dd;

            }
        });
    }
    
    protected void onPause() 
    {
        super.onPause();
    }
    //------------------------------------------------------------------------------
    protected void onResume()
    {
        super.onResume();
    }
    
    protected void onDestroy()
    {
        super.onDestroy();
        //if( launcher != null )
        //	launcher.onDestroy();
    }

    
    @Override
    public void onClick(View v) {
	    getRuntimeABI();
            launcher.setOption("gameUrl", "http://game.yqwzzzm.layabox.com/lastBattle/h5/astoh5.max.html?relatedid=624");
//            launcher.setOption("gameUrl", "http://test.layabox.com/NewDemo/haxe/default.html?relatedId=74");
            //launcher.setOption("gameUrl", "http://http://10.10.20.9:2016/all.mm.dawawa/default.html?relatedId=43&jointypeid=50059&url=mobilegame/index.html");
            //launcher.setOption("url", "http://baidu.mm.dawawa.com/default.html?url=mobilegame/index.html&sp=40&loginType=1&jointypeid=50059&gameid=24");
            launcher.setFullScreen(true);
            launcher.setScreenOrientation("landscape");
//            launcher.start(plugin);
            launcher.start("layaPlugin");

	        launcher.setOption("gameUrl", "http://ldc.layabox.com/test/LayaAir2D/simplegamedemo.max.html");
	        launcher.setFullScreen(true);
	        launcher.setScreenOrientation("landscape");
	        launcher.start("layaAirPlugin");
    }
	private String getRuntimeABI()
	{
		Log.d("", ">>>>>>>>>>>demo");
		String arch = Build.CPU_ABI;
		String cpu_name = getCpuName();
		Toast.makeText(this, arch, Toast.LENGTH_LONG).show();
//    	 String arch1=Build.CPU_ABI2;
		if("arm64-v8a".equals(arch))
//    	 if("AArch64".equals(cpu_name) && "arm64-v8a".equals(arch))
		{
			return "layaPlugin_64";
		}
		else
		{
			return "layaPlugin";
		}
	}
    public static String getCpuName() {
        try {
                FileReader fr = new FileReader("/proc/cpuinfo");
                BufferedReader br = new BufferedReader(fr);
                String text = br.readLine();
				String[] pair = null; 
				pair = text.split(":"); 
//				if (pair.length != 2) continue; 
				String val = pair[1].trim();
				String n = "";
				for(int j =0;j<val.length();j++){
					
					if(val.charAt(j) == ' ')break;
					String temp = val.charAt(j) + ""; 
					n += temp;
				}
                String[] array = text.split(":\\s+", 2);
                for (int i = 0; i < array.length; i++) {
                }
//                return array[1];
                String tm = n;
                return tm;
        } catch (FileNotFoundException e) {
                e.printStackTrace();
        } catch (IOException e) {
                e.printStackTrace();
        }
        return null;
    }
}
