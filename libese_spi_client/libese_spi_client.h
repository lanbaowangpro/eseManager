#ifndef __LIBESE_SPI_CLIENT_H__
#define __LIBESE_SPI_CLIENT_H__
#ifdef __cplusplus
//extern "C" {
#endif
int32_t ca_sendEseCommand(void* cmd, uint32_t cmdLen, void* rsp, uint32_t rspLen);
int32_t ca_getEseCplc(uint8_t* cplc_buf, uint32_t* cplc_len);
//#ifdef __cplusplus
//}
//#endif
#endif

