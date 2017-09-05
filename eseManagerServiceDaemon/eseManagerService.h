//Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170825> modify for eseManager begin
#include "IeseManagerService.h"
#include <binder/BinderService.h>

namespace android {
	class eseManagerService : public BinderService<eseManagerService>, public BneseManagerService
	{
		public:
			eseManagerService();
			static const char* getServiceName() { return "eseManagerService"; }//will be the service name
			virtual int32_t setSomething(int32_t a);
			virtual int32_t getSomething();
			virtual int32_t sendCommand(void* cmd, uint32_t cmdLen, void* rsp, uint32_t rspLen);
			virtual int32_t sendCommandEx(uint32_t cmd_id, uint8_t *buffer, uint32_t len);
			virtual int32_t getEseCplc(uint8_t* cplc_buf, uint32_t* cplc_len);

		protected:
			int myParam;
	  };
}
//Gionee <GN_BSP_ESEMANAGER_SUPPORT> <donghe> <20170825> modify for eseManager end
