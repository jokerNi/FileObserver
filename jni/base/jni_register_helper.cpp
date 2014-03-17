
#include "base/jni_register_helper.h"
#include <android/log.h>

#undef LOG_TAG
#define LOG_TAG "jni_android"

namespace base {

bool JNIRegisterHelper::doRegister(JNIEnv* env)
{
    __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG,
    		"doRegister, size:%d",
    		register_jni_func_->size());

    for (size_t i = 0; i < register_jni_func_->size(); i++) {

        __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG,
                            "doRegister, name:%s", 
                            register_jni_func_->at(i).first);

        if(!register_jni_func_->at(i).second(env))
            return false;
    }

    return true;
}

}

