
#include "uhos_ctlv.h"

// utlv_s.h
#define UTLV_T_CT_IS_NUM(i) ((utlv_ctype_uint8 <= (i)) && ((i) <= utlv_ctype_double))

//string, need more space for '\0'
#define UTLV_T_CT_IS_STRING(i) (utlv_ctype_string == (i))

//bindata, not len set, caller can use tlv.len as data_len
#define UTLV_T_CT_IS_DATA(i)   (utlv_ctype_bindata == (i))

//if new type add, the macro below may be changed (NUM, STR, DAT)
#define UTLV_T_CT_IS_EASY_SET_TYPE(i) ((utlv_ctype_uint8 <= (i)) && ((i) <= utlv_ctype_bindata))

#define UTLV_T_CT_IS_BASE_ARRAY(i) ((utlv_ctype_arr_uint8 <= (i)) && ((i) <= utlv_ctype_arr_double))
#define UTLV_T_CT_IS_OBJECT(i) (utlv_ctype_object == (i))
#define UTLV_T_CT_IS_VALID(i)  ((utlv_ctype_uint8 <= (i)) && ((i) <= utlv_ctype_object))

#define UTLV_T_CT_IS_ARRAY(i)  (utlv_ctype_array_flag == ((i) & utlv_ctype_array_flag))
#define UTLV_T_CT_GET_TYPE(i)  ((i) & utlv_ctype_mask)
#define UTLV_T_CT_SET_ARRAY(i) ((i) | utlv_ctype_array_flag)

typedef struct utlv_s_ {
    uhos_u32 tag;
    uhos_u32 len;
    const uhos_u8 *vbuf;
} utlv_s_t;

typedef uhos_s32 (*utlv_s_dec_fp) (
    const uhos_u8  *buf,
    const uhos_s32 buf_l,
    utlv_s_t *tlv);
typedef uhos_s32 (*utlv_s_enc_fp) (
    const utlv_s_t *tlv,
    uhos_u8 *buf,
    const uhos_s32  buf_l);

typedef uhos_s32 (*utlv_s_d2val_fp) (
    utlv_tag_ctype_t ct,
    const uhos_u8 *buf,
    uhos_u32 buf_l,
    void *pout_val);

typedef struct utlv_s_ctx_ {
    utlv_s_type_t tlv_s_type;
    utlv_s_dec_fp tlv_s_dec_func;
    utlv_s_enc_fp tlv_s_enc_func;
    utlv_s_d2val_fp  tlv_s_d2etv_func;
} utlv_s_ctx_t;

const utlv_s_ctx_t* utlv_s_ctx_get(utlv_s_type_t t);
uhos_void utlv_s_ctx_put(const utlv_s_ctx_t* sctx);

uhos_u32 utlv_s_get_tlv_num(const utlv_s_ctx_t *sctx, const uhos_u8 *buf, uhos_u32 len);

// utlv_s_v1.h
const utlv_s_ctx_t* utlv_s_v1_ctx_get(utlv_s_type_t t);

#define UTLV_S_V1_TAG_BYTES 2
#define UTLV_S_V1_LEN_BYTES 2
#define UTLV_S_V1_HEAD_SIZE (UTLV_S_V1_TAG_BYTES + UTLV_S_V1_LEN_BYTES)

    //todo need to check system endian then convert
static uhos_u16 b2d_be_16(const uhos_u8 *buf)
{
    uhos_u16 ret=0;
    ret = (buf[0]<<8) | (buf[1]);

    return ret;
}
static uhos_u32 b2d_be_32(const uhos_u8 *buf)
{
    uhos_u32 ret=0;
    uhos_u32 i;

    for(i=0; i<sizeof(uhos_u32); i++)
        ret = (ret << 8) + buf[i];

    return ret;
}

static uhos_u64 b2d_be_64(const uhos_u8 *buf)
{
    uhos_u64 ret=0;
    uhos_s32 i;

    for(i=0; i<sizeof(uhos_u64); i++)
        ret = (ret << 8) + buf[i];

    return ret;
}

static void d2b_be_16(uhos_u8 *buf, uhos_u16 val)
{
    if(!buf) return;

    buf[0] = ((val>>8) & 0xFF);
    buf[1] = (val & 0xFF);
}

static uhos_s32 utlv_s_dec_v1(
    const uhos_u8  *buf,
    const uhos_s32  buf_l,
    utlv_s_t *tlv)
{
    uhos_u16 Tag, Len;
    uhos_u32 i=0;
    if(!buf || buf_l < UTLV_S_V1_HEAD_SIZE || !tlv) {
        UTLV_LOGE("invaldparam: %p, %d, %p.", buf, buf_l, tlv);
        return UHOS_FAILURE;
    }
    Tag = Len = 0;

    i=0;
    Tag = b2d_be_16(&buf[i]);
    i += UTLV_S_V1_TAG_BYTES;

    Len = b2d_be_16(&buf[i]);
    i += UTLV_S_V1_LEN_BYTES;

    if(buf_l < (i+Len)) {
        UTLV_LOGE("No enough space. %d <= %d. i %d, L %d.",
            buf_l, (i+Len), i, Len);
        UTLV_HEX_DUMP("NES", buf, i);
        return UHOS_FAILURE;
    }

    tlv->tag = Tag;
    tlv->len = Len;
    tlv->vbuf = &buf[i];

    return UHOS_SUCCESS;
}

static uhos_s32 utlv_s_d2num_v1(
    utlv_tag_ctype_t ct,
    const uhos_u8 *buf,
    uhos_u32 buf_l,
    void *pout_val)
{
    uhos_s32 ret = UHOS_FAILURE;

    if(!buf || !pout_val) {
        UTLV_LOGE("null input: %p, %p.", buf, pout_val);
        return ret;
    }

    switch(ct) {
    case utlv_ctype_uint8: //same way for u8 and s8
    case utlv_ctype_sint8:
    case utlv_ctype_arr_uint8: //arr_item
    case utlv_ctype_arr_sint8:
        if(sizeof(uhos_u8) != buf_l) {
            UTLV_LOGE("TSW1: ct %d, s %ld, l %d.",
                ct, sizeof(uhos_u8), buf_l);
            break;
        }
        *((uhos_u8*)pout_val) = buf[0];
        ret = sizeof(uhos_u8);
    break;

    case utlv_ctype_uint16: //same way for u16 and s16
    case utlv_ctype_sint16:
    case utlv_ctype_arr_uint16:
    case utlv_ctype_arr_sint16:
        if(sizeof(uhos_u16) != buf_l) {
            UTLV_LOGE("TSW1: ct %d, s %ld, l %d.",
                ct, sizeof(uhos_u16), buf_l);
            break;
        }
        *((uhos_u16*)pout_val) = b2d_be_16(buf);
        ret = sizeof(uhos_u16);
    break;

    case utlv_ctype_enum:   //enum as s32
    case utlv_ctype_uint32: //same way for u32 and s32
    case utlv_ctype_sint32:
    case utlv_ctype_arr_uint32:
    case utlv_ctype_arr_sint32:
    case utlv_ctype_arr_enum:
        if(sizeof(uhos_u32) != buf_l) {
            UTLV_LOGE("TSW1: ct %d, s %ld, l %d.",
                ct, sizeof(uhos_u32), buf_l);
            break;
        }
        *((uhos_u32*)pout_val) = b2d_be_32(buf);
        ret = sizeof(uhos_u32);
    break;

    case utlv_ctype_uint64: //same way for u64 and s64
    case utlv_ctype_sint64:
    case utlv_ctype_arr_uint64:
    case utlv_ctype_arr_sint64:
        if(sizeof(uhos_u64) != buf_l) {
            UTLV_LOGE("TSW1: ct %d, s %ld, l %d.",
                ct, sizeof(uhos_u64), buf_l);
            break;
        }
        *((uhos_u64*)pout_val) = b2d_be_64(buf);
        ret = sizeof(uhos_u64);
    break;

    default:
        UTLV_LOGE("USPT: %d.", ct);
        ret = UHOS_FAILURE;
    break;
    }

    return ret;
}

static uhos_s32 utlv_s_d2str_v1(
    utlv_tag_ctype_t ct,
    const uhos_u8 *buf,
    uhos_u32 buf_l,
    void *pout_val)
{
    uhos_s32 ret = UHOS_FAILURE;

    if(!buf || !pout_val) {
        UTLV_LOGE("null input: %p, %p.", buf, pout_val);
        return ret;
    }
    if(buf_l < 0) {
        UTLV_LOGE("invalid param: buf_l %d.", buf_l);
        return ret;
    }

    if(buf_l > 0) {
        uhos_libc_memcpy(pout_val, buf, buf_l);
    }
    ((uhos_s8*)pout_val)[buf_l] = '\0';

    ret = buf_l + 1;  // '\0'

    return ret;
}

static uhos_s32 utlv_s_d2dat_v1(
    utlv_tag_ctype_t ct,
    const uhos_u8 *buf,
    uhos_u32 buf_l,
    void *pout_val)
{
    uhos_s32 ret = UHOS_FAILURE;

    if(!buf || !pout_val) {
        UTLV_LOGE("null input: %p, %p.", buf, pout_val);
        return ret;
    }
    if(buf_l < 0) {
        UTLV_LOGE("invalid param: buf_l %d.", buf_l);
        return ret;
    }

    if(buf_l > 0) {
        uhos_libc_memcpy(pout_val, buf, buf_l);
    }

    ret = buf_l;

    return ret;
}

static uhos_s32 utlv_s_d2etv_v1(
    utlv_tag_ctype_t ct,
    const uhos_u8 *buf,
    uhos_u32 buf_l,
    void *pout_val)
{
    if(UTLV_T_CT_IS_NUM(ct) || UTLV_T_CT_IS_BASE_ARRAY(ct)) {
        return utlv_s_d2num_v1(ct, buf, buf_l, pout_val);
    } else if(UTLV_T_CT_IS_STRING(ct)) {
        return utlv_s_d2str_v1(ct, buf, buf_l, pout_val);
    } else if(UTLV_T_CT_IS_DATA(ct)) {
        return utlv_s_d2dat_v1(ct, buf, buf_l, pout_val);
    }

    UTLV_LOGE("ct %d not easy_set type.", ct);
    return UHOS_FAILURE;
}

static uhos_s32 utlv_s_enc_v1(
    const utlv_s_t *tlv,
    uhos_u8 *buf,
    const uhos_s32  buf_l)
{
    uhos_s32 i=0;

    if(!tlv || !buf) {
        UTLV_LOGE("null input: %p, %p.", tlv, buf);
        return UHOS_FAILURE;
    }

    i = UTLV_S_V1_HEAD_SIZE + tlv->len;
    if(buf_l<i) {
        UTLV_LOGE("NES: %d < %d. tlen %d, head %d.",
            buf_l, i, tlv->len, UTLV_S_V1_HEAD_SIZE);
        UTLV_LOGE("tlv %p, buf %p.", tlv, buf);
        return UHOS_FAILURE;
    }

    i = 0;
    d2b_be_16(&buf[i], (uhos_u16) tlv->tag);
    i += UTLV_S_V1_TAG_BYTES;

    d2b_be_16(&buf[i], (uhos_u16) tlv->len);
    i += UTLV_S_V1_LEN_BYTES;

    uhos_libc_memcpy(&buf[i], tlv->vbuf, tlv->len);
    i += tlv->len;

    return i;
}

static const utlv_s_ctx_t s_v1_ctx = {
    UTLV_S_TYPE_V1,
    utlv_s_dec_v1,
    utlv_s_enc_v1,
    utlv_s_d2etv_v1,
};

const utlv_s_ctx_t* utlv_s_v1_ctx_get(utlv_s_type_t t)
{
    if(UTLV_S_TYPE_V1 == t) return &s_v1_ctx;

    UTLV_LOGE("type(%d) is not TYPE_V1(%d).", t, UTLV_S_TYPE_V1);
    return UHOS_NULL;
}

// utlv_s.c
const utlv_s_ctx_t* utlv_s_ctx_get(utlv_s_type_t t)
{
    switch(t) {
        case UTLV_S_TYPE_V1:
            return utlv_s_v1_ctx_get(t);
        break;

        default:
        UTLV_LOGE("Not support type %d.", t);
        return UHOS_NULL;
    }

    return UHOS_NULL;
}
/*
 * so far noting to do
*/
uhos_void utlv_s_ctx_put(const utlv_s_ctx_t* sctx)
{
}

uhos_u32 utlv_s_get_tlv_num(const utlv_s_ctx_t *sctx, const uhos_u8 *buf, uhos_u32 len)
{
    uhos_u32 num = 0;
    uhos_s32 ret;
    utlv_s_t tlv;
    uhos_u32 tmp_len = len;
    const uhos_u8 *tmp_buf = buf;

    while(tmp_len > 0) {
        ret = sctx->tlv_s_dec_func(tmp_buf, tmp_len, &tlv);
        if(ret<0) return num;
        tmp_buf = tlv.vbuf + tlv.len;
        tmp_len = len - (tmp_buf-buf);
        num ++;
    }

    return num;
}

//utlv_c.h
typedef uhos_s32 (*utlv_enc_pf) (
    const utlv_ctx_t *ctx,
    const utlv_tag_info_t *ptaginfo,
    uhos_void *in_param,
    utlv_data_t *pOut_data);

typedef uhos_s32 (*utlv_dec_pf) (
    const utlv_ctx_t *ctx,
    const utlv_tag_info_t *ptaginfo,
    utlv_data_t *pIn_data,
    uhos_void *out_param);

typedef uhos_s32 (*utlv_free_pf) (
    const utlv_ctx_t *ctx,
    const utlv_tag_info_t *ptaginfo,
    uhos_void *param,
    uhos_u32 num);

typedef struct utlv_ctx_ {
    const utlv_s_ctx_t *sctx;
    utlv_enc_pf enc_func;
    utlv_dec_pf dec_func;
    utlv_free_pf free_func;
} utlv_ctx_t;

//utlv_c.c

static uhos_s32 utlv_decode_inter(
    const utlv_ctx_t *ctx,
    const utlv_tag_info_t *ptaginfo,
    utlv_data_t *pIn_data,
    uhos_void *out_param);

static uhos_s32 utlv_free_inter(
    const utlv_ctx_t *ctx,
    const utlv_tag_info_t *ptaginfo,
    uhos_void *param,
    uhos_u32 arr_size);

static const utlv_tag_info_t* utlv_get_subtag(const utlv_tag_info_t *pTI, uhos_u32 T)
{
    uhos_s32 i;

    if(UHOS_NULL == pTI) {
        UTLV_LOGE("pTI null.");
        return UHOS_NULL;
    }

    for(i=0; i<pTI->nsubtags; i++) {
        if(T == pTI->psubtags[i].tag) {
            return &(pTI->psubtags[i]);
        }
    }

    UTLV_LOGW("No subtag found. pTAG 0x%x, subTAG 0x%x.",
        pTI->tag, T);

    return UHOS_NULL;
}

static uhos_u32 utlv_get_ctype_size(utlv_tag_ctype_t ct)
{
    uhos_u32 uret=0;

    switch(ct) {
    case utlv_ctype_uint8:
    case utlv_ctype_sint8:
    case utlv_ctype_arr_uint8:
    case utlv_ctype_arr_sint8:
        uret = 1;
    break;

    case utlv_ctype_uint16:
    case utlv_ctype_sint16:
    case utlv_ctype_arr_uint16:
    case utlv_ctype_arr_sint16:
        uret = 2;
    break;

    case utlv_ctype_uint32:
    case utlv_ctype_sint32:
    case utlv_ctype_arr_uint32:
    case utlv_ctype_arr_sint32:
        uret = 4;
    break;

    case utlv_ctype_uint64:
    case utlv_ctype_sint64:
    case utlv_ctype_arr_uint64:
    case utlv_ctype_arr_sint64:
        uret = 8;
    break;

    default:
        UTLV_LOGE("unhandle the ctype %d.", ct);
    break;
    }

    return uret;
}

static uhos_s32 utlv_set_u32_to_p(
        utlv_tag_ctype_t ct,
        uhos_void *ParentParam,
        uhos_u32 val)
{
    uhos_s32 ret = UHOS_FAILURE;

    switch(ct) {
    case utlv_ctype_uint8:
    case utlv_ctype_sint8:
    case utlv_ctype_arr_uint8:
    case utlv_ctype_arr_sint8:
    {
        *((uhos_u8 *)ParentParam) = (uhos_u8) val;
        ret = UHOS_SUCCESS;
        UTLV_LOGD("u32 %d -> u8 %d, %p.", val, *((uhos_u8 *)ParentParam), ParentParam);
    }
    break;

    case utlv_ctype_uint16:
    case utlv_ctype_sint16:
    case utlv_ctype_arr_uint16:
    case utlv_ctype_arr_sint16:
    {
        *((uhos_u16 *)ParentParam) = (uhos_u16) val;
        ret = UHOS_SUCCESS;
        UTLV_LOGD("u32 %d -> u16 %d, %p.", val, *((uhos_u16 *)ParentParam), ParentParam);
    }
    break;

    case utlv_ctype_uint32:
    case utlv_ctype_sint32:
    case utlv_ctype_arr_uint32:
    case utlv_ctype_arr_sint32:
        *((uhos_u32 *)ParentParam) = val;
        ret = UHOS_SUCCESS;
        UTLV_LOGD("u32 %d -> u32 %d, %p.", val, *((uhos_u32 *)ParentParam), ParentParam);
    break;

    default:
        UTLV_LOGE("unhandle the ctype %d.", ct);
        ret = UHOS_FAILURE;
    break;
    }

    return ret;
}

static uhos_u32 utlv_get_int_frm_p(void *p, uhos_size_t off, utlv_tag_ctype_t ct)
{
    uhos_u32 uret=0;
    uhos_u8 *pD = p;
    pD += off;

    switch(ct) {
    case utlv_ctype_uint8:
    case utlv_ctype_sint8:
    case utlv_ctype_arr_uint8:
    case utlv_ctype_arr_sint8:
    {
        uhos_u8 u8_tmp;
        u8_tmp = *((uhos_u8 *)pD);
        uret = u8_tmp;
    }
    break;

    case utlv_ctype_uint16:
    case utlv_ctype_sint16:
    case utlv_ctype_arr_uint16:
    case utlv_ctype_arr_sint16:
    {
        uhos_u16 u16_tmp;
        u16_tmp = *((uhos_u16*)pD);
        uret = u16_tmp;
    }
    break;

    case utlv_ctype_uint32:
    case utlv_ctype_sint32:
    case utlv_ctype_arr_uint32:
    case utlv_ctype_arr_sint32:
        uret = *((uhos_u32*)pD);
    break;

    default:
        UTLV_LOGE("unhandle the ctype %d.", ct);
    break;
    }

    return uret;
}

static uhos_u32 utlv_get_num_frm_member(
    const utlv_ctx_t *ctx,
    const utlv_tag_info_t *ptaginfo,
    uhos_u32 mem_tag,
    uhos_void *param)
{
    const utlv_tag_info_t *pnumtag;
    uhos_u32 arr_size = 0;

    pnumtag = utlv_get_subtag(ptaginfo, mem_tag);
    if(UHOS_NULL != pnumtag) {
        arr_size = utlv_get_int_frm_p(param,
                        pnumtag->offset,
                        UTLV_T_CT_GET_TYPE(pnumtag->ctype));
    }

    return arr_size;
}

static uhos_s32 utlv_set_u32_to_member_by_ctype(
    utlv_tag_ctype_t ct,
    uhos_void *ParentParam,
    uhos_u32 val)
{
    if(UHOS_NULL == ParentParam) {
        UTLV_LOGE("input NULL, %p.", ParentParam);
        return UHOS_FAILURE;
    }

    return utlv_set_u32_to_p(UTLV_T_CT_GET_TYPE(ct),
        ParentParam,
        val);
}

static uhos_s32 utlv_encode_inter(
    const utlv_ctx_t *ctx,
    const utlv_tag_info_t *ptaginfo,
    uhos_void *in_param,
    utlv_data_t *pOut_data)
{
    UTLV_LOGE("Not implement yet.");
    return UHOS_FAILURE;
}

static uhos_s32 utlv_dec_set_base_array(
    const utlv_ctx_t *ctx,
    const utlv_tag_info_t *ptaginfo,
    utlv_data_t *pIn_data,
    uhos_void *out_param)
{
    uhos_s32 ret = UHOS_FAILURE;
    uhos_s32 i;
    uhos_s32 num;
    uhos_u32 ct_size;
    const uhos_u8 *pTmpBuf;
    uhos_u8 *pVal;

    UTLV_LOGW("IN tag 0x%x, ct %d.", ptaginfo->tag, ptaginfo->ctype);
    ct_size = utlv_get_ctype_size(UTLV_T_CT_GET_TYPE(ptaginfo->ctype));
    if(ct_size != ptaginfo->item_size || 0 == ct_size) {
        UTLV_LOGE("tag 0x%x, ct %d: ct_size %d neq item_size %ld.",
            ptaginfo->tag,
            UTLV_T_CT_GET_TYPE(ptaginfo->ctype),
            ct_size,
            ptaginfo->item_size);
        return ret;
    }

    num = pIn_data->buf_l / ct_size;
    if(0 >= num) {
        UTLV_LOGE("tag 0x%x, ct %d: num wrong %d frm %d / %d.",
            ptaginfo->tag,
            UTLV_T_CT_GET_TYPE(ptaginfo->ctype),
            num,
            pIn_data->buf_l,
            ct_size);
        return ret;
    }
    UTLV_LOGW("num %d, ct_size %d, arr_size %d.",
        num, ct_size, pIn_data->array_size);
    if(pIn_data->array_size>0 && num != pIn_data->array_size) {
        UTLV_LOGW("array_size %d != num %d.", pIn_data->array_size, num);
    }

    pTmpBuf = pIn_data->buf;
    pVal    = out_param;
    for(i=0; i<num; i++) {
        ret = ctx->sctx->tlv_s_d2etv_func(
                UTLV_T_CT_GET_TYPE(ptaginfo->ctype),
                pTmpBuf,
                ct_size,
                pVal);
        if(ret<0) {
            UTLV_LOGE("tag 0x%x, ct %d: d2etv fail %d.",
                ptaginfo->tag, ptaginfo->ctype, ret);
            break;
        }
        pTmpBuf += ct_size;
        pVal    += ct_size;
    }

    UTLV_LOGW("OUT. ret %d, tag 0x%x, ct %d.",
        ret, ptaginfo->tag, ptaginfo->ctype);
    return ret;
}

static uhos_s32 utlv_dec_object(
    const utlv_ctx_t *ctx,
    const utlv_tag_info_t *ptaginfo,
    utlv_data_t *pIn_data,
    uhos_void *out_param)
{
    uhos_s32 ret = UHOS_FAILURE;
    uhos_u32 tmp_len;
    const uhos_u8 *tmp_buf;
    utlv_s_t tlv;
    utlv_data_t tmp_data;
    const utlv_tag_info_t *psubtag;
    uhos_s32 i=0;

    tmp_len = pIn_data->buf_l;
    tmp_buf = pIn_data->buf;

    while(tmp_len>0) {
        uhos_u32 arr_size = 1;
        uhos_void *pObjAddr = UHOS_NULL;
        uhos_libc_memset(&tlv, 0, sizeof(utlv_s_t));
        ret = ctx->sctx->tlv_s_dec_func(tmp_buf, tmp_len, &tlv);
        if(ret<0) {
            UTLV_LOGE("dec fail: %d.", ret);
            UTLV_HEX_DUMP("decbuf", tmp_buf, tmp_len);
            return ret;
        }
        tmp_buf = tlv.vbuf + tlv.len;
        if(pIn_data->buf_l < (tmp_buf - pIn_data->buf)) {
            UTLV_LOGE("NED: buf_l %d, tlv_l %d. buf %p, v %p.",
                pIn_data->buf_l, tlv.len, pIn_data->buf, tlv.vbuf);
            tmp_len = 0; //set it as no data status
            return UHOS_FAILURE;
        }
        tmp_len = pIn_data->buf_l - (tmp_buf - pIn_data->buf);

        if(0 == tlv.len) {
            UTLV_LOGW("LIZ: pTag 0x%x, Tag 0x%x, len is %d (0).",
                ptaginfo->tag, tlv.tag, tlv.len);
            continue;
        }

        //set tmp_data, array_size is 0 now
        uhos_libc_memset(&tmp_data, 0, sizeof(utlv_data_t));
        tmp_data.buf = tlv.vbuf;
        tmp_data.buf_l = tlv.len;

        //we get tlv, need to decode it
        psubtag = utlv_get_subtag(ptaginfo, tlv.tag);
        if(UHOS_NULL == psubtag) {
            UTLV_LOGE("not found taginfo for 0x%x in 0x%x. SKIP!!!",
                tlv.tag, ptaginfo->tag);
            //may new type, just skip then next tlv, so continue, not break
            continue;
        }
        i++;
        if(UTLV_T_CT_IS_ARRAY(psubtag->ctype)) {
            uhos_u32 tmp_arr_size = 0;
            tmp_arr_size = utlv_get_num_frm_member(ctx,
                ptaginfo,
                psubtag->array_num_tag,
                out_param);
            if(0 == tmp_arr_size) {
            //not get num from numtag
            //try to get tlv's num
                UTLV_LOGW("not get num from num_tag 0x%x.",
                    psubtag->array_num_tag);
                tmp_arr_size = utlv_s_get_tlv_num(ctx->sctx, tlv.vbuf, tlv.len);
            }
            if(0 >= tmp_arr_size) {
                UTLV_LOGW("arr_tag 0x%x, ct %d, arr_size is 0, %d. set it as 1.",
                    tlv.tag, psubtag->ctype, tmp_arr_size);
                tmp_arr_size = 1;
            }
            arr_size = tmp_arr_size;
            tmp_data.array_size = arr_size;
            UTLV_LOGW("arr_size is %d.", arr_size);
        }

        if(UTLV_T_CT_IS_BASE_ARRAY(psubtag->ctype)) {
            uhos_u32 tmp_arr_size = 0;
            tmp_arr_size = utlv_get_num_frm_member(ctx,
                ptaginfo,
                psubtag->array_num_tag,
                out_param);
            if(0 == tmp_arr_size) {
                tmp_arr_size = tlv.len/psubtag->item_size;
            }
            arr_size = tmp_arr_size;
            tmp_data.array_size = arr_size;
            //set num to member
            utlv_set_u32_to_member_by_ctype(
                psubtag->num_ctype,
                ((uhos_u8*)out_param + psubtag->num_offset),
                tmp_arr_size);
            UTLV_LOGW("arr_size is %d.", arr_size);
        }

        if(UTLV_T_CT_IS_STRING(psubtag->ctype)) {
            uhos_u32 tmp_arr_size = 0;
            tmp_arr_size = utlv_get_num_frm_member(ctx,
                ptaginfo,
                psubtag->array_num_tag,
                out_param);
            UTLV_LOGW("tmp_arr_size is %d.", tmp_arr_size);
            UTLV_LOGW("subt: t 0x%x, l %d. %p.", tlv.tag, tlv.len, tlv.vbuf);
            if(0 == tmp_arr_size) {
                tmp_arr_size = tlv.len + 1;
                UTLV_LOGW("tmp_arr_size is %d.", tmp_arr_size);
            }
            arr_size = tmp_arr_size;
            tmp_data.array_size = arr_size;
            UTLV_LOGW("arr_size is %d.", arr_size);
        }

        if(UTLV_T_CT_IS_DATA(psubtag->ctype)) {
            uhos_u32 tmp_arr_size = 0;
            tmp_arr_size = utlv_get_num_frm_member(ctx,
                ptaginfo,
                psubtag->array_num_tag,
                out_param);
            if(0 == tmp_arr_size) {
                tmp_arr_size = tlv.len;
            }
            arr_size = tmp_arr_size;
            tmp_data.array_size = arr_size;
            UTLV_LOGW("arr_size is %d.", arr_size);
        }

        if(psubtag->needtomalloc) {
            uhos_u8 *pTmp, *pTmp2;
            pObjAddr = (uhos_u8 *) out_param + psubtag->offset;
            pTmp = (uhos_u8 *) out_param;
            pTmp += psubtag->offset;
            if(UHOS_NULL != (*(uhos_u8 **)pTmp)) {
                UTLV_LOGW("tag 0x%x, set needtomalloc, but member %p not NULL.",
                    psubtag->tag, (*(uhos_u8 **)pTmp));
            }
            UTLV_LOGW("ptag 0x%x, tag 0x%x, zalloc %ld. %d, ct %d.",
                ptaginfo->tag, psubtag->tag, psubtag->item_size * arr_size, arr_size, psubtag->ctype);
            pTmp2 = uhos_libc_zalloc(psubtag->item_size * arr_size);
            if(UHOS_NULL == pTmp2) {
                UTLV_LOGE("OOM: %ld * %d.", psubtag->item_size, arr_size);
                continue;
            }
            UTLV_LOGW("TryMalloc: ptag 0x%x, i %d,tag 0x%x; param %p, offset %ld, pTmp %p, maddr %p.",
                ptaginfo->tag, i, psubtag->tag,
                out_param, psubtag->offset, pTmp, pTmp2);
            UTLV_LOGE("subObjAddr %p, %p, alloc_addr %p.", pObjAddr, *(uhos_void**)pObjAddr, pTmp2);
            *((uhos_u8**)(pTmp)) = pTmp2;
            pObjAddr = pTmp2;
        } else {
            pObjAddr = (uhos_u8 *) out_param + psubtag->offset;
        }
        UTLV_LOGE("subObjAddr %p.\n", pObjAddr);
        ret = utlv_decode_inter(ctx,
                psubtag,
                &tmp_data,
                pObjAddr);
        if(ret < 0) {
            UTLV_LOGE("tag 0x%x, decode err %d.SKIP!!!", psubtag->tag, ret);
            continue;
        }
        continue;
    }

    return ret;
}

static uhos_s32 utlv_dec_array(
    const utlv_ctx_t *ctx,
    const utlv_tag_info_t *ptaginfo,
    utlv_data_t *pIn_data,
    uhos_void *out_param)
{
    uhos_s32 ret = UHOS_FAILURE;
    uhos_u32 tmp_len;
    const uhos_u8 *tmp_buf;
    utlv_s_t tlv;
    utlv_data_t tmp_data;
    const utlv_tag_info_t *psubtag;
    const utlv_tag_info_t *pnumtag;
    uhos_u32 subitem_offset = 0;
    uhos_u32 arr_total_size;
    uhos_s32 i=0;

    tmp_len = pIn_data->buf_l;
    tmp_buf = pIn_data->buf;
    arr_total_size = pIn_data->array_size;

    while(tmp_len>0 && arr_total_size>0) {
        uhos_u32 arr_size = 1;
        uhos_libc_memset(&tlv, 0, sizeof(utlv_s_t));
        uhos_u8 *pItemAddr = ((uhos_u8*)out_param + subitem_offset);

        ret = ctx->sctx->tlv_s_dec_func(tmp_buf, tmp_len, &tlv);
        if(ret<0) {
            UTLV_LOGE("dec fail: %d.", ret);
            UTLV_HEX_DUMP("decbuf", tmp_buf, tmp_len);
            return ret;
        }
        tmp_buf = tlv.vbuf + tlv.len;
        if(pIn_data->buf_l < (tmp_buf - pIn_data->buf)) {
            UTLV_LOGE("NED: buf_l %d, tlv_l %d. buf %p, v %p.",
                pIn_data->buf_l, tlv.len, pIn_data->buf, tlv.vbuf);
            tmp_len = 0; //set it as no data status
            return UHOS_FAILURE;
        }
        tmp_len = pIn_data->buf_l - (tmp_buf - pIn_data->buf);

        if(0 == tlv.len) {
            UTLV_LOGW("pTag 0x%x, Tag 0x%x, len is %d (0).",
                ptaginfo->tag, tlv.tag, tlv.len);
            continue;
        }

        //set tmp_data, array_size is 0 now
        uhos_libc_memset(&tmp_data, 0, sizeof(utlv_data_t));
        tmp_data.buf = tlv.vbuf;
        tmp_data.buf_l = tlv.len;

        //we get tlv, need to decode it
        psubtag = utlv_get_subtag(ptaginfo, tlv.tag);
        if(UHOS_NULL == psubtag) {
            UTLV_LOGE("not found taginfo for 0x%x. SKIP!!!", tlv.tag);
            //may new type, just skip then next tlv, so continue, not break
            continue;
        }
        i ++;

        //firstly check if subtag is array
        //  and get arr_size if array
        if(UTLV_T_CT_IS_ARRAY(psubtag->ctype)) {
            uhos_u32 tmp_arr_size = 0;
            pnumtag = utlv_get_subtag(ptaginfo, psubtag->array_num_tag);
            if(UHOS_NULL != pnumtag) {
                tmp_arr_size = utlv_get_int_frm_p(out_param,
                        pnumtag->offset,
                        UTLV_T_CT_GET_TYPE(pnumtag->ctype));
                if(tmp_arr_size <= 0) {
                    UTLV_LOGW("arr_tag 0x%x, ct %d, found 0x%x in ptag 0x%x.\n"
                    "But arr_size invalid %d. offset %ld, numtag 0x%x, ct %d.",
                    tlv.tag, psubtag->ctype, psubtag->array_num_tag, ptaginfo->tag,
                    tmp_arr_size, pnumtag->offset, pnumtag->tag, pnumtag->ctype);
                    tmp_arr_size = 0;
                } else {
                    //set array_size here
                    tmp_data.array_size = arr_size;
                }
            } else {
                UTLV_LOGW("arr_tag 0x%x, ct %d, not found for 0x%x in ptag 0x%x.",
                    tlv.tag, psubtag->ctype, psubtag->array_num_tag, ptaginfo->tag);
            }
            if(0 == tmp_arr_size) {
            //not get num from numtag
            //try to get tlv's num
                tmp_arr_size = utlv_s_get_tlv_num(ctx->sctx, tlv.vbuf, tlv.len);
            }
            if(0 >= tmp_arr_size) {
                UTLV_LOGW("arr_tag 0x%x, ct %d, arr_size is 0, %d. set it as 1.",
                    tlv.tag, psubtag->ctype, tmp_arr_size);
                tmp_arr_size = 1;
            }
            arr_size = tmp_arr_size;
            tmp_data.array_size = arr_size;
            UTLV_LOGW("arr_size is %d.", arr_size);
        }

        //check psubtag, if need to malloc
        if(psubtag->needtomalloc) {
            uhos_u8 *pTmp;
            if(UHOS_NULL != *((uhos_u8**)pItemAddr)) {
                UTLV_LOGW("tag 0x%x, set needtomalloc, but member %p, %p not NULL.",
                    psubtag->tag, pItemAddr, *((uhos_u8**)pItemAddr));
            }
            UTLV_LOGW("ptag 0x%x, tag 0x%x, zalloc %ld.", ptaginfo->tag, psubtag->tag, psubtag->item_size * arr_size);
            pTmp = uhos_libc_zalloc(psubtag->item_size * arr_size);
            if(UHOS_NULL == pTmp) {
                UTLV_LOGE("OOM: %ld * %d.", psubtag->item_size, arr_size);
                continue;
            }
            UTLV_LOGW("TryMalloc ptag 0x%x,i %d, tag 0x%x; param %p, offset %d, pp %p, pm %p.",
                ptaginfo->tag,i,psubtag->tag, out_param, subitem_offset, pItemAddr, pTmp);
            UTLV_LOGW("ptag 0x%x, tag 0x%x.", ptaginfo->tag, psubtag->tag);
            UTLV_LOGW("pItemAddr %p, pTmp %p.", pItemAddr, pTmp);
            *((uhos_u8**)pItemAddr) = pTmp;
            UTLV_LOGW("pItemAddr %p.", pItemAddr);
        }

        UTLV_LOGW("pItemAddr %p.", pItemAddr);
        ret = utlv_decode_inter(ctx,
                psubtag,
                &tmp_data,
                (uhos_void*)pItemAddr);
        subitem_offset += ptaginfo->item_size;
        arr_total_size --;

        if(ret < 0) {
            UTLV_LOGE("tag 0x%x, decode err %d.SKIP!!!", psubtag->tag, ret);
            continue;
        }
        continue;
    }

    if(tmp_len>0 || arr_total_size>0) {
        UTLV_LOGE("tag 0x%x, some data not handle. len: %d, %d, size: %d, %d.",
            ptaginfo->tag,
            pIn_data->buf_l, tmp_len,
            pIn_data->array_size, arr_total_size);
    }

    return ret;
}

static uhos_s32 utlv_decode_inter(
    const utlv_ctx_t *ctx,
    const utlv_tag_info_t *ptaginfo,
    utlv_data_t *pIn_data,
    uhos_void *out_param)
{
    uhos_s32 ret = UHOS_FAILURE;

    UTLV_LOGD("IN, tag 0x%x, %p, %p, %p, %p.",
        ptaginfo->tag, ctx, ptaginfo, pIn_data, out_param);
    UTLV_LOGD("buf %p, buf_l %d, 0x%x %x %x %x %x %x.",
    pIn_data->buf, pIn_data->buf_l,
    pIn_data->buf[0],
    pIn_data->buf[1],
    pIn_data->buf[2],
    pIn_data->buf[3],
    pIn_data->buf[4],
    pIn_data->buf[5]);

    //check ctype
    if(UTLV_T_CT_IS_EASY_SET_TYPE(ptaginfo->ctype)) {
        ret = ctx->sctx->tlv_s_d2etv_func(
                ptaginfo->ctype,
                pIn_data->buf,
                pIn_data->buf_l,
                out_param);
    } else if(UTLV_T_CT_IS_BASE_ARRAY(ptaginfo->ctype)) {
        ret = utlv_dec_set_base_array(ctx,
                ptaginfo, pIn_data, out_param);
    } else if(UTLV_T_CT_IS_OBJECT(ptaginfo->ctype)) {
        ret = utlv_dec_object(ctx,
                ptaginfo, pIn_data, out_param);
    } else if(UTLV_T_CT_IS_ARRAY(ptaginfo->ctype)) {
        ret = utlv_dec_array(ctx,
                ptaginfo, pIn_data, out_param);
    } else {
        UTLV_LOGE("Unkown ctype %d.", ptaginfo->ctype);
        //more info to print
        ret = UHOS_FAILURE;
    }

    UTLV_LOGD("Out: ret %d, tag 0x%x, ct %d.",
        ret, ptaginfo->tag, ptaginfo->ctype);

    return ret;
}

static uhos_s32 utlv_free_item_in_array(
    const utlv_ctx_t *ctx,
    const utlv_tag_info_t *ptaginfo,
    uhos_void *param,
    uhos_u32 num)
{
    uhos_s32 i;
    uhos_s32 ret = UHOS_FAILURE;
    const utlv_tag_info_t *psubtag;
    uhos_u8 *pSubParam = UHOS_NULL;

    //shall be only one subtag
    if(ptaginfo->nsubtags != 1) {
        UTLV_LOGE("array, tag 0x%x, but nsubtags %d is not 1.", ptaginfo->tag, ptaginfo->nsubtags);
    }
    if(ptaginfo->nsubtags < 1) return ret;
    if(num<1) {
        UTLV_LOGE("array, num %d is less 1.", num);
        return ret;
    }

    psubtag = &(ptaginfo->psubtags[0]);

    UTLV_LOGW("tag 0x%x, subtag 0x%x is %ld, sis %ld, off %ld, p %p, num %d.",
        ptaginfo->tag, psubtag->tag, ptaginfo->item_size,
        psubtag->item_size, ptaginfo->offset, param, num);

    if(UHOS_NULL == param) {
        UTLV_LOGW("p is null, nothing to free.");
        return ret;
    }

    pSubParam = param;
    for(i=0; i<num; i++) {
        if(UTLV_T_CT_IS_ARRAY(psubtag->ctype)) {
            uhos_u32 sub_arr_size;
            sub_arr_size = utlv_get_num_frm_member(ctx,
                ptaginfo, psubtag->array_num_tag, param);
            utlv_free_item_in_array(ctx,
                psubtag,
                pSubParam, sub_arr_size);
        } else {
            ret = utlv_free_inter(ctx, psubtag,
                pSubParam, 0);
        }
        pSubParam += psubtag->item_size;
    }

	return UHOS_SUCCESS;
}

static uhos_s32 utlv_free_inter(
    const utlv_ctx_t *ctx,
    const utlv_tag_info_t *ptaginfo,
    uhos_void *param,
    uhos_u32 arr_size)
{
    uhos_s32 i;
    uhos_s32 ret = UHOS_FAILURE;
    const utlv_tag_info_t *psubtag;

    UTLV_LOGW("tag 0x%x, is %ld, off %ld, p %p, arr_size %d.",
        ptaginfo->tag, ptaginfo->item_size, ptaginfo->offset, param, arr_size);

    if(UHOS_NULL == param) {
        UTLV_LOGW("p is null, nothing to free.");
        return ret;
    }

    if(UTLV_T_CT_IS_ARRAY(ptaginfo->ctype)) {
        utlv_free_item_in_array(ctx,
            ptaginfo,
            param,
            arr_size);
    } else {
        for(i=0; i<ptaginfo->nsubtags; i++) {
            uhos_void *pSubParam = UHOS_NULL;
            psubtag = &(ptaginfo->psubtags[i]);
            if(UHOS_NULL == psubtag) continue;
            //calcuate pSubParam
            if(psubtag->needtomalloc) {
                pSubParam = *(uhos_u8**)(((uhos_u8*)param) + psubtag->offset);
            } else {
                pSubParam = ((uhos_u8*)param) + psubtag->offset;
            }
            if(UTLV_T_CT_IS_ARRAY(psubtag->ctype)) {
                uhos_u32 sub_arr_size;
                sub_arr_size = utlv_get_num_frm_member(ctx,
                    ptaginfo, psubtag->array_num_tag, param);
                utlv_free_item_in_array(ctx,
                    psubtag,
                    pSubParam, sub_arr_size);
            } else {
                ret = utlv_free_inter(ctx, psubtag,
                    pSubParam, 0);
            }

            if(psubtag->needtomalloc) {
                uhos_u8* pTmp;
                pTmp = param;
                pTmp += psubtag->offset;
                UTLV_LOGW("TryFree: pta:0x%x,i:%d, ta:0x%x, param:%p, offset:%ld, pTmp:%p, %p.",
                    ptaginfo->tag, i, psubtag->tag, param, psubtag->offset, pTmp, pSubParam);
                if(UHOS_NULL != pSubParam) {
                    UTLV_LOGW("free: %p, %p, ptag: 0x%x, i %d, tag 0x%x.",
                        *((uhos_u8**)pTmp), pSubParam, ptaginfo->tag, i, psubtag->tag);
                    uhos_libc_free(pSubParam);
                    pSubParam = UHOS_NULL;
                }
            }
        }
    }

    return ret;
}

utlv_ctx_t *utlv_get_ctx(utlv_s_type_t t)
{
    utlv_ctx_t *ctx = UHOS_NULL;
    const utlv_s_ctx_t* sctx = UHOS_NULL;
    sctx = utlv_s_ctx_get(t);
    if(UHOS_NULL == sctx) {
        return UHOS_NULL;
    }

    ctx = uhos_libc_zalloc(sizeof(utlv_ctx_t));
    if(UHOS_NULL == ctx) {
        UTLV_LOGE("OOM: ctx %ld.", sizeof(utlv_ctx_t));
        return UHOS_NULL;
    }

    ctx->sctx = sctx;
    ctx->enc_func = utlv_encode_inter;
    ctx->dec_func = utlv_decode_inter;
    ctx->free_func = utlv_free_inter;

    return ctx;
}

uhos_void utlv_put_ctx(utlv_ctx_t *ctx)
{
    if(ctx) uhos_libc_free(ctx);
}

uhos_s32 utlv_encode(
    const utlv_ctx_t *ctx,
    const utlv_tag_info_t *ptaginfo,
    uhos_void *in_param,
    utlv_data_t *pOut_data)
{
    if(!ctx || !ptaginfo || !in_param || !pOut_data) {
        UTLV_LOGE("null input: %p, %p, %p, %p.",
            ctx, ptaginfo, in_param, pOut_data);
        return UHOS_FAILURE;
    }
    if(!ctx->sctx || !ctx->enc_func || !ctx->dec_func || !ctx->free_func)
    {
        UTLV_LOGE("wrong ctx: %p, %p, %p, %p.",
            ctx->sctx, ctx->enc_func, ctx->dec_func, ctx->free_func);
        return UHOS_FAILURE;
    }

    return utlv_encode_inter(ctx, ptaginfo, in_param, pOut_data);
}

uhos_s32 utlv_decode(
    const utlv_ctx_t *ctx,
    const utlv_tag_info_t *ptaginfo,
    utlv_data_t *pIn_data,
    uhos_void *out_param)
{
    if(!ctx || !ptaginfo || !pIn_data || !out_param) {
        UTLV_LOGE("null input: %p, %p, %p, %p.",
            ctx, ptaginfo, pIn_data, out_param);
        return UHOS_FAILURE;
    }
    if(!ctx->sctx || !ctx->enc_func || !ctx->dec_func || !ctx->free_func)
    {
        UTLV_LOGE("wrong ctx: %p, %p, %p, %p.",
            ctx->sctx, ctx->enc_func, ctx->dec_func, ctx->free_func);
        return UHOS_FAILURE;
    }
    if(!pIn_data->buf && pIn_data->buf_l<1) {
        UTLV_LOGE("wrong in_data: %p, %d.", pIn_data->buf, pIn_data->buf_l);
        return UHOS_FAILURE;
    }

    return utlv_decode_inter(ctx, ptaginfo, pIn_data, out_param);
}

uhos_s32 utlv_free(
    const utlv_ctx_t *ctx,
    const utlv_tag_info_t *ptaginfo,
    uhos_void *param,
    uhos_u32 num)
{
    if(!ctx || ! ptaginfo || !param) {
         UTLV_LOGE("null input: %p, %p, %p.",
            ctx, ptaginfo, param);
        return UHOS_FAILURE;
    }

    return utlv_free_inter(ctx, ptaginfo, param, num);
}
