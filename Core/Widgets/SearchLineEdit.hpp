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

#ifndef SIELO_BROWSER_SEARCHLINEEDIT_HPP
#define SIELO_BROWSER_SEARCHLINEEDIT_HPP

#include <QWidget>
#include <QLineEdit>

#include <QSize>
#include <QVariant>

#include <QFocusEvent>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QInputMethodEvent>

namespace Sn {
class ClearButton;

class SearchButton;

class SearchLineEdit : public QWidget {
Q_OBJECT
	Q_PROPERTY(QString inactiveText
					   READ
					   inactiveText
					   WRITE
					   setInactiveText)


public:
	SearchLineEdit(QWidget* parent = nullptr);
	~SearchLineEdit();

	QLineEdit* lineEdit() const { return m_lineEdit; }

	QString inactiveText() const { return m_inactiveText; }
	void setInactiveText(const QString& text);

	QMenu* menu() const;
	void setMenu(QMenu* menu);

	QSize sizeHint();
	QVariant inputMethodeQuery(Qt::InputMethodQuery property) const;

signals:
	void textChanged(const QString& text);

protected:
	void focusInEvent(QFocusEvent* event);
	void focusOutEvent(QFocusEvent* event);
	void keyPressEvent(QKeyEvent* event);
	void paintEvent(QPaintEvent* event);
	void resizeEvent(QResizeEvent* event);
	void inputMethodEvent(QInputMethodEvent* event);
	bool event(QEvent* event);

private:
	void updateGeometries();
	void initStyleOption(QStyleOptionFrame* option) const;

	SearchButton* m_searchButton{nullptr};
	QLineEdit* m_lineEdit{nullptr};
	ClearButton* m_clearButton{nullptr};

	QString m_inactiveText{};
};
}


#endif //SIELO_BROWSER_SEARCHLINEEDIT_HPP
