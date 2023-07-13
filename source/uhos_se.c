
#include "uh_mutex.h"  //uhos_mutex_t
#include "se_defines.h"
#include "trigger_inner.h"

#include "se_uhos_defines.h"
#include "se_uhos_api_inner.h"

//EPP defines
#define EPP_HEAD      0xFF
#define EPP_TRAN_BYTE 0x55
#define EPP_START_BIT_MAX 15
// head(2) + len(1) + addr(6) + cmd(1) + chksum(1)
#define EPP_MIN_BYTES 11
// addr(6) + cmd(1) + chksum(1)
#define EPP_LEN_MIN   8
#define EPP_ADDR_LEN  6
#define EPP_FRAMETYPE_OFF 9 
#define EPP_STATUSCMD_OFF 10
//return len of epp data, 0 means invalid
static uhos_u32 isvalid_epp(const byte_t *pEpp)
{
	uhos_u32 uret = 0;
	uhos_u32 uff55_l = 0, i,j;
	uhos_u32 sum = 0;
	uhos_u8  epp_len = 0;
	if(UHOS_NULL == pEpp) {
		UHPT_LOGE("input null.");
		return uret;
	}
	if(UHOS_NULL == pEpp->buf || pEpp->l < EPP_MIN_BYTES) {
		UHPT_LOGE("invalid: buf %p, l %d, minBytes %d.", pEpp->buf, pEpp->l, EPP_MIN_BYTES);
		return uret;
	}
	if(EPP_HEAD != pEpp->buf[0] && EPP_HEAD != pEpp->buf[1]) {
		UHPT_LOGE("invalid: no EPP_HEAD 0x%02x,%02x.",
			pEpp->buf[0], pEpp->buf[1]);
		return uret;
	}
	epp_len = pEpp->buf[2];
	if(EPP_HEAD == epp_len || epp_len < EPP_LEN_MIN) {
		UHPT_LOGE("invalid: epp_len %d, shall not %d, and < %d.",
			epp_len, EPP_HEAD, EPP_LEN_MIN);
		return uret;
	}

	UHPT_LOGD("epp_len %d, buf_l %d.", epp_len, pEpp->l);

	//len
	sum = epp_len;
	//addr
	for(i=3,j=0; (j<EPP_ADDR_LEN) && (i<pEpp->l); j++) {
		sum += pEpp->buf[i];
		if(EPP_HEAD == pEpp->buf[i]) {
			if((i+1) >= pEpp->l) {
				UHPT_LOGE("No enough data: i %d, l %d, need more.",
					i+1, pEpp->l);
				return uret;
			}
			if(EPP_TRAN_BYTE != pEpp->buf[i+1]) {
				UHPT_LOGE("invalid EPP: i %d, FF need 55, but 0x%x.",
					i, pEpp->buf[i+1]);
				return uret;
			}
			sum += EPP_TRAN_BYTE;
			i++;
			uff55_l ++;
		}
		i++;
	}
	j ++; //since the len is not counted

	//frameType
	if(i >= pEpp->l) {
		UHPT_LOGE("No enough data: i %d, l %d, need more.",
			i, pEpp->l);
		return uret;
	}
	if(EPP_HEAD == pEpp->buf[i] || 0 == pEpp->buf[i]) {
		UHPT_LOGE("invalid EPP: frameType in %d shall not be 0xff/0x00.", i);
		return uret;
	}
	sum += pEpp->buf[i++];
	j ++;

	//data
	for(; j<epp_len && i<pEpp->l; j++) {
		sum += pEpp->buf[i];
		if(EPP_HEAD == pEpp->buf[i]) {
			if((i+1) >= pEpp->l) {
				UHPT_LOGE("No enough data: i %d, l %d, need more.",
					i+1, pEpp->l);
				return uret;
			}
			if(EPP_TRAN_BYTE != pEpp->buf[i+1]) {
				UHPT_LOGE("invalid EPP: i %d, FF need 55, but 0x%x.",
					i, pEpp->buf[i+1]);
				return uret;
			}
			sum += EPP_TRAN_BYTE;
			i++;
			uff55_l ++;
		}
		i++;
	}
	
	if(i >= pEpp->l) {
		UHPT_LOGE("%d is short, need >= %d.", pEpp->l, i);
		return uret;
	}

	if((sum & 0xFF) != pEpp->buf[i]) {
		UHPT_LOGE("sum 0x%x, %02x ERROR, not match to srcsum 0x%x.",
			sum, (uhos_u8)(sum & 0xFF), pEpp->buf[i]);
		return uret;
	}

	if(EPP_HEAD == pEpp->buf[i]) {
		i++;
		if((i < pEpp->l) && (EPP_TRAN_BYTE == pEpp->buf[i])) {
			UHPT_LOGD("chksum 0xff with 55.");
		} else {
			UHPT_LOGE("chksum 0xff, but i %d l %d", i, pEpp->l);
			if(i < pEpp->l) {
				UHPT_LOGE(" no 55, but 0x%x.", pEpp->buf[i]);
			}
			UHPT_LOGE("But SKIP!!!");
		}
	}

	uret = epp_len + 2;
	UHPT_LOGD("uret %d, epp_len %d, i %d, j %d, l %d, ff55L %d.",
		uret, epp_len, i, j, pEpp->l, uff55_l);

	return uret;
}

static uhos_void EppRemoveTranByte(const byte_t *pEpp, uhos_u8 *pBuf, uhos_u8 uL) {
    uhos_u32 i,j;

	// no params checking, caller ensure it 

    i = j = 0;
	do {
		pBuf[j] = pEpp->buf[i];
		if(EPP_HEAD == pEpp->buf[i]) {
			if(EPP_TRAN_BYTE == pEpp->buf[i+1]) {
				i ++;
			}
		}
		i ++;
		j ++;
	} while(i < pEpp->l && j < uL);

	return;
}

static uhos_u16 bin2u16(uhos_u8 *pD)
{
	uhos_u16 ret = 0;
	ret = pD[0];
	ret = (ret <<8) + pD[1];

	return ret;
}
static uhos_s32 CmpNumVal_u16(uhos_u16 val, uhos_u16 targetV)
{
	uhos_s32 ret = 0;

	ret = val - targetV;

	return ret;
}

enum {
op_undefined,

op_eq,  //equal
op_ne,  //not equal
op_gt,  //greater than
op_lt,  //less than
op_ge,  //greater than or equal
op_le,  //less than or equal

op_last
};

static ConditionState_t getCondStateByOpForEq(uhos_u8 op)
{
	switch(op) {
		case op_eq:
		case op_ge:
		case op_le:
			return Cond_true;

		case op_ne:
		case op_gt:
		case op_lt:
			return Cond_false;

		default:
			
			return Cond_unknown;
	}
}
static ConditionState_t getCondStateByOpForGreater(uhos_u8 op)
{
	switch(op) {
		case op_ge:
		case op_gt:
			return Cond_true;

		case op_eq:
		case op_le:
		case op_ne:
		case op_lt:
			return Cond_false;

		default:
			return Cond_unknown;
	}
}
static ConditionState_t getCondStateByOpForLess(uhos_u8 op)
{
	switch(op) {
		case op_le:
		case op_lt:
			return Cond_true;

		case op_eq:
		case op_ne:
		case op_ge:
		case op_gt:
			return Cond_false;

		default:
			return Cond_unknown;
	}
}

static ConditionState_t getCondState(uhos_s32 cmpRet, uhos_u8 op)
{
	ConditionState_t cmp_state = Cond_unknown;

	if(0 == cmpRet) {
        cmp_state = getCondStateByOpForEq(op);
    } else if(cmpRet > 0) {
        cmp_state = getCondStateByOpForGreater(op);
    } else {
        cmp_state = getCondStateByOpForLess(op);
    }

	return cmp_state;
}

static ConditionState_t CmpEppNumVal(ConditionInfo* pCI, uhos_s32 inV)
{
	uhos_u16 val=0;
	uhos_u16 targetVal = 0;
	uhos_s32 cmpRet = 0;
	ConditionState_t cmp_state = Cond_unknown;

	val = (uhos_u16)(inV & 0xFFFF); //only care u16
	if(1 == pCI->nBytesValue) {
		targetVal = pCI->value[0];
		targetVal = targetVal & 0xFF;
	} else {
		targetVal = bin2u16((uhos_void*)pCI->value);
	}
	cmpRet = CmpNumVal_u16(val, targetVal);

	cmp_state = getCondState(cmpRet, pCI->Operation);

	return cmp_state;
}
static uhos_s32 GetEppNumVal(uhos_u8 *pD, uhos_u32 maxLen, ConditionInfo* pCI, uhos_u32 *pOut)
{
	uhos_s32 ret=-1;
	uhos_u32 uPos=0;
	uhos_u16 wordVal = 0;
	uhos_u16 retVal = 0;
	
	uPos = pCI->StartWord * 2;
	if(maxLen < uPos+1) {
		UHPT_LOGD("no enough data: ml %d, sw %d, sb %d, up %d.",
			maxLen, pCI->StartWord, pCI->StartBit, uPos);
		return ret;
	}

	wordVal = bin2u16(&pD[uPos]);
	retVal = (wordVal >> pCI->StartBit) & (~((1 < (pCI->Length+1))-1));
	UHPT_LOGD("NumVal 0x%0x, 0x%0x, SW %d, SB %d, L %d.",
		retVal, wordVal, pCI->StartWord, pCI->StartBit, pCI->Length);
	
	return ((uhos_s32) retVal);  //may it -1 if it is 0xffff?
	
}
static uhos_s32 GetEppString(uhos_u8 *pD, uhos_u32 maxLen, ConditionInfo* pCI, uhos_u8 **pStr) {
	uhos_s32 ret=-1;
    uhos_u32 uPos=0;

	uPos = pCI->StartWord * 2;
	
	if(maxLen < uPos + ((pCI->Length-1)/8)) {
		UHPT_LOGD("no enough data: ml %d, sw %d, len %d, up %d, (len-1)/8 %d.",
            maxLen, pCI->StartWord, pCI->Length, uPos, ((pCI->Length-1)/8));
		return ret;
	}

	*pStr = pD + uPos;

	return 0;
}
static ConditionState_t CmpEppStrVal(ConditionInfo* pCI, uhos_u8 *pD)
{
	uhos_s32 cmpRet = 0;
	ConditionState_t cmp_state;

	cmpRet = uhos_libc_strncmp((uhos_void*)pD, pCI->value, pCI->nBytesValue);
	cmp_state = getCondState(cmpRet, pCI->Operation);

	return cmp_state;
}
//return 1, if it is number
//return 0, otherwise
static uhos_s32 IsCaeTypeIsNum(uhos_u8 caeType) {
	switch(caeType) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 6:
		return 1;

	default:
		return 0;
	}
	return 0;
}
static uhos_s32 IsCaeTypeIsStr(uhos_u8 caeType) {
	if(0xD == caeType) return 1;

	return 0;
}

static ConditionState_t GetEppCondResult(ConditionInfo* pCI, uhos_u8 *pD, uhos_u32 len)
{
	ConditionState_t ret_state = Cond_unknown;
	uhos_u8 FrameType;
	uhos_u16 StatusCmd;
	uhos_u32 uval = 0;
	uhos_s32 ret;
	uhos_u8 *pStr = UHOS_NULL;
	
	FrameType = pD[EPP_FRAMETYPE_OFF];
	StatusCmd = bin2u16(&pD[EPP_STATUSCMD_OFF]);
	if(FrameType != pCI->FrameType) {
		if( (2 == FrameType || 6 == FrameType)
			&& (2 == pCI->FrameType || 6 == pCI->FrameType))
		{
			UHPT_LOGD("FrameType 02 06 as same");
		} else {
			UHPT_LOGD("FrameType dismatch: 0x%02x, 0x%02x.",
				FrameType, pCI->FrameType);
			goto end;
		}
	}
	UHPT_LOGD("FrameType match: 0x%02x, 0x%02x.",
                FrameType, pCI->FrameType);

	if(StatusCmd != pCI->StatusCmd) {
		UHPT_LOGD("StatusCmd dismatch: 0x%02x, 0x%02x.",
			StatusCmd, pCI->StatusCmd);
		goto end;
	}

	UHPT_LOGD("caeType %d, so far only support num val.", pCI->CaeType);
	if(IsCaeTypeIsNum(pCI->CaeType)) {
		ret = GetEppNumVal(&pD[EPP_STATUSCMD_OFF], (len - EPP_STATUSCMD_OFF), pCI, &uval);
		if(ret < 0) {
			UHPT_LOGD("Fail to get num, ret %d, buflen %d, sw %d, sb %d, bits %d.",
				ret,(len - EPP_STATUSCMD_OFF),
				pCI->StartWord, pCI->StartBit, pCI->Length);
			goto end;
		}
		
		ret_state = CmpEppNumVal(pCI, uval);
	} else if(IsCaeTypeIsStr(pCI->CaeType)) {
		ret = GetEppString(&pD[EPP_STATUSCMD_OFF], (len - EPP_STATUSCMD_OFF), pCI, &pStr);
		if(ret < 0 || UHOS_NULL == pStr) {
			UHPT_LOGD("Fail to get str, ret %d, %p, buflen %d, sw %d, sb %d, bits %d.",
				ret, pStr, (len - EPP_STATUSCMD_OFF),
				pCI->StartWord, pCI->StartBit, pCI->Length);
		}
		ret_state = CmpEppStrVal(pCI, pStr);
	} else {
		UHPT_LOGD("caeType %d not support, only support num or char.", pCI->CaeType);
		goto end;
	}

end:

	return ret_state;
}
static ConditionState_t GetCondResult(ConditionInfo* pCI, const byte_t *pEpp)
{
	ConditionState_t ret_state = Cond_unknown;
	uhos_u32 ureal_len = 0;
	uhos_u8 *pMallocBuf = UHOS_NULL;
	uhos_u8 *pEppBuf = UHOS_NULL;

	if(UHOS_NULL == pCI->value) {
		UHPT_LOGE("value null in ConditionInfo");
		goto end;
	}
	if(pCI->nBytesValue < 1) {
		UHPT_LOGE("nBytesValue %d wrong.", pCI->nBytesValue);
		goto end;
	}
	if(op_undefined >= pCI->Operation || pCI->Operation >= op_last) {
		UHPT_LOGE("Operation %d in ConditionInfo WRONG.", pCI->Operation);
		goto end;
	}
	if(pCI->StartBit > EPP_START_BIT_MAX) {
		UHPT_LOGE("StartBit %d (>%d) in ConditionInfo WRONG.", pCI->StartBit, EPP_START_BIT_MAX);
		goto end;
	}
	if(0 == pCI->Length) {
		UHPT_LOGE("Length %d in ConditionInfo WRONG.", pCI->Length);
		goto end;
	}
	//chk and get len without ff 55
    ureal_len = isvalid_epp(pEpp);
	if(0 == ureal_len) {
		UHPT_LOGE("invalid epp data");
		//dump EPP
		//return Cond_unknown;
		goto end;
	}

	UHPT_LOGD("%d, %d.", ureal_len, pEpp->l);
	//remove 55, ff55 --> ff
    if(ureal_len < pEpp->l) {
		pMallocBuf = uhos_libc_zalloc(ureal_len + 1);
		if(UHOS_NULL == pMallocBuf) {
			UHPT_LOGE("OOM: %d.", ureal_len + 1);
			//return Cond_unknown;
			goto end;
		}
		EppRemoveTranByte(pEpp, pMallocBuf, ureal_len);
		pEppBuf = pMallocBuf;
	} else {
		pEppBuf = pEpp->buf;
	}

	//epp data now is pEppBuf and ureal_len
	//Dump pEpp->buf, pEpp->l and pEppBuf, ureal_len to check
	
    ret_state = GetEppCondResult(pCI, pEppBuf, ureal_len);

end:

	if(pMallocBuf) {
		uhos_libc_free(pMallocBuf);
		pMallocBuf = UHOS_NULL;
		pEppBuf = UHOS_NULL;
	}

	return ret_state;
}

//Device's Trigger data
static uhos_mutex_t gTS_mutex;
static DevTriggerStatusHead_t gTS_head;
//UHOS_MUTEX_WAIT_FOREVER
static uhos_void TS_LOCK(uhos_u32 ms) {
	if(UHOS_NULL == gTS_mutex)
		uhos_mutex_create(&gTS_mutex);

	if(UHOS_NULL == gTS_mutex) {
		UHPT_LOGE("Fail to uhos_mutex_create");
		return;
	}
	uhos_mutex_wait(gTS_mutex, ms);
}
static uhos_void TS_UNLOCK() {
	uhos_mutex_release(gTS_mutex);
}

static uhos_s32 isSameBuf(uhos_u8 *pD1, uhos_u8 *pD2, uhos_size_t n)
{
	uhos_s32 ret = -1;
	if(pD1[0] && pD2[0])
		ret = uhos_libc_strncmp((uhos_void*)pD1, (uhos_void*)pD2, n);

	return ret;
}
//return 0 if same
static uhos_s32 isSameDevInfo(DeviceId *dev1, DeviceId *dev2)
{
	uhos_s32 ret = -1;
 
    if(UHOS_NULL == dev1 || UHOS_NULL == dev2) {
		UHPT_LOGE("input null: %p, %p.", dev1, dev2);
		return ret;
	}

	//id
	ret = isSameBuf((uhos_void*)dev1->id, (uhos_void*)dev2->id, sizeof(dev1->id));
	if(0 == ret) {
		UHPT_LOGD("id same [%s]", dev1->id);
		return ret;
	}

	//TempId
	ret = isSameBuf((uhos_void*)dev1->TempId, (uhos_void*)dev2->TempId, sizeof(dev1->TempId));
	if(0 == ret) {
		UHPT_LOGD("TempId same [%s]", dev1->TempId);
		return ret;
	}

	//BleMac
	ret = isSameBuf((uhos_void*)dev1->BleMac, (uhos_void*)dev2->BleMac, sizeof(dev1->BleMac));
	if(0 == ret) {
		UHPT_LOGD("BleMac same [%s]", dev1->BleMac);
		return ret;
	}
	
	return ret;
}

static DevTriggerStatus_t *TryFindItemByDevInfo(
	struct list_head *pHead, DeviceId *pDevInfo)
{
	DevTriggerStatus_t *ret, *pos, *next;

	ret = pos = next = UHOS_NULL;
	list_for_each_entry_safe(pos, next, pHead, list) {
		if(pos && !isSameDevInfo(&(pos->devInfo), pDevInfo))
		{
			ret = pos;
		}
	}

	return ret;
}
static uhos_void DevInfoCpy(DeviceId *pTdev, DeviceId *pSdev)
{
	//id
	uhos_libc_memcpy(pTdev->id, pSdev->id, sizeof(pTdev->id));

	//TempId
	uhos_libc_memcpy(pTdev->TempId, pSdev->TempId, sizeof(pTdev->TempId));

	//BleMac
	uhos_libc_memcpy(pTdev->BleMac, pSdev->BleMac, sizeof(pTdev->BleMac));
}
static uhos_void SceneInfoSet(TriggerStatus_t *pTS,
	uhos_u16 sid, uhos_u8 rid, uhos_u8 cid)
{
	pTS->sid = sid;
	pTS->rid = rid;
	pTS->cid = cid;
}

//return 0: macthed
//others: not match
static uhos_s32 isSameCid(TriggerStatus_t *pTS,
	uhos_u16 sid, uhos_u8 rid, uhos_u8 cid)
{
	if(pTS) {
		if(sid == pTS->sid && rid == pTS->rid && cid == pTS->cid) {
			return 0;
		}
	}

	return 1;
}
static TriggerStatus_t *TryFindItemBySinfo(
	struct list_head *pHead,
	uhos_u16 sid, uhos_u8 rid,
	uhos_u8 cid)
{
    TriggerStatus_t *ret, *pos, *next;

    ret = pos = next = UHOS_NULL;
    list_for_each_entry_safe(pos, next, pHead, list) {
        if(pos && !isSameCid(pos, sid, rid, cid))
        {
            ret = pos;
        }
    }

    return ret;

}
static uhos_s32 UpdateCondForDev(uhos_u16 sid, uhos_u8 rid,
	uhos_u8 cid, DeviceId *pDevInfo, ConditionState_t cr)
{
	uhos_s32 isChanged = 0;
	DevTriggerStatus_t *pDTS = UHOS_NULL;
	TriggerStatus_t *pTS = UHOS_NULL;
	
	TS_LOCK(UHOS_MUTEX_WAIT_FOREVER);

	if(0 == gTS_head.inited) {
		INIT_LIST_HEAD(&gTS_head.trigger_head);
		gTS_head.inited = 1;
	}

	//find DevTriggerStatus_t
	pDTS = TryFindItemByDevInfo(&gTS_head.trigger_head, pDevInfo);
	if(UHOS_NULL == pDTS) {
		//not found, so create and add
		pDTS = uhos_libc_zalloc(sizeof(DevTriggerStatus_t));
		if(UHOS_NULL == pDTS) {
			UHPT_LOGE("OOM: %d.", sizeof(DevTriggerStatus_t));
			goto end;
		}
		//init data
		DevInfoCpy(&pDTS->devInfo, pDevInfo);
		INIT_LIST_HEAD(&pDTS->status_head);
		uhos_se_list_add(&pDTS->list, &gTS_head.trigger_head);
	}

	pTS = TryFindItemBySinfo(&pDTS->status_head, sid, rid, cid);
	if(UHOS_NULL == pTS) {
		//not found
		pTS = uhos_libc_zalloc(sizeof(TriggerStatus_t));
		if(UHOS_NULL == pTS) {
			UHPT_LOGE("OOM: %d.", sizeof(TriggerStatus_t));
			goto end;
		}
		//init data
		SceneInfoSet(pTS, sid, rid, cid);
		uhos_se_list_add(&pTS->list, &pDTS->status_head);
	}

	//check if ischanged
	if(cr == pTS->state) {
		UHPT_LOGD("no changed: %d -> %d, %d, %d, %d. %d.",
			pTS->state, cr, sid, rid, cid);
		isChanged = 0;
	} else {
		UHPT_LOGD("CHANGED: %d -> %d, %d, %d, %d.",
			pTS->state, cr, sid, rid, cid);
		isChanged = 1;
		pTS->isTriggered = 1;
		pTS->state = cr;
	}

end:
	TS_UNLOCK();

	return isChanged;
}
static uhos_void SceneInfoCpy(TriggerState_t *pTState, TriggerStatus_t *TS)
{
	(pTState->condInfo).tid = TS->sid;
	(pTState->condInfo).rid = TS->rid;
	(pTState->condInfo).id  = TS->cid;
	pTState->state = TS->state;
}
//static uhos_s32 FetchClearDevStatus(DeviceId *pDevInfo, TriggerStatus* status, uhos_s32 maxN)
static uhos_s32 FetchClearDevStatus(DeviceId *pDevInfo, TriggerStatus* status)
{
	uhos_s32 i = 0, num=0;
	DevTriggerStatus_t *pDTS = UHOS_NULL;
    //TriggerStatus_t *pTS = UHOS_NULL;
	TriggerState_t *pTState = UHOS_NULL;
	struct list_head *pHead;
	TriggerStatus_t *pos, *next;

	TS_LOCK(UHOS_MUTEX_WAIT_FOREVER);
	if(0 == gTS_head.inited) {
		UHPT_LOGE("gTS_head not inited.");
		goto end;
	}

	pDTS = TryFindItemByDevInfo(&gTS_head.trigger_head, pDevInfo);
	if(UHOS_NULL == pDTS) {
		UHPT_LOGE("pDTS null, not found for DevInfo");
		//PRINT_DEVINFO
		goto end;
	}

	//shall here to get num, then malloc, then copy
	pHead = &pDTS->status_head;
	if(UHOS_NULL == pHead) {
		UHPT_LOGE("status_head null in pDTS.");
		goto end;
	}

	pos = next = UHOS_NULL;
	num = 0;
    list_for_each_entry_safe(pos, next, pHead, list) {
        if(pos && pos->isTriggered)
        {
            num ++;
        }
    }

	if(num > 0) {
		status->TriStates = uhos_libc_zalloc(num * sizeof(TriggerState_t));
		if(UHOS_NULL == status->TriStates) {
			UHPT_LOGE("OOM: %d, %d.", (num * sizeof(TriggerState_t)), num);
			goto end;
		}
	} else {
		UHPT_LOGD("No changed cid.");
		goto end;
	}
	
	pos = next = UHOS_NULL;
	pHead = &pDTS->status_head;
    i = 0;
	list_for_each_entry_safe(pos, next, pHead, list) {
        if(pos && pos->isTriggered)
        {
			if(i >= num) {
				UHPT_LOGE("i %d >= num %d. WRONG", i, num);
				break;
			}
			pTState = status->TriStates + i;
			SceneInfoCpy(pTState, pos);
            pos->isTriggered = 0;
			i++;
			status->nTriStates = i;
        }
    }

end:
	TS_UNLOCK();

	UHPT_LOGD("i %d, n %d.", i, num);
	return i;
}
static uhos_s32 chkCondForEpp(SceneInfo *pSceneInfo, const byte_t *pEpp, TriggerStatus* status, DeviceId *pDevInfo)
{
	uhos_s32 ret = SE_FAILED;
	uhos_s32 i,j,num;
	if(UHOS_NULL == pSceneInfo->Rules) {
		UHPT_LOGE("no Rules, Ruels is null.");
		return ret;
	}
    num = 0;
    for(i=0; i<pSceneInfo->RuleNum; i++) {
		RuleInfo *pRI = pSceneInfo->Rules+i;
		if(UHOS_NULL == pRI->Conditions) {
			UHPT_LOGD("Conditions null, i %d.", i);
			continue;
		}
		for(j=0; j<pRI->ConditionNum; j++) {
			ConditionInfo *pCI = pRI->Conditions+j;
			ConditionState_t CondResult;
			uhos_s32 isChanged;
			CondResult = GetCondResult(pCI, pEpp);
			if(Cond_unknown == CondResult) {
				UHPT_LOGD("CR unknown, info %d, %d, %d; i %d, j%d.",
					pSceneInfo->Id, pRI->RuleId, pCI->CondId, i, j);
				//hexdump pEpp?
				continue;
			}
			isChanged = UpdateCondForDev(pSceneInfo->Id, pRI->RuleId, pCI->CondId, pDevInfo, CondResult);
			if(isChanged) num++;
			UHPT_LOGD("CR %d, isC %d, num %d. info: %d, %d, %d; i %d, j%d.",
				CondResult, isChanged, num,
					pSceneInfo->Id, pRI->RuleId, pCI->CondId, i, j);
		}
	}

	num = FetchClearDevStatus(pDevInfo, status);

	ret = 0;

	return ret;
}

uhos_s32 se_uhos_chkDevCond(Trigger *trigger,
	const DevProperties* properties, 
	TriggerStatus* status)
{
    uhos_s32 ret = SE_FAILED;

	if(UHOS_NULL == trigger || UHOS_NULL == properties || UHOS_NULL == status)
	{
		UHPT_LOGE("input null: %p, %p, %p.", trigger, properties, status);
		return SE_FAILED;
	}

    if(DevProp_type_epp != properties->type) {
		UHPT_LOGE("DevProp_type %d wrong.", properties->type);
		return SE_FAILED;
	}

    //ret = chkCondForEpp(&(trigger->SceneData), &(properties->data.epp), status, &(trigger->Trigger));
	ret = chkCondForEpp(&(trigger->SceneData), &(properties->data.epp), status, &(trigger->TriggerDev));
    status->task_no = properties->task_no;
	//devinfo copy
	DevInfoCpy(&status->TriggerDev, &trigger->TriggerDev);  //or Trigger
	DevInfoCpy(&status->ScheduleDev, &trigger->Scheduler);
   
	return ret; 
}
