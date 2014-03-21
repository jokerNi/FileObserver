package com.tencent.qlauncher;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.os.RemoteException;

public class UninstallObserver
{
	private static String sUrl;
	
	public static int setHttpRequestOnUninstall(Context context, String url)
	{
		if (context == null || url == null)
			return -1;
		
		sUrl = url;

		bindAidlService(context);
		
		return 0;
	}
	
	private static void bindAidlService(Context context) {
        Intent serviceIntent = new Intent(context, UninstallReportService.class);
        context.startService(serviceIntent);
        context.bindService(serviceIntent, new ServiceConnection() {
			@Override
			public void onServiceDisconnected(ComponentName name) {
			}
			
			@Override
			public void onServiceConnected(ComponentName name, IBinder service) {
				
				IUninstallReport uninstallReport = IUninstallReport.Stub.asInterface(service);
				if (uninstallReport != null) {
					if (sUrl == null)
						return;
					
					Bundle bundle = new Bundle();
					bundle.putString("url", sUrl);
					try {
						uninstallReport.sendRequest(UninstallReportService.RequestType.SetReportUrl.ordinal(), bundle);
						uninstallReport.sendRequest(UninstallReportService.RequestType.Stop.ordinal(), null);
					} catch (RemoteException e) {
						e.printStackTrace();
					}
				}
			}
		}, Context.BIND_AUTO_CREATE);
    }
	
}
