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

#include "NativeFileObserver.h"
#include <jni.h>
#include <string>
#include "MyLog.h"
#include "BackendServer.h"
#include "base/jni_register_helper.h"
#include "jni/NativeFileObserver_jni.h"
#include "jce_header/Observer.h"
#include "ProtocolUtil.h"

using namespace std;
using namespace Observer;

extern int kListenPort;

namespace NativeFileObserver 
{
static jint CreateHandler(JNIEnv* env, jobject obj)
{
    return (jint)(new NativeFileObserver(env, obj));
}

NativeFileObserver::NativeFileObserver(JNIEnv *env, jobject obj)
{
}

void NativeFileObserver::startWatching(JNIEnv* env, jobject obj, jstring jpath)
{
    XLOG("NativeFileObserver::startWatching");
    const char* path = env->GetStringUTFChars(jpath, NULL);

    FilePath filePath;
    filePath.sFilePath = path;
    string strPath;
    ProtocolUtil::writeTo(filePath, strPath);
    
    ControlMsg msg;
    msg.eCtrlType = E_CTRL_FILE_PATH;
    msg.sSeq = "0";
    msg.vbData.assign(strPath.begin(), strPath.end());

    JceOutputStream<BufferWriter> os;
    msg.writeTo(os);
    BackendServer::SendRequest(kListenPort, os.getBuffer(), os.getLength());
}

void NativeFileObserver::stopWatching(JNIEnv* env, jobject obj)
{
    XLOG("NativeFileObserver::stopWatching");
}

void NativeFileObserver::setOnDeleteRequestInfo(JNIEnv *env, jobject obj, jstring jurl, 
            jstring jguid, jstring jversion)
{
    const char* url = env->GetStringUTFChars(jurl, NULL);
    const char* guid = env->GetStringUTFChars(jguid, NULL);
    const char* version = env->GetStringUTFChars(jversion, NULL);

    string data;
    ReqOnDel req;
    req.sUrl = url;
    req.sGuid = guid;
    ProtocolUtil::writeTo(req, data);
    
    ControlMsg msg;
    msg.eCtrlType = E_CTRL_REQ_ON_DEL;
    msg.sSeq = "0";
    msg.vbData.assign(data.begin(), data.end());

    JceOutputStream<BufferWriter> os;
    msg.writeTo(os);
    BackendServer::SendRequest(kListenPort, os.getBuffer(), os.getLength());
    
    env->ReleaseStringUTFChars(jurl, url);
    env->ReleaseStringUTFChars(jguid, guid);
    env->ReleaseStringUTFChars(jversion, version);
}

}

BASE_REGISTER_JNI_FUNC(NativeFileObserver, NativeFileObserver::RegisterNativesImpl)
