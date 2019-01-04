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

#include "SharedDefines.hpp"

#include <QString>

#include <QUrlQuery>

namespace Sn
{
class SIELO_SHAREDLIB Scripts {
public:
	static QString autoFill()
	{
		QString source = QLatin1String("(function() {"
			"function findUsername(inputs) {"
			"    for (var i = 0; i < inputs.length; ++i)"
			"        if (inputs[i].type == 'text' && inputs[i].value.length && inputs[i].name.indexOf('user') != -1)"
			"            return inputs[i].value;"
			"    for (var i = 0; i < inputs.length; ++i)"
			"        if (inputs[i].type == 'text' && inputs[i].value.length && inputs[i].name.indexOf('name') != -1)"
			"            return inputs[i].value;"
			"    for (var i = 0; i < inputs.length; ++i)"
			"        if (inputs[i].type == 'text' && inputs[i].value.length)"
			"            return inputs[i].value;"
			"    for (var i = 0; i < inputs.length; ++i)"
			"        if (inputs[i].type == 'email' && inputs[i].value.length)"
			"            return inputs[i].value;"
			"    return '';"
			"}"
			""
			"function registerForm(form) {"
			"    form.addEventListener('submit', function() {"
			"        var form = this;"
			"        var data = '';"
			"        var password = '';"
			"        var inputs = form.getElementsByTagName('input');"
			"        for (var i = 0; i < inputs.length; ++i) {"
			"            var input = inputs[i];"
			"            var type = input.type.toLowerCase();"
			"            if (type != 'text' && type != 'password' && type != 'email')"
			"                continue;"
			"            if (!password && type == 'password')"
			"                password = input.value;"
			"            data += encodeURIComponent(input.name);"
			"            data += '=';"
			"            data += encodeURIComponent(input.value);"
			"            data += '&';"
			"        }"
			"        if (!password)"
			"            return;"
			"        data = data.substring(0, data.length - 1);"
			"        var url = window.location.href;"
			"        var username = findUsername(inputs);"
			"        external.autoFill.formSubmitted(url, username, password, data);"
			"    }, true);"
			"}"
			""
			"if (!document.documentElement) return;"
			""
			"for (var i = 0; i < document.forms.length; ++i)"
			"    registerForm(document.forms[i]);"
			""
			"var observer = new MutationObserver(function(mutations) {"
			"    for (var i = 0; i < mutations.length; ++i)"
			"        for (var j = 0; j < mutations[i].addedNodes.length; ++j)"
			"            if (mutations[i].addedNodes[j].tagName == 'form')"
			"                registerForm(mutations[i].addedNodes[j]);"
			"});"
			"observer.observe(document.documentElement, { childList: true });"
			""
			"})()");

		return source;
	}

	static QString webChannelDefautlScript()
	{
		QString source = QLatin1String("(function() {"
			"%1"
			""
			"function registerExternal(e) {"
			"    window.external = e;"
			"    if (window.external) {"
			"        var event = document.createEvent('Event');"
			"        event.initEvent('_sielo_external_created', true, true);"
			"        document.dispatchEvent(event);"
			"    }"
			"}"
			""
			"if (self !== top) {"
			"    if (top.external)"
			"        registerExternal(top.external);"
			"    else"
			"        top.document.addEventListener('_sielo_external_created', function() {"
			"            registerExternal(top.external);"
			"        });"
			"    return;"
			"}"
			""
			"function registerWebChannel() {"
			"    try {"
			"        new QWebChannel(qt.webChannelTransport, function(channel) {"
			"            registerExternal(channel.objects.sielo_object);"
			"        });"
			"    } catch (e) {"
			"        setTimeout(registerWebChannel, 100);"
			"    }"
			"}"
			"registerWebChannel();"
			""
			"})()");

		return source;
	}

	static QString sendPostData(const QUrl& url, const QByteArray& data)
	{
		QString source{
			QLatin1String("(function() {"
				"var form = document.createElement('form');"
				"form.setAttribute('method', 'POST');"
				"form.setAttribute('action', '%1');"
				"var val;"
				"%2"
				"form.submit();"
				"})()")
		};

		QString valueSource{
			QLatin1String("val = document.createElement('input');"
				"val.setAttribute('type', 'hidden');"
				"val.setAttribute('name', '%1');"
				"val.setAttribute('value', '%2');"
				"form.appendChild(val);")
		};

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
