// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file is autogenerated by
//     jni_generator/jni_generator.py
// For
//     com/example/fileobserver/uninstall/NativeFileObserver

#ifndef com_example_fileobserver_uninstall_NativeFileObserver_JNI
#define com_example_fileobserver_uninstall_NativeFileObserver_JNI

#include <jni.h>

#undef LOG_TAG
#define LOG_TAG "NativeFileObserver"
#include <android/log.h>

#include "base/jni_android.h"
#include "base/scoped_java_ref.h"
#include "base/basictypes.h"
//#include "base/logging.h"

using base::ScopedJavaLocalRef;

// Step 1: forward declarations.
namespace {
const char kNativeFileObserverClassPath[] =
    "com/example/fileobserver/uninstall/NativeFileObserver";
// Leaking this jclass as we cannot use LazyInstance from some threads.
jclass g_NativeFileObserver_clazz = NULL;
}  // namespace

namespace NativeFileObserver {
static jint CreateHandler(JNIEnv* env, jobject obj);

static void StartWatching(JNIEnv* env, jobject obj,
    jint handler,
    jstring path)
{
    NativeFileObserver* native = reinterpret_cast<NativeFileObserver*>(handler);
    return native->startWatching(env, obj, path);
}

static void StopWatching(JNIEnv* env, jobject obj,
    jint handler)
{
    NativeFileObserver* native = reinterpret_cast<NativeFileObserver*>(handler);
    return native->stopWatching(env, obj);
}

static void SetOnDeleteRequestInfo(JNIEnv* env, jobject obj,
    jint handler,
    jstring url)
{
    NativeFileObserver* native = reinterpret_cast<NativeFileObserver*>(handler);
    return native->setOnDeleteRequestInfo(env, obj, url);
}

// Step 2: method stubs.

// Step 3: RegisterNatives.

static bool RegisterNativesImpl(JNIEnv* env) {

  g_NativeFileObserver_clazz = reinterpret_cast<jclass>(env->NewGlobalRef(
      env->FindClass(kNativeFileObserverClassPath)));
  static const JNINativeMethod kMethodsNativeFileObserver[] = {
    { "nativeCreateHandler",
"("
")"
"I", reinterpret_cast<void*>(CreateHandler) },
    { "nativeStartWatching",
"("
"I"
"Ljava/lang/String;"
")"
"V", reinterpret_cast<void*>(StartWatching) },
    { "nativeStopWatching",
"("
"I"
")"
"V", reinterpret_cast<void*>(StopWatching) },
    { "nativeSetOnDeleteRequestInfo",
"("
"I"
"Ljava/lang/String;"
")"
"V", reinterpret_cast<void*>(SetOnDeleteRequestInfo) },
  };
  const int kMethodsNativeFileObserverSize =
      arraysize(kMethodsNativeFileObserver);

  if (env->RegisterNatives(g_NativeFileObserver_clazz,
                           kMethodsNativeFileObserver,
                           kMethodsNativeFileObserverSize) < 0) {

  //by jarod
  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,
        "RegisterNatives failed in %s", __FILE__);

    return false;
  }

  return true;
}
}
#endif  // com_example_fileobserver_NativeFileObserver_JNI
