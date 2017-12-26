#include "iwkats_core.h"

IWKATS_NAMESPACE_BEGIN

char* GetErrorMsgByCode(int err_code) {
  switch(err_code) {
    case kLimitReqSeconds: return "每秒发送请求数量超限";
    case kQueueLimit: return "请求队列超限";
    case kNetwordError: return "网络原因ctp失败";
    case kSuccess: return "Success";
    case kUnpackMsgError: return "解包失败";
    case kUnsupportMsg: return "不支持的请求";
    case kNeedLoginFirst: return "必须先登录";
    default: return "未知错误";
    }
}

IWKATS_NAMESPACE_END
