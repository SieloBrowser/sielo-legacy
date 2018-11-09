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

#include "JSOptions.hpp"

#include "Utils/Settings.hpp"

namespace Sn {

JSOptions::JSOptions(QWidget* parent) :
	QDialog(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);

	QIcon icon = windowIcon();
	Qt::WindowFlags flags = windowFlags();
	Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;

	flags = flags & (~helpFlag);
	setWindowFlags(flags);
	setWindowIcon(icon);

	setupUI();

	Settings settings{};

	settings.beginGroup("Web-Settings");

	m_jscanOpenWindow->setChecked(settings.value("allowJavaScriptOpenWindow", false).toBool());
	m_jscanChangeSize->setChecked(settings.value("allowJavaScriptChangeGeometry", true).toBool());
	m_jscanAccessClipboard->setChecked(settings.value("allowJavaScriptAccessClipboard", false).toBool());

	settings.endGroup();

	connect(m_buttonBox, &QDialogButtonBox::accepted, this, &JSOptions::accept);
	connect(m_buttonBox, &QDialogButtonBox::rejected, this, &JSOptions::reject);
}

JSOptions::~JSOptions()
{
	// Empty
}

void JSOptions::accept()
{
	Settings settings{};

	settings.beginGroup("Web-Settings");

	settings.setValue("allowJavaScriptOpenWindow", m_jscanOpenWindow->isChecked());
	settings.setValue("allowJavaScriptChangeGeometry", m_jscanChangeSize->isChecked());
	settings.setValue("allowJavaScriptAccessClipboard", m_jscanAccessClipboard->isChecked());

	settings.endGroup();

	QDialog::close();
}

void JSOptions::setupUI()
{
	resize(214, 193);

	m_layout = new QVBoxLayout(this);
	m_optionsLayout = new QVBoxLayout();

	m_desc = new QLabel(this);
	m_desc->setText(tr("JavaScript Options"));

	m_jscanOpenWindow = new QCheckBox(this);
	m_jscanOpenWindow->setText(tr("Open popup windows"));

	m_jscanChangeSize = new QCheckBox(this);
	m_jscanChangeSize->setText(tr("Change window size"));

	m_jscanAccessClipboard = new QCheckBox(this);
	m_jscanAccessClipboard->setText(tr("Access clipboard"));

	m_spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

	m_optionsLayout->addWidget(m_jscanChangeSize);
	m_optionsLayout->addWidget(m_jscanAccessClipboard);
	m_optionsLayout->addWidget(m_jscanOpenWindow);
	m_optionsLayout->addItem(m_spacer);

	m_buttonBox = new QDialogButtonBox(this);
	m_buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);

	m_layout->addWidget(m_desc);
	m_layout->addLayout(m_optionsLayout);
	m_layout->addWidget(m_buttonBox);
}

}