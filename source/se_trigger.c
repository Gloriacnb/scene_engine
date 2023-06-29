
#include "se_trigger.h"
#include "trigger_inner.h"
#include <stdlib.h>

Trigger* makeNullTriggerHandle(void) {
    __trigger* tri = calloc(sizeof(__trigger), 1);
    return (Trigger*)tri;    
}
SE_ERR configureTrigger(Trigger* trigger, const TriggerInfo* config) {
    
    return SE_FAILED;
}

TriggerStatus evaluateDeviceTriggerConditions(Trigger* trigger, const DevProperties* properties) {
    TriggerStatus st = {0};
    return st;
}

SE_ERR deleteTriggerData(Trigger* trigger, const TriggerConfig* config) {
    return SE_FAILED;
}