// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/scoped_java_ref.h"
#include "base/jni_android.h"

namespace base {

JavaRef<jobject>::JavaRef() : obj_(NULL) {}

JavaRef<jobject>::JavaRef(JNIEnv* env, jobject obj) : obj_(obj) {
  // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  // comment out by gavinzhou
  /*
  if (obj) {
    DCHECK(env && env->GetObjectRefType(obj) == JNILocalRefType);
  }
  */
  // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
}

JavaRef<jobject>::~JavaRef() {
}

JNIEnv* JavaRef<jobject>::SetNewLocalRef(JNIEnv* env, jobject obj) {
  if (!env) {
    env = AttachCurrentThread();
  } else {
    // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    // comment out by gavinzhou
    //DCHECK_EQ(env, AttachCurrentThread());  // Is |env| on correct thread.
    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
  }
  if (obj)
    obj = env->NewLocalRef(obj);
  if (obj_)
    env->DeleteLocalRef(obj_);
  obj_ = obj;
  return env;
}

void JavaRef<jobject>::SetNewGlobalRef(JNIEnv* env, jobject obj) {
  if (!env) {
    env = AttachCurrentThread();
  } else {
    // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    // comment out by gavinzhou
    //DCHECK_EQ(env, AttachCurrentThread());  // Is |env| on correct thread.
    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
  }
  if (obj)
    obj = env->NewGlobalRef(obj);
  if (obj_)
    env->DeleteGlobalRef(obj_);
  obj_ = obj;
}

void JavaRef<jobject>::ResetLocalRef(JNIEnv* env) {
  if (obj_) {
    // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    // comment out by gavinzhou
    //DCHECK_EQ(env, AttachCurrentThread());  // Is |env| on correct thread.
    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    env->DeleteLocalRef(obj_);
    obj_ = NULL;
  }
}

void JavaRef<jobject>::ResetGlobalRef() {
  if (obj_) {
    AttachCurrentThread()->DeleteGlobalRef(obj_);
    obj_ = NULL;
  }
}

jobject JavaRef<jobject>::ReleaseInternal() {
  jobject obj = obj_;
  obj_ = NULL;
  return obj;
}

}  // namespace base

#undef AttachCurrentThread
