package com.tencent.qlauncher.uninstall;


interface IUninstallReport {
    

    int sendRequest(int type, in Bundle data);
    
}