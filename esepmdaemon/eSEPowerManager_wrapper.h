/**
 * Copyright (c) 2017 Gonee Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Gionee Technologies, Inc.
 */
#define ESE_BINDING_RIGHT			0
#define ESE_NO_BINDING_RELATION		1
#define ESE_COMMUNICATION_ERROR		2
#define ESE_BINDING_ERROR 			3

#ifdef __cplusplus
 extern "C" {
#endif
 
int32_t powerOn();
int32_t powerOff();
int32_t getState();
int32_t killall();
int32_t geteSEBindingStatus();

#ifdef __cplusplus
 }
#endif

