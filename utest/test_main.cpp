#include <gtest/gtest.h>
#include "se_scheduler.h"
#include "test_data.h"

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
    TemplateInfo info;
    info.TemplateSize = 1024;
    strcpy(info.LocalDevId.id, LOCAL_DEV_ID);
    auto rtn = loadSceneTemplateFile(sh, &info);
    EXPECT_EQ(rtn, SE_SUCCESS);
    DeviceInfo dev_info;
    strcpy(dev_info.PairDev.id, EXEC_DEV_ID);
    dev_info.devAbility = 0;
    dev_info.presetting_type = 1;
    dev_info.block.Tinfo.Id = 100;
    dev_info.block.Tinfo.Version = 1;
    dev_info.block.Tinfo.RuleNum = 2;
    dev_info.block.Tinfo.Rules = new RuleInfo[2];
    RuleInfo* rules = dev_info.block.Tinfo.Rules;
    for (size_t i = 0; i < 2; i++)
    {
        rules[i].RuleId = i;
        
    }
    
    ExecutorInfo ExeInfo;
    EXPECT_EQ(pairExecutorDevice(sh, &dev_info, &ExeInfo), SE_SUCCESS);
    EXPECT_EQ(ExeInfo.role, 1);
    EXPECT_STREQ(ExeInfo.Obj.id, LOCAL_DEV_ID);
    EXPECT_STREQ(ExeInfo.ObjDev.id, EXEC_DEV_ID);

}

TEST(ScenePairDevice, TestPairTrigger)
{
    
}