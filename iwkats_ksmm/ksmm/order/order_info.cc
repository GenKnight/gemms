#include "order_info.h"

#include <sstream>

#include "core/util.hpp"
#include "ksmm/common_define.hpp"

#include "ksmm/ksmm_trader.h"

IWKATS_NAMESPACE_BEGIN


OrderInfo::OrderInfo(std::shared_ptr<ORDER_INFO> order_ptr)
  : order_ptr_(order_ptr) {

  std::ostringstream oss;
  oss << "[" << order_ptr->szStkCode
      << "] 叫卖委托状态:"
      <<  GetOrderStatusMean(order_ptr->chAskOrderStatus).toLocal8Bit().toStdString()
      << ", 叫买委托状态: " << GetOrderStatusMean(order_ptr->chBidOrderStatus).toLocal8Bit().toStdString()
      << ", 委托合同序号: " << order_ptr->szOrderID;
  info_msg_ = core_util::toUtf8(oss.str().data());
  oss.str("");

  const std::shared_ptr<ORDER_INFO>& pOrderInfo = order_ptr;
  oss << "股票[" << pOrderInfo->szStkCode << "]"
    << " <叫卖状态: " << GetOrderStatusMean(pOrderInfo->chAskOrderStatus).toLocal8Bit().toStdString()
    << " > <叫买状态: " << GetOrderStatusMean(pOrderInfo->chBidOrderStatus).toLocal8Bit().toStdString()
    << "> 叫卖价: " << pOrderInfo->dAskPrice << "; 叫卖量: " << pOrderInfo->nAskVolume
    << "; 卖撤单量: " << pOrderInfo->nAskCancelVol
    << " 叫买价: " << pOrderInfo->dBidPrice << "; 叫买量: " << pOrderInfo->nBidVolume
    << "; 买撤单量: " << pOrderInfo->nBidCancelVol
    << "; 委托合同序号: " << pOrderInfo->szOrderID;
  const std::string& order_details = oss.str();
  LOG_INFO << order_details;
  order_details_ = QString::fromLocal8Bit(order_details.data());
}

void OrderInfo::cancelOrder()
{
  // 如果没有全部成交，则撤单
  if(!order_ptr_->bAllMatched) {
    Trader::Api()->cancelOrder(order_ptr_->szOrderID);
  }
}

IWKATS_NAMESPACE_END
