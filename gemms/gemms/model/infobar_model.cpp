#include "infobar_model.h"

#include "ksmm/ksmm_trader.h"

#include "core/util.hpp"

IWKATS_NAMESPACE_BEGIN

InfoBarModel::InfoBarModel(QObject *parent) {

  // 错误处理
  connect(Trader::Spi(), &KSMMTradingSpi::sigRspError, this, &InfoBarModel::slotNotice);

}

void InfoBarModel::slotNotice(std::shared_ptr<LBM_ERROR_INFO> pRspError)
{
  if(0 == pRspError->nAssetID
     || QString::fromLocal8Bit(pRspError->szErrorInfo).contains("没有取得数据")) {
    return;
  }

  // 是否属于报价错误
  QString str_req_id(pRspError->szUserDefine);
  if(str_req_id.isEmpty()) {
    QString code = QString::fromLocal8Bit(pRspError->szStkCode);
    std::ostringstream oss;
    oss << "资产单元编号: " << pRspError->nAssetID << ", "
        << "; 委托号: " << pRspError->szOrderID << " "
        << "; 错误: " << pRspError->szErrorInfo;
    QString error_msg = QString::fromLocal8Bit(oss.str().data());
    LOG_INFO << error_msg.toLocal8Bit().toStdString();
    emit sigNotice(code, ErrorNotice, error_msg);
  }
}


IWKATS_NAMESPACE_END
