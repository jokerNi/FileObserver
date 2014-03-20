package com.tencent.qlauncher.uninstall;

import android.content.Context;

public class UninstallMonitor
{
	private static boolean sLoadSuccess = false;
    private final String TAG = "UninstallMonitor";
    private Context mContext;
    private NativeFileObserver mFileObserver;
    private boolean mStart = false;
    
    static {
    	try
        {
            System.loadLibrary("monitor");
            sLoadSuccess = true;
        }
        catch (UnsatisfiedLinkError e)
        {
            sLoadSuccess = false;
        }
    }
    
    public UninstallMonitor(Context context)
    {
        assert (context != null);
        mContext = context;
        mFileObserver = new NativeFileObserver("/data/data/" + context.getPackageName());
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
    	if (!sLoadSuccess)
    		return false;
    	
    	if (mStart)
    		return true;
    	
    	mFileObserver.startWatching();
    	mStart = true;
    	return true;
    }
}
