#Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170825> modify for eseManager begin
LOCAL_PATH:= $(call my-dir)

#======build native and daemon===
include $(CLEAR_VARS)

LOCAL_SRC_FILES += \
	eseManagerService.cpp \
	IeseManagerService.cpp \
	main_eseManagerService.cpp

LOCAL_C_INCLUDES := $(TOP)/system/core/fingerprintd

LOCAL_SHARED_LIBRARIES += \
	libbinder \
	libcutils \
	libutils \
	libese_spi_tac \
	libfingerprint \
	liblog

LOCAL_MODULE := esemanagerdaemon
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)

#======build proxy=====
include $(CLEAR_VARS)

LOCAL_SRC_FILES += \
	eseManager_wrapper.cpp \
	IeseManagerService.cpp

LOCAL_SHARED_LIBRARIES += \
	libbinder \
	libcutils \
	libutils \
	liblog

LOCAL_MODULE := libesemanager
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)

#======build client test===

include $(CLEAR_VARS)

LOCAL_SRC_FILES := main_eseManagerClient.cpp
LOCAL_SHARED_LIBRARIES += \
	libbinder \
	libcutils \
	libutils \
	libesemanager \
	liblog

LOCAL_MODULE := main_eseManagerClient
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)
#Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170825> modify for eseManager end