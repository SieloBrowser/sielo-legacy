/***********************************************************************************
** MIT License                                                                    **
**                                                                                **
** Copyright (c) 2017 Victor DENIS (victordenis01@gmail.com)                      **
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

#include "Widgets/Preferences/AdBlockPage.hpp"

#include <QSettings>

#include "AdBlock/Manager.hpp"

#include "Application.hpp"

namespace Sn {

AdBlockPage::AdBlockPage(QWidget* parent) :
	QWidget(parent)
{
	m_layout = new QGridLayout(this);

	m_warning = new QLabel(tr(
		"<html><head/><body><p><span style=\" font-weight:600;\">Warning! This is an experimental AdBlock!</span></p></body></html>"),
						   this);
	m_warning->setAlignment(Qt::AlignCenter);

	m_adBlockIcon = new QLabel(tr("AdBlock icon"), this);
	m_adBlockIcon->setPixmap(Application::getAppIcon("adblock", "preferences").pixmap(32, 32));

	m_enableAdBlock = new QCheckBox(tr("Enable Ad block"), this);

	QSizePolicy sizePolicy{QSizePolicy::Expanding, QSizePolicy::Minimum};

	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(m_enableAdBlock->sizePolicy().hasHeightForWidth());

	m_enableAdBlock->setSizePolicy(sizePolicy);

	m_spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

	m_layout->addWidget(m_warning, 0, 0, 1, 2);
	m_layout->addWidget(m_adBlockIcon, 1, 0, 1, 1);
	m_layout->addWidget(m_enableAdBlock, 1, 1, 1, 1);
	m_layout->addItem(m_spacer, 2, 0, 1, 1);

	loadSettings();
}

AdBlockPage::~AdBlockPage()
{
	// Empty
}

void AdBlockPage::loadSettings()
{
	QSettings settings{};

	settings.beginGroup("AdBlock-Settings");

	m_enableAdBlock->setChecked(settings.value(QLatin1String("enabled"), false).toBool());

	settings.endGroup();
}

void AdBlockPage::save()
{
	QSettings settings{};

	settings.beginGroup("AdBlock-Settings");

	if (ADB::Manager::instance()->isEnabled() != m_enableAdBlock->isChecked()) {
		bool enabled{m_enableAdBlock->isChecked()};

		ADB::Manager::instance()->setEnabled(enabled);

		settings.setValue(QLatin1String("enabled"), enabled);

	}

	settings.endGroup();
}

}