
#define START_BINDING               1094
#define CLEAR_BINDING_INFO          1095
#define GET_ESE_BINDDING_STATUS     1096

#ifdef __cplusplus
extern "C" {
#endif

int32_t ese_getEseCplc(uint8_t* cplc_buf, uint32_t* cplc_len);
int32_t ese_sendCommand(void* cmd, uint32_t cmdLen, void* rsp, uint32_t rspLen);
int32_t ese_sendCommandEx(uint32_t cmd_id, uint8_t *buffer, uint32_t len);

#ifdef __cplusplus
}
#endif

