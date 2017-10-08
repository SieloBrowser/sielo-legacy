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

#include "AboutDialog.hpp"

#include <QIcon>

#include "Application.hpp"

namespace Sn {

AboutDialog::AboutDialog(QWidget* parent) :
	QDialog(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);

	setupUI();

	connect(m_closeButtonBox, SIGNAL(clicked(QAbstractButton * )), this, SLOT(close()));

	showAbout();
}

AboutDialog::~AboutDialog()
{
	// Empty
}

void AboutDialog::setupUI()
{
	resize(468, 412);
	setWindowTitle(tr("About Sielo"));

	m_layout = new QVBoxLayout(this);
	m_layout->setSpacing(0);

	m_icon = new QLabel(this);
	m_icon->setPixmap(QIcon(":icons/other/aboutsielo.png").pixmap(382, 92));
	m_icon->setAlignment(Qt::AlignCenter);

	m_content = new QTextBrowser(this);
	m_content->setOpenExternalLinks(true);
	m_content->setOpenLinks(true);

	m_closeButtonBox = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this);

	m_layout->addWidget(m_icon);
	m_layout->addWidget(m_content);
	m_layout->addWidget(m_closeButtonBox);
}

void AboutDialog::showAbout()
{
	if (m_html.isEmpty()) {
		m_html += "<center><div style='margin: 20px;'>";
		m_html += tr("<p><b>Sielo version %1</b><br />").arg(Application::currentVersion);
		m_html += tr("<b>QtWebEngine version %1</b></p>").arg(qVersion());
		m_html += QLatin1String("<p>Copyright &copy; 2017 Victor DENIS<br />");
		m_html += QLatin1String("<a href=\"mailto:admin@feldrise.com\">admin@feldrise.com</a></p>");
		m_html += tr("<p><b>Main developer:</b><br />%1 &lt;%2&gt;</p>")
			.arg("Victor DENIS", "<a href=\"mailto:victordenis01@gmail.com\">victordenis01@gmail.com</a>");
		m_html += tr("<p><b>Contributors:</b><br />%1</p>").arg(
			QString::fromUtf8("hotaru70o<br />"
								  "ilearn32<br />"
								  "gbdivers")
		);
		m_html += "</div></center>";
	}

	m_content->setHtml(m_html);
}

}