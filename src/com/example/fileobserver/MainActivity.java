package com.example.fileobserver;

import android.os.Bundle;
import android.os.FileObserver;
import android.app.Activity;

public class MainActivity extends Activity 
{
	private FileObserver mFileObserver;
	public static final String DEAMON = "daemon";
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		mFileObserver = new MyFileObserver(getCacheDir().getAbsolutePath());
		mFileObserver.startWatching();
	}

	@Override
	protected void onDestroy()
	{
		super.onDestroy();
		mFileObserver.stopWatching();
	}
}
