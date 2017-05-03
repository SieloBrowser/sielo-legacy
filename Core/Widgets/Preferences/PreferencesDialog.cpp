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

#include "Widgets/Preferences/PreferencesDialog.hpp"


#include "Widgets/Preferences/AdBlockPage.hpp"

namespace Sn {

PreferencesDialog::PreferencesDialog(QWidget* parent) :
	QDialog(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);

	setupUI();

	connect(m_buttonBox, &QDialogButtonBox::clicked, this, &PreferencesDialog::buttonClicked);
}

PreferencesDialog::~PreferencesDialog()
{
	// Empty
}

void PreferencesDialog::saveSettings()
{
	m_pageAdBlock->save();
}

void PreferencesDialog::buttonClicked(QAbstractButton* button)
{
	switch (m_buttonBox->buttonRole(button)) {
	case QDialogButtonBox::ApplyRole:
		saveSettings();
		break;
	case QDialogButtonBox::AcceptRole:
		saveSettings();
		close();
		break;
	case QDialogButtonBox::RejectRole:
		close();
		break;
	default:
		break;
	}
}

void PreferencesDialog::setupUI()
{
	m_layout = new QVBoxLayout(this);

	m_layoutButton = new QHBoxLayout();

	m_pages = new QTabWidget(this);

	m_pageAdBlock = new AdBlockPage(m_pages);

	m_pages->addTab(m_pageAdBlock, tr("AdBlock"));

	m_buttonSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

	m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Apply | QDialogButtonBox::Cancel | QDialogButtonBox::Ok, this);

	m_layoutButton->addItem(m_buttonSpacer);
	m_layoutButton->addWidget(m_buttonBox);

	m_layout->addWidget(m_pages);
	m_layout->addLayout(m_layoutButton);

}

}