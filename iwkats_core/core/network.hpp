/********************************************************************
 *  Created:    2017/10/18 17:15
 *  File name:  network.hpp
 *  Author:     harvey
 *  Purpose:
 *
 *  Copyright 2010-2013, All Rights Reserved.
 ********************************************************************/
#pragma once

#include "iwkats_core.h"

#include <chrono>
#include <string>
#include <sstream>

#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>

#include "logging.h"

IWKATS_NAMESPACE_BEGIN

class IWKATS_CORE_EXPORT MsgUtil {
public:
  static bool GetCtpMsgOpcode(const std::string& msg, CtpOpcode opcode) {
    return FromJson(msg, "head", opcode);
  }

  static bool GetCtpStatus(const std::string& msg, Status& status) {
    return FromJson(msg, "status", status);
  }

  template <typename T>
  static bool GetCtpBody(const std::string& msg, T& body) {
    return FromJson(msg, "body", body);
  }

  template <typename ConnPtr>
  static bool SendMsgByConnection(ConnPtr& connection, const std::string& msg) {
    // 发送消息
    try {
      connection->send(msg);
    } catch(std::exception& e) {
      LOG_ERROR << "发送消息失败: " << e.what();
      return false;
    }
    return true;
  }

  // 解消息
  template <typename T>
  static bool FromJson(const std::string& msg, const std::string& name, T& value) {
    try {
      std::istringstream iss(msg);
      cereal::JSONInputArchive archive(iss);
      archive(
        cereal::make_nvp(name, value)
      );
    } catch(std::exception& e) {
      LOG_ERROR << "解析Json获取 " << name << " 错误: " << e.what();
      return false;
    }
    return true;
  }
};

IWKATS_NAMESPACE_END


