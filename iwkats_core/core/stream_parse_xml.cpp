#include "stream_parse_xml.h"

#include <QFile>
#include <QDebug>
#include <QIODevice>

#include "logging.h"

namespace iwkats {

StreamParseXml::StreamParseXml(const QString& file_name) 
	: file_name_(file_name) {


}

bool StreamParseXml::ReadXml(const ParseFunc& parse_func) {
	QFile file(file_name_);
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		LOG_ERROR << "Xml配置文件打开失败 " + file_name_.toStdString();
		return false;
	}
	reader_.setDevice(&file);
	while(!reader_.atEnd() && !reader_.hasError()) {
		QXmlStreamReader::TokenType token = reader_.readNext();
		if(QXmlStreamReader::StartDocument == token) {
			continue;
		}
		if(reader_.isStartElement()) {
			QString element_name = reader_.name().toString();
			QXmlStreamAttributes attributes = reader_.attributes();
			parse_func(element_name, attributes);
		}
		if(reader_.hasError()) {
			qDebug() << "   Error Type:" << reader_.error();
			qDebug() << " Error String:" << reader_.errorString();
			qDebug() << "  Line Number:" << reader_.lineNumber();
			qDebug() << "Column Number:" << reader_.columnNumber();
			qDebug() << " Char. Offset:" << reader_.characterOffset();
		}
	}
	file.close();
	return true;
}

} /*namespace iwkats*/
