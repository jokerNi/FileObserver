package com.example.fileobserver;

import android.util.Log;

public class NativeFileObserver
{
    private static final String TAG = "NativeFileObserver";
    private static boolean sLoadSuccess = false;
    private String mPath;
    private int mHandler;
    
    public NativeFileObserver(String path) 
    {
        mPath = path;
        
        if (sLoadSuccess == false)
        {
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
	public void setHttpRequestOnDelete(String url, String guid, String version)
	{
        nativeSetOnDeleteRequestInfo(mHandler, url, guid, version);
	}

	private native int nativeCreateHandler();
	private native void nativeStartWatching(int handler, String path);
	private native void nativeStopWatching(int handler);
	private native void nativeSetOnDeleteRequestInfo(int handler, String url, String guid, String version);
}
