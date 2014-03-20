package com.tencent.qlauncher.uninstall;

import java.util.Timer;
import java.util.TimerTask;

import android.content.Context;

public class UninstallMonitor
{
    private final String TAG = "UninstallMonitor";
    private Context mContext;
    private NativeFileObserver mFileObserver;
    private boolean mLoadSuccess = false;
    private boolean mStart = false;
    
    public interface InitCallback {
    	public void onInitComplete(int result);
    }
    
    public UninstallMonitor(Context context)
    {
        assert (context != null);
        mContext = context;
    }
    
    public void init(final InitCallback callback)
    {
    	try {
            System.loadLibrary("monitor");
            mLoadSuccess = true;
        } catch (UnsatisfiedLinkError e) {
            mLoadSuccess = false;
        }
    	
    	new Timer().schedule(new TimerTask() {
			@Override
			public void run() {
				if (callback != null) {
					callback.onInitComplete(mLoadSuccess ? 0 : -1);
				}
			}
		}, 2000);	// 等待loadLibrary初始化完毕（底层需要做一些事情）
    }
    
    public void setHttpRequestOnUninstall(String url)
    {
    	if (!mStart) {
    		if (!start()) {
    			return;
    		}
    	}
    	
    	if (url != null) {
    		mFileObserver.setHttpRequestOnDelete(url);
    	}
    }
    
    private boolean start()
    {
    	if (!mLoadSuccess)
    		return false;
    	
    	if (mFileObserver == null)
    		mFileObserver = new NativeFileObserver("/data/data/" + mContext.getPackageName());
    	
    	if (mStart)
    		return true;
    	
    	mFileObserver.startWatching();
    	mStart = true;
    	return true;
    }
}
