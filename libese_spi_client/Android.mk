LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libese_spi_tac
LOCAL_MODULE_TAGS := optional

#LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libmtee/include/

LOCAL_SHARED_LIBRARIES := \
        libmtee \
	liblog \
        libcutils \
        libutils \
        libdl

LOCAL_SRC_FILES := libese_spi_client.cpp 

include $(BUILD_SHARED_LIBRARY)

#====buind==EXECUTABLE==============================

#include $(CLEAR_VARS)  
#LOCAL_MODULE := libmtee      
#LOCAL_SRC_FILES := libmtee.so
#LOCAL_EXPORT_C_INCLUDES := ./include
#include $(PREBUILT_SHARED_LIBRARY)      
  

include $(CLEAR_VARS)
#LOCAL_SRC_FILES:= 80t_spi_test.c
LOCAL_SRC_FILES:= 80t-test-ap.cpp libese_spi_client.cpp
LOCAL_MODULE:= spitest-80t-tee

LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES  := libmtee   
LOCAL_LDLIBS += -llog -ldl -pie -fPIE

LOCAL_CFLAGS += -DANDROID_LOG_PRINT -pie -fPIE

include $(BUILD_EXECUTABLE)

