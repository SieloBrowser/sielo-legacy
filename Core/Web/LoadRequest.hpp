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
#ifndef CORE_LOADREQUEST_HPP
#define CORE_LOADREQUEST_HPP

#include "SharedDefines.hpp"

#include <QUrl>
#include <QByteArray>

namespace Sn {

class SIELO_SHAREDLIB LoadRequest {
public:
	enum Operation {
		GetOp = 0,
		PostOp = 1
	};

	LoadRequest() :
		m_operation(GetOp) {}
	LoadRequest(const LoadRequest& other) :
		m_url(other.m_url),
		m_operation(other.m_operation),
		m_data(other.m_data) {}
	LoadRequest(const QUrl& url, Operation operation = GetOp, const QByteArray& data = QByteArray()) :
		m_url(url),
		m_operation(operation),
		m_data(data) {}

	LoadRequest& operator=(const LoadRequest& other)
	{
		m_url = other.m_url;
		m_operation = other.m_operation;
		m_data = other.m_data;
		return *this;
	}

	bool isValid() const { return m_url.isValid(); }
	bool isEmpty() const { return m_url.isEmpty(); }

	QUrl url() const { return m_url; }
	void setUrl(const QUrl& url) { m_url = url; }

	QString urlString() const { return QUrl::fromPercentEncoding(m_url.toEncoded()); }

	Operation operation() const { return m_operation; }
	void setOperation(Operation operation) { m_operation = operation; }

	QByteArray data() const { return m_data; }
	void setData(const QByteArray& data) { m_data = data; }

private:
	QUrl m_url{};
	Operation m_operation{};
	QByteArray m_data{};
};

} // namespace Sn
#endif // CORE_LOADREQUEST_HPP
