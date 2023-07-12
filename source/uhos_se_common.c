
#include "se_uhos_common.h"

uhos_s32 uhos_se_list_add(struct list_head *new, struct list_head *head)
{
    if(UHOS_NULL == new || UHOS_NULL == head) {
        UHPT_LOGE("null to add: %p, %p.", new, head);
        return -1;
    }
    if(UHOS_NULL != new->next || UHOS_NULL != new->prev) {
        UHPT_LOGE("Try to add one already in list: %p, %p.",
            new->next, new->prev);
        return -2;
    }
    list_add(new, head);
    return 0;
}

uhos_s32 uhos_se_list_add_tail(struct list_head *new, struct list_head *head)
{
    if(UHOS_NULL == new || UHOS_NULL == head) {
        UHPT_LOGE("null to add: %p, %p.", new, head);
        return -1;
    }
    if(UHOS_NULL != new->next || UHOS_NULL != new->prev) {
        UHPT_LOGE("Try to add one already in list: %p, %p.",
            new->next, new->prev);
        return -2;
    }
    list_add_tail(new, head);
    return 0;
}

uhos_s32 uhos_se_list_del(struct list_head *entry)
{
    if(UHOS_NULL == entry) {
        UHPT_LOGE("null to del: %p.", entry);
        return -1;
    }
    if(UHOS_NULL == entry->next || UHOS_NULL == entry->prev) {
        UHPT_LOGE("Try to del one not in list: %p, %p.",
            entry->next, entry->prev);
        return -2;
    }
    list_del(entry);
    return 0;
}

