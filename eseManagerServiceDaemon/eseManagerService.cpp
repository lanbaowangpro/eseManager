//Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170825> modify for eseManager begin
#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>
#include <utils/Log.h>
#include <binder/Parcel.h>
#include <binder/IPCThreadState.h>
#include <utils/threads.h>

#include <cutils/properties.h>
#include "eseManagerService.h"
#include "../libese_spi_client/libese_spi_client.h"
#include <fingerprint_wrapper.h>

#define LOG_NDEBUG 0
#define LOG_TAG "eseManagerService"

#ifndef SYSTEM_UID
#define SYSTEM_UID 1000
#endif

#ifndef ROOT_UID
#define ROOT_UID 0
#endif

#ifndef RADIO_UID
#define RADIO_UID 1001
#endif

#define START_BINDING               1094
#define CLEAR_BINDING_INFO          1095
#define GET_ESE_BINDDING_STATUS     1096


namespace android {

    int32_t geteSEBindingStatus()
    {
        int32_t status = fp_geteSEBindingStatus();
        ALOGD("%s from fp CA status: %d ", __FUNCTION__, status);
        return status;
    }

    int32_t startBinding()
    {
        int32_t status = fp_startBinding();
        ALOGD("%s from fp CA status: %d ", __FUNCTION__, status);
        return status;
    }

    int32_t clearBindingInfo(uint8_t *verify_data, uint32_t len)
    {
        int32_t status = fp_clearBindingInfo(verify_data, len);
        ALOGD("%s from fp CA status %d ", __FUNCTION__, status);
        return status;
    }
    eseManagerService::eseManagerService()
    {
        myParam = 0;
    }

    int32_t eseManagerService::setSomething(int a)
    {
        ALOGD(" eseManagerService::setSomething a = %d myParam %d", a, myParam);
        myParam += a;
        return 0;//OK
    }

    int32_t eseManagerService::getSomething()
    {
        ALOGD("#eseManagerService::getSomething myParam = %d", myParam);
        return myParam;
    }

    int32_t eseManagerService::sendCommand(void* cmd, uint32_t cmdLen, void* rsp, uint32_t rspLen)
    {
        status_t ret = UNKNOWN_ERROR;
        int uid = IPCThreadState::self()->getCallingUid();
        uid %= 100000;
        if ((uid != SYSTEM_UID) && (uid != ROOT_UID)) {
            ALOGE("Bad UID: %d", uid);
            ret = PERMISSION_DENIED;
            return ret;
        }
        ret = ca_sendEseCommand(cmd, cmdLen, rsp, rspLen);
        if (ret < 0) {
            return ret;
        }
        ret = NO_ERROR;
        return ret;
    }

    int32_t eseManagerService::sendCommandEx(uint32_t cmd_id, uint8_t *buffer, uint32_t len)
    {
        int32_t ret = UNKNOWN_ERROR;
        int uid = IPCThreadState::self()->getCallingUid();
        uid %= 100000;
        if ((uid != SYSTEM_UID) && (uid != ROOT_UID) && (uid != RADIO_UID)) {
            ALOGE("Bad UID: %d", uid);
            ret = PERMISSION_DENIED;
            return ret;
        }

        switch (cmd_id) {
        case GET_ESE_BINDDING_STATUS:
            return geteSEBindingStatus();
        case START_BINDING:
            return startBinding();
        case CLEAR_BINDING_INFO:
            return clearBindingInfo(buffer, len);
        default:
            ret = UNKNOWN_ERROR;
            break;
        }
        return ret;
    }

    int32_t eseManagerService::getEseCplc(uint8_t* cplc_buf, uint32_t* cplc_len)
    {
        int32_t ret = UNKNOWN_ERROR;
        if (cplc_buf == NULL || cplc_len == NULL || *cplc_len == 0) {
            return BAD_VALUE;
        }
        ALOGD("Calling getEseCplc");
        ret = ca_getEseCplc(cplc_buf, cplc_len);
        if (ret < 0) {
            return ret;
        }
        return NO_ERROR;
    }
}
//Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170825> modify for eseManager end

