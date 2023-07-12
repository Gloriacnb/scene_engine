#ifndef SE_UHOS_COMMON_H
#define SE_UHOS_COMMON_H

#include "uhos_port.h"
#include "uh_k_list.h"

#ifdef __cplusplus
extern "C" {
#endif

uhos_s32 uhos_se_list_add(struct list_head *new, struct list_head *head);
uhos_s32 uhos_se_list_add_tail(struct list_head *new, struct list_head *head);
uhos_s32 uhos_se_list_del(struct list_head *entry);

#ifdef __cplusplus
}
#endif
#endif  //SE_UHOS_COMMON_H
