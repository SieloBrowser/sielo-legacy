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
#ifndef SIELOBROWSER_LineEdit_HPP
#define SIELOBROWSER_LineEdit_HPP

#include "SharedDefines.hpp"

#include <QWidget>
#include <QLineEdit>

#include <QHBoxLayout>
#include <QTextLayout>

#include <QAction>

#include <QList>
#include <QEvent>

#include <array>

namespace Sn
{
class SIELO_SHAREDLIB SideWidget: public QWidget {
Q_OBJECT
public:
	SideWidget(QWidget* parent = nullptr);

signals:

	void sizeHintChanged();

protected:
	bool event(QEvent* event);
};

class SIELO_SHAREDLIB LineEdit: public QLineEdit {
Q_OBJECT

public:
	enum WidgetPosition {
		LeftSide,
		RightSide
	};

	enum EditAction {
		Undo = 0,
		Redo = 1,
		Cut = 2,
		Copy = 3,
		Paste = 4,
		PasteAndGo = 5,
		Delete = 6,
		ClearAll = 7,
		SelectAll = 8
	};

	LineEdit(QWidget* parent = nullptr);

	void addWidget(QWidget* widget, WidgetPosition position);
	void removeWidget(QWidget* widget);
	void setWidgetSpacing(int spacing);
	int widgetSpacing() const;
	int leftMargin() const { return m_leftMargin; }

	void setTextFormat(const QList<QTextLayout::FormatRange>& format);
	void clearTextFormat();

	int minHeight() const { return m_minHeight; }
	void setMinHeight(int height);

	QSize sizeHint() const;
	QAction *editAction(EditAction action) const;

public slots:
	void setLeftMargin(int margin);
	void updateTextMargins();

protected:
	void focusInEvent(QFocusEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseDoubleClickEvent(QMouseEvent* event);
	void resizeEvent(QResizeEvent* event) override;
	bool event(QEvent* event);

	QMenu *createContextMenu();

private slots:
	void updateActions();
	void updatePasteActions();
	void slotDelete();

private:
	SideWidget* m_leftWidget{nullptr};
	SideWidget* m_rightWidget{nullptr};
	QHBoxLayout* m_leftLayout{nullptr};
	QHBoxLayout* m_rightLayout{nullptr};
	QHBoxLayout* mainLayout{nullptr};
	
	std::array<QAction*, 9> m_editActions;

	int m_minHeight{0};
	int m_leftMargin{-1};
	bool m_ignoreMousePress{false};
};
}

#endif //SIELOBROWSER_LineEdit_HPP
