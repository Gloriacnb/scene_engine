cmake_minimum_required(VERSION 3.10)

project(scene_engine_test)

set(CMAKE_CXX_STANDARD 11)

# 添加 Google Test 子目录
add_subdirectory(/usr/src/googletest
                 ${CMAKE_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

include_directories(/usr/src/googletest/include)

# 添加测试源文件
add_executable(unit_tests test_main.cpp)

# 链接 Google Test 库
target_link_libraries(unit_tests 
    gtest_main
    scene_engine
)

target_include_directories(unit_tests
    PUBLIC ${PROJECT_SOURCE_DIR}/../include
)
target_link_directories(unit_tests
        PUBLIC ${PROJECT_SOURCE_DIR}/../build/
)
# 添加测试任务
include(GoogleTest)
gtest_discover_tests(unit_tests)
