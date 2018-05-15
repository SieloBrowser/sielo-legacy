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

#include "AesInterface.hpp"

#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/sha.h>

#include <QDebug>
#include <QMessageBox>

namespace Sn {

const int AesInterface::VERSION = 1;

QByteArray AesInterface::createRandomData(int length)
{
	uchar* randomData{static_cast<uchar*>(malloc(length))};

	RAND_bytes(randomData, length);

	QByteArray data{(char*) randomData, length};

	free(randomData);

	return data;
}

AesInterface::AesInterface(QObject* parent) :
	QObject(parent),
	m_ok(false)
{
	m_encodedCTX = EVP_CIPHER_CTX_new();
	m_decodedCTX = EVP_CIPHER_CTX_new();
	EVP_CIPHER_CTX_init(m_encodedCTX);
	EVP_CIPHER_CTX_init(m_decodedCTX);
}

AesInterface::~AesInterface()
{
	EVP_CIPHER_CTX_cleanup(m_encodedCTX);
	EVP_CIPHER_CTX_cleanup(m_decodedCTX);
	EVP_CIPHER_CTX_free(m_encodedCTX);
	EVP_CIPHER_CTX_free(m_decodedCTX);
}

QByteArray AesInterface::encrypt(const QByteArray& plainData, const QByteArray& password)
{
	if (!init(EVP_PKEY_MO_ENCRYPT, password)) {
		m_ok = false;
		return plainData;
	}

	int dataLength{plainData.size()};
	int cipherLength{dataLength + AES_BLOCK_SIZE};
	int finalLength{0};
	uchar* cipherText{static_cast<uchar*>(malloc(cipherLength))};

	EVP_EncryptInit_ex(m_encodedCTX, nullptr, nullptr, nullptr, nullptr);
	EVP_EncryptUpdate(m_encodedCTX, cipherText, &cipherLength, (uchar*) plainData.data(), dataLength);
	EVP_EncryptFinal_ex(m_encodedCTX, cipherText + cipherLength, &finalLength);

	dataLength = cipherLength + finalLength;

	QByteArray out{(char*) cipherText, dataLength};
	out = QByteArray::number(AesInterface::VERSION) + '$' + m_iVector.toBase64() + '$' + out.toBase64();

	free(cipherText);

	m_ok = true;
	return out;
}

QByteArray AesInterface::decrypt(const QByteArray& cipherData, const QByteArray& password)
{
	m_ok = false;

	if (cipherData.isEmpty()) {
		m_ok = true;
		return QByteArray();
	}

	QList<QByteArray> cipherSections(cipherData.split('$'));

	if (cipherSections.size() != 3) {
		qWarning() << "Decrypt error: It seems datas are corupted";
		return QByteArray();
	}

	if (cipherSections[0].toInt() > AesInterface::VERSION) {
		QMessageBox::warning(nullptr,
							 tr("Warning!"),
							 tr("Datas have been encrypted with a newer version of Sielo. Please install the latest version!"));
		return QByteArray();
	}

	if (cipherSections[0].toInt() != 1) {
		qWarning() << "There is a version error for decoder";
		return QByteArray();
	}

	if (!init(EVP_PKEY_MO_DECRYPT, password, QByteArray::fromBase64(cipherSections[1])))
		return QByteArray();

	QByteArray cipherArray{QByteArray::fromBase64(cipherSections[2])};
	int cipherLength{cipherArray.size()};
	int plainTextLength{cipherLength};
	int finalLength{0};
	uchar* cipherText{(uchar*) cipherArray.data()};
	uchar* plainText{static_cast<uchar*>(malloc(plainTextLength + AES_BLOCK_SIZE))};

	EVP_DecryptInit_ex(m_decodedCTX, nullptr, nullptr, nullptr, nullptr);
	EVP_DecryptUpdate(m_decodedCTX, plainText, &plainTextLength, cipherText, cipherLength);

	int success{EVP_DecryptFinal_ex(m_decodedCTX, plainText + plainTextLength, &finalLength)};

	cipherLength = plainTextLength + finalLength;

	QByteArray result{(char*) plainText, cipherLength};

	free(plainText);

	if (success != 1)
		return QByteArray();

	m_ok = true;
	return result;

}

bool AesInterface::init(int evpMode, const QByteArray& password, const QByteArray& iVector)
{
	m_iVector.clear();

	int i{};
	const int nrounds{5};
	uchar Key[EVP_MAX_KEY_LENGTH];

	i = EVP_BytesToKey(EVP_aes_256_cbc(),
					   EVP_sha256(),
					   nullptr,
					   (uchar*) password.data(),
					   password.size(),
					   nrounds,
					   Key,
					   nullptr);

	if (i != 32) {
		qWarning("Key size is %d bits - should be 256 bits", i * 8);
		return false;
	}

	int result{0};

	if (evpMode == EVP_PKEY_MO_ENCRYPT) {
		m_iVector = createRandomData(EVP_MAX_IV_LENGTH);
		result = EVP_EncryptInit_ex(m_encodedCTX, EVP_aes_256_cbc(), nullptr, Key, (uchar*) m_iVector.constData());
	}
	else if (evpMode == EVP_PKEY_MO_DECRYPT)
		result = EVP_DecryptInit_ex(m_decodedCTX, EVP_aes_256_cbc(), nullptr, Key, (uchar*) iVector.constData());

	if (result == 0) {
		qWarning() << "EVP is not initialized";
		return false;
	}

	return true;

}

}
