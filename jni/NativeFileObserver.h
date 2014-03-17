#ifndef NATIVE_FILE_OBSERVER_H_
#define NATIVE_FILE_OBSERVER_H_

#include <jni.h>
#include <string>


namespace NativeFileObserver
{

class NativeFileObserver
{
public:
    NativeFileObserver(JNIEnv* env, jobject obj);
    void startWatching(JNIEnv* env, jobject obj, jstring jpath);
    void stopWatching(JNIEnv* env, jobject obj);
    void setOnDeleteRequestInfo(JNIEnv *env, jobject obj, jstring jurl, 
            jstring jguid, jstring jversion);

private:
    std::string mUrl;
    std::string mGuid;
    std::string mVersion;
};

}
#endif  // NATIVE_FILE_OBSERVER_H_