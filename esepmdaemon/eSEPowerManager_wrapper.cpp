/**
 * Copyright (c) 2017 Gionee Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Gionee Technologies, Inc.
 */

#include <android/log.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <string>
#include <unistd.h>
#include <iostream>

#include <eSEPowerManager.h>
#include <eSEPowerManager_wrapper.h>


using namespace android;

#undef LOG_TAG
#define LOG_TAG "esepmClient"

static const char* esePMerviceName = "eSEPowerManagerService";
static sp<IeSEPowerManager> g_esePM_proxy = 0;

class esePowerManagerDeathObserver : public IBinder::DeathRecipient {

public:
    virtual void binderDied(const wp<IBinder>& who) {
        ALOGW("esepmdaemon died [%p]", who.unsafe_get());
        g_esePM_proxy.clear();
    }
};

static sp<IeSEPowerManager> getEsePMServiceClient()
{
    int32_t count = 0;
    static sp<esePowerManagerDeathObserver> esePMDeathObserver = 0;
    if (g_esePM_proxy == 0) {
        sp<IServiceManager> sm = defaultServiceManager();
        sp<IBinder> binder;
        do {
            binder = sm->getService(String16(esePMerviceName));
            if (binder != 0)
                break;
            ALOGW("%s not published, waiting...", esePMerviceName);
            usleep(10*1000); // 10ms
            count++;
        } while (count < 5);
        if (binder != 0) {
            esePMDeathObserver = new esePowerManagerDeathObserver();
            binder->linkToDeath(esePMDeathObserver);
        }
        g_esePM_proxy = interface_cast<IeSEPowerManager>(binder);
        if (g_esePM_proxy == 0) {
            ALOGE("%s: NULL proxy", __FUNCTION__);
            return 0;
        }
    }
    ALOGD("%s success", __FUNCTION__);
    return g_esePM_proxy;
}

int32_t powerOn()
{
    if (g_esePM_proxy == 0) {
        g_esePM_proxy = getEsePMServiceClient();
        if (g_esePM_proxy == 0) {
            ALOGE("NULL proxy");
            return -1;
        }
    }
    sp<eSEPowerManagerCb> cb = new eSEPowerManagerCb();

    int32_t status = g_esePM_proxy->powerOn(cb);
    return status;
}

int32_t powerOff()
{
    if (g_esePM_proxy == 0) {
        g_esePM_proxy = getEsePMServiceClient();
        if (g_esePM_proxy == 0) {
            ALOGE("NULL proxy");
            return -1;
        }
    }
    return g_esePM_proxy->powerOff();
}

int32_t getState()
{
    if (g_esePM_proxy == 0) {
        g_esePM_proxy = getEsePMServiceClient();
        if (g_esePM_proxy == 0) {
            ALOGE("NULL proxy");
            return -1;
        }
    }
    return g_esePM_proxy->getState();
}

int32_t killall()
{
    if (g_esePM_proxy == 0) {
        g_esePM_proxy = getEsePMServiceClient();
        if (g_esePM_proxy == 0) {
            ALOGE("NULL proxy");
            return -1;
        }
    }
    return g_esePM_proxy->killall();
}

int32_t geteSEBindingStatus()
{
    if (g_esePM_proxy == 0) {
        g_esePM_proxy = getEsePMServiceClient();
        if (g_esePM_proxy == 0) {
            ALOGE("NULL proxy");
            return -1;
        }
    }

    int32_t status = g_esePM_proxy->geteSEBindingStatus();
    ALOGD("%s %d", __FUNCTION__, status);
    return status;
}
