/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <jni.h>
#include <string>
#include <fstream>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <android/log.h>
#include "MyLog.h"
#include "EchoTcpServer.h"
#include "SimpleTcpClient.h"
#include "FileDeleteObserver.h"
#include "jni/NativeFileObserver_jni.h"
#include "base/jni_register_helper.h"
#include <curl/curl.h>

using namespace std;
bool gKeepAliveDaemonProcess = true;

namespace NativeFileObserver 
{
static void reallyStartWatching(const char* path);
bool isDaemonRunning();
void* DaemonEchoThread(void* params);
int testCurl();

static EchoTcpServer* sEchoServer = NULL;
static string sUrl;
static string sGuid;
static string sVersion;

typedef void* (*ThreadProc)(void*);
int createThread(ThreadProc proc)
{
    bool success = false;

    pthread_t threadId;
    pthread_attr_t attributes;
    pthread_attr_init(&attributes);

    success = !pthread_create(&threadId, &attributes, proc, NULL);

    pthread_attr_destroy(&attributes);

    return success;
}
static const int kListenPort = 53000;

static void StartWatching(JNIEnv* env, jobject obj, jstring jpath)
{
    XLOG("StartWatching begin");
    const char* path = env->GetStringUTFChars(jpath, NULL);
    if (isDaemonRunning())
    {
        XLOG("StartWatching daemon already exist, return");
        return;
    }

    pid_t pid;
    pid = fork();
    if (pid < 0)
    {
        XLOG("fork failed");
    }
    else if (pid == 0)
    {
        XLOG("in new process, id is %d, ppid is %d", getpid(), getppid());
        reallyStartWatching(path);
    }
    else
    {
        XLOG("in origin process, id is %d", getpid());
        env->ReleaseStringUTFChars(jpath, path);
    }
}

static void StopWatching(JNIEnv* env, jobject obj)
{
    XLOG("nativeStopWatching");
}

static void SetOnDeleteRequestInfo(JNIEnv* env, jobject obj, jstring jurl, 
    jstring jguid, jstring jversion)
{
    const char* url = env->GetStringUTFChars(jurl, NULL);
    const char* guid = env->GetStringUTFChars(jguid, NULL);
    const char* version = env->GetStringUTFChars(jversion, NULL);

    sUrl = url;
    sGuid = guid;
    sVersion = version;
    
    XLOG("nativeSetOnDeleteRequestInfo url=%s, guid=%s, version=%s", url, guid, version);

    env->ReleaseStringUTFChars(jurl, url);
    env->ReleaseStringUTFChars(jguid, guid);
    env->ReleaseStringUTFChars(jversion, version);
}


static void reallyStartWatching(const char* path)
{
    XLOG("reallyStartWatching path=%s", path);
    createThread(DaemonEchoThread);
    FileDeleteObserver observer(path);
    observer.setHttpRequestOnDelete(sUrl, sGuid, sVersion);
    observer.startWatching();

    while (gKeepAliveDaemonProcess)
    {
        //XLOG("StartWatching in while loop");

        usleep(1000 * 1000 * 2);
        //break;
        
        //XLOG("StartWatching leave while loop");
    }

    if (sEchoServer)
        sEchoServer->stop();

    XLOG("reallyStartWatching exit");
    usleep(1000 * 1000 * 10);   // Wait a while for other components finish exist
    exit(0);
}

void* DaemonEchoThread(void* params)
{
    XLOG("DaemonEchoThread start");

    sEchoServer = new EchoTcpServer(kListenPort);
    sEchoServer->start();

    XLOG("DaemonEchoThread end");
}

bool isDaemonRunning()
{
    return EchoTcpServer::isServerAlive(kListenPort);
}

int testCurl()
{
  CURL *curl;
  CURLcode res;
 
  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "http://www.baidu.com");
    /* example.com is redirected, so we tell libcurl to follow redirection */ 
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
 
    /* Perform the request, res will get the return code */ 
    res = curl_easy_perform(curl);
    /* Check for errors */ 
    if(res != CURLE_OK)
    {
      XLOG("curl_easy_perform() failed: %s", curl_easy_strerror(res));
    }
    else
    {
      long retcode = 0;
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &retcode);
      XLOG("curl_easy_perform() success, response code=%d", retcode);
    }
 
    /* always cleanup */ 
    curl_easy_cleanup(curl);
  }
  return 0;
}
}

BASE_REGISTER_JNI_FUNC(NativeFileObserver, NativeFileObserver::RegisterNativesImpl)

