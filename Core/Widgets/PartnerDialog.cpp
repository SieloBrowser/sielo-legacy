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

#include "PartnerDialog.hpp"

#include <QPixmap>
#include <QGraphicsDropShadowEffect>

#include "Application.hpp"

namespace Sn {

PartnerDialog::PartnerDialog(QWidget* parent) :
		QDialog(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Partners"));
	setWindowIcon(Application::getAppIcon("ic_sielo"));

	resize(636, 432);
	setupUI();
	setupPartners();
	updateLabels();

	if (m_images.count() == 1) {
		m_previous->setEnabled(false);
		m_next->setEnabled(false);
	}

	connect(m_previous, &QPushButton::clicked, this, &PartnerDialog::previous);
	connect(m_next, &QPushButton::clicked, this, &PartnerDialog::next);
}

PartnerDialog::~PartnerDialog()
{
	// Empty
}

void PartnerDialog::previous()
{
	if (m_currentIndex == 0)
		return;

	--m_currentIndex;
	updateLabels();
	m_next->setEnabled(true);

	if (m_currentIndex == 0)
		m_previous->setEnabled(false);
}

void PartnerDialog::next()
{
	if (m_currentIndex == m_images.count() - 1)
		return;

	++m_currentIndex;
	updateLabels();
	m_previous->setEnabled(true);

	if (m_currentIndex == m_images.count() - 1)
		m_next->setEnabled(false);
}

void PartnerDialog::setupUI()
{
	m_layout = new QVBoxLayout(this);
	m_buttonsLayout = new QHBoxLayout();

	m_image = new QLabel(this);
	m_image->setAlignment(Qt::AlignCenter);
	m_image->setObjectName(QLatin1String("partners-image"));

	m_desc = new QLabel(this);
	m_desc->setWordWrap(true);
	m_desc->setObjectName(QLatin1String("partners-description"));

	m_spacer = new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);
	m_previous = new QPushButton(tr("Previous"), this);
	m_next = new QPushButton(tr("Next"), this);

	m_buttonsLayout->addItem(m_spacer);
	m_buttonsLayout->addWidget(m_previous);
	m_buttonsLayout->addWidget(m_next);

	m_layout->addWidget(m_image);
	m_layout->addWidget(m_desc);
	m_layout->addLayout(m_buttonsLayout);

	QGraphicsDropShadowEffect* effect{new QGraphicsDropShadowEffect(this)};
	effect->setBlurRadius(6);
	effect->setOffset(0, 2);
	effect->setColor(QColor(0, 0, 0));
	m_image->setGraphicsEffect(effect);
}

void PartnerDialog::setupPartners()
{
	m_images << ":data/partners/bs.jpg"
			 << ":data/partners/lmdpc.jpg";

	QString bsDesc = tr("<b>Bit-Studio.com</b><br/><br/>"
						"Created in 2011, this concept aims to promote promising IT "
						"projects in order to help them take off.<br/>"
						"After a few years of operation, the project was put on hold. "
						"It was too much work for one man. In 2018, a team was formed "
						"and Bit-Studio was reborn from its ashes, stronger than ever.<br/>"
						"From then on, discover regularly, in preview, the mastodons of "
						"tomorrow on Bit-Studio.com.");

	QString lmdpcDesc = tr("<b>Le Monde Du PC</b><br/><br/>"
						   "\"Le Monde Du PC\", put online in September 2017, is a site "
						   "that aims to share our knowledge so that, for example, some "
						   "subjects in computing are not only understood by those who "
						   "master them. Today, the team is complete and a 2.0 version will "
						   "normally be released in September.");

	m_descs << bsDesc
			<< lmdpcDesc;
}

void PartnerDialog::updateLabels()
{
	m_image->setPixmap(QPixmap(m_images[m_currentIndex]));
	m_desc->setText(m_descs[m_currentIndex]);
}

}