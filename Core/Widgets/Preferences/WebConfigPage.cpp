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

#include "WebConfigPage.hpp"

#include "Utils/Settings.hpp"

#include "Web/WebView.hpp"

#include "Application.hpp"

namespace Sn {

WebConfigPage::WebConfigPage(QWidget* parent) :
	QWidget(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);

	setupUI();
	loadSettings();
}

WebConfigPage::~WebConfigPage()
{
	// Empty
}

void WebConfigPage::loadSettings()
{
	Settings settings{};

	settings.beginGroup("Web-Settings");

	m_allowPeeper->setChecked(settings.value("allowPlugins", true).toBool());
	m_allowJavaScript->setChecked(settings.value("allowJavaScript", true).toBool());
	m_includeLinksInFocusChain->setChecked(settings.value("includeLinkInFocusChain", false).toBool());
	m_enableXSS->setChecked(settings.value("XSSAuditing", false).toBool());
	m_animatedScrolling->setChecked(settings.value("animateScrolling", true).toBool());
	m_enableSpacialAnimation->setChecked(settings.value("spatialNavigation", false).toBool());

	m_wheelSpin->setValue(settings.value("wheelScrollLines", Application::instance()->wheelScrollLines()).toInt());

	for (int level : WebView::zoomLevels())
		m_defaultZoom->addItem(QString("%1%").arg(level));
	m_defaultZoom->setCurrentIndex(settings.value("defaultZoomLevel", WebView::zoomLevels().indexOf(100)).toInt());

	settings.endGroup();
}

void WebConfigPage::save()
{
	Settings settings{};

	settings.beginGroup("Web-Settings");

	settings.setValue("allowPlugins", m_allowPeeper->isChecked());
	settings.setValue("allowJavaScript", m_allowJavaScript->isChecked());
	settings.setValue("includeLinkInFocusChain", m_includeLinksInFocusChain->isChecked());
	settings.setValue("XSSAuditing", m_enableXSS->isChecked());
	settings.setValue("animateScrolling", m_animatedScrolling->isChecked());
	settings.setValue("spatialNavigation", m_enableSpacialAnimation->isChecked());

	settings.setValue("wheelScrollLines", m_wheelSpin->value());

	settings.setValue("defaultZoomLevel", m_defaultZoom->currentIndex());

	settings.endGroup();
}

void WebConfigPage::setupUI()
{
	m_layout = new QVBoxLayout(this);
	m_wheelLayout = new QHBoxLayout();
	m_defaultZoomLayout = new QHBoxLayout();

	m_allowPeeper = new QCheckBox(tr("Allow Pepper Plugins (Flash plugin)"), this);
	m_allowJavaScript = new QCheckBox(tr("Allow JavaScript"), this);
	m_includeLinksInFocusChain = new QCheckBox(tr("Include links in focus chain"), this);
	m_enableXSS = new QCheckBox(tr("Enable XSS Auditing"), this);
	m_animatedScrolling = new QCheckBox(tr("Animated scrolling"), this);
	m_enableSpacialAnimation = new QCheckBox(tr("Enable spatial navigation"), this);

	m_line1 = new QFrame(this);
	m_line1->setFrameShape(QFrame::HLine);

	m_descWheel = new QLabel(tr("Mouse wheel scrolls"), this);
	m_wheelSpin = new QSpinBox(this);
	m_lineLabel = new QLabel(tr("lines on page"), this);

	m_line2 = new QFrame(this);
	m_line2->setFrameShape(QFrame::HLine);

	m_descDefaultZoom = new QLabel(tr("Default zoom on pages: "), this);
	m_defaultZoom = new QComboBox(this);
	m_defaultZoomSpacer = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

	m_layoutSpacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

	m_wheelLayout->addWidget(m_descWheel);
	m_wheelLayout->addWidget(m_wheelSpin);
	m_wheelLayout->addWidget(m_lineLabel);

	m_defaultZoomLayout->addWidget(m_descDefaultZoom);
	m_defaultZoomLayout->addWidget(m_defaultZoom);
	m_defaultZoomLayout->addItem(m_defaultZoomSpacer);

	m_layout->addWidget(m_allowPeeper);
	m_layout->addWidget(m_allowJavaScript);
	m_layout->addWidget(m_includeLinksInFocusChain);
	m_layout->addWidget(m_enableXSS);
	m_layout->addWidget(m_animatedScrolling);
	m_layout->addWidget(m_enableSpacialAnimation);
	m_layout->addWidget(m_line1);
	m_layout->addLayout(m_wheelLayout);
	m_layout->addWidget(m_line2);
	m_layout->addLayout(m_defaultZoomLayout);
	m_layout->addItem(m_layoutSpacer);
}

}
