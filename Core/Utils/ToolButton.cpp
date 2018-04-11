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

#include "Utils/ToolButton.hpp"

#include <QStyle>
#include <QStyleOptionToolButton>

#include <QPainter>

#include <QApplication>

namespace Sn {
ToolButton::ToolButton(QWidget* parent) :
	QToolButton(parent),
	m_menu(nullptr)
{
	setMinimumWidth(16);

	QStyleOptionToolButton option{};
	initStyleOption(&option);

	m_pressTimer.setSingleShot(true);
	m_pressTimer.setInterval(QApplication::style()->styleHint(QStyle::SH_ToolButton_PopupDelay, &option, this));

	connect(&m_pressTimer, &QTimer::timeout, this, &ToolButton::showMenu);
}

void ToolButton::setMultiIcon(const QImage& image)
{
	m_options |= MultiIconOption;
	m_multiIcon = image;
	setFixedSize(m_multiIcon.width(), m_multiIcon.height() / 4);

	update();
}

void ToolButton::setThemeIcon(const QString& icon)
{
	m_themeIcon = icon;
	setIcon(QIcon::fromTheme(m_themeIcon));
}

void ToolButton::setFallbackIcon(const QIcon& fallbackIcon)
{
	if (icon().isNull())
		setIcon(fallbackIcon);
}

void ToolButton::setIcon(const QIcon& icon)
{
	if (m_options & MultiIconOption)
		setFixedSize(sizeHint());

	m_options &= ~MultiIconOption;
	QToolButton::setIcon(icon);
}

void ToolButton::setMenu(QMenu* menu)
{
	Q_ASSERT(menu);

	if (menu && m_menu)
		disconnect(m_menu, &QMenu::aboutToHide, this, &ToolButton::menuAboutToHide);

	m_menu = menu;

	connect(m_menu, &QMenu::aboutToHide, this, &ToolButton::menuAboutToHide);
}

void ToolButton::setShowMenuInside(bool enable)
{
	if (enable)
		m_options |= ShowMenuInsideOption;
	else
		m_options &= ~ShowMenuInsideOption;
}

void ToolButton::setToolBarButtonLook(bool enable)
{
	if (enable) {
		m_options |= ToolBarLookOption;

		QStyleOption option{};
		option.initFrom(this);

		int size{style()->pixelMetric(QStyle::PM_ToolBarIconSize, &option, this)};
		setIconSize(QSize(size, size));
	}
	else
		m_options &= ~ToolBarLookOption;

	setProperty("toolbar-look", QVariant(enable));

	style()->unpolish(this);
	style()->polish(this);
}

void ToolButton::mousePressEvent(QMouseEvent* event)
{
	if (popupMode() == QToolButton::DelayedPopup)
		m_pressTimer.start();

	if (event->button() == Qt::LeftButton && menu() && popupMode() == QToolButton::InstantPopup) {
		setDown(true);
		showMenu();
	}
	else if (event->button() == Qt::RightButton && menu()) {
		setDown(true);
		showMenu();
	}
	else
		QToolButton::mousePressEvent(event);
}

void ToolButton::mouseReleaseEvent(QMouseEvent* event)
{
	m_pressTimer.stop();

	if (event->button() == Qt::MiddleButton && rect().contains(event->pos())) {
		emit middleMouseClicked();
		setDown(false);
	}
	else if (event->button() == Qt::LeftButton && rect().contains(event->pos())
			 && event->modifiers() == Qt::ControlModifier) {
		emit controlClicked();
		setDown(false);
	}
	else
		QToolButton::mouseReleaseEvent(event);
}

void ToolButton::mouseDoubleClickEvent(QMouseEvent* event)
{
	QToolButton::mouseDoubleClickEvent(event);

	m_pressTimer.stop();

	if (event->button() == Qt::LeftButton)
		emit doubleClicked();
}

void ToolButton::contextMenuEvent(QContextMenuEvent* event)
{
	if (menu())
		return;

	QToolButton::contextMenuEvent(event);
}

void ToolButton::paintEvent(QPaintEvent* event)
{
	if (!(m_options & MultiIconOption)) {
		QToolButton::paintEvent(event);
		return;
	}

	QPainter painter{};

	const int width{m_multiIcon.width()};
	const int heightOn4{m_multiIcon.height() / 4};

	if (!isEnabled())
		painter.drawImage(0, 0, m_multiIcon, 0, heightOn4 * 3, width, heightOn4);
	else if (isDown())
		painter.drawImage(0, 0, m_multiIcon, 0, heightOn4 * 2, width, heightOn4);
	else if (underMouse())
		painter.drawImage(0, 0, m_multiIcon, 0, heightOn4 * 1, width, heightOn4);
	else
		painter.drawImage(0, 0, m_multiIcon, 0, heightOn4 * 0, width, heightOn4);
}

void ToolButton::menuAboutToHide()
{
	setDown(false);
	emit aboutToHideMenu();
}

void ToolButton::showMenu()
{
	if (!m_menu || m_menu->isVisible())
		return;

	emit aboutToShowMenu();

	QPoint position{};

	if (m_options & ShowMenuInsideOption) {
		position = mapToGlobal(rect().bottomRight());

		if (QApplication::layoutDirection() == Qt::RightToLeft)
			position.setX(position.x() - rect().width());
		else
			position.setX(position.x() - m_menu->sizeHint().width());
	}
	else
		position = mapToGlobal(rect().bottomLeft());

	m_menu->popup(position);
}

}