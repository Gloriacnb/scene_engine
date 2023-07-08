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


uint32_t uhos_scene_data_load(uint32_t id, uint32_t type, uint32_t offset, uint8_t* data, uint32_t data_len);

#ifdef __cplusplus
}
#endif
