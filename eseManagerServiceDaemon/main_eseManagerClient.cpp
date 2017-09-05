//Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170825> modify for eseManager begin
#include <stdio.h>
#include <utils/Log.h>
#include <string.h>
#include "eseManager_wrapper.h"

#define LOG_NDEBUG 0
#define LOG_TAG "main_eseManagerClient"

using namespace android;

int32_t main(int argc, char* argv[]) {

    if (argc == 1) {
        ALOGD("usage: [fptest 1]                --startBinding");
        ALOGD("usage: [fptest 2]                --geteSEBindingStatus");
        ALOGD("usage: [fptest 3 verify_data]    --clearBindingInfo");
        ALOGD("default geteSEBindingStatus");
        ALOGD("geteSEBindingStatus %d" ,ese_sendCommandEx(GET_ESE_BINDDING_STATUS, NULL, 0));
        return 0;
    }

    if (strncmp(argv[1], "1", 1) == 0) {
        ALOGD("startBinding %d" ,ese_sendCommandEx(START_BINDING, NULL, 0));
    } else if (strncmp(argv[1], "2", 1) == 0) {
        ALOGD("client geteSEBindingStatus %d" ,ese_sendCommandEx(GET_ESE_BINDDING_STATUS, NULL, 0));
    } else if (strncmp(argv[1], "3", 1) == 0) {
        if (argc < 2 || argv[2] == NULL) {
            ALOGD("arguments error, usage: fptest 3 verify_data");
            return -1;
        }
        char *verify_data = argv[2];
        uint32_t len = strlen(verify_data);

        ALOGD("client verify_data %s, len = %d", verify_data, len);

        ALOGD("clearBindingInfo %d" ,ese_sendCommandEx(CLEAR_BINDING_INFO, (uint8_t*) verify_data, len));
    }
    ALOGI("Done");
    return 0;
}
//Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170825> modify for eseManager end
