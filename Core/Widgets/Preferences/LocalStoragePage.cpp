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

#include "LocalStoragePage.hpp"

#include <QFileDialog>
#include <QMessageBox>

#include "Utils/Settings.hpp"

#include "Application.hpp"

namespace Sn {

LocalStoragePage::LocalStoragePage(QWidget* parent) :
	QWidget(parent)
{
	setupUI();
	loadSettings();

	connect(m_networkStorageCheck, &QCheckBox::stateChanged, this, &LocalStoragePage::allowNetworkStorageChanged);
	connect(m_cacheSlider, &QSlider::valueChanged, this, &LocalStoragePage::cacheValueChanged);
	connect(m_changeStorageLocationButton,
			&QPushButton::clicked,
			this,
			&LocalStoragePage::changeStorageLocationClicked);

	connect(m_savingHistoryCheck, &QCheckBox::stateChanged, this, &LocalStoragePage::allowSavingHistoryChanged);
	connect(m_localHTML5StorageCheck, &QCheckBox::stateChanged, this, &LocalStoragePage::allowHTML5StorageChanged);
	connect(m_deleteHTML5Button, &QPushButton::clicked, this, &LocalStoragePage::deleteHTML5StorageClicked);
}

LocalStoragePage::~LocalStoragePage()
{
	// Empty
}

void LocalStoragePage::loadSettings()
{
	Settings settings{};

	settings.beginGroup("Web-Settings");

	m_networkStorageCheck->setChecked(settings.value("allowLocalCache", true).toBool());
	allowNetworkStorageChanged(m_networkStorageCheck->isChecked());

	m_cacheSlider->setValue(settings.value("localCacheSize", 100).toInt());
	m_MBLabel->setText(settings.value("localCacheSize", 50).toString() + " MB");
	m_storageLocationEdit
		->setText(settings.value("cachePath", QWebEngineProfile::defaultProfile()->cachePath()).toString());

	m_localHTML5StorageCheck->setEnabled(false);
	allowHTML5StorageChanged(false);

	m_savingHistoryCheck->setChecked(settings.value("allowHistory", true).toBool());
	m_deleteHistoryOnCloseCheck->setChecked(settings.value("deleteHistoryOnClose", false).toBool());
	allowSavingHistoryChanged(m_savingHistoryCheck->isChecked());
}

void LocalStoragePage::save()
{
	Settings settings{};

	settings.beginGroup("Web-Settings");

	settings.setValue("allowLocalCache", m_networkStorageCheck->isChecked());
	settings.setValue("localCacheSize", m_cacheSlider->value());
	settings.setValue("cachePath", m_storageLocationEdit->text());

	settings.setValue("allowHistory", m_savingHistoryCheck->isChecked());
	settings.setValue("deleteHistoryOnClose", m_deleteHistoryOnCloseCheck->isChecked());
}

void LocalStoragePage::allowNetworkStorageChanged(bool allow)
{
	m_maxLabel->setEnabled(allow);
	m_MBLabel->setEnabled(allow);
	m_cacheSlider->setEnabled(allow);
	m_storageLabel->setEnabled(allow);
	m_storageLocationEdit->setEnabled(allow);
	m_changeStorageLocationButton->setEnabled(allow);
}

void LocalStoragePage::allowSavingHistoryChanged(bool allow)
{
	m_deleteHistoryOnCloseCheck->setEnabled(allow);
}

void LocalStoragePage::allowHTML5StorageChanged(bool allow)
{
	m_deleteHTML5Check->setEnabled(allow);
	m_deleteHTML5Button->setEnabled(allow);
}

void LocalStoragePage::cacheValueChanged(int value)
{
	m_MBLabel->setText(QString::number(value) + " MB");
}

void LocalStoragePage::changeStorageLocationClicked()
{
	QString storageFolder{QFileDialog::getExistingDirectory(this, QString(), m_storageLocationEdit->text())};

	if (storageFolder.isEmpty())
		return;

	m_storageLocationEdit->setText(storageFolder);
}

void LocalStoragePage::deleteHTML5StorageClicked()
{
	QMessageBox::critical(this, tr("Error"), tr("Not work for now..."));
}

void LocalStoragePage::setupUI()
{
	m_layout = new QVBoxLayout(this);

	m_networkStorageLayout = new QGridLayout();
	m_MBLayout = new QHBoxLayout();
	m_storageLocationLayout = new QHBoxLayout();

	m_networkStorageCheck = new QCheckBox(tr("Allow storing network cache on disk"), this);

	m_maxLabel = new QLabel(tr("Maximum "), this);
	m_MBLabel = new QLabel(tr("100 MB"), this);

	m_cacheSlider = new QSlider(Qt::Horizontal, this);
	m_cacheSlider->setMinimum(20);
	m_cacheSlider->setMaximum(200);
	m_cacheSlider->setValue(100);

	m_storageLabel = new QLabel(tr("Store cache in:"), this);

	m_storageLocationEdit = new QLineEdit(this);
	QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(m_cacheSlider->sizePolicy().hasHeightForWidth());
	m_storageLocationEdit->setSizePolicy(sizePolicy);
	m_changeStorageLocationButton = new QPushButton("...", this);

	m_spacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

	m_MBLayout->addWidget(m_maxLabel);
	m_MBLayout->addWidget(m_MBLabel);

	m_storageLocationLayout->addWidget(m_storageLocationEdit);
	m_storageLocationLayout->addWidget(m_changeStorageLocationButton);

	m_networkStorageLayout->addWidget(m_networkStorageCheck, 0, 0, 1, 2);
	m_networkStorageLayout->addLayout(m_MBLayout, 1, 0, 1, 1);
	m_networkStorageLayout->addWidget(m_cacheSlider, 1, 1, 1, 1);
	m_networkStorageLayout->addWidget(m_storageLabel, 2, 0, 1, 1);
	m_networkStorageLayout->addLayout(m_storageLocationLayout, 2, 1, 1, 1);

	m_otherStorageLayout = new QGridLayout();

	m_savingHistoryCheck = new QCheckBox(tr("Allow saving history"), this);
	m_deleteHistoryOnCloseCheck = new QCheckBox(tr("Delete history on close"), this);
	m_localHTML5StorageCheck = new QCheckBox(tr("Allow local storage of HTML5 web content"), this);
	m_deleteHTML5Check = new QCheckBox(tr("Delete locally stored HTML5 web content on close"), this);

	m_deleteHTML5Button = new QPushButton(tr("Delete now"), this);

	m_leftSpacer = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Preferred);
	m_rightSpacer = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Preferred);

	m_otherStorageLayout->addWidget(m_savingHistoryCheck, 0, 0, 1, 3);
	m_otherStorageLayout->addWidget(m_deleteHistoryOnCloseCheck, 1, 1, 1, 1);
	m_otherStorageLayout->addWidget(m_localHTML5StorageCheck, 2, 0, 1, 3);
	m_otherStorageLayout->addWidget(m_deleteHTML5Check, 3, 1, 1, 1);
	m_otherStorageLayout->addWidget(m_deleteHTML5Button, 3, 2, 1, 1);
	m_otherStorageLayout->addItem(m_leftSpacer, 1, 0, 1, 1);
	m_otherStorageLayout->addItem(m_rightSpacer, 1, 3, 1, 1);

	m_layout->addLayout(m_networkStorageLayout);
	m_layout->addLayout(m_otherStorageLayout);
	m_layout->addItem(m_spacer);
}

}