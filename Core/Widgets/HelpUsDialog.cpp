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

#include "HelpUsDialog.hpp"

#include <QIcon>

#include "Application.hpp"

namespace Sn {
HelpUsDialog::HelpUsDialog(QWidget* parent) :
		QDialog(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Help Us"));
	setWindowIcon(Application::getAppIcon("ic_sielo"));

	QIcon icon = windowIcon();
	Qt::WindowFlags flags = windowFlags();
	Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;

	flags = flags & (~helpFlag);
	setWindowFlags(flags);
	setWindowIcon(icon);

	m_html = tr(
			"<p>There are many ways to help us. You can <a href=\"https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=MU7RV3SGKTFFS\" target=\"_blank\">make a donation</a>, help us improve the source code, or simply share and talk about Sielo to your entourage.</p>"
	"<p>You can also join our <a href=\"https://discord.gg/7MVvDaS\" target=\"_blank\">Discord server</a></p>");

	setupUI();

	connect(m_text, &QTextBrowser::anchorClicked, this, &HelpUsDialog::openLink);
	connect(m_closeButtonBox, SIGNAL(clicked(QAbstractButton * )), this, SLOT(close()));
}

HelpUsDialog::~HelpUsDialog()
{
	// Empty
}

void HelpUsDialog::openLink(const QUrl& link)
{
	Application::instance()->addNewTab(link);
	m_text->setHtml(m_html);
}

void HelpUsDialog::setupUI()
{
	setWindowTitle(tr("Help Us"));

	m_layout = new QVBoxLayout(this);

	m_text = new QTextBrowser(this);
	m_text->setOpenLinks(true);
	m_text->setOpenExternalLinks(false);
	m_text->document()->setDefaultStyleSheet("a {color: rgb(0, 100, 255); }");
	m_text->setHtml(m_html);

	m_closeButtonBox = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this);

	m_layout->addWidget(m_text);
	m_layout->addWidget(m_closeButtonBox);

}
}