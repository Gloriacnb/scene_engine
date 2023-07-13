#include <gtest/gtest.h>
#include "se_scheduler.h"
#include "test_data.h"
#include "scheduler_inner.h"


static Scheduler* sh = nullptr;
class SceneEngineTestFix : public testing::Test {
protected:

    static void SetUpTestSuite() {
        std::cout << "run before first case ..." << std::endl;

        sh = makeNullSchedulerHandle();
        TemplateInfo info;
        info.TemplateSize = 1024;
        info.LocalAbility = 7;
        strcpy(info.LocalDevId.id, LOCAL_DEV_ID);
        auto rtn = loadSceneTemplateFile(sh, &info);
    }

    static void TearDownTestSuite() {

    }
    virtual void SetUp() override {
        std::cout << "enter into SetUp()" << std::endl;

    }

    virtual void TearDown() override {
        std::cout << "exit from TearDown" << std::endl;
    }


};


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

TEST_F(SceneEngineTestFix, TestLoadTemplate) {
    __scheduler* sch = (__scheduler*)sh;
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
}
TEST_F(SceneEngineTestFix, TestPairExecutor)
{
    __scheduler* sch = (__scheduler*)sh;


    DeviceInfo dev_info;
    strcpy(dev_info.PairDev.id, EXEC_DEV_ID);
    dev_info.devAbility = 0;
    dev_info.presetting_type = 1;
    dev_info.block.Tinfo.Id = 100;
    dev_info.block.Tinfo.Version = 1;
    dev_info.block.Tinfo.RuleNum = 2;
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
    EXPECT_EQ(ExeInfo->TemplateInfo.RuleNum, 2);
    EXPECT_EQ(ExeInfo->TemplateInfo.Rules[1].RuleId, 110);

    EXPECT_NE(sch->PairDevList, nullptr);
    EXPECT_STREQ(sch->PairDevList->PairDev.id, EXEC_DEV_ID);
    EXPECT_EQ(sch->PairDevList->next, nullptr);
    EXPECT_EQ(sch->PairDevList->Role, 1);
}

TEST_F(SceneEngineTestFix, TestPairTrigger) {
    __scheduler* sch = (__scheduler*)sh;
    DeviceInfo dev_info;
    strcpy(dev_info.PairDev.id, TRIG_DEV_ID);
    dev_info.devAbility = 0;
    dev_info.presetting_type = 1;
    dev_info.block.Tinfo.Id = 100;
    dev_info.block.Tinfo.Version = 1;
    dev_info.block.Tinfo.RuleNum = 2;
    dev_info.block.Tinfo.Rules = new RuleInfo[2]();
    RuleInfo* rules = dev_info.block.Tinfo.Rules;
    rules[0].RuleId = 100;
    rules[0].ConditionNum = 2;
    rules[0].Conditions = new ConditionInfo[2]();
    rules[0].Conditions[0].CondId = 1;
    rules[0].Conditions[1].CondId = 2;

    rules[1].RuleId = 110;
    rules[1].ConditionNum = 2;
    rules[1].Conditions = new ConditionInfo[2]();
    rules[1].Conditions[0].CondId = 4;
    rules[1].Conditions[1].CondId = 5;

    TriggerInfo* TriInfo = new ExecutorInfo();
    EXPECT_EQ(pairTriggerDevice(sh, &dev_info, TriInfo), SE_SUCCESS);
    EXPECT_EQ(TriInfo->role, 0);
    EXPECT_STREQ(TriInfo->Obj.id, LOCAL_DEV_ID);
    EXPECT_STREQ(TriInfo->ObjDev.id, TRIG_DEV_ID);
    EXPECT_EQ(TriInfo->TemplateInfo.RuleNum, 2);

    EXPECT_NE(sch->PairDevList, nullptr);
    EXPECT_NE(sch->PairDevList->next, nullptr);
    EXPECT_STREQ(sch->PairDevList->next->PairDev.id, TRIG_DEV_ID);
    EXPECT_EQ(sch->PairDevList->next->Role, 0);

    configResult rslt;
    strcpy(rslt.ConfiguredDev.id, TRIG_DEV_ID);
    strcpy(rslt.NotifiedDev.id, LOCAL_DEV_ID);
    rslt.Result = SE_SUCCESS;
    EXPECT_EQ(executorConfigResultNotification(sh, &rslt), SE_SUCCESS);
}
TEST(DeepCopyTest, ConditionInfo) {
    // 创建源结构体
    ConditionInfo src;
    src.CondId = 1;
    src.OptType = 2;
    src.CondType = 3;
    // ... 设置其他成员的值
    src.nBytesValue = 6;
    src.value = new char[6];
    strcpy(src.value, "Hello");

    // 执行深拷贝
    ConditionInfo* dst = new ConditionInfo();
    SE_ERR result = deepCopyConditionInfo(&src, dst);

    // 验证拷贝是否成功
    EXPECT_EQ(result, SE_SUCCESS);
    EXPECT_NE(dst, nullptr);
    EXPECT_NE(&src, dst);
    EXPECT_EQ(dst->CondId, src.CondId);
    EXPECT_EQ(dst->OptType, src.OptType);
    EXPECT_EQ(dst->CondType, src.CondType);
    // ... 验证其他成员的值
    EXPECT_EQ(dst->nBytesValue, 6);
    EXPECT_STREQ(dst->value, "Hello");
    // 清理内存
    free(dst->value);
    free(dst);
}

// 添加其他类似的测试用例
