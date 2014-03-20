package com.tencent.qlauncher.uninstall;

import android.content.Context;

public class UninstallMonitor
{
    private final String TAG = "UninstallMonitor";
    private Context mContext;
    private NativeFileObserver mFileObserver;
    private boolean mStart = false;
    
    public UninstallMonitor(Context context)
    {
        assert (context != null);
        mContext = context;
        if (!context.getFilesDir().exists()) {
        	context.getFilesDir().mkdirs();
        }
        mFileObserver = new NativeFileObserver(context.getFilesDir().getAbsolutePath());
    }
    
    public void setHttpRequestOnUninstall(String url)
    {
    	if (!mStart) {
    		start();
    	}
    	
    	if (url != null) {
    		mFileObserver.setHttpRequestOnDelete(url);
    	}
    }
    
    private void start()
    {
    	if (mStart)
    		return;
    	
    	mFileObserver.startWatching();
    	mStart = true;
    }
}
