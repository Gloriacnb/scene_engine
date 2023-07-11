#include <gtest/gtest.h>
#include "se_scheduler.h"
#include "test_data.h"
#include "scheduler_inner.h"

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
    info.LocalAbility = 7;
    strcpy(info.LocalDevId.id, LOCAL_DEV_ID);
    auto rtn = loadSceneTemplateFile(sh, &info);

    __scheduler* sch = (__scheduler*)sh;
    EXPECT_EQ(rtn, SE_SUCCESS);
    EXPECT_EQ(sch->LocalAbility, 7);
    EXPECT_EQ(sch->SceneId, 100);
    EXPECT_EQ(sch->SceneVer, 1);
    EXPECT_EQ(sch->TemplateData.RuleNum, 2);
    EXPECT_EQ(sch->TemplateData.Rules[0].RuleId, 100);
    EXPECT_EQ(sch->TemplateData.Rules[0].ConditionNum, 2);
    EXPECT_EQ(sch->TemplateData.Rules[1].RuleId, 110);
    EXPECT_EQ(sch->TemplateData.Rules[1].ConditionNum, 2);

    EXPECT_EQ(sch->TemplateData.Rules[0].Conditions[0].CondId, 1);
    EXPECT_EQ(sch->TemplateData.Rules[1].Conditions[0].CondId, 4);

    DeviceInfo dev_info;
    strcpy(dev_info.PairDev.id, EXEC_DEV_ID);
    dev_info.devAbility = 0;
    dev_info.presetting_type = 1;
    dev_info.block.Tinfo.Id = 100;
    dev_info.block.Tinfo.Version = 1;
    dev_info.block.Tinfo.RuleNum = 1;
    dev_info.block.Tinfo.Rules = new RuleInfo[2]();
    RuleInfo* rules = dev_info.block.Tinfo.Rules;
    rules[0].RuleId = 100;
    rules[0].ActionGNum = 1;
    rules[0].ActionGs = new ActionGroup();
    rules[0].ActionGs[0].ActsNum = 1;
    rules[0].ActionGs[0].actions = new ActionInfo();
    rules[0].ActionGs[0].actions[0].ActId = 3;

    rules[1].RuleId = 110;
    rules[1].ActionGNum = 1;
    rules[1].ActionGs = new ActionGroup();
    rules[1].ActionGs[0].ActsNum = 1;
    rules[1].ActionGs[0].actions = new ActionInfo();
    rules[1].ActionGs[0].actions[0].ActId = 6;
    ExecutorInfo* ExeInfo = new ExecutorInfo();
    EXPECT_EQ(pairExecutorDevice(sh, &dev_info, ExeInfo), SE_SUCCESS);
    EXPECT_EQ(ExeInfo->role, 1);
    EXPECT_STREQ(ExeInfo->Obj.id, LOCAL_DEV_ID);
    EXPECT_STREQ(ExeInfo->ObjDev.id, EXEC_DEV_ID);
    EXPECT_EQ(ExeInfo->TemplateInfo.RuleNum, 1);
    // EXPECT_EQ(ExeInfo->TemplateInfo.Rules[1].RuleId, 110);

}

TEST(ScenePairDevice, TestPairTrigger)
{
    
}