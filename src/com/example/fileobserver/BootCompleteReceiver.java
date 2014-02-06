package com.example.fileobserver;

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
			
			NativeFileObserver observer = new NativeFileObserver(context.getCacheDir().getAbsolutePath());
			observer.startWatching();
		}
	}
}
