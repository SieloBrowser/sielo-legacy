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

#include "Download/DownloadModel.hpp"

#include <QList>

#include "Utils/AutoSaver.hpp"

#include "Widgets/EllipseLabel.hpp"

#include "Download/DownloadManager.hpp"
#include "Download/DownloadWidget.hpp"

namespace Sn {

DownloadModel::DownloadModel(DownloadManager* downloadManager, QObject* parent) :
	QAbstractListModel(parent),
	m_downloadManager(downloadManager)
{
	// Empty
}

QVariant DownloadModel::data(const QModelIndex& index, int role) const
{
	if (index.row() < 0 || index.row() >= rowCount(index.parent()))
		return QVariant();

	if (role == Qt::ToolTipRole) {
		if (!m_downloadManager->m_downloads[index.row()]->downloadedSuccessfully())
			return m_downloadManager->m_downloads[index.row()]->downloadInfoLabel()->text();
	}

	return QVariant();
}

int DownloadModel::rowCount(const QModelIndex& parent) const
{
	return (parent.isValid()) ? 0 : m_downloadManager->m_downloads.count();
}

bool DownloadModel::removeRows(int row, int count, const QModelIndex& parent)
{
	if (parent.isValid())
		return false;

	int lastRow{row + count - 1};

	for (int i{lastRow}; i >= row; --i) {
		if (m_downloadManager->m_downloads[i]->downloadedSuccessfully()) {
			beginRemoveRows(parent, i, i);

			m_downloadManager->m_downloads.takeAt(i)->deleteLater();

			endRemoveRows();
		}
	}

	m_downloadManager->m_saver->changeOccurred();
	return true;
}

}