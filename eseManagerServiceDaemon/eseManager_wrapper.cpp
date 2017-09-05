#include <stdio.h>
#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>
#include "IeseManagerService.h"
#include "eseManager_wrapper.h"
#define LOG_NDEBUG 0
#define LOG_TAG "eseManager_wrapper"

using namespace android;

static const char* eseManagerServiceName = "eseManagerService";
static sp<IeseManagerService> g_eseManager_proxy = 0;

class eseManagerDeathObserver : public IBinder::DeathRecipient {
public:
        virtual void binderDied(const wp<IBinder>& who) {
            ALOGW("eseManagerservice died [%p]", who.unsafe_get());
            g_eseManager_proxy.clear();
        }
};

static sp<IeseManagerService> getEseManagerServiceClient()
{
    int32_t count = 0;
    static sp<eseManagerDeathObserver> eseDeathObserver = 0;
    if (g_eseManager_proxy == 0) {
        sp<IServiceManager> sm = defaultServiceManager();
        sp<IBinder> binder;
        do {
            binder = sm->getService(String16(eseManagerServiceName));
            if (binder != 0)
                break;
            ALOGW("eseManagerService not published, waiting...");
            usleep(10*1000); // 10ms
            count++;
        } while (count < 5);
        if (binder != 0) {
            eseDeathObserver = new eseManagerDeathObserver();
            binder->linkToDeath(eseDeathObserver);
        }
        g_eseManager_proxy = interface_cast<IeseManagerService>(binder);
        if (g_eseManager_proxy == 0) {
            ALOGE("%s: NULL proxy", __FUNCTION__);
            return 0;
        }
    }
    ALOGD("%s success", __FUNCTION__);
    return g_eseManager_proxy;
}

int32_t ese_getEseCplc(uint8_t* cplc_buf, uint32_t* cplc_len)
{
    int32_t ret = -1;
    if (g_eseManager_proxy == 0) {
        g_eseManager_proxy= getEseManagerServiceClient();
        if (g_eseManager_proxy == 0) {
          ALOGE("NULL proxy");
          ret = -1;
          return ret;
        }
    }

    ret = g_eseManager_proxy->getEseCplc(cplc_buf, cplc_len);
    if (ret < 0) {
        ALOGE("%s failed ret = %d", __FUNCTION__, ret);
    }
    return ret;
}

int32_t ese_sendCommandEx(uint32_t cmd_id, uint8_t *buffer, uint32_t len)
{
    int32_t ret = -1;
    if (g_eseManager_proxy == 0) {
        g_eseManager_proxy= getEseManagerServiceClient();
        if (g_eseManager_proxy == 0) {
          ALOGE("NULL proxy");
          ret = -1;
          return ret;
        }
    }

    ret = g_eseManager_proxy->sendCommandEx(cmd_id, buffer, len);
    if (ret < 0) {
        ALOGE("%s failed ret = %d", __FUNCTION__, ret);
    }

    return ret;
}

