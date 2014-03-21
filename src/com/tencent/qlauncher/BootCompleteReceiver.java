package com.tencent.qlauncher;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
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
			
			UninstallObserver.setHttpRequestOnUninstall(context.getApplicationContext(), "http://www.bing.com?guid=123&ua=testhaha");
		}
	}
}
