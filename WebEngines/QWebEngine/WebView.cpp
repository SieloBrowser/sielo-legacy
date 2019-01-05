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

#include "WebView.hpp"

#include <QTimer>

#include <QStackedLayout>
#include <QApplication>

#include "WebPage.hpp"

namespace Engine {
WebView::WebView(QWidget* parent) :
	QWebEngineView(parent)
{
	installEventFilter(this);

	QApplication::instance()->installEventFilter(this);

	if (parentWidget())
		parentWidget()->installEventFilter(this);

	// Hack to find widget that receives input events
	QStackedLayout *l = qobject_cast<QStackedLayout*>(layout());
	connect(l, &QStackedLayout::currentChanged, this, [this]()
	{
		QTimer::singleShot(0, this, [this]()
		{
			m_child = focusProxy();
			if (!m_child) {
				qCritical() << "Focus proxy is null!";
				return;
			}

			m_child->installEventFilter(this);
		});
	});
}

bool WebView::eventFilter(QObject* watched, QEvent* event)
{
	if (watched->parent() == this) {
		std::string test = "";
		switch (event->type()) {
		case QEvent::Wheel:
			newWheelEvent(dynamic_cast<QWheelEvent*>(event));
			break;
		case QEvent::MouseButtonPress:
			test = watched->metaObject()->className();
			newMousePressEvent(dynamic_cast<QMouseEvent*>(event));
			break;
		case QEvent::MouseButtonRelease:
			newMouseReleaseEvent(dynamic_cast<QMouseEvent*>(event));
			break;
		case QEvent::MouseMove:
			newMouseMoveEvent(dynamic_cast<QMouseEvent*>(event));
			break;
		case QEvent::FocusIn:
			emit focusChanged(true);
			break;
		case QEvent::FocusOut:
			emit focusChanged(false);
			break;
		default:
			break;
		}
	}

	if (watched == parentWidget()) {
		switch (event->type()) {
		case QEvent::KeyPress:
			newKeyPressEvent(dynamic_cast<QKeyEvent*>(event));
			break;
		case QEvent::KeyRelease:
			newKeyReleaseEvent(dynamic_cast<QKeyEvent*>(event));
		default:
			break;
		}
	}

	if (watched == this) {
		switch (event->type()) {
		case QEvent::KeyPress:
		case QEvent::KeyRelease:
		case QEvent::MouseButtonPress:
		case QEvent::MouseButtonRelease:
		case QEvent::MouseMove:
		case QEvent::Wheel:
			return true;

		case QEvent::Hide:
			if (isFullScreen()) {
				triggerPageAction(QWebEnginePage::ExitFullScreen);
			}
			break;

		default:
			break;
		}
	}

	const bool res = QWebEngineView::eventFilter(watched, event);

	if (watched->parent() == this) {
		switch (event->type()) {
		case QEvent::FocusIn:
		case QEvent::FocusOut:
			emit focusChanged(hasFocus());
			break;

		default:
			break;
		}
	}

	return res;
}

WebHistory* WebView::history() const
{
	return new WebHistory(QWebEngineView::history());
}

QWidget* WebView::inputWidget() const
{
	if (m_child)
		return m_child;
	else
		return const_cast<WebView*>(this);
}
}
