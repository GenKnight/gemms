/********************************************************************
 *  Created:    2017/10/18 14:42
 *  File name:  stream_parse_xml.h
 *  Author:     harvey
 *  Purpose:
 *
 *  Copyright 2010-2013, All Rights Reserved.
 ********************************************************************/
#pragma once

#include <functional>
#include <QXmlStreamReader>
#include <QXmlStreamAttributes>

#include "iwkats_core_global.h"

namespace iwkats {

class IWKATS_CORE_EXPORT StreamParseXml {
public:
	using ParseFunc = std::function<void(const QString&, const QXmlStreamAttributes& attributes)>;
	explicit StreamParseXml(const QString& file_name);

	// 获取每一个 element中的 属性
	bool ReadXml(const ParseFunc& parse_func);

private:
	QString file_name_;
	QXmlStreamReader reader_;
};


} /*namespace iwkats*/
