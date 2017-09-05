//Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170825> modify for eseManager begin
#ifndef IeseManagerService_H
#define IeseManagerService_H

#include <binder/IInterface.h>

namespace android {
	class IeseManagerService : public IInterface {
		public:
			DECLARE_META_INTERFACE(eseManagerService);
			virtual int32_t setSomething(int32_t a) = 0;
			virtual int32_t getSomething() = 0;
			virtual int32_t sendCommand(void* cmd, uint32_t cmdLen, void* rsp, uint32_t rspLen) = 0;
			virtual int32_t sendCommandEx(uint32_t cmd_id, uint8_t *buffer, uint32_t len) = 0;
			virtual int32_t getEseCplc(uint8_t* cplc_buf, uint32_t* cplc_len) = 0;
	};

	class BneseManagerService : public BnInterface<IeseManagerService> {
		public:
			virtual status_t onTransact( uint32_t code,
											const Parcel& data,
											Parcel* reply,
											uint32_t flags = 0);
	};

}
#endif
//Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170825> modify for eseManager end
