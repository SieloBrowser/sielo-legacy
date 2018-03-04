/***********************************************************************************
** MIT License                                                                    **
**                                                                                **
** Copyright (c) 2017 Victor DENIS (victordenis01@gmail.com)                      **
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
#ifndef SIELOBROWSER_TABSTACKEDWIDGET_HPP
#define SIELOBROWSER_TABSTACKEDWIDGET_HPP

#include <QWidget>
#include <QStackedWidget>
#include <QVBoxLayout>

#include <QEvent>
#include <QKeyEvent>

namespace Sn {
class ComboTabBar;

class NavigationToolBar;

class TabStackedWidget: public QWidget {
Q_OBJECT

public:
	TabStackedWidget(QWidget* parent = nullptr);
	~TabStackedWidget();

	ComboTabBar* tabBar() { return m_comboTabBar; }
	void setTabBar(ComboTabBar* tab);

	void setNavigationToolBar(NavigationToolBar* navigationToolBar);

	bool documentMode() const;
	void setDocumentMode(bool enable);

	int addTab(QWidget* widget, const QString& label, bool pinned = false);
	int insertTab(int index, QWidget* widget, const QString& label, bool pinned = false);

	QString tabText(int index) const;
	void setTabText(int index, const QString& label);

	QString tabToolTip(int index) const;
	void setTabToolTip(int index, const QString& tip);

	int pinUnPinTab(int index, const QString& title = QString());

	void removeTab(int index);

	int currentIndex() const;
	int indexOf(QWidget* widget) const;
	int count() const;
	QWidget* currentWidget() const;
	QWidget* widget(int index) const;

signals:
	void currentChanged(int index);
	void tabCloseRequested(int index);
	void pinStateChanged(int index, bool pinned);

public slots:
	void setCurrentIndex(int index);
	void setCurrentWidget(QWidget* widget);
	void setUpLayout();

protected:
	bool eventFilter(QObject* obj, QEvent* event);
	void keyPressEvent(QKeyEvent* event);

private slots:
	void showTab(int index);
	void tabWasMoved(int from, int to);
	void tabWasRemoved(int index);

private:
	bool validIndex(int index) const;
	void selectTabOnRemove();

	QStackedWidget* m_stack{nullptr};
	QVBoxLayout* m_layout{nullptr};

	ComboTabBar* m_comboTabBar{nullptr};

	bool m_dirtyTabBar{false};
	int m_currentIndex{-1};
	int m_previousIndex{-1};

};

}

#endif //SIELOBROWSER_TABSTACKEDWIDGET_HPP
