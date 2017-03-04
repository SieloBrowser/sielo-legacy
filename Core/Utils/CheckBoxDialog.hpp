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

#ifndef CORE_CHECKBOXDIALOG_HPP
#define CORE_CHECKBOXDIALOG_HPP
#pragma once

#include <QDialog>
#include <QDialogButtonBox>

#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QLabel>
#include <QCheckBox>
#include <QSpacerItem>

namespace Sn {

class CheckBoxDialog: public QDialog {
Q_OBJECT
public:
	CheckBoxDialog(const QDialogButtonBox::StandardButtons& buttons, QWidget* parent = nullptr);

	void setIcon(const QIcon& icon);

	void setText(const QString& text);
	void setCheckBoxText(const QString& text);

	bool isChecked() const;
	void setDefaultCheckState(Qt::CheckState state);

public slots:
	int exec();

private:
	void setupUi();

	QVBoxLayout* m_layout{nullptr};
	QHBoxLayout* m_textLayout{nullptr};
	QHBoxLayout* m_buttonLayout{nullptr};

	QLabel* m_iconLabel{nullptr};
	QLabel* m_textLabel{nullptr};

	QDialogButtonBox* m_buttonBox{nullptr};
	QCheckBox* m_checkBox{nullptr};
	QSpacerItem* m_spacerItem{nullptr};
};

}

#endif // CORE_CHECKBOXDIALOG_HPP
