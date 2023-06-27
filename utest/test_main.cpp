#include <gtest/gtest.h>
#include "se_scheduler.h"

// 测试环境运行情况的测试用例
TEST(EnvironmentTest, TestEnvironment)
{
    // 断言示例：验证某些条件或预期结果

    // 验证 1 + 1 是否等于 2
    EXPECT_EQ(1 + 1, 2);

    // 验证某个变量的值是否符合预期
    int value = 10;
    EXPECT_GT(value, 5);
    EXPECT_LE(value, 20);

    // 验证某个条件是否满足
    bool condition = true;
    EXPECT_TRUE(condition);

    // 验证某个条件是否不满足
    bool another_condition = false;
    EXPECT_FALSE(another_condition);

    // ... 添加更多的断言验证

}

TEST(ScenePairDevice, TestPairExecutor)
{
    Scheduler* sh = makeNullSchedulerHandle();
    templateInfo info;
    auto rtn = loadSceneTemplateFile(sh, &info);
    EXPECT_EQ(rtn, SUCCESS);
    DeviceInfo DevInfo;
    // DevInfo.
    // ExecutorInfo ExeInfo;
    // pairExecutorDevice(sh, &DevInfo, &ExeInfo);
}