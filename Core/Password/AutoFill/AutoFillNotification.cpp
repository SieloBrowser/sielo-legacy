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

#include "AutoFillNotification.hpp"

#include <QSizePolicy>

#include "Application.hpp"

namespace Sn {

AutoFillNotification::AutoFillNotification(const QUrl& url, const PageFormData& formData,
										   const PasswordEntry& updateData) :
	QWidget(),
	m_url(url),
	m_formData(formData),
	m_updateData(updateData)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setAutoFillBackground(true);

	setupUI();

	connect(m_updateButton, &QPushButton::clicked, this, &AutoFillNotification::update);
	connect(m_rememberButton, &QPushButton::clicked, this, &AutoFillNotification::remember);
	connect(m_neverButton, &QPushButton::clicked, this, &AutoFillNotification::never);
	connect(m_notNowButton, &QPushButton::clicked, this, &AutoFillNotification::hide);
}

AutoFillNotification::~AutoFillNotification()
{
	// Empty
}

void AutoFillNotification::update()
{
	Application::instance()->autoFill()->updateEntry(m_updateData);
	hide();
}

void AutoFillNotification::remember()
{
	Application::instance()->autoFill()->addEntry(m_url, m_formData);
	hide();
}

void AutoFillNotification::never()
{
	Application::instance()->autoFill()->blockStoringForUrl(m_url);
	hide();
}

void AutoFillNotification::setupUI()
{
	m_layout = new QHBoxLayout(this);
	m_layout->setSpacing(3);
	m_buttonLayout = new QHBoxLayout();

	m_icon = new QLabel(this);
	m_icon->setPixmap(QPixmap(":/icons/other/login.png"));
	m_desc = new QLabel(this);

	QSizePolicy descSizePolicy{QSizePolicy::Expanding, QSizePolicy::Preferred};
	descSizePolicy.setHorizontalStretch(0);
	descSizePolicy.setVerticalStretch(0);
	descSizePolicy.setHeightForWidth(m_desc->hasHeightForWidth());

	m_desc->setSizePolicy(descSizePolicy);
	m_updateButton = new QPushButton(tr("Update"), this);
	m_rememberButton = new QPushButton(tr("Remember"), this);
	m_neverButton = new QPushButton(tr("Never For This Site"), this);
	m_notNowButton = new QPushButton(tr("Not Now"), this);

	m_buttonLayout->addWidget(m_updateButton);
	m_buttonLayout->addWidget(m_rememberButton);
	m_buttonLayout->addWidget(m_neverButton);
	m_buttonLayout->addWidget(m_notNowButton);

	m_layout->addWidget(m_icon);
	m_layout->addWidget(m_desc);
	m_layout->addLayout(m_buttonLayout);
}

}