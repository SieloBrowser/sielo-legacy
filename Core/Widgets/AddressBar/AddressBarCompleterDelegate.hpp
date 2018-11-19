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
#ifndef SIELOBROWSER_ADDRESSBARCOMPLETERDELEGATE_HPP
#define SIELOBROWSER_ADDRESSBARCOMPLETERDELEGATE_HPP

#include "SharedDefines.hpp"

#include <QStyledItemDelegate>

#include <QTextLayout>

namespace Sn
{
class SIELO_SHAREDLIB AddressBarCompleterDelegate: public QStyledItemDelegate {
	Q_OBJECT
public:
	AddressBarCompleterDelegate(QObject* parent = nullptr);

	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index);

	void setShowSwitchToTab(bool enable);
	void setOriginalText(const QString& originalText);

	bool isUrlOrDomain(const QString& text) const;
	QSizeF viewItemTextLayout(QTextLayout &textLayout, int lineWidth) const;
private:
	// Waiting for additional settings
	bool drawSwitchToTab() const { return m_drawSwitchToTab; };

	int viewItemDrawText(QPainter* painter,
	                     const QStyleOptionViewItem* option,
	                     const QRect& rect,
	                     const QString& text, const QColor& color,
	                     const QString& searchText = QString()) const;

	int m_rowHeight{0};
	int m_padding{0};
	bool m_drawSwitchToTab{true};
	QString m_originalText{};
};
}

#endif //SIELOBROWSER_ADDRESSBARCOMPLETERDELEGATE_HPP
