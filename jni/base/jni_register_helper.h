#ifndef JNI_REGISTER_HELPER_H_
#define JNI_REGISTER_HELPER_H_

#include "util/tc_functor.h"
#include <android/log.h>
#include <jni.h>
#include <string>
#include <vector>
#include <utility>

using namespace taf;
using namespace std;

namespace base {

class JNIRegisterHelper {

public:

	typedef TC_Functor<bool, TL::TLMaker<JNIEnv*>::Result> registry_func_type;

	typedef std::vector<std::pair<const char*, registry_func_type > >  RegisterHelper;

    static JNIRegisterHelper& getInstance()
    {
        static JNIRegisterHelper inst;
        return inst;
    }

    /**
     * define help
     */
    class Helper {
    public:
        Helper(const char* name, const registry_func_type& rf) {

        	JNIRegisterHelper::getInstance().registerJniFunc(name, rf);

        }
    };

    /**
     * do real register
     * 
     * @return bool 
     */
    bool doRegister(JNIEnv* env);

    /**
     * 
     * @param name 
     * @param rf 
     */
    void registerJniFunc(const char* name, const registry_func_type& rf) {

        register_jni_func_->push_back(std::make_pair(name, rf));

        __android_log_print(ANDROID_LOG_DEBUG, "X5JniRegisterHelper", "Helper, name:%s, size:%d", name, 
                                register_jni_func_->size());

    }

protected:
    JNIRegisterHelper() : register_jni_func_(new RegisterHelper())
    {

    }
    ~JNIRegisterHelper()
    {
        delete register_jni_func_;
    }

protected:

    friend class Helper;

    RegisterHelper *register_jni_func_;
};

}

#define BASE_REGISTER_JNI_FUNC(name, rf) \
    static base::JNIRegisterHelper::Helper g_##name(#name, base::JNIRegisterHelper::registry_func_type(rf));


#endif  // JNI_REGISTER_HELPER_H_
