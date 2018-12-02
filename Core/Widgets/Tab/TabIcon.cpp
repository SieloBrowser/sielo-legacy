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

#include "Widgets/Tab/TabIcon.hpp"

#include <QPainter>
#include <QRect>

#include "Web/WebPage.hpp"
#include "Web/WebView.hpp"
#include "Web/Tab/WebTab.hpp"
#include "Web/Tab/TabbedWebView.hpp"

namespace Sn {

static const int ANIMATION_INTERVAL = 25;

TabIcon::Data* TabIcon::s_data = Q_NULLPTR;

TabIcon::TabIcon(QWidget* parent) :
	QWidget(parent),
	m_tab(nullptr),
	m_currentFrame(0),
	m_animationRunning(false),
	m_audioIconDisplayed(false)
{
	setObjectName("tab-icon");
	if (!s_data) {
		s_data = new TabIcon::Data;

		s_data->animationPixmap = QIcon(QLatin1String(":icons/tabs/loading.png")).pixmap(512, 16);
		
		if (s_data->animationPixmap.height() != 0)
			s_data->framesCount = s_data->animationPixmap.width() / s_data->animationPixmap.height();
		else
			s_data->framesCount = 1;

		s_data->audioPlayingPixmap =
			Application::getAppIcon("audioplaying", "tabs");
		s_data->audioMutedPixmap =
			Application::getAppIcon("audiomuted", "tabs");
	}

	m_updateTimer = new QTimer(this);
	m_updateTimer->setInterval(ANIMATION_INTERVAL);

	m_hideTimer = new QTimer(this);
	m_hideTimer->setInterval(250);

	connect(m_updateTimer, &QTimer::timeout, this, &TabIcon::updateAnimationFrame);
	connect(m_hideTimer, &QTimer::timeout, this, &TabIcon::hide);

	resize(16, 16);
}

void TabIcon::setWebTab(WebTab* tab)
{
	m_tab = tab;

	connect(m_tab->webView(), &TabbedWebView::loadStarted, this, &TabIcon::showLoadingAnimation);
	connect(m_tab->webView(), &TabbedWebView::loadFinished, this, &TabIcon::hideLoadingAnimation);
	connect(m_tab->webView(), &TabbedWebView::iconChanged, this, &TabIcon::updateIcon);
	connect(m_tab->webView()->page(), &WebPage::recentlyAudibleChanged, this, &TabIcon::updateAudioIcon);

	updateIcon();
}

void TabIcon::updateIcon()
{
	m_sitePixmap = m_tab->icon(false).pixmap(16);

	if (m_sitePixmap.isNull())
		m_hideTimer->start();
	else
		showNormal();

	update();
}

void TabIcon::showLoadingAnimation()
{
	m_currentFrame = 0;

	updateAnimationFrame();
	show();
}

void TabIcon::hideLoadingAnimation()
{
	m_animationRunning = false;

	m_updateTimer->stop();
	updateIcon();
}

void TabIcon::updateAudioIcon(bool recentlyAudible)
{
	if (m_tab->isMuted() || (!m_tab->isMuted() && recentlyAudible)) {
		setToolTip(m_tab->isMuted() ? tr("Unmut Tab") : tr("Mute tab"));
		m_audioIconDisplayed = true;
		show();
	}
	else {
		setToolTip(QString());
		m_audioIconDisplayed = false;
		hide();
	}

	update();
}

void TabIcon::updateAnimationFrame()
{
	if (!m_animationRunning) {
		m_updateTimer->start();
		m_animationRunning = true;
	}

	update();
	m_currentFrame = (m_currentFrame + 1) % s_data->framesCount;
}

void TabIcon::show()
{
	if (!shouldBeVisible())
		return;

	m_hideTimer->stop();

	if (isVisible())
		return;

	setFixedSize(16, 16);
	emit resized();

	QWidget::show();
}

void TabIcon::hide()
{
	if (shouldBeVisible() || isHidden())
		return;

	setFixedSize(0, 0);
	emit resized();

	QWidget::hide();
}

bool TabIcon::shouldBeVisible() const
{
	return (!m_sitePixmap.isNull() || m_animationRunning || m_audioIconDisplayed);
}

void TabIcon::paintEvent(QPaintEvent* event)
{
	Q_UNUSED(event);

	QPainter painter{this};

	const int size{16};
	const int pixmapSize{qRound(size * s_data->animationPixmap.devicePixelRatio())};

	QRect r = rect();
	r.setX((r.width() - size) / 2);
	r.setY((r.height() - size) / 2);
	r.setWidth(size);
	r.setHeight(size);

	if (m_animationRunning && !m_tab->application())
		painter
			.drawPixmap(r, s_data->animationPixmap, QRect(m_currentFrame * pixmapSize, 0, pixmapSize, pixmapSize));
	else if (m_audioIconDisplayed)
		painter.drawPixmap(r,
						   m_tab->isMuted() ? s_data->audioMutedPixmap.pixmap(16) : s_data->audioPlayingPixmap
							   .pixmap(16));
	else
		painter.drawPixmap(r, m_sitePixmap);

}

void TabIcon::mousePressEvent(QMouseEvent* event)
{
	if (m_audioIconDisplayed && event->button() == Qt::LeftButton)
		m_tab->toggleMuted();
	else
		QWidget::mousePressEvent(event);
}

}
