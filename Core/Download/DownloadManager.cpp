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

#include "Download/DownloadManager.hpp"

#include <QHeaderView>
#include <QStyle>

#include <QMetaEnum>

#include "Utils/AutoSaver.hpp"
#include "Utils/Settings.hpp"

#include "View/TableView.hpp"

#include "Widgets/EllipseLabel.hpp"

#include "Download/DownloadWidget.hpp"
#include "Download/DownloadModel.hpp"

#include "Application.hpp"

namespace Sn {

DownloadManager::DownloadManager(QWidget* parent) :
	QDialog(parent),
	m_saver(new AutoSaver(this)),
	m_removePolicy(Never)
{
	QIcon icon = windowIcon();
	Qt::WindowFlags flags = windowFlags();
	Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;

	flags = flags & (~helpFlag);
	setWindowFlags(flags);
	setWindowIcon(icon);

	setupUI();

	m_view->setShowGrid(false);
	m_view->verticalHeader()->hide();
	m_view->horizontalHeader()->hide();
	m_view->setAlternatingRowColors(true);
	m_view->horizontalHeader()->setStretchLastSection(true);

	m_model = new DownloadModel(this);

	m_view->setModel(m_model);

	connect(m_buttonCleanUp, &QPushButton::clicked, this, &DownloadManager::cleanup);

	load();
}

DownloadManager::~DownloadManager()
{
	m_saver->changeOccurred();
	m_saver->saveIfNeccessary();

	if (m_iconProvider)
		delete m_iconProvider;
}

int DownloadManager::activeDownloads() const
{
	int count{0};

	for (int i{0}; i < m_downloads.count(); ++i) {
		if (m_downloads[i]->m_buttonStop->isEnabled())
			++count;
	}

	return count;
}

void DownloadManager::setRemovePolicy(RemovePolicy policy)
{
	if (policy == m_removePolicy)
		return;

	m_removePolicy = policy;
	m_saver->changeOccurred();
}

void DownloadManager::downlaod(Engine::DownloadItem* download)
{
	DownloadWidget* widget{new DownloadWidget(download, this)};

	addItem(widget);
}

void DownloadManager::cleanup()
{
	if (m_downloads.isEmpty())
		return;

	m_model->removeRows(0, m_downloads.count());
	updateItemCount();

	if (m_downloads.isEmpty() && m_iconProvider) {
		delete m_iconProvider;
		m_iconProvider = nullptr;
	}

	m_saver->changeOccurred();
}

void DownloadManager::save() const
{
	Settings settings{};

	settings.beginGroup(QLatin1String("Download-Settings"));

	QMetaEnum removePolicyEnum{staticMetaObject.enumerator(staticMetaObject.indexOfEnumerator("RemovePolicy"))};

	settings
		.setValue(QLatin1String("removeDownloadPolicy"), QLatin1String(removePolicyEnum.valueToKey(m_removePolicy)));
	settings.setValue(QLatin1String("size"), size());

	if (m_removePolicy == Exit)
		return;

	for (int i{0}; i < m_downloads.count(); ++i) {
		QString key{QString(QLatin1String("download_%1_")).arg(i)};

		settings.setValue(key + QLatin1String("url"), m_downloads[i]->m_url);
		settings.setValue(key + QLatin1String("location"), m_downloads[i]->m_file.filePath());
		settings.setValue(key + QLatin1String("done"), m_downloads[i]->downloadedSuccessfully());
	}

	int i{m_downloads.count()};
	QString key{QString(QLatin1String("download_%1_")).arg(i)};

	while (settings.contains(key + QLatin1String("url"))) {
		settings.remove(key + QLatin1String("url"));
		settings.remove(key + QLatin1String("location"));
		settings.remove(key + QLatin1String("done"));

		key = QString(QLatin1String("download_%1_")).arg(++i);
	}
}

void DownloadManager::updateRow()
{
	DownloadWidget* widget = qobject_cast<DownloadWidget*>(sender());
	int row{m_downloads.indexOf(widget)};

	if (row == -1)
		return;

	if (!m_iconProvider)
		m_iconProvider = new QFileIconProvider();

	QIcon icon{m_iconProvider->icon(widget->m_file)};

	if (icon.isNull())
		icon = style()->standardIcon(QStyle::SP_FileIcon);

	widget->m_fileIcon->setPixmap(icon.pixmap(48, 48));
	m_view->setRowHeight(row, widget->minimumSizeHint().height());

	bool remove{false};

	if (!widget->downloading() && Application::instance()->privateBrowsing())
		remove = true;

	if (widget->downloadedSuccessfully() && removePolicy() == DownloadManager::SuccessFullDownlad)
		remove = true;

	if (remove)
		m_model->removeRow(row);

	m_buttonCleanUp->setEnabled(m_downloads.count() - activeDownloads() > 0);
}

void DownloadManager::addItem(DownloadWidget* item)
{
	connect(item, &DownloadWidget::statusChanged, this, &DownloadManager::updateRow);

	int row{m_downloads.count()};

	m_model->beginInsertRows(QModelIndex(), row, row);

	m_downloads.append(item);

	m_model->endInsertRows();

	updateItemCount();

	if (row == 0)
		show();

	m_view->setIndexWidget(m_model->index(row, 0), item);

	QIcon icon{style()->standardIcon(QStyle::SP_FileIcon)};

	item->m_fileIcon->setPixmap(icon.pixmap(48, 48));

	m_view->setRowHeight(row, item->sizeHint().height());
}

void DownloadManager::updateItemCount()
{
	int count{m_downloads.count()};

	m_itemCount->setText(count == 1 ? tr("1 Download") : tr("%1 Downloads").arg(count));
}

void DownloadManager::setupUI()
{
	resize(332, 252);
	setWindowTitle(tr("Downloads"));

	m_layout = new QGridLayout(this);
	m_layout->setSpacing(0);
	m_layout->setContentsMargins(0, 0, 0, 0);

	m_layoutButtons = new QHBoxLayout();

	m_view = new TableView(this);

	m_buttonCleanUp = new QPushButton(tr("Clean up"), this);
	m_buttonCleanUp->setEnabled(false);

	m_buttonSpacer1 = new QSpacerItem(58, 24, QSizePolicy::Expanding, QSizePolicy::Minimum);

	m_itemCount = new QLabel(tr("0 Items"), this);

	m_buttonSpacer2 = new QSpacerItem(148, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

	m_layoutButtons->addWidget(m_buttonCleanUp);
	m_layoutButtons->addItem(m_buttonSpacer1);

	m_layout->addWidget(m_view, 0, 0, 1, 3);
	m_layout->addLayout(m_layoutButtons, 1, 0, 1, 1);
	m_layout->addWidget(m_itemCount, 1, 1, 1, 1);
	m_layout->addItem(m_buttonSpacer2, 1, 2, 1, 1);
}

void DownloadManager::load()
{
	Settings settings{};

	settings.beginGroup(QLatin1String("Download-Settings"));

	QSize size{settings.value(QLatin1String("size")).toSize()};

	if (size.isValid())
		resize(size);

	QByteArray value{settings.value(QLatin1String("removeDownloadsPolicy"), QLatin1String("Never")).toByteArray()};
	QMetaEnum removePolicyEnum{staticMetaObject.enumerator(staticMetaObject.indexOfEnumerator("RemovePolicy"))};

	m_removePolicy = removePolicyEnum.keysToValue(value) == -1 ? Never : static_cast<RemovePolicy>(removePolicyEnum
		.keysToValue(value));

	int i{0};
	QString key{QString(QLatin1String("download_%1_")).arg(i)};

	while (settings.contains(key + QLatin1String("url"))) {
		QUrl url{settings.value(key + QLatin1String("url")).toUrl()};
		QString fileName{settings.value(key + QLatin1String("location")).toString()};
		bool done{settings.value(key + QLatin1String("done"), true).toBool()};

		if (done && !url.isEmpty() && !fileName.isEmpty()) {
			DownloadWidget* widget{new DownloadWidget(nullptr, this)};

			widget->m_file.setFile(fileName);
			widget->m_fileNameLabel->setText(widget->m_file.fileName());
			widget->m_url = url;
			widget->m_buttonStop->setVisible(false);
			widget->m_buttonStop->setEnabled(false);
			widget->m_buttonOpen->setVisible(true);
			widget->m_buttonOpen->setEnabled(true);
			widget->m_progress->setMaximum(100);
			widget->m_progress->setValue(widget->m_progress->maximum());
			widget->m_progress->setTextVisible(true);
			widget->m_progress->setFormat(tr("Download completed"));

			addItem(widget);
		}

		key = QString(QLatin1String("download_%1_")).arg(++i);
	}

	m_buttonCleanUp->setEnabled(m_downloads.count() - activeDownloads() > 0);
}

}