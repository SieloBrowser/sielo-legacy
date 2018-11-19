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
#ifndef CORE_CERTIFICATEINFORMATION_HPP
#define CORE_CERTIFICATEINFORMATION_HPP

#include "SharedDefines.hpp"

#include <QSslCertificate>

#include <QWidget>
#include <QAction>
#include <QButtonGroup>
#include <QLabel>

#include <QHeaderView>

#include <QFormLayout>

namespace Sn {

class SIELO_SHAREDLIB CertificateInformation: public QWidget {
public:
	explicit CertificateInformation(const QSslCertificate& certificate, QWidget* parent = nullptr);
	~CertificateInformation();

	static QString showCertificateInformation(const QString& string);
	static QString showCertificateInformation(const QStringList& stringList);

	static QString clearCertificateSpecialSymbols(const QString& string);
	static QString clearCertificateSpecialSymbols(const QStringList& stringList);

	static QString certificateItemText(const QSslCertificate& certificate);

private:
	void setupUi();

	QFormLayout* m_formLayout;
	QLabel* m_lIssuedTo;
	QLabel* m_lCN1;
	QLabel* m_issuedToCN;
	QLabel* m_lO1;
	QLabel* m_issuedToO;
	QLabel* m_lOU1;
	QLabel* m_issuedToOU;
	QLabel* m_lSN;
	QLabel* m_issuedToSN;
	QLabel* m_lIssuedBy;
	QLabel* m_lCN2;
	QLabel* m_issuedByCN;
	QLabel* m_lO2;
	QLabel* m_issuedByO;
	QLabel* m_lOU2;
	QLabel* m_issuedByOU;
	QLabel* m_lValidity;
	QLabel* m_lIO;
	QLabel* m_validityIssuedOn;
	QLabel* m_lEN;
	QLabel* m_validityExpiresOn;

};
}

#endif //CORE_CERTIFICATEINFORMATION_HPP
