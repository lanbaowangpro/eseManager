/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include <android/log.h>
#include <stdlib.h>
#include <utils/RefBase.h>
#include <binder/IInterface.h>
#include <binder/IBinder.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include <string>
#include <unistd.h>

#include "eSEPowerManager.h"

#include "IeSEPowerManager.cpp"
#include "BneSEPowerManager.cpp"
#include "BpeSEPowerManager.cpp"

