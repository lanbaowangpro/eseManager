LOCAL_PATH:= $(call my-dir)

#======build native and daemon
include $(CLEAR_VARS)

LOCAL_SRC_FILES += \
	BneSEPowerManager.cpp \
	eSEPowerManager.cpp \
	IeSEPowerManager.cpp \
	eSEPowerManagerServer.cpp
#Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170822> add for eseManager begin
LOCAL_C_INCLUDES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include \
					$(TARGET_OUT_HEADERS)/common/inc \
					$(TOP)/system/core/fingerprintd

LOCAL_SHARED_LIBRARIES += \
	libbinder \
	libcutils \
	libutils \
	libfingerprint \
	liblog

LOCAL_MODULE := esepmdaemon
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)

#======build proxy=====
include $(CLEAR_VARS)

LOCAL_SRC_FILES += \
	eSEPowerManager_wrapper.cpp \
	BneSEPowerManager.cpp \
	IeSEPowerManager.cpp \
	BpeSEPowerManager.cpp

LOCAL_SHARED_LIBRARIES += \
	libbinder \
	libcutils \
	libutils \
	liblog

LOCAL_MODULE := libesepm
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)

#======build client test=====

include $(CLEAR_VARS)

LOCAL_SRC_FILES := eSEPowerManagerClient.cpp
LOCAL_SHARED_LIBRARIES += \
	libbinder \
	libcutils \
	libutils \
	libesepm \
	liblog

LOCAL_MODULE := esepmclient
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)
#Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170822> add for eseManager end