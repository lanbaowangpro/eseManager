#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <utils/Log.h>

#include "eseManagerService.h"
#define LOG_NDEBUG 0
#define LOG_TAG "main_eseManagerService"

#define EASY_START_BINDER_SERVICE 0

using namespace android;

int32_t main(int argc, char** argv)
{
#if EASY_START_BINDER_SERVICE
    eseManagerService::publishAndJoinThreadPool();//使用了父类BinderService的函数
#else
    sp<ProcessState> proc(ProcessState::self());
    sp<IServiceManager> sm(defaultServiceManager());
    sm->addService(String16(eseManagerService::getServiceName()), new eseManagerService());
    ProcessState::self()->startThreadPool();
    IPCThreadState::self()->joinThreadPool();
#endif

    return 0;
}