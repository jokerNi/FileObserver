package com.example.fileobserver;

import android.os.Bundle;
import android.os.IBinder;
import android.os.RemoteException;
import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;

public class MainActivity extends Activity implements ServiceConnection 
{
	public static final String DEAMON = "daemon";
	private IUninstallReport mReport;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		bindAidlService(this);
	}

	@Override
	protected void onDestroy() {
		unBindAidlService(this);
		super.onDestroy();
	}
	
	private void bindAidlService(Context context) {
        Intent serviceIntent = new Intent(context, UninstallReportService.class);
        context.startService(serviceIntent);
        context.bindService(serviceIntent, this, Context.BIND_AUTO_CREATE);
    }
	
	private void unBindAidlService(Context context) {
        context.unbindService(this);
        mReport = null;
    }

	@Override
	public void onServiceConnected(ComponentName name, IBinder service) {
		mReport = IUninstallReport.Stub.asInterface(service);
		if (mReport != null) {
			Bundle bundle = new Bundle();
			bundle.putString("url", "http://www.bing.com?guid=123&ua=testua");
			try {
				mReport.sendRequest(UninstallReportService.RequestType.SetReportUrl.ordinal(), bundle);
			} catch (RemoteException e) {
				e.printStackTrace();
			}
		}
	}

	@Override
	public void onServiceDisconnected(ComponentName name) {
		mReport = null;
	}
}
