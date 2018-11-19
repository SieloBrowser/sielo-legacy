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
#ifndef SIELOBROWSER_TOOLBUTTON_HPP
#define SIELOBROWSER_TOOLBUTTON_HPP

#include "SharedDefines.hpp"

#include <QToolButton>
#include <QTimer>

#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QPaintEvent>

#include <QImage>

#include <QMenu>

namespace Sn {
class SIELO_SHAREDLIB ToolButton: public QToolButton {
Q_OBJECT

	Q_PROPERTY(QSize fixedsize
				   READ
					   size
				   WRITE
				   setFixedSize)
	Q_PROPERTY(int fixedwidth
				   READ
					   width
				   WRITE
				   setFixedWidth)
	Q_PROPERTY(int fixedheight
				   READ
					   height
				   WRITE
				   setFixedHeight)
	Q_PROPERTY(QImage multiIcon
				   READ
					   multiIcon
				   WRITE
				   setMultiIcon)
	Q_PROPERTY(QIcon icon
				   READ
					   icon
				   WRITE
				   setIcon)
	Q_PROPERTY(QString themeIcon
				   READ
					   themeIcon
				   WRITE
				   setThemeIcon)
	Q_PROPERTY(QIcon fallbackIcon
				   READ
					   fallbackIcon
				   WRITE
				   setFallbackIcon)

public:
	ToolButton(QWidget* parent = nullptr);

	QImage multiIcon() const { return m_multiIcon; }
	void setMultiIcon(const QImage& image);

	QString themeIcon() const { return m_themeIcon; }
	void setThemeIcon(const QString& icon);

	QIcon fallbackIcon() const { return icon(); }
	void setFallbackIcon(const QIcon& fallbackIcon);

	QIcon icon() const { return QToolButton::icon(); }
	void setIcon(const QIcon& icon);

	QMenu* menu() const { return m_menu; }
	void setMenu(QMenu* menu);

	bool showMenuInside() { return static_cast<bool >(m_options & ShowMenuInsideOption); }
	void setShowMenuInside(bool enable);

	bool toolbarButtonLook() const { return static_cast<bool>(m_options & ToolBarLookOption); }
	void setToolBarButtonLook(bool enable);

signals:
	void middleMouseClicked();
	void controlClicked();
	void doubleClicked();

	void aboutToShowMenu();
	void aboutToHideMenu();

protected:
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void mouseDoubleClickEvent(QMouseEvent* event);
	void contextMenuEvent(QContextMenuEvent* event);
	void paintEvent(QPaintEvent* event);

private slots:
	void menuAboutToHide();
	void showMenu();

private:
	QImage m_multiIcon{};
	QString m_themeIcon{};
	QTimer m_pressTimer{};
	QMenu* m_menu{nullptr};

	enum Options {
		MultiIconOption = 1,
		ShowMenuInsideOption = 2,
		ToolBarLookOption = 3
	};
	Q_DECLARE_FLAGS(OptionsFlags, Options)
	OptionsFlags m_options;
};
}
#endif //SIELOBROWSER_TOOLBUTTON_HPP
