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
#include <fstream>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <android/log.h>
#include <stdlib.h>
#include "MyLog.h"
#include "BackendServer.h"
#include "SimpleTcpClient.h"
#include "FileDeleteObserver.h"
#include "jni/NativeFileObserver_jni.h"
#include "base/jni_register_helper.h"

using namespace std;

static const int kListenPort = 53000;

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
    if (!BackendServer::IsServerAlive(kListenPort))
    {
        BackendServer::Start(kListenPort, path);
    }
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

    BackendServer::SetData(url, guid, version);

    env->ReleaseStringUTFChars(jurl, url);
    env->ReleaseStringUTFChars(jguid, guid);
    env->ReleaseStringUTFChars(jversion, version);
}

}

BASE_REGISTER_JNI_FUNC(NativeFileObserver, NativeFileObserver::RegisterNativesImpl)
