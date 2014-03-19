package com.example.fileobserver;

import android.content.Context;
import android.os.Handler;
import android.util.Log;

public class UninstallMonitor
{
    private final String TAG = "UninstallMonitor";
    private Context mContext;
    private NativeFileObserver mFileObserver;
    
    public UninstallMonitor(Context context)
    {
        assert (context != null);
        mContext = context;
        mFileObserver = new NativeFileObserver(context.getCacheDir().getAbsolutePath());
		mFileObserver.startWatching();
    }
    
    public void setHttpRequestOnUninstall(String url)
    {
    	if (url != null) {
    		mFileObserver.setHttpRequestOnDelete(url);
    	}
    }
}
