/**
 * @file se_port.h
 * @author shiqingpeng (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-06-28
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once

#include "se_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get the Locak Device Id object
 * 
 * @param[out] did 
 * @return SE_ERR 
 */
SE_ERR getLocakDeviceId(DeviceId* did);



#ifdef __cplusplus
}
#endif
