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

#include "CertificateInformation.hpp"

namespace Sn {

QString CertificateInformation::showCertificateInformation(const QString& string)
{
	if (string.isEmpty())
		return QObject::tr("<not set in certificate>");
	else
		return clearCertificateSpecialSymbols(string);
}

QString CertificateInformation::showCertificateInformation(const QStringList& stringList)
{
	if (stringList.isEmpty())
		return QString();

	return showCertificateInformation(stringList[0]);
}

QString CertificateInformation::clearCertificateSpecialSymbols(const QString& string)
{
	QString n = string.toHtmlEscaped();

	if (!n.contains(QLatin1String("\\"))) {
		return n;
	}

	n.replace(QLatin1String("\\xC3\\x80"), QLatin1String("A"));
	n.replace(QLatin1String("\\xC3\\x81"), QLatin1String("A"));
	n.replace(QLatin1String("\\xC3\\x82"), QLatin1String("A"));
	n.replace(QLatin1String("\\xC3\\x83"), QLatin1String("A"));
	n.replace(QLatin1String("\\xC3\\x84"), QLatin1String("A"));
	n.replace(QLatin1String("\\xC3\\x85"), QLatin1String("A"));
	n.replace(QLatin1String("\\xC3\\x86"), QLatin1String("AE"));
	n.replace(QLatin1String("\\xC3\\x87"), QLatin1String("C"));
	n.replace(QLatin1String("\\xC3\\x88"), QLatin1String("E"));
	n.replace(QLatin1String("\\xC3\\x89"), QLatin1String("E"));
	n.replace(QLatin1String("\\xC3\\x8A"), QLatin1String("E"));
	n.replace(QLatin1String("\\xC3\\x8B"), QLatin1String("E"));
	n.replace(QLatin1String("\\xC3\\x8C"), QLatin1String("I"));
	n.replace(QLatin1String("\\xC3\\x8D"), QLatin1String("I"));
	n.replace(QLatin1String("\\xC3\\x8E"), QLatin1String("I"));
	n.replace(QLatin1String("\\xC3\\x8F"), QLatin1String("I"));
	n.replace(QLatin1String("\\xC3\\x90"), QLatin1String("D"));
	n.replace(QLatin1String("\\xC3\\x91"), QLatin1String("N"));
	n.replace(QLatin1String("\\xC3\\x92"), QLatin1String("O"));
	n.replace(QLatin1String("\\xC3\\x93"), QLatin1String("O"));
	n.replace(QLatin1String("\\xC3\\x94"), QLatin1String("O"));
	n.replace(QLatin1String("\\xC3\\x95"), QLatin1String("O"));
	n.replace(QLatin1String("\\xC3\\x96"), QLatin1String("O"));
	n.replace(QLatin1String("\\xC3\\x98"), QLatin1String("O"));
	n.replace(QLatin1String("\\xC3\\x99"), QLatin1String("U"));
	n.replace(QLatin1String("\\xC3\\x9A"), QLatin1String("U"));
	n.replace(QLatin1String("\\xC3\\x9B"), QLatin1String("U"));
	n.replace(QLatin1String("\\xC3\\x9C"), QLatin1String("U"));
	n.replace(QLatin1String("\\xC3\\x9D"), QLatin1String("Y"));
	n.replace(QLatin1String("\\xC3\\x9E"), QLatin1String("P"));
	n.replace(QLatin1String("\\xC3\\x9F"), QLatin1String("ss"));
	n.replace(QLatin1String("\\xC9\\x99"), QLatin1String("e"));
	n.replace(QLatin1String("\\xC3\\xA0"), QLatin1String("a"));
	n.replace(QLatin1String("\\xC3\\xA1"), QLatin1String("a"));
	n.replace(QLatin1String("\\xC3\\xA2"), QLatin1String("a"));
	n.replace(QLatin1String("\\xC3\\xA3"), QLatin1String("a"));
	n.replace(QLatin1String("\\xC3\\xA4"), QLatin1String("a"));
	n.replace(QLatin1String("\\xC3\\xA5"), QLatin1String("a"));
	n.replace(QLatin1String("\\xC3\\xA6"), QLatin1String("ae"));
	n.replace(QLatin1String("\\xC3\\xA7"), QLatin1String("c"));
	n.replace(QLatin1String("\\xC3\\xA8"), QLatin1String("e"));
	n.replace(QLatin1String("\\xC3\\xA9"), QLatin1String("e"));
	n.replace(QLatin1String("\\xC3\\xAA"), QLatin1String("e"));
	n.replace(QLatin1String("\\xC3\\xAB"), QLatin1String("e"));
	n.replace(QLatin1String("\\xC3\\xAC"), QLatin1String("i"));
	n.replace(QLatin1String("\\xC3\\xAD"), QLatin1String("i"));
	n.replace(QLatin1String("\\xC3\\xAE"), QLatin1String("i"));
	n.replace(QLatin1String("\\xC3\\xAF"), QLatin1String("i"));
	n.replace(QLatin1String("\\xC3\\xB0"), QLatin1String("o"));
	n.replace(QLatin1String("\\xC3\\xB1"), QLatin1String("n"));
	n.replace(QLatin1String("\\xC3\\xB2"), QLatin1String("o"));
	n.replace(QLatin1String("\\xC3\\xB3"), QLatin1String("o"));
	n.replace(QLatin1String("\\xC3\\xB4"), QLatin1String("o"));
	n.replace(QLatin1String("\\xC3\\xB5"), QLatin1String("o"));
	n.replace(QLatin1String("\\xC3\\xB6"), QLatin1String("o"));
	n.replace(QLatin1String("\\xC3\\xB8"), QLatin1String("o"));
	n.replace(QLatin1String("\\xC3\\xB9"), QLatin1String("u"));
	n.replace(QLatin1String("\\xC3\\xBA"), QLatin1String("u"));
	n.replace(QLatin1String("\\xC3\\xBB"), QLatin1String("u"));
	n.replace(QLatin1String("\\xC3\\xBC"), QLatin1String("u"));
	n.replace(QLatin1String("\\xC3\\xBD"), QLatin1String("y"));
	n.replace(QLatin1String("\\xC3\\xBE"), QLatin1String("p"));
	n.replace(QLatin1String("\\xC3\\xBF"), QLatin1String("y"));
	n.replace(QLatin1String("\\xC7\\xBF"), QLatin1String("o"));
	n.replace(QLatin1String("\\xC4\\x80"), QLatin1String("A"));
	n.replace(QLatin1String("\\xC4\\x81"), QLatin1String("a"));
	n.replace(QLatin1String("\\xC4\\x82"), QLatin1String("A"));
	n.replace(QLatin1String("\\xC4\\x83"), QLatin1String("a"));
	n.replace(QLatin1String("\\xC4\\x84"), QLatin1String("A"));
	n.replace(QLatin1String("\\xC4\\x85"), QLatin1String("a"));
	n.replace(QLatin1String("\\xC4\\x86"), QLatin1String("C"));
	n.replace(QLatin1String("\\xC4\\x87"), QLatin1String("c"));
	n.replace(QLatin1String("\\xC4\\x88"), QLatin1String("C"));
	n.replace(QLatin1String("\\xC4\\x89"), QLatin1String("c"));
	n.replace(QLatin1String("\\xC4\\x8A"), QLatin1String("C"));
	n.replace(QLatin1String("\\xC4\\x8B"), QLatin1String("c"));
	n.replace(QLatin1String("\\xC4\\x8C"), QLatin1String("C"));
	n.replace(QLatin1String("\\xC4\\x8D"), QLatin1String("c"));
	n.replace(QLatin1String("\\xC4\\x8E"), QLatin1String("D"));
	n.replace(QLatin1String("\\xC4\\x8F"), QLatin1String("d"));
	n.replace(QLatin1String("\\xC4\\x90"), QLatin1String("D"));
	n.replace(QLatin1String("\\xC4\\x91"), QLatin1String("d"));
	n.replace(QLatin1String("\\xC4\\x92"), QLatin1String("E"));
	n.replace(QLatin1String("\\xC4\\x93"), QLatin1String("e"));
	n.replace(QLatin1String("\\xC4\\x94"), QLatin1String("E"));
	n.replace(QLatin1String("\\xC4\\x95"), QLatin1String("e"));
	n.replace(QLatin1String("\\xC4\\x96"), QLatin1String("E"));
	n.replace(QLatin1String("\\xC4\\x97"), QLatin1String("e"));
	n.replace(QLatin1String("\\xC4\\x98"), QLatin1String("E"));
	n.replace(QLatin1String("\\xC4\\x99"), QLatin1String("e"));
	n.replace(QLatin1String("\\xC4\\x9A"), QLatin1String("E"));
	n.replace(QLatin1String("\\xC4\\x9B"), QLatin1String("e"));
	n.replace(QLatin1String("\\xC4\\x9C"), QLatin1String("G"));
	n.replace(QLatin1String("\\xC4\\x9D"), QLatin1String("g"));
	n.replace(QLatin1String("\\xC4\\x9E"), QLatin1String("G"));
	n.replace(QLatin1String("\\xC4\\x9F"), QLatin1String("g"));
	n.replace(QLatin1String("\\xC4\\xA0"), QLatin1String("G"));
	n.replace(QLatin1String("\\xC4\\xA1"), QLatin1String("g"));
	n.replace(QLatin1String("\\xC4\\xA2"), QLatin1String("G"));
	n.replace(QLatin1String("\\xC4\\xA3"), QLatin1String("g"));
	n.replace(QLatin1String("\\xC4\\xA4"), QLatin1String("H"));
	n.replace(QLatin1String("\\xC4\\xA5"), QLatin1String("h"));
	n.replace(QLatin1String("\\xC4\\xA6"), QLatin1String("H"));
	n.replace(QLatin1String("\\xC4\\xA7"), QLatin1String("h"));
	n.replace(QLatin1String("\\xC4\\xA8"), QLatin1String("I"));
	n.replace(QLatin1String("\\xC4\\xA9"), QLatin1String("i"));
	n.replace(QLatin1String("\\xC4\\xAA"), QLatin1String("I"));
	n.replace(QLatin1String("\\xC4\\xAB"), QLatin1String("i"));
	n.replace(QLatin1String("\\xC4\\xAC"), QLatin1String("I"));
	n.replace(QLatin1String("\\xC4\\xAD"), QLatin1String("i"));
	n.replace(QLatin1String("\\xC4\\xAE"), QLatin1String("I"));
	n.replace(QLatin1String("\\xC4\\xAF"), QLatin1String("i"));
	n.replace(QLatin1String("\\xC4\\xB0"), QLatin1String("I"));
	n.replace(QLatin1String("\\xC4\\xB1"), QLatin1String("i"));
	n.replace(QLatin1String("\\xC4\\xB2"), QLatin1String("IJ"));
	n.replace(QLatin1String("\\xC4\\xB3"), QLatin1String("ij"));
	n.replace(QLatin1String("\\xC4\\xB4"), QLatin1String("J"));
	n.replace(QLatin1String("\\xC4\\xB5"), QLatin1String("j"));
	n.replace(QLatin1String("\\xC4\\xB6"), QLatin1String("K"));
	n.replace(QLatin1String("\\xC4\\xB7"), QLatin1String("k"));
	n.replace(QLatin1String("\\xC4\\xB8"), QLatin1String("k"));
	n.replace(QLatin1String("\\xC4\\xB9"), QLatin1String("L"));
	n.replace(QLatin1String("\\xC4\\xBA"), QLatin1String("l"));
	n.replace(QLatin1String("\\xC4\\xBB"), QLatin1String("L"));
	n.replace(QLatin1String("\\xC4\\xBC"), QLatin1String("l"));
	n.replace(QLatin1String("\\xC4\\xBD"), QLatin1String("L"));
	n.replace(QLatin1String("\\xC4\\xBE"), QLatin1String("l"));
	n.replace(QLatin1String("\\xC4\\xBF"), QLatin1String("L"));
	n.replace(QLatin1String("\\xC5\\x80"), QLatin1String("l"));
	n.replace(QLatin1String("\\xC5\\x81"), QLatin1String("L"));
	n.replace(QLatin1String("\\xC5\\x82"), QLatin1String("l"));
	n.replace(QLatin1String("\\xC5\\x83"), QLatin1String("N"));
	n.replace(QLatin1String("\\xC5\\x84"), QLatin1String("n"));
	n.replace(QLatin1String("\\xC5\\x85"), QLatin1String("N"));
	n.replace(QLatin1String("\\xC5\\x86"), QLatin1String("n"));
	n.replace(QLatin1String("\\xC5\\x87"), QLatin1String("N"));
	n.replace(QLatin1String("\\xC5\\x88"), QLatin1String("n"));
	n.replace(QLatin1String("\\xC5\\x89"), QLatin1String("n"));
	n.replace(QLatin1String("\\xC5\\x8A"), QLatin1String("N"));
	n.replace(QLatin1String("\\xC5\\x8B"), QLatin1String("n"));
	n.replace(QLatin1String("\\xC5\\x8C"), QLatin1String("O"));
	n.replace(QLatin1String("\\xC5\\x8D"), QLatin1String("o"));
	n.replace(QLatin1String("\\xC5\\x8E"), QLatin1String("O"));
	n.replace(QLatin1String("\\xC5\\x8F"), QLatin1String("o"));
	n.replace(QLatin1String("\\xC5\\x90"), QLatin1String("O"));
	n.replace(QLatin1String("\\xC5\\x91"), QLatin1String("o"));
	n.replace(QLatin1String("\\xC5\\x92"), QLatin1String("CE"));
	n.replace(QLatin1String("\\xC5\\x93"), QLatin1String("ce"));
	n.replace(QLatin1String("\\xC5\\x94"), QLatin1String("R"));
	n.replace(QLatin1String("\\xC5\\x95"), QLatin1String("r"));
	n.replace(QLatin1String("\\xC5\\x96"), QLatin1String("R"));
	n.replace(QLatin1String("\\xC5\\x97"), QLatin1String("r"));
	n.replace(QLatin1String("\\xC5\\x98"), QLatin1String("R"));
	n.replace(QLatin1String("\\xC5\\x99"), QLatin1String("r"));
	n.replace(QLatin1String("\\xC5\\x9A"), QLatin1String("S"));
	n.replace(QLatin1String("\\xC5\\x9B"), QLatin1String("s"));
	n.replace(QLatin1String("\\xC5\\x9C"), QLatin1String("S"));
	n.replace(QLatin1String("\\xC5\\x9D"), QLatin1String("s"));
	n.replace(QLatin1String("\\xC5\\x9E"), QLatin1String("S"));
	n.replace(QLatin1String("\\xC5\\x9F"), QLatin1String("s"));
	n.replace(QLatin1String("\\xC5\\xA0"), QLatin1String("S"));
	n.replace(QLatin1String("\\xC5\\xA1"), QLatin1String("s"));
	n.replace(QLatin1String("\\xC5\\xA2"), QLatin1String("T"));
	n.replace(QLatin1String("\\xC5\\xA3"), QLatin1String("t"));
	n.replace(QLatin1String("\\xC5\\xA4"), QLatin1String("T"));
	n.replace(QLatin1String("\\xC5\\xA5"), QLatin1String("t"));
	n.replace(QLatin1String("\\xC5\\xA6"), QLatin1String("T"));
	n.replace(QLatin1String("\\xC5\\xA7"), QLatin1String("t"));
	n.replace(QLatin1String("\\xC5\\xA8"), QLatin1String("U"));
	n.replace(QLatin1String("\\xC5\\xA9"), QLatin1String("u"));
	n.replace(QLatin1String("\\xC5\\xAA"), QLatin1String("U"));
	n.replace(QLatin1String("\\xC5\\xAB"), QLatin1String("u"));
	n.replace(QLatin1String("\\xC5\\xAC"), QLatin1String("U"));
	n.replace(QLatin1String("\\xC5\\xAD"), QLatin1String("u"));
	n.replace(QLatin1String("\\xC5\\xAE"), QLatin1String("U"));
	n.replace(QLatin1String("\\xC5\\xAF"), QLatin1String("u"));
	n.replace(QLatin1String("\\xC5\\xB0"), QLatin1String("U"));
	n.replace(QLatin1String("\\xC5\\xB1"), QLatin1String("u"));
	n.replace(QLatin1String("\\xC5\\xB2"), QLatin1String("U"));
	n.replace(QLatin1String("\\xC5\\xB3"), QLatin1String("u"));
	n.replace(QLatin1String("\\xC5\\xB4"), QLatin1String("W"));
	n.replace(QLatin1String("\\xC5\\xB5"), QLatin1String("w"));
	n.replace(QLatin1String("\\xC5\\xB6"), QLatin1String("Y"));
	n.replace(QLatin1String("\\xC5\\xB7"), QLatin1String("y"));
	n.replace(QLatin1String("\\xC5\\xB8"), QLatin1String("Y"));
	n.replace(QLatin1String("\\xC5\\xB9"), QLatin1String("Z"));
	n.replace(QLatin1String("\\xC5\\xBA"), QLatin1String("z"));
	n.replace(QLatin1String("\\xC5\\xBB"), QLatin1String("Z"));
	n.replace(QLatin1String("\\xC5\\xBC"), QLatin1String("z"));
	n.replace(QLatin1String("\\xC5\\xBD"), QLatin1String("Z"));
	n.replace(QLatin1String("\\xC5\\xBE"), QLatin1String("z"));
	n.replace(QLatin1String("\\xC6\\x8F"), QLatin1String("E"));
	n.replace(QLatin1String("\\xC6\\xA0"), QLatin1String("O"));
	n.replace(QLatin1String("\\xC6\\xA1"), QLatin1String("o"));
	n.replace(QLatin1String("\\xC6\\xAF"), QLatin1String("U"));
	n.replace(QLatin1String("\\xC6\\xB0"), QLatin1String("u"));
	n.replace(QLatin1String("\\xC7\\x8D"), QLatin1String("A"));
	n.replace(QLatin1String("\\xC7\\x8E"), QLatin1String("a"));
	n.replace(QLatin1String("\\xC7\\x8F"), QLatin1String("I"));
	n.replace(QLatin1String("\\xC7\\x93"), QLatin1String("U"));
	n.replace(QLatin1String("\\xC7\\x90"), QLatin1String("i"));
	n.replace(QLatin1String("\\xC7\\x91"), QLatin1String("O"));
	n.replace(QLatin1String("\\xC7\\x92"), QLatin1String("o"));
	n.replace(QLatin1String("\\xC7\\x97"), QLatin1String("U"));
	n.replace(QLatin1String("\\xC7\\x94"), QLatin1String("u"));
	n.replace(QLatin1String("\\xC7\\x95"), QLatin1String("U"));
	n.replace(QLatin1String("\\xC7\\x96"), QLatin1String("u"));
	n.replace(QLatin1String("\\xC7\\x9B"), QLatin1String("U"));
	n.replace(QLatin1String("\\xC7\\x98"), QLatin1String("u"));
	n.replace(QLatin1String("\\xC7\\x99"), QLatin1String("U"));
	n.replace(QLatin1String("\\xC7\\x9A"), QLatin1String("u"));
	n.replace(QLatin1String("\\xC7\\xBD"), QLatin1String("ae"));
	n.replace(QLatin1String("\\xC7\\x9C"), QLatin1String("u"));
	n.replace(QLatin1String("\\xC7\\xBB"), QLatin1String("a"));
	n.replace(QLatin1String("\\xC7\\xBC"), QLatin1String("AE"));
	n.replace(QLatin1String("\\xC7\\xBE"), QLatin1String("O"));
	n.replace(QLatin1String("\\xC7\\xBA"), QLatin1String("A"));

	n.replace(QLatin1String("\\xC2\\x82"), QLatin1String(","));
	n.replace(QLatin1String("\\xC2\\x84"), QLatin1String(",,"));
	n.replace(QLatin1String("\\xC2\\x85"), QLatin1String("..."));
	n.replace(QLatin1String("\\xC2\\x88"), QLatin1String("^"));
	n.replace(QLatin1String("\\xC2\\x91"), QLatin1String("\\x27"));
	n.replace(QLatin1String("\\xC2\\x92"), QLatin1String("\\x27"));
	n.replace(QLatin1String("\\xC2\\x93"), QLatin1String("\\x22"));
	n.replace(QLatin1String("\\xC2\\x94"), QLatin1String("\\x22"));
	n.replace(QLatin1String("\\xC2\\x96"), QLatin1String("-"));
	n.replace(QLatin1String("\\xC2\\x97"), QLatin1String("--"));
	n.replace(QLatin1String("\\xC2\\xA6"), QLatin1String("|"));
	n.replace(QLatin1String("\\xC2\\xAB"), QLatin1String("<<"));
	n.replace(QLatin1String("\\xC2\\xBB"), QLatin1String(">>"));
	n.replace(QLatin1String("\\xC2\\xBC"), QLatin1String("1/4"));
	n.replace(QLatin1String("\\xC2\\xBD"), QLatin1String("1/2"));
	n.replace(QLatin1String("\\xC2\\xBE"), QLatin1String("3/4"));
	n.replace(QLatin1String("\\xCA\\xBF"), QLatin1String("\\x27"));
	n.replace(QLatin1String("\\xCC\\xA8"), QString());
	n.replace(QLatin1String("\\xCC\\xB1"), QString());

	return n;
}

QString CertificateInformation::clearCertificateSpecialSymbols(const QStringList& stringList)
{
	if (stringList.isEmpty())
		return QString();

	return clearCertificateSpecialSymbols(stringList[0]);
}

QString CertificateInformation::certificateItemText(const QSslCertificate& certificate)
{
	QString commonName{certificate.subjectInfo(QSslCertificate::CommonName).isEmpty() ? QString() : certificate
		.subjectInfo(QSslCertificate::CommonName)[0]};
	QString organization{certificate.subjectInfo(QSslCertificate::Organization).isEmpty() ? QString() : certificate
		.subjectInfo(QSslCertificate::Organization)[0]};

	if (commonName.isEmpty())
		return clearCertificateSpecialSymbols(organization);

	return clearCertificateSpecialSymbols(commonName);
}

CertificateInformation::CertificateInformation(const QSslCertificate& certificate, QWidget* parent) :
	QWidget(parent)
{
	setupUi();

	m_issuedToCN->setText(showCertificateInformation(certificate.subjectInfo(QSslCertificate::CommonName)));
	m_issuedToO->setText(showCertificateInformation(certificate.subjectInfo(QSslCertificate::Organization)));
	m_issuedToOU->setText(showCertificateInformation(certificate.subjectInfo(QSslCertificate::OrganizationalUnitName)));
	m_issuedToSN->setText(showCertificateInformation(certificate.serialNumber()));

	m_issuedByCN->setText(showCertificateInformation(certificate.subjectInfo(QSslCertificate::CommonName)));
	m_issuedByO->setText(showCertificateInformation(certificate.subjectInfo(QSslCertificate::Organization)));
	m_issuedByOU->setText(showCertificateInformation(certificate.subjectInfo(QSslCertificate::OrganizationalUnitName)));

	//TODO: Validity, need to manage language

}

CertificateInformation::~CertificateInformation()
{
	// Empty
}

void CertificateInformation::setupUi()
{
	if (objectName().isEmpty())
		setObjectName(QStringLiteral("this"));

	resize(512, 264);

	m_formLayout = new QFormLayout(this);
	m_formLayout->setObjectName(QStringLiteral("m_formLayout"));

	m_lIssuedTo = new QLabel(this);
	m_lIssuedTo->setObjectName(QStringLiteral("m_lIssuedTo"));
	m_lIssuedTo->setText(QObject::tr("<b>Issued To</b>"));

	m_lCN1 = new QLabel(this);
	m_lCN1->setObjectName(QStringLiteral("m_lCN1"));
	m_lCN1->setText(QObject::tr("Common Name (CN):"));

	m_issuedToCN = new QLabel(this);
	m_issuedToCN->setObjectName(QStringLiteral("m_issuedToCN"));
	m_issuedToCN->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::TextSelectableByMouse);

	m_lO1 = new QLabel(this);
	m_lO1->setObjectName(QStringLiteral("m_lO1"));
	m_lO1->setText(QObject::tr("Organization (O):"));

	m_issuedToO = new QLabel(this);
	m_issuedToO->setObjectName(QStringLiteral("m_issuedToO"));
	m_issuedToO->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::TextSelectableByMouse);

	m_lOU1 = new QLabel(this);
	m_lOU1->setObjectName(QStringLiteral("m_lOU1"));
	m_lOU1->setText(QObject::tr("Organizational Unit (OU):"));

	m_issuedToOU = new QLabel(this);
	m_issuedToOU->setObjectName(QStringLiteral("m_issuedToOU"));
	m_issuedToOU->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::TextSelectableByMouse);

	m_lSN = new QLabel(this);
	m_lSN->setObjectName(QStringLiteral("m_lSN"));
	m_lSN->setText(QObject::tr("Serial Number:"));

	m_issuedToSN = new QLabel(this);
	m_issuedToSN->setObjectName(QStringLiteral("m_issuedToSN"));
	m_issuedToSN->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::TextSelectableByMouse);

	m_lIssuedBy = new QLabel(this);
	m_lIssuedBy->setObjectName(QStringLiteral("m_lIssuedBy"));
	m_lIssuedBy->setText(QObject::tr("<b>Issued By</b>"));

	m_lCN2 = new QLabel(this);
	m_lCN2->setObjectName(QStringLiteral("m_lCN2"));
	m_lCN2->setText(QObject::tr("Common Name (CN):"));

	m_issuedByCN = new QLabel(this);
	m_issuedByCN->setObjectName(QStringLiteral("m_issuedByCN"));
	m_issuedByCN->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::TextSelectableByMouse);

	m_lO2 = new QLabel(this);
	m_lO2->setObjectName(QStringLiteral("m_lO2"));
	m_lO2->setText(QObject::tr("Organization (O):"));

	m_issuedByO = new QLabel(this);
	m_issuedByO->setObjectName(QStringLiteral("m_issuedByO"));
	m_issuedByO->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::TextSelectableByMouse);

	m_lOU2 = new QLabel(this);
	m_lOU2->setObjectName(QStringLiteral("m_lOU2"));
	m_lOU2->setText(QObject::tr("Organizational Unit (OU):"));

	m_issuedByOU = new QLabel(this);
	m_issuedByOU->setObjectName(QStringLiteral("m_issuedByOU"));
	m_issuedByOU->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::TextSelectableByMouse);

	m_lValidity = new QLabel(this);
	m_lValidity->setObjectName(QStringLiteral("m_lValidity"));
	m_lValidity->setText(QObject::tr("<b>Validity</b>"));

	m_lIO = new QLabel(this);
	m_lIO->setObjectName(QStringLiteral("m_lIO"));
	m_lIO->setText(QObject::tr("Issued On:"));

	m_validityIssuedOn = new QLabel(this);
	m_validityIssuedOn->setObjectName(QStringLiteral("m_validityIssuedOn"));
	m_validityIssuedOn->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::TextSelectableByMouse);

	m_lEN = new QLabel(this);
	m_lEN->setObjectName(QStringLiteral("m_lON"));
	m_lEN->setText(QObject::tr("Expires On:"));

	m_validityExpiresOn = new QLabel(this);
	m_validityExpiresOn->setObjectName(QStringLiteral("m_validityExpiresOn"));
	m_validityExpiresOn->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::TextSelectableByMouse);

	m_formLayout->setWidget(0, QFormLayout::SpanningRole, m_lIssuedTo);
	m_formLayout->setWidget(1, QFormLayout::LabelRole, m_lCN1);
	m_formLayout->setWidget(1, QFormLayout::FieldRole, m_issuedToCN);
	m_formLayout->setWidget(2, QFormLayout::FieldRole, m_issuedToO);
	m_formLayout->setWidget(2, QFormLayout::LabelRole, m_lO1);
	m_formLayout->setWidget(3, QFormLayout::FieldRole, m_issuedToOU);
	m_formLayout->setWidget(3, QFormLayout::LabelRole, m_lOU1);
	m_formLayout->setWidget(4, QFormLayout::FieldRole, m_issuedToSN);
	m_formLayout->setWidget(4, QFormLayout::LabelRole, m_lSN);
	m_formLayout->setWidget(5, QFormLayout::LabelRole, m_lIssuedBy);
	m_formLayout->setWidget(6, QFormLayout::LabelRole, m_lCN2);
	m_formLayout->setWidget(6, QFormLayout::FieldRole, m_issuedByCN);
	m_formLayout->setWidget(7, QFormLayout::LabelRole, m_lO2);
	m_formLayout->setWidget(7, QFormLayout::FieldRole, m_issuedByO);
	m_formLayout->setWidget(8, QFormLayout::LabelRole, m_lOU2);
	m_formLayout->setWidget(8, QFormLayout::FieldRole, m_issuedByOU);
	m_formLayout->setWidget(9, QFormLayout::LabelRole, m_lValidity);
	m_formLayout->setWidget(10, QFormLayout::LabelRole, m_lIO);
	m_formLayout->setWidget(10, QFormLayout::FieldRole, m_validityIssuedOn);
	m_formLayout->setWidget(11, QFormLayout::LabelRole, m_lEN);
	m_formLayout->setWidget(11, QFormLayout::FieldRole, m_validityExpiresOn);

//	QMetaObject::connectSlotsByName(this);
}
}