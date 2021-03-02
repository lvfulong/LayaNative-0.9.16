package demo;

import com.layabox.layaboxbrowser.R;
import com.laya.iexternalinterface.ILoadingView;

import android.content.Context;
import android.graphics.Color;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.TextView;


public class LoadingView extends LinearLayout implements ILoadingView {
	private ProgressBar 	mProgressBar = null;
	private Context 		mContext = null;
	private LinearLayout 	mLayout = null;
	private TextView 		mTv = null;
	private View            mLoadingView = null;
	private Handler         mHandler = null;
	public LoadingView(Context context) {
		super(context);
		mContext = context;
		LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT,
				LinearLayout.LayoutParams.MATCH_PARENT);
		lp.gravity = Gravity.CENTER;
		//this.setOrientation(LinearLayout.VERTICAL);
		this.setLayoutParams(lp);
		mLoadingView = LayoutInflater.from(context).inflate(R.layout.loading, null);
		this.addView(mLoadingView,lp);
		this.setBackgroundColor(Color.WHITE);
		mHandler = new Handler(Looper.getMainLooper());
		
		if( mProgressBar == null ){
			mProgressBar = (ProgressBar)mLoadingView.findViewById(R.id.pluginLoadingProgressBar);
            mProgressBar.setMax(100);
            mProgressBar.setProgress(10);
		}
        mTv = (TextView)mLoadingView.findViewById(R.id.pluginLoadingText);
        mTv.setText("加载配置文件。。。。。。");
	}

	public void setLoadingExtra(int progress){
		progress = (int) (80+progress*0.2f);
		mProgressBar.setProgress(progress);
        mTv.setText("正在加载laya引擎，请稍后..."+(progress)+"%");
	}
	//loading开始
	@Override
	public void LoadingStart(String pluginName) {
        mProgressBar = (ProgressBar)mLoadingView.findViewById(R.id.pluginLoadingProgressBar);
        mProgressBar.setMax(100);
        mProgressBar.setProgress(10);
        mTv = (TextView)mLoadingView.findViewById(R.id.pluginLoadingText);
        mTv.setText("正在加载laya引擎，请稍后...");
	}
	//	loading进度
	@Override
	public void LoadingProgress(String pluginName,final float progress) {
	   // Log.e("",">>>>> LoadingProgress = "+progress);
    	if(mProgressBar == null)
    		return;
    	int nProgress = 10+(int) (progress*70);
        mProgressBar.setProgress(nProgress);
        mTv.setText("正在加载laya引擎，请稍后... "+nProgress+"%");
	}
	//	loading结束
	@Override
	public void LoadingFinish(String pluginName) {
		//this.removeAllViews();
	}
}
