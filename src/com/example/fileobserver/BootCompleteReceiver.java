package com.example.fileobserver;

import java.io.File;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.FileObserver;
import android.util.Log;

public class BootCompleteReceiver extends BroadcastReceiver
{
	@Override
	public void onReceive(Context context, Intent intent) 
	{
		final String action = intent.getAction();
		Log.d("BootCompleteReceiver::onReceive", action);
		if(Intent.ACTION_BOOT_COMPLETED.equals(action)) 
		{
			Log.d("BootCompleteReceiver::onReceive", "receive complete");
			File file = new File(context.getCacheDir() + File.separator + MainActivity.DEAMON);
			if (!file.exists())
				file.mkdir();
			
			FileObserver observer = new MyFileObserver(file.getAbsolutePath());
			observer.startWatching();
		}
	}
}
