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

#include "PasswordPage.hpp"

#include "Utils/Settings.hpp"

namespace Sn {

PasswordPage::PasswordPage(QWidget* parent) :
	QWidget(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);

	setupUI();
	loadSettings();

	connect(m_allowCheck, &QCheckBox::toggled, this, &PasswordPage::allowChanged);
}

PasswordPage::~PasswordPage()
{
	// Empty
}

void PasswordPage::loadSettings()
{
	Settings settings{};

	m_allowCheck->setChecked(settings.value("Settings/savePasswordsOnSites", true).toBool());
	allowChanged(m_allowCheck->isChecked());
}

void PasswordPage::save()
{
	Settings settings{};

	settings.setValue("Settings/savePasswordsOnSites", m_allowCheck->isChecked());
}

void PasswordPage::allowChanged(bool allow)
{
	m_autoFillManager->setVisible(allow);
}

void PasswordPage::setupUI()
{
	m_autoFillFrame = new QFrame(this);

	m_layout = new QGridLayout(this);
	m_autoFillLayout = new QHBoxLayout(m_autoFillFrame);

	m_desc = new QLabel(tr("<b>AutoFill options</b>"), this);
	m_allowCheck = new QCheckBox(tr("Allow saving passwords from sites"));
	m_autoFillManager = new AutoFillManager(this);

	QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(m_autoFillFrame->sizePolicy().hasHeightForWidth());
	m_autoFillFrame->setSizePolicy(sizePolicy);

	m_leftSpacer = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Preferred);
	m_rightSpacer = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Preferred);

	m_autoFillLayout->addWidget(m_autoFillManager);

	m_layout->addWidget(m_desc, 0, 0, 1, 3);
	m_layout->addItem(m_leftSpacer, 1, 0, 1, 1);
	m_layout->addWidget(m_allowCheck, 1, 1, 1, 1);
	m_layout->addItem(m_rightSpacer, 1, 2, 1, 1);
	m_layout->addWidget(m_autoFillFrame, 2, 0, 1, 3);
}

}