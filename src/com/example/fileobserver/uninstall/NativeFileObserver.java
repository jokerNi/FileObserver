package com.example.fileobserver.uninstall;

import android.util.Log;

public class NativeFileObserver
{
    private static final String TAG = "NativeFileObserver";
    private static boolean sLoadSuccess = false;
    private String mPath;
    private int mHandler;
    
    static {
    	try
        {
            System.loadLibrary("monitor");
            sLoadSuccess = true;
        }
        catch (UnsatisfiedLinkError e)
        {
            Log.e(TAG, e.toString());
            sLoadSuccess = false;
        }
    }
    
    public NativeFileObserver(String path) 
    {
        mPath = path;
        if (sLoadSuccess)
        	mHandler = nativeCreateHandler();
    }
    
    public void startWatching()
    {
        if (sLoadSuccess)
            nativeStartWatching(mHandler, mPath);
    }
    
    public void stopWatching()
    {
        if (sLoadSuccess)
            nativeStopWatching(mHandler);
    }
	public void setHttpRequestOnDelete(String url)
	{
        nativeSetOnDeleteRequestInfo(mHandler, url);
	}

	private native int nativeCreateHandler();
	private native void nativeStartWatching(int handler, String path);
	private native void nativeStopWatching(int handler);
	private native void nativeSetOnDeleteRequestInfo(int handler, String url);
}
