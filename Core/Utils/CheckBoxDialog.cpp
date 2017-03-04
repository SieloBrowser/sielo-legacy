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

#include "Utils/CheckBoxDialog.hpp"

namespace Sn {

CheckBoxDialog::CheckBoxDialog(const QDialogButtonBox::StandardButtons& buttons, QWidget* parent) :
	QDialog(parent,
			Qt::MSWindowsFixedSizeDialogHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint
			| Qt::WindowCloseButtonHint)
{
	setupUi();

	m_buttonBox->setStandardButtons(buttons);
}

void CheckBoxDialog::setIcon(const QIcon& icon)
{
	m_iconLabel->setPixmap(icon.pixmap(48, 48));
	m_iconLabel->setFixedWidth(48);
}

void CheckBoxDialog::setText(const QString& text)
{
	m_textLabel->setText(text);
}

void CheckBoxDialog::setCheckBoxText(const QString& text)
{
	m_checkBox->setText(text);
}

bool CheckBoxDialog::isChecked() const
{
	return m_checkBox->isChecked();
}

void CheckBoxDialog::setDefaultCheckState(Qt::CheckState state)
{
	m_checkBox->setChecked(state == Qt::Checked);
}

int CheckBoxDialog::exec()
{
	m_buttonBox->setFocus();
	setMaximumSize(size());

	return QDialog::exec();
}

void CheckBoxDialog::setupUi()
{
	m_layout = new QVBoxLayout(this);
	m_textLayout = new QHBoxLayout();
	m_buttonLayout = new QHBoxLayout();

	m_iconLabel = new QLabel(this);
	m_textLabel = new QLabel(this);

	m_textLayout->addWidget(m_iconLabel);
	m_textLayout->addWidget(m_textLabel);

	m_buttonBox = new QDialogButtonBox(this);
	m_checkBox = new QCheckBox(this);
	m_spacerItem = new QSpacerItem(10, 10, QSizePolicy::Expanding);

	m_buttonLayout->addItem(m_spacerItem);
	m_buttonLayout->addWidget(m_checkBox);
	m_buttonLayout->addWidget(m_buttonBox);

	m_layout->addLayout(m_textLayout);
	m_layout->addLayout(m_buttonLayout);
	setLayout(m_layout);
}

}
