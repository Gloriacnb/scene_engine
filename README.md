## 组件结构

#### 必需结构：

1. include：外部头文件目录
2. source：源文件目录
3. package.yaml：组件描述文件
4. sample：接口使用用例
5. utest：单元测试


### 构建指南：

组件支持单独编译为静态库，请首先安装cmake
为了减少硬件依赖，请切换solution 为 linuxenv


#### 编译组件：
在scene_engine目录下
```bash
cmake -B build
cmake --build build
```

构建完成后，库文件在 build/libscene_engine.a

#### 单元测试：
需要首先安装 googletest
ubuntu下运行：
```bash
sudo apt install libgtest-dev
```

进入 utest目录下

```bash
cmake -B build
cmake --build build
```

构建完成后，可执行文件在 utest/build/unit_tests
在utest目录下执行：
```bash
./build/unit_tests 
Running main() from /usr/src/googletest/googletest/src/gtest_main.cc
[==========] Running 1 test from 1 test suite.
[----------] Global test environment set-up.
[----------] 1 test from EnvironmentTest
[ RUN      ] EnvironmentTest.TestEnvironment
[       OK ] EnvironmentTest.TestEnvironment (0 ms)
[----------] 1 test from EnvironmentTest (0 ms total)

[----------] Global test environment tear-down
[==========] 1 test from 1 test suite ran. (0 ms total)
[  PASSED  ] 1 test.
```
