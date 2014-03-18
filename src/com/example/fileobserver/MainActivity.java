package com.example.fileobserver;

import android.os.Bundle;
import android.os.Handler;
import android.app.Activity;

public class MainActivity extends Activity 
{
	private NativeFileObserver mFileObserver;
	public static final String DEAMON = "daemon";
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		mFileObserver = new NativeFileObserver(getCacheDir().getAbsolutePath());
		
		new Handler().postDelayed(new Runnable() {
			@Override
			public void run() {
				mFileObserver.startWatching();
				mFileObserver.setHttpRequestOnDelete("http://www.bing.com?guid=123&ua=test");
			}
		}, 1000);
	}

	@Override
	protected void onDestroy()
	{
		super.onDestroy();
	}
}
