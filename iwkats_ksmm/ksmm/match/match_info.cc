#include "match_info.h"

#include <sstream>

#include "core/util.hpp"
#include "core/ttspeech.h"

#include "ksmm/common_define.hpp"

IWKATS_NAMESPACE_BEGIN

MatchInfo::MatchInfo(std::shared_ptr<MATCH_INFO> match_ptr)
  : match_ptr_(match_ptr)
{
  std::ostringstream oss;
  oss << "[" << match_ptr->szStkCode << "] 成交编号" << match_ptr->szOrderID;
  info_msg_ = core_util::toUtf8(oss.str().data());
  oss.str("");

  /////////////////////////////////////////////////////////////////////////////

  const std::shared_ptr<MATCH_INFO>& pMatchInfo = match_ptr;
  oss << "股票[" << pMatchInfo->szStkCode << "]"
      << " 撤单标志: " << GetIsWithdrawMean(pMatchInfo->chIsWithdraw).toLocal8Bit().toStdString()
      << "; 叫卖价格: " << pMatchInfo->dAskOrderPrice
      << "; 叫卖委托数量: " << pMatchInfo->nAskOrderQty
      << "; 叫卖成交数量: " << pMatchInfo->nAskMatchedQty
      << "; 叫买价格: " << pMatchInfo->dBidOrderPrice
      << "; 叫买委托数量: " << pMatchInfo->nBidOrderQty
      << "; 叫买成交数量: " << pMatchInfo->nBidMatchedQty
      << "; 委托合同序号: " << pMatchInfo->szOrderID;

  const std::string& match_details = oss.str();
  LOG_INFO << match_details;
  match_details_ = QString::fromLocal8Bit(match_details.data());
  oss.str("");

  /////////////////////////////////////////////////////////////////////////////

  int nAskMatchVol = pMatchInfo->nAskMatchedQty;
  int nBidMatchVol = pMatchInfo->nBidMatchedQty;

  bool bNeedSay = false;
  oss << NumberToSpeech(pMatchInfo->szStkCode).toStdString();
  if("T" == QString(pMatchInfo->chIsWithdraw)) {
    oss << " 已撤单";
  } else {
    bNeedSay = true;
    oss << "已成交";
  }

  if(std::abs(nAskMatchVol) > 0) {
    oss << " 叫卖成交 "
        << std::abs(nAskMatchVol)
        << " 股; ";
  }

  if(std::abs(nBidMatchVol) > 0) {
    oss << " 叫买成交 "
        << std::abs(nBidMatchVol)
        << " 股";
  }
  speech_msg_ = QString::fromLocal8Bit(oss.str().data());
  if(bNeedSay) {
    TTSpeech::GetPtr()->say(speech_msg_);
  }
}

IWKATS_NAMESPACE_END
