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

#include "NavigationControlDialog.hpp"

#include "Utils/Settings.hpp"

#include "Application.hpp"

namespace Sn
{
NavigationControlDialog::NavigationControlDialog(QWidget* parent) :
	QDialog(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setObjectName("dialog-navigation-control");
	setWindowTitle(tr("Navigation Controls"));
	setupUI();

	connect(m_floatingButton, &QPushButton::toggled, this, &NavigationControlDialog::buttonCheckChanged);
	connect(m_buttonBox, &QDialogButtonBox::accepted, this, &NavigationControlDialog::close);

	m_floatingButton->setChecked(true);
	m_toolbars->setChecked(false);
}

NavigationControlDialog::~NavigationControlDialog()
{
	// Empty
}

void NavigationControlDialog::save()
{
	Settings settings{};

	settings.setValue("Settings/useTopToolBar", m_toolbars->isChecked());

	Application::instance()->loadSettings();
}

void NavigationControlDialog::closeEvent(QCloseEvent* event)
{
	save();

	QDialog::closeEvent(event);
}

void NavigationControlDialog::buttonCheckChanged(bool checked)
{
	m_toolbars->setChecked(!checked);
	m_floatingButton->setChecked(checked);
}

void NavigationControlDialog::setupUI()
{
	resize(438, 383);

	m_layout = new QGridLayout(this);

	m_desc = new QLabel(
		tr("Which type of control do you prefer for your navigation? (this can always be changed in settings later.)"),
		this);

	m_floatingButtonImage = new QLabel(this);
	m_floatingButtonImage->setPixmap(Application::getAppIcon("floating-button", "preferences").pixmap(195, 148));
	m_toolbarsImage = new QLabel(this);
	m_toolbarsImage->setPixmap(Application::getAppIcon("toolbars", "preferences").pixmap(195, 148));

	m_floatingButton = new QPushButton(tr("Floating Button"), this);
	m_toolbars = new QPushButton(tr("Toolbars"), this);

	m_floatingButton->setCheckable(true);
	m_toolbars->setCheckable(true);

	m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, this);

	m_layout->addWidget(m_desc, 0, 0, 1, 2);
	m_layout->addWidget(m_floatingButtonImage, 1, 0, 1, 1);
	m_layout->addWidget(m_toolbarsImage, 1, 1, 1, 1);
	m_layout->addWidget(m_floatingButton, 2, 0, 1, 1);
	m_layout->addWidget(m_toolbars, 2, 1, 1, 1);
	m_layout->addWidget(m_buttonBox, 3, 0, 1, 2);
}
}
