package com.example.fileobserver.uninstall;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.os.RemoteException;

public class UninstallObserver
{
	private static IUninstallReport sUninstallReport;
	private static String sUrl;
	private static UninstallServiceConnection mServiceConnection;
	
	public static int setHttpRequestOnUninstall(Context context, String url)
	{
		if (context == null || url == null)
			return -1;
		
		sUrl = url;
		if (mServiceConnection == null) {
			mServiceConnection = new UninstallServiceConnection();
		}

		bindAidlService(context);
		
		return 0;
	}
	
	private static void bindAidlService(Context context) {
        Intent serviceIntent = new Intent(context, UninstallReportService.class);
        context.startService(serviceIntent);
        context.bindService(serviceIntent, mServiceConnection, Context.BIND_AUTO_CREATE);
    }
	
	private static class UninstallServiceConnection implements ServiceConnection
	{
		@Override
		public void onServiceConnected(ComponentName name, IBinder service) {
			sUninstallReport = IUninstallReport.Stub.asInterface(service);
			if (sUninstallReport != null) {
				Bundle bundle = new Bundle();
				bundle.putString("url", sUrl);
				try {
					sUninstallReport.sendRequest(UninstallReportService.RequestType.SetReportUrl.ordinal(), bundle);
				} catch (RemoteException e) {
					e.printStackTrace();
				}
			}
		}

		@Override
		public void onServiceDisconnected(ComponentName name) {
			sUninstallReport = null;
		}
	}
}
