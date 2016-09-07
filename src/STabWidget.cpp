#include "STabWidget.h"
#include "SWebView.h"

STabWidget::STabWidget(QWidget * parent) : 
	QTabWidget(parent)
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
