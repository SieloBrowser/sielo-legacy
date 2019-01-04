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
#ifndef CORE_WEBHITTESTRESULT_HPP
#define CORE_WEBHITTESTRESULT_HPP

#include "SharedDefines.hpp"

#include <QUrl>
#include <QRect>
#include <QString>
#include <QVariantMap>

#include <QWebEngine/ContextMenuData.hpp>
#include <QWebEngine/WebPage.hpp>

namespace Sn {

class WebPage;

class SIELO_SHAREDLIB WebHitTestResult {
public:
	WebHitTestResult(const WebPage* page, const QPoint& pos);

	void updateWithContextMenuData(const Engine::ContextMenuData& data);

	QUrl baseUrl() const;
	QString alternateText() const;
	QRect boundingRect() const;
	QUrl imageUrl() const;
	bool isContentEditable() const;
	bool isContentSelected() const;
	bool isNull() const;
	QString linkTitle() const;
	QUrl linkUrl() const;
	QUrl mediaUrl() const;
	bool mediaPaused() const;
	bool mediaMuted() const;
	QPoint pos() const;
	QPointF viewportPos() const;
	QString tagName() const;

private:
	bool m_isNull{true};
	QUrl m_baseUrl{QUrl()};
	QString m_alternateText{QString()};
	QRect m_boundingRect{QRect()};
	QUrl m_imageUrl{QUrl()};
	bool m_isContentEditable{false};
	bool m_isContentSelected{false};
	QString m_linkTitle{QString()};
	QUrl m_linkUrl{QUrl()};
	QUrl m_mediaUrl{QUrl()};
	bool m_mediaPaused{false};
	bool m_mediaMuted{false};
	QPoint m_pos{QPoint()};
	QPointF m_viewportPos{QPointF()};
	QString m_tagName{QString()};
};

}

#endif // CORE_WEBHITTESTRESULT_HPP
