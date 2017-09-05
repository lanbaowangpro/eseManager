/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
//Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170819> add for eseManager begin
#pragma once

#include <binder/Parcel.h>
#include <binder/BinderService.h>
#include "BneSEPowerManager.h"
#include "BpeSEPowerManager.h"

namespace android {

    class eSEPowerManager: public BinderService<eSEPowerManager>, public BneSEPowerManager
    {
        friend class BinderService<eSEPowerManager>;

        public:
            class ClientDiedNotifier: public IBinder::DeathRecipient {
                public:
                    ClientDiedNotifier(uint32_t pid, void *service) {
                        mPid = pid;
                        mService = service;
                    }
                protected:
                    virtual void binderDied(const wp<IBinder> &who);

                private:
                    uint32_t mPid;
                    void *mService;
            };

        private:
            int32_t nq_node = -1;
            struct service_data {
                sp<IBinder> l_serviceCb;
                sp<eSEPowerManager::ClientDiedNotifier> l_deathNotifier;
                int32_t client_threadCount;
            };
            std::map<uint32_t, struct service_data> pidsMap;
            void printPidsMap();
            bool isPidsMapEmpty();
            bool isPowerOnAllowed();

        public:
            eSEPowerManager();
            ~eSEPowerManager();
            virtual status_t onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags = 0);
            virtual int powerOn(const sp<IeSEPowerManagerCb> &notifier);
            virtual int powerOff();
            virtual int getState();
            virtual int killall();
            virtual int geteSEBindingStatus();
    };

    class eSEPowerManagerCb: public BneSEPowerManagerCb {
        public:
            virtual void notifyCallback(int32_t event){};
    };
}; // namespace android
//Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170819> add for eseManager end