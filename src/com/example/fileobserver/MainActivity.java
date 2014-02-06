package com.example.fileobserver;

import android.os.Bundle;
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
		mFileObserver.startWatching();
	}

	@Override
	protected void onDestroy()
	{
		super.onDestroy();
		mFileObserver.stopWatching();
	}
}
