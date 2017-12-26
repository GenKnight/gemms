#pragma  once

#include "core/iwkats_core.h"
#include "ksmm/common_define.hpp"

#include <QObject>

IWKATS_NAMESPACE_BEGIN

class IWKATS_CORE_EXPORT KSMMUtil {
public:
  static QString GenerateReqId(int type);


};

IWKATS_NAMESPACE_END
