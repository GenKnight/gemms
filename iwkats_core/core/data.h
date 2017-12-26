/********************************************************************
 *  Created:    2017/10/24 17:04
 *  File name:  data.h
 *  Author:     harvey
 *  Purpose:
 *
 *  Copyright 2010-2013, All Rights Reserved.
 ********************************************************************/
#pragma once

#include "iwkats_core.h"

#include <array>

#include "util.hpp"

IWKATS_NAMESPACE_BEGIN

struct IWKATS_CORE_EXPORT RawLine {
  enum TransactionType {
    KK, DK, SK, KP, DP, SP, SH, DH, KH
  }; // see below for meaning
  enum TransactionDir {
    SELL, BUY
  };

  std::string market;
  std::string ticker;
  TimePoint time;
  double latestPrice;
  int64_t openInterest;
  int64_t openInterestDelta;
  double tradedMoney;
  double tradedVol;
  int64_t opening;
  int64_t closing;
  TransactionType transType;
  TransactionDir transDir;
  std::array<double, 5> askPrice, bidPrice;
  std::array<int, 5> askVol, bidVol;

  double lastTradedVol;
};

const std::unordered_map<std::string, RawLine::TransactionType> transactionTypeLKP =
{{"空开", RawLine::KK},{"多开", RawLine::DK},{"双开", RawLine::SK},
{"空平", RawLine::KP},{"多开", RawLine::DP},{"双平", RawLine::SP},
{"双换", RawLine::SH},{"多换", RawLine::DH},{"空换", RawLine::KH}};

const std::unordered_map<std::string, RawLine::TransactionDir> transactionDirLKP =
{{"S", RawLine::SELL},{"B", RawLine::BUY}};

IWKATS_NAMESPACE_END