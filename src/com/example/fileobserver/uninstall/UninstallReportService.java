package com.example.fileobserver.uninstall;

import android.app.Service;
import android.content.Intent;
import android.os.Bundle;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;

public class UninstallReportService extends Service 
{
    public static final String TAG = "RemoteService";
    public enum RequestType { SetReportUrl }
    private UninstallMonitor mMonitor;
    
    @Override
    public void onCreate()
    {
    	super.onCreate();
    	if (mMonitor == null) {
    		mMonitor = new UninstallMonitor(getApplicationContext());
    	}
    }
    
    private IUninstallReport.Stub mBinder = new IUninstallReport.Stub() 
    {
        @Override
        public int sendRequest(int type, Bundle data) throws RemoteException 
        {
            RequestType reqType = RequestType.values()[type];
            Log.d(TAG, "sendRemoteRequest type=" + reqType);
            int result = -1;
            switch (reqType)
            {
                case SetReportUrl:
                	if (data != null) {
                		String url = data.getString("url");
                		if (url != null) {
                			mMonitor.setHttpRequestOnUninstall(url);
                			result = 0;
                		}
                	}
                    break;
                default:
                    break;
            }
            
            return result;
        }
    };
    
    @Override
    public IBinder onBind(Intent intent) 
    {
        return mBinder;
    }
}
