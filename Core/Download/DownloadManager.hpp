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
#ifndef SIELOBROWSER_DOWNLOADMANAGER_HPP
#define SIELOBROWSER_DOWNLOADMANAGER_HPP

#include "SharedDefines.hpp"

#include <QFileIconProvider>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QSpacerItem>

#include <QWebEngine/DownloadItem.hpp>

namespace Sn {
class DownloadWidget;
class DownloadModel;

class AutoSaver;

class TableView;

class SIELO_SHAREDLIB DownloadManager: public QDialog {
Q_OBJECT
	Q_PROPERTY(RemovePolicy
				   removePolicy
				   READ
				   removePolicy
				   WRITE
				   setRemovePolicy)

public:
	enum RemovePolicy {
		Never,
		Exit,
		SuccessFullDownlad
	};
	Q_ENUM(RemovePolicy)

	DownloadManager(QWidget* parent = nullptr);
	~DownloadManager();

	int activeDownloads() const;

	RemovePolicy removePolicy() const { return m_removePolicy; }
	void setRemovePolicy(RemovePolicy policy);

public slots:
	void downlaod(Engine::DownloadItem* download);
	void cleanup();

private slots:
	void save() const;
	void updateRow();

private:
	void setupUI();

	void addItem(DownloadWidget* item);
	void updateItemCount();
	void load();

	AutoSaver* m_saver{nullptr};

	DownloadModel* m_model{nullptr};
	QFileIconProvider* m_iconProvider{};
	QList<DownloadWidget*> m_downloads;
	RemovePolicy m_removePolicy;

	QGridLayout* m_layout{nullptr};
	QHBoxLayout* m_layoutButtons{nullptr};

	TableView* m_view{nullptr};
	QPushButton* m_buttonCleanUp{nullptr};
	QSpacerItem* m_buttonSpacer1{nullptr};
	QLabel* m_itemCount{nullptr};
	QSpacerItem* m_buttonSpacer2{nullptr};

	friend class DownloadModel;
};
}

#endif //SIELOBROWSER_DOWNLOADMANAGER_HPP
