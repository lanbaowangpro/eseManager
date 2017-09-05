#Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170825> modify for eseManager begin
LOCAL_PATH := $(call my-dir)

#=========build static com.gionee.esemanager.jar=================

#include $(CLEAR_VARS)

#LOCAL_MODULE_TAGS := optional
#LOCAL_CERTIFICATE := platform
#LOCAL_PROGUARD_ENABLED := disabled
#LOCAL_SRC_FILES := $(call all-subdir-java-files)
#LOCAL_MODULE:= com.gionee.esemanager

#include $(BUILD_STATIC_JAVA_LIBRARY)

###################################################
#======build libeseManager_jni.so==============

include $(CLEAR_VARS)

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../

LOCAL_SRC_FILES := \
	jni/eseManager_jni.cpp

LOCAL_SHARED_LIBRARIES += \
	libbinder \
	libcutils \
	libutils \
	libesepm \
	libesemanager \
	liblog

LOCAL_MODULE := libeseManager_jni
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)

###################################################
#======build getCplcTest==============

include $(CLEAR_VARS)

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../

LOCAL_SRC_FILES := \
	jni/getCplcTest.cpp

LOCAL_SHARED_LIBRARIES += \
	libbinder \
	libcutils \
	libutils \
	libesepm \
	libesemanager \
	liblog

LOCAL_MODULE := getCplcTest
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
#Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170825> modify for eseManager end
