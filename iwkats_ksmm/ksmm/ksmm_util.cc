#include "ksmm_util.h"

#include <ctime>
#include <chrono>

IWKATS_NAMESPACE_BEGIN

QString KSMMUtil::GenerateReqId(int type) {
  std::chrono::system_clock::time_point curr = std::chrono::system_clock::now();
  std::time_t tt = std::chrono::system_clock::to_time_t(curr);
  QString str_id = QString::number(type) + "," + QString::number(tt);
  return str_id;
}

IWKATS_NAMESPACE_END
