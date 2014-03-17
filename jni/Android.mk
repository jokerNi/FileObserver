# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := monitor
BASE_SRC_FILES  := base/jni_android.cpp \
				   base/jni_register_helper.cpp \
                   base/scoped_java_ref.cpp		   
				   
LOCAL_SRC_FILES += \
	library_loader.cpp \
	$(BASE_SRC_FILES) \
    NativeFileObserver.cpp \
    FileObserver.cpp \
    BackendServer.cpp \
    SimpleTcpClient.cpp \
    FileDeleteObserver.cpp \
    event_queue.c \

#NDK_ROOT := /usr/local/install/android-ndk-r8e
#NDK_ROOT := C:/cygwin/home/Administrator/android-ndk-r8e
NDK_ROOT := C:/cygwin/home/aarontang/android-ndk-r8e
THIRD_PARTY_ROOT := ./third_party

LOCAL_LDLIBS += \
    -llog \
    -lz \
    -L$(THIRD_PARTY_ROOT)/curl-7.34.0/lib -lcurl


LOCAL_C_INCLUDES += \
    $(THIRD_PARTY_ROOT)/curl-7.34.0/include \
    $(LOCAL_PATH)/base \
    $(LOCAL_PATH)/jni \
	$(LOCAL_PATH)/jce \


LOCAL_CPPFLAGS  := -fexceptions -Wno-deprecated -Wconversion-null -fsigned-char
LOCAL_CFLAGS    :=  -D_GLIBCXX_PERMIT_BACKWARD_HASH
	
include $(BUILD_SHARED_LIBRARY)
LOCAL_PATH := $(call my-dir)
