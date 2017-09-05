/**
 * @author Rick
 *
 * Reference:
 *         [frameworks/base/services/core/jni/onload.cpp]
 */
/*
 ============================================================================
 ============================================================================
 */

//Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170825> modify for eseManager begin
#define LOG_NDEBUG 0
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>


#include <binder/IServiceManager.h>
#include <JNIHelp.h>
#include <jni.h>
#include <utils/Log.h>

#include <eseManagerServiceDaemon/eseManager_wrapper.h>
#include <esepmdaemon/eSEPowerManager_wrapper.h>
#include <pthread.h>

#define LOG_TAG "eseManager_jni"

using namespace android;

static const char *classPathName = "com/gionee/esemanager/esemanagerAPI";

struct send_cmd {
    uint32_t cmd_len;
    unsigned char tApdu[200];
};

struct send_cmd_rsp {
    uint32_t rsp_len;
    unsigned char rApdu[200];
};

struct thread_info {
    pthread_t thread_id;
    uint32_t count;
};

static int32_t volatile g_quit = 0;
static void ese_powerOff_handler(int sig)
{
    ALOGD("test quit, try to exit !");
    g_quit = 1;
}
static int getCplC()
{
    int32_t ret = -1;
    uint8_t r_cplc[100] = {0};
    uint32_t len = 100;

    ret = powerOn();
    if (ret < 0) {
        ALOGE("eSE poweron fail !");
        return NULL;
    }
    ret = ese_getEseCplc(r_cplc, &len);

    powerOff();

    if (ret < 0) {
        ALOGE("%s failed, ret = %d", __FUNCTION__, ret);
        return NULL;
    }
    ALOGD("Get CPLC actually len = %d", len);


    return len;
}

static void* getCplcThread(void *arg)
{
    thread_info *p_tinfo = (thread_info*) arg;

    while (p_tinfo->count && g_quit == 0){
        ALOGD("thread %d: running..", gettid());
        getCplC();
        p_tinfo->count--;
    }
    return NULL;
}

int main(int argc, char **argv)
{
    ALOGD("start ....");

    int thread_count, i;
    int ret = -1;

    if (argv[1] == NULL) {
        thread_count = 3;
    } else {
        thread_count = atoi(argv[1]);
        if (thread_count > 100) {
            thread_count = 100;
        }
    }

    ALOGD("thread_count %d", thread_count);
    signal(SIGINT, ese_powerOff_handler);

    struct thread_info *tinfo;
    tinfo = (struct thread_info*) calloc(thread_count, sizeof(thread_info));

    for (i = 0; i < thread_count; i++) {
        tinfo[i].count = 1000000;
        ret = pthread_create(&tinfo[i].thread_id, NULL, getCplcThread, (void*)&tinfo[i]);
        if(ret != 0) {
            ALOGE("Create pthread error!\n");
            return 0;
        }

    }

    for (i = 0; i < thread_count; i++) {
        pthread_join(tinfo[i].thread_id, NULL);
    }

    ALOGD("This is the main process exit: geteSE State 0x%02x .\n", getState());
    free(tinfo);
    return 0;
}

