#include "..\includes\STabWidget.hpp"
#include "..\includes\SWebView.hpp"
#include "..\includes\SMainWindow.hpp"

STabWidget::STabWidget(SMainWindow * parent) : 
	QTabWidget(parent),
	m_parent(parent)
{
}

STabWidget::~STabWidget()
{
	// Empty
}

void STabWidget::createWebTab(QString title, SWebView * view, QUrl url)
{
	QWidget* tabPage{ new QWidget(this) };
	if (view == nullptr)
		view = new SWebView(tabPage, url);
	QVBoxLayout* pageLayout{ new QVBoxLayout };

	pageLayout->addWidget(view);
	tabPage->setLayout(pageLayout);

	addTab(tabPage, title);

}
