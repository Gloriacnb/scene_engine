## 组件结构

#### 必需结构：

1. include：外部头文件目录
2. source：源文件目录
3. package.yaml：组件描述文件
4. sample：接口使用用例
5. utest：单元测试


### 构建指南：

组件支持使用cmake单独编译为静态库，同时生成单元测试可执行程序，请首先安装cmake
```bash
sudo apt install cmake
```
为了支持单元测试 
需要首先安装 googletest
ubuntu下运行：
```bash
sudo apt install libgtest-dev
```
#### 编译组件：
在scene_engine目录下
```bash
cmake -B cmake_build
cmake --build cmake_build
```

构建完成后，
库文件在 cmake_build/libscene_engine.a
单元测试 cmake_build/unit_tests

#### 单元测试：

构建完成后，可执行文件在 cmake_build/unit_tests
在scene_engine目录下执行：
```bash
./cmake_build/unit_tests 
Running main() from /usr/src/googletest/googletest/src/gtest_main.cc
[==========] Running 3 tests from 2 test suites.
[----------] Global test environment set-up.
[----------] 1 test from EnvironmentTest
[ RUN      ] EnvironmentTest.TestEnvironment
[       OK ] EnvironmentTest.TestEnvironment (0 ms)
[----------] 1 test from EnvironmentTest (0 ms total)

[----------] 2 tests from ScenePairDevice
[ RUN      ] ScenePairDevice.TestPairExecutor
[       OK ] ScenePairDevice.TestPairExecutor (0 ms)
[ RUN      ] ScenePairDevice.TestPairTrigger
[       OK ] ScenePairDevice.TestPairTrigger (0 ms)
[----------] 2 tests from ScenePairDevice (0 ms total)

[----------] Global test environment tear-down
[==========] 3 tests from 2 test suites ran. (0 ms total)
[  PASSED  ] 3 tests.
```
