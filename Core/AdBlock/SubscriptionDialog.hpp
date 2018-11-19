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
#ifndef SIELO_BROWSER_ADBSUBSCRIPTIONDIALOG_HPP
#define SIELO_BROWSER_ADBSUBSCRIPTIONDIALOG_HPP

#include "SharedDefines.hpp"

#include <QWidget>
#include <QDialog>

#include <QVector>

#include <QFormLayout>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QDialogButtonBox>

namespace Sn {
namespace ADB {

class SIELO_SHAREDLIB SubscriptionDialog: public QDialog {
Q_OBJECT

public:
	SubscriptionDialog(QWidget* parent);

	QString title() const;
	QString address() const;

private slots:
	void indexChanged(int index);

private:
	struct ADBSubscription {
		QString title{};
		QString url{};

		ADBSubscription() {}
		ADBSubscription(const QString& t, const QString& u) :
			title(t),
			url(u) {}
	};

	void setupUI();

	QFormLayout* m_layout{nullptr};

	QLabel* m_desc{nullptr};
	QComboBox* m_knownSubscriptionBox{nullptr};
	QLabel* m_titleDesc{nullptr};
	QLineEdit* m_title{nullptr};
	QLabel* m_addressDesc{nullptr};
	QLineEdit* m_address{nullptr};
	QDialogButtonBox* m_buttonBox{nullptr};

	QVector<ADBSubscription> m_knownSubscriptions;
};

}
}

#endif //SIELO_BROWSER_ADBSUBSCRIPTIONDIALOG_HPP
