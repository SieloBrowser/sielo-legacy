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

#include "AutoFillWidget.hpp"

#include <QPoint>

#include <QLayout>
#include <QPushButton>

#include <QWebEngine/WebProfile.hpp>

#include "Web/WebView.hpp"
#include "Web/WebPage.hpp"

namespace Sn {

AutoFillWidget::AutoFillWidget(WebView* view, QWidget* parent) :
	QFrame(parent, Qt::Popup),
	m_view(view),
	m_alignement(Qt::AlignRight)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
	setLineWidth(1);
	setMidLineWidth(2);

	setupUI();
}

AutoFillWidget::~AutoFillWidget()
{
	// Empty
}

void AutoFillWidget::setFormData(const QVector<PasswordEntry>& data)
{
	m_data = data;

	for (int i{0}; i < data.count(); ++i) {
		const PasswordEntry entry = data[i];
		if (entry.username.isEmpty())
			continue;

		QPushButton* button{new QPushButton(this)};

		button->setIcon(QIcon(":/icons/other/login.png"));
		button->setStyleSheet("text-align: left; font-weight: bold;");
		button->setFlat(true);
		button->setText(entry.username);
		button->setProperty("data-index", i);

		m_buttonLayout->addWidget(button, i, 0, 1, 1);

		connect(button, &QPushButton::clicked, this, &AutoFillWidget::loginToPage);
	}
}

void AutoFillWidget::showAt(QWidget* parent)
{
	if (!parent || !parent->parentWidget())
		return;

	parent = parent->parentWidget();

	layout()->invalidate();
	layout()->activate();

	QPoint point{parent->mapToGlobal(QPoint(0, 0))};

	if (m_alignement == Qt::AlignRight)
		point.setX(point.x() + parent->width() - width());
	point.setY(point.y() + parent->height());

	move(point);

	QFrame::show();
}

void AutoFillWidget::setPopupAlignement(Qt::Alignment alignement)
{
	m_alignement = alignement;
}

void AutoFillWidget::loginToPage()
{
	QPushButton* button = qobject_cast<QPushButton*>(sender());

	if (!button || !m_view)
		return;

	bool ok{};
	int index{button->property("data_index").toInt(&ok)};

	// container = m_data ; index =  index
	if (ok && (index >= 0 && m_data.count() > index)) {
		QString source = QLatin1String("(function() {"
										   "var data = '%1'.split('&');"
										   "var inputs = document.getElementsByTagName('input');"
										   ""
										   "for (var i = 0; i < data.length; ++i) {"
										   "    var pair = data[i].split('=');"
										   "    if (pair.length != 2)"
										   "        continue;"
										   "    var key = decodeURIComponent(pair[0]);"
										   "    var val = decodeURIComponent(pair[1]);"
										   "    for (var j = 0; j < inputs.length; ++j) {"
										   "        var input = inputs[j];"
										   "        var type = input.type.toLowerCase();"
										   "        if (type != 'text' && type != 'password' && type != 'email')"
										   "            continue;"
										   "        if (input.name == key)"
										   "            input.value = val;"
										   "    }"
										   "}"
										   ""
										   "})()");

		const PasswordEntry entry = m_data[index];
		QString data{entry.data};

		data.replace(QLatin1String("'"), QLatin1String("\\'"));

		m_view->page()->runJavaScript(source.arg(data), Engine::WebProfile::ScriptWorldId::ApplicationWorld);
	}

	close();
}

void AutoFillWidget::setupUI()
{
	m_layout = new QGridLayout(this);
	m_loginLayout = new QGridLayout();
	m_buttonLayout = new QGridLayout();

	m_loginLabel = new QLabel(tr("Login as:"), this);

	m_loginLayout->addWidget(m_loginLabel, 0, 0, 1, 1);

	m_layout->addLayout(m_loginLayout, 0, 0, 1, 1);
	m_layout->addLayout(m_buttonLayout, 1, 0, 1, 1);
}

}