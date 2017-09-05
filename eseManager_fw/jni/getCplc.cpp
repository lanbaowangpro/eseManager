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

static jbyteArray native_eseSendCommand(JNIEnv* env, jobject thiz, jbyteArray cmd)
{
/*
    uint32_t ret = -1;
    uint32_t cmdLen;
    if (cmd == NULL) {
        return NULL;
    }

    cmdLen = env->GetArrayLength(cmd);

    if (cmdLen > 200) {
        ALOGE("cmdLen is too long %d", cmdLen);
        return NULL;
    }
    ALOGE("cmdLen %d", cmdLen);
    jbyte* cmdPtr = env->GetByteArrayElements(cmd, NULL);

    struct send_cmd apdu_to_transmit = {0};
    struct send_cmd_rsp msgrsp = {0};
    //unsigned char tmp[] = {0x80,0xca,0x9f,0x7f,0x00};
    //memcpy(apdu_to_transmit.tApdu, tmp, cmdLen);
    memcpy(apdu_to_transmit.tApdu, cmdPtr, cmdLen);
    env->ReleaseByteArrayElements(cmd, cmdPtr, 0);

    apdu_to_transmit.cmd_len = cmdLen;

    if (g_esePM_proxy == 0) {
        g_esePM_proxy = getEsePMServiceClient();
        if (g_esePM_proxy == 0) {
          ALOGE("NULL proxy");
          return NULL;
        }
    }

    if (g_eseManager_proxy == 0) {
        g_eseManager_proxy= getEseManagerServiceClient();
        if (g_eseManager_proxy == 0) {
          ALOGE("NULL proxy");
          return NULL;
        }
    }
    sp<eSEPowerManagerCb> cb = new eSEPowerManagerCb();

    g_esePM_proxy->powerOn(cb);

    ret = g_eseManager_proxy->sendCommand((void*)&apdu_to_transmit, sizeof(apdu_to_transmit), (void*)&msgrsp, sizeof(msgrsp));

    g_esePM_proxy->powerOff();

    if (ret < 0) {
        ALOGE("g_eseManager_proxy->getEseCplc failed ret = %d", ret);
        return NULL;
    }

    ALOGD("Actually len = %d", msgrsp.rsp_len);
    // alloc byte array in java side;
    jbyteArray open = env->NewByteArray(msgrsp.rsp_len);
    if (open != NULL) {
        //copy native buffer into java side;
        env->SetByteArrayRegion(open, 0, msgrsp.rsp_len, (const jbyte*)msgrsp.rApdu);
    }

    return open;
*/
    return NULL;
}

static jint native_eseSendCommandEx(JNIEnv* env, jobject thiz, jint cmd_id, jbyteArray bufferArray)
{
    int32_t ret = -1;
    uint32_t buffer_len = 0;
    uint8_t buffer[100] = {0};

    if (bufferArray != NULL) {
        buffer_len = env->GetArrayLength(bufferArray);
        ALOGD("buffer_len %d", buffer_len);
        if (buffer_len > 100) {
            ALOGD("buffer_len %d is too long !", buffer_len);
            return -1;
        }
        jbyte* bufferPtr = env->GetByteArrayElements(bufferArray, NULL);
        memcpy(buffer, bufferPtr, buffer_len);
        env->ReleaseByteArrayElements(bufferArray, bufferPtr, 0);
    } else {
        buffer_len = 0;
    }

    ret = ese_sendCommandEx(cmd_id, buffer, buffer_len);

    if (ret < 0) {
        ALOGE("%s ret = %d", __FUNCTION__, ret);
    }

    ALOGD("%s cmd_id = %d, ret = %d", __FUNCTION__, cmd_id, ret);
    return ret;
}
static jbyteArray native_eseGetCplc(JNIEnv* env, jobject thiz)
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
    // alloc byte array in java side;
    jbyteArray open = env->NewByteArray(len);
    if (open != NULL) {
        //copy native buffer into java side;
        env->SetByteArrayRegion(open, 0, len, (const jbyte*) (r_cplc));
    }

    return open;
}

static jint native_eseGetBindingStatus(JNIEnv* env, jobject thiz)
{
    //get from eSEPowerManager_wrapper.h
    int32_t status = geteSEBindingStatus();
    ALOGD("%s %d", __FUNCTION__, status);
    return status;
}

static jboolean native_eseValidateChipLegality(JNIEnv* env, jobject thiz)
{
    int32_t ret = -1;
    uint8_t r_cplc[100] = {0};
    uint32_t len = 100;

    ret = powerOn();
    if (ret < 0) {
        ALOGE("eSE poweron fail !");
        return JNI_FALSE;
    }
    ret = ese_getEseCplc(r_cplc, &len);

    powerOff();

    if (ret < 0) {
        ALOGE("%s failed, ret = %d", __FUNCTION__, ret);
        return JNI_FALSE;
    }
    ALOGD("Get CPLC actually len = %d", len);
    if (len < 47) {
        return JNI_FALSE;
    }

    // For valid CPLC, r_cplc[3] and r_cplc[4] must be 0x47, 0x90
    if (r_cplc[3] != 0x47 || r_cplc[4] != 0x90) {
        return JNI_FALSE;
    }

    return JNI_TRUE;

}


static JNINativeMethod methods[] = {
    {"native_eseGetCplc", "()[B", (void*)native_eseGetCplc},
    {"native_eseSendCommand", "([B)[B", (void*)native_eseSendCommand},
    {"native_eseSendCommandEx", "(I[B)I", (void*)native_eseSendCommandEx},
    {"native_eseGetBindingStatus", "()I", (void*)native_eseGetBindingStatus},
    {"native_eseValidateChipLegality", "()Z", (void*)native_eseValidateChipLegality},
};
static int32_t registerNativeMethods(JNIEnv* env, const char* className,
   JNINativeMethod* gMethods, int numMethods)
{
    jclass clazz;
    clazz = env->FindClass(className);
    if (clazz == NULL) {
        ALOGE("Native registration unable to find class '%s'", className);
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        ALOGE("RegisterNatives failed for '%s'", className);
        return JNI_FALSE;
    }
    return JNI_TRUE;
}
/*
 * Register native methods for all classes we know about.
 *
 * returns JNI_TRUE on success.
 */
static int32_t registerNatives(JNIEnv* env)
{
  if (!registerNativeMethods(env, classPathName,
                 methods, sizeof(methods) / sizeof(methods[0]))) {
    return JNI_FALSE;
  }
  return JNI_TRUE;
}


typedef union {
    JNIEnv* env;
    void* venv;
} UnionJNIEnvToVoid;

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    UnionJNIEnvToVoid uenv;
    uenv.venv = NULL;
    jint result = -1;
    JNIEnv* env = NULL;

    ALOGI("JNI_OnLoad");
    if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_4) != JNI_OK) {
        ALOGE("ERROR: GetEnv failed");
        goto bail;
    }
    env = uenv.env;
    if (registerNatives(env) != JNI_TRUE) {
        ALOGE("ERROR: registerNatives failed");
        goto bail;
    }

    result = JNI_VERSION_1_4;

bail:
    return result;
}

void JNI_OnUnload(JavaVM *vm, void *reserved) {

}
//Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170825> modify for eseManager end

int main()
{
    return 0;
}

