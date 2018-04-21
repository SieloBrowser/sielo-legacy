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

#include "AutoFillJsObject.hpp"


#include "Utils/ExternalJsObject.hpp"

#include "Password/AutoFill/AutoFill.hpp"

#include "Application.hpp"

namespace Sn {
AutoFillJsObject::AutoFillJsObject(ExternalJsObject* parent)
	:
	QObject(parent),
	m_jsObject(parent)
{
	// Empty
}

void AutoFillJsObject::formSubmitted(const QString& frameUrl, const QString& username, const QString& password,
									 const QByteArray& data)
{
	PageFormData formData;
	formData.username = username;
	formData.password = password;
	formData.postData = data;

	Application::instance()->autoFill()->saveForm(m_jsObject->page(), QUrl(frameUrl), formData);
}

}