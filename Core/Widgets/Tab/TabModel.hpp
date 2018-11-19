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
#ifndef SIELOBROWSER_TABMODEL_HPP
#define SIELOBROWSER_TABMODEL_HPP

#include "SharedDefines.hpp"

#include <QAbstractListModel>

#include <QMimeData>

#include <QPointer>


namespace Sn
{
class WebTab;

class TabWidget;

class SIELO_SHAREDLIB TabModelMimeData: public QMimeData {
	Q_OBJECT

public:
	TabModelMimeData();
	~TabModelMimeData() = default;

	WebTab* tab() const { return m_tab; }
	void setTab(WebTab* tab);

	bool hasFormat(const QString& format) const override;
	QStringList formats() const override;

	static QString mimeType();

private:
	QPointer<WebTab> m_tab{};
};

class SIELO_SHAREDLIB TabModel: public QAbstractListModel {
//	Q_OBJECT
//
//public:
//	enum Roles {
//		WebTabRole = Qt::UserRole + 1,
//		TitleRole = Qt::UserRole + 2,
//		IconRole = Qt::UserRole + 3,
//		PinnedRole = Qt::UserRole + 4,
//		RestoredRole = Qt::UserRole + 5,
//		CurrentTabRole = Qt::UserRole + 6,
//		LoadingRole = Qt::UserRole + 7,
//		AudioPlayingRole = Qt::UserRole + 8,
//		AudioMutedRole = Qt::UserRole + 9,
//		BackgroundActivityRole = Qt::UserRole + 10
//	};
//
//	TabModel(TabWidget* tabWidget, QObject* parent = nullptr);
//
//	QModelIndex tabIndex(WebTab* tab) const;
//	WebTab* tab(const QModelIndex& index) const;
//
//	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
//	Qt::ItemFlags flags(const QModelIndex& index) const override;
//	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
//
//	Qt::DropActions supportedDropActions() const override;
//	QStringList mimeTypes() const override;
//	QMimeData* mimeData(const QModelIndexList& indexes) const override;
//	bool canDropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) const override;
//	bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) override;
//
//private:
//	void tabInserted(int index);
//	void tabRemoved(int index);
//	void tabMoved(int from, int to);
//
//	TabWidget* m_tabWidget;
//	QVector<WebTab*> m_tabs;
};
}

#endif //SIELOBROWSER_TABMODEL_HPP