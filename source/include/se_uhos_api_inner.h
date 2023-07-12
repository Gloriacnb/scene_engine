#ifndef SE_UHOS_API_INNER_H
#define SE_UHOS_API_INNER_H

#include "uhos_port.h"
#include "se_uhos_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

uhos_s32 se_uhos_chkDevCond(Trigger *trigger,
    const DevProperties* properties,
    TriggerStatus* status);

#ifdef __cplusplus
}
#endif
#endif  //SE_UHOS_API_INNER_H
