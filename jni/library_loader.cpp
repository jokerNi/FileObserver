#include "base/jni_android.h"
#include "base/jni_register_helper.h"
#include "base/jni_log.h"
#include "MyLog.h"
#include <jni.h>

static void sighandler( int sig_no )
{
	LOG(DEBUG) << "sighandler:" << sig_no << endl;
}

jint JNI_OnLoad(JavaVM *vm, void *reserved) 
{
    XLOG("JNI_OnLoad begin");
	signal(SIGKILL, sighandler);

	base::InitVM(vm);

    JNIEnv *env = base::AttachCurrentThread();

    if (!base::JNIRegisterHelper::getInstance().doRegister(env)) {
      return -1;
    }

    XLOG("JNI_OnLoad end");  
    return JNI_VERSION_1_4;
}

jint JNI_OnUnLoad(JavaVM *jvm, void *reserved) {

	return 0;
}


