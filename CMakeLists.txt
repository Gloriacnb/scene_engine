cmake_minimum_required(VERSION 3.10)
project(scene_engine)

# 添加您的源文件
add_library(scene_engine STATIC
    source/se_executor.c
    source/se_scheduler.c
    source/se_trigger.c
    source/uhos_ctlv.c
)

target_include_directories(scene_engine
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/source/include
)

target_compile_options(scene_engine PRIVATE -Wall -Wextra)

target_compile_definitions(scene_engine
    PRIVATE NO_DEBUG_TLV
    PRIVATE NOT_BUILD_IN_UHOS
)

# 编译单元测试
# 添加 Google Test 子目录
add_subdirectory(/usr/src/googletest
                 ${CMAKE_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

include_directories(/usr/src/googletest/include)

add_executable(unit_tests
    utest/test_main.cpp
    utest/test_port.cpp
)

target_link_libraries(unit_tests
    PRIVATE scene_engine
    PRIVATE gtest_main
)

target_link_directories(unit_tests
    PRIVATE /usr/include/gtest
)

include(GoogleTest)
gtest_discover_tests(unit_tests)
