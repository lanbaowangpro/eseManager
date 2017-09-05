/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define LOG_TAG "eSEPowerManagerServer"

#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>

#include "eSEPowerManager.h"

#undef NDEBUG

using namespace android;

int32_t main()
{
    ALOGD("%s is starting", eSEPowerManager::getServiceName());
    sp<ProcessState> proc(ProcessState::self());
    ProcessState::self()->startThreadPool();
    defaultServiceManager()->addService(String16("eSEPowerManagerService"), new eSEPowerManager);
    IPCThreadState::self()->joinThreadPool();
    return 0;
}
