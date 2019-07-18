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
#ifndef SIELO_BROWSER_EXTERNALJSOBJECT_HPP
#define SIELO_BROWSER_EXTERNALJSOBJECT_HPP

#include "SharedDefines.hpp"

#include "Web/WebPage.hpp"

#include <QObject>

#include <QWebChannel>

namespace Sn {
class AutoFillJsObject;

class SIELO_SHAREDLIB ExternalJsObject: public QObject {
Q_OBJECT
	Q_PROPERTY(QObject* autoFill
				   READ
					   autoFill
				   CONSTANT)

public:
	explicit ExternalJsObject(WebPage* page);

	WebPage* page() const;

	static void setupWebChannel(QWebChannel* webChannel, WebPage* page);

	static void registerExtraObject(const QString& id, QObject* object);
	static void unregisterExtraObject(const QString& id);

private:
	QObject* autoFill() const;

	WebPage* m_page;
	AutoFillJsObject* m_autoFill;
};
}
#endif //SIELO_BROWSER_EXTERNALJSOBJECT_HPP
