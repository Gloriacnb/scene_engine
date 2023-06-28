/**
 * @file test_port.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-06-28
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "se_port.h"
#include <string.h>
#include "test_data.h"


SE_ERR getLocakDeviceId(DeviceId* did) {
    memcpy(did->id, LOCAL_DEV_ID, sizeof(LOCAL_DEV_ID));
    return SE_SUCCESS;
}