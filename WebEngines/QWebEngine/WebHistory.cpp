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

#include "WebHistory.hpp"

namespace Engine {

WebHistory::WebHistory(QWebEngineHistory* history) :
		QObject(),
		m_history(history)
{
	// Empty
}

bool WebHistory::canGoBack() const
{
	return m_history->canGoBack();
}

bool WebHistory::canGoForward() const
{
	return m_history->canGoForward();
}

QUrl WebHistory::backUrl() const
{
	return m_history->backItem().url();
}

QUrl WebHistory::forwardUrl() const
{
	return m_history->forwardItem().url();
}

int WebHistory::itemCount() const
{
	return m_history->items().count();
}

void WebHistory::back()
{
	m_history->back();
}

void WebHistory::forward()
{
	m_history->forward();
}

QDataStream& operator<<(QDataStream& stream, const WebHistory& item)
{
	stream << item.m_history;

	return stream;
}

QDataStream& operator>>(QDataStream& stream, WebHistory& item)
{
	stream >> *item.m_history;

	return stream;
}

}