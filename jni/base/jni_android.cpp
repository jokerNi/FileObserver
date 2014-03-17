#include "base/jni_android.h"
#include "base/basictypes.h"
#include <android/log.h>
#undef LOG_TAG
#define LOG_TAG "jni_android"

namespace base {

JavaVM *g_jvm = NULL;

void InitVM(JavaVM *vm)
{
	g_jvm = vm;
}

JNIEnv *AttachCurrentThread()
{
	JNIEnv *env = NULL;
	jint ret = g_jvm->AttachCurrentThread(&env, NULL);
	return env;
}

JNIEnv *GetEnv()
{
	JNIEnv *env = NULL;
	g_jvm->GetEnv((void**)&env, JNI_VERSION_1_4);
	return env;
}



void DetachCurrent()
{
	g_jvm->DetachCurrentThread();
}

void CheckException(JNIEnv* env)
{
    if (env->ExceptionCheck() != 0)
    {
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "Uncaught java exception");
        env->ExceptionDescribe();
    }
}

std::string ConvertJavaStringToUTF8(JNIEnv *env, jstring str)
{
	if(!str)
	{
		return "";
	}

	std::string result;

	const char *chars = env->GetStringUTFChars(str, NULL);

	result = chars;

	env->ReleaseStringUTFChars(str, chars);

	CheckException(env);

	return result;
}

ScopedJavaLocalRef<jstring> ConvertUTF8ToJavaString(JNIEnv *env, std::string str)
{

   jclass strClass = env->FindClass("java/lang/String");

   jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");

   jbyteArray bytes = env->NewByteArray(str.length());

   env->SetByteArrayRegion(bytes, 0, str.length(), (jbyte*)str.data());

   jstring encoding = env->NewStringUTF("UTF-8");

   jstring ret = (jstring)env->NewObject(strClass, ctorID, bytes, encoding);

   env->DeleteLocalRef(bytes);

   env->DeleteLocalRef(encoding);

   env->DeleteLocalRef(strClass);

   ScopedJavaLocalRef<jstring> sj(env, ret);

   return sj;

}

std::string ConvertJbyteArrayToString(JNIEnv *env, jbyteArray jarray)
{
	jbyte *c = env->GetByteArrayElements(jarray, NULL);

	env->ReleaseByteArrayElements(jarray, c, 0);

	return std::string((char*)c, env->GetArrayLength(jarray));
}

//ScopedJavaLocalRef<jbyteArray> ConvertStringToJbyteArray(JNIEnv *env, std::string buffer)
//{
//	jbyteArray jarray = env->NewByteArray(buffer.size());
//
//	env->SetByteArrayRegion(jarray, 0, buffer.size(), (jbyte *)buffer.c_str());
//
//	return jarray;
//}

} // end of namespace base
