LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_PROGUARD_ENABLED := disabled
LOCAL_RESOURCE_DIR := $(LOCAL_PATH)/res
LOCAL_RESOURCE_DIR += frameworks/support/v7/appcompat/res

LOCAL_STATIC_JAVA_LIBRARIES += android-support-v7-appcompat \
							   android-support-v4 \
							   com.gionee.esemanager


LOCAL_AAPT_FLAGS := --auto-add-overlay
LOCAL_AAPT_FLAGS += --extra-packages android.support.v7.appcompat


LOCAL_SRC_FILES := $(call all-java-files-under, java)

LOCAL_SDK_VERSION := current

LOCAL_CERTIFICATE := platform

LOCAL_PACKAGE_NAME := eseManagerTest

include $(BUILD_PACKAGE)


#===build pre-build com.gionee.esemanager.jar====================

##################################################

include $(CLEAR_VARS)

LOCAL_PREBUILT_STATIC_JAVA_LIBRARIES += com.gionee.esemanager:libs/com.gionee.esemanager.jar

include $(BUILD_MULTI_PREBUILT)