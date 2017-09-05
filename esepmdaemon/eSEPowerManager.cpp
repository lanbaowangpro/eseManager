/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <binder/Parcel.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <binder/IPCThreadState.h>
#include "eSEPowerManager.h"
#include "pn553.h"
//Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170819> add for eseManager begin
#include "p73.h"
//Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170819> add for eseManager end
#include <fingerprint_wrapper.h>

#define LOG_TAG             "eSEPowerManager"
//Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170819> add for eseManager begin
#define nfc_dev_node        "/dev/p73"
//Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170819> add for eseManager end

#ifndef SYSTEM_UID
#define SYSTEM_UID 1000
#endif

#ifndef ROOT_UID
#define ROOT_UID 0
#endif

#ifndef RADIO_UID
#define RADIO_UID 1001
#endif

#define USE_PERMISSION

namespace android {
/*
* return value
*  0  binding right
*  1  no binding relations, new eSE
*  2  communication error
*  3  binding error
*/
int eSEPowerManager::geteSEBindingStatus() {
    //get from fp ca
    int32_t bind_status = fp_geteSEBindingStatus();
    ALOGD("update from CA/TA %s:%d", __FUNCTION__, bind_status);
    return bind_status;
}

bool eSEPowerManager::isPowerOnAllowed() {
    bool ret = false;
    static bool isFirstCalled = true;
    if (isFirstCalled == true) {
        isFirstCalled = false;
        ret = true;
    } else {
        //use cached binding status
        int status = geteSEBindingStatus();
        if (status != ESE_BINDING_RIGHT && status != ESE_NO_BINDING_RELATION) {
            ALOGE("eSE binding error, poweron is forbidden,status = %d", status);
            ret = false;
        } else {
            ret = true;
        }
    }
    return ret;
}

/*
 * This method will print the list of the registered PIDs.
 */

void  eSEPowerManager::printPidsMap() {
    for(std::map<uint32_t, struct service_data>::iterator it = pidsMap.begin(); it != pidsMap.end(); ++it)
    {
        ALOGD("MAP PID=%d !",it->first);
    }
}

/*
 * This method will check if the PidsMap is empty.
 * if it's not the case, it will print all the pids stored.
 */

bool  eSEPowerManager::isPidsMapEmpty() {
    bool status = pidsMap.empty();
    if (status) {
        ALOGD("pidsMap empty");
    } else {
        printPidsMap();
    }
    return status;
}

eSEPowerManager::eSEPowerManager()
{
}

eSEPowerManager::~eSEPowerManager()
{
}

int eSEPowerManager::powerOn(const sp<IeSEPowerManagerCb> &notifier)
{
    int ret = -1;

    if (isPowerOnAllowed() == false) {
        ALOGE("get cur PowerState 0x%02x", getState());
        ret = PERMISSION_DENIED;
        return ret;
    }

    IPCThreadState* self = IPCThreadState::self();
    int uid = self->getCallingUid();
    uid %= 100000;
    if ((uid != SYSTEM_UID) && (uid != ROOT_UID) && (uid != RADIO_UID)) {
        ALOGE("Bad UID: %d", uid);
        ret = PERMISSION_DENIED;
    #ifdef USE_PERMISSION
        return ret;
    #endif
    }

    int pid = self->getCallingPid();

    ALOGD("Start to Power ON - PID=%d",pid);

    int ese_current_state = -1;
    if (nq_node < 0) {
        nq_node = open(nfc_dev_node, O_RDWR);
        if (nq_node < 0) {
            ALOGE("%s: eSE opening failed : %s",__func__, strerror(errno));
            return nq_node;
        }
    }
    //ret = ioctl(nq_node, ESE_GET_PWR, 0);
    //Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170819> add for eseManager begin
    ret = ioctl(nq_node, P61_GET_SPM_STATUS, &ese_current_state);
    //Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170819> add for eseManager end
    if (ret < 0) {
        ALOGE("%s: eSE ioctl P61_GET_PWR_STATUS failed : %s",__func__, strerror(errno));
        return ret;
    }
    ALOGD("ese_current_state 0x%02x", ese_current_state);
    if (ese_current_state & (P61_STATE_SPI|P61_STATE_SPI_PRIO)) {
        ALOGD("eSE already powered ON");
    } else if (ese_current_state & P61_STATE_DWNLD) {
        ALOGE("0x%02x, NFCC fw is downloading, power operation is forbidden!", ese_current_state);
        return -EBUSY;
    } else {
        if (!isPidsMapEmpty()) {
            //here the pid list is out of sync with the eSE power states.
            ALOGW("pidList out of sync ..."); // nothing to be done here.
        }
        //ret = ioctl(nq_node, ESE_SET_PWR, 0);
        //Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170819> add for eseManager begin
        ret = ioctl(nq_node, P61_SET_POWER_SCHEME, 2);
        //Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170819> add for eseManager end
        if (ret < 0) {
            ALOGE("%s: eSE ioctl P544_SET_POWER_SCHEME failed : %s",__func__, strerror(errno));
            return ret;
        }
        //Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170819> add for eseManager begin
        ret = ioctl(nq_node, P61_SET_SPM_PWR, 1);
        //Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170819> add for eseManager end
        if (ret < 0) {
            ALOGE("%s: eSE ioctl P61_SET_SPI_PWR failed : %s",__func__, strerror(errno));
            return ret;
        }
    }
    struct service_data data = {0};
    data.l_serviceCb = asBinder(notifier);
    data.l_deathNotifier = new ClientDiedNotifier(pid, this);
    if (data.l_serviceCb == 0) {
        ALOGE("%s: No notifier",__func__);
        if (isPidsMapEmpty())
        {
            //Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170819> add for eseManager begin
            ret = ioctl(nq_node, P61_SET_SPM_PWR, 0);
            //Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170819> add for eseManager end
            if (ret < 0) {
                ALOGE("%s: eSE ioctl P61_SET_SPI_PWR failed : %s",__func__, strerror(errno));
                return ret;
            }
            ALOGD("eSE powered OFF - PID=%d", pid);
            if (close(nq_node)) {
                ALOGE("%s: eSE ioctl close failed : %s",__func__, strerror(errno));
            } else {
                nq_node = -1;
            }
        }
        return -1;
    }
    status_t ltdStat = data.l_serviceCb->linkToDeath(data.l_deathNotifier);
    if (ltdStat != NO_ERROR) {
        ALOGE("%s: Link to death failed",__func__);
        return -1;
    }
    ALOGD("eSE powered ON - PID=%d", pid);
    if (!pidsMap.count(pid)) {
        data.client_threadCount = 1;
        pidsMap.insert( std::pair<uint32_t, struct service_data> (pid, data));
    } else {
        ALOGD("pidsMap contains this PID, don't insert, just update client_threadCount");
        pidsMap[pid].client_threadCount++;
        int32_t count = pidsMap[pid].client_threadCount;
        ALOGD(" PID %d client_threadCount %d", pid, count);
    }
    ret = 1;
    return ret;
}

int eSEPowerManager::powerOff()
{
    int ret = -1;
    IPCThreadState* self = IPCThreadState::self();
    int uid = self->getCallingUid();
    uid %= 100000;
    if ((uid != SYSTEM_UID) && (uid != ROOT_UID) && (uid != RADIO_UID)) {
        ALOGE("Bad UID: %d", uid);
        ret = PERMISSION_DENIED;
    #ifdef USE_PERMISSION
        return ret;
    #endif
    }

    int pid = self->getCallingPid();
    ALOGD("Start to Power OFF - PID=%d",pid);

    if (!pidsMap.count(pid)) {
        ALOGE("PID=%d not registered !",pid);
        return -1;
    }
    if (pidsMap[pid].l_serviceCb != 0 && pidsMap[pid].l_deathNotifier != 0) {
        pidsMap[pid].l_serviceCb->unlinkToDeath(pidsMap[pid].l_deathNotifier);
    }
    pidsMap[pid].client_threadCount--;
    if (pidsMap[pid].client_threadCount > 0) {
        ALOGE("PID %d client_threadCount is %d, don't power off", pid, pidsMap[pid].client_threadCount);
        return 1;
    }
    pidsMap.erase(pid);
    if (!isPidsMapEmpty()) {
        ALOGE("isPidsMapEmpty false, eSE is in use, don't power off");
        return 1;
    }
    if (nq_node < 0) {
        ALOGE("%s: eSE file not opened - let's assume it's off",__func__);
        return 0;
    }
    // add protection
    int ese_current_state = -1;
    ret = ioctl(nq_node, P61_GET_SPM_STATUS, &ese_current_state);
    if (ese_current_state & P61_STATE_DWNLD) {
        ALOGE("0x%02x, NFCC fw is downloading, power operation is forbidden!", ese_current_state);
        return -EBUSY;
    }

    //Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170819> add for eseManager begin
    ret = ioctl(nq_node, P61_SET_SPM_PWR, 0);
    //Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170819> add for eseManager end
    if (ret < 0) {
        ALOGE("%s: eSE ioctl P61_SET_SPI_PWR failed : %s",__func__, strerror(errno));
        return ret;
    }
    ALOGD("eSE powered OFF - PID=%d", pid);
    ret = 0;
    if (close(nq_node)) {
        ALOGE("%s: eSE ioctl close failed : %s",__func__, strerror(errno));
    } else {
        nq_node = -1;
    }
    return ret;
}

int eSEPowerManager::getState()
{
    int ret = -1;
    int ese_current_state = -1;
    if (nq_node < 0) {
        nq_node = open(nfc_dev_node, O_RDWR);
        if (nq_node < 0) {
            ALOGE("%s: eSE opening failed : %s",__func__, strerror(errno));
            return nq_node;
        }
    }
    //Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170819> add for eseManager begin
    ret = ioctl(nq_node, P61_GET_SPM_STATUS, &ese_current_state);
    //Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170819> add for eseManager end
    if (ret < 0) {
        ALOGE("%s: eSE ioctl failed : %s",__func__, strerror(errno));
    }
    return ese_current_state;
}

int eSEPowerManager::killall()
{
    int ret = -1;
    IPCThreadState* self = IPCThreadState::self();
    int uid = self->getCallingUid();
    uid %= 100000;
    if ((uid != SYSTEM_UID) && (uid != ROOT_UID) && (uid != RADIO_UID)) {
        ALOGE("Bad UID: %d", uid);
        ret = PERMISSION_DENIED;
    #ifdef USE_PERMISSION
        return ret;
    #endif
    }
    for(std::map<uint32_t, struct service_data>::iterator it = pidsMap.begin(); it != pidsMap.end(); ++it)
    {
        if (pidsMap[it->first].l_serviceCb != 0 && pidsMap[it->first].l_deathNotifier != 0) {
            pidsMap[it->first].l_serviceCb->unlinkToDeath(pidsMap[it->first].l_deathNotifier);
        }
    }
    pidsMap.clear();
    if (nq_node < 0) {
        nq_node = open(nfc_dev_node, O_RDWR);
        if (nq_node < 0) {
            ALOGE("%s: eSE opening failed : %s",__func__, strerror(errno));
            return nq_node;
        }
    }
    // add protection
    int ese_current_state = -1;
    ret = ioctl(nq_node, P61_GET_SPM_STATUS, &ese_current_state);
    if (ese_current_state & P61_STATE_DWNLD) {
        ALOGE("0x%02x, NFCC fw is downloading, power operation is forbidden!", ese_current_state);
        return -EBUSY;
    }
    //Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170819> add for eseManager begin
    ret = ioctl(nq_node, P61_SET_SPM_PWR, 0);
    //Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170819> add for eseManager end
    if (ret < 0) {
        ALOGE("%s: eSE ioctl failed : %s",__func__, strerror(errno));
        return ret;
    }
    if (close(nq_node)) {
        ALOGE("%s: eSE ioctl close failed : %s",__func__, strerror(errno));
    } else {
        nq_node = -1;
    }
    ALOGD("eSE powered OFF");
    return ret;
}

status_t eSEPowerManager::onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
  {
    return BneSEPowerManager::onTransact(code, data, reply, flags);
  }

void eSEPowerManager::ClientDiedNotifier::binderDied(const wp<IBinder> &who) {
    const wp<IBinder> unused = who;
    eSEPowerManager *service = reinterpret_cast <eSEPowerManager*>(mService);

    ALOGW("ESEP - Client died");
    service->pidsMap.erase(mPid);
    do {
        if (!service->isPidsMapEmpty()) {
            ALOGW("isPidsMapEmpty false, eSE is in use, don't power off");
            break;
        }
        if (service->nq_node < 0) {
            ALOGE("%s: eSE file not opened - let's assume it's off",__func__);
            break;
        }
        //power off
        //Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170819> add for eseManager begin
        if (ioctl(service->nq_node, P61_SET_SPM_PWR, 0) < 0) {
        //Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170819> add for eseManager end
            ALOGE("%s: eSE ioctl P61_SET_SPI_PWR failed : %s",__func__, strerror(errno));
            break;
        }
        ALOGD("eSE powered OFF - PID=%d", mPid);
    } while (0);
}

}; // namespace android