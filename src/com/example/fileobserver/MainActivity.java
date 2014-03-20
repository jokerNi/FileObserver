package com.example.fileobserver;

import com.example.fileobserver.uninstall.UninstallObserver;

import android.os.Bundle;
import android.app.Activity;

public class MainActivity extends Activity
{
	public static final String DEAMON = "daemon";
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		UninstallObserver.setHttpRequestOnUninstall(getApplicationContext(), "http://www.bing.com?guid=123&ua=testhaha");
	}
}
