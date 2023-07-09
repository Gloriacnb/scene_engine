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

uint32_t uhos_scene_data_load(uint32_t id, uint32_t type, uint32_t offset, uint8_t* data, uint32_t data_len) {
    std::ifstream file("/home/peter/work/scene-dev/services/scene_engine/utest/test_tlv_v1_scheduler.bin", std::ios::binary);

    if (!file) {
        std::cout << "Failed to open file" << std::endl;
        return 0;
    }

    // 获取文件大小
    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // 校验数据长度是否足够
    if (fileSize > data_len) {

        std::cout << "Insufficient buffer size " << fileSize << ":" << data_len << std::endl;
        return 0;
    }

    // 读取文件内容到 data
    std::vector<uint8_t> buffer(fileSize);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), fileSize)) {
        std::cout << "Failed to read file" << std::endl;
        return 0;
    }

    // 将读取到的数据拷贝到输出参数 data
    std::copy(buffer.begin(), buffer.end(), data);

    return fileSize;
}

}