#include <stdint.h>

typedef struct
{
    char* devId;
    char* typeId;

    // bit0: scheduler
    // bit1: triger
    // bit2: actor
    uint8_t devAbility;
}se_device_info;

typedef struct 
{
    /* data */
}se_presetting_data;

typedef struct 
{
    char* template_data;
    char* scheduler_devId;
    char* triger_devId;
}se_triger_config_data;

typedef struct
{
    char* template_data;
    char* actor_devId;
}se_actor_config_data;

typedef struct 
{
    /* data */
}se_device_attr_data;

typedef struct 
{
    /**
     * 调度设备ID
     * 触发设备ID
     * 触发器ID
     * 场景模板ID
     * 场景规则ID
    */

}se_triger_info;

typedef struct 
{
    /**
     * 调度设备ID
     * 执行设备ID
     * 执行器设备ID
     * 场景模板ID
     * 场景规则ID
    */
}se_actor_info;


/**
 * 解析预置数据，生成完整触发器信息
*/
se_triger_config_data* getTrigerConfig(const se_device_info*, const se_presetting_data*);

/**
 * 本地创建一个触发器对象
*/
bool se_create_triger(se_triger_config_data*);

/**
 * 解析预置数据，生成执行器完整信息
*/
se_actor_config_data* getActorConfig(const se_device_info*, const se_presetting_data*);

/**
 * 本地创建一个执行器对象
*/
bool se_create_actor(se_actor_config_data*);

/**
 * 判断触发器是否触发
*/
se_triger_info se_get_triger_info(const se_device_info*, se_device_attr_data*);

/**
 * 触发场景规则
*/
se_actor_info se_do_triger(const se_triger_info*);

/**
 * 执行器执行
*/
bool se_do_actor(se_actor_info);