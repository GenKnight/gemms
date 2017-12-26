/********************************************************************
 *  Created:    2017/10/18 16:05
 *  File name:  iwkats_core.h
 *  Author:     harvey
 *  Purpose:
 *
 *  Copyright 2010-2013, All Rights Reserved.
 ********************************************************************/
#pragma once

#include "iwkats_core_global.h"

#include <set>
#include <vector>
#include <string>
#include <QObject>

#include <memory>

#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/set.hpp>

IWKATS_NAMESPACE_BEGIN

// 消息代码
enum CtpOpcode {
  C2S_UNKNOWN_MSG,                          //未知消息
  C2S_REQ_SUBSCRIBE_MSG,                    //订阅
  C2S_REQ_UNSUBSCRIBE_MSG,                  //取消订阅
  S2C_RSP_BROADCAST_DEPTH_MARKET_DATA,		//广播行情
  C2S_REQ_LOGIN_TRADE_SERVER,               //登录交易服务器
};

// 错误代码
enum ErrorCode {
  kLimitReqSeconds = -3,                    //每秒发送请求数量超限
  kQueueLimit = -2,                         //请求队列超限
  kNetwordError = -1,                       //网络原因ctp失败
  kSuccess = 0,
  kUnpackMsgError,
  kUnsupportMsg,
  kNeedLoginFirst,
};

// 根据错误代码获取错误消息
extern "C" IWKATS_CORE_EXPORT char* GetErrorMsgByCode(int err_code);

// 错误状态
struct IWKATS_CORE_EXPORT Status {
  Status() = default;
  Status(ErrorCode code) : err_code(code) {}
  Status(ErrorCode code, const std::string& msg) : err_code(code), err_msg(msg) {}

  template <typename Archive>
  void serialize(Archive& ar) {
    if(err_msg.empty()) {
      err_msg = GetErrorMsgByCode(err_code);
    }

    ar(cereal::make_nvp("code", err_code),
       cereal::make_nvp("message", err_msg)
    );
  }

  bool ok() const {
    return kSuccess != err_code;
  }

  QString errorMsg() {
    if(err_msg.empty()) {
      err_msg = GetErrorMsgByCode(err_code);
    }
    return QString::fromStdString(err_msg);
  }

  ErrorCode err_code{kSuccess};
  std::string err_msg{""};
};

IWKATS_NAMESPACE_END

