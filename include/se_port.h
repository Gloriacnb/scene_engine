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

#ifdef __cplusplus
extern "C" {
#endif


#include "se_defines.h"


/**
 * @brief Get the Locak Device Id object
 * 
 * @param[out] did 
 * @return SE_ERR 
 */
SE_ERR getLocalDeviceId(DeviceId* did);

uint32_t uhos_scene_data_load(uint32_t id, uint32_t type, uint32_t offset, uint8_t* data, uint8_t data_len);

#ifdef __cplusplus
}
#endif
