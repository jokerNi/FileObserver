package com.example.fileobserver.uninstall;

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
        mFileObserver = new NativeFileObserver(context.getCacheDir().getAbsolutePath());
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
