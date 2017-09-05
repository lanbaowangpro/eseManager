//Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170825> modify for eseManager begin
#include "IeseManagerService.h"
#include <binder/Parcel.h>
#include <binder/IInterface.h>
#include <utils/Log.h>

#define LOG_NDEBUG 0
#define LOG_TAG "IeseManagerService"

namespace android {

    enum {
        SET_SOMETHING = IBinder::FIRST_CALL_TRANSACTION,
        GET_SOMETHING,
        CMD_SEND_COMMAND,
        CMD_SEND_COMMAND_EX,
        CMD_GETCPLC,
    };

    //------------------------------------proxy side--------------------------------

    class BpeseManagerService : public BpInterface<IeseManagerService>
    {
        public:
            BpeseManagerService(const sp<IBinder>& impl)
                : BpInterface<IeseManagerService>(impl)
            {
            }

            virtual int32_t setSomething(int a)
            {
                ALOGD(" BpeseManagerService::setSomething a = %d ", a);
                Parcel data,reply;
                data.writeInterfaceToken(IeseManagerService::getInterfaceDescriptor());
                data.writeInt32(a);
                remote()->transact(SET_SOMETHING,data,&reply);
                return reply.readInt32();
            }

            virtual int32_t getSomething()
            {
                ALOGD(" BpeseManagerService::getSomething ");
                Parcel data,reply;
                data.writeInterfaceToken(IeseManagerService::getInterfaceDescriptor());
                remote()->transact(GET_SOMETHING,data,&reply);
                return reply.readInt32();
            }

            virtual int32_t sendCommand(void* cmd, uint32_t cmdLen, void* rsp, uint32_t rspLen)
            {
                Parcel data, reply;
                status_t rv = UNKNOWN_ERROR;
                do {
                   if ((!cmd) || (!rsp) || (cmdLen == 0) || (rspLen == 0)) {
                    ALOGE("NULL params");
                    rv = BAD_VALUE;
                    break;
                  }
                  data.writeInterfaceToken(IeseManagerService::getInterfaceDescriptor());
                  data.writeInt32(cmdLen);
                  data.writeInt32(rspLen);
                  data.write(cmd, cmdLen);
                  // binder call
                  rv = remote()->transact(CMD_SEND_COMMAND, data, &reply);
                  if (rv != NO_ERROR) {
                    ALOGE("Couldn't contact remote: %d", rv);
                    break;
                  }

                  //copy memory from binder
                  memcpy(rsp, reply.readInplace(rspLen), rspLen);
                  ALOGD("Response read: %u bytes", rspLen);
                } while (0);
                return rv;
            }

            virtual int32_t sendCommandEx(uint32_t cmd_id, uint8_t *buffer, uint32_t len)
            {
                Parcel data, reply;
                status_t rv = UNKNOWN_ERROR;
                do {

                    data.writeInterfaceToken(IeseManagerService::getInterfaceDescriptor());
                    data.writeInt32(cmd_id);
                    data.writeInt32(len);
                    data.write(buffer, len);
                    // binder call
                    rv = remote()->transact(CMD_SEND_COMMAND_EX, data, &reply);
                    if (rv != NO_ERROR) {
                        ALOGE("Couldn't contact remote: %d", rv);
                        break;
                  }

                  rv = reply.readInt32();

                } while (0);
                return rv;
            }


            virtual int32_t getEseCplc (uint8_t* cplc_buf, uint32_t* cplc_len)
            {
                Parcel data, reply;
                status_t ret = UNKNOWN_ERROR;

                ALOGD("original cplc_len = %d", *cplc_len);
                if (cplc_buf == NULL || cplc_len == NULL || *cplc_len == 0) {
                    ret = BAD_VALUE;
                    return ret;
                }
                memset(cplc_buf, 0x00, *cplc_len);

                data.writeInterfaceToken(IeseManagerService::getInterfaceDescriptor());
                data.writeUint32(*cplc_len); // write buffer length

                ret = remote()->transact(CMD_GETCPLC, data, &reply);
                if (NO_ERROR != ret) {
                    ALOGE("Bpese_spi ERROR: transact(CMD_GETCPLC) failed! ret = %d", ret);
                    return ret;
                }

                // copy memory allocated by server on binder, only once copy operation happened
                ret = reply.read(cplc_buf, *cplc_len);
                if(NO_ERROR != ret ) {
                    ALOGE("Bpese_spi Daemon read cplc_buf failed!");
                    return ret;
                }

                // get Actually cplc length;
                *cplc_len = reply.readUint32();
                return NO_ERROR;
            }
    };
    //---------------------- interface--------------------
    IMPLEMENT_META_INTERFACE(eseManagerService, "android.hardware.IeseManagerService");


    //------------------------------------server side--------------------------------
    status_t BneseManagerService::onTransact (
        uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags) {
            status_t ret = UNKNOWN_ERROR;
            switch (code) {
                case SET_SOMETHING: {
                    ALOGD("BneseManagerService::onTransact  SET_SOMETHING ");
                    CHECK_INTERFACE(IeseManagerService, data, reply);
                    ret = reply->writeInt32(setSomething((int) data.readInt32()));
                    break;
                }
                case GET_SOMETHING: {
                    ALOGD("BneseManagerService::onTransact  GET_SOMETHING ");
                    CHECK_INTERFACE(IeseManagerService, data, reply);
                    ret = reply->writeInt32(getSomething());
                    break;
                }
                case CMD_SEND_COMMAND: {
                    ALOGD("BneseManagerService::onTransact  CMD_SEND_COMMAND ");
                    CHECK_INTERFACE(IeseManagerService, data, reply);

                    uint32_t cmdLen = data.readInt32();
                    uint32_t rspLen = data.readInt32();
                    uint8_t *cmd = NULL;
                    uint8_t *rsp = NULL;

                    if (UINT32_MAX - cmdLen < rspLen) {
                        ALOGE("Attempted buffer overflow: %u %u", cmdLen, rspLen);
                        ret = BAD_VALUE;
                        break;
                    }
                    if ((cmdLen == 0) || (rspLen == 0)) {
                        ALOGE("Zero length: %u %u", cmdLen, rspLen);
                        ret = BAD_VALUE;
                        break;
                    }

                    cmd = (uint8_t*) data.readInplace(cmdLen);
                    //Alloc memory on binder
                    rsp = (uint8_t*) reply->writeInplace(rspLen);
                    memset(rsp, 0x00, rspLen);
                    ret = sendCommand(cmd, cmdLen, rsp, rspLen);
                    break;
                }

                case CMD_SEND_COMMAND_EX: {
                    ALOGD("BneseManagerService::onTransact  CMD_SEND_COMMAND_EX ");
                    CHECK_INTERFACE(IeseManagerService, data, reply);

                    uint32_t cmd_id = data.readInt32();
                    uint32_t buffer_len = data.readInt32();
                    uint8_t *buffer = NULL;

                    buffer = (uint8_t*) data.readInplace(buffer_len);

                    int32_t status = sendCommandEx(cmd_id, buffer, buffer_len);
                    ALOGD("status = %d", status);
                    ret = reply->writeInt32(status);
                    break;
                }
                case CMD_GETCPLC: {
                    ALOGD("BneseManagerService::onTransact  CMD_GETCPLC ");
                    CHECK_INTERFACE(IeseManagerService, data, reply);

                    uint32_t cplc_len = data.readUint32();

                    if (cplc_len == 0) {
                        ALOGE("bad value");
                        ret = BAD_VALUE;
                        break;
                    }
                    // Alloc memory on binder
                    ALOGD("Alloc memory on binder");
                    uint8_t* cplc_buf = (uint8_t*) reply->writeInplace(cplc_len);
                    memset(cplc_buf, 0x00, cplc_len);

                    ret = getEseCplc(cplc_buf, &cplc_len);

                    if (ret == NO_ERROR) {
                        reply->writeUint32(cplc_len);
                    } else {
                        ALOGE("%s getEseCplc failed", __FUNCTION__);
                    }
                    break;
                }
                default: {
                    ALOGD("Unknown binder command ID: %d", code);
                    return BBinder::onTransact(code, data, reply, flags);
                }
            }
            return ret;
    }
}
//Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170825> modify for eseManager end

