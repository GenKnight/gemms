/********************************************************************
 *  Created:    2017/10/18 16:09
 *  File name:  config.h
 *  Author:     harvey
 *  Purpose:
 *
 *  Copyright 2010-2013, All Rights Reserved.
 ********************************************************************/
#pragma once

#include "iwkats_core_global.h"

#include <QString>
#include <memory>

IWKATS_NAMESPACE_BEGIN

struct MysqlInfo {
	QString host;
	int port;
	QString username;
	QString password;
	QString database;
};

struct CtpAccountInfo {
	QString broker_id;
	QString investor_id;
	QString password;
	QString md_front_addr;
	QString td_front_addr;
};

using MysqlInfoPtr = std::shared_ptr<MysqlInfo>;
using CtpAccountInfoPtr = std::shared_ptr<CtpAccountInfo>;

IWKATS_NAMESPACE_END
