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
#include <fstream>
#include <iostream>
#include <vector>

extern "C" {

SE_ERR getLocalDeviceId(DeviceId* did) {
    memcpy(did->id, LOCAL_DEV_ID, sizeof(LOCAL_DEV_ID));
    return SE_SUCCESS;
}


uint32_t uhos_scene_data_load(uint32_t id, uint32_t type, uint32_t offset, uint8_t* data, uint8_t data_len) {
    std::ifstream file("test_tlv_v1_01.bin", std::ios::binary);

    if (!file) {
        return 0;
    }

    // 获取文件大小
    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // 校验数据长度是否足够
    if (fileSize > data_len) {
        return 0;
    }

    // 读取文件内容到 data
    std::vector<uint8_t> buffer(fileSize);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), fileSize)) {
        return 0;
    }

    // 将读取到的数据拷贝到输出参数 data
    std::copy(buffer.begin(), buffer.end(), data);

    return fileSize;
}

}