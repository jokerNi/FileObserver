package com.tencent.qlauncher.uninstall;

import com.tencent.qlauncher.uninstall.UninstallMonitor.InitCallback;

import android.app.Service;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Handler.Callback;
import android.os.HandlerThread;
import android.os.IBinder;
import android.os.Message;
import android.os.RemoteException;
import android.util.Log;

public class UninstallReportService extends Service implements Callback 
{
    public static final String TAG = "UninstallReportService";
    public enum RequestType { SetReportUrl, Stop }
    private UninstallMonitor mMonitor;
    private boolean mInitSuccess;
    private HandlerThread mHandlerThread;
    private Handler mHandler;
    private enum SelfMessage { Msg_SetUninstallUrl, Msg_Stop }
    
    @Override
    public void onCreate()
    {
    	super.onCreate();
    	
    	mInitSuccess = false;
		mMonitor = new UninstallMonitor(getApplicationContext());
    	mMonitor.init(new InitCallback() {
			@Override
			public void onInitComplete(int result) {
				mInitSuccess = (result == 0 ? true : false);
			}
		});
    	
    	mHandlerThread = new HandlerThread("uninstall");
    	mHandlerThread.start();
    	mHandler = new Handler(mHandlerThread.getLooper(), this);
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
                		Message msg = mHandler.obtainMessage(SelfMessage.Msg_SetUninstallUrl.ordinal(), data.get("url"));
                		if (mInitSuccess)
                			mHandler.sendMessage(msg);
                		else
                			mHandler.sendMessageDelayed(msg, 2000);
                	}
                	break;
                case Stop:
		            {
		            	Message msg = mHandler.obtainMessage(SelfMessage.Msg_Stop.ordinal());
		            	mHandler.sendMessageDelayed(msg, 2000);
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

	@Override
	public boolean handleMessage(Message msg) 
	{
		SelfMessage selfMsg = SelfMessage.values()[msg.what];
		switch (selfMsg) {
			case Msg_SetUninstallUrl:
				String url = (String) msg.obj;
				if (url != null) {
        			mMonitor.setHttpRequestOnUninstall(url);
            	}
				break;
			case Msg_Stop:
				stopSelf();
				break;
		}
		return true;
	}
}
