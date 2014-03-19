package com.example.fileobserver.uninstall;


interface IUninstallReport {
    

    int sendRequest(int type, in Bundle data);
    
}