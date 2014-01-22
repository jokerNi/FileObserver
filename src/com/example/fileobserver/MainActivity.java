package com.example.fileobserver;

import java.io.File;

import android.os.Bundle;
import android.os.FileObserver;
import android.app.Activity;

public class MainActivity extends Activity 
{
	private FileObserver mFileObserver;
	private static final String DEAMON = "daemon";
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		File file = new File(getCacheDir() + File.separator + DEAMON);
		if (!file.exists())
			file.mkdir();
		
		mFileObserver = new MyFileObserver(file.getAbsolutePath());
		mFileObserver.startWatching();
	}

	@Override
	protected void onDestroy()
	{
		super.onDestroy();
		mFileObserver.stopWatching();
	}
}
