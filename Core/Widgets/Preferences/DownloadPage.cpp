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

#include "Widgets/Preferences/DownloadPage.hpp"

#include <QStandardPaths>

#include <QFileDialog>

#include "Utils/Settings.hpp"

namespace Sn {

DownloadPage::DownloadPage(QWidget* parent) :
	QWidget(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);

	setupUI();

	loadSettings();

	connect(m_radioAlwaysAsk, &QRadioButton::toggled, this, &DownloadPage::radioAlwaysAskChanged);

	connect(m_choosePath, &QPushButton::clicked, this, &DownloadPage::choosePath);
}

DownloadPage::~DownloadPage()
{
	// Empty
}

void DownloadPage::loadSettings()
{
	QString defaultLocation{QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)};

	Settings settings{};

	settings.beginGroup("Download-Settings");

	m_path->setText(settings.value(QLatin1String("downloadDirectory"), defaultLocation).toString());

	if (settings.value(QLatin1String("alwaysAsk"), false).toBool()) {
		m_radioAlwaysAsk->setChecked(true);
		m_radioCustomPath->setChecked(false);

		m_path->setEnabled(false);
		m_choosePath->setEnabled(false);
	}
	else {
		m_radioAlwaysAsk->setChecked(false);
		m_radioCustomPath->setChecked(true);

		m_path->setEnabled(true);
		m_choosePath->setEnabled(true);
	}

	settings.endGroup();
}

void DownloadPage::save()
{
	Settings settings{};

	settings.beginGroup("Download-Settings");

	settings.setValue(QLatin1String("downloadDirectory"), m_path->text());
	settings.setValue(QLatin1String("alwaysAsk"), m_radioAlwaysAsk->isChecked());

	settings.endGroup();
}

void DownloadPage::choosePath()
{
	QString newPath{QFileDialog::getExistingDirectory(this, QString(), m_path->text())};

	if (!newPath.isEmpty())
		m_path->setText(newPath);
}

void DownloadPage::radioAlwaysAskChanged(bool enabled)
{
	if (enabled) {
		m_path->setEnabled(false);
		m_choosePath->setEnabled(false);
	}
	else {
		m_path->setEnabled(true);
		m_choosePath->setEnabled(true);
	}
}

void DownloadPage::setupUI()
{
	m_layout = new QVBoxLayout(this);

	m_layoutPath = new QHBoxLayout();
	m_layoutPath->setContentsMargins(32, -1, -1, -1);

	m_radioAlwaysAsk = new QRadioButton(tr("Always aks the download location"), this);

	m_radioCustomPath = new QRadioButton(tr("Use custom download location"), this);
	m_radioCustomPath->setChecked(true);

	m_path = new QLineEdit(this);
	m_path->setPlaceholderText(tr("Downloads location"));

	m_choosePath = new QPushButton(tr("..."), this);

	m_spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

	m_layoutPath->addWidget(m_path);
	m_layoutPath->addWidget(m_choosePath);

	m_layout->addWidget(m_radioAlwaysAsk);
	m_layout->addWidget(m_radioCustomPath);
	m_layout->addLayout(m_layoutPath);
	m_layout->addItem(m_spacer);
}

}