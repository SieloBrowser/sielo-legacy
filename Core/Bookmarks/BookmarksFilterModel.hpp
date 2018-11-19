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
#ifndef SIELOBROWSER_BOOKMARKSFILTERMODEL_HPP
#define SIELOBROWSER_BOOKMARKSFILTERMODEL_HPP

#include "SharedDefines.hpp"

#include <QSortFilterProxyModel>

#include <QTimer>

namespace Sn
{
class SIELO_SHAREDLIB BookmarksFilterModel: public QSortFilterProxyModel {
Q_OBJECT

public:
	BookmarksFilterModel(QAbstractItemModel* parent);
	~BookmarksFilterModel();

public slots:
	void setFilterFixedString(const QString& pattern);

protected:
	bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const;

private slots:
	void startFiltering();

private:
	QString m_pattern{};
	QTimer* m_filterTimer{};
};
}

#endif //SIELOBROWSER_BOOKMARKSFILTERMODEL_HPP
