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
#ifndef SIELOBROWSER_REGEXP_HPP
#define SIELOBROWSER_REGEXP_HPP

#include "SharedDefines.hpp"

#include <QStringList>

#include <QRegularExpression>

namespace Sn {

class SIELO_SHAREDLIB RegExp: public QRegularExpression {
public:
	RegExp();
	RegExp(const QString& pattern, Qt::CaseSensitivity caseSensitivity = Qt::CaseSensitive);
	RegExp(const RegExp& regExp);

	void setMinimal(bool minimal);
	int indexIn(const QString& string, int offset = 0) const;
	int matchedLength() const;
	QString capture(int nth = 0) const;

private:
	QStringList m_capturedTexts;
	int m_matchedLength{-1};
};
}
#endif //SIELOBROWSER_REGEXP_HPP
