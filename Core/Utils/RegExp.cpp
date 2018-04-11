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

#include "Utils/RegExp.hpp"

namespace Sn {

RegExp::RegExp() :
	QRegularExpression(QString(), QRegularExpression::DotMatchesEverythingOption),
	m_matchedLength(-1)
{
	// Empty
}

RegExp::RegExp(const QString& pattern, Qt::CaseSensitivity caseSensitivity) :
	QRegularExpression(pattern, QRegularExpression::DotMatchesEverythingOption),
	m_matchedLength(-1)
{
	if (caseSensitivity == Qt::CaseInsensitive)
		setPatternOptions(patternOptions() | QRegularExpression::CaseInsensitiveOption);
}

RegExp::RegExp(const RegExp& regExp) :
	QRegularExpression(regExp),
	m_matchedLength(-1)
{
	// Empty
}

void RegExp::setMinimal(bool minimal)
{
	QRegularExpression::PatternOptions options;

	if (minimal)
		options = patternOptions() | QRegularExpression::InvertedGreedinessOption;
	else
		options = patternOptions() & ~QRegularExpression::InvertedGreedinessOption;

	setPatternOptions(options);
}

int RegExp::indexIn(const QString& string, int offset) const
{
	RegExp* that = const_cast<RegExp*>(this);
	QRegularExpressionMatch expressionMatch{match(string, offset)};

	if (!expressionMatch.hasMatch()) {
		that->m_matchedLength = -1;
		that->m_capturedTexts.clear();
		return -1;
	}

	that->m_matchedLength = expressionMatch.capturedLength();
	that->m_capturedTexts = expressionMatch.capturedTexts();

	return expressionMatch.capturedStart();
}

int RegExp::matchedLength() const
{
	return m_matchedLength;
}

QString RegExp::capture(int nth) const
{
	if (!(nth >= 0 && m_capturedTexts.count() > nth))
		return QString();

	return m_capturedTexts[nth];
}

}