#ifndef SE_UHOS_DEFINES_H
#define SE_UHOS_DEFINES_H

#include "se_uhos_common.h"

#ifdef __cplusplus
extern "C" {
#endif

//trigger_states: dev -> status

typedef struct {
    struct list_head list;
	uhos_u16 sid;
	uhos_u8  rid;
	uhos_u8  cid;
	uhos_u8  isTriggered;
	ConditionState_t state;
} TriggerStatus_t;

typedef struct {
	struct list_head list;
    DeviceId devInfo;
	struct list_head status_head;
} DevTriggerStatus_t;

typedef struct {
	struct list_head trigger_head;
	uhos_u8 inited;
} DevTriggerStatusHead_t;


#ifdef __cplusplus
}
#endif
#endif //SE_UHOS_DEFINES_H
