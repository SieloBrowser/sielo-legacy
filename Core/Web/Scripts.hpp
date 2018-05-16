/***********************************************************************************
** MIT License                                                                    **
**                                                                                **
** Copyright (c) 2018 Victor DENIS (victordenis01@gmail.com)                      **
**                                                                                **
** Permission is hereby granted, free of charge, to any person obtaining a copy   **
** of this software and associated documentation files (the "Software"), to deal  **
** in the Software without restriction, including without limitation the rights   **
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      **
** copies of the Software, and to permit persons to whom the Software is          **
** furnished to do so, subject to the following conditions:                       **
**                                                                                **
** The above copyright notice and this permission notice shall be included in all **
** copies or substantial portions of the Software.                                **
**                                                                                **
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     **
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       **
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    **
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         **
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  **
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  **
** SOFTWARE.                                                                      **
***********************************************************************************/

#pragma once
#ifndef CORE_SCRIPTS_HPP
#define CORE_SCRIPTS_HPP

#include <QString>

#include <QWebEngineView>
#include <QUrlQuery>

namespace Sn {

class Scripts {
public:
	static QString sendPostData(const QUrl& url, const QByteArray& data)
	{
		QString source{QLatin1String("(function() {"
									 "var form = document.createElement('form');"
									 "form.setAttribute('method', 'POST');"
									 "form.setAttribute('action', '%1');"
									 "var val;"
									 "%2"
									 "form.submit();"
									 "})()")};

		QString valueSource{QLatin1String("val = document.createElement('input');"
										  "val.setAttribute('type', 'hidden');"
										  "val.setAttribute('name', '%1');"
										  "val.setAttribute('value', '%2');"
										  "form.appendChild(val);")};

		QString values{};
		QUrlQuery query{data};

		const auto& queryItems = query.queryItems(QUrl::FullyDecoded);

		for (const auto& pair : queryItems) {
			QString value{pair.first};
			QString key{pair.second};
			value.replace(QLatin1String("'"), QLatin1String("\\'"));
			key.replace(QLatin1String("'"), QLatin1String("\\'"));
			values.append(valueSource.arg(value, key));
		}

		return source.arg(url.toString(), values);
	}

	static QString getAllMetaAttributes()
	{
		QString source = QLatin1String("(function() {"
									   "var out = [];"
									   "var meta = document.getElementsByTagName('meta');"
									   "for (var i = 0; i < meta.length; ++i) {"
									   "    var e = meta[i];"
									   "    out.push({"
									   "        name: e.getAttribute('name'),"
									   "        content: e.getAttribute('content'),"
									   "        httpequiv: e.getAttribute('http-equiv')"
									   "    });"
									   "}"
									   "return out;"
									   "})()");

		return source;
	}

	static QString getAllImages()
	{
		QString source = QLatin1String("(function() {"
									   "var out = [];"
									   "var imgs = document.getElementsByTagName('img');"
									   "for (var i = 0; i < imgs.length; ++i) {"
									   "    var e = imgs[i];"
									   "    out.push({"
									   "        src: e.src,"
									   "        alt: e.alt"
									   "    });"
									   "}"
									   "return out;"
									   "})()");

		return source;
	}

};
}

#endif //CORE_SCRIPTS_HPP
