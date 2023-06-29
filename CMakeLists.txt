cmake_minimum_required(VERSION 3.12)
project(scene_engine)

# 添加您的源文件
add_library(scene_engine STATIC
    source/se_executor.c
    source/se_scheduler.c
    source/se_trigger.c
    # ...
)

# 配置编译选项等

# 可以添加其他目标和依赖
target_include_directories(scene_engine 
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/source/include

)

target_compile_options(scene_engine 
    PUBLIC -Wall
)