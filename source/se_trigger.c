
#include "se_trigger.h"
#include "trigger_inner.h"
#include <stdlib.h>

Trigger* makeNullTriggerHandle(void) {
    __trigger* tri = calloc(sizeof(__trigger), 1);
    return (Trigger*)tri;    
}
SE_ERR configureTrigger(Trigger* trigger, const TriggerInfo* config, configResult* result) {
    if( trigger == NULL || config == NULL ) {
        return SE_FAILED;
    }
    __trigger* trig = (__trigger*)trigger;
    trig->Scheduler = config->Scheduler;
    trig->Trigger = config->Obj;
    trig->TriggerDev = config->ObjDev;
    trig->SceneData = config->TemplateInfo; //@todo 这里只做了浅拷贝，如果使用有问题再修改

    result->ConfiguredDev = trig->Trigger;
    result->NotifiedDev = trig->Scheduler;
    result->Result = SE_SUCCESS;
    return SE_SUCCESS;
}

TriggerStatus evaluateDeviceTriggerConditions(Trigger* trigger, const DevProperties* properties) {
    TriggerStatus st = {0};
    return st;
}

SE_ERR deleteTriggerData(Trigger* trigger, const TriggerInfo* config) {
    return SE_FAILED;
}