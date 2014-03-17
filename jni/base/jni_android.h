#ifndef BASE_ANDROID_JNI_ANDROID_MTT_H_
#define BASE_ANDROID_JNI_ANDROID_MTT_H_

#include <jni.h>
#include <string>
#include "base/scoped_java_ref.h"
namespace base {

void InitVM(JavaVM *vm);

JNIEnv *AttachCurrentThread();

void DetachCurrent();

JNIEnv *GetEnv();

void CheckException(JNIEnv* env);

std::string ConvertJavaStringToUTF8(JNIEnv *env, jstring str);

ScopedJavaLocalRef<jstring> ConvertUTF8ToJavaString(JNIEnv *env, std::string str);

std::string ConvertJbyteArrayToString(JNIEnv *env, jbyteArray buffer);

//ScopedJavaLocalRef<jbyteArray> ConvertStringToJbyteArray(JNIEnv *env, std::string buffer);

} // end of namespace base

#endif // endof BASE_ANDROID_JNI_ANDROID_MTT_H_
